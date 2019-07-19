#include "OperandSwappingWatermarker.hpp"

#include <algorithm>

#include <wasm-traversal.h>

#include "../BitStreamWriter.hpp"
#include "../CircularBitStreamReader.hpp"
#include "../Commutativity.hpp"
#include "../Comparator.hpp"

namespace kyut::watermarker {
    namespace {
        enum class SideEffect : std::uint32_t {
            none = 0,
            readOnly = 1,
            write = 2,
        };

        bool swapOperands(wasm::Binary &expr) {
            if (const auto newOp = getSwappedPredicate(expr.op)) {
                expr.op = *newOp;
                std::swap(expr.left, expr.right);

                return true;
            }

            return false;
        }

        // Watermarking visitor
        template <typename F>
        struct OperandSwappingVisitor : wasm::OverriddenVisitor<OperandSwappingVisitor<F>, SideEffect> {
            F f;

            explicit OperandSwappingVisitor(F f)
                : f(std::move(f)) {}

            SideEffect visitExpressionList(const wasm::ExpressionList &exprs) {
                auto effect = SideEffect::none;

                for (const auto expr : exprs) {
                    effect = (std::max)(visit(expr), effect);
                }

                return effect;
            }

            SideEffect visitBlock(wasm::Block *expr) {
                return visitExpressionList(expr->list);
            }

            SideEffect visitIf(wasm::If *expr) {
                return (std::max)({
                    visit(expr->condition),
                    visit(expr->ifTrue),
                    visit(expr->ifFalse),
                });
            }

            SideEffect visitLoop(wasm::Loop *expr) {
                return visit(expr->body);
            }

            SideEffect visitBreak(wasm::Break *expr) {
                visit(expr->value);
                visit(expr->condition);

                return SideEffect::write;
            }

            SideEffect visitSwitch(wasm::Switch *expr) {
                return (std::max)(visit(expr->condition), visit(expr->value));
            }

            SideEffect visitCall(wasm::Call *expr) {
                visitExpressionList(expr->operands);

                // It is difficult to estimate the side effects of the function calls
                return SideEffect::write;
            }

            SideEffect visitCallIndirect(wasm::CallIndirect *expr) {
                visit(expr->target);
                visitExpressionList(expr->operands);

                // It is difficult to estimate the side effects of the function calls
                return SideEffect::write;
            }

            SideEffect visitGetLocal([[maybe_unused]] wasm::GetLocal *expr) {
                return SideEffect::readOnly;
            }

            SideEffect visitSetLocal(wasm::SetLocal *expr) {
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitGetGlobal([[maybe_unused]] wasm::GetGlobal *expr) {
                return SideEffect::readOnly;
            }

            SideEffect visitSetGlobal(wasm::SetGlobal *expr) {
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitLoad(wasm::Load *expr) {
                return (std::max)(visit(expr->ptr), SideEffect::readOnly);
            }

            SideEffect visitStore(wasm::Store *expr) {
                visit(expr->ptr);
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitConst([[maybe_unused]] wasm::Const *expr) {
                return SideEffect::none;
            }

            SideEffect visitUnary(wasm::Unary *expr) {
                return visit(expr->value);
            }

            SideEffect visitBinary(wasm::Binary *expr) {
                if (!isCommutative(expr->op)) {
                    // The operands of noncommutative instructions cannot be swapped
                    return (std::max)(visit(expr->left), visit(expr->right));
                }

                if (!(*expr->left < *expr->right) && !(*expr->right < *expr->left)) {
                    // If both sides are the same or cannot be ordered, skip visitding
                    return (std::max)(visit(expr->left), visit(expr->right));
                }

                // Sort both of the operands
                auto [lo, hi] = std::minmax(expr->left, expr->right, [](auto a, auto b) { return *a < *b; });

                const auto loEffect = visit(lo);
                const auto hiEffect = visit(hi);

                if (static_cast<std::uint32_t>(loEffect) + static_cast<std::uint32_t>(hiEffect) >= 3) {
                    // The operands have side effect and cannot be swapped
                    return (std::max)(loEffect, hiEffect);
                }

                f(*expr, *lo, *hi);

                return (std::max)(loEffect, hiEffect);
            }

            SideEffect visitSelect(wasm::Select *expr) {
                return (std::max)({
                    visit(expr->condition),
                    visit(expr->ifTrue),
                    visit(expr->ifFalse),
                });
            }

            SideEffect visitDrop(wasm::Drop *expr) {
                return visit(expr->value);
            }

            SideEffect visitReturn(wasm::Return *expr) {
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitHost(wasm::Host *expr) {
                visitExpressionList(expr->operands);

                return SideEffect::write;
            }

            SideEffect visitNop([[maybe_unused]] wasm::Nop *exp) {
                return SideEffect::none;
            }

            SideEffect visitUnreachable([[maybe_unused]] wasm::Unreachable *expr) {
                return SideEffect::write;
            }

            SideEffect visitAtomicRMW(wasm::AtomicRMW *expr) {
                visit(expr->ptr);
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitAtomicCmpxchg(wasm::AtomicCmpxchg *expr) {
                visit(expr->ptr);
                visit(expr->expected);
                visit(expr->replacement);

                return SideEffect::write;
            }

            SideEffect visitAtomicWait(wasm::AtomicWait *expr) {
                visit(expr->ptr);
                visit(expr->expected);
                visit(expr->timeout);

                return SideEffect::write;
            }

            SideEffect visitAtomicNotify(wasm::AtomicNotify *expr) {
                visit(expr->ptr);
                visit(expr->notifyCount);

                return SideEffect::write;
            }

            SideEffect visitSIMDExtract(wasm::SIMDExtract *expr) {
                return visit(expr->vec);
            }

            SideEffect visitSIMDReplace(wasm::SIMDReplace *expr) {
                return (std::max)(visit(expr->vec), visit(expr->value));
            }

            SideEffect visitSIMDShuffle(wasm::SIMDShuffle *expr) {
                return (std::max)(visit(expr->left), visit(expr->right));
            }

            SideEffect visitSIMDBitselect(wasm::SIMDBitselect *expr) {
                return (std::max)({
                    visit(expr->cond),
                    visit(expr->left),
                    visit(expr->right),
                });
            }

            SideEffect visitSIMDShift(wasm::SIMDShift *expr) {
                return (std::max)(visit(expr->vec), visit(expr->shift));
            }

            SideEffect visitMemoryInit(wasm::MemoryInit *expr) {
                visit(expr->dest);
                visit(expr->offset);
                visit(expr->size);

                return SideEffect::write;
            }

            SideEffect visitDataDrop([[maybe_unused]] wasm::DataDrop *expr) {
                return SideEffect::write;
            }

            SideEffect visitMemoryCopy(wasm::MemoryCopy *expr) {
                visit(expr->dest);
                visit(expr->source);
                visit(expr->size);

                return SideEffect::write;
            }

            SideEffect visitMemoryFill(wasm::MemoryFill *expr) {
                visit(expr->dest);
                visit(expr->value);
                visit(expr->size);

                return SideEffect::write;
            }

            SideEffect visit(wasm::Expression *expr) {
                if (expr == nullptr) {
                    return SideEffect::none;
                }
                return static_cast<wasm::OverriddenVisitor<OperandSwappingVisitor<F>, SideEffect> *>(this)->visit(expr);
            }

            void visitFunction(wasm::Function *function) {
                visit(function->body);
            }
        };
    } // namespace

    std::size_t embedOperandSwapping(wasm::Module &module, CircularBitStreamReader &stream) {
        // Sort functions in the module by name
        std::vector<wasm::Function *> functions;
        functions.reserve(module.functions.size());

        std::transform(std::begin(module.functions),
                       std::end(module.functions),
                       std::back_inserter(functions),
                       [](const auto &f) { return f.get(); });

        std::sort(
            std::begin(functions), std::end(functions), [](const auto a, const auto b) { return a->name < b->name; });

        // Embed watermarks
        const auto posStart = stream.tell();

        OperandSwappingVisitor visitor{
            [&](wasm::Binary &expr, wasm::Expression &lo, [[maybe_unused]] wasm::Expression &hi) {
                // Embed watermarks by swapping operands
                const bool bit = stream.readBit();

                if (bit == (expr.left == &lo)) {
                    swapOperands(expr);
                }
            }};

        for (const auto f : functions) {
            visitor.visitFunction(f);
        }

        return stream.tell() - posStart;
    }

    std::size_t extractOperandSwapping(wasm::Module &module, BitStreamWriter &stream) {
        // Sort functions in the module by name
        std::vector<wasm::Function *> functions;
        functions.reserve(module.functions.size());

        std::transform(std::begin(module.functions),
                       std::end(module.functions),
                       std::back_inserter(functions),
                       [](const auto &f) { return f.get(); });

        std::sort(
            std::begin(functions), std::end(functions), [](const auto a, const auto b) { return a->name < b->name; });

        // Extract watermarks
        const auto posStart = stream.tell();

        OperandSwappingVisitor visitor{
            [&](wasm::Binary &expr, wasm::Expression &lo, [[maybe_unused]] wasm::Expression &hi) {
                // Extract watermarks from the order of operands
                stream.writeBit(expr.left != &lo);
            }};

        for (const auto f : functions) {
            visitor.visitFunction(f);
        }

        return stream.tell() - posStart;
    }
} // namespace kyut::watermarker

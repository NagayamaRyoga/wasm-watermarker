#include "OperandSwapping.hpp"

#include <boost/range/algorithm_ext/erase.hpp>
#include "../BitStreamWriter.hpp"
#include "../CircularBitStreamReader.hpp"
#include "../wasm-ext/Compare.hpp"
#include "wasm-traversal.h"

namespace kyut::methods::operand_swapping {
    namespace {
        enum class SideEffect : std::uint32_t {
            none = 0,
            read_only = 1,
            write = 2,
        };

        bool swap_operands(wasm::Binary& expr) {
            if (const auto swapped_op = swapped_binary_op(expr.op)) {
                expr.op = *swapped_op;
                std::swap(expr.left, expr.right);

                return true;
            }

            return false;
        }

        template <typename Action>
        struct OperandSwapVisitor : wasm::OverriddenVisitor<OperandSwapVisitor<Action>, SideEffect> {
            Action action;

            explicit OperandSwapVisitor(Action action)
                : action(std::move(action)) {
            }

            SideEffect visitExpressionList(const wasm::ExpressionList& list) {
                auto effect = SideEffect::none;

                for (const auto& expr : list) {
                    effect = (std::max)(visit(expr), effect);
                }

                return effect;
            }

            SideEffect visitBlock(wasm::Block* expr) {
                return visitExpressionList(expr->list);
            }

            SideEffect visitIf(wasm::If* expr) {
                return (std::max)({
                    visit(expr->condition),
                    visit(expr->ifTrue),
                    visit(expr->ifFalse),
                });
            }

            SideEffect visitLoop(wasm::Loop* expr) {
                return visit(expr->body);
            }

            SideEffect visitBreak(wasm::Break* expr) {
                visit(expr->value);
                visit(expr->condition);

                return SideEffect::write;
            }

            SideEffect visitSwitch(wasm::Switch* expr) {
                visit(expr->condition);
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitCall(wasm::Call* expr) {
                visitExpressionList(expr->operands);

                return SideEffect::write;
            }

            SideEffect visitCallIndirect(wasm::CallIndirect* expr) {
                visit(expr->target);
                visitExpressionList(expr->operands);

                return SideEffect::write;
            }

            SideEffect visitLocalGet([[maybe_unused]] wasm::LocalGet* expr) {
                return SideEffect::read_only;
            }

            SideEffect visitLocalSet(wasm::LocalSet* expr) {
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitGlobalGet([[maybe_unused]] wasm::GlobalGet* expr) {
                return SideEffect::read_only;
            }

            SideEffect visitGlobalSet(wasm::GlobalSet* expr) {
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitLoad(wasm::Load* expr) {
                return (std::max)(visit(expr->ptr), SideEffect::read_only);
            }

            SideEffect visitStore(wasm::Store* expr) {
                visit(expr->ptr);
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitConst([[maybe_unused]] wasm::Const* expr) {
                return SideEffect::none;
            }

            SideEffect visitUnary(wasm::Unary* expr) {
                return visit(expr->value);
            }

            SideEffect visitBinary(wasm::Binary* expr) {
                if (!is_commutative(expr->op)) {
                    return (std::max)(visit(expr->left), visit(expr->right));
                }

                if (!(*expr->left < *expr->right) && !(*expr->right < *expr->left)) {
                    return (std::max)(visit(expr->left), visit(expr->right));
                }

                // Sort the operands
                auto [lo, hi] = std::minmax(expr->left, expr->right, [](auto a, auto b) { return *a < *b; });

                const auto effect_lo = visit(lo);
                const auto effect_hi = visit(hi);

                if (static_cast<std::uint32_t>(effect_lo) + static_cast<std::uint32_t>(effect_hi) < 3) {
                    // The operands can be swapped
                    action(*expr, *lo, *hi);
                }

                return (std::max)(effect_lo, effect_hi);
            }

            SideEffect visitSelect(wasm::Select* expr) {
                return (std::max)({
                    visit(expr->ifTrue),
                    visit(expr->ifFalse),
                    visit(expr->condition),
                });
            }

            SideEffect visitDrop(wasm::Drop* expr) {
                return visit(expr->value);
            }

            SideEffect visitReturn(wasm::Return* expr) {
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitMemorySize([[maybe_unused]] wasm::MemorySize* expr) {
                return SideEffect::read_only;
            }

            SideEffect visitMemoryGrow(wasm::MemoryGrow* expr) {
                visit(expr->delta);

                return SideEffect::write;
            }

            SideEffect visitNop([[maybe_unused]] wasm::Nop* expr) {
                return SideEffect::none;
            }

            SideEffect visitUnreachable([[maybe_unused]] wasm::Unreachable* expr) {
                return SideEffect::none;
            }

            SideEffect visitAtomicRMW(wasm::AtomicRMW* expr) {
                visit(expr->ptr);
                visit(expr->value);

                return SideEffect::write;
            }

            SideEffect visitAtomicCmpxchg(wasm::AtomicCmpxchg* expr) {
                visit(expr->ptr);
                visit(expr->expected);
                visit(expr->replacement);

                return SideEffect::write;
            }

            SideEffect visitAtomicWait(wasm::AtomicWait* expr) {
                visit(expr->ptr);
                visit(expr->expected);
                visit(expr->timeout);

                return SideEffect::write;
            }

            SideEffect visitAtomicNotify(wasm::AtomicNotify* expr) {
                visit(expr->ptr);
                visit(expr->notifyCount);

                return SideEffect::write;
            }

            SideEffect visitAtomicFence([[maybe_unused]] wasm::AtomicFence* expr) {
                return SideEffect::write;
            }

            SideEffect visitSIMDExtract(wasm::SIMDExtract* expr) {
                return visit(expr->vec);
            }

            SideEffect visitSIMDReplace(wasm::SIMDReplace* expr) {
                return (std::max)(visit(expr->vec), visit(expr->value));
            }

            SideEffect visitSIMDShuffle(wasm::SIMDShuffle* expr) {
                return (std::max)(visit(expr->left), visit(expr->right));
            }

            SideEffect visitSIMDTernary(wasm::SIMDTernary* expr) {
                return (std::max)({
                    visit(expr->a),
                    visit(expr->b),
                    visit(expr->c),
                });
            }

            SideEffect visitSIMDShift(wasm::SIMDShift* expr) {
                return (std::max)(visit(expr->vec), visit(expr->shift));
            }

            SideEffect visitSIMDLoad(wasm::SIMDLoad* expr) {
                return (std::max)({
                    visit(expr->ptr),
                    SideEffect::read_only,
                });
            }

            SideEffect visitMemoryInit(wasm::MemoryInit* expr) {
                visit(expr->dest);
                visit(expr->offset);
                visit(expr->size);

                return SideEffect::write;
            }

            SideEffect visitDataDrop([[maybe_unused]] wasm::DataDrop* expr) {
                return SideEffect::write;
            }

            SideEffect visitMemoryCopy(wasm::MemoryCopy* expr) {
                visit(expr->dest);
                visit(expr->source);
                visit(expr->size);

                return SideEffect::write;
            }

            SideEffect visitMemoryFill(wasm::MemoryFill* expr) {
                visit(expr->dest);
                visit(expr->value);
                visit(expr->size);

                return SideEffect::write;
            }

            SideEffect visitPop([[maybe_unused]] wasm::Pop* expr) {
                return SideEffect::write;
            }

            SideEffect visitRefNull([[maybe_unused]] wasm::RefNull* expr) {
                return SideEffect::read_only;
            }

            SideEffect visitRefIsNull(wasm::RefIsNull* expr) {
                return (std::max)(visit(expr->value), SideEffect::read_only);
            }

            SideEffect visitRefFunc([[maybe_unused]] wasm::RefFunc* expr) {
                return SideEffect::read_only;
            }

            SideEffect visitTry(wasm::Try* expr) {
                visit(expr->body);
                visit(expr->catchBody);

                return SideEffect::write;
            }

            SideEffect visitThrow(wasm::Throw* expr) {
                visitExpressionList(expr->operands);

                return SideEffect::write;
            }

            SideEffect visitRethrow([[maybe_unused]] wasm::Rethrow* expr) {
                return SideEffect::write;
            }

            SideEffect visitBrOnExn([[maybe_unused]] wasm::BrOnExn* expr) {
                return SideEffect::write;
            }

            SideEffect visitTupleMake(wasm::TupleMake* expr) {
                return visitExpressionList(expr->operands);
            }

            SideEffect visitTupleExtract(wasm::TupleExtract* expr) {
                return visit(expr->tuple);
            }

            SideEffect visit(wasm::Expression* expr) {
                if (expr == nullptr) {
                    return SideEffect::none;
                }
                return wasm::OverriddenVisitor<OperandSwapVisitor<Action>, SideEffect>::visit(expr);
            }

            void visitFunction(wasm::Function* func) {
                visit(func->body);
            }
        };
    } // namespace

    std::size_t embed(CircularBitStreamReader& r, wasm::Module& module) {
        std::vector<wasm::Function*> functions{};
        functions.reserve(module.functions.size());

        std::transform(
            std::begin(module.functions),
            std::end(module.functions),
            std::back_inserter(functions),
            [](const auto& f) { return f.get(); });

        // Remove functions without bodies
        boost::range::remove_erase_if(
            functions,
            [](const wasm::Function* f) { return f->body == nullptr; });

        // Sort functions in the module by thier body expression
        std::sort(
            std::begin(functions),
            std::end(functions),
            [](const wasm::Function* a, const wasm::Function* b) { return *a->body < *b->body; });

        // Embed the watermark
        std::size_t size_bits = 0;

        OperandSwapVisitor visitor{
            [&](wasm::Binary& expr, wasm::Expression& lo, [[maybe_unused]] wasm::Expression& hi) {
                // Embed watermark bit into the binary expression
                const bool bit = r.read_bit();

                if (bit == (expr.left == &lo)) {
                    swap_operands(expr);
                }

                size_bits += 1;
            }};

        for (const auto& f : functions) {
            visitor.visitFunction(f);
        }

        return size_bits;
    }

    std::size_t extract(BitStreamWriter& w, wasm::Module& module) {
        std::vector<wasm::Function*> functions{};
        functions.reserve(module.functions.size());

        std::transform(
            std::begin(module.functions),
            std::end(module.functions),
            std::back_inserter(functions),
            [](const auto& f) { return f.get(); });

        // Remove functions without bodies
        boost::range::remove_erase_if(
            functions,
            [](const wasm::Function* f) { return f->body == nullptr; });

        // Sort functions in the module by thier body expression
        std::sort(
            std::begin(functions),
            std::end(functions),
            [](const wasm::Function* a, const wasm::Function* b) { return *a->body < *b->body; });

        // Extract the watermark
        std::size_t size_bits = 0;

        OperandSwapVisitor visitor{
            [&](wasm::Binary& expr, wasm::Expression& lo, [[maybe_unused]] wasm::Expression& hi) {
                // Extract watermark bit from the binary expression
                const bool bit = expr.left != &lo;

                w.write_bit(bit);

                size_bits += 1;
            }};

        for (const auto& f : functions) {
            visitor.visitFunction(f);
        }

        return size_bits;
    }
} // namespace kyut::methods::operand_swapping

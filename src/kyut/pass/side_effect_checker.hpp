#ifndef INCLUDE_kyut_pass_side_effect_checker_hpp
#define INCLUDE_kyut_pass_side_effect_checker_hpp

#include <wasm-traversal.h>

#include "../side_effect.hpp"

namespace kyut::pass {
    class SideEffectCheckingVisitor : public wasm::OverriddenVisitor<SideEffectCheckingVisitor, SideEffect> {
    public:
        explicit SideEffectCheckingVisitor() = default;

        SideEffectCheckingVisitor(const SideEffectCheckingVisitor &) = delete;
        SideEffectCheckingVisitor(SideEffectCheckingVisitor &&) = delete;

        SideEffectCheckingVisitor &operator=(const SideEffectCheckingVisitor &) = delete;
        SideEffectCheckingVisitor &operator=(SideEffectCheckingVisitor &&) = delete;

        ~SideEffectCheckingVisitor() noexcept = default;

        SideEffect visitBlock(wasm::Block *curr) {
            auto side_effect = SideEffect::none;

            for (const auto &expr : curr->list) {
                side_effect = (std::max)(visit(expr), side_effect);
            }

            return side_effect;
        }

        SideEffect visitIf(wasm::If *curr) {
            auto side_effect = SideEffect::none;

            side_effect = (std::max)(visit(curr->condition), side_effect);
            side_effect = (std::max)(visit(curr->ifTrue), side_effect);

            if (curr->ifFalse) {
                side_effect = (std::max)(visit(curr->ifFalse), side_effect);
            }

            return side_effect;
        }

        SideEffect visitLoop(wasm::Loop *curr) {
            return visit(curr->body);
        }

        SideEffect visitBreak(wasm::Break *curr) {
            if (curr->condition) {
                visit(curr->condition);
            }

            if (curr->value) {
                visit(curr->value);
            }

            return SideEffect::write;
        }

        SideEffect visitSwitch(wasm::Switch *curr) {
            visit(curr->condition);

            if (curr->value) {
                visit(curr->value);
            }

            return SideEffect::write;
        }

        SideEffect visitCall(wasm::Call *curr) {
            for (const auto &expr : curr->operands) {
                visit(expr);
            }

            return SideEffect::write;
        }

        SideEffect visitCallIndirect(wasm::CallIndirect *curr) {
            visit(curr->target);

            for (const auto &expr : curr->operands) {
                visit(expr);
            }

            return SideEffect::write;
        }

        SideEffect visitGetLocal([[maybe_unused]] wasm::GetLocal *curr) {
            return SideEffect::read;
        }

        SideEffect visitSetLocal(wasm::SetLocal *curr) {
            visit(curr->value);
            return SideEffect::write;
        }

        SideEffect visitGetGlobal([[maybe_unused]] wasm::GetGlobal *curr) {
            return SideEffect::read;
        }

        SideEffect visitSetGlobal(wasm::SetGlobal *curr) {
            visit(curr->value);
            return SideEffect::write;
        }

        SideEffect visitLoad(wasm::Load *curr) {
            return (std::max)(visit(curr->ptr), SideEffect::read);
        }

        SideEffect visitStore(wasm::Store *curr) {
            visit(curr->ptr);
            visit(curr->value);
            return SideEffect::write;
        }

        SideEffect visitAtomicRMW(wasm::AtomicRMW *curr) {
            visit(curr->ptr);
            visit(curr->value);
            return SideEffect::write;
        }

        SideEffect visitAtomicCmpxchg(wasm::AtomicCmpxchg *curr) {
            visit(curr->ptr);
            visit(curr->expected);
            visit(curr->replacement);
            return SideEffect::write;
        }

        SideEffect visitAtomicWait(wasm::AtomicWait *curr) {
            visit(curr->ptr);
            visit(curr->expected);
            visit(curr->timeout);
            return SideEffect::write;
        }

        SideEffect visitAtomicNotify(wasm::AtomicNotify *curr) {
            visit(curr->ptr);
            visit(curr->notifyCount);
            return SideEffect::write;
        }

        SideEffect visitSIMDExtract(wasm::SIMDExtract *curr) {
            return visit(curr->vec);
        }

        SideEffect visitSIMDReplace(wasm::SIMDReplace *curr) {
            auto side_effect = SideEffect::none;

            side_effect = (std::max)(visit(curr->vec), side_effect);
            side_effect = (std::max)(visit(curr->value), side_effect);

            return side_effect;
        }

        SideEffect visitSIMDShuffle(wasm::SIMDShuffle *curr) {
            auto side_effect = SideEffect::none;

            side_effect = (std::max)(visit(curr->left), side_effect);
            side_effect = (std::max)(visit(curr->right), side_effect);

            return side_effect;
        }

        SideEffect visitSIMDBitselect(wasm::SIMDBitselect *curr) {
            auto side_effect = SideEffect::none;

            side_effect = (std::max)(visit(curr->cond), side_effect);
            side_effect = (std::max)(visit(curr->left), side_effect);
            side_effect = (std::max)(visit(curr->right), side_effect);

            return side_effect;
        }

        SideEffect visitSIMDShift(wasm::SIMDShift *curr) {
            auto side_effect = SideEffect::none;

            side_effect = (std::max)(visit(curr->vec), side_effect);
            side_effect = (std::max)(visit(curr->shift), side_effect);

            return side_effect;
        }

        SideEffect visitMemoryInit(wasm::MemoryInit *curr) {
            visit(curr->dest);
            visit(curr->offset);
            visit(curr->size);
            return SideEffect::write;
        }

        SideEffect visitDataDrop([[maybe_unused]] wasm::DataDrop *curr) {
            return SideEffect::write;
        }

        SideEffect visitMemoryCopy(wasm::MemoryCopy *curr) {
            visit(curr->dest);
            visit(curr->source);
            visit(curr->size);
            return SideEffect::write;
        }

        SideEffect visitMemoryFill(wasm::MemoryFill *curr) {
            visit(curr->dest);
            visit(curr->value);
            visit(curr->size);
            return SideEffect::write;
        }

        SideEffect visitConst([[maybe_unused]] wasm::Const *curr) {
            return SideEffect::none;
        }

        SideEffect visitUnary(wasm::Unary *curr) {
            return visit(curr->value);
        }

        SideEffect visitBinary(wasm::Binary *curr) {
            auto side_effect = SideEffect::none;

            side_effect = (std::max)(visit(curr->left), side_effect);
            side_effect = (std::max)(visit(curr->right), side_effect);

            return side_effect;
        }

        SideEffect visitSelect(wasm::Select *curr) {
            auto side_effect = SideEffect::none;

            side_effect = (std::max)(visit(curr->condition), side_effect);
            side_effect = (std::max)(visit(curr->ifTrue), side_effect);
            side_effect = (std::max)(visit(curr->ifFalse), side_effect);

            return side_effect;
        }

        SideEffect visitDrop(wasm::Drop *curr) {
            return visit(curr->value);
        }

        SideEffect visitReturn(wasm::Return *curr) {
            if (curr->value) {
                visit(curr->value);
            }

            return SideEffect::write;
        }

        SideEffect visitHost(wasm::Host *curr) {
            for (const auto &expr : curr->operands) {
                visit(expr);
            }

            if (curr->op == wasm::HostOp::CurrentMemory) {
                return SideEffect::read;
            } else {
                return SideEffect::write;
            }
        }

        SideEffect visitNop([[maybe_unused]] wasm::Nop *curr) {
            return SideEffect::none;
        }

        SideEffect visitUnreachable([[maybe_unused]] wasm::Unreachable *curr) {
            WASM_UNREACHABLE();
        }
        SideEffect visitFunctionType([[maybe_unused]] wasm::FunctionType *curr) {
            WASM_UNREACHABLE();
        }
        SideEffect visitExport([[maybe_unused]] wasm::Export *curr) {
            WASM_UNREACHABLE();
        }
        SideEffect visitGlobal([[maybe_unused]] wasm::Global *curr) {
            WASM_UNREACHABLE();
        }
        SideEffect visitFunction([[maybe_unused]] wasm::Function *curr) {
            WASM_UNREACHABLE();
        }
        SideEffect visitTable([[maybe_unused]] wasm::Table *curr) {
            WASM_UNREACHABLE();
        }
        SideEffect visitMemory([[maybe_unused]] wasm::Memory *curr) {
            WASM_UNREACHABLE();
        }
        SideEffect visitModule([[maybe_unused]] wasm::Module *curr) {
            WASM_UNREACHABLE();
        }
    };
} // namespace kyut::pass

#endif // INCLUDE_kyut_pass_side_effect_checker_hpp

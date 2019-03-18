#include "operand_swapping_watermarker.hpp"

#include <pass.h>

namespace kyut::pass {
    class OperandSwappingWatermarkingVisitor
        : public wasm::OverriddenVisitor<OperandSwappingWatermarkingVisitor, bool> {
    public:
        explicit OperandSwappingWatermarkingVisitor() = default;

        OperandSwappingWatermarkingVisitor(const OperandSwappingWatermarkingVisitor &) = delete;
        OperandSwappingWatermarkingVisitor(OperandSwappingWatermarkingVisitor &&) = delete;

        OperandSwappingWatermarkingVisitor &operator=(const OperandSwappingWatermarkingVisitor &) = delete;
        OperandSwappingWatermarkingVisitor &operator=(OperandSwappingWatermarkingVisitor &&) = delete;

        ~OperandSwappingWatermarkingVisitor() noexcept = default;

        bool visitBlock(wasm::Block *curr) {
            bool has_side_effect = false;

            for (const auto &expr : curr->list) {
                has_side_effect = visit(expr) || has_side_effect;
            }

            return has_side_effect;
        }

        bool visitIf(wasm::If *curr) {
            bool has_side_effect = false;

            has_side_effect = visit(curr->condition) || has_side_effect;
            has_side_effect = visit(curr->ifTrue) || has_side_effect;

            if (curr->ifFalse) {
                has_side_effect = visit(curr->ifFalse) || has_side_effect;
            }

            return has_side_effect;
        }

        bool visitLoop(wasm::Loop *curr) {
            return visit(curr->body);
        }

        bool visitBreak(wasm::Break *curr) {
            if (curr->condition) {
                visit(curr->condition);
            }

            if (curr->value) {
                visit(curr->value);
            }

            return true;
        }

        bool visitSwitch(wasm::Switch *curr) {
            visit(curr->condition);

            if (curr->value) {
                visit(curr->value);
            }

            return true;
        }

        bool visitCall(wasm::Call *curr) {
            for (const auto &expr : curr->operands) {
                visit(expr);
            }

            return true;
        }

        bool visitCallIndirect(wasm::CallIndirect *curr) {
            visit(curr->target);

            for (const auto &expr : curr->operands) {
                visit(expr);
            }

            return true;
        }

        bool visitGetLocal([[maybe_unused]] wasm::GetLocal *curr) {
            return false;
        }

        bool visitSetLocal(wasm::SetLocal *curr) {
            visit(curr->value);
            return true;
        }

        bool visitGetGlobal([[maybe_unused]] wasm::GetGlobal *curr) {
            return false;
        }

        bool visitSetGlobal(wasm::SetGlobal *curr) {
            visit(curr->value);
            return true;
        }

        bool visitLoad(wasm::Load *curr) {
            return visit(curr->ptr);
        }

        bool visitStore(wasm::Store *curr) {
            visit(curr->ptr);
            visit(curr->value);
            return true;
        }

        bool visitAtomicRMW(wasm::AtomicRMW *curr) {
            visit(curr->ptr);
            visit(curr->value);
            return true;
        }

        bool visitAtomicCmpxchg(wasm::AtomicCmpxchg *curr) {
            visit(curr->ptr);
            visit(curr->expected);
            visit(curr->replacement);
            return true;
        }

        bool visitAtomicWait(wasm::AtomicWait *curr) {
            visit(curr->ptr);
            visit(curr->expected);
            visit(curr->timeout);
            return true;
        }

        bool visitAtomicWake(wasm::AtomicWake *curr) {
            visit(curr->ptr);
            visit(curr->wakeCount);
            return true;
        }

        bool visitSIMDExtract(wasm::SIMDExtract *curr) {
            return visit(curr->vec);
        }

        bool visitSIMDReplace(wasm::SIMDReplace *curr) {
            bool has_side_effect = false;

            has_side_effect = visit(curr->vec) || has_side_effect;
            has_side_effect = visit(curr->value) || has_side_effect;

            return has_side_effect;
        }

        bool visitSIMDShuffle(wasm::SIMDShuffle *curr) {
            bool has_side_effect = false;

            has_side_effect = visit(curr->left) || has_side_effect;
            has_side_effect = visit(curr->right) || has_side_effect;

            return has_side_effect;
        }

        bool visitSIMDBitselect(wasm::SIMDBitselect *curr) {
            bool has_side_effect = false;

            has_side_effect = visit(curr->cond) || has_side_effect;
            has_side_effect = visit(curr->left) || has_side_effect;
            has_side_effect = visit(curr->right) || has_side_effect;

            return has_side_effect;
        }

        bool visitSIMDShift(wasm::SIMDShift *curr) {
            bool has_side_effect = false;

            has_side_effect = visit(curr->vec) || has_side_effect;
            has_side_effect = visit(curr->shift) || has_side_effect;

            return has_side_effect;
        }

        bool visitMemoryInit(wasm::MemoryInit *curr) {
            visit(curr->dest);
            visit(curr->offset);
            visit(curr->size);
            return true;
        }

        bool visitDataDrop([[maybe_unused]] wasm::DataDrop *curr) {
            return true;
        }

        bool visitMemoryCopy(wasm::MemoryCopy *curr) {
            visit(curr->dest);
            visit(curr->source);
            visit(curr->size);
            return true;
        }

        bool visitMemoryFill(wasm::MemoryFill *curr) {
            visit(curr->dest);
            visit(curr->value);
            visit(curr->size);
            return true;
        }

        bool visitConst([[maybe_unused]] wasm::Const *curr) {
            return false;
        }

        bool visitUnary(wasm::Unary *curr) {
            return visit(curr->value);
        }

        bool visitBinary(wasm::Binary *curr) {
            bool has_side_effect = false;

            has_side_effect = visit(curr->left) || has_side_effect;
            has_side_effect = visit(curr->right) || has_side_effect;

            return has_side_effect;
        }

        bool visitSelect(wasm::Select *curr) {
            bool has_side_effect = false;

            has_side_effect = visit(curr->condition) || has_side_effect;
            has_side_effect = visit(curr->ifTrue) || has_side_effect;
            has_side_effect = visit(curr->ifFalse) || has_side_effect;

            return has_side_effect;
        }

        bool visitDrop(wasm::Drop *curr) {
            return visit(curr->value);
        }

        bool visitReturn(wasm::Return *curr) {
            if (curr->value) {
                visit(curr->value);
            }

            return true;
        }

        bool visitHost(wasm::Host *curr) {
            for (const auto &expr : curr->operands) {
                visit(expr);
            }

            return curr->op != wasm::HostOp::CurrentMemory;
        }

        bool visitNop([[maybe_unused]] wasm::Nop *curr) {
            return false;
        }

        bool visitUnreachable([[maybe_unused]] wasm::Unreachable *curr) {
            WASM_UNREACHABLE();
        }
        bool visitFunctionType([[maybe_unused]] wasm::FunctionType *curr) {
            WASM_UNREACHABLE();
        }
        bool visitExport([[maybe_unused]] wasm::Export *curr) {
            WASM_UNREACHABLE();
        }
        bool visitGlobal([[maybe_unused]] wasm::Global *curr) {
            WASM_UNREACHABLE();
        }
        bool visitFunction([[maybe_unused]] wasm::Function *curr) {
            WASM_UNREACHABLE();
        }
        bool visitTable([[maybe_unused]] wasm::Table *curr) {
            WASM_UNREACHABLE();
        }
        bool visitMemory([[maybe_unused]] wasm::Memory *curr) {
            WASM_UNREACHABLE();
        }
        bool visitModule([[maybe_unused]] wasm::Module *curr) {
            WASM_UNREACHABLE();
        }
    };

    class OperandSwappingWatermarkingPass : public wasm::Pass {
    public:
        explicit OperandSwappingWatermarkingPass() = default;

        OperandSwappingWatermarkingPass(const OperandSwappingWatermarkingPass &) = delete;
        OperandSwappingWatermarkingPass(OperandSwappingWatermarkingPass &&) = delete;

        OperandSwappingWatermarkingPass &operator=(const OperandSwappingWatermarkingPass &) = delete;
        OperandSwappingWatermarkingPass &operator=(OperandSwappingWatermarkingPass &&) = delete;

        ~OperandSwappingWatermarkingPass() noexcept = default;

        bool modifiesBinaryenIR() noexcept override {
            return true;
        }

        void run([[maybe_unused]] wasm::PassRunner *runner, wasm::Module *module) override {
            OperandSwappingWatermarkingVisitor visitor{};

            for (const auto &func : module->functions) {
                visitor.visit(func->body);
            }
        }
    };

    void embedWatermarkOperandSwapping(wasm::Module &module) {
        wasm::PassRunner runner{&module};
        runner.add<OperandSwappingWatermarkingPass>();
        runner.run();
    }
} // namespace kyut::pass

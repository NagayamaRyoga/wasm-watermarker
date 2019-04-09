#include "operand_swapping_watermarker.hpp"

#include <pass.h>

#include "../commutativity.hpp"
#include "../comparison.hpp"

#include "side_effect_checker.hpp"

namespace kyut::pass {
    class OperandSwappingWatermarkingVisitor
        : public wasm::UnifiedExpressionVisitor<OperandSwappingWatermarkingVisitor, SideEffect> {
    public:
        explicit OperandSwappingWatermarkingVisitor(CircularBitStream &stream)
            : stream_(stream) {}

        OperandSwappingWatermarkingVisitor(const OperandSwappingWatermarkingVisitor &) = delete;
        OperandSwappingWatermarkingVisitor(OperandSwappingWatermarkingVisitor &&) = delete;

        OperandSwappingWatermarkingVisitor &operator=(const OperandSwappingWatermarkingVisitor &) = delete;
        OperandSwappingWatermarkingVisitor &operator=(OperandSwappingWatermarkingVisitor &&) = delete;

        ~OperandSwappingWatermarkingVisitor() noexcept = default;

        SideEffect visitExpression(wasm::Expression *curr) {
            return SideEffectCheckingVisitor{}.visit(curr);
        }

        SideEffect visitBinary(wasm::Binary *curr) {
            auto side_effect_left = visit(curr->left);
            auto side_effect_right = visit(curr->right);

            // operands can be swapped if [write(=2), none(=0)] or [read(=1), read(=1)]
            auto can_swap_operands =
                (static_cast<std::int32_t>(side_effect_left) + static_cast<std::int32_t>(side_effect_left) <= 2);

            if (isCommutative(curr->op) && can_swap_operands) {
                const auto bit = stream_.read_bit();

                if (bit == (*curr->left < *curr->right)) {
                    swapOperands(*curr);
                }
            }

            return (std::max)(side_effect_left, side_effect_right);
        }

    private:
        CircularBitStream &stream_;
    };

    class OperandSwappingWatermarkingPass : public wasm::Pass {
    public:
        explicit OperandSwappingWatermarkingPass(CircularBitStream &stream)
            : stream_(stream) {}

        OperandSwappingWatermarkingPass(const OperandSwappingWatermarkingPass &) = delete;
        OperandSwappingWatermarkingPass(OperandSwappingWatermarkingPass &&) = delete;

        OperandSwappingWatermarkingPass &operator=(const OperandSwappingWatermarkingPass &) = delete;
        OperandSwappingWatermarkingPass &operator=(OperandSwappingWatermarkingPass &&) = delete;

        ~OperandSwappingWatermarkingPass() noexcept = default;

        bool modifiesBinaryenIR() noexcept override {
            return true;
        }

        void run([[maybe_unused]] wasm::PassRunner *runner, wasm::Module *module) override {
            OperandSwappingWatermarkingVisitor visitor{stream_};

            for (const auto &func : module->functions) {
                visitor.visit(func->body);
            }
        }

    private:
        CircularBitStream &stream_;
    };

    void embedWatermarkOperandSwapping(wasm::Module &module, CircularBitStream &stream) {
        wasm::PassRunner runner{&module};
        runner.add<OperandSwappingWatermarkingPass>(std::ref(stream));
        runner.run();
    }
} // namespace kyut::pass

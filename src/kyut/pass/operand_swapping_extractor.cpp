#include "operand_swapping_extractor.hpp"

#include <pass.h>

#include "../commutativity.hpp"
#include "../comparison.hpp"

#include "side_effect_checker.hpp"

namespace kyut::pass {
    class OperandSwappingExtractingVisitor
        : public wasm::UnifiedExpressionVisitor<OperandSwappingExtractingVisitor, SideEffect> {
    public:
        explicit OperandSwappingExtractingVisitor(BitWriter &writer)
            : writer_(writer) {}

        OperandSwappingExtractingVisitor(const OperandSwappingExtractingVisitor &) = delete;
        OperandSwappingExtractingVisitor(OperandSwappingExtractingVisitor &&) = delete;

        OperandSwappingExtractingVisitor &operator=(const OperandSwappingExtractingVisitor &) = delete;
        OperandSwappingExtractingVisitor &operator=(OperandSwappingExtractingVisitor &&) = delete;

        ~OperandSwappingExtractingVisitor() noexcept = default;

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
                const auto bit = !(*curr->left < *curr->right);

                writer_.write_bit(bit);
            }

            return (std::max)(side_effect_left, side_effect_right);
        }

    private:
        BitWriter &writer_;
    };

    class OperandSwappingExtractingPass : public wasm::Pass {
    public:
        explicit OperandSwappingExtractingPass(BitWriter &writer)
            : writer_(writer) {}

        OperandSwappingExtractingPass(const OperandSwappingExtractingPass &) = delete;
        OperandSwappingExtractingPass(OperandSwappingExtractingPass &&) = delete;

        OperandSwappingExtractingPass &operator=(const OperandSwappingExtractingPass &) = delete;
        OperandSwappingExtractingPass &operator=(OperandSwappingExtractingPass &&) = delete;

        ~OperandSwappingExtractingPass() noexcept = default;

        bool modifiesBinaryenIR() noexcept override {
            return false;
        }

        void run([[maybe_unused]] wasm::PassRunner *runner, wasm::Module *module) override {
            OperandSwappingExtractingVisitor visitor{writer_};

            for (const auto &func : module->functions) {
                if (func->body) {
                    visitor.visit(func->body);
                }
            }
        }

    private:
        BitWriter &writer_;
    };

    void extractWatermarkOperandSwapping(wasm::Module &module, BitWriter &writer) {
        wasm::PassRunner runner{&module};
        runner.add<OperandSwappingExtractingPass>(std::ref(writer));
        runner.run();
    }
} // namespace kyut::pass

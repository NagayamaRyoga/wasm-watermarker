#include "OperandSwappingWatermarker.hpp"

#include <algorithm>

#include "../BitStreamWriter.hpp"
#include "../CircularBitStreamReader.hpp"

namespace kyut::watermarker {
    namespace {
// Expression types
#define EXPR_TYPES()                                                                                                   \
    EXPR_TYPE(Block)                                                                                                   \
    EXPR_TYPE(If)                                                                                                      \
    EXPR_TYPE(Loop)                                                                                                    \
    EXPR_TYPE(Break)                                                                                                   \
    EXPR_TYPE(Switch)                                                                                                  \
    EXPR_TYPE(Call)                                                                                                    \
    EXPR_TYPE(CallIndirect)                                                                                            \
    EXPR_TYPE(GetLocal)                                                                                                \
    EXPR_TYPE(SetLocal)                                                                                                \
    EXPR_TYPE(GetGlobal)                                                                                               \
    EXPR_TYPE(SetGlobal)                                                                                               \
    EXPR_TYPE(Load)                                                                                                    \
    EXPR_TYPE(Store)                                                                                                   \
    EXPR_TYPE(Const)                                                                                                   \
    EXPR_TYPE(Unary)                                                                                                   \
    EXPR_TYPE(Binary)                                                                                                  \
    EXPR_TYPE(Select)                                                                                                  \
    EXPR_TYPE(Drop)                                                                                                    \
    EXPR_TYPE(Return)                                                                                                  \
    EXPR_TYPE(Host)                                                                                                    \
    EXPR_TYPE(Nop)                                                                                                     \
    EXPR_TYPE(Unreachable)                                                                                             \
    EXPR_TYPE(AtomicRMW)                                                                                               \
    EXPR_TYPE(AtomicCmpxchg)                                                                                           \
    EXPR_TYPE(AtomicWait)                                                                                              \
    EXPR_TYPE(AtomicNotify)                                                                                            \
    EXPR_TYPE(SIMDExtract)                                                                                             \
    EXPR_TYPE(SIMDReplace)                                                                                             \
    EXPR_TYPE(SIMDShuffle)                                                                                             \
    EXPR_TYPE(SIMDBitselect)                                                                                           \
    EXPR_TYPE(SIMDShift)                                                                                               \
    EXPR_TYPE(MemoryInit)                                                                                              \
    EXPR_TYPE(DataDrop)                                                                                                \
    EXPR_TYPE(MemoryCopy)                                                                                              \
    EXPR_TYPE(MemoryFill)

        // Watermark embedder
        std::size_t embedExpression(wasm::Expression *expr, CircularBitStreamReader &stream);

        std::size_t embedExpressionList(const wasm::ExpressionList &exprs, CircularBitStreamReader &stream) {
            std::size_t numBits = 0;

            for (const auto expr : exprs) {
                numBits += embedExpression(expr, stream);
            }

            return numBits;
        }

        std::size_t embedBlock(wasm::Block &expr, CircularBitStreamReader &stream) {
            return embedExpressionList(expr.list, stream);
        }

        std::size_t embedIf(wasm::If &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.condition, stream) + embedExpression(expr.ifTrue, stream) +
                   embedExpression(expr.ifFalse, stream);
        }

        std::size_t embedLoop(wasm::Loop &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.body, stream);
        }

        std::size_t embedBreak(wasm::Break &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.value, stream) + embedExpression(expr.condition, stream);
        }

        std::size_t embedSwitch(wasm::Switch &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.condition, stream) + embedExpression(expr.value, stream);
        }

        std::size_t embedCall(wasm::Call &expr, CircularBitStreamReader &stream) {
            return embedExpressionList(expr.operands, stream);
        }

        std::size_t embedCallIndirect(wasm::CallIndirect &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.target, stream) + embedExpressionList(expr.operands, stream);
        }

        std::size_t embedGetLocal([[maybe_unused]] wasm::GetLocal &expr,
                                  [[maybe_unused]] CircularBitStreamReader &stream) {
            return 0;
        }

        std::size_t embedSetLocal(wasm::SetLocal &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.value, stream);
        }

        std::size_t embedGetGlobal([[maybe_unused]] wasm::GetGlobal &expr,
                                   [[maybe_unused]] CircularBitStreamReader &stream) {
            return 0;
        }

        std::size_t embedSetGlobal(wasm::SetGlobal &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.value, stream);
        }

        std::size_t embedLoad(wasm::Load &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.ptr, stream);
        }

        std::size_t embedStore(wasm::Store &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.ptr, stream) + embedExpression(expr.value, stream);
        }

        std::size_t embedConst([[maybe_unused]] wasm::Const &expr, [[maybe_unused]] CircularBitStreamReader &stream) {
            return 0;
        }

        std::size_t embedUnary(wasm::Unary &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.value, stream);
        }

        std::size_t embedBinary(wasm::Binary &expr, CircularBitStreamReader &stream) {
            // TODO: implement watermarking
            return embedExpression(expr.left, stream) + embedExpression(expr.right, stream);
        }

        std::size_t embedSelect(wasm::Select &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.condition, stream) + embedExpression(expr.ifTrue, stream) +
                   embedExpression(expr.ifFalse, stream);
        }

        std::size_t embedDrop(wasm::Drop &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.value, stream);
        }

        std::size_t embedReturn(wasm::Return &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.value, stream);
        }

        std::size_t embedHost(wasm::Host &expr, CircularBitStreamReader &stream) {
            return embedExpressionList(expr.operands, stream);
        }

        std::size_t embedNop([[maybe_unused]] wasm::Nop &expr, [[maybe_unused]] CircularBitStreamReader &stream) {
            return 0;
        }

        std::size_t embedUnreachable([[maybe_unused]] wasm::Unreachable &expr,
                                     [[maybe_unused]] CircularBitStreamReader &stream) {
            return 0;
        }

        std::size_t embedAtomicRMW(wasm::AtomicRMW &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.ptr, stream) + embedExpression(expr.value, stream);
        }

        std::size_t embedAtomicCmpxchg(wasm::AtomicCmpxchg &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.ptr, stream) + embedExpression(expr.expected, stream) +
                   embedExpression(expr.replacement, stream);
        }

        std::size_t embedAtomicWait(wasm::AtomicWait &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.ptr, stream) + embedExpression(expr.expected, stream) +
                   embedExpression(expr.timeout, stream);
        }

        std::size_t embedAtomicNotify(wasm::AtomicNotify &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.ptr, stream) + embedExpression(expr.notifyCount, stream);
        }

        std::size_t embedSIMDExtract(wasm::SIMDExtract &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.vec, stream);
        }

        std::size_t embedSIMDReplace(wasm::SIMDReplace &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.vec, stream) + embedExpression(expr.value, stream);
        }

        std::size_t embedSIMDShuffle(wasm::SIMDShuffle &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.left, stream) + embedExpression(expr.right, stream);
        }

        std::size_t embedSIMDBitselect(wasm::SIMDBitselect &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.cond, stream) + embedExpression(expr.left, stream) +
                   embedExpression(expr.right, stream);
        }

        std::size_t embedSIMDShift(wasm::SIMDShift &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.vec, stream) + embedExpression(expr.shift, stream);
        }

        std::size_t embedMemoryInit(wasm::MemoryInit &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.dest, stream) + embedExpression(expr.offset, stream) +
                   embedExpression(expr.size, stream);
        }

        std::size_t embedDataDrop([[maybe_unused]] wasm::DataDrop &expr,
                                  [[maybe_unused]] CircularBitStreamReader &stream) {
            return 0;
        }

        std::size_t embedMemoryCopy(wasm::MemoryCopy &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.dest, stream) + embedExpression(expr.source, stream) +
                   embedExpression(expr.size, stream);
        }

        std::size_t embedMemoryFill(wasm::MemoryFill &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.dest, stream) + embedExpression(expr.value, stream) +
                   embedExpression(expr.size, stream);
        }

        std::size_t embedExpression(wasm::Expression *expr, CircularBitStreamReader &stream) {
            if (expr == nullptr) {
                return 0;
            }

            switch (expr->_id) {
#define EXPR_TYPE(name)                                                                                                \
    case ::wasm::Expression::name##Id:                                                                                 \
        return embed##name(*expr->cast<::wasm::name>(), stream);
                EXPR_TYPES()
#undef EXPR_TYPE

            default:
                WASM_UNREACHABLE();
            }
        }

        std::size_t embedFunction(wasm::Function &function, CircularBitStreamReader &stream) {
            return embedExpression(function.body, stream);
        }
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

        // Number of bits embedded in the module
        std::size_t numBits = 0;

        // Embed watermarks
        for (const auto f : functions) {
            numBits += embedFunction(*f, stream);
        }

        return numBits;
    }

    std::size_t extractOperandSwapping(wasm::Module &module, BitStreamWriter &stream) {
        (void)module;
        (void)stream;
        return 0;
    }
} // namespace kyut::watermarker

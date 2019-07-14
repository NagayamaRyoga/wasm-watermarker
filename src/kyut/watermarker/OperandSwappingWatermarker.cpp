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

        enum class SideEffect : std::uint32_t {
            none = 0,
            readOnly = 1,
            write = 2,
        };

        // Watermark embedder
        SideEffect embedExpression(wasm::Expression *expr, CircularBitStreamReader &stream);

        SideEffect embedExpressionList(const wasm::ExpressionList &exprs, CircularBitStreamReader &stream) {
            auto effect = SideEffect::none;

            for (const auto expr : exprs) {
                effect = (std::max)(embedExpression(expr, stream), effect);
            }

            return effect;
        }

        SideEffect embedBlock(wasm::Block &expr, CircularBitStreamReader &stream) {
            return embedExpressionList(expr.list, stream);
        }

        SideEffect embedIf(wasm::If &expr, CircularBitStreamReader &stream) {
            return (std::max)({
                embedExpression(expr.condition, stream),
                embedExpression(expr.ifTrue, stream),
                embedExpression(expr.ifFalse, stream),
            });
        }

        SideEffect embedLoop(wasm::Loop &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.body, stream);
        }

        SideEffect embedBreak(wasm::Break &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.value, stream);
            embedExpression(expr.condition, stream);

            return SideEffect::write;
        }

        SideEffect embedSwitch(wasm::Switch &expr, CircularBitStreamReader &stream) {
            return (std::max)(embedExpression(expr.condition, stream), embedExpression(expr.value, stream));
        }

        SideEffect embedCall(wasm::Call &expr, CircularBitStreamReader &stream) {
            embedExpressionList(expr.operands, stream);

            // It is difficult to estimate the side effects of the function calls
            return SideEffect::write;
        }

        SideEffect embedCallIndirect(wasm::CallIndirect &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.target, stream);
            embedExpressionList(expr.operands, stream);

            // It is difficult to estimate the side effects of the function calls
            return SideEffect::write;
        }

        SideEffect embedGetLocal([[maybe_unused]] wasm::GetLocal &expr,
                                 [[maybe_unused]] CircularBitStreamReader &stream) {
            return SideEffect::readOnly;
        }

        SideEffect embedSetLocal(wasm::SetLocal &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.value, stream);

            return SideEffect::write;
        }

        SideEffect embedGetGlobal([[maybe_unused]] wasm::GetGlobal &expr,
                                  [[maybe_unused]] CircularBitStreamReader &stream) {
            return SideEffect::readOnly;
        }

        SideEffect embedSetGlobal(wasm::SetGlobal &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.value, stream);

            return SideEffect::write;
        }

        SideEffect embedLoad(wasm::Load &expr, CircularBitStreamReader &stream) {
            return (std::max)(embedExpression(expr.ptr, stream), SideEffect::readOnly);
        }

        SideEffect embedStore(wasm::Store &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.ptr, stream);
            embedExpression(expr.value, stream);

            return SideEffect::write;
        }

        SideEffect embedConst([[maybe_unused]] wasm::Const &expr, [[maybe_unused]] CircularBitStreamReader &stream) {
            return SideEffect::none;
        }

        SideEffect embedUnary(wasm::Unary &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.value, stream);
        }

        SideEffect embedBinary(wasm::Binary &expr, CircularBitStreamReader &stream) {
            // TODO: implement watermarking
            return (std::max)(embedExpression(expr.left, stream), embedExpression(expr.right, stream));
        }

        SideEffect embedSelect(wasm::Select &expr, CircularBitStreamReader &stream) {
            return (std::max)({
                embedExpression(expr.condition, stream),
                embedExpression(expr.ifTrue, stream),
                embedExpression(expr.ifFalse, stream),
            });
        }

        SideEffect embedDrop(wasm::Drop &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.value, stream);
        }

        SideEffect embedReturn(wasm::Return &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.value, stream);

            return SideEffect::write;
        }

        SideEffect embedHost(wasm::Host &expr, CircularBitStreamReader &stream) {
            embedExpressionList(expr.operands, stream);

            return SideEffect::write;
        }

        SideEffect embedNop([[maybe_unused]] wasm::Nop &expr, [[maybe_unused]] CircularBitStreamReader &stream) {
            return SideEffect::none;
        }

        SideEffect embedUnreachable([[maybe_unused]] wasm::Unreachable &expr,
                                    [[maybe_unused]] CircularBitStreamReader &stream) {
            return SideEffect::write;
        }

        SideEffect embedAtomicRMW(wasm::AtomicRMW &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.ptr, stream);
            embedExpression(expr.value, stream);

            return SideEffect::write;
        }

        SideEffect embedAtomicCmpxchg(wasm::AtomicCmpxchg &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.ptr, stream);
            embedExpression(expr.expected, stream);
            embedExpression(expr.replacement, stream);

            return SideEffect::write;
        }

        SideEffect embedAtomicWait(wasm::AtomicWait &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.ptr, stream);
            embedExpression(expr.expected, stream);
            embedExpression(expr.timeout, stream);

            return SideEffect::write;
        }

        SideEffect embedAtomicNotify(wasm::AtomicNotify &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.ptr, stream);
            embedExpression(expr.notifyCount, stream);

            return SideEffect::write;
        }

        SideEffect embedSIMDExtract(wasm::SIMDExtract &expr, CircularBitStreamReader &stream) {
            return embedExpression(expr.vec, stream);
        }

        SideEffect embedSIMDReplace(wasm::SIMDReplace &expr, CircularBitStreamReader &stream) {
            return (std::max)(embedExpression(expr.vec, stream), embedExpression(expr.value, stream));
        }

        SideEffect embedSIMDShuffle(wasm::SIMDShuffle &expr, CircularBitStreamReader &stream) {
            return (std::max)(embedExpression(expr.left, stream), embedExpression(expr.right, stream));
        }

        SideEffect embedSIMDBitselect(wasm::SIMDBitselect &expr, CircularBitStreamReader &stream) {
            return (std::max)({
                embedExpression(expr.cond, stream),
                embedExpression(expr.left, stream),
                embedExpression(expr.right, stream),
            });
        }

        SideEffect embedSIMDShift(wasm::SIMDShift &expr, CircularBitStreamReader &stream) {
            return (std::max)(embedExpression(expr.vec, stream), embedExpression(expr.shift, stream));
        }

        SideEffect embedMemoryInit(wasm::MemoryInit &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.dest, stream);
            embedExpression(expr.offset, stream);
            embedExpression(expr.size, stream);

            return SideEffect::write;
        }

        SideEffect embedDataDrop([[maybe_unused]] wasm::DataDrop &expr,
                                 [[maybe_unused]] CircularBitStreamReader &stream) {
            return SideEffect::write;
        }

        SideEffect embedMemoryCopy(wasm::MemoryCopy &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.dest, stream);
            embedExpression(expr.source, stream);
            embedExpression(expr.size, stream);

            return SideEffect::write;
        }

        SideEffect embedMemoryFill(wasm::MemoryFill &expr, CircularBitStreamReader &stream) {
            embedExpression(expr.dest, stream);
            embedExpression(expr.value, stream);
            embedExpression(expr.size, stream);

            return SideEffect::write;
        }

        SideEffect embedExpression(wasm::Expression *expr, CircularBitStreamReader &stream) {
            if (expr == nullptr) {
                return SideEffect::none;
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

        void embedFunction(wasm::Function &function, CircularBitStreamReader &stream) {
            embedExpression(function.body, stream);
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

        // Embed watermarks
        std::size_t posStart = stream.tell();

        for (const auto f : functions) {
            embedFunction(*f, stream);
        }

        return stream.tell() - posStart;
    }

    std::size_t extractOperandSwapping(wasm::Module &module, BitStreamWriter &stream) {
        (void)module;
        (void)stream;

        WASM_UNREACHABLE();
    }
} // namespace kyut::watermarker

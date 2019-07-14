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

        std::optional<wasm::BinaryOp> getSwappedPredicate(wasm::BinaryOp op) {
            switch (op) {
                // Commutative instructions
            case wasm::AddInt32:
            case wasm::MulInt32:
            case wasm::AndInt32:
            case wasm::OrInt32:
            case wasm::XorInt32:
            case wasm::EqInt32:
            case wasm::NeInt32:

            case wasm::AddInt64:
            case wasm::MulInt64:
            case wasm::AndInt64:
            case wasm::OrInt64:
            case wasm::XorInt64:
            case wasm::EqInt64:
            case wasm::NeInt64:

            case wasm::AddFloat32:
            case wasm::MulFloat32:
            case wasm::MinFloat32:
            case wasm::MaxFloat32:
            case wasm::EqFloat32:
            case wasm::NeFloat32:

            case wasm::AddFloat64:
            case wasm::MulFloat64:
            case wasm::MinFloat64:
            case wasm::MaxFloat64:
            case wasm::EqFloat64:
            case wasm::NeFloat64:
                return op;

                // Comparators
            case wasm::LtSInt32:
                return wasm::GtSInt32;
            case wasm::LtUInt32:
                return wasm::GtUInt32;
            case wasm::LeSInt32:
                return wasm::GeSInt32;
            case wasm::LeUInt32:
                return wasm::GeUInt32;
            case wasm::GtSInt32:
                return wasm::LtSInt32;
            case wasm::GtUInt32:
                return wasm::LtUInt32;
            case wasm::GeSInt32:
                return wasm::LeSInt32;
            case wasm::GeUInt32:
                return wasm::LeUInt32;

            case wasm::LtSInt64:
                return wasm::GtSInt64;
            case wasm::LtUInt64:
                return wasm::GtUInt64;
            case wasm::LeSInt64:
                return wasm::GeSInt64;
            case wasm::LeUInt64:
                return wasm::GeUInt64;
            case wasm::GtSInt64:
                return wasm::LtSInt64;
            case wasm::GtUInt64:
                return wasm::LtUInt64;
            case wasm::GeSInt64:
                return wasm::LeSInt64;
            case wasm::GeUInt64:
                return wasm::LeUInt64;

            case wasm::LtFloat32:
                return wasm::GtFloat32;
            case wasm::LeFloat32:
                return wasm::GeFloat32;
            case wasm::GtFloat32:
                return wasm::LtFloat32;
            case wasm::GeFloat32:
                return wasm::LeFloat32;

            case wasm::LtFloat64:
                return wasm::GtFloat64;
            case wasm::LeFloat64:
                return wasm::GeFloat64;
            case wasm::GtFloat64:
                return wasm::LtFloat64;
            case wasm::GeFloat64:
                return wasm::LeFloat64;

                // Noncommutative instructions
            case wasm::SubInt32:
            case wasm::DivSInt32:
            case wasm::DivUInt32:
            case wasm::RemSInt32:
            case wasm::RemUInt32:
            case wasm::ShlInt32:
            case wasm::ShrUInt32:
            case wasm::ShrSInt32:
            case wasm::RotLInt32:
            case wasm::RotRInt32:

            case wasm::SubInt64:
            case wasm::DivSInt64:
            case wasm::DivUInt64:
            case wasm::RemSInt64:
            case wasm::RemUInt64:
            case wasm::ShlInt64:
            case wasm::ShrUInt64:
            case wasm::ShrSInt64:
            case wasm::RotLInt64:
            case wasm::RotRInt64:

            case wasm::SubFloat32:
            case wasm::DivFloat32:
            case wasm::CopySignFloat32:

            case wasm::SubFloat64:
            case wasm::DivFloat64:
            case wasm::CopySignFloat64:
                return std::nullopt;

            // Commutative SIMD instructions
            case wasm::EqVecI8x16:
            case wasm::NeVecI8x16:
            case wasm::EqVecI16x8:
            case wasm::NeVecI16x8:
            case wasm::EqVecI32x4:
            case wasm::NeVecI32x4:
            case wasm::EqVecF32x4:
            case wasm::NeVecF32x4:
            case wasm::EqVecF64x2:
            case wasm::NeVecF64x2:
            case wasm::AndVec128:
            case wasm::OrVec128:
            case wasm::XorVec128:
            case wasm::AddVecI8x16:
            case wasm::AddSatSVecI8x16:
            case wasm::AddSatUVecI8x16:
            case wasm::MulVecI8x16:
            case wasm::AddVecI16x8:
            case wasm::AddSatSVecI16x8:
            case wasm::AddSatUVecI16x8:
            case wasm::MulVecI16x8:
            case wasm::AddVecI32x4:
            case wasm::MulVecI32x4:
            case wasm::AddVecI64x2:
            case wasm::AddVecF32x4:
            case wasm::MulVecF32x4:
            case wasm::MinVecF32x4:
            case wasm::MaxVecF32x4:
            case wasm::AddVecF64x2:
            case wasm::MulVecF64x2:
            case wasm::MinVecF64x2:
            case wasm::MaxVecF64x2:
                return op;

            // SIMD comparators
            case wasm::LtSVecI8x16:
                return wasm::GtSVecI8x16;
            case wasm::LtUVecI8x16:
                return wasm::GtUVecI8x16;
            case wasm::GtSVecI8x16:
                return wasm::LtSVecI8x16;
            case wasm::GtUVecI8x16:
                return wasm::LtUVecI8x16;
            case wasm::LeSVecI8x16:
                return wasm::GeSVecI8x16;
            case wasm::LeUVecI8x16:
                return wasm::GeUVecI8x16;
            case wasm::GeSVecI8x16:
                return wasm::LeSVecI8x16;
            case wasm::GeUVecI8x16:
                return wasm::LeUVecI8x16;

            case wasm::LtSVecI16x8:
                return wasm::GtSVecI16x8;
            case wasm::LtUVecI16x8:
                return wasm::GtUVecI16x8;
            case wasm::GtSVecI16x8:
                return wasm::LtSVecI16x8;
            case wasm::GtUVecI16x8:
                return wasm::LtUVecI16x8;
            case wasm::LeSVecI16x8:
                return wasm::GeSVecI16x8;
            case wasm::LeUVecI16x8:
                return wasm::GeUVecI16x8;
            case wasm::GeSVecI16x8:
                return wasm::LeSVecI16x8;
            case wasm::GeUVecI16x8:
                return wasm::LeUVecI16x8;

            case wasm::LtSVecI32x4:
                return wasm::GtSVecI32x4;
            case wasm::LtUVecI32x4:
                return wasm::GtUVecI32x4;
            case wasm::GtSVecI32x4:
                return wasm::LtSVecI32x4;
            case wasm::GtUVecI32x4:
                return wasm::LtUVecI32x4;
            case wasm::LeSVecI32x4:
                return wasm::GeSVecI32x4;
            case wasm::LeUVecI32x4:
                return wasm::GeUVecI32x4;
            case wasm::GeSVecI32x4:
                return wasm::LeSVecI32x4;
            case wasm::GeUVecI32x4:
                return wasm::LeUVecI32x4;

            case wasm::LtVecF32x4:
                return wasm::GtVecF32x4;
            case wasm::GtVecF32x4:
                return wasm::LtVecF32x4;
            case wasm::LeVecF32x4:
                return wasm::GeVecF32x4;
            case wasm::GeVecF32x4:
                return wasm::LeVecF32x4;

            case wasm::LtVecF64x2:
                return wasm::GtVecF64x2;
            case wasm::GtVecF64x2:
                return wasm::LtVecF64x2;
            case wasm::LeVecF64x2:
                return wasm::GeVecF64x2;
            case wasm::GeVecF64x2:
                return wasm::LeVecF64x2;

                // Noncommutative SIMD instructions
            case wasm::SubVecI8x16:
            case wasm::SubSatSVecI8x16:
            case wasm::SubSatUVecI8x16:
            case wasm::SubVecI16x8:
            case wasm::SubSatSVecI16x8:
            case wasm::SubSatUVecI16x8:
            case wasm::SubVecI32x4:
            case wasm::SubVecI64x2:
            case wasm::SubVecF32x4:
            case wasm::DivVecF32x4:
            case wasm::SubVecF64x2:
            case wasm::DivVecF64x2:
                return std::nullopt;

            default:
                WASM_UNREACHABLE();
            }
        }

        bool isCommutative(wasm::BinaryOp op) {
            return getSwappedPredicate(op).has_value();
        }

        bool swapOperands(wasm::Binary &expr) {
            if (const auto newOp = getSwappedPredicate(expr.op)) {
                expr.op = *newOp;
                std::swap(expr.left, expr.right);

                return true;
            }

            return false;
        }

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
            if (!isCommutative(expr.op)) {
                // The operands of noncommutative instructions cannot be swapped
                return (std::max)(embedExpression(expr.left, stream), embedExpression(expr.right, stream));
            }

            // TODO: implement watermarking
            const auto leftSideEffect = embedExpression(expr.left, stream);
            const auto rightSideEffect = embedExpression(expr.right, stream);

            (void)swapOperands;

            return (std::max)(leftSideEffect, rightSideEffect);
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

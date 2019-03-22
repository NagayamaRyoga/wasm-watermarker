#ifndef INCLUDE_kyut_commutativity_hpp
#define INCLUDE_kyut_commutativity_hpp

#include <wasm.h>

namespace kyut {
    constexpr bool isCommutative(wasm::BinaryOp op) noexcept {
        switch (op) {
        case wasm::BinaryOp::AddInt32:
        case wasm::BinaryOp::MulInt32:
        case wasm::BinaryOp::AndInt32:
        case wasm::BinaryOp::OrInt32:
        case wasm::BinaryOp::XorInt32:
        case wasm::BinaryOp::EqInt32:
        case wasm::BinaryOp::NeInt32:

        case wasm::BinaryOp::AddInt64:
        case wasm::BinaryOp::MulInt64:
        case wasm::BinaryOp::AndInt64:
        case wasm::BinaryOp::OrInt64:
        case wasm::BinaryOp::XorInt64:
        case wasm::BinaryOp::EqInt64:
        case wasm::BinaryOp::NeInt64:

        case wasm::BinaryOp::AddFloat32:
        case wasm::BinaryOp::MulFloat32:
        case wasm::BinaryOp::MinFloat32:
        case wasm::BinaryOp::MaxFloat32:
        case wasm::BinaryOp::EqFloat32:
        case wasm::BinaryOp::NeFloat32:

        case wasm::BinaryOp::AddFloat64:
        case wasm::BinaryOp::MulFloat64:
        case wasm::BinaryOp::MinFloat64:
        case wasm::BinaryOp::MaxFloat64:
        case wasm::BinaryOp::EqFloat64:
        case wasm::BinaryOp::NeFloat64:
            return true;

            // Relational operators
        case wasm::BinaryOp::LtSInt32:
        case wasm::BinaryOp::LtUInt32:
        case wasm::BinaryOp::LeSInt32:
        case wasm::BinaryOp::LeUInt32:
        case wasm::BinaryOp::GtSInt32:
        case wasm::BinaryOp::GtUInt32:
        case wasm::BinaryOp::GeSInt32:
        case wasm::BinaryOp::GeUInt32:

        case wasm::BinaryOp::LtSInt64:
        case wasm::BinaryOp::LtUInt64:
        case wasm::BinaryOp::LeSInt64:
        case wasm::BinaryOp::LeUInt64:
        case wasm::BinaryOp::GtSInt64:
        case wasm::BinaryOp::GtUInt64:
        case wasm::BinaryOp::GeSInt64:
        case wasm::BinaryOp::GeUInt64:

        case wasm::BinaryOp::LtFloat32:
        case wasm::BinaryOp::LeFloat32:
        case wasm::BinaryOp::GtFloat32:
        case wasm::BinaryOp::GeFloat32:
            return true;

            // TODO: SIMD operators
        default:
            return false;
        }
    }

    inline bool swapOperands(wasm::Binary &expr) noexcept {
        if (!isCommutative(expr.op)) {
            return false;
        }

        // Invert relational operator
        expr.op = [](wasm::BinaryOp op) noexcept {
            switch (op) {
            case wasm::BinaryOp::LtSInt32:
                return wasm::BinaryOp::GtSInt32;
            case wasm::BinaryOp::LtUInt32:
                return wasm::BinaryOp::GtUInt32;
            case wasm::BinaryOp::LeSInt32:
                return wasm::BinaryOp::GeSInt32;
            case wasm::BinaryOp::LeUInt32:
                return wasm::BinaryOp::GeUInt32;
            case wasm::BinaryOp::GtSInt32:
                return wasm::BinaryOp::LtSInt32;
            case wasm::BinaryOp::GtUInt32:
                return wasm::BinaryOp::LtUInt32;
            case wasm::BinaryOp::GeSInt32:
                return wasm::BinaryOp::LeSInt32;
            case wasm::BinaryOp::GeUInt32:
                return wasm::BinaryOp::LeUInt32;
            case wasm::BinaryOp::LtSInt64:
                return wasm::BinaryOp::GtSInt64;
            case wasm::BinaryOp::LtUInt64:
                return wasm::BinaryOp::GtUInt64;
            case wasm::BinaryOp::LeSInt64:
                return wasm::BinaryOp::GeSInt64;
            case wasm::BinaryOp::LeUInt64:
                return wasm::BinaryOp::GeUInt64;
            case wasm::BinaryOp::GtSInt64:
                return wasm::BinaryOp::LtSInt64;
            case wasm::BinaryOp::GtUInt64:
                return wasm::BinaryOp::LtUInt64;
            case wasm::BinaryOp::GeSInt64:
                return wasm::BinaryOp::LeSInt64;
            case wasm::BinaryOp::GeUInt64:
                return wasm::BinaryOp::LeUInt64;
            case wasm::BinaryOp::LtFloat32:
                return wasm::BinaryOp::GtFloat32;
            case wasm::BinaryOp::LeFloat32:
                return wasm::BinaryOp::GeFloat32;
            case wasm::BinaryOp::GtFloat32:
                return wasm::BinaryOp::LtFloat32;
            case wasm::BinaryOp::GeFloat32:
                return wasm::BinaryOp::LeFloat32;

                // TODO: SIMD operations
            default:
                return op;
            }
        }
        (expr.op);

        // Swap operands
        std::swap(expr.left, expr.right);

        return true;
    }
} // namespace kyut

#endif // INCLUDE_kyut_commutativity_hpp

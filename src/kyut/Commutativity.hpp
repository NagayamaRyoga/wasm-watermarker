#ifndef INCLUDE_kyut_Commutativity_hpp
#define INCLUDE_kyut_Commutativity_hpp

#include <wasm.h>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

namespace kyut {
    [[nodiscard]]
    inline boost::optional<wasm::BinaryOp> getSwappedPredicate(wasm::BinaryOp op) {
        static_assert(wasm::InvalidBinary == 152);

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

        default:
            return boost::none;
        }
    }

    [[nodiscard]]
    inline bool isCommutative(wasm::BinaryOp op) {
        return getSwappedPredicate(op).has_value();
    }

} // namespace kyut

#endif // INCLUDE_kyut_Commutativity_hpp

#ifndef INCLUDE_kyut_comparison_hpp
#define INCLUDE_kyut_comparison_hpp

#include <tuple>

#include <wasm.h>

namespace wasm {
    bool operator<(const Literal &lhs, const Literal &rhs) noexcept {
        if (lhs.type != rhs.type) {
            return lhs.type < rhs.type;
        }

        switch (lhs.type) {
        case Type::none:
        case Type::unreachable:
            return false;

        case Type::i32:
            return lhs.geti32() < rhs.geti32();

        case Type::i64:
            return lhs.geti64() < rhs.geti64();

        case Type::f32:
            return lhs.getf32() < rhs.getf32();

        case Type::f64:
            return lhs.getf64() < rhs.getf64();

        case Type::v128:
            return lhs.getv128() < rhs.getv128();

        default:
            WASM_UNREACHABLE();
        }
    }

    bool operator<(const Expression &lhs, const Expression &rhs) noexcept {
        if (lhs._id != rhs._id) {
            return lhs._id < rhs._id;
        }

        switch (lhs._id) {
        case Expression::Id::BlockId: {
            const auto &l = static_cast<const Block &>(lhs);
            const auto &r = static_cast<const Block &>(rhs);

            if (l.type != r.type) {
                return l.type < r.type;
            }

            // FIXME: use std.
            auto lIt = std::begin(l.list);
            auto lEnd = std::end(l.list);
            auto rIt = std::begin(r.list);
            auto rEnd = std::end(r.list);

            for (; lIt != lEnd && rIt != rEnd; ++lIt, ++rIt) {
                if (**lIt < **rIt) {
                    return true;
                } else if (**rIt < **lIt) {
                    return false;
                }
            }

            return !(lIt != lEnd) && rIt != rEnd;

            // return std::lexicographical_compare(std::begin(l.list),
            //                                     std::end(l.list),
            //                                     std::begin(r.list),
            //                                     std::end(r.list),
            //                                     [](const auto &a, const auto &b) { return *a < *b; });
        }

        case Expression::Id::IfId: {
            const auto &l = static_cast<const If &>(lhs);
            const auto &r = static_cast<const If &>(rhs);

            if (std::tie(l.type, *l.condition, *l.ifTrue) < std::tie(r.type, *r.condition, *r.ifTrue)) {
                return true;
            }

            if (std::tie(r.type, *r.condition, *r.ifTrue) < std::tie(l.type, *l.condition, *l.ifTrue)) {
                return false;
            }

            if (r.ifFalse == nullptr) {
                return false;
            }

            if (l.ifFalse == nullptr) {
                return true;
            }

            return *l.ifFalse < *r.ifFalse;
        }

        case Expression::Id::LoopId: {
            const auto &l = static_cast<const Loop &>(lhs);
            const auto &r = static_cast<const Loop &>(rhs);
            return std::tie(l.type, *l.body) < std::tie(r.type, *r.body);
        }

        case Expression::Id::BreakId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::SwitchId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::CallId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::CallIndirectId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::GetLocalId: {
            const auto &l = static_cast<const GetLocal &>(lhs);
            const auto &r = static_cast<const GetLocal &>(rhs);
            return std::tie(l.type, l.index) < std::tie(r.type, r.index);
        }

        case Expression::Id::SetLocalId: {
            const auto &l = static_cast<const SetLocal &>(lhs);
            const auto &r = static_cast<const SetLocal &>(rhs);
            return std::tie(l.type, l.index, *l.value) < std::tie(r.type, r.index, *r.value);
        }

        case Expression::Id::GetGlobalId: {
            const auto &l = static_cast<const GetGlobal &>(lhs);
            const auto &r = static_cast<const GetGlobal &>(rhs);
            return std::tie(l.type, l.name) < std::tie(r.type, r.name);
        }

        case Expression::Id::SetGlobalId: {
            const auto &l = static_cast<const SetGlobal &>(lhs);
            const auto &r = static_cast<const SetGlobal &>(rhs);
            return std::tie(l.type, l.name, *l.value) < std::tie(r.type, r.name, *r.value);
        }

        case Expression::Id::LoadId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::StoreId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::ConstId: {
            const auto &l = static_cast<const Const &>(lhs);
            const auto &r = static_cast<const Const &>(rhs);
            return std::tie(l.type, l.value) < std::tie(r.type, r.value);
        }

        case Expression::Id::UnaryId: {
            const auto &l = static_cast<const Unary &>(lhs);
            const auto &r = static_cast<const Unary &>(rhs);
            return std::tie(l.type, *l.value) < std::tie(r.type, *r.value);
        }

        case Expression::Id::BinaryId: {
            const auto &l = static_cast<const Binary &>(lhs);
            const auto &r = static_cast<const Binary &>(rhs);
            return std::tie(l.type, *l.left, *l.right) < std::tie(r.type, *r.left, *r.right);
        }

        case Expression::Id::SelectId: {
            const auto &l = static_cast<const Select &>(lhs);
            const auto &r = static_cast<const Select &>(rhs);
            return std::tie(l.type, *l.condition, *l.ifTrue, *l.ifFalse) <
                   std::tie(r.type, *r.condition, *r.ifTrue, *r.ifFalse);
        }

        case Expression::Id::DropId: {
            const auto &l = static_cast<const Drop &>(lhs);
            const auto &r = static_cast<const Drop &>(rhs);
            return std::tie(l.type, *l.value) < std::tie(r.type, *r.value);
        }

        case Expression::Id::ReturnId: {
            const auto &l = static_cast<const Return &>(lhs);
            const auto &r = static_cast<const Return &>(rhs);

            if (l.type != r.type) {
                return l.type < r.type;
            }

            if (r.value == nullptr) {
                return false;
            }

            if (l.value == nullptr) {
                return true;
            }

            return *l.value < *r.value;
        }

        case Expression::Id::HostId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::AtomicRMWId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::AtomicCmpxchgId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::AtomicWaitId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::AtomicWakeId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::SIMDExtractId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::SIMDReplaceId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::SIMDShuffleId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::SIMDBitselectId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::SIMDShiftId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::MemoryInitId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::DataDropId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::MemoryCopyId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::MemoryFillId:
            WASM_UNREACHABLE(); // TODO:

        case Expression::Id::NopId:
        case Expression::Id::UnreachableId:
            return false;

        default:
            WASM_UNREACHABLE();
        }
    }
} // namespace wasm

#endif // INCLUDE_kyut_comparison_hpp

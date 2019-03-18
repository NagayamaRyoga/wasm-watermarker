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

    bool operator<(const ExpressionList &lhs, const ExpressionList &rhs) noexcept {
        auto lIt = lhs.begin();
        auto lEnd = lhs.end();
        auto rIt = rhs.begin();
        auto rEnd = rhs.end();

        for (; lIt != lEnd && rIt != rEnd; ++lIt, ++rIt) {
            if (**lIt < **rIt) {
                return true;
            } else if (**rIt < **lIt) {
                return false;
            }
        }

        return !(lIt != lEnd) && rIt != rEnd;
    }

    bool operator<(const Expression &lhs, const Expression &rhs) noexcept {
        if (lhs._id != rhs._id) {
            return lhs._id < rhs._id;
        }

        switch (lhs._id) {
        case Expression::Id::BlockId: {
            const auto &l = static_cast<const Block &>(lhs);
            const auto &r = static_cast<const Block &>(rhs);

            return std::tie(l.type, l.list) < std::tie(r.type, r.list);
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

        case Expression::Id::CallId: {
            const auto &l = static_cast<const Call &>(lhs);
            const auto &r = static_cast<const Call &>(rhs);
            return std::tie(l.type, l.operands, l.target) < std::tie(r.type, r.operands, r.target);
        }

        case Expression::Id::CallIndirectId: {
            const auto &l = static_cast<const CallIndirect &>(lhs);
            const auto &r = static_cast<const CallIndirect &>(rhs);
            return std::tie(l.type, l.operands, *l.target) < std::tie(r.type, r.operands, *r.target);
        }

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

        case Expression::Id::LoadId: {
            const auto &l = static_cast<const Load &>(lhs);
            const auto &r = static_cast<const Load &>(rhs);
            return std::tie(l.type, l.bytes, l.signed_, l.offset, l.align, l.isAtomic, *l.ptr) <
                   std::tie(r.type, r.bytes, r.signed_, r.offset, r.align, r.isAtomic, *r.ptr);
        }

        case Expression::Id::StoreId: {
            const auto &l = static_cast<const Store &>(lhs);
            const auto &r = static_cast<const Store &>(rhs);
            return std::tie(l.type, l.bytes, l.offset, l.align, l.isAtomic, *l.ptr, *l.value) <
                   std::tie(r.type, r.bytes, r.offset, r.align, r.isAtomic, *r.ptr, *r.value);
        }

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

        case Expression::Id::AtomicRMWId: {
            const auto &l = static_cast<const AtomicRMW &>(lhs);
            const auto &r = static_cast<const AtomicRMW &>(rhs);
            return std::tie(l.type, l.op, l.bytes, l.offset, *l.ptr, *l.value) <
                   std::tie(r.type, r.op, r.bytes, r.offset, *r.ptr, *r.value);
        }

        case Expression::Id::AtomicCmpxchgId: {
            const auto &l = static_cast<const AtomicCmpxchg &>(lhs);
            const auto &r = static_cast<const AtomicCmpxchg &>(rhs);
            return std::tie(l.type, l.bytes, l.offset, *l.ptr, *l.expected, *l.replacement) <
                   std::tie(r.type, r.bytes, r.offset, *r.ptr, *r.expected, *r.replacement);
        }

        case Expression::Id::AtomicWaitId: {
            const auto &l = static_cast<const AtomicWait &>(lhs);
            const auto &r = static_cast<const AtomicWait &>(rhs);
            return std::tie(l.type, l.offset, *l.ptr, *l.expected, *l.timeout) <
                   std::tie(r.type, r.offset, *r.ptr, *r.expected, *r.timeout);
        }

        case Expression::Id::AtomicWakeId: {
            const auto &l = static_cast<const AtomicWake &>(lhs);
            const auto &r = static_cast<const AtomicWake &>(rhs);
            return std::tie(l.type, l.offset, *l.ptr, *l.wakeCount) < std::tie(r.type, r.offset, *r.ptr, *r.wakeCount);
        }

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

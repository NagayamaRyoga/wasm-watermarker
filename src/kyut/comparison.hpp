#ifndef INCLUDE_kyut_comparison_hpp
#define INCLUDE_kyut_comparison_hpp

#include <tuple>

#include <boost/optional.hpp> // boost::optional is more useful than std::optional

#include <wasm.h>

namespace wasm {
    bool operator<(const Expression &lhs, const Expression &rhs) noexcept;
    bool operator<(const ExpressionList &lhs, const ExpressionList &rhs) noexcept;

    constexpr bool operator<(const Literal &lhs, const Literal &rhs) noexcept {
        if (lhs.type != rhs.type) {
            return lhs.type < rhs.type;
        }

        switch (lhs.type) {
        case Type::none:
        case Type::unreachable:
        case Type::except_ref:
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

    inline bool operator<(const Expression &lhs, const Expression &rhs) noexcept {
        constexpr auto opt = [](const Expression *p) -> boost::optional<const Expression &> {
            if (p) {
                return {*p};
            }
            return boost::none;
        };

        if (lhs._id != rhs._id) {
            return lhs._id < rhs._id;
        }

        switch (lhs._id) {
        case Expression::Id::BlockId: {
            const auto &l = *lhs.cast<Block>();
            const auto &r = *rhs.cast<Block>();

            return std::forward_as_tuple(l.type, l.list) < std::forward_as_tuple(r.type, r.list);
        }

        case Expression::Id::IfId: {
            const auto &l = *lhs.cast<If>();
            const auto &r = *rhs.cast<If>();

            return std::forward_as_tuple(l.type, *l.condition, *l.ifTrue, opt(l.ifFalse)) <
                   std::forward_as_tuple(r.type, *r.condition, *r.ifTrue, opt(r.ifFalse));
        }

        case Expression::Id::LoopId: {
            const auto &l = *lhs.cast<Loop>();
            const auto &r = *rhs.cast<Loop>();

            return std::forward_as_tuple(l.type, *l.body) < std::forward_as_tuple(r.type, *r.body);
        }

        case Expression::Id::BreakId: {
            const auto &l = *lhs.cast<Break>();
            const auto &r = *rhs.cast<Break>();

            return std::forward_as_tuple(l.type, opt(l.value), opt(l.condition)) <
                   std::forward_as_tuple(r.type, opt(r.value), opt(r.condition));
        }

        case Expression::Id::SwitchId: {
            const auto &l = *lhs.cast<Switch>();
            const auto &r = *rhs.cast<Switch>();

            return std::forward_as_tuple(l.type, opt(l.condition), opt(l.value)) <
                   std::forward_as_tuple(r.type, opt(r.condition), opt(r.value));
        }

        case Expression::Id::CallId: {
            const auto &l = *lhs.cast<Call>();
            const auto &r = *rhs.cast<Call>();

            return std::forward_as_tuple(l.type, l.operands, l.target) <
                   std::forward_as_tuple(r.type, r.operands, r.target);
        }

        case Expression::Id::CallIndirectId: {
            const auto &l = *lhs.cast<CallIndirect>();
            const auto &r = *rhs.cast<CallIndirect>();

            return std::forward_as_tuple(l.type, l.operands, *l.target) <
                   std::forward_as_tuple(r.type, r.operands, *r.target);
        }

        case Expression::Id::GetLocalId: {
            const auto &l = *lhs.cast<GetLocal>();
            const auto &r = *rhs.cast<GetLocal>();

            return std::forward_as_tuple(l.type, l.index) < std::forward_as_tuple(r.type, r.index);
        }

        case Expression::Id::SetLocalId: {
            const auto &l = *lhs.cast<SetLocal>();
            const auto &r = *rhs.cast<SetLocal>();

            return std::forward_as_tuple(l.type, l.index, *l.value) < std::forward_as_tuple(r.type, r.index, *r.value);
        }

        case Expression::Id::GetGlobalId: {
            const auto &l = *lhs.cast<GetGlobal>();
            const auto &r = *rhs.cast<GetGlobal>();

            return std::forward_as_tuple(l.type, l.name) < std::forward_as_tuple(r.type, r.name);
        }

        case Expression::Id::SetGlobalId: {
            const auto &l = *lhs.cast<SetGlobal>();
            const auto &r = *rhs.cast<SetGlobal>();

            return std::forward_as_tuple(l.type, l.name, *l.value) < std::forward_as_tuple(r.type, r.name, *r.value);
        }

        case Expression::Id::LoadId: {
            const auto &l = *lhs.cast<Load>();
            const auto &r = *rhs.cast<Load>();

            return std::forward_as_tuple(l.type, l.bytes, l.offset, l.align, l.isAtomic, *l.ptr) <
                   std::forward_as_tuple(r.type, r.bytes, r.offset, r.align, r.isAtomic, *r.ptr);
        }

        case Expression::Id::StoreId: {
            const auto &l = *lhs.cast<Store>();
            const auto &r = *rhs.cast<Store>();

            return std::forward_as_tuple(l.type, l.bytes, l.offset, l.align, l.isAtomic, *l.ptr, *l.value) <
                   std::forward_as_tuple(r.type, r.bytes, r.offset, r.align, r.isAtomic, *r.ptr, *r.value);
        }

        case Expression::Id::ConstId: {
            const auto &l = *lhs.cast<Const>();
            const auto &r = *rhs.cast<Const>();

            return std::forward_as_tuple(l.type, l.value) < std::forward_as_tuple(r.type, r.value);
        }

        case Expression::Id::UnaryId: {
            const auto &l = *lhs.cast<Unary>();
            const auto &r = *rhs.cast<Unary>();

            return std::forward_as_tuple(l.type, l.op, *l.value) < std::forward_as_tuple(r.type, r.op, *r.value);
        }

        case Expression::Id::BinaryId: {
            const auto &l = *lhs.cast<Binary>();
            const auto &r = *rhs.cast<Binary>();

            const auto &[ll, lr] = *l.left < *l.right ? std::forward_as_tuple(*l.left, *l.right)
                                                      : std::forward_as_tuple(*l.right, *l.left);

            const auto &[rl, rr] = *r.left < *r.right ? std::forward_as_tuple(*r.left, *r.right)
                                                      : std::forward_as_tuple(*r.right, *r.left);

            return std::forward_as_tuple(l.type, l.op, ll, lr) < std::forward_as_tuple(r.type, r.op, rl, rr);
        }

        case Expression::Id::SelectId: {
            const auto &l = *lhs.cast<Select>();
            const auto &r = *rhs.cast<Select>();

            return std::forward_as_tuple(l.type, *l.condition, *l.ifTrue, *l.ifFalse) <
                   std::forward_as_tuple(r.type, *r.condition, *r.ifTrue, *r.ifFalse);
        }

        case Expression::Id::DropId: {
            const auto &l = *lhs.cast<Drop>();
            const auto &r = *rhs.cast<Drop>();

            return std::forward_as_tuple(l.type, *l.value) < std::forward_as_tuple(r.type, *r.value);
        }

        case Expression::Id::ReturnId: {
            const auto &l = *lhs.cast<Return>();
            const auto &r = *rhs.cast<Return>();

            return std::forward_as_tuple(l.type, opt(l.value)) < std::forward_as_tuple(r.type, opt(r.value));
        }

        case Expression::Id::HostId: {
            const auto &l = *lhs.cast<Host>();
            const auto &r = *rhs.cast<Host>();

            return std::forward_as_tuple(l.type, l.op, l.operands, l.nameOperand) <
                   std::forward_as_tuple(r.type, r.op, r.operands, r.nameOperand);
        }

        case Expression::Id::AtomicRMWId: {
            const auto &l = *lhs.cast<AtomicRMW>();
            const auto &r = *rhs.cast<AtomicRMW>();

            return std::forward_as_tuple(l.type, l.op, l.bytes, l.offset, *l.ptr, *l.value) <
                   std::forward_as_tuple(r.type, r.op, r.bytes, r.offset, *r.ptr, *r.value);
        }

        case Expression::Id::AtomicCmpxchgId: {
            const auto &l = *lhs.cast<AtomicCmpxchg>();
            const auto &r = *rhs.cast<AtomicCmpxchg>();

            return std::forward_as_tuple(l.type, l.bytes, l.offset, *l.ptr, *l.expected, *l.replacement) <
                   std::forward_as_tuple(r.type, r.bytes, r.offset, *r.ptr, *r.expected, *r.replacement);
        }

        case Expression::Id::AtomicWaitId: {
            const auto &l = *lhs.cast<AtomicWait>();
            const auto &r = *rhs.cast<AtomicWait>();

            return std::forward_as_tuple(l.type, l.offset, *l.ptr, *l.expected, *l.timeout) <
                   std::forward_as_tuple(r.type, r.offset, *r.ptr, *r.expected, *r.timeout);
        }

        case Expression::Id::AtomicNotifyId: {
            const auto &l = *lhs.cast<AtomicNotify>();
            const auto &r = *rhs.cast<AtomicNotify>();

            return std::forward_as_tuple(l.type, l.offset, *l.ptr, *l.notifyCount) <
                   std::forward_as_tuple(r.type, r.offset, *r.ptr, *r.notifyCount);
        }

        case Expression::Id::SIMDExtractId: {
            const auto &l = *lhs.cast<SIMDExtract>();
            const auto &r = *rhs.cast<SIMDExtract>();

            return std::forward_as_tuple(l.type, l.op, *l.vec, l.index) <
                   std::forward_as_tuple(r.type, r.op, *r.vec, r.index);
        }

        case Expression::Id::SIMDReplaceId: {
            const auto &l = *lhs.cast<SIMDReplace>();
            const auto &r = *rhs.cast<SIMDReplace>();

            return std::forward_as_tuple(l.type, l.op, *l.vec, l.index, *l.value) <
                   std::forward_as_tuple(r.type, r.op, *r.vec, r.index, *r.value);
        }

        case Expression::Id::SIMDShuffleId: {
            const auto &l = *lhs.cast<SIMDShuffle>();
            const auto &r = *rhs.cast<SIMDShuffle>();

            return std::forward_as_tuple(l.type, *l.left, *l.right, l.mask) <
                   std::forward_as_tuple(r.type, *r.left, *r.right, r.mask);
        }

        case Expression::Id::SIMDBitselectId: {
            const auto &l = *lhs.cast<SIMDBitselect>();
            const auto &r = *rhs.cast<SIMDBitselect>();

            return std::forward_as_tuple(l.type, *l.left, *l.right, *l.cond) <
                   std::forward_as_tuple(r.type, *r.left, *r.right, *r.cond);
        }

        case Expression::Id::SIMDShiftId: {
            const auto &l = *lhs.cast<SIMDShift>();
            const auto &r = *rhs.cast<SIMDShift>();

            return std::forward_as_tuple(l.type, l.op, *l.vec, *l.shift) <
                   std::forward_as_tuple(r.type, r.op, *r.vec, *r.shift);
        }

        case Expression::Id::MemoryInitId: {
            const auto &l = *lhs.cast<MemoryInit>();
            const auto &r = *rhs.cast<MemoryInit>();

            return std::forward_as_tuple(l.type, l.segment, *l.dest, *l.offset, *l.size) <
                   std::forward_as_tuple(r.type, r.segment, *r.dest, *r.offset, *r.size);
        }

        case Expression::Id::DataDropId: {
            const auto &l = *lhs.cast<DataDrop>();
            const auto &r = *rhs.cast<DataDrop>();

            return std::forward_as_tuple(l.type, l.segment) < std::forward_as_tuple(r.type, r.segment);
        }

        case Expression::Id::MemoryCopyId: {
            const auto &l = *lhs.cast<MemoryCopy>();
            const auto &r = *rhs.cast<MemoryCopy>();

            return std::forward_as_tuple(l.type, *l.dest, *l.source, *l.size) <
                   std::forward_as_tuple(r.type, *r.dest, *r.source, *r.size);
        }

        case Expression::Id::MemoryFillId: {
            const auto &l = *lhs.cast<MemoryFill>();
            const auto &r = *rhs.cast<MemoryFill>();

            return std::forward_as_tuple(l.type, *l.dest, *l.value, *l.size) <
                   std::forward_as_tuple(r.type, *r.dest, *r.value, *r.size);
        }

        case Expression::Id::NopId:
        case Expression::Id::UnreachableId:
            return false;

        default:
            WASM_UNREACHABLE();
        }
    } // namespace wasm

    inline bool operator<(const ExpressionList &lhs, const ExpressionList &rhs) noexcept {
        return std::lexicographical_compare(
            std::begin(lhs), std::end(lhs), std::begin(rhs), std::end(rhs), [](const auto &a, const auto &b) {
                return *a < *b;
            });
    }
} // namespace wasm

#endif // INCLUDE_kyut_comparison_hpp

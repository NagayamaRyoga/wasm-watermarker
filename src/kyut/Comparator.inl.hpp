#ifndef INCLUDE_kyut_Comparator_inl_hpp
#define INCLUDE_kyut_Comparator_inl_hpp

#include <wasm.h>

#include <boost/optional.hpp>

namespace wasm {
    [[nodiscard]] inline bool operator<(const Literal &a, const Literal &b) {
        if (a.type != b.type) {
            return a.type < b.type;
        }

        switch (a.type) {
        case none:
            return false;
        case i32:
            return a.geti32() < b.geti32();
        case i64:
            return a.geti64() < b.geti64();
        case f32:
            return a.getf32() < b.getf32();
        case f64:
            return a.getf64() < b.getf64();
        case v128:
            return a.getv128() < b.getv128();
        case anyref:
        case exnref:
            return false;
        case unreachable:
            return false;
        default:
            WASM_UNREACHABLE();
        }
    }

    [[nodiscard]] inline bool operator<(const Expression &a, const Expression &b) {
        static_assert(Expression::NumExpressionIds == 44);

        constexpr auto opt = [](const Expression *p) -> boost::optional<const Expression &> {
            if (p == nullptr) {
                return boost::none;
            }

            return *p;
        };

        if (std::tie(a._id, a.type) != std::tie(b._id, b.type)) {
            return std::tie(a._id, a.type) < std::tie(b._id, b.type);
        }

        switch (a._id) {
        case Expression::BlockId: {
            const auto &x = *a.cast<Block>();
            const auto &y = *b.cast<Block>();

            return x.list < y.list;
        }
        case Expression::IfId: {
            const auto &x = *a.cast<If>();
            const auto &y = *b.cast<If>();

            return std::forward_as_tuple(*x.condition, *x.ifTrue, opt(x.ifFalse)) <
                   std::forward_as_tuple(*y.condition, *y.ifTrue, opt(y.ifFalse));
        }
        case Expression::LoopId: {
            const auto &x = *a.cast<Loop>();
            const auto &y = *b.cast<Loop>();

            return *x.body < *y.body;
        }
        case Expression::BreakId: {
            const auto &x = *a.cast<Break>();
            const auto &y = *b.cast<Break>();

            return std::forward_as_tuple(opt(x.value), opt(x.condition)) <
                   std::forward_as_tuple(opt(y.value), opt(y.condition));
        }
        case Expression::SwitchId: {
            const auto &x = *a.cast<Switch>();
            const auto &y = *b.cast<Switch>();

            return std::forward_as_tuple(opt(x.value), opt(x.condition)) <
                   std::forward_as_tuple(opt(y.value), opt(y.condition));
        }
        case Expression::CallId: {
            const auto &x = *a.cast<Call>();
            const auto &y = *b.cast<Call>();

            return std::forward_as_tuple(x.operands, x.target) < std::forward_as_tuple(y.operands, y.target);
        }
        case Expression::CallIndirectId: {
            const auto &x = *a.cast<CallIndirect>();
            const auto &y = *b.cast<CallIndirect>();

            return std::forward_as_tuple(x.operands, *x.target) < std::forward_as_tuple(y.operands, *y.target);
        }
        case Expression::LocalGetId: {
            const auto &x = *a.cast<LocalGet>();
            const auto &y = *b.cast<LocalGet>();

            return x.index < y.index;
        }
        case Expression::LocalSetId: {
            const auto &x = *a.cast<LocalSet>();
            const auto &y = *b.cast<LocalSet>();

            return std::tie(x.index, *x.value) < std::tie(y.index, *y.value);
        }
        case Expression::GlobalGetId: {
            const auto &x = *a.cast<GlobalGet>();
            const auto &y = *b.cast<GlobalGet>();

            return x.name < y.name;
        }
        case Expression::GlobalSetId: {
            const auto &x = *a.cast<GlobalSet>();
            const auto &y = *b.cast<GlobalSet>();

            return std::tie(*x.value, x.name) < std::tie(*y.value, y.name);
        }
        case Expression::LoadId: {
            const auto &x = *a.cast<Load>();
            const auto &y = *b.cast<Load>();

            return *x.ptr < *y.ptr;
        }
        case Expression::StoreId: {
            const auto &x = *a.cast<Store>();
            const auto &y = *b.cast<Store>();

            return std::tie(*x.ptr, *x.value) < std::tie(*y.ptr, *y.value);
        }
        case Expression::ConstId: {
            const auto &x = *a.cast<Const>();
            const auto &y = *b.cast<Const>();

            return x.value < y.value;
        }
        case Expression::UnaryId: {
            const auto &x = *a.cast<Unary>();
            const auto &y = *b.cast<Unary>();

            return std::tie(x.op, *x.value) < std::tie(y.op, *y.value);
        }
        case Expression::BinaryId: {
            const auto &x = *a.cast<Binary>();
            const auto &y = *b.cast<Binary>();

            // Normalize expression
            constexpr auto normalize =
                [](const wasm::Binary &bin) -> std::tuple<wasm::BinaryOp, wasm::Expression &, wasm::Expression &> {
                if (!kyut::isCommutative(bin.op)) {
                    // Noncommutative
                    return {bin.op, *bin.left, *bin.right};
                }

                // Commutative
                if (*bin.right < *bin.left) {
                    return {*kyut::getSwappedPredicate(bin.op), *bin.right, *bin.left};
                } else {
                    return {bin.op, *bin.left, *bin.right};
                }
            };

            return normalize(x) < normalize(y);
        }
        case Expression::SelectId: {
            const auto &x = *a.cast<Select>();
            const auto &y = *b.cast<Select>();

            return std::tie(*x.ifTrue, *x.ifFalse, *x.condition) < std::tie(*y.ifTrue, *y.ifFalse, *y.condition);
        }
        case Expression::DropId: {
            const auto &x = *a.cast<Drop>();
            const auto &y = *b.cast<Drop>();

            return *x.value < *y.value;
        }
        case Expression::ReturnId: {
            const auto &x = *a.cast<Return>();
            const auto &y = *b.cast<Return>();

            return opt(x.value) < opt(y.value);
        }
        case Expression::HostId: {
            const auto &x = *a.cast<Host>();
            const auto &y = *b.cast<Host>();

            return std::tie(x.op, x.operands) < std::tie(y.op, y.operands);
        }
        case Expression::NopId: {
            return false;
        }
        case Expression::UnreachableId: {
            return false;
        }
        case Expression::AtomicRMWId: {
            const auto &x = *a.cast<AtomicRMW>();
            const auto &y = *b.cast<AtomicRMW>();

            return std::tie(x.op, x.bytes, x.offset.addr, *x.ptr, *x.value) <
                   std::tie(y.op, y.bytes, y.offset.addr, *y.ptr, *y.value);
        }
        case Expression::AtomicCmpxchgId: {
            const auto &x = *a.cast<AtomicCmpxchg>();
            const auto &y = *b.cast<AtomicCmpxchg>();

            return std::tie(x.bytes, x.offset.addr, *x.ptr, *x.expected, *x.replacement) <
                   std::tie(y.bytes, y.offset.addr, *y.ptr, *y.expected, *y.replacement);
        }
        case Expression::AtomicWaitId: {
            const auto &x = *a.cast<AtomicWait>();
            const auto &y = *b.cast<AtomicWait>();

            return std::tie(x.offset.addr, *x.ptr, *x.expected, *x.timeout, x.expectedType) <
                   std::tie(y.offset.addr, *y.ptr, *y.expected, *y.timeout, y.expectedType);
        }
        case Expression::AtomicNotifyId: {
            const auto &x = *a.cast<AtomicNotify>();
            const auto &y = *b.cast<AtomicNotify>();

            return std::tie(x.offset.addr, *x.ptr, *x.notifyCount) < std::tie(y.offset.addr, *y.ptr, *y.notifyCount);
        }
        case Expression::AtomicFenceId: {
            [[maybe_unused]] const auto &x = *a.cast<AtomicFence>();
            [[maybe_unused]] const auto &y = *b.cast<AtomicFence>();

            return false;
        }
        case Expression::SIMDExtractId: {
            const auto &x = *a.cast<SIMDExtract>();
            const auto &y = *b.cast<SIMDExtract>();

            return std::tie(x.op, *x.vec, x.index) < std::tie(y.op, *y.vec, y.index);
        }
        case Expression::SIMDReplaceId: {
            const auto &x = *a.cast<SIMDReplace>();
            const auto &y = *b.cast<SIMDReplace>();

            return std::tie(x.op, *x.vec, x.index, *x.value) < std::tie(y.op, *y.vec, y.index, *y.value);
        }
        case Expression::SIMDShuffleId: {
            const auto &x = *a.cast<SIMDShuffle>();
            const auto &y = *b.cast<SIMDShuffle>();

            return std::tie(*x.left, *x.right, x.mask) < std::tie(*y.left, *y.right, y.mask);
        }
        case Expression::SIMDTernaryId: {
            const auto &x = *a.cast<SIMDTernary>();
            const auto &y = *b.cast<SIMDTernary>();

            return std::tie(*x.a, *x.b, *x.c) < std::tie(*y.a, *y.b, *y.c);
        }
        case Expression::SIMDShiftId: {
            const auto &x = *a.cast<SIMDShift>();
            const auto &y = *b.cast<SIMDShift>();

            return std::tie(x.op, *x.vec, *x.shift) < std::tie(y.op, *y.vec, *y.shift);
        }
        case Expression::SIMDLoadId: {
            const auto &x = *a.cast<SIMDLoad>();
            const auto &y = *b.cast<SIMDLoad>();

            return std::tie(x.op, x.offset, x.align, *x.ptr) < std::tie(y.op, y.offset, y.align, *y.ptr) ;
        }
        case Expression::MemoryInitId: {
            const auto &x = *a.cast<MemoryInit>();
            const auto &y = *b.cast<MemoryInit>();

            return std::tie(x.segment, *x.dest, *x.offset, *x.size) < std::tie(y.segment, *y.dest, *y.offset, *y.size);
        }
        case Expression::DataDropId: {
            const auto &x = *a.cast<DataDrop>();
            const auto &y = *b.cast<DataDrop>();

            return x.segment < y.segment;
        }
        case Expression::MemoryCopyId: {
            const auto &x = *a.cast<MemoryCopy>();
            const auto &y = *b.cast<MemoryCopy>();

            return std::tie(*x.dest, *x.source, *x.size) < std::tie(*y.dest, *y.source, *y.size);
        }
        case Expression::MemoryFillId: {
            const auto &x = *a.cast<MemoryFill>();
            const auto &y = *b.cast<MemoryFill>();

            return std::tie(*x.dest, *x.value, *x.size) < std::tie(*y.dest, *y.value, *y.size);
        }
        case Expression::PushId:{
            const auto &x = *a.cast<Push>();
            const auto &y = *b.cast<Push>();

            return std::tie(*x.value) < std::tie(*y.value);
        }
        case Expression::PopId:{
            [[maybe_unused]] const auto &x = *a.cast<Pop>();
            [[maybe_unused]] const auto &y = *b.cast<Pop>();

            return false;
        }
        case Expression::TryId:{
            const auto &x = *a.cast<Try>();
            const auto &y = *b.cast<Try>();

            return std::forward_as_tuple(opt(x.body), opt(x.catchBody)) < std::forward_as_tuple(opt(y.body), opt(y.catchBody));
        }
        case Expression::ThrowId:{
            const auto &x = *a.cast<Throw>();
            const auto &y = *b.cast<Throw>();

            return std::tie(x.operands) < std::tie(y.operands);
        }
        case Expression::RethrowId:{
            const auto &x = *a.cast<Rethrow>();
            const auto &y = *b.cast<Rethrow>();

            return std::forward_as_tuple(opt(x.exnref)) < std::forward_as_tuple(opt(y.exnref));
        }
        case Expression::BrOnExnId:{
            const auto &x = *a.cast<BrOnExn>();
            const auto &y = *b.cast<BrOnExn>();

            return std::forward_as_tuple(opt(x.exnref)) < std::forward_as_tuple(opt(y.exnref));
        }
        default:
            WASM_UNREACHABLE();
        }
    } // namespace wasm

    [[nodiscard]] inline bool operator<(const ExpressionList &a, const ExpressionList &b) {
        return std::lexicographical_compare(
            std::begin(a), std::end(a), std::begin(b), std::end(b), [](const auto &x, const auto &y) {
                return *x < *y;
            });
    }
} // namespace wasm

#endif // INCLUDE_kyut_Comparator_inl_hpp

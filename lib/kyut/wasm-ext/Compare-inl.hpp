#ifndef INCLUDE_kyut_wasm_ext_Compare_inl_hpp
#define INCLUDE_kyut_wasm_ext_Compare_inl_hpp

#include "Compare.hpp"

#include <algorithm>
#include "../Commutativity.hpp"

namespace wasm {
    inline bool operator<(const Literal& a, const Literal& b) {
        return std::less<Literal>{}(a, b);
    }

    inline bool operator<(const Expression& a, const Expression& b) {
        static_assert(Expression::NumExpressionIds == 49);

        // Expr* -> optional<Expr&>
        constexpr auto opt = [](const Expression* p) -> boost::optional<const Expression&> {
            if (p == nullptr) {
                return boost::none;
            }
            return *p;
        };

        if (a._id != b._id) {
            return a._id < b._id;
        }

        if (a.type != b.type) {
            return a.type < b.type;
        }

        switch (a._id) {
            case Expression::Id::BlockId: {
                const auto& x = *a.cast<Block>();
                const auto& y = *b.cast<Block>();

                return x.list < y.list;
            }
            case Expression::Id::IfId: {
                const auto& x = *a.cast<If>();
                const auto& y = *b.cast<If>();

                return std::forward_as_tuple(*x.condition, *x.ifTrue, opt(x.ifFalse)) <
                       std::forward_as_tuple(*y.condition, *y.ifTrue, opt(y.ifFalse));
            }
            case Expression::Id::LoopId: {
                const auto& x = *a.cast<Loop>();
                const auto& y = *b.cast<Loop>();

                return *x.body < *y.body;
            }
            case Expression::Id::BreakId: {
                const auto& x = *a.cast<Break>();
                const auto& y = *b.cast<Break>();

                return std::forward_as_tuple(opt(x.value), opt(x.condition)) <
                       std::forward_as_tuple(opt(y.value), opt(y.condition));
            }
            case Expression::Id::SwitchId: {
                const auto& x = *a.cast<Switch>();
                const auto& y = *b.cast<Switch>();

                return std::forward_as_tuple(opt(x.value), opt(x.condition)) <
                       std::forward_as_tuple(opt(y.value), opt(y.condition));
            }
            case Expression::Id::CallId: {
                const auto& x = *a.cast<Call>();
                const auto& y = *b.cast<Call>();

                return x.operands < y.operands;
            }
            case Expression::Id::CallIndirectId: {
                const auto& x = *a.cast<CallIndirect>();
                const auto& y = *b.cast<CallIndirect>();

                return std::forward_as_tuple(x.operands, *x.target) < std::forward_as_tuple(y.operands, *y.target);
            }
            case Expression::Id::LocalGetId: {
                const auto& x = *a.cast<LocalGet>();
                const auto& y = *b.cast<LocalGet>();

                return x.index < y.index;
            }
            case Expression::Id::LocalSetId: {
                const auto& x = *a.cast<LocalSet>();
                const auto& y = *b.cast<LocalSet>();

                return std::tie(x.index, *x.value) < std::tie(y.index, *y.value);
            }
            case Expression::Id::GlobalGetId: {
                const auto& x = *a.cast<GlobalGet>();
                const auto& y = *b.cast<GlobalGet>();

                return x.name < y.name;
            }
            case Expression::Id::GlobalSetId: {
                const auto& x = *a.cast<GlobalSet>();
                const auto& y = *b.cast<GlobalSet>();

                return std::tie(*x.value, x.name) < std::tie(*y.value, y.name);
            }
            case Expression::Id::LoadId: {
                const auto& x = *a.cast<Load>();
                const auto& y = *b.cast<Load>();

                return *x.ptr < *y.ptr;
            }
            case Expression::Id::StoreId: {
                const auto& x = *a.cast<Store>();
                const auto& y = *b.cast<Store>();

                return std::tie(*x.ptr, *x.value) < std::tie(*y.ptr, *y.value);
            }
            case Expression::Id::ConstId: {
                const auto& x = *a.cast<Const>();
                const auto& y = *b.cast<Const>();

                return x.value < y.value;
            }
            case Expression::Id::UnaryId: {
                const auto& x = *a.cast<Unary>();
                const auto& y = *b.cast<Unary>();

                return std::tie(x.op, *x.value) < std::tie(y.op, *y.value);
            }
            case Expression::Id::BinaryId: {
                const auto& x = *a.cast<Binary>();
                const auto& y = *b.cast<Binary>();

                constexpr auto normalize = [](const wasm::Binary& node)
                    -> std::tuple<wasm::BinaryOp, wasm::Expression&, wasm::Expression&> {
                    if (!kyut::is_commutative(node.op)) {
                        // Non-commutative binary expr
                        return {node.op, *node.left, *node.right};
                    }

                    // Commutative binary expr
                    if (*node.left < *node.right) {
                        return {node.op, *node.left, *node.right};
                    } else {
                        return {*kyut::swapped_binary_op(node.op), *node.right, *node.left};
                    }
                };

                return normalize(x) < normalize(y);
            }
            case Expression::Id::SelectId: {
                const auto& x = *a.cast<Select>();
                const auto& y = *b.cast<Select>();

                return std::tie(*x.ifTrue, *x.ifFalse, *x.condition) < std::tie(*y.ifTrue, *y.ifFalse, *y.condition);
            }
            case Expression::Id::DropId: {
                const auto& x = *a.cast<Drop>();
                const auto& y = *b.cast<Drop>();

                return *x.value < *y.value;
            }
            case Expression::Id::ReturnId: {
                const auto& x = *a.cast<Return>();
                const auto& y = *b.cast<Return>();

                return opt(x.value) < opt(y.value);
            }
            case Expression::Id::MemorySizeId: {
                [[maybe_unused]] const auto& x = *a.cast<MemorySize>();
                [[maybe_unused]] const auto& y = *b.cast<MemorySize>();

                return false;
            }
            case Expression::Id::MemoryGrowId: {
                const auto& x = *a.cast<MemoryGrow>();
                const auto& y = *b.cast<MemoryGrow>();

                return opt(x.delta) < opt(y.delta);
            }
            case Expression::Id::NopId: {
                [[maybe_unused]] const auto& x = *a.cast<Nop>();
                [[maybe_unused]] const auto& y = *b.cast<Nop>();

                return false;
            }
            case Expression::Id::UnreachableId: {
                [[maybe_unused]] const auto& x = *a.cast<Unreachable>();
                [[maybe_unused]] const auto& y = *b.cast<Unreachable>();

                return false;
            }
            case Expression::Id::AtomicRMWId: {
                const auto& x = *a.cast<AtomicRMW>();
                const auto& y = *b.cast<AtomicRMW>();

                return std::tie(x.op, x.bytes, x.offset, *x.ptr, *x.value) <
                       std::tie(y.op, y.bytes, y.offset, *y.ptr, *y.value);
            }
            case Expression::Id::AtomicCmpxchgId: {
                const auto& x = *a.cast<AtomicCmpxchg>();
                const auto& y = *b.cast<AtomicCmpxchg>();

                return std::tie(x.bytes, x.offset, *x.ptr, *x.expected, *x.replacement) <
                       std::tie(y.bytes, y.offset, *y.ptr, *y.expected, *y.replacement);
            }
            case Expression::Id::AtomicWaitId: {
                const auto& x = *a.cast<AtomicWait>();
                const auto& y = *b.cast<AtomicWait>();

                return std::tie(x.offset, *x.ptr, *x.expected, *x.timeout, x.expectedType) <
                       std::tie(y.offset, *y.ptr, *y.expected, *y.timeout, y.expectedType);
            }
            case Expression::Id::AtomicNotifyId: {
                const auto& x = *a.cast<AtomicNotify>();
                const auto& y = *b.cast<AtomicNotify>();

                return std::tie(x.offset, *x.ptr, *x.notifyCount) < std::tie(y.offset, *y.ptr, *y.notifyCount);
            }
            case Expression::Id::AtomicFenceId: {
                [[maybe_unused]] const auto& x = *a.cast<AtomicFence>();
                [[maybe_unused]] const auto& y = *b.cast<AtomicFence>();

                return false;
            }
            case Expression::Id::SIMDExtractId: {
                const auto& x = *a.cast<SIMDExtract>();
                const auto& y = *b.cast<SIMDExtract>();

                return std::tie(x.op, *x.vec, x.index) < std::tie(y.op, *y.vec, y.index);
            }
            case Expression::Id::SIMDReplaceId: {
                const auto& x = *a.cast<SIMDReplace>();
                const auto& y = *b.cast<SIMDReplace>();

                return std::tie(x.op, *x.vec, x.index, *x.value) < std::tie(y.op, *y.vec, y.index, *y.value);
            }
            case Expression::Id::SIMDShuffleId: {
                const auto& x = *a.cast<SIMDShuffle>();
                const auto& y = *b.cast<SIMDShuffle>();

                return std::tie(*x.left, *x.right, x.mask) < std::tie(*y.left, *y.right, y.mask);
            }
            case Expression::Id::SIMDTernaryId: {
                const auto& x = *a.cast<SIMDTernary>();
                const auto& y = *b.cast<SIMDTernary>();

                return std::tie(*x.a, *x.b, *x.c) < std::tie(*y.a, *y.b, *y.c);
            }
            case Expression::Id::SIMDShiftId: {
                const auto& x = *a.cast<SIMDShift>();
                const auto& y = *b.cast<SIMDShift>();

                return std::tie(x.op, *x.vec, *x.shift) < std::tie(y.op, *y.vec, *y.shift);
            }
            case Expression::Id::SIMDLoadId: {
                const auto& x = *a.cast<SIMDLoad>();
                const auto& y = *b.cast<SIMDLoad>();

                return std::tie(x.op, x.offset, x.align, *x.ptr) < std::tie(y.op, y.offset, y.align, *y.ptr);
            }
            case Expression::Id::MemoryInitId: {
                const auto& x = *a.cast<MemoryInit>();
                const auto& y = *b.cast<MemoryInit>();

                return std::tie(x.segment, *x.dest, *x.offset, *x.size) < std::tie(y.segment, *y.dest, *y.offset, *y.size);
            }
            case Expression::Id::DataDropId: {
                const auto& x = *a.cast<DataDrop>();
                const auto& y = *b.cast<DataDrop>();

                return x.segment < y.segment;
            }
            case Expression::Id::MemoryCopyId: {
                const auto& x = *a.cast<MemoryCopy>();
                const auto& y = *b.cast<MemoryCopy>();

                return std::tie(*x.dest, *x.source, *x.size) < std::tie(*y.dest, *y.source, *y.size);
            }
            case Expression::Id::MemoryFillId: {
                const auto& x = *a.cast<MemoryFill>();
                const auto& y = *b.cast<MemoryFill>();

                return std::tie(*x.dest, *x.value, *x.size) < std::tie(*y.dest, *y.value, *y.size);
            }
            case Expression::Id::PopId: {
                [[maybe_unused]] const auto& x = *a.cast<Pop>();
                [[maybe_unused]] const auto& y = *b.cast<Pop>();

                return false;
            }
            case Expression::Id::RefNullId: {
                [[maybe_unused]] const auto& x = *a.cast<RefNull>();
                [[maybe_unused]] const auto& y = *b.cast<RefNull>();

                return false;
            }
            case Expression::Id::RefIsNullId: {
                const auto& x = *a.cast<RefIsNull>();
                const auto& y = *b.cast<RefIsNull>();

                return *x.value < *y.value;
            }
            case Expression::Id::RefFuncId: {
                [[maybe_unused]] const auto& x = *a.cast<RefFunc>();
                [[maybe_unused]] const auto& y = *b.cast<RefFunc>();

                return false;
            }
            case Expression::Id::TryId: {
                const auto& x = *a.cast<Try>();
                const auto& y = *b.cast<Try>();

                return std::forward_as_tuple(opt(x.body), opt(x.catchBody)) < std::forward_as_tuple(opt(y.body), opt(y.catchBody));
            }
            case Expression::Id::ThrowId: {
                const auto& x = *a.cast<Throw>();
                const auto& y = *b.cast<Throw>();

                return x.operands < y.operands;
            }
            case Expression::Id::RethrowId: {
                const auto& x = *a.cast<Rethrow>();
                const auto& y = *b.cast<Rethrow>();

                return std::forward_as_tuple(opt(x.exnref)) < std::forward_as_tuple(opt(y.exnref));
            }
            case Expression::Id::BrOnExnId: {
                const auto& x = *a.cast<BrOnExn>();
                const auto& y = *b.cast<BrOnExn>();

                return std::forward_as_tuple(opt(x.exnref)) < std::forward_as_tuple(opt(y.exnref));
            }
            case Expression::Id::TupleMakeId: {
                const auto& x = *a.cast<TupleMake>();
                const auto& y = *b.cast<TupleMake>();

                return x.operands < y.operands;
            }
            case Expression::Id::TupleExtractId: {
                const auto& x = *a.cast<TupleExtract>();
                const auto& y = *b.cast<TupleExtract>();

                return std::tie(*x.tuple, x.index) < std::tie(*y.tuple, y.index);
            }
            default: {
                WASM_UNREACHABLE("unknown expression id");
            }
        }
    }

    inline bool operator<(const ExpressionList& a, const ExpressionList& b) {
        return std::lexicographical_compare(
            std::begin(a),
            std::end(a),
            std::begin(b),
            std::end(b),
            [](const Expression* a, const Expression* b) {
                return *a < *b;
            });
    }
} // namespace wasm

#endif // INCLUDE_kyut_wasm_ext_Compare_inl_hpp

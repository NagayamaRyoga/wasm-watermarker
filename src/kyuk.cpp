#include <fmt/printf.h>
#include "cmdline.h"
#include "pass.h"
#include "wasm-io.h"
#include "wasm-validator.h"

namespace {
    const std::string program_name = "kyuk";
    const std::string version = "0.1.0";

    std::uint32_t insert_data(wasm::Module& module, const std::string& watermark) {
        // Calculate new data offset for watermark storage (and counter)
        std::uint32_t offset = 0;

        for (const auto& seg : module.memory.segments) {
            std::uint32_t begin = seg.offset->cast<wasm::Const>()->value.geti32();
            std::uint32_t size = seg.data.size();

            offset = (std::max)(begin + size, offset);
        }

        const std::uint32_t align = 16;
        offset = offset % align == 0 ? offset : (offset / align + 1) * align;

        std::vector<char> data;

        // Insert the counter area
        // counter has type of i32
        data.emplace_back(0x00);
        data.emplace_back(0x00);
        data.emplace_back(0x00);
        data.emplace_back(0x00);

        // Insert the watermark
        data.emplace_back('\x01'); // Non-zero backward sentinel (in the current implementation)
        for (const auto& x : watermark) {
            data.emplace_back(x); // Watermark cannot contain null character (in the current implementation)
        }
        data.emplace_back('\0'); // Need null terminated (in the current implementation)

        // Add the memory segment to the module
        const auto offset_expr = module.allocator.alloc<wasm::Const>();
        offset_expr->set(wasm::Literal{static_cast<std::int32_t>(offset)});

        module.memory.segments.emplace_back(offset_expr, data);

        return offset;
    }

    wasm::Expression* build_new_operand(::MixedArena& allocator, std::uint32_t data_offset) {
        const auto counter_offset = data_offset + 0;
        const auto watermark_offset = data_offset + 4;

        // (i32.const value)
        const auto i32_const = [&](std::uint32_t value) {
            const auto expr = allocator.alloc<wasm::Const>();
            expr->set(wasm::Literal{value});
            return expr;
        };

        // load i
        // (i32.load offset=<i> (i32.const 0))
        const auto load_i = [&]() {
            const auto load = allocator.alloc<wasm::Load>();
            load->type = wasm::Type::i32;
            load->bytes = 4;
            load->signed_ = false;
            load->offset = counter_offset;
            load->align = 4;
            load->isAtomic = false;
            load->ptr = i32_const(0);

            return load;
        };

        // store i
        // (i32.store offset=<i> (i32.const 0) <expr>)
        const auto store_i = [&](wasm::Expression* expr) {
            const auto store = allocator.alloc<wasm::Store>();
            store->bytes = 4;
            store->offset = counter_offset;
            store->align = 4;
            store->isAtomic = false;
            store->ptr = i32_const(0);
            store->value = expr;
            store->valueType = wasm::Type::i32;

            return store;
        };

        // (i32.add <left> <right>)
        const auto add = [&](wasm::Expression* left, wasm::Expression* right) {
            const auto add = allocator.alloc<wasm::Binary>();
            add->type = wasm::Type::i32;
            add->op = wasm::AddInt32;
            add->left = left;
            add->right = right;

            return add;
        };

        // inc i
        const auto inc_i = [&]() {
            return store_i(add(load_i(), i32_const(1)));
        };

        // load w[i]
        // (i32.load8_u offset=<w> (i32.load <i>))
        const auto load_w = [&]() {
            const auto load = allocator.alloc<wasm::Load>();
            load->type = wasm::Type::i32;
            load->bytes = 1;
            load->signed_ = false;
            load->offset = watermark_offset;
            load->align = 1;
            load->isAtomic = false;
            load->ptr = load_i();

            return load;
        };

        // inc i if w is not ended
        const auto inc_i_if = [&]() {
            const auto if_ = allocator.alloc<wasm::If>();
            if_->condition = load_w();
            if_->ifTrue = inc_i();
            if_->ifFalse = nullptr;

            return if_;
        };

        // (block $1 i32
        //   ;; increment
        //   ;; if w[i] { i = i+1 }
        //   (if
        //     ;; w[i]
        //     (i32.load8_s offset=off+4
        //       (i32.load offset=off
        //         (i32.const 0)
        //       )
        //     )
        //     ;; i = i+1
        //     (i32.store offset=off
        //       (i32.const 0)
        //       ;; i+1
        //       (i32.add
        //         ;; load i
        //         (i32.load offset=off
        //           (i32.const 0)
        //         )
        //         (i32.const 1)
        //       )
        //     )
        //   )
        //   ;; load watermark
        //   ;; w[i]
        //   (i32.load8_s offset=off+4
        //     ;; i
        //     (i32.load offset=off
        //       (i32.const 0)
        //     )
        //   )
        // )
        const auto block = allocator.alloc<wasm::Block>();
        block->type = wasm::Type::i32;
        block->list.push_back(inc_i_if());
        block->list.push_back(load_w());

        return block;
    }

    void modify_call(wasm::Module& module, std::uint32_t data_offset, wasm::Call& call) {
        call.operands.push_back(build_new_operand(module.allocator, data_offset));
    }

    void add_parameter(wasm::Function& f) {
        wasm::Type new_param{wasm::Type::i32};

        switch (f.sig.params.getID()) {
            case wasm::Type::none:
                // No parameter
                f.sig.params = new_param;
                break;

            case wasm::Type::i32:
            case wasm::Type::i64:
            case wasm::Type::f32:
            case wasm::Type::f64:
            case wasm::Type::v128:
                // 1 primitive parameter
                f.sig.params = wasm::Type{f.sig.params, new_param};
                break;

            default:
                if (f.sig.params.isTuple()) {
                    // 2 or more parameters
                    wasm::Tuple tuple{};
                    for (const auto& t : f.sig.params) {
                        tuple.types.emplace_back(t);
                    }
                    tuple.types.emplace_back(new_param);
                    f.sig.params = wasm::Type{tuple};
                } else {
                    // May not be reached
                    throw std::runtime_error{"unknown parameter type"};
                }
                break;
        }
    }

    class FunctionCallVisitor : public wasm::OverriddenVisitor<FunctionCallVisitor, void> {
        wasm::Module& module_;
        std::uint32_t data_offset_;

    public:
        explicit FunctionCallVisitor(wasm::Module& module, std::uint32_t data_offset)
            : module_(module)
            , data_offset_(data_offset) {
        }

        void visit(const wasm::ExpressionList& l) {
            for (const auto& p : l) {
                visit(p);
            }
        }

        void visit(wasm::Expression* p) {
            if (p) {
                wasm::OverriddenVisitor<FunctionCallVisitor, void>::visit(p);
            }
        }

        void visitBlock(wasm::Block* p) {
            visit(p->list);
        }

        void visitIf(wasm::If* p) {
            visit(p->condition);
            visit(p->ifTrue);
            visit(p->ifFalse);
        }

        void visitLoop(wasm::Loop* p) {
            visit(p->body);
        }

        void visitBreak(wasm::Break* p) {
            visit(p->value);
            visit(p->condition);
        }

        void visitSwitch(wasm::Switch* p) {
            visit(p->condition);
            visit(p->value);
        }

        void visitCall(wasm::Call* p) {
            visit(p->operands);

            const auto f = module_.getFunctionOrNull(p->target);
            if (f == nullptr || f->body != nullptr) {
                return;
            }

            // `f` is an import function
            // Pass an extra argument to `f`
            modify_call(module_, data_offset_, *p);
        }

        void visitCallIndirect(wasm::CallIndirect* p) {
            visit(p->target);
            visit(p->operands);
        }

        void visitLocalGet([[maybe_unused]] wasm::LocalGet* p) {
        }

        void visitLocalSet(wasm::LocalSet* p) {
            visit(p->value);
        }

        void visitGlobalGet([[maybe_unused]] wasm::GlobalGet* p) {
        }

        void visitGlobalSet(wasm::GlobalSet* p) {
            visit(p->value);
        }

        void visitLoad(wasm::Load* p) {
            visit(p->ptr);
        }

        void visitStore(wasm::Store* p) {
            visit(p->ptr);
            visit(p->value);
        }

        void visitConst([[maybe_unused]] wasm::Const* p) {
        }

        void visitUnary(wasm::Unary* p) {
            visit(p->value);
        }

        void visitBinary(wasm::Binary* p) {
            visit(p->left);
            visit(p->right);
        }

        void visitSelect(wasm::Select* p) {
            visit(p->ifTrue);
            visit(p->ifFalse);
            visit(p->condition);
        }

        void visitDrop(wasm::Drop* p) {
            visit(p->value);
        }

        void visitReturn(wasm::Return* p) {
            visit(p->value);
        }

        void visitMemorySize([[maybe_unused]] wasm::MemorySize* p) {
        }

        void visitMemoryGrow(wasm::MemoryGrow* p) {
            visit(p->delta);
        }

        void visitNop([[maybe_unused]] wasm::Nop* p) {
        }

        void visitUnreachable([[maybe_unused]] wasm::Unreachable* p) {
        }

        void visitAtomicRMW(wasm::AtomicRMW* p) {
            visit(p->ptr);
            visit(p->value);
        }

        void visitAtomicCmpxchg(wasm::AtomicCmpxchg* p) {
            visit(p->ptr);
            visit(p->expected);
            visit(p->replacement);
        }

        void visitAtomicWait(wasm::AtomicWait* p) {
            visit(p->ptr);
            visit(p->expected);
            visit(p->timeout);
        }

        void visitAtomicNotify(wasm::AtomicNotify* p) {
            visit(p->ptr);
            visit(p->notifyCount);
        }

        void visitAtomicFence([[maybe_unused]] wasm::AtomicFence* p) {
        }

        void visitSIMDExtract(wasm::SIMDExtract* p) {
            visit(p->vec);
        }

        void visitSIMDReplace(wasm::SIMDReplace* p) {
            visit(p->vec);
            visit(p->value);
        }

        void visitSIMDShuffle(wasm::SIMDShuffle* p) {
            visit(p->left);
            visit(p->right);
        }

        void visitSIMDTernary(wasm::SIMDTernary* p) {
            visit(p->a);
            visit(p->b);
            visit(p->c);
        }

        void visitSIMDShift(wasm::SIMDShift* p) {
            visit(p->vec);
            visit(p->shift);
        }

        void visitSIMDLoad(wasm::SIMDLoad* p) {
            visit(p->ptr);
        }

        void visitMemoryInit(wasm::MemoryInit* p) {
            visit(p->dest);
            visit(p->offset);
            visit(p->size);
        }

        void visitDataDrop([[maybe_unused]] wasm::DataDrop* p) {
        }

        void visitMemoryCopy(wasm::MemoryCopy* p) {
            visit(p->dest);
            visit(p->source);
            visit(p->size);
        }

        void visitMemoryFill(wasm::MemoryFill* p) {
            visit(p->dest);
            visit(p->value);
            visit(p->size);
        }

        void visitPop([[maybe_unused]] wasm::Pop* p) {
        }

        void visitRefNull([[maybe_unused]] wasm::RefNull* p) {
        }

        void visitRefIsNull(wasm::RefIsNull* p) {
            visit(p->value);
        }

        void visitRefFunc([[maybe_unused]] wasm::RefFunc* p) {
        }

        void visitTry(wasm::Try* p) {
            visit(p->body);
            visit(p->catchBody);
        }

        void visitThrow(wasm::Throw* p) {
            visit(p->operands);
        }

        void visitRethrow([[maybe_unused]] wasm::Rethrow* p) {
        }

        void visitBrOnExn([[maybe_unused]] wasm::BrOnExn* p) {
        }

        void visitTupleMake(wasm::TupleMake* p) {
            visit(p->operands);
        }

        void visitTupleExtract(wasm::TupleExtract* p) {
            visit(p->tuple);
        }

        void visitFunction(wasm::Function* f) {
            visit(f->body);
        }
    };
} // namespace

int main(int argc, char* argv[]) {
    cmdline::parser options{};

    options.add("help", 'h', "Print help message");
    options.add("version", 'v', "Print version");

    options.add<std::string>("output", 'o', "Output filename", true);
    options.add<std::string>("watermark", 'w', "Watermark to embed", true);
    options.add("debug", 'd', "Preserve debug info");

    options.set_program_name(program_name);
    options.footer("filename");

    // Parse command line arguments.
    // Exit the program if help flag is specified or arguments are invalid.
    options.parse_check(argc, argv);

    if (options.exist("version")) {
        // Show the program version.
        fmt::print("{} v{}\n", program_name, version);
        std::exit(EXIT_SUCCESS);
    }

    if (options.get<std::string>("watermark").empty()) {
        // Zero-length watermark.
        fmt::print(std::cerr, "no watermark\n");
        fmt::print(std::cerr, "{}", options.usage());
        std::exit(EXIT_FAILURE);
    }

    if (options.rest().size() == 0) {
        // No input file specified.
        fmt::print(std::cerr, "no input file\n");
        fmt::print(std::cerr, "{}", options.usage());
        std::exit(EXIT_FAILURE);
    }

    if (options.rest().size() > 1) {
        // Too many input files.
        fmt::print(std::cerr, "too many input files\n");
        fmt::print(std::cerr, "{}", options.usage());
        std::exit(EXIT_FAILURE);
    }

    const auto input = options.rest()[0];
    const auto output = options.get<std::string>("output");
    const auto watermark = options.get<std::string>("watermark");
    const auto preserve_debug = options.exist("debug");

    try {
        wasm::Module module{};
        wasm::ModuleReader{}.read(input, module);

        // Insert a watermark data
        const std::uint32_t offset = insert_data(module, watermark);

        (void)offset;

        // Embedding
        for (const auto& f : module.functions) {
            if (f->body == nullptr) {
                // `f` is an import function
                // Add extra parameters to `f`
                add_parameter(*f);
            } else {
                // `f` is not an import function
                // Add extra arguments to import function callings
                FunctionCallVisitor{module, offset}.visitFunction(f.get());
            }
        }

        // Validation
        if (!wasm::WasmValidator{}.validate(module)) {
            std::exit(EXIT_FAILURE);
        }

        wasm::ModuleWriter w{};
        w.setDebugInfo(preserve_debug);
        w.writeBinary(module, output);
    } catch (const std::exception& e) {
        fmt::print(std::cerr, "error: {}\n", e.what());
        std::exit(EXIT_FAILURE);
    } catch (const wasm::ParseException& e) {
        e.dump(std::cerr);
        std::exit(EXIT_FAILURE);
    }
}

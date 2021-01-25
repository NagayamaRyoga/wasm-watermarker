.PHONY: all
all: \
	zlib/function-reorder/ANebjkXMZJwaaRMM \
	zlib/export-reorder/ANebjkXMZJwaaRMM \
	zlib/operand-swap/ANebjkXMZJwaaRMM \
	zlib/function-reorder/Qg5lb3WN8a7eRFRb \
	zlib/export-reorder/Qg5lb3WN8a7eRFRb \
	zlib/operand-swap/Qg5lb3WN8a7eRFRb \
	zlib/function-reorder/oVs8OISLiQH0RXlq \
	zlib/export-reorder/oVs8OISLiQH0RXlq \
	zlib/operand-swap/oVs8OISLiQH0RXlq

.PHONY: bench-zlib
bench-zlib: \
	zlib/raw/wasm-flate \
	zlib/function-reorder/ANebjkXMZJwaaRMM \
	zlib/export-reorder/ANebjkXMZJwaaRMM \
	zlib/operand-swap/ANebjkXMZJwaaRMM \
	zlib/function-reorder/Qg5lb3WN8a7eRFRb \
	zlib/export-reorder/Qg5lb3WN8a7eRFRb \
	zlib/operand-swap/Qg5lb3WN8a7eRFRb \
	zlib/function-reorder/oVs8OISLiQH0RXlq \
	zlib/export-reorder/oVs8OISLiQH0RXlq \
	zlib/operand-swap/oVs8OISLiQH0RXlq

	node zlib/raw/bench.js | tee bench-zlib.txt
	node zlib/function-reorder/ANebjkXMZJwaaRMM/bench.js | tee -a bench-zlib.txt
	node zlib/export-reorder/ANebjkXMZJwaaRMM/bench.js | tee -a bench-zlib.txt
	node zlib/operand-swap/ANebjkXMZJwaaRMM/bench.js | tee -a bench-zlib.txt
	node zlib/function-reorder/Qg5lb3WN8a7eRFRb/bench.js | tee -a bench-zlib.txt
	node zlib/export-reorder/Qg5lb3WN8a7eRFRb/bench.js | tee -a bench-zlib.txt
	node zlib/operand-swap/Qg5lb3WN8a7eRFRb/bench.js | tee -a bench-zlib.txt
	node zlib/function-reorder/oVs8OISLiQH0RXlq/bench.js | tee -a bench-zlib.txt
	node zlib/export-reorder/oVs8OISLiQH0RXlq/bench.js | tee -a bench-zlib.txt
	node zlib/operand-swap/oVs8OISLiQH0RXlq/bench.js | tee -a bench-zlib.txt

zlib/raw/wasm-flate: ../node_modules/wasm-flate
	cp -r $< $@

# $(eval $(call embed,watermark,method))
define embed
.PHONY: zlib/$2/$1
zlib/$2/$1: zlib/$2/$1/bench.js zlib/$2/$1/wasm-flate/wasm_flate.js zlib/$2/$1/wasm-flate/wasm_flate_bg.wasm

zlib/$2/$1/bench.js: zlib/raw/bench.js
	mkdir -p $${@D}
	cp $$< $$@

zlib/$2/$1/wasm-flate/wasm_flate.js: zlib/raw/wasm-flate
	mkdir -p $${@D}
	cp $$</wasm_flate.js $$@

zlib/$2/$1/wasm-flate/wasm_flate_bg.wasm: zlib/raw/wasm-flate
	mkdir -p $${@D}
	snpi -m $2 -w $1 -o $$@ $$</wasm_flate_bg.wasm
endef

$(eval $(call embed,ANebjkXMZJwaaRMM,function-reorder))
$(eval $(call embed,ANebjkXMZJwaaRMM,export-reorder))
$(eval $(call embed,ANebjkXMZJwaaRMM,operand-swap))

$(eval $(call embed,Qg5lb3WN8a7eRFRb,function-reorder))
$(eval $(call embed,Qg5lb3WN8a7eRFRb,export-reorder))
$(eval $(call embed,Qg5lb3WN8a7eRFRb,operand-swap))

$(eval $(call embed,oVs8OISLiQH0RXlq,function-reorder))
$(eval $(call embed,oVs8OISLiQH0RXlq,export-reorder))
$(eval $(call embed,oVs8OISLiQH0RXlq,operand-swap))


.PHONY: all
all: zlib jq

### zlib
.PHONY: zlib
zlib: \
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
define embed_zlib
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

$(eval $(call embed_zlib,ANebjkXMZJwaaRMM,function-reorder))
$(eval $(call embed_zlib,ANebjkXMZJwaaRMM,export-reorder))
$(eval $(call embed_zlib,ANebjkXMZJwaaRMM,operand-swap))

$(eval $(call embed_zlib,Qg5lb3WN8a7eRFRb,function-reorder))
$(eval $(call embed_zlib,Qg5lb3WN8a7eRFRb,export-reorder))
$(eval $(call embed_zlib,Qg5lb3WN8a7eRFRb,operand-swap))

$(eval $(call embed_zlib,oVs8OISLiQH0RXlq,function-reorder))
$(eval $(call embed_zlib,oVs8OISLiQH0RXlq,export-reorder))
$(eval $(call embed_zlib,oVs8OISLiQH0RXlq,operand-swap))

### jq
.PHONY: jq
jq: \
	jq/function-reorder/ANebjkXMZJwaaRMM \
	jq/export-reorder/ANebjkXMZJwaaRMM \
	jq/operand-swap/ANebjkXMZJwaaRMM \
	jq/function-reorder/Qg5lb3WN8a7eRFRb \
	jq/export-reorder/Qg5lb3WN8a7eRFRb \
	jq/operand-swap/Qg5lb3WN8a7eRFRb \
	jq/function-reorder/oVs8OISLiQH0RXlq \
	jq/export-reorder/oVs8OISLiQH0RXlq \
	jq/operand-swap/oVs8OISLiQH0RXlq

jq/raw/jq-web: ../node_modules/jq-web
	cp -r $< $@

# $(eval $(call embed,watermark,method))
define embed_jq
.PHONY: jq/$2/$1
jq/$2/$1: jq/$2/$1/bench.js jq/$2/$1/large-file.json jq/$2/$1/jq-web/jq.wasm.min.js jq/$2/$1/jq-web/jq.wasm.wasm

jq/$2/$1/bench.js: jq/raw/bench.js
	mkdir -p $${@D}
	cp $$< $$@

jq/$2/$1/large-file.json: jq/raw/large-file.json
	mkdir -p $${@D}
	cp $$< $$@

jq/$2/$1/jq-web/jq.wasm.min.js: jq/raw/jq-web
	mkdir -p $${@D}
	cp $$</jq.wasm.min.js $$@

jq/$2/$1/jq-web/jq.wasm.wasm: jq/raw/jq-web
	mkdir -p $${@D}
	snpi -m $2 -w $1 -o $$@ $$</jq.wasm.wasm
endef

$(eval $(call embed_jq,ANebjkXMZJwaaRMM,function-reorder))
$(eval $(call embed_jq,ANebjkXMZJwaaRMM,export-reorder))
$(eval $(call embed_jq,ANebjkXMZJwaaRMM,operand-swap))

$(eval $(call embed_jq,Qg5lb3WN8a7eRFRb,function-reorder))
$(eval $(call embed_jq,Qg5lb3WN8a7eRFRb,export-reorder))
$(eval $(call embed_jq,Qg5lb3WN8a7eRFRb,operand-swap))

$(eval $(call embed_jq,oVs8OISLiQH0RXlq,function-reorder))
$(eval $(call embed_jq,oVs8OISLiQH0RXlq,export-reorder))
$(eval $(call embed_jq,oVs8OISLiQH0RXlq,operand-swap))

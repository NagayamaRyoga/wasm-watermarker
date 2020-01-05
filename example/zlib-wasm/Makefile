SNPI := ../../build/src/snpi
AS := ../../build/binaryen/src/binaryen-build/bin/wasm-as

W1 := 60b725f1
W2 := 0c9c85c7
W3 := 0d97880d
W4 := fe8191b3

SRC := zlib-sample.wasm

TARGETS := \
	dist/zlib-1-${W1}.wasm \
	dist/zlib-2-${W1}.wasm \
	dist/zlib-3-${W1}.wasm \
	dist/zlib-1-${W2}.wasm \
	dist/zlib-2-${W2}.wasm \
	dist/zlib-3-${W2}.wasm \
	dist/zlib-1-${W3}.wasm \
	dist/zlib-2-${W3}.wasm \
	dist/zlib-3-${W3}.wasm \
	dist/zlib-1-${W4}.wasm \
	dist/zlib-2-${W4}.wasm \
	dist/zlib-3-${W4}.wasm

all: ${TARGETS}

bench: all
	node benchmark.js

dist/zlib-1-%.wasm: zlib-sample.wasm
	@mkdir -p ${@D}
	${SNPI} $< funcord ${@:dist/zlib-1-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

dist/zlib-2-%.wasm: zlib-sample.wasm
	@mkdir -p ${@D}
	${SNPI} $< opswap ${@:dist/zlib-2-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

dist/zlib-3-%.wasm: dist/zlib-1-%.wasm
	@mkdir -p ${@D}
	${SNPI} $< opswap ${@:dist/zlib-3-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

clean:
	${RM} dist

.PHONY: all bench clean

SNPI := ../../build/src/snpi
AS := ../../build/binaryen/src/binaryen-build/bin/wasm-as

W1 := 60b725f1
W2 := 0c9c85c7
W3 := 0d97880d
W4 := fe8191b3

SRC := zlib-sample.wasm

TARGETS := \
	zlib-1-${W1}.wasm \
	zlib-2-${W1}.wasm \
	zlib-3-${W1}.wasm \
	zlib-1-${W2}.wasm \
	zlib-2-${W2}.wasm \
	zlib-3-${W2}.wasm \
	zlib-1-${W3}.wasm \
	zlib-2-${W3}.wasm \
	zlib-3-${W3}.wasm \
	zlib-1-${W4}.wasm \
	zlib-2-${W4}.wasm \
	zlib-3-${W4}.wasm

all: ${TARGETS}
	node benchmark.js

zlib-1-%.wasm: zlib-sample.wasm
	${SNPI} $< funcord ${@:zlib-1-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

zlib-2-%.wasm: zlib-sample.wasm
	${SNPI} $< opswap ${@:zlib-2-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

zlib-3-%.wasm: zlib-1-%.wasm
	${SNPI} $< opswap ${@:zlib-3-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

clean:
	${RM} zlib-1-*.wasm zlib-2-*.wasm zlib-3-*.wasm *.wast

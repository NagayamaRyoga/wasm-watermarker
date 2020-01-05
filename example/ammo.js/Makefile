SNPI := ../../build/src/snpi
AS := ../../build/binaryen/src/binaryen-build/bin/wasm-as

W1 := 60b725f1
W2 := 0c9c85c7
W3 := 0d97880d
W4 := fe8191b3

all: \
	ammo-1-${W1} ammo-2-${W1} ammo-3-${W1} \
	ammo-1-${W2} ammo-2-${W2} ammo-3-${W2} \
	ammo-1-${W3} ammo-2-${W3} ammo-3-${W3} \
	ammo-1-${W4} ammo-2-${W4} ammo-3-${W4} \
	#

ammo-1-${W1}: ammo-1-${W1}/ammo.wasm.js ammo-1-${W1}/ammo.wasm.wasm
ammo-1-${W2}: ammo-1-${W2}/ammo.wasm.js ammo-1-${W2}/ammo.wasm.wasm
ammo-1-${W3}: ammo-1-${W3}/ammo.wasm.js ammo-1-${W3}/ammo.wasm.wasm
ammo-1-${W4}: ammo-1-${W4}/ammo.wasm.js ammo-1-${W4}/ammo.wasm.wasm
ammo-2-${W1}: ammo-2-${W1}/ammo.wasm.js ammo-2-${W1}/ammo.wasm.wasm
ammo-2-${W2}: ammo-2-${W2}/ammo.wasm.js ammo-2-${W2}/ammo.wasm.wasm
ammo-2-${W3}: ammo-2-${W3}/ammo.wasm.js ammo-2-${W3}/ammo.wasm.wasm
ammo-2-${W4}: ammo-2-${W4}/ammo.wasm.js ammo-2-${W4}/ammo.wasm.wasm
ammo-3-${W1}: ammo-3-${W1}/ammo.wasm.js ammo-3-${W1}/ammo.wasm.wasm
ammo-3-${W2}: ammo-3-${W2}/ammo.wasm.js ammo-3-${W2}/ammo.wasm.wasm
ammo-3-${W3}: ammo-3-${W3}/ammo.wasm.js ammo-3-${W3}/ammo.wasm.wasm
ammo-3-${W4}: ammo-3-${W4}/ammo.wasm.js ammo-3-${W4}/ammo.wasm.wasm

ammo-%/ammo.wasm.js: ammo.js/builds/ammo.wasm.js
	@mkdir -p ${@D}
	cp $< $@

ammo-1-%/ammo.wasm.wasm: ammo.js/builds/ammo.wasm.wasm
	@mkdir -p ${@D}
	${SNPI} $< funcord ${@:zlib-1-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

ammo-2-%/ammo.wasm.wasm: ammo.js/builds/ammo.wasm.wasm
	@mkdir -p ${@D}
	${SNPI} $< opswap ${@:zlib-2-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

ammo-3-%/ammo.wasm.wasm: ammo-1-%/ammo.wasm.wasm
	@mkdir -p ${@D}
	${SNPI} $< opswap ${@:zlib-3-%.wasm=%} > ${@:%.wasm=%.wast}
	${AS} ${@:%.wasm=%.wast} -o $@
	${RM} ${@:%.wasm=%.wast}

clean:
	${RM} -rf ammo-*

.PHONY: all clean

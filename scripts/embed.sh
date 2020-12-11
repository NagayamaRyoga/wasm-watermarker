#!/bin/sh
SNPI="./build/bin/snpi"
PISN="./build/bin/pisn"

OUT_DIR="./out"

WASM_AMMO_JS="./node_modules/ammo.js/builds/ammo.wasm.wasm"
WASM_JQ_WEB="./node_modules/jq-web/jq.wasm.wasm"
WASM_SOURCE_MAP="./node_modules/source-map/lib/mappings.wasm"
WASM_WASM_FLATE="./node_modules/wasm-flate/wasm_flate_bg.wasm"

function copy() {
    local file="$1"
    local out="$2"

    echo "cp $file $out"
    mkdir -p "$(dirname "$out")"
    cp "$file" "$out"
}

function embed() {
    local file="$1"
    local out="$2"
    local method="$3"
    local watermark="$4"
    local chunk_size="${5:-20}"

    echo "$SNPI -o $out -m $method -w $watermark -c $chunk_size $file"
    mkdir -p "$(dirname "$out")"

    local embedded_size
    embedded_size="$("$SNPI" -o "$out" -m "$method" -w "$watermark" -c "$chunk_size" "$file")"
    echo "$embedded_size"
}

copy "$WASM_AMMO_JS" "$OUT_DIR/ammo.js/ammo.wasm.wasm"
copy "$WASM_JQ_WEB" "$OUT_DIR/jq-web/jq.wasm.wasm"
copy "$WASM_SOURCE_MAP" "$OUT_DIR/source-map/mappings.wasm"
copy "$WASM_WASM_FLATE" "$OUT_DIR/wasm-flate/wasm_flate_bg.wasm"

embed "$WASM_AMMO_JS" "$OUT_DIR/ammo.js/ammo.wasm-op-TEST.wasm" "operand-swap" "TEST"
embed "$WASM_JQ_WEB" "$OUT_DIR/jq-web/jq-op-TEST.wasm.wasm" "operand-swap" "TEST"
embed "$WASM_SOURCE_MAP" "$OUT_DIR/source-map/mappings-op-TEST.wasm" "operand-swap" "TEST"
embed "$WASM_WASM_FLATE" "$OUT_DIR/wasm-flate/wasm_flate_bg-op-TEST.wasm" "operand-swap" "TEST"

embed "$WASM_AMMO_JS" "$OUT_DIR/ammo.js/ammo.wasm-fn-TEST.wasm" "function-reorder" "TEST"
embed "$WASM_JQ_WEB" "$OUT_DIR/jq-web/jq-fn-TEST.wasm.wasm" "function-reorder" "TEST"
embed "$WASM_SOURCE_MAP" "$OUT_DIR/source-map/mappings-fn-TEST.wasm" "function-reorder" "TEST"
embed "$WASM_WASM_FLATE" "$OUT_DIR/wasm-flate/wasm_flate_bg-fn-TEST.wasm" "function-reorder" "TEST"

embed "$WASM_AMMO_JS" "$OUT_DIR/ammo.js/ammo.wasm-ex-TEST.wasm" "export-reorder" "TEST"
embed "$WASM_JQ_WEB" "$OUT_DIR/jq-web/jq-ex-TEST.wasm.wasm" "export-reorder" "TEST"
embed "$WASM_SOURCE_MAP" "$OUT_DIR/source-map/mappings-ex-TEST.wasm" "export-reorder" "TEST"
embed "$WASM_WASM_FLATE" "$OUT_DIR/wasm-flate/wasm_flate_bg-ex-TEST.wasm" "export-reorder" "TEST"

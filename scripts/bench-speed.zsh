#!/usr/bin/env zsh
zmodload zsh/datetime # enable EPOCHREALTIME

WATERMARK="Test"

embed() {
    local method="$1"
    local proj="$2"
    local wasm="$3"
    local limit="$4"
    local size="$5"

    local out="./out/size-bench/$method/$proj/$size/$(basename "$wasm")"
    mkdir -p "$(dirname "$out")"

    echo -n "$method\t$proj\t$size\t"
    if [ "$size" -le "$limit" ]; then
        # warm up
        snpi -m "$method" -o "$out" -w "$WATERMARK" -l "$size" "$wasm" > /dev/null
        snpi -m "$method" -o "$out" -w "$WATERMARK" -l "$size" "$wasm" > /dev/null
        snpi -m "$method" -o "$out" -w "$WATERMARK" -l "$size" "$wasm" > /dev/null

        local start="$EPOCHREALTIME"
        for i in {1..100}; do
            snpi -m "$method" -o "$out" -w "$WATERMARK" -l "$size" "$wasm" > /dev/null
        done
        local end="$EPOCHREALTIME"
        local duration_ms="$((($end - $start) * 1000 / 100))"
        echo "$duration_ms"
    else
        echo ""
    fi
}

embed_size() {
    local method="$1"
    local proj="$2"
    local wasm="$3"
    local limit="$4"

    embed "$method" "$proj" "$wasm" "$limit" 10
    embed "$method" "$proj" "$wasm" "$limit" 20
    embed "$method" "$proj" "$wasm" "$limit" 50
    embed "$method" "$proj" "$wasm" "$limit" 100
    embed "$method" "$proj" "$wasm" "$limit" 200
    embed "$method" "$proj" "$wasm" "$limit" 500
    embed "$method" "$proj" "$wasm" "$limit" 1000
    embed "$method" "$proj" "$wasm" "$limit" 2000
    embed "$method" "$proj" "$wasm" "$limit" 5000
    embed "$method" "$proj" "$wasm" "$limit" 10000
    embed "$method" "$proj" "$wasm" "$limit" 20000
    embed "$method" "$proj" "$wasm" "$limit" 50000
}

embed_proj() {
    local proj="$1"
    local wasm="$2"
    local limit_1="$3"
    local limit_2="$4"
    local limit_3="$5"

    embed_size "function-reorder"   "$proj" "$wasm"   "$limit_1"
    embed_size "export-reorder"     "$proj" "$wasm"   "$limit_2"
    embed_size "operand-swap"       "$proj" "$wasm"   "$limit_3"
}

embed_proj "Source Map" "./node_modules/source-map/lib/mappings.wasm"    128    67   3130
embed_proj "wasm-flate" "./node_modules/wasm-flate/wasm_flate_bg.wasm"  1040    44   4996
embed_proj "ammo.js"    "./node_modules/ammo.js/builds/ammo.wasm.wasm"  5308  4102  29177
embed_proj "jq-web"     "./node_modules/jq-web/jq.wasm.wasm"            1976    18  22302
embed_proj "vim-wasm"   "./node_modules/vim-wasm/vim.wasm"             14589    56  56392

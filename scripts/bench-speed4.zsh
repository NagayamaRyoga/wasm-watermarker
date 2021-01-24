#!/usr/bin/env zsh
zmodload zsh/datetime # enable EPOCHREALTIME

method4() {
    local proj="$1"
    local wasm="$2"
    local size_bits="$3"
    local size="$(($size_bits / 8))"

    local out="./out/size-bench/dyn/$proj/$size_bits/$(basename "$wasm")"
    mkdir -p "$(dirname "$out")"

    local watermark="$(head -c "$size" /dev/random | base64 | head -c "$size")"

    # warm up
    kyuk -w "$watermark" -o "$out" "$wasm"
    kyuk -w "$watermark" -o "$out" "$wasm"
    kyuk -w "$watermark" -o "$out" "$wasm"

    local times=100
    local start="$EPOCHREALTIME"
    for i in {1..$times}; do
        kyuk -w "$watermark" -o "$out" "$wasm"
    done
    local end="$EPOCHREALTIME"
    local duration_ms="$((($end - $start) * 1000 / $times))"

    echo "$proj\t$size_bits\t$duration_ms"
}

method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 10
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 20
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 50
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 100
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 200
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 500
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 1000
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 2000
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 5000
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 10000
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 20000
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 50000
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 100000
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 200000
method4 "jq-web" "./node_modules/jq-web/jq.wasm.wasm" 500000

method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 10
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 20
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 50
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 100
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 200
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 500
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 1000
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 2000
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 5000
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 10000
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 20000
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 50000
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 100000
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 200000
method4 "vim-wasm" "./node_modules/vim-wasm/vim.wasm" 500000

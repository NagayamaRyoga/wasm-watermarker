#!/usr/bin/env zsh
method4() {
    local proj="$1"
    local wasm="$2"
    local size_bits="$3"
    local size="$(($size_bits / 8))"

    local out="./out/size-bench/dyn/$proj/$size_bits/$(basename "$wasm")"
    mkdir -p "$(dirname "$out")"

    local watermark="$(head -c "$size" /dev/random | base64 | head -c "$size")"
    kyuk -w "$watermark" -o "$out" "$wasm"
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

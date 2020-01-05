#!/usr/bin/env zsh
cd "$(dirname "$0")" || exit 1
path=("$PWD/../build/src" $path[@])

wasms=(
    $PWD/styled-jsx/mappings.wasm
    $PWD/zlib-wasm/zlib-sample.wasm
    $PWD/ammo.js/ammo.js/builds/ammo.wasm.wasm
)

echo "name, size[B], funcs, opswap[bit], funcord[bit]"
for i in $wasms[@]
do
    size="$(stat -f%z "$i")"
    funcs="$(funccnt "$i")"
    funcord="$((snpi "$i" funcord "Test" > /dev/null) |& awk '{print $1}')"
    opswap="$((snpi "$i" opswap "Test" > /dev/null) |& awk '{print $1}')"
    echo "${i/$PWD\//}, $size, $funcs, $opswap, $funcord"
done

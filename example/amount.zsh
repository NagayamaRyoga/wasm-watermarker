#!/usr/bin/env zsh
path=("$PWD/../build/src" $path[@])

wasms=(
    $PWD/styled-jsx/mappings.wasm
    $PWD/zlib-wasm/zlib-sample.wasm
    $PWD/ammo.wasm.wasm
)

echo "name, funcs, size[B], funcord[B], opswap[B]"
for i in $wasms[@]
do
    size="$(stat -f%z "$i")"
    funcs="$(funccnt "$i")"
    funcord="$((snpi "$i" funcord "Test" > /dev/null) |& awk '{print $1}')"
    opswap="$((snpi "$i" opswap "Test" > /dev/null) |& awk '{print $1}')"
    echo "${i/$PWD\//}, $size, $funcs, $funcord, $opswap"
done

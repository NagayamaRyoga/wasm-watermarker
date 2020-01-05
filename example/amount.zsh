#!/usr/bin/env zsh
path=("${0:A:h}/../build/src" $path[@])

wasms=(
    "${0:A:h}/styled-jsx/mappings.wasm"
    "${0:A:h}/zlib-wasm/zlib-sample.wasm"
    "${0:A:h}/ammo.js/ammo.js/builds/ammo.wasm.wasm"
)

echo "name, size[B], funcs, opswap[bit], funcord[bit]"
for i in $wasms[@]
do
    size="$(stat -f%z "$i")"
    funcs="$(funccnt "$i")"
    funcord="$((snpi "$i" funcord "Test" > /dev/null) |& awk '{print $1}')"
    opswap="$((snpi "$i" opswap "Test" > /dev/null) |& awk '{print $1}')"
    echo "${i/${0:A:h}\//}, $size, $funcs, $opswap, $funcord"
done

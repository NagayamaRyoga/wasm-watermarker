# Wasm Watermarker

Software Watermark Embedder for WebAssembly

## How to Clone

```shell
$ git clone --recursive git@github.com:NagayamaRyoga/wasm-watermarker.git
$ cd wasm-watermarker
```

## How to Build

```shell
$ cmake -B build -GCMAKE_BUILD_TYPE=Release .
$ cmake --build build
```

## How to Run

```shell
$ snpi <input-wasm> <watermark>
$ pisn <input-wasm>
```

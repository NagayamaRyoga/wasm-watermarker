# Wasm Watermarker

Software Watermark Embedder for WebAssembly

## Environment

Language: C++17

OS: Ubuntu 18.04

Compilers: GCC, Clang, emcc

Libraries: Boost, Binaryen 1.38.30

## How to Clone

```shell
$ git clone --recursive git@github.com:NagayamaRyoga/wasm-watermarker.git
$ cd wasm-watermarker
```

## How to Build

```shell
$ mkdir build && cd build
$ cmake ..
$ make
```

## How to Run

```shell
$ snpi <input-wasm> <watermark>
$ pisn <input-wasm>
```

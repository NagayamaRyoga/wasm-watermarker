# Wasm Watermarker

WebAssemblyに対する電子透かし埋め込み器

## 開発環境

Language: C++17

OS: Ubuntu 18.04

Compilers: GCC, Clang, emcc

Libraries: binaryen

## 命名規約

## プロジェクトのダウンロード

```shell
$ git clone git@github.com:NagayamaRyoga/wasm-watermarker.git
$ cd wasm-watermarker
```

## ビルド

```shell
$ mkdir build && cd build
$ cmake ..
$ make
```

## 実行方法

```shell
$ wasm-watermarker <input-wasm>
```

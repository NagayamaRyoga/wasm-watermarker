# Wasm Watermarker

WebAssembly に対する電子透かし埋め込み器

## 開発環境

Language: C++17

OS: Ubuntu 18.04

Compilers: GCC, Clang, emcc

Libraries: Boost, Binaryen 1.38.30

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
$ snpi <input-wasm> <watermark>
$ pisn <input-wasm>
```

## ライセンス表示

### example/ammo.wasm.wasm

Copyright (c) 2011 ammo.js contributors

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

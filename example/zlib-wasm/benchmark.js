const Crypto = require("crypto");
const assert = require("assert");

const size = 1_000_000;
const rawData = Crypto.randomBytes(size);

(async function() {
  const libPaths = [
    `zlib-sample.wasm`,
    "dist/zlib-1-0c9c85c7.wasm",
    "dist/zlib-1-0d97880d.wasm",
    "dist/zlib-1-60b725f1.wasm",
    "dist/zlib-1-fe8191b3.wasm",
    "dist/zlib-2-0c9c85c7.wasm",
    "dist/zlib-2-0d97880d.wasm",
    "dist/zlib-2-60b725f1.wasm",
    "dist/zlib-2-fe8191b3.wasm",
    "dist/zlib-3-0c9c85c7.wasm",
    "dist/zlib-3-0d97880d.wasm",
    "dist/zlib-3-60b725f1.wasm",
    "dist/zlib-3-fe8191b3.wasm"
  ];

  const libs = await Promise.all(
    libPaths.map(
      async path => await require("./zlib").initialize(`${__dirname}/${path}`)
    )
  );

  const compressed = libs[0].deflate(rawData);

  // Validation
  libs.forEach((Zlib, i) => {
    assert.deepEqual(
      Zlib.deflate(rawData),
      compressed,
      `${libPaths[i]} - .deflate()`
    );
    assert.deepEqual(
      Zlib.inflate(compressed),
      rawData,
      `${libPaths[i]} - .inflate()`
    );
  });

  const compressionRate = compressed.length / rawData.length;

  console.log(`raw size: ${rawData.length}bytes`);
  console.log(`deflated size: ${compressed.length}bytes`);
  console.log(`compression rate: ${(compressionRate * 100).toFixed(1)}%`);

  libs.forEach((Zlib, i) => {
    // Warming up
    console.time(`Warm - ${libPaths[i]}`);
    for (let i = 0; i < 10; i++) {
      Zlib.deflate(rawData);
    }
    console.timeEnd(`Warm - ${libPaths[i]}`);

    // Benchmark
    console.time(`Benchmark - ${libPaths[i]}`);
    for (let i = 0; i < 100; i++) {
      Zlib.deflate(rawData);
    }
    console.timeEnd(`Benchmark - ${libPaths[i]}`);
  });
})();

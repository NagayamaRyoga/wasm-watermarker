const Crypto = require("crypto");
const assert = require("assert");

const size = 10_000_000;
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

  console.log(libPaths.join("\t"));

  for (let i = 0; i < 100; i++) {
    const keys = [...libs.keys()].sort(() => Math.random() - 0.5);
    const row = [];

    keys.forEach(i => {
      const start = Date.now();
      libs[i].deflate(rawData);
      const end = Date.now();
      row[i] = end - start;
    });
    console.log(row.join("\t"));
  }
})();

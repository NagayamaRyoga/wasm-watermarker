const Crypto = require('crypto');
const assert = require('assert');

const size = 1_000_000;
const rawData = Crypto.randomBytes(size);

(async function () {
  const libPaths = [
    `zlib-sample.wasm`,
    `zlib-funcord.wasm`,
    `zlib-opswap.wasm`,
    `zlib-funcord-opswap.wasm`,
  ];

  const libs = await Promise.all(libPaths.map(async path => await require('./zlib').initialize(`${__dirname}/${path}`)));

  const compressed = libs[0].deflate(rawData);

  // Validation
  libs.forEach((Zlib, i) => {
    assert.deepEqual(Zlib.deflate(rawData), compressed, `${libPaths[i]} - .deflate()`);
    assert.deepEqual(Zlib.inflate(compressed), rawData, `${libPaths[i]} - .inflate()`);
  });

  console.log(`raw size: ${rawData.length}bytes`);
  console.log(`deflated size: ${compressed.length}bytes`);
  console.log(`compression rate: ${(compressed.length / rawData.length * 100).toFixed(1)}%`)

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
}());


const Crypto = require('crypto');
const assert = require('assert');

const size = 1_000_000;
const rawData = Crypto.randomBytes(size);

(async function () {
  const libPaths = [
    './zlib-sample.wasm',
  ];

  const libs = await Promise.all(libPaths.map(async path => await require('./zlib').initialize(path)));

  // Validation
  libs.forEach((Zlib, i) => {
    assert.deepEqual(Zlib.inflate(Zlib.deflate(rawData)), rawData, libPaths[i]);
  });

  // Warming up
  libs.forEach((Zlib, i) => {
    console.time(`Warm - ${libPaths[i]}`);
    for (let i = 0; i < 5; i++) {
      Zlib.deflate(rawData);
    }
    console.timeEnd(`Warm - ${libPaths[i]}`);
  });

  // Benchmark
  libs.forEach((Zlib, i) => {
    console.time(`Benchmark - ${libPaths[i]}`);
    for (let i = 0; i < 100; i++) {
      Zlib.deflate(rawData);
    }
    console.timeEnd(`Benchmark - ${libPaths[i]}`);
  });
}());


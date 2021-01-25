(async function() {
    const Crypto = require('crypto');
    const assert = require('assert');
    const fs = require('fs').promises;

    // Generate random bytes
    const size = 2_000_000;
    const rawData = new Uint8Array(Crypto.randomBytes(size));

    // Initialize wasm-flate
    globalThis.self = globalThis;
    const wasm = await WebAssembly.compile(await fs.readFile(`${__dirname}/wasm-flate/wasm_flate_bg.wasm`));
    require(`${__dirname}/wasm-flate/wasm_flate.js`);
    const flate = globalThis.wasm_bindgen;
    await flate(wasm);

    // Check if wasm-flate work properly
    const compressed = flate.deflate_encode_raw(rawData);
    assert.deepEqual(flate.deflate_decode_raw(compressed), rawData);

    // Warm up
    flate.deflate_encode_raw(rawData);
    flate.deflate_encode_raw(rawData);
    flate.deflate_encode_raw(rawData);

    const count = 100;
    const start = Date.now();
    for (let i = 0; i < count; i++) {
        flate.deflate_encode_raw(rawData);
    }
    const end = Date.now();
    const duration_ms = end - start;

    console.log(`${__dirname}\t${duration_ms / count}`);
})();

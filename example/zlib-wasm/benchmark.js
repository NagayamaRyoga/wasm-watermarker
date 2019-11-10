(async function() {
  const Zlib = await require('./zlib').initialize('./zlib-sample.wasm');

  const rawData = Buffer.from([...Array(10000).keys()]);
  const compressedData = Zlib.deflate(rawData);
  const uncompressedData = Zlib.inflate(compressedData);

  console.log(uncompressedData.equals(rawData));
}());


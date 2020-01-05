require("./testutils");

const runTest = testname => {
  console.log(`Run test: ${testname}`);
  globalThis.Ammo = require("./ammo.js/builds/ammo.wasm"); // `Ammo` must set every time :(
  require(`./ammo.js/tests/${testname}`);
};

runTest("2");
runTest("3");
runTest("basics");

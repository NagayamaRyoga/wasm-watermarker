// ammo.js/tests/testutils.js
globalThis.assert = (x, msg) => {
  if (!x) throw "Assertion: " + (msg || "Failure") + " at " + new Error().stack;
};

globalThis.assertEq = (x, y, msg) =>
  assert(
    x === y,
    (msg ? msg + " : " : "") + x + " should be equal to " + y + "."
  );

globalThis.print = console.log;

// Run tests
const runTest = testname => {
  console.log(`Run test: ${testname}`);
  globalThis.Ammo = require("./ammo.js/builds/ammo.wasm"); // `Ammo` must set every time :(
  require(`./ammo.js/tests/${testname}`);
};

runTest("2");
runTest("3");
runTest("basics");

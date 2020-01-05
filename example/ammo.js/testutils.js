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

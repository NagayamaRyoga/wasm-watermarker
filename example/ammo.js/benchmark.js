require("./testutils");
const stress = require("./stress");

(async () => {
  const runBench = dir => {
    const Ammo = require(`./${dir}/ammo.wasm`);

    return new Promise(resolve => {
      console.log(`Run bench: ${dir}`);

      // Wrap `Ammo()` by real `Promise` object because it returns just Promise-like object
      // and awaiting it acts buggy.
      Ammo().then(Ammo => {
        stress(Ammo);
        resolve();
      });
    });
  };

  const wasms = [
    "ammo.js/builds",
    "ammo-1-0c9c85c7",
    "ammo-1-0d97880d",
    "ammo-1-60b725f1",
    "ammo-1-fe8191b3",
    "ammo-2-0c9c85c7",
    "ammo-2-0d97880d",
    "ammo-2-60b725f1",
    "ammo-2-fe8191b3",
    "ammo-3-0c9c85c7",
    "ammo-3-0d97880d",
    "ammo-3-60b725f1",
    "ammo-3-fe8191b3"
  ];

  for (wasm of wasms) {
    await runBench(wasm);
  }
})();

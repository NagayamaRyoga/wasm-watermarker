require("./testutils");
const stress = require("./stress");

(async () => {
  const runBench = dir => {
    const Ammo = require(`./${dir}/ammo.wasm`);

    return new Promise(resolve => {
      // Wrap `Ammo()` by real `Promise` object because it returns just Promise-like object
      // and awaiting it acts buggy.
      Ammo().then(Ammo => {
        const dur = stress(Ammo);
        resolve(dur);
      });
    });
  };

  const wasms = [
    "ammo.js/builds",
    "dist/ammo-1-0c9c85c7",
    "dist/ammo-1-0d97880d",
    "dist/ammo-1-60b725f1",
    "dist/ammo-1-fe8191b3",
    "dist/ammo-2-0c9c85c7",
    "dist/ammo-2-0d97880d",
    "dist/ammo-2-60b725f1",
    "dist/ammo-2-fe8191b3",
    "dist/ammo-3-0c9c85c7",
    "dist/ammo-3-0d97880d",
    "dist/ammo-3-60b725f1",
    "dist/ammo-3-fe8191b3"
  ];

  const row = [];
  const keys = [...wasms.keys()].sort(() => Math.random() - 0.5);
  for (const i of keys) {
    row[i] = await runBench(wasms[i]);
  }
  console.log(row.join("\t"));
})();

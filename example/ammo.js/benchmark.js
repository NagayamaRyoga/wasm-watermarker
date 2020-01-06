require("./testutils");
const stress = require("./stress");

const WARM = 3;
const N = 10 + WARM;

(async () => {
  const runBench = dir => {
    const Ammo = require(`./${dir}/ammo.wasm`);

    return new Promise(resolve => {
      console.error(`${dir}`);

      // Wrap `Ammo()` by real `Promise` object because it returns just Promise-like object
      // and awaiting it acts buggy.
      Ammo().then(Ammo => {
        const durs = [];
        for (let i = 0; i < N; i++) {
          const dur = stress(Ammo);
          durs.push(dur);
          console.error(dur);
        }
        resolve({ dir, durs });
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

  const results = [];
  for (wasm of wasms) {
    results.push(await runBench(wasm));
  }

  console.log(JSON.stringify(results));
})();

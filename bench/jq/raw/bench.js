const fs = require('fs');
const assert = require('assert');
const jq = require(`${__dirname}/jq-web/jq.wasm.min.js`);

const testData = fs.readFileSync(`${__dirname}/large-file.json`).toString();
const query = '.[5000].payload.commits | map(.message) | join("\\n")';

const expected = '"tdiary: fix nginx configuration (use try_files, alias, and named location)\\nfluentd-ui: fix nginx configuration (use try_files, alias, and named location)\\ngeminabox: fix nginx configuration (use try_files, alias, and named location)\\nMerge branch \'master\' into fix/nginx-configs\\nMerge branch \'master\' into fix/nginx-configs\\ndebian: updated README.md\\nMerge pull request #11 from minimum2scp/fix/nginx-configs\\n\\nFixed nginx configurations for some images (use try_files, alias, and named locattions)\\nMerge branch \'master\' into feature/es-kibana4"';

jq.onInitialized.addListener(() => {
    // Check if jq-web works properly
    assert.equal(jq.raw(testData, query), expected);
});

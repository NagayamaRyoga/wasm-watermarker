import * as util from "util";
import * as childProcess from "child_process";
import * as fs from "fs";
import * as express from "express";

const port = 3000;
const watermarker = `${__dirname}/../../build/src/kyut`;

const app = express();

const server = app.listen(port, () => {
    console.log(`server listening at port ${port}`);
});

app.get(
    "/wasm/add.wasm",
    async (req: express.Request, res: express.Response) => {
        const path = `${__dirname}/../wasm/add.wasm`;
        const watermark = req.query.v || "";

        res.writeHead(200, { "Content-Type": "application/wasm" });

        console.time("kyut");
        const process = childProcess.spawn(watermarker, [path, watermark, "-"]);
        process.stdout.pipe(res);
        res.on("finish", () => console.timeEnd("kyut"));
    }
);

app.use(express.static(`${__dirname}/../static`));

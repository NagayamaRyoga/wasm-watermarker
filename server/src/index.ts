import * as fs from "fs";
import * as express from "express";

const port = 3000;

const app = express();

const server = app.listen(port, () => {
    console.log(`server listening at port ${port}`);
});

app.get("/wasm/add.wasm", (req: express.Request, res: express.Response) => {
    const binary = fs.readFileSync(`${__dirname}/../wasm/add.wasm`);

    res.writeHead(200, { "Content-Type": "application/wasm" });
    res.end(binary);
});

app.use(express.static(`${__dirname}/../static`));

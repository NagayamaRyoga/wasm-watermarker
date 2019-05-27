import * as express from "express";

const port = 3000;

const app = express();

const server = app.listen(port, () => {
    console.log(`server listening at port ${port}`);
});

app.get("/", (req: express.Request, res: express.Response) => {
    res.send("Hello, world!");
});

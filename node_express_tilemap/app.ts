import express from 'express';
import cors from 'cors';
import routes from "./routes";

const app = express();

app.use(cors());
app.use(express.json());
app.use(express.static('public'));


const PORT = 3000;

//启动
app.listen(PORT, async () => {
    console.log(`App is running at http://127.0.0.1:${PORT}`);
    routes(app);
});

/**
 * https://juejin.cn/post/7069770431871320078
 */

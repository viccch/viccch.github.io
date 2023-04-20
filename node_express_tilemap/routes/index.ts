import { Express, Request, Response, Router } from 'express';
import TileMap from './tilemap.routes'

//路由配置接口
interface RouterConf {
    path: string,
    router: Router,
    // meta?: unknow
};

function routes(app: Express) {

    //根目录
    app.get('/', (req: Request, res: Response) => { res.location('/index.html') });

    routerConf.forEach((conf) => app.use(conf.path, conf.router))
};

//路由配置
const routerConf: Array<RouterConf> = [
    { path: '/tilemap', router: TileMap }
];

export default routes;
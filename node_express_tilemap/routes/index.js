"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const tilemap_routes_1 = __importDefault(require("./tilemap.routes"));
;
function routes(app) {
    //根目录
    app.get('/', (req, res) => { res.location('/index.html'); });
    routerConf.forEach((conf) => app.use(conf.path, conf.router));
}
;
//路由配置
const routerConf = [
    { path: '/tilemap', router: tilemap_routes_1.default }
];
exports.default = routes;

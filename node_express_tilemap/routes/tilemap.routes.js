"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = require("express");
const fs_1 = __importDefault(require("fs"));
const router = (0, express_1.Router)();
router.get('/:z/:x/:y', (req, res) => {
    let z = parseInt(req.params.z);
    let x = parseInt(req.params.x);
    let y = parseInt(req.params.y);
    // console.log("z=", z, "x=", x, "y=", y);
    if (isNaN(z) || isNaN(x) || isNaN(y)) {
        res.status(404).send('not found');
        return;
    }
    let path = `./tilemap/z${z}/z${z}x${x}y${y}.jpeg`;
    if (false == fs_1.default.existsSync(path)) {
        res.status(404).send("not found");
        return;
    }
    const rs = fs_1.default.createReadStream(path);
    rs.pipe(res);
    // const img = fs.readFileSync(path);
    // res.send(img);
});
router.get('/', (req, res) => {
    if (undefined == req.query.z
        || undefined == req.query.x
        || undefined == req.query.y) {
        res.status(404).send("not found");
        return;
    }
    let z = parseInt(req.query.z.toString());
    let x = parseInt(req.query.x.toString());
    let y = parseInt(req.query.y.toString());
    // console.log("z=", z, "x=", x, "y=", y);
    if (isNaN(z) || isNaN(x) || isNaN(y)) {
        res.status(404).send('not found');
        return;
    }
    let path = `./tilemap/z${z}/z${z}x${x}y${y}.jpeg`;
    if (false == fs_1.default.existsSync(path)) {
        res.status(404).send("not found");
        return;
    }
    const rs = fs_1.default.createReadStream(path);
    rs.pipe(res);
    // const img = fs.readFileSync(path);
    // res.send(img);
});
exports.default = router;

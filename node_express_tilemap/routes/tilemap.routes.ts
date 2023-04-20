import { Router} from "express";
import fs from 'fs';

const router = Router();

router.get('/:z/:x/:y',(req,res)=>{
    
    let z = parseInt(req.params.z);
    let x = parseInt(req.params.x);
    let y = parseInt(req.params.y);

    // console.log("z=", z, "x=", x, "y=", y);

    if (isNaN(z) || isNaN(x) || isNaN(y)) {
        res.status(404).send('not found');
        return;
    }

    let path = `./tilemap/z${z}/z${z}x${x}y${y}.jpeg`;

    if(false==fs.existsSync(path))
    {
        res.status(404).send("not found");
        return;
    }

    const rs = fs.createReadStream( path);
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

    if(false==fs.existsSync(path))
    {
        res.status(404).send("not found");
        return;
    }

    const rs = fs.createReadStream( path);
    rs.pipe(res);

    // const img = fs.readFileSync(path);
    // res.send(img);
});

export default router;
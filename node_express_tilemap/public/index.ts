window.onload = () => {
    const btn_get = <HTMLElement>document.querySelector("#btn_get");
    const btn_clear = <HTMLElement>document.querySelector("#btn_clear");
    const box = document.querySelector("#map_box") as HTMLElement;
    const img = document.createElement("img") as HTMLImageElement;
    img.setAttribute("id", "img");
    box.appendChild(img);
    btn_get.onclick = btn_get_click;
    btn_clear.onclick = btn_clear_click;
}

function btn_get_click() {
    let z = parseInt((document.querySelector("#z") as HTMLInputElement).value.toString());
    let x = parseInt((document.querySelector("#x") as HTMLInputElement).value.toString());
    let y = parseInt((document.querySelector("#y") as HTMLInputElement).value.toString());
    // console.log("sss", z, x, y);
    if (isNaN(z) || isNaN(x) || isNaN(y)) {
        alert("error:参数不是数字");
        return;
    }
    get_tile(z, x, y);
}

function btn_clear_click() {
    const img = document.querySelector("#img") as HTMLImageElement;
    img.src = "";
}

function get_tile(z: number, x: number, y: number) {
    let url = `http://127.0.0.1:3000/tilemap?z=${z}&x=${x}&y=${y}`;

    let img = (document.querySelector("#img") as HTMLImageElement);
    const myRequest = new Request(url);
    fetch(myRequest)
        .then(function (response) {
            return (response.ok) ? (response.blob()) : (null);
        })
        .then(function (myBlob) {
            if (null === myBlob)
                return;
            let objectURL = URL.createObjectURL(myBlob);
            img.src = objectURL;
        });
}

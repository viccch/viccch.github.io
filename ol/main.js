// import Map from 'ol/Map.js';
// import TileLayer from 'ol/layer/Tile.js';
// import View from 'ol/View.js';
// import XYZ from 'ol/source/XYZ.js';
// import { useGeographic } from 'ol/proj.js';


import Map from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/Map.js';
import TileLayer from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/layer/Tile.js';
import View from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/View.js';
import XYZ from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/source/XYZ.js';
import { useGeographic } from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/proj.js';


useGeographic();

const map = new Map({
  layers: [
    new TileLayer({
      source: new XYZ({
        // url:'http://t3.tianditu.com/DataServer?T=img_w&x={x}&y={y}&l={z}&tk=49ea1deec0ffd88ef13a3f69987e9a63'
        url: 'http://localhost:3000/tilemap?z={z}&x={x}&y={y}'
      }),
    }),
  ],
  target: 'map',
  view: new View({
    projection: 'EPSG:4326',
    center: [116.396982, 39.91808],
    // center: [0,0],
    zoom: 18,
    minZoom: 14,
    maxZoom: 18,
    extent: [116.335159, 39.952988, 116.45867, 39.866017]
  }),
});

document.getElementById('zoom-out').onclick = function () {
  const view = map.getView();
  const zoom = view.getZoom();
  if (zoom > view.getMinZoom)
    view.setZoom(zoom - 1);
};

document.getElementById('zoom-in').onclick = function () {
  const view = map.getView();
  const zoom = view.getZoom();
  if (zoom < view.getMaxZoom)
    view.setZoom(zoom + 1);
};



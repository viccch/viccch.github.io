// import Map from 'ol/Map.js';
// import TileLayer from 'ol/layer/Tile.js';
// import View from 'ol/View.js';
// import XYZ from 'ol/source/XYZ.js';


import Map from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/Map.js';
import TileLayer from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/layer/Tile.js';
import View from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/View.js';
import XYZ from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/source/XYZ.js';
// import { useGeographic } from 'https://cdnjs.cloudflare.com/ajax/libs/openlayers/7.3.0/proj.js';



const map = new Map({
  layers: [
    new TileLayer({
      source: new XYZ({
        url:'http://t3.tianditu.com/DataServer?T=img_w&x={x}&y={y}&l={z}&tk=49ea1deec0ffd88ef13a3f69987e9a63'
      }),
    }),
  ],
  target: 'map',
  view: new View({
    center: [0, 0],
    zoom: 2,
  }),
});

document.getElementById('zoom-out').onclick = function () {
  const view = map.getView();
  const zoom = view.getZoom();
  view.setZoom(zoom - 1);
};

document.getElementById('zoom-in').onclick = function () {
  const view = map.getView();
  const zoom = view.getZoom();
  view.setZoom(zoom + 1);
};

// t-storms.js — requires weather-shapes.js
// Animation: rain falls and every few seconds lightning double-flashes,
// lighting up the cloud.
function drawTStorms(x, y, size = 100, t = wxTime()) {
  const cycle = t % 3.2;
  const flash = cycle < 0.1 || (cycle > 0.2 && cycle < 0.3) ? 1 : 0;
  const cloudCol = lerpColor(color(WX.stormCloud), color(WX.stormFlash), flash * 0.7);

  wxBegin(x, y, size);
  wxFallingRain([-22, -8, 28], 2, 34, t, 1.8);
  wxCloud(0, -10, 76, cloudCol, WX.stormShade);
  wxBolt(3, 26, 32 * (1 + flash * 0.1), 0.45 + 0.55 * flash);
  wxEnd();
}

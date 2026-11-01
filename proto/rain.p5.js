// rain.js — requires weather-shapes.js
// Animation: steady rain falling from under the cloud.
function drawRain(x, y, size = 100, t = wxTime()) {
  wxBegin(x, y, size);
  wxFallingRain([-20, -6, 8, 22], 2, 36, t, 1.5, 12);
  wxCloud(0, -10, 76);
  wxEnd();
}

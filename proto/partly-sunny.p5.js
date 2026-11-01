// partly-sunny.js — requires weather-shapes.js
// Animation: sun rays turn while the cloud drifts back and forth.
function drawPartlySunny(x, y, size = 100, t = wxTime()) {
  wxBegin(x, y, size);
  wxSun(-12, -12, 16, 8, t * 0.3);
  wxCloud(6 + Math.sin(t * 0.7) * 4, 12, 66);
  wxEnd();
}

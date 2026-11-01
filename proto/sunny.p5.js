// sunny.js — requires weather-shapes.js
// Animation: rays turn slowly while the sun gently breathes.
function drawSunny(x, y, size = 100, t = wxTime()) {
  wxBegin(x, y, size);
  wxSun(0, 0, 22, 10, t * 0.25, 0.04 * Math.sin(t * 2));
  wxEnd();
}

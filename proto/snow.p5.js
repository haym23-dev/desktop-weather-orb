// snow.js — requires weather-shapes.js
// Animation: flakes drift down slowly, swaying and turning as they fall.
function drawSnow(x, y, size = 100, t = wxTime()) {
  wxBegin(x, y, size);
  wxFallingSnow([-18, 4, 24], 6, 36, t);
  wxCloud(0, -10, 76);
  wxEnd();
}

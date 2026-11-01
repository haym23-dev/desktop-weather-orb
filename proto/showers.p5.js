// showers.js — requires weather-shapes.js
// Animation: light rain falls from the cloud while the sun turns behind it.
function drawShowers(x, y, size = 100, t = wxTime()) {
  wxBegin(x, y, size);
  wxSun(-14, -18, 14, 8, t * 0.3);
  wxFallingRain([-8, 8, 24], 10, 30, t, 1.1);
  wxCloud(4 + Math.sin(t * 0.7) * 2, 0, 66);
  wxEnd();
}

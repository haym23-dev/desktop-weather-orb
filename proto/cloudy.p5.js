// cloudy.js — requires weather-shapes.js
// Animation: the two clouds drift past each other in opposite directions.
function drawCloudy(x, y, size = 100, t = wxTime()) {
  const drift = Math.sin(t * 0.6) * 4;
  wxBegin(x, y, size);
  wxCloud(-10 - drift, -6, 56, WX.cloudShade, WX.stormCloud);
  wxCloud(6 + drift, 10 + Math.sin(t * 1.1) * 1, 70);
  wxEnd();
}

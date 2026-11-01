// partly-cloudy-night.js — requires weather-shapes.js
// Animation: a star twinkles and the cloud drifts in front of the moon.
function drawPartlyCloudyNight(x, y, size = 100, t = wxTime()) {
  wxBegin(x, y, size);
  wxMoon(-12, -10 + Math.sin(t * 1.2) * 1, 22);
  wxTwinkle(18, -30, 5, t, 1);
  wxCloud(6 + Math.sin(t * 0.6) * 5, 14, 66);
  wxEnd();
}

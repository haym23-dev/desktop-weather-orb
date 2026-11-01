// clear-night.js — requires weather-shapes.js
// Animation: stars twinkle out of step while the moon floats gently.
function drawClearNight(x, y, size = 100, t = wxTime()) {
  wxBegin(x, y, size);
  wxMoon(-4, 4 + Math.sin(t * 1.2) * 1.5, 30);
  wxTwinkle(22, -22, 7, t, 0);
  wxTwinkle(34, 4, 4, t, 2.1);
  wxTwinkle(4, -36, 4, t, 4.2);
  wxEnd();
}

// fog.js — requires weather-shapes.js
// Animation: fog bands slide sideways and thin out at different rates.
function drawFog(x, y, size = 100, t = wxTime()) {
  const bands = [
    [-34, 26, -21],
    [-24, 36, -7],
    [-36, 22, 7],
    [-20, 34, 21],
  ];
  wxBegin(x, y, size);
  push();
  strokeWeight(7);
  strokeCap(ROUND);
  bands.forEach(([x1, x2, by], i) => {
    const shift = Math.sin(t * 0.8 + i * 1.3) * 6;
    stroke(wxFade(WX.fog, 0.6 + 0.4 * Math.sin(t * 1.2 + i * 2.1)));
    line(x1 + shift, by, x2 + shift, by);
  });
  pop();
  wxEnd();
}

// weather-shapes.js
// Shared building blocks for the weather icons (p5.js global mode).
// Every icon draws in a 100 x 100 local box centred on (0, 0),
// so coordinates below run roughly from -50 to 50.
// Animations are driven by a time value `t` in seconds.

const WX = {
  sun: '#FFB703',
  sunCore: '#FFC93C',
  cloud: '#F4F7FA',
  cloudShade: '#C3CDD7',
  stormCloud: '#8E9AA8',
  stormShade: '#5F6B78',
  stormFlash: '#D9E1EA',
  rain: '#4FB3FF',
  snow: '#DDF1FF',
  bolt: '#FFD60A',
  moon: '#F2E3A0',
  star: '#F2E3A0',
  fog: '#AEB8C2',
};

// Default clock for the icons: seconds since the sketch started.
function wxTime() {
  return millis() / 1000;
}

// A colour with an alpha from 0 to 1.
function wxFade(col, a) {
  const c = color(col);
  c.setAlpha(255 * constrain(a, 0, 1));
  return c;
}

// Move to (x, y) and scale so the icon is `size` pixels wide.
function wxBegin(x, y, size) {
  push();
  translate(x, y);
  scale(size / 100);
}

function wxEnd() {
  pop();
}

// Sun: a disc with triangular rays (same idea as drawSunRays).
// `spin` rotates the rays; `pulse` grows the core (0.05 = 5%).
function wxSun(cx, cy, r, rays = 10, spin = 0, pulse = 0) {
  push();
  translate(cx, cy);
  noStroke();
  fill(WX.sun);
  push();
  rotate(spin);
  const distance = r * 1.5;
  const t = r * 0.28;
  for (let i = 0; i < rays; i++) {
    const angle = map(i, 0, rays, 0, TWO_PI);
    push();
    translate(cos(angle) * distance, sin(angle) * distance);
    rotate(angle + HALF_PI);
    triangle(-t, t, t, t, 0, -t);
    pop();
  }
  pop();
  fill(WX.sunCore);
  circle(0, 0, r * 2 * (1 + pulse));
  pop();
}

// One cloud silhouette: a rounded base plus three puffs.
function wxCloudShape(w) {
  const h = w * 0.32;
  rectMode(CENTER);
  rect(0, h * 0.25, w, h, h / 2);
  circle(-w * 0.22, 0, w * 0.42);
  circle(w * 0.08, -w * 0.12, w * 0.56);
  circle(w * 0.3, h * 0.05, w * 0.36);
}

// Cloud with a soft offset shadow for depth.
function wxCloud(cx, cy, w, col = WX.cloud, shade = WX.cloudShade) {
  push();
  translate(cx, cy);
  noStroke();
  push();
  translate(w * 0.03, w * 0.04);
  fill(shade);
  wxCloudShape(w);
  pop();
  fill(col);
  wxCloudShape(w);
  pop();
}

// Rain streaks that fall from y0 down `fall` units on a loop,
// fading in and out. Draw before the cloud so they appear from under it.
function wxFallingRain(xs, y0, fall, t, speed = 1.4, len = 10) {
  push();
  strokeWeight(5);
  strokeCap(ROUND);
  xs.forEach((x, i) => {
    const p = (t * speed + i * 0.37) % 1;
    const y = y0 + p * fall;
    const drift = -p * fall * 0.3;
    stroke(wxFade(WX.rain, Math.sin(p * Math.PI) * 1.4));
    line(x + drift, y, x + drift - len * 0.35, y + len);
  });
  pop();
}

// Six-armed snowflake, optionally rotated and faded.
function wxSnowflake(cx, cy, r, spin = 0, alpha = 1) {
  push();
  translate(cx, cy);
  rotate(spin);
  stroke(wxFade(WX.snow, alpha));
  strokeWeight(3);
  strokeCap(ROUND);
  for (let i = 0; i < 3; i++) {
    const a = HALF_PI + (i * PI) / 3;
    line(-cos(a) * r, -sin(a) * r, cos(a) * r, sin(a) * r);
  }
  pop();
}

// Snowflakes that drift down on a loop, swaying and turning.
function wxFallingSnow(xs, y0, fall, t, speed = 0.45, r = 6) {
  xs.forEach((x, i) => {
    const p = (t * speed + i * 0.33) % 1;
    const sway = Math.sin(t * 2 + i * 1.7) * 4;
    wxSnowflake(x + sway, y0 + p * fall, r, t * 1.2 + i, Math.sin(p * Math.PI) * 1.5);
  });
}

// Lightning bolt, `s` units tall.
function wxBolt(cx, cy, s, alpha = 1) {
  push();
  translate(cx, cy);
  scale(s / 20);
  noStroke();
  fill(wxFade(WX.bolt, alpha));
  beginShape();
  vertex(2, -10);
  vertex(-6, 2);
  vertex(0, 2);
  vertex(-3, 10);
  vertex(7, -3);
  vertex(1, -3);
  vertex(4, -10);
  endShape(CLOSE);
  pop();
}

// Crescent moon built from two circle arcs, so it works on any background.
function wxMoon(cx, cy, R) {
  const r = R * 0.85;
  const ox = R * 0.55;
  const oy = -R * 0.35; // centre of the "cut-out" circle
  const d = Math.hypot(ox, oy);
  const phi = Math.atan2(oy, ox);
  const alpha = Math.acos((d * d + R * R - r * r) / (2 * d * R));
  const steps = 48;

  push();
  translate(cx, cy);
  noStroke();
  fill(WX.moon);
  beginShape();

  // Outer edge: the long way round, away from the cut-out.
  for (let i = 0; i <= steps; i++) {
    const a = phi + alpha + ((TWO_PI - 2 * alpha) * i) / steps;
    vertex(R * Math.cos(a), R * Math.sin(a));
  }

  // Inner edge: along the cut-out circle, back to the start.
  const b1 = Math.atan2(R * Math.sin(phi - alpha) - oy, R * Math.cos(phi - alpha) - ox);
  const b2 = Math.atan2(R * Math.sin(phi + alpha) - oy, R * Math.cos(phi + alpha) - ox);
  let span = (((b2 - b1) % TWO_PI) + TWO_PI) % TWO_PI;
  const mid = b1 + span / 2;
  if (Math.hypot(ox + r * Math.cos(mid), oy + r * Math.sin(mid)) > R) {
    span -= TWO_PI; // the other direction stays inside the moon
  }
  for (let i = 1; i < steps; i++) {
    const a = b1 + (span * i) / steps;
    vertex(ox + r * Math.cos(a), oy + r * Math.sin(a));
  }

  endShape(CLOSE);
  pop();
}

// Four-pointed star.
function wxStar(cx, cy, s, alpha = 1) {
  push();
  translate(cx, cy);
  noStroke();
  fill(wxFade(WX.star, alpha));
  beginShape();
  vertex(0, -s);
  vertex(s * 0.25, -s * 0.25);
  vertex(s, 0);
  vertex(s * 0.25, s * 0.25);
  vertex(0, s);
  vertex(-s * 0.25, s * 0.25);
  vertex(-s, 0);
  vertex(-s * 0.25, -s * 0.25);
  endShape(CLOSE);
  pop();
}

// Star that twinkles: grows, shrinks and brightens on its own rhythm.
function wxTwinkle(cx, cy, s, t, phase = 0) {
  const k = 0.5 + 0.5 * Math.sin(t * 2.6 + phase);
  wxStar(cx, cy, s * (0.65 + 0.35 * k), 0.55 + 0.45 * k);
}

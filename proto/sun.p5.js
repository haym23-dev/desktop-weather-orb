/**
 * Sun drawing - For use on p5js.org
 */

function setup() {
  createCanvas(360, 360);
}

function draw() {
  // Black background
  background(0xFFFF);

  let cx = 179.5;
  let cy = 179.5
  let r = 40;
  drawSun(cx, cy, r);
}

function drawSun(centerX, centerY, radius) {
  noStroke();
  fill("#b4481a");
  circle(centerX, centerY, radius);
  
  let numberOfTriangles = 10;
  let distance = 140;
  let triangleSize = 20;
  drawSunRays(centerX, centerY, numberOfTriangles, distance, triangleSize);
}

function drawSunRays(centerX, centerY, numberOfTriangles, distance, triangleSize) {
  for (let i = 0; i < numberOfTriangles; i++) {
    let angle = map(i, 0, numberOfTriangles, 0, TWO_PI);
    let x = centerX + cos(angle) * distance;
    let y = centerY + sin(angle) * distance;
    
    push();
    translate(x, y);
    rotate(angle + HALF_PI);
    triangle(-triangleSize, triangleSize, triangleSize, triangleSize, 0, -triangleSize);
    pop();
  }
}

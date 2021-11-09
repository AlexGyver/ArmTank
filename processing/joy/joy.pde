int cx = 180, cy = 120;
int R = 100;
int vx = 0, vy = 0;
int vr, vl;
float tx, ty;
float tangle;
PGraphics tank;

void setup() {
  size(700, 700);
  strokeWeight(3);
  tank = createGraphics(60, 60);
  tx = width / 2;
  ty = height / 2;
}

void draw() {
  background(255);
  getJoy();
  drawJoy();
  drawTank();
}

void getJoy() {
  if (dist(cx, cy, mouseX, mouseY) <= R) {
    vx = mouseX - cx;
    vy = cy - mouseY;
  } else vx = vy = 0;
  //vx = constrain(vx, -R, R);
  //vy = constrain(vy, -R, R);
  /*
  float angle;
   if (vx != 0) angle = atan((float)abs(vy)/abs(vx));
   else angle = HALF_PI;
   int maxX = int(R * cos(angle));
   int maxY = int(R * sin(angle));
   vx = constrain(vx, -maxX, maxX);
   vy = constrain(vy, -maxY, maxY);
   */
}

void drawJoy() {
  noFill();
  line(cx-R, cy, cx+R, cy);
  line(cx, cy-R, cx, cy+R);
  circle(cx, cy, R*2);
  rectMode(CENTER);
  rect(cx, cy, R*2, R*2);
  fill(0);
  circle(cx+vx, cy-vy, 30);

  rectMode(CORNER);
  fill(0);
  vr = (vy + vx);
  vl = (vy - vx);
  vr = constrain (vr, -R, R);
  vl = constrain (vl, -R, R);
  rect(cx + R + 30, cy, 30, -vl);
  rect(cx - R - 60, cy, 30, -vr);
}



void drawTank() {
  tangle += (vr-vl) / 5000.0;
  float v = (vr+vl) / 100.0;
  tx += v * cos(tangle);
  ty += v * sin(tangle);
  
  imageMode(CENTER);
  translate(tx, ty);
  rotate(tangle);
  tank.beginDraw();
  tank.background(255);
  tank.rectMode(CENTER);
  tank.noStroke();
  tank.fill(#00ACE8);
  tank.rect(30, 30, 40, 30);
  tank.fill(0);
  tank.rect(40, 30, 5, 20);
  tank.rect(30, 6, 50, 12);
  tank.rect(30, 60-6, 50, 12);
  tank.endDraw();
  image(tank, 0, 0);
}

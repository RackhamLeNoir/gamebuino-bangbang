#include "bullet.h"

Bullet::Bullet()
{
  
}

void Bullet::shoot(Cannon *cannon)
{
  _c = cannon;
  _x = _c->getX();
  _y = trajectory(_x);
}

bool Bullet::move()
{
  _x++;
  if (_x >= LCDWIDTH)
    return false;
  _y = trajectory(_x);
  return true;
}

bool Bullet::onCannon(const Cannon &c) const
{
  int dx = (int)c.getX() - (int)_x;
  int dy = (int)c.getY() - (int)_y;
  return dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2;
}

void Bullet::draw(Gamebuino &gb) const
{
  gb.display.setColor(BLACK);
  //debug trajectoire
  /*for (uint8_t i = _c->getX() ; i < LCDWIDTH; i++)
  {
    int py = constrain(LCDHEIGHT - trajectory(i), 0, LCDHEIGHT - 1);
    gb.display.drawPixel(i, py);
  }*/
  //-----------------

  if (_x < LCDWIDTH && _y > 0 && _y < LCDHEIGHT)
    gb.display.fillRect(_x, LCDHEIGHT - _y, 2, 2);
}

long int Bullet::trajectory(uint8_t posx) const
{
  float x = posx - _c->getX();
  float t = tan(_c->getAngle() * PI / 180.0);
  float f = 5. * (float)_c->getForce() * (float)_c->getForce() / 100.;
  return t * x - (1 + t*t)*x*x / f + _c->getY() + 2;
}


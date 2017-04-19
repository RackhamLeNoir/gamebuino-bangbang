#ifndef __BULLET__
#define __BULLET__

#include <Gamebuino.h>

#include "Cannon.h"

class Bullet
{
  public:
    Bullet();

    void shoot(Cannon *cannon);

    bool move();

    bool onCannon(const Cannon &c) const;

    void draw(Gamebuino &gb) const;

    unsigned int getX() { return _x; }
    long int getY() { return _y; }

    void setY(long int y) { _y = y; }
    
  private:
    long int trajectory(uint8_t posx) const;
    unsigned int _x;
    long int _y;
    Cannon *_c;
};

#endif


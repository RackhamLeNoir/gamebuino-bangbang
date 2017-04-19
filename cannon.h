#ifndef __CANNON__
#define __CANNON__

#include <Gamebuino.h>

class Cannon
{
  public:

    Cannon(unsigned int x, unsigned int y, uint8_t angle, float force, bool dir_right);

    void draw(const Gamebuino &gb, bool aiming) const;

    void move(uint8_t ypos);

    uint8_t getAngle() { return _angle; }
    uint8_t getForce() { return _force; }
    uint8_t getX() { return _x; }
    uint8_t getY() { return _y; }
    bool getDirRight() { return _dir_right; }

    void up();
    void down();
    void longer();
    void shorter();
    
  private:
    unsigned int _x;
    unsigned int _y;
    uint8_t _angle;
    uint8_t _force;  
    bool _dir_right;

};

#endif


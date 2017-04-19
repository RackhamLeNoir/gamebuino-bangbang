#ifndef __TERRAIN__
#define __TERRAIN__

#include <Gamebuino.h>

#include "bullet.h"

class Terrain
{
  public:
    Terrain();

    void generate();

    uint8_t get(uint8_t pos) const;

    void draw(const Gamebuino &gb) const;

    bool collision(Bullet &b);

  private:
    void interpolate_terrain(uint8_t min, uint8_t max);
  
    uint8_t terrain[LCDWIDTH];
};

#endif


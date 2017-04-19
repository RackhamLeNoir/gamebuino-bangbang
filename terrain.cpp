#include "terrain.h"

Terrain::Terrain()
{
  memset(terrain, 0, LCDWIDTH * sizeof(uint8_t));
}

void Terrain::generate()
{
  terrain[0] = random(5, 20);
  terrain[LCDWIDTH - 1] = random(5, 20);
  interpolate_terrain(0, LCDWIDTH - 1);
  uint8_t s = 2;
  float d = 20.0, r = 0.2;
  while (s < LCDWIDTH - 1)
  {
    uint8_t segmentsize = LCDWIDTH / s;
    for (uint8_t i = 0 ; i < s - 1 ; i++)
    {
        terrain[segmentsize * (i + 1)] = constrain(terrain[segmentsize * (i + 1)] + random(-(int)d, (int)d), 0, LCDHEIGHT - 1);
        interpolate_terrain(segmentsize * i, segmentsize * (i + 1));
        d *= pow(2, -r);
    }
    interpolate_terrain(LCDWIDTH - segmentsize, LCDWIDTH - 1);
    s += 1;
  }
}

void Terrain::interpolate_terrain(uint8_t min, uint8_t max)
{
  int diff = terrain[max] - terrain[min];
  int l = max - min;
  for (int i = min ; i < max ; i++)
  {
    terrain[i] = terrain[min] + diff * (i - min) / l;
  }
}

uint8_t Terrain::get(uint8_t pos) const
{
  return terrain[pos];
}

void Terrain::draw(const Gamebuino &gb) const
{
  gb.display.setColor(BLACK);
  for (uint8_t i = 0 ; i < LCDWIDTH ; i++)
    gb.display.drawFastVLine(i, LCDHEIGHT - terrain[i], terrain[i]);
}

bool Terrain::collision(Bullet &b)
{
  uint8_t bx = (uint8_t)constrain(b.getX(), 0, LCDWIDTH - 1);
  uint8_t by = (uint8_t)constrain(b.getY(), 0, LCDHEIGHT - 1);
  if (by <= terrain[bx])
  {
    terrain[bx - 1] = constrain(terrain[bx - 1] - 1, 0, LCDHEIGHT);
    terrain[bx] = constrain(terrain[bx] - 2, 0, LCDHEIGHT);
    if (bx < LCDWIDTH - 1)
      terrain[bx + 1] = constrain(terrain[bx + 1] - 1, 0, LCDHEIGHT);
    b.setY(terrain[bx]);
    return true;
  }
  return false;
}


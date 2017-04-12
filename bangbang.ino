#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

extern const byte font3x5[];
extern const byte font5x7[];

#define PLAYER1AIMING   0
#define PLAYER1SHOOTING 1
#define PLAYER2AIMING   2
#define PLAYER2SHOOTING 3
#define END             4

uint8_t gamestate = 0;
uint8_t bulletpos = 0;

uint8_t terrain[LCDWIDTH];

struct cannon_t
{
  unsigned int x;
  unsigned int y;
  uint8_t angle;
  float force;  
};

cannon_t cannon1, cannon2;

float rad_of_deg(uint8_t angle)
{
  return angle * PI / 180.0;
}

void interpolate_terrain(uint8_t min, uint8_t max)
{
  int diff = terrain[max] - terrain[min];
  int l = max - min;
  for (int i = min ; i < max ; i++)
  {
    terrain[i] = terrain[min] + diff * (i - min) / l;
  }
}

void midpoint_terrain()
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

void init_game()
{
  gamestate = 0;

  midpoint_terrain();

  cannon1 = { 10, terrain[10] + 1, 45, 2 };
  cannon2 = { LCDWIDTH - 10, terrain[LCDWIDTH - 10] + 1, 45, 3 };
}

void setup()
{
  gb.begin();
  gb.titleScreen(F("Bang! Bang!"));
  gb.pickRandomSeed();
  gb.battery.show = false;

  init_game();
}

float trajectoire(float x, cannon_t c)
{
  float t = tan(c.angle * PI / 180.0);
  return x * t - x*x*(1 + t*t)/(5 * c.force * c.force);
}

void drawgame()
{
  //terrain
  for (int i = 0 ; i < LCDWIDTH ; i++)
    gb.display.drawFastVLine(i, LCDHEIGHT - terrain[i], terrain[i]);

  //cannon 1
  gb.display.drawCircle(cannon1.x, LCDHEIGHT - (cannon1.y + 2), 2);
  gb.display.drawLine(
    cannon1.x, 
    LCDHEIGHT - (cannon1.y + 2), 
    cannon1.x + 4 * cos(rad_of_deg(cannon1.angle)), 
    LCDHEIGHT - (cannon1.y + 2 + 4 * sin(rad_of_deg(cannon1.angle))));

  //cannon 2
  gb.display.drawCircle(cannon2.x, LCDHEIGHT - (cannon2.y + 2), 2);
  gb.display.drawLine(
    cannon2.x, 
    LCDHEIGHT - (cannon2.y + 2), 
    cannon2.x - 4 * cos(rad_of_deg(cannon2.angle)), 
    LCDHEIGHT - (cannon2.y + 2 + 4 * sin(rad_of_deg(cannon2.angle))));

  //cursor under the aiming cannon
  gb.display.setColor(GRAY);
  if (gamestate == PLAYER1AIMING)
    gb.display.drawTriangle(
      cannon1.x - 1, LCDHEIGHT - (cannon1.y - 3),
      cannon1.x, LCDHEIGHT - (cannon1.y - 2),
      cannon1.x + 1, LCDHEIGHT - (cannon1.y - 3));
  else if (gamestate == PLAYER2AIMING)
    gb.display.drawTriangle(
      cannon2.x - 1, LCDHEIGHT - (cannon2.y - 3),
      cannon2.x, LCDHEIGHT - (cannon2.y - 2),
      cannon2.x + 1, LCDHEIGHT - (cannon2.y - 3));

  gb.display.setColor(BLACK);
  //debug trajectoire
  for (int i = 0 ; i < LCDWIDTH - cannon1.x; i++)
  {
    int py = constrain(LCDHEIGHT - (trajectoire(i, cannon1) + cannon1.y + 2), 0, LCDHEIGHT);
    gb.display.drawPixel(cannon1.x + i, py);
  }

  //bullet
  if (gamestate == PLAYER1SHOOTING)
    gb.display.fillRect(cannon1.x + bulletpos, LCDHEIGHT - (cannon1.y + 2 + trajectoire(bulletpos, cannon1)), 2, 2);
  else if (gamestate == PLAYER2SHOOTING)
    gb.display.fillRect(cannon2.x - bulletpos, LCDHEIGHT - (cannon2.y + 2 + trajectoire(bulletpos, cannon2)), 2, 2);
}

void drawwin()
{
  gb.display.setFont(font5x7);
  gb.display.cursorX = (LCDWIDTH - 6 * 8) / 2;
  gb.display.cursorY = 12;
  gb.display.println("You win!");

  gb.display.setFont(font3x5);
  gb.display.cursorX = (LCDWIDTH - 4 * 19) / 2;
  gb.display.cursorY = 35;
  gb.display.println("Press \25 to continue");
}

void inputsgame()
{
  if(gb.buttons.pressed(BTN_A))
  {
    if (gamestate == PLAYER1AIMING)
    {
      gamestate = PLAYER1SHOOTING;
      bulletpos = 0;
    }
  }
  if(gb.buttons.pressed(BTN_C))
    gb.titleScreen(F("Taquin"));

  if (gb.buttons.repeat(BTN_UP, 1))
    cannon1.angle = constrain(cannon1.angle + 1, 0, 89);
  else if (gb.buttons.repeat(BTN_DOWN, 1))
    cannon1.angle = constrain(cannon1.angle - 1, 0, 89);
  if (gb.buttons.repeat(BTN_LEFT, 1))
    cannon1.force = constrain(cannon1.force - 0.2, 2, 20);
  else if (gb.buttons.repeat(BTN_RIGHT, 1))
    cannon1.force = constrain(cannon1.force + 0.2, 2, 20);
}

void inputswin()
{
  if(gb.buttons.pressed(BTN_A))
    init_game();
  if(gb.buttons.pressed(BTN_C))
    gb.titleScreen(F("Taquin"));
}

void updategame()
{
  if (gamestate == PLAYER1SHOOTING)
  {
    bulletpos++;
    uint8_t bullety = cannon1.y + (uint8_t)trajectoire(bulletpos, cannon1);
    uint8_t terrainy = terrain[bulletpos + cannon1.x];
    if (cannon1.x + bulletpos > LCDWIDTH || bullety < 0)
      gamestate == PLAYER1AIMING; //TEST
    else if (bullety < terrainy)
    {
      int dx = (int)(cannon1.x + bulletpos) - (int)cannon2.x;
      int dy = (int)bullety - (int)cannon2.y;
      if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2)
      {
        gamestate = END;
        return;
      }
      gamestate = PLAYER1AIMING;
      terrain[bulletpos + cannon1.x - 1] = min(terrain[bulletpos + cannon1.x - 1], terrainy - 1);
      if (bulletpos - 1 < LCDWIDTH)
        terrain[bulletpos + cannon1.x + 1] = min(terrain[bulletpos + cannon1.x + 1], terrainy - 1);
      terrain[bulletpos + cannon1.x] -= 2;
    }
    cannon1.y = terrain[10] + 1;
    cannon2.y = terrain[LCDWIDTH - 10];
  }
}

void loop(){
  if(gb.update())
  {
    if (gamestate == END)
    {
      inputswin();
      drawwin();
    }
    else
    {
      updategame();
      inputsgame();
      drawgame();
    }
  }
}


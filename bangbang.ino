#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

extern const byte font3x5[];

struct cannon_t
{
  unsigned int x;
  unsigned int y;
  bool dir_r;
  uint8_t angle;
  float force;  
};

cannon_t cannon;

void init_game()
{
  cannon = { 10, 5, true, 45, 2 };
}

void setup()
{
  gb.begin();
  gb.titleScreen(F("Bang! Bang!"));
  gb.pickRandomSeed();
  gb.battery.show = false;

  init_game();
}

float trajectoire(float x)
{
  float t = tan(cannon.angle * PI / 180.0);
  return x * t - x*x*(1 + t*t)/(5 * cannon.force * cannon.force);
}

void drawgame()
{
  //terrain
  gb.display.fillRect(0, LCDHEIGHT - cannon.y, LCDWIDTH, cannon.y);

  //cannon
  gb.display.drawCircle(cannon.x, LCDHEIGHT - (cannon.y + 2), 2);
  gb.display.drawLine(cannon.x, LCDHEIGHT - (cannon.y + 2), cannon.x + 4 * cos(cannon.angle * PI / 180.0), LCDHEIGHT - (cannon.y + 2 + 4 * sin(cannon.angle * PI / 180.0)));

  //debug trajectoire
  for (int i = 0 ; i < LCDWIDTH - cannon.x; i++)
  {
    int py = constrain(LCDHEIGHT - (trajectoire(i) + cannon.y + 2), 0, LCDHEIGHT);
    gb.display.drawPixel(cannon.x + i, py);
  }

  //debug info
  gb.display.setFont(font3x5);
  gb.display.println(cannon.angle);
  gb.display.println(cannon.force);
}

void drawwin()
{
}

void inputsgame()
{
/*  if(gb.buttons.pressed(BTN_1))
    fire();*/
  if(gb.buttons.pressed(BTN_C))
    gb.titleScreen(F("Taquin"));

  if (gb.buttons.repeat(BTN_UP, 1))
    cannon.angle = constrain(cannon.angle + 1, 0, 89);
  else if (gb.buttons.repeat(BTN_DOWN, 1))
    cannon.angle = constrain(cannon.angle - 1, 0, 89);
  if (gb.buttons.repeat(BTN_LEFT, 1))
    cannon.force = constrain(cannon.force - 0.2, 2, 20);
  else if (gb.buttons.repeat(BTN_RIGHT, 1))
    cannon.force = constrain(cannon.force + 0.2, 2, 20);
}

void inputswin()
{
  if(gb.buttons.pressed(BTN_A))
    init_game();
  if(gb.buttons.pressed(BTN_C))
    gb.titleScreen(F("Taquin"));
}

bool gamewon()
{
  return false;
}

void loop(){
  if(gb.update())
  {
    if (gamewon())
    {
      inputswin();
      drawwin();
    }
    else
    {
      inputsgame();
      drawgame();
    }
  }
}


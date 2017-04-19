#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

#include "bullet.h"
#include "cannon.h"
#include "terrain.h"

extern const byte font3x5[];
extern const byte font5x7[];

#define PLAYER1AIMING   0
#define PLAYER1SHOOTING 1
#define PLAYER2AIMING   2
#define PLAYER2SHOOTING 3
#define END             4

uint8_t gamestate = 0;

Cannon cannon1(10, 0, 45, 35, true);
Cannon cannon2(LCDWIDTH - 10, 0, 45, 35, false);
Terrain terrain;
Bullet bullet;

void init_game()
{
  gamestate = 0;

  terrain.generate();

  cannon1.move(terrain.get(10) + 1);
  cannon2.move(terrain.get(LCDWIDTH - 10) + 1);
}

void setup()
{
  gb.begin();
  gb.titleScreen(F("Bang! Bang!"));
  gb.pickRandomSeed();
  gb.battery.show = false;

  init_game();
}

void drawgame()
{
  terrain.draw(gb);
  cannon1.draw(gb, gamestate == PLAYER1AIMING);
  cannon2.draw(gb, gamestate == PLAYER2AIMING);

  //if (gamestate == PLAYER1SHOOTING || gamestate == PLAYER2SHOOTING)
    bullet.draw(gb);
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
{/*
  if(gb.buttons.pressed(BTN_A))
  {
    if (gamestate == PLAYER1AIMING)
    {
      gamestate = PLAYER1SHOOTING;
    }
  }*/
  if(gb.buttons.pressed(BTN_C))
    gb.titleScreen(F("Taquin"));

  if (gamestate == PLAYER1AIMING)
  {
    if(gb.buttons.pressed(BTN_A))
    {
      gamestate = PLAYER1SHOOTING;
      bullet.shoot(&cannon1);
    }

    if (gb.buttons.repeat(BTN_UP, 1))
      cannon1.up();
    else if (gb.buttons.repeat(BTN_DOWN, 1))
      cannon1.down();
    if (gb.buttons.repeat(BTN_LEFT, 1))
      cannon1.shorter();
    else if (gb.buttons.repeat(BTN_RIGHT, 1))
      cannon1.longer();
  }
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
    //advance bullet
    //if it is outside of the game area
    //then switch to next state
    if (!bullet.move())
    {
      gamestate = PLAYER1AIMING; //TEST
      return;
    }
    uint8_t ty = terrain.get(bullet.getX());
    if (ty < 0 && bullet.getY() <= 0)
    {
      gamestate = PLAYER1AIMING; //TEST
      return;
    }
    else if (bullet.getY() < ty)
    {
      terrain.collision(bullet);
      cannon1.move(terrain.get(10) + 1);
      cannon2.move(terrain.get(LCDWIDTH - 10) + 1);
      if (cannon2.getY() < 0 || bullet.onCannon(cannon2))
      {
        gamestate = END;
        return;
      }
      gamestate = PLAYER1AIMING;
    }
/*
    bulletpos++;
    int bullety = constrain((int)cannon1.y + 2 + (int)trajectoire(bulletpos, cannon1), 0, LCDHEIGHT);
    uint8_t terrainy = terrain[bulletpos + cannon1.x];
    if (cannon1.x + bulletpos >= LCDWIDTH || bullety == 0)
      gamestate = PLAYER1AIMING; //TEST
    else if (bullety < terrainy)
    {
      int dx = (int)(cannon1.x + bulletpos) - (int)cannon2.x;
      int dy = (int)terrainy - (int)cannon2.y;
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
    cannon2.y = terrain[LCDWIDTH - 10];*/
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


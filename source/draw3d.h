#ifndef DRAW3D_H_ /* Include guard */
#define DRAW3D_H_

#include <NEMain.h>

enum DirectionEnum
{
    EAST = 0,
    WEST = 1,
    NORTH = 2,
    SOUTH = 3,
    UP = 5,
    DOWN = 4
};

extern int playerX;
extern int playerXFixed;
extern int playerY;
extern int playerZ;
extern int playerZ;
extern int playerZFixed;

void Draw3DScene(void);
void drawEastFace();
void drawWestFace();
void drawNorthFace();
void drawSouthFace();
void drawUpFace();
void drawDownFace();
void drawUpFace2(int coef);
#endif // DRAW3D_H_
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "main.h"
#include "draw3d.h"

int maxCpu = -10;
int playerX;
int playerXFixed;
int playerY;
int playerZ;
int playerZ;
int playerZFixed;

void Draw3DScene(void)
{
	NE_CameraUse(Camera);

	// This set material's color to drawing color (default = white)
	NE_MaterialUse(Texture);

	int xSize = 1;
	int ySize = 16;

	// glRotateX(0);
	//  Begin drawing

	// glRotateY(rotation);
	// glScalef(0.1,.)
	// glTranslatef32(floattof32(-playerPositionF.x), floattof32(-playerPositionF.y), floattof32(-playerPositionF.z));
	// glTranslatef32(floattof32(-CHUNCK_X_SIZE * 1.1f), floattof32(0), floattof32(CHUNCK_Z_SIZE));
	glMatrixMode(GL_POSITION);
	playerX = playerPosition.x / 4096;
	playerXFixed = (playerPosition.x - floattof32(0.5)) / 4096;
	playerY = playerPosition.y / 4096;
	playerZ = playerPosition.z / 4096;
	playerZFixed = (playerPosition.z - floattof32(0.5)) / 4096;
	NE_PolyBegin(GL_QUAD);
	// for (int x = 0; x < CHUNCK_X_SIZE; x++)
	// {
	// 	for (int y = 0; y < CHUNCK_Y_SIZE; y++)
	// 	{
	// 		for (int z = 0; z < CHUNCK_Z_SIZE; z++)
	// 		{
	// 			if (chunk[x][y][z] != 1)
	// 			{
	// 				if (z <= playerZFixed)
	// 				{
	// 					if (z == CHUNCK_Z_SIZE - 1 || chunk[x][y][z + 1] == 1)
	// 						// East
	// 						drawEastFace();
	// 				}
	// 				else
	// 				{
	// 					if (z == 0 || chunk[x][y][z - 1] == 1)
	// 						// West
	// 						drawWestFace();
	// 				}

	// 				if (x <= playerXFixed)
	// 				{
	// 					if (x == CHUNCK_X_SIZE - 1 || chunk[x + 1][y][z] == 1)
	// 						// North
	// 						drawNorthFace();
	// 				}
	// 				else
	// 				{
	// 					if (x == 0 || chunk[x - 1][y][z] == 1)
	// 						// South
	// 						drawSouthFace();
	// 				}

	// 				if (y <= playerY)
	// 				{
	// 					if (y == CHUNCK_Y_SIZE - 1 || chunk[x][y + 1][z] == 1)
	// 						// Up
	// 						drawUpFace();
	// 				}
	// 				else
	// 				{
	// 					if (y == 0 || chunk[x][y - 1][z] == 1)
	// 						// Down
	// 						drawDownFace();
	// 				}
	// 			}
	// 			glTranslatef32(floattof32(0), floattof32(0), floattof32(2));
	// 		}
	// 		glTranslatef32(floattof32(0), floattof32(2), floattof32(-CHUNCK_Z_SIZE * 2));
	// 	}
	// 	glTranslatef32(floattof32(2), floattof32(-CHUNCK_Y_SIZE * 2), floattof32(0));
	// }

	// glTranslatef32(floattof32(10), floattof32(0), floattof32(10));
	drawUpFace2(8);

	for (int i = 0; i < greedyCount; i++)
	{
		continue;
		glPushMatrix();
		glTranslatef32(inttov16(greedyChunk[i].position.x), inttov16(greedyChunk[i].position.y), inttov16(greedyChunk[i].position.z));
		glScalef32(inttov16(greedyChunk[i].scale.x), inttov16(greedyChunk[i].scale.y), inttov16(greedyChunk[i].scale.z));

		switch (greedyChunk[i].direction)
		{
		case EAST:
			drawEastFace();
			break;

		case WEST:
			drawWestFace();
			break;

		case NORTH:
			drawNorthFace();
			break;

		case SOUTH:
			drawSouthFace();
			break;

		case UP:
			drawUpFace();
			break;

		case DOWN:
			drawDownFace();
			break;
		}
		glPopMatrix(1);
	}
	NE_PolyEnd();
	// Set view in 2D mode
	NE_2DViewInit();

	if (playerX != LastPlayerChunckCollisionPosition.x || playerY != LastPlayerChunckCollisionPosition.y || playerZ != LastPlayerChunckCollisionPosition.z)
	{
		LastPlayerChunckCollisionPosition.x = playerX;
		LastPlayerChunckCollisionPosition.y = playerY;
		LastPlayerChunckCollisionPosition.z = playerZ;
		generateCollisions();
	}

	if (maxCpu >= 0)
	{
		if (maxCpu < NE_GetCPUPercent())
			maxCpu = NE_GetCPUPercent();
	}
	else
	{
		maxCpu++;
	}

	// DEBUG show CPU usage
	char CPU[40];
	sprintf(CPU, "CPU: %d%%, Mem: %d%% poly: %d", NE_GetCPUPercent(), NE_TextureFreeMemPercent(), NE_GetPolygonCount());
	//   sprintf(CPU, "CPU: %d%%, MCPU: %d%%", NE_GetCPUPercent(), maxCpu);
	//       sprintf(CPU, "CPU: %d%%, Mem: %d%% mem: %d", NE_GetCPUPercent(), NE_TextureFreeMemPercent(), NE_TextureFreeMem());
	//   sprintf(CPU, "%d %d %d", playerModel->x, playerModel->y, playerModel->z);
	// sprintf(CPU, "%f %f %f", playerModel->x / 4096.0f, playerModel->y / 4096.0f, playerModel->z / 4096.0f);

	NE_TextPrint(0,		   // Font slot
				 1, 1,	   // Coordinates x(column), y(row)
				 NE_White, // Color
				 CPU);

	// DEBUG show CPU usage
	// char posDebug[40];
	// sprintf(posDebug, "%f %f %f", lookRotation.x, lookRotation.y, lookRotation.z);
	// NE_TextPrint(0,		   // Font slot
	// 			 1, 2,	   // Coordinates x(column), y(row)
	// 			 NE_White, // Color
	// 			 posDebug);

	NE_2DDrawQuad(256 / 2 - 2, 192 / 2 - 2, 256 / 2 + 2, 192 / 2 + 2, 3, RGB15(0, 0, 0)); // Background
}

void drawEastFace()
{
	u32 LightIntensity = RGB15(20, 20, 20);

	NE_PolyColor(LightIntensity); // Set next vertices color
	GFX_TEX_COORD = allBlocks[0].faces.eastUV.uv0;
	// NE_PolyVertex(-1, 1, 1); // Send new vertex
	glVertex3v16(inttov16(-1), inttov16(1), inttov16(1));

	GFX_TEX_COORD = allBlocks[0].faces.eastUV.uv1;
	// NE_PolyVertex(-1, -1, 1);
	glVertex3v16(inttov16(-1), inttov16(-1), inttov16(1));

	GFX_TEX_COORD = allBlocks[0].faces.eastUV.uv2;
	// NE_PolyVertex(1, -1, 1);
	glVertex3v16(inttov16(1), inttov16(-1), inttov16(1));

	GFX_TEX_COORD = allBlocks[0].faces.eastUV.uv3;
	// NE_PolyVertex(1, 1, 1);
	glVertex3v16(inttov16(1), inttov16(1), inttov16(1));
}

void drawWestFace()
{
	u32 LightIntensity = RGB15(20, 20, 20);

	NE_PolyColor(LightIntensity); // Set next vertices color

	GFX_TEX_COORD = allBlocks[0].faces.westUV.uv0;
	// NE_PolyVertex(1, 1, -1); // Send new vertex
	glVertex3v16(inttov16(1), inttov16(1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.westUV.uv1;
	// NE_PolyVertex(1, -1, -1);
	glVertex3v16(inttov16(1), inttov16(-1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.westUV.uv2;
	// NE_PolyVertex(-1, -1, -1);
	glVertex3v16(inttov16(-1), inttov16(-1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.westUV.uv3;
	// NE_PolyVertex(-1, 1, -1);
	glVertex3v16(inttov16(-1), inttov16(1), inttov16(-1));
}

void drawNorthFace()
{
	u32 LightIntensity = RGB15(20, 20, 20);

	NE_PolyColor(LightIntensity); // Set next vertices color

	GFX_TEX_COORD = allBlocks[0].faces.northUV.uv0;
	// NE_PolyVertex(1, 1, 1); // Send new vertex
	glVertex3v16(inttov16(1), inttov16(1), inttov16(1));

	GFX_TEX_COORD = allBlocks[0].faces.northUV.uv1;
	// NE_PolyVertex(1, -1, 1);
	glVertex3v16(inttov16(1), inttov16(-1), inttov16(1));

	GFX_TEX_COORD = allBlocks[0].faces.northUV.uv2;
	// NE_PolyVertex(1, -1, -1);
	glVertex3v16(inttov16(1), inttov16(-1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.northUV.uv3;
	// NE_PolyVertex(1, 1, -1);
	glVertex3v16(inttov16(1), inttov16(1), inttov16(-1));
}

void drawSouthFace()
{
	u32 LightIntensity = RGB15(20, 20, 20);

	NE_PolyColor(LightIntensity); // Set next vertices color

	GFX_TEX_COORD = allBlocks[0].faces.northUV.uv0;
	// NE_PolyVertex(-1, 1, -1); // Send new vertex
	glVertex3v16(inttov16(-1), inttov16(1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.southUV.uv1;
	// NE_PolyVertex(-1, -1, -1);
	glVertex3v16(inttov16(-1), inttov16(-1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.southUV.uv2;
	// NE_PolyVertex(-1, -1, 1);
	glVertex3v16(inttov16(-1), inttov16(-1), inttov16(1));

	GFX_TEX_COORD = allBlocks[0].faces.southUV.uv3;
	// NE_PolyVertex(-1, 1, 1);
	glVertex3v16(inttov16(-1), inttov16(1), inttov16(1));
}

void drawUpFace()
{
	u32 LightIntensity = RGB15(15, 31, 15);

	NE_PolyColor(LightIntensity); // Set next vertices color

	GFX_TEX_COORD = allBlocks[0].faces.upUV.uv0;
	// NE_PolyVertex(1, 1, 1); // Send new vertex
	glVertex3v16(inttov16(1), inttov16(1), inttov16(1));

	GFX_TEX_COORD = allBlocks[0].faces.upUV.uv1;
	// NE_PolyVertex(1, 1, -1);
	glVertex3v16(inttov16(1), inttov16(1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.upUV.uv2;
	// NE_PolyVertex(-1, 1, -1);
	glVertex3v16(inttov16(-1), inttov16(1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.upUV.uv3;
	// NE_PolyVertex(-1, 1, 1);
	glVertex3v16(inttov16(-1), inttov16(1), inttov16(1));
}

void drawDownFace()
{
	u32 LightIntensity = RGB15(20, 20, 20);

	NE_PolyColor(LightIntensity); // Set next vertices color

	GFX_TEX_COORD = allBlocks[0].faces.downUV.uv0;
	// NE_PolyVertex(-1, -1, 1); // Send new vertex
	glVertex3v16(inttov16(-1), inttov16(-1), inttov16(1));

	GFX_TEX_COORD = allBlocks[0].faces.downUV.uv1;
	// NE_PolyVertex(-1, -1, -1);
	glVertex3v16(inttov16(-1), inttov16(-1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.downUV.uv2;
	// NE_PolyVertex(1, -1, -1);
	glVertex3v16(inttov16(1), inttov16(-1), inttov16(-1));

	GFX_TEX_COORD = allBlocks[0].faces.downUV.uv3;
	// NE_PolyVertex(1, -1, 1);
	glVertex3v16(inttov16(1), inttov16(-1), inttov16(1));
}
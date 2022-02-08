// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "main.h"
#include "draw3d.h"
#include "noise.h"

#include "terrain256_bin.h"
#include "text_bmp_bin.h"

NE_Camera *Camera;
NE_Material *Texture;
NE_Material *TextMaterial;
NE_Palette *textPalette;
NE_Palette *terrainPalette;

NE_Material *doorMaterial;
NE_Palette *doorPalette;

#define TextureSizeX 512
#define TileResolution 16
#define TextureSizeY 304

Block allBlocks[BLOCK_COUNT];
float rotation;
int newblockYpos;

int seed = 564;
float noiseScale = 0.25;

int chunk[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE];

int chunkTest[3][3][3];
GreedyFace greedyChunk[GREEDY_COUNT];

float xWithoutY, zWithoutY;
Vector3 lookRotation;
float CameraAngleX = 150;
float playerAngleY = 20;

Vector3Int playerPosition;
Vector3Int LastPlayerChunckCollisionPosition;

NE_Model *playerModel;
NE_Physics *playerPhysics;
uint32 keys;
uint32 keysdown;
uint32 keysup;
touchPosition touch;
NE_Physics *physics[3 * CHUNCK_Y_SIZE * 3];
NE_Model *models[3 * CHUNCK_Y_SIZE * 3];

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int greedyCount = 0;

void initBlock()
{
	allBlocks[0].id = 0;
	allBlocks[0].facesPosition.upUV.x = 15;
	allBlocks[0].facesPosition.upUV.y = 10;

	allBlocks[0].facesPosition.downUV.x = 8;
	allBlocks[0].facesPosition.downUV.y = 6;

	allBlocks[0].facesPosition.eastUV.x = 12;
	allBlocks[0].facesPosition.eastUV.y = 10;

	allBlocks[0].facesPosition.northUV.x = 12;
	allBlocks[0].facesPosition.northUV.y = 10;

	allBlocks[0].facesPosition.southUV.x = 12;
	allBlocks[0].facesPosition.southUV.y = 10;

	allBlocks[0].facesPosition.westUV.x = 12;
	allBlocks[0].facesPosition.westUV.y = 10;

	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		int topLeftCornerX = allBlocks[i].facesPosition.upUV.x * TileResolution;
		int topLeftCornerY = allBlocks[i].facesPosition.upUV.y * TileResolution;
		int bottomRightCornerX = topLeftCornerX + TileResolution;
		int bottomRightCornerY = topLeftCornerY + TileResolution;

		allBlocks[i].faces.upUV.uv0 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(topLeftCornerY));
		allBlocks[i].faces.upUV.uv1 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.upUV.uv2 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.upUV.uv3 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(topLeftCornerY));

		topLeftCornerX = allBlocks[i].facesPosition.downUV.x * TileResolution;
		topLeftCornerY = allBlocks[i].facesPosition.downUV.y * TileResolution;
		bottomRightCornerX = topLeftCornerX + TileResolution;
		bottomRightCornerY = topLeftCornerY + TileResolution;

		allBlocks[i].faces.downUV.uv0 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(topLeftCornerY));
		allBlocks[i].faces.downUV.uv1 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.downUV.uv2 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.downUV.uv3 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(topLeftCornerY));

		topLeftCornerX = allBlocks[i].facesPosition.eastUV.x * TileResolution;
		topLeftCornerY = allBlocks[i].facesPosition.eastUV.y * TileResolution;
		bottomRightCornerX = topLeftCornerX + TileResolution;
		bottomRightCornerY = topLeftCornerY + TileResolution;

		allBlocks[i].faces.eastUV.uv0 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(topLeftCornerY));
		allBlocks[i].faces.eastUV.uv1 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.eastUV.uv2 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.eastUV.uv3 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(topLeftCornerY));

		topLeftCornerX = allBlocks[i].facesPosition.northUV.x * TileResolution;
		topLeftCornerY = allBlocks[i].facesPosition.northUV.y * TileResolution;
		bottomRightCornerX = topLeftCornerX + TileResolution;
		bottomRightCornerY = topLeftCornerY + TileResolution;

		allBlocks[i].faces.northUV.uv0 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(topLeftCornerY));
		allBlocks[i].faces.northUV.uv1 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.northUV.uv2 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.northUV.uv3 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(topLeftCornerY));

		topLeftCornerX = allBlocks[i].facesPosition.southUV.x * TileResolution;
		topLeftCornerY = allBlocks[i].facesPosition.southUV.y * TileResolution;
		bottomRightCornerX = topLeftCornerX + TileResolution;
		bottomRightCornerY = topLeftCornerY + TileResolution;

		allBlocks[i].faces.southUV.uv0 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(topLeftCornerY));
		allBlocks[i].faces.southUV.uv1 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.southUV.uv2 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.southUV.uv3 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(topLeftCornerY));

		topLeftCornerX = allBlocks[i].facesPosition.westUV.x * TileResolution;
		topLeftCornerY = allBlocks[i].facesPosition.westUV.y * TileResolution;
		bottomRightCornerX = topLeftCornerX + TileResolution;
		bottomRightCornerY = topLeftCornerY + TileResolution;

		allBlocks[i].faces.westUV.uv0 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(topLeftCornerY));
		allBlocks[i].faces.westUV.uv1 = TEXTURE_PACK(inttot16(topLeftCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.westUV.uv2 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(bottomRightCornerY));
		allBlocks[i].faces.westUV.uv3 = TEXTURE_PACK(inttot16(bottomRightCornerX), inttot16(topLeftCornerY));
	}
}

void generateCollisions()
{
	int count = 0;
	for (int x = playerX - 1; x < playerX + 2; x++)
	{
		if (x < 0 || x >= CHUNCK_X_SIZE)
			continue;

		for (int y = 0; y < CHUNCK_Y_SIZE; y++)
		{
			if (y < 0 || y >= CHUNCK_Y_SIZE)
				continue;

			for (int z = playerZ - 1; z < playerZ + 2; z++)
			{
				if (z < 0 || z >= CHUNCK_Z_SIZE)
					continue;

				if (chunk[x][y][z] == 0)
				{
					if (models[count] != NULL)
					{
						NE_PhysicsDelete(physics[count]);
						NE_ModelDelete(models[count]);
					}

					models[count] = NE_ModelCreate(NE_Static);
					physics[count] = NE_PhysicsCreate(NE_BoundingBox);
					NE_PhysicsSetModel(physics[count], (void *)models[count]); // Physics object and Model assigned to it
					NE_PhysicsEnable(physics[count], false);
					NE_PhysicsSetSize(physics[count], 1, 1, 1);
					NE_ModelSetCoord(models[count], x, y, z);

					count++;
				}
			}
		}
	}
}

void UpdateLookRotation(float angleX, float angleY)
{
	// Math formula to get a point position on sphere from the middle of the sphere with 2 angle
	float TempS = angleY / 512.0 * M_TWOPI;
	float TempT = (384 - angleX) / 512.0 * M_TWOPI;

	double cosTempT = cos(TempT);
	double cosTempS = cos(TempS);
	double SinTempS = sin(TempS);

	lookRotation.x = SinTempS * cosTempT;
	lookRotation.y = -sin(TempT);
	lookRotation.z = cosTempS * cosTempT;

	xWithoutY = -SinTempS;
	zWithoutY = -cosTempS;
}

bool getHitDistance(Vector3Int corner1, Vector3Int corner2, Vector3 dirfrac, Vector3Int startPosition, float *t)
{
	int t1 = (corner1.x - startPosition.x) * dirfrac.x;
	int t2 = (corner2.x - startPosition.x) * dirfrac.x;
	int t3 = (corner1.y - startPosition.y) * dirfrac.y;
	int t4 = (corner2.y - startPosition.y) * dirfrac.y;
	int t5 = (corner1.z - startPosition.z) * dirfrac.z;
	int t6 = (corner2.z - startPosition.z) * dirfrac.z;

	int tmin = MAX(MAX(MIN(t1, t2), MIN(t3, t4)), MIN(t5, t6));
	int tmax = MIN(MIN(MAX(t1, t2), MAX(t3, t4)), MAX(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us, if tmin > tmax, ray doesn't intersect AABB
	if (tmax < 0 || tmin > tmax)
	{
	}
	else
	{
		if (*t > tmin)
		{
			*t = tmin;
			return true;
		}
	}
	return false;
}

bool raycast(Vector3Int startPosition, Vector3 lookRotation, float maxDistance, Vector3Int *blockFound, Vector3Int *normal)
{
	Vector3 dirfrac;
	dirfrac.x = 1.0f / lookRotation.x;
	dirfrac.y = 1.0f / lookRotation.y;
	dirfrac.z = 1.0f / lookRotation.z;

	const int distanceInit = 99999 * 8192;
	float t = distanceInit;
	Vector3Int lastBlock;
	lastBlock.x = lastBlock.y = lastBlock.z = 0;

	int playerPosX = playerPosition.x / 4096.0;
	int playerPosY = playerPosition.y / 4096.0;
	int playerPosZ = playerPosition.z / 4096.0;

	int minX = playerPosX - 1, maxX = playerPosX + 2;
	int minY = playerPosY - 1, maxY = playerPosY + 2;
	int minZ = playerPosZ - 1, maxZ = playerPosZ + 2;

	if (lookRotation.x < 0)
		minX -= 2;
	else
		maxX += 2;

	if (lookRotation.y < 0)
		minY -= 2;
	else
		maxY += 2;

	if (lookRotation.z < 0)
		minZ -= 2;
	else
		maxZ += 2;

	printf("pp %d %d %d\n", playerPosX, playerPosY, playerPosZ);
	printf("i %d %d %d\n", minX, minY, minZ);
	printf("a %d %d %d\n", maxX, maxY, maxZ);

	for (int blockIndexX = minX; blockIndexX < maxX; blockIndexX++)
	{
		if (blockIndexX < 0 || blockIndexX >= CHUNCK_X_SIZE)
			continue;

		for (int blockIndexY = minY; blockIndexY < maxY; blockIndexY++)
		{
			if (blockIndexY < 0 || blockIndexY >= CHUNCK_Y_SIZE)
				continue;

			for (int blockIndexZ = minZ; blockIndexZ < maxZ; blockIndexZ++)
			{
				if (blockIndexZ < 0 || blockIndexZ >= CHUNCK_Z_SIZE)
					continue;

				if (chunk[blockIndexX][blockIndexY][blockIndexZ] == 1)
					continue;

				Vector3Int corner1;
				Vector3Int corner2;

				corner1.x = floattof32(blockIndexX - 0.5);
				corner1.y = floattof32(blockIndexY - 0.5);
				corner1.z = floattof32(blockIndexZ - 0.5);

				corner2.x = floattof32(blockIndexX + 0.5);
				corner2.y = floattof32(blockIndexY + 0.5);
				corner2.z = floattof32(blockIndexZ + 0.5);
				float newBlockDistance = t;
				if (getHitDistance(corner1, corner2, dirfrac, startPosition, &newBlockDistance))
				{
					if (newBlockDistance < t)
					{
						t = newBlockDistance;
						lastBlock.x = blockIndexX;
						lastBlock.y = blockIndexY;
						lastBlock.z = blockIndexZ;
					}
				}
			}
		}
	}
	if (t != distanceInit)
	{
		normal->x = lastBlock.x;
		normal->y = lastBlock.y;
		normal->z = lastBlock.z;

		float nx = (playerPosition.x + t * lookRotation.x) / 4096.0f;
		float ny = (playerPosition.y + t * lookRotation.y) / 4096.0f;
		float nz = (playerPosition.z + t * lookRotation.z) / 4096.0f;

		if (nx >= lastBlock.x + 0.5)
		{
			normal->x++;
		}
		else if (nx <= lastBlock.x - 0.5)
		{
			normal->x--;
		}

		if (ny >= lastBlock.y)
		{
			normal->y++;
		}
		else if (ny <= lastBlock.y - 1)
		{
			normal->y--;
		}

		if (nz >= lastBlock.z + 0.5)
		{
			normal->z++;
		}
		else if (nz <= lastBlock.z - 0.5)
		{
			normal->z--;
		}

		printf("p %d %d %d\n", lastBlock.x, lastBlock.y, lastBlock.z);
		printf("n %d %d %d\n", normal->x, normal->y, normal->z);
		printf("n2 %f %f %f\n", nx, ny, nz);

		printf("\n\n");

		blockFound->x = lastBlock.x;
		blockFound->y = lastBlock.y;
		blockFound->z = lastBlock.z;
		return true;
	}
	else
	{
		printf("No block detected.");
		printf("\n\n");
	}
	return false;
}

float sign(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y)
{
	return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

bool PointInTriangleInt(int px, int py, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y)
{
	int d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(px, py, p1x, p1y, p2x, p2y);
	d2 = sign(px, py, p2x, p2y, p3x, p3y);
	d3 = sign(px, py, p3x, p3y, p1x, p1y);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

void readKeys()
{
	scanKeys();
	keys = keysHeld();
	keysdown = keysDown();
	keysup = keysUp();
	touchRead(&touch);
}

void RotatePlayer()
{
	int AngleSpeed = 2;

	// Change player rotation
	if (keys & KEY_A)
	{
		playerAngleY -= AngleSpeed;
	}
	else if (keys & KEY_Y)
	{
		playerAngleY += AngleSpeed;
	}

	// Change camera rotation
	if (keys & KEY_X && CameraAngleX > 9)
	{
		CameraAngleX -= AngleSpeed;
	}
	else if (keys & KEY_B && CameraAngleX < 245)
	{
		CameraAngleX += AngleSpeed;
	}
}

void MovePlayer(int CurrentSpeed)
{
	playerPhysics->xspeed = 0;
	playerPhysics->zspeed = 0;
	if (keys & KEY_UP)
	{
		playerPhysics->xspeed += CurrentSpeed * xWithoutY * 2;
		playerPhysics->zspeed += CurrentSpeed * zWithoutY * 2;
	}
	else if (keys & KEY_DOWN)
	{
		playerPhysics->xspeed += -CurrentSpeed * xWithoutY * 2;
		playerPhysics->zspeed += -CurrentSpeed * zWithoutY * 2;
	}

	if (keys & KEY_RIGHT)
	{
		playerPhysics->xspeed += CurrentSpeed * -zWithoutY * 2;
		playerPhysics->zspeed += CurrentSpeed * xWithoutY * 2;
	}
	else if (keys & KEY_LEFT)
	{
		playerPhysics->xspeed += CurrentSpeed * zWithoutY * 2;
		playerPhysics->zspeed += CurrentSpeed * -xWithoutY * 2;
	}

	if (keys & KEY_SELECT)
	{
		playerPhysics->yspeed = JumpForce;
	}
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	initBlock();

	// greedyCount = 0;
	// greedyChunk[greedyCount].position.x = 0;
	// greedyChunk[greedyCount].position.y = 0;
	// greedyChunk[greedyCount].position.z = 0;
	// greedyChunk[greedyCount].scale.x = 1;
	// greedyChunk[greedyCount].scale.y = 1;
	// greedyChunk[greedyCount].scale.z = 1;
	// greedyChunk[greedyCount].direction = 0;
	// greedyCount++;

	// greedyChunk[greedyCount].position.x = 3;
	// greedyChunk[greedyCount].position.y = 0;
	// greedyChunk[greedyCount].position.z = 0;
	// greedyChunk[greedyCount].scale.x = 2;
	// greedyChunk[greedyCount].scale.y = 2;
	// greedyChunk[greedyCount].scale.z = 1;
	// greedyChunk[greedyCount].direction = 0;
	// greedyCount++;

	// greedyChunk[greedyCount].position.x = 6;
	// greedyChunk[greedyCount].position.y = 0;
	// greedyChunk[greedyCount].position.z = 0;
	// greedyChunk[greedyCount].scale.x = 1;
	// greedyChunk[greedyCount].scale.y = 1;
	// greedyChunk[greedyCount].scale.z = 1;
	// greedyChunk[greedyCount].direction = 0;
	// greedyCount++;

	for (int x = 0; x < CHUNCK_X_SIZE; x++)
	{
		for (int y = 0; y < CHUNCK_Y_SIZE; y++)
		{
			for (int z = 0; z < CHUNCK_Z_SIZE; z++)
			{
				float height = (noise2((((float)x / (float)CHUNCK_X_SIZE) / noiseScale) + seed, (((float)z / (float)CHUNCK_Z_SIZE) / noiseScale) + seed) + 1) * 0.5;
				height *= CHUNCK_Y_SIZE;

				if (y > height)
					chunk[x][y][z] = 1;
				else
					chunk[x][y][z] = 0;
			}
		}
	}

	// NE_Init3D();
	NE_InitDual3D();
	consoleDemoInit();
	NE_ClippingPlanesSetI(floattof32(0.1), floattof32(90.0)); // Set render distance
	// NE_AntialiasEnable(true);

	// NE_OutliningSetColor()
	//  Set background color
	NE_ClearColorSet(RGB15(17, 26, 29), 31, 63); // Blue sky

	textPalette = NE_PaletteCreate();
	TextMaterial = NE_MaterialCreate();
	NE_MaterialTexLoadBMPtoRGB256(TextMaterial, textPalette, (void *)text_bmp_bin, true); // Load bmp font format

	// Create font
	NE_TextInit(0,			  // Font slot
				TextMaterial, // Image
				8, 8);		  // Size of one character (x, y)

	Camera = NE_CameraCreate();

	playerModel = NE_ModelCreate(NE_Static);
	playerPhysics = NE_PhysicsCreate(NE_BoundingBox);
	NE_PhysicsSetModel(playerPhysics, (void *)playerModel); // Physics object and Model assigned to it
	NE_PhysicsEnable(playerPhysics, true);
	NE_PhysicsSetGravity(playerPhysics, 0.0065);
	NE_PhysicsSetSize(playerPhysics, 0.5, 2, 0.5);
	NE_PhysicsSetFriction(playerPhysics, 1);
	NE_PhysicsOnCollision(playerPhysics, NE_ColBounce);
	NE_PhysicsSetBounceEnergy(playerPhysics, 0);

	NE_CameraSet(Camera,
				 0, 20, 1,
				 0, 0, 0,
				 0, 1, 0);

	playerPosition.x = floattof32(3);
	playerPosition.y = floattof32(10);
	playerPosition.z = floattof32(2);
	NE_ModelSetCoordI(playerModel, playerPosition.x, playerPosition.y, playerPosition.z);

	// NE_MaterialTexLoadBMPtoRGBA(Texture, (void *)bmp24bit_bin, 1);
	// NE_MaterialTexLoadBMPtoRGBA(Texture, (void *)terrain_bin, 1);
	terrainPalette = NE_PaletteCreate();
	Texture = NE_MaterialCreate();
	NE_MaterialTexLoadBMPtoRGB256(Texture, terrainPalette, (void *)terrain256_bin, 1);

	// Avant 90112
	// Après BMP : 89856
	// doorPalette = NE_PaletteCreate();
	// doorMaterial = NE_MaterialCreate();
	// // NE_MaterialTexLoadBMPtoRGB256(doorMaterial, doorPalette, (void *)acacia_trapdoor_bin, 1);
	// NE_MaterialTexLoad(doorMaterial, GL_RGB256, 16, 16, TEXGEN_TEXCOORD, (u8 *)acacia_trapdoor_tex_bin);
	// NE_PaletteLoad(doorPalette, (u16 *)acacia_trapdoor_pal_bin, 256, GL_RGB256);
	// NE_MaterialTexSetPal(doorMaterial, doorPalette);

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			for (int z = 0; z < 3; z++)
			{
				chunkTest[x][y][z] = 0;
			}
		}
	}

	// chunkTest[0][0][2] = 1;
	chunkTest[0][0][0] = 1;
	// chunkTest[0][2][0] = 1;

	// for (int x = 0; x < CHUNCK_X_SIZE; x++)
	// {
	// 	for (int y = 0; y < CHUNCK_Y_SIZE; y++)
	// 	{
	// 		for (int z = 0; z < CHUNCK_Z_SIZE; z++)
	// 		{
	// 			chunk[x][y][z] = 1;
	// 		}
	// 	}
	// }
	// chunk[1][0][0] = 0;
	// chunk[1][1][0] = 0;
	// chunk[2][0][0] = 0;
	// chunk[2][1][0] = 0;

	// for (int x = 0; x < CHUNCK_X_SIZE; x++)
	// {
	// 	for (int y = 0; y < CHUNCK_Y_SIZE; y++)
	// 	{
	// 		for (int z = 0; z < CHUNCK_Z_SIZE; z++)
	// 		{
	// 			chunk[x][y][z] = 0;
	// 		}
	// 	}
	// }
	// chunk[1][CHUNCK_Y_SIZE - 1][1] = 1;
	// chunk[2][CHUNCK_Y_SIZE - 1][0] = 1;

	// chunk[CHUNCK_X_SIZE - 1][CHUNCK_Y_SIZE - 1][0] = 1;
	// chunk[CHUNCK_X_SIZE - 1][CHUNCK_Y_SIZE - 1][CHUNCK_Z_SIZE - 1] = 1;

	// enum DirectionEnum direction = EAST;
	// for (enum DirectionEnum dir = 0; dir < 2; dir++)
	// {
	// 	// if (dir <= 3)
	// 	// if (dir == 2 || dir == 3)
	// 	// continue;
	// 	int xDir = 0;
	// 	int yDir = 0;
	// 	int zDir = 0;
	// 	int sideMax = 3 - 1;
	// 	if (dir == EAST)
	// 	{
	// 		zDir = 1;
	// 	}
	// 	else if (dir == WEST)
	// 	{
	// 		zDir = -1;
	// 		sideMax = 0;
	// 	}
	// 	else if (dir == NORTH)
	// 	{
	// 		xDir = -1;
	// 		sideMax = 0;
	// 	}
	// 	else if (dir == SOUTH)
	// 	{
	// 		xDir = 1;
	// 	}
	// 	else if (dir == UP)
	// 	{
	// 		yDir = 1;
	// 	}
	// 	else if (dir == DOWN)
	// 	{
	// 		yDir = -1;
	// 		sideMax = 0;
	// 	}

	// 	int verified[3][3][3];
	// 	int verifiedCount = 0;
	// 	for (int x = 0; x < 3; x++)
	// 	{
	// 		for (int y = 0; y < 3; y++)
	// 		{
	// 			for (int z = 0; z < 3; z++)
	// 			{
	// 				verified[x][y][z] = 0;
	// 			}
	// 		}
	// 	}
	// 	printf("DEBUT direction : %d\n", dir);

	// 	// for (int x = 0; x < 3; x++)
	// 	// {
	// 	// 	// ONLY DEBUG
	// 	// 	for (int y = 0; y < 3; y++)
	// 	// 	{
	// 	// 		for (int z = 0; z < 3; z++)
	// 	// 		{
	// 	// 			printf("%d", chunkTest[x][y][z]);
	// 	// 		}
	// 	// 		printf("\n");
	// 	// 	}
	// 	// 	printf("\n");
	// 	// }
	// 	for (int z = 0; z < 3; z++)
	// 	{
	// 		// ONLY DEBUG
	// 		for (int x = 0; x < 3; x++)
	// 		{
	// 			for (int y = 0; y < 3; y++)
	// 			{
	// 				int a, b, c;
	// 				getIndexOrder(dir, x, y, z, &a, &b, &c);
	// 				// printf("x%d a%d", x, a);
	// 				printf("%d", chunkTest[a][b][c]);
	// 				// printf("%d", chunkTest[z][x][y]);
	// 			}
	// 			printf("\n");
	// 		}

	// 		while (verifiedCount != 3 * 3 * (z + 1))
	// 		{
	// 			printf("slice %d ", z);
	// 			int startXPosition = -1;
	// 			int finalXPosition = -1;
	// 			int startYPosition = -1;
	// 			int finalYPosition = -1;
	// 			bool visible = true;

	// 			int currentBlockId = -1;

	// 			for (int x = 0; x < 3; x++)
	// 			{
	// 				for (int y = 0; y < 3; y++)
	// 				{
	// 					if (verified[x][y][z] == 0)
	// 					{
	// 						startXPosition = x;
	// 						startYPosition = y;
	// 						currentBlockId = chunkTest[x][y][z];
	// 						if (z != sideMax)
	// 						{
	// 							visible = chunkTest[x + xDir][y + yDir][z + zDir] == 1; // AIR
	// 						}
	// 						if (currentBlockId == 1)
	// 							visible = false;

	// 						x = 3;
	// 						y = 3;
	// 						printf("start %d:%d v:%d", startXPosition, startYPosition, visible);
	// 					}
	// 				}
	// 			}
	// 			for (int x = startXPosition; x < 3; x++)
	// 			{
	// 				if (chunkTest[x][startYPosition][z] != currentBlockId || (!visible && z != sideMax && chunkTest[x + xDir][startYPosition + yDir][z + zDir] == 1) || (visible && z != sideMax && chunkTest[x + xDir][startYPosition + yDir][z + zDir] == 0))
	// 				{
	// 					finalXPosition = x - 1;
	// 					printf("x %d", x);
	// 					break;
	// 				}
	// 			}

	// 			// If the scan hit the end of the list
	// 			if (finalXPosition == -1)
	// 			{
	// 				finalXPosition = 3 - 1;
	// 			}
	// 			for (int x = startXPosition; x < finalXPosition + 1; x++)
	// 			{
	// 				for (int y = startYPosition; y < 3; y++)
	// 				{
	// 					if (chunkTest[x][y][z] != currentBlockId || verified[x][y][z] == 1 || (!visible && z != sideMax && chunkTest[x + xDir][y + yDir][z + zDir] == 1) || (visible && z != sideMax && chunkTest[x + xDir][y + yDir][z + zDir] == 0))
	// 					{
	// 						finalYPosition = y;

	// 						if (startYPosition != y)
	// 						{
	// 							finalYPosition--;
	// 						}
	// 						x = 3;
	// 						y = 3;
	// 					}
	// 				}
	// 			}

	// 			// If the scan hit the end of the list
	// 			if (finalYPosition == -1)
	// 			{
	// 				finalYPosition = 3 - 1;
	// 			}

	// 			printf("fin %d:%d\n", finalXPosition, finalYPosition);
	// 			for (int x = startXPosition; x < finalXPosition + 1; x++)
	// 			{
	// 				for (int y = startYPosition; y < finalYPosition + 1; y++)
	// 				{
	// 					verified[x][y][z] = 1;
	// 					verifiedCount++;
	// 				}
	// 			}

	// 			// Add face to greedychunk
	// 			if (visible)
	// 			{
	// 				greedyChunk[greedyCount].position.y = -(finalXPosition + startXPosition);
	// 				greedyChunk[greedyCount].position.x = (finalYPosition + startYPosition);
	// 				greedyChunk[greedyCount].position.z = z * 2;
	// 				greedyChunk[greedyCount].scale.y = (finalXPosition - startXPosition + 1);
	// 				greedyChunk[greedyCount].scale.x = (finalYPosition - startYPosition + 1);
	// 				greedyChunk[greedyCount].scale.z = 1;
	// 				greedyChunk[greedyCount].direction = dir;
	// 				greedyCount++;
	// 			}
	// 		}
	// 	}
	// }

	makeGreedy();

	while (1)
	{
		readKeys();
		RotatePlayer();
		playerPosition.x = playerPhysics->model->x;
		playerPosition.y = playerPhysics->model->y;
		playerPosition.z = playerPhysics->model->z;
		UpdateLookRotation(CameraAngleX, playerAngleY);
		MovePlayer(150);

		NE_CameraSetI(Camera,
					  playerPosition.x * 2, playerPosition.y * 2 + floattof32(1), playerPosition.z * 2,
					  playerPosition.x * 2 + floattof32(lookRotation.x), playerPosition.y * 2 + floattof32(lookRotation.y) + floattof32(1), playerPosition.z * 2 + floattof32(lookRotation.z),
					  floattof32(0), floattof32(1), floattof32(0));

		if (keysdown & KEY_L)
		{
			Vector3Int Position;
			Position.x = playerPosition.x;
			Position.y = playerPosition.y + floattof32(0.5);
			Position.z = playerPosition.z;

			Vector3Int foundBlock;
			Vector3Int normal;
			if (raycast(Position, lookRotation, 3, &foundBlock, &normal))
			{
				chunk[foundBlock.x][foundBlock.y][foundBlock.z] = 1;
				generateCollisions();
				makeGreedy();
			}
		}

		if (keysdown & KEY_R)
		{
			Vector3Int Position;
			Position.x = playerPosition.x;
			Position.y = playerPosition.y + floattof32(0.5);
			Position.z = playerPosition.z;

			Vector3Int foundBlock;
			Vector3Int normal;
			if (raycast(Position, lookRotation, 3, &foundBlock, &normal))
			{
				chunk[normal.x][normal.y][normal.z] = 0;
				generateCollisions();
				makeGreedy();
			}
		}

		playerPhysics->yspeed -= playerPhysics->gravity;
		int bposx = playerModel->x;
		int bposy = playerModel->y;
		int bposz = playerModel->z;
		int posx = playerModel->x + playerPhysics->xspeed;
		int posy = playerModel->y + playerPhysics->yspeed;
		int posz = playerModel->z + playerPhysics->zspeed;

		bool xCollision2 = false;
		bool yCollision2 = false;
		bool zCollision2 = false;

		// NE_Physics *physics[3 * CHUNCK_Y_SIZE * 3];
		for (int i = 0; i < 3 * CHUNCK_Y_SIZE * 3; i++)
		{
			if (physics[i])
			{
				// printf("A");
				int otherposx = physics[i]->model->x;
				int otherposy = physics[i]->model->y;
				int otherposz = physics[i]->model->z;

				int xSizeSum = (physics[i]->xsize + playerPhysics->xsize) >> 1;
				int ySizeSum = (physics[i]->ysize + playerPhysics->ysize) >> 1;
				int zSizeSum = (physics[i]->zsize + playerPhysics->zsize) >> 1;
				bool xCollision = (abs(posx - otherposx) < xSizeSum);
				bool yCollision = (abs(posy - otherposy) < ySizeSum);
				bool yCollisionOffset = (abs(posy + floattof32(0.5) - otherposy) <= ySizeSum);
				bool yCollisionOffset2 = (abs(posy + floattof32(1) - otherposy) <= ySizeSum);
				bool zCollision = (abs(posz - otherposz) < zSizeSum);
				bool xCollisionOld = (abs(bposx - otherposx) < xSizeSum);
				bool yCollisionOld = (abs(bposy - otherposy) < ySizeSum);
				bool zCollisionOld = (abs(bposz - otherposz) < zSizeSum);

				// bool collision = xCollision && yCollision && zCollision;

				bool anycollision = xCollision || yCollision || zCollision || xCollisionOld || yCollisionOld || zCollisionOld;
				if (!anycollision)
					continue;

				if (yCollision && xCollision && zCollision && xCollisionOld && zCollisionOld)
				{
					playerPhysics->yspeed = 0;
					posy = otherposy + physics[i]->ysize * 1.5;
					continue;
				}
				// if (anycollision)
				// printf("%d %d %d %d %d %d\n", xCollision, yCollision, zCollision, xCollisionOld, yCollisionOld, zCollisionOld);

				// if (xCollision && !xCollisionOld && yCollisionOffset && zCollision || xCollision && !xCollisionOld && !yCollisionOffset && yCollisionOld && zCollision) // PAS TROP MAL
				if ((xCollision && !xCollisionOld && yCollisionOffset && zCollision && zCollisionOld) || (xCollision && !xCollisionOld && !yCollisionOffset && yCollisionOld && zCollision && zCollisionOld))
				{
					// printf("a%d %d %d old %d %d %d of %d %d x%d y%d z%d", xCollision, yCollision, zCollision, xCollisionOld, yCollisionOld, zCollisionOld, yCollisionOffset, yCollisionOffset2, otherposx / 4096, otherposy / 4096, otherposz / 4096);
					playerPhysics->xspeed = 0;
					// posx = bposx;
					xCollision2 = true;
					continue;
				}

				if ((zCollision && !zCollisionOld && yCollisionOffset && xCollision) || (zCollision && !zCollisionOld && !yCollisionOffset && yCollisionOld && xCollision)) // PAS TROP MAL
				{
					// printf("b%d %d %d old %d %d %d x%d y%d z%d\n", xCollision, yCollision, zCollision, xCollisionOld, yCollisionOld, zCollisionOld, otherposx / 4096, otherposy / 4096, otherposz / 4096);
					playerPhysics->zspeed = 0;
					// posx = bposx;
					zCollision2 = true;
					continue;
				}
			}
		}

		if (!yCollision2)
		{
			playerModel->y = posy;
		}
		if (!xCollision2)
		{
			playerModel->x = posx;
		}
		if (!zCollision2)
		{
			playerModel->z = posz;
		}

		// NE_PhysicsUpdate(playerPhysics);
		NE_Process(Draw3DScene);
		NE_WaitForVBL(NE_CAN_SKIP_VBL);
	}

	return 0;
}

void getIndexOrder(enum DirectionEnum direction, int x, int y, int z, int *a, int *b, int *c)
{
	if (direction == EAST || direction == WEST)
	{
		*a = x;
		*b = y;
		*c = z;
	}
	else if (direction == UP || direction == DOWN)
	{
		// zxt
		*a = y;
		*b = z;
		*c = x;
	}
	else /* if (direction == NORTH || direction == SOUTH)*/
	{
		*a = z;
		*b = x;
		*c = y;
	}
}

int getVerified(enum DirectionEnum direction, int (*list)[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE], int x, int y, int z)
{
	if (direction == EAST || direction == WEST)
	{
		return (*list)[x][y][z];
	}
	else if (direction == UP || direction == DOWN)
	{
		return (*list)[y][z][x];
	}
	else if (direction == NORTH || direction == SOUTH)
	{
		return (*list)[z][x][y];
	}
	return -1;
}

void setVerified(enum DirectionEnum direction, int (*list)[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE], int x, int y, int z, int value)
{
	if (direction == EAST || direction == WEST)
	{
		(*list)[x][y][z] = value;
	}
	else if (direction == UP || direction == DOWN)
	{
		(*list)[y][z][x] = value;
	}
	else if (direction == NORTH || direction == SOUTH)
	{
		(*list)[z][x][y] = value;
	}
}

int getChunck(enum DirectionEnum direction, int list[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE], int x, int y, int z)
{
	if (direction == EAST || direction == WEST)
	{
		return list[x][y][z];
	}
	else if (direction == UP || direction == DOWN)
	{
		return list[y][z][x];
	}
	else if (direction == NORTH || direction == SOUTH)
	{
		return list[z][x][y];
	}
	return -1;
}

void setChunck(enum DirectionEnum direction, int (*list)[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE], int x, int y, int z, int value)
{
	if (direction == EAST || direction == WEST)
	{
		(*list)[x][y][z] = value;
	}
	else if (direction == UP || direction == DOWN)
	{
		(*list)[y][z][x] = value;
	}
	else if (direction == NORTH || direction == SOUTH)
	{
		(*list)[z][x][y] = value;
	}
}

void makeGreedy()
{
	// CPU Avant greedy 55% 16x16x16
	// CPU Après greedy 5% 16x16x16
	greedyCount = 0;
	for (enum DirectionEnum dir = 0; dir < 6; dir++)
	{
		// if (dir <= 3)
		// if (dir == 2 || dir == 3)
		// continue;
		int zDir = 0;
		int sideMax = CHUNCK_Z_SIZE - 1;
		if (dir == EAST)
		{
			zDir = 1;
		}
		else if (dir == WEST)
		{
			zDir = -1;
			sideMax = 0;
		}
		else if (dir == NORTH)
		{
			// zDir = -1;
			// sideMax = 0;
			zDir = 1;
		}
		else if (dir == SOUTH)
		{
			zDir = -1;
			sideMax = 0;
			// zDir = 1;
		}
		else if (dir == UP)
		{
			zDir = 1;
			// zDir = -1;
			// sideMax = 0;
		}
		else if (dir == DOWN)
		{
			// zDir = 1;
			zDir = -1;
			sideMax = 0;
		}

		int verified[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE];
		int verifiedCount = 0;
		for (int x = 0; x < CHUNCK_X_SIZE; x++)
		{
			for (int y = 0; y < CHUNCK_Y_SIZE; y++)
			{
				for (int z = 0; z < CHUNCK_Z_SIZE; z++)
				{
					verified[x][y][z] = 0;
				}
			}
		}
		// printf("DEBUT direction : %d\n", dir);

		for (int z = 0; z < CHUNCK_Z_SIZE; z++)
		{
			// ONLY DEBUG
			// for (int x = 0; x < CHUNCK_X_SIZE; x++)
			// {
			// 	for (int y = 0; y < CHUNCK_Y_SIZE; y++)
			// 	{
			// 		// printf("x%d a%d", x, a);
			// 		printf("%d", getChunck(dir, chunk, x, y, z));
			// 	}
			// 	printf("\n");
			// }
			// continue;
			while (verifiedCount != CHUNCK_X_SIZE * CHUNCK_Y_SIZE * (z + 1))
			// while (verifiedCount < CHUNCK_X_SIZE * CHUNCK_Y_SIZE * (z + 1))
			{
				// printf("slc%d ", z);
				int startXPosition = -1;
				int finalXPosition = -1;
				int startYPosition = -1;
				int finalYPosition = -1;
				bool visible = true;

				int currentBlockId = -1;

				for (int x = 0; x < CHUNCK_X_SIZE; x++)
				{
					for (int y = 0; y < CHUNCK_Y_SIZE; y++)
					{
						if (getVerified(dir, &verified, x, y, z) == 0)
						{
							startXPosition = x;
							startYPosition = y;
							currentBlockId = getChunck(dir, chunk, x, y, z);
							if (z != sideMax)
							{
								visible = getChunck(dir, chunk, x, y, z + zDir) == 1; // AIR
							}
							// if (currentBlockId == 1)
							// visible = false;

							x = CHUNCK_X_SIZE;
							y = CHUNCK_Y_SIZE;
							// printf("Strt%d:%dV:%d", startXPosition, startYPosition, visible);
							//   verifiedCount += 1; /////////////////////////////////////////////////////////:
						}
					}
				}
				for (int x = startXPosition; x < CHUNCK_X_SIZE; x++)
				{
					if (getChunck(dir, chunk, x, startYPosition, z) != currentBlockId || (!visible && z != sideMax && getChunck(dir, chunk, x, startYPosition, z + zDir) == 1) || (visible && z != sideMax && getChunck(dir, chunk, x, startYPosition, z + zDir) == 0))
					{
						finalXPosition = x - 1;
						// printf("X%d", x);
						break;
					}
				}
				// printf("\n");
				//  If the scan hit the end of the list
				if (finalXPosition == -1)
				{
					finalXPosition = CHUNCK_X_SIZE - 1;
				}
				for (int y = startYPosition; y < CHUNCK_Y_SIZE; y++)
				{
					for (int x = startXPosition; x < finalXPosition + 1; x++)
					{
						// for (int y = startYPosition; y < CHUNCK_Y_SIZE; y++)
						//{
						// if (z == 1)
						// {
						// 	printf("x%dy%dv%d\n", x, y, getChunck(dir, chunk, x, y, z + zDir));
						// }
						if (getChunck(dir, chunk, x, y, z) != currentBlockId || getVerified(dir, &verified, x, y, z) == 1 || (!visible && z != sideMax && getChunck(dir, chunk, x, y, z + zDir) == 1) || (visible && z != sideMax && getChunck(dir, chunk, x, y, z + zDir) == 0))
						{
							finalYPosition = y;

							if (startYPosition != y)
							{
								finalYPosition--;
							}
							// printf("Y%d", y);
							x = CHUNCK_X_SIZE;
							y = CHUNCK_Y_SIZE;
						}
					}
				}

				// If the scan hit the end of the list
				if (finalYPosition == -1)
				{
					finalYPosition = CHUNCK_Y_SIZE - 1;
				}

				// printf("fin %d:%d\n", finalXPosition, finalYPosition);
				for (int x = startXPosition; x < finalXPosition + 1; x++)
				{
					for (int y = startYPosition; y < finalYPosition + 1; y++)
					{
						setVerified(dir, &verified, x, y, z, 1);
						verifiedCount++;
					}
				}

				// Add face to greedychunk
				if (visible && currentBlockId != 1)
				{
					int a, b, c;
					getIndexOrder(dir, (finalXPosition + startXPosition), (finalYPosition + startYPosition), z * 2, &a, &b, &c);
					int a2, b2, c2;
					getIndexOrder(dir, (finalXPosition - startXPosition + 1), (finalYPosition - startYPosition + 1), 1, &a2, &b2, &c2);
					greedyChunk[greedyCount].position.x = a;
					greedyChunk[greedyCount].position.y = b;
					greedyChunk[greedyCount].position.z = c;
					greedyChunk[greedyCount].scale.x = a2;
					greedyChunk[greedyCount].scale.y = b2;
					greedyChunk[greedyCount].scale.z = c2;
					greedyChunk[greedyCount].direction = dir;
					greedyCount++;
				}
			}
		}
	}
}
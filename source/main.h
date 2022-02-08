#ifndef MAIN_H_ /* Include guard */
#define MAIN_H_

#include <NEMain.h>
#include "draw3d.h"

// Math functions
#include <math.h>

#define JumpForce 400
#define CHUNCK_X_SIZE 16
#define CHUNCK_Y_SIZE 16
#define CHUNCK_Z_SIZE 16
#define BLOCK_COUNT 1

#define GREEDY_COUNT 4000

// 3D point
typedef struct
{
    float x;
    float y;
    float z;
} Vector3;

// 3D point
typedef struct
{
    int x;
    int y;
    int z;
} Vector3Int;

// 3D point
typedef struct
{
    int x;
    int y;
} Vector2Int;

// 3D point
typedef struct
{
    // Vector2Int topLeft;
    // Vector2Int bottomRight;
    u32 uv0;
    u32 uv1;
    u32 uv2;
    u32 uv3;
} UV;

// 3D point
typedef struct
{
    Vector3Int scale;
    Vector3Int position;
    enum DirectionEnum direction;
} GreedyFace;

// 3D point
typedef struct
{
    UV downUV;
    UV upUV;
    UV northUV;
    UV southUV;
    UV westUV;
    UV eastUV;
    // u32 downUV;
    // u32 upUV;
    // u32 northUV;
    // u32 southUV;
    // u32 westUV;
    // u32 eastUV;
} Faces;

// 3D point
typedef struct
{
    Vector2Int downUV;
    Vector2Int upUV;
    Vector2Int northUV;
    Vector2Int southUV;
    Vector2Int westUV;
    Vector2Int eastUV;
} FacesPosition;

// 3D point
typedef struct
{
    int id;
    int x;
    int y;
    Faces faces;
    FacesPosition facesPosition;
} Block;

void generateCollisions();
void UpdateLookRotation(float angleX, float angleY);
bool getHitDistance(Vector3Int corner1, Vector3Int corner2, Vector3 dirfrac, Vector3Int startPosition, float *t);
bool raycast(Vector3Int startPosition, Vector3 lookRotation, float maxDistance, Vector3Int *blockFound, Vector3Int *normal);
float sign(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y);
bool PointInTriangleInt(int px, int py, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y);
void readKeys();
void RotatePlayer();
void MovePlayer(int CurrentSpeed);
void getIndexOrder(enum DirectionEnum direction, int x, int y, int z, int *a, int *b, int *c);
int getVerified(enum DirectionEnum direction, int (*list)[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE], int x, int y, int z);
void setVerified(enum DirectionEnum direction, int (*list)[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE], int x, int y, int z, int value);
int getChunck(enum DirectionEnum direction, int list[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE], int x, int y, int z);
void setChunck(enum DirectionEnum direction, int (*list)[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE], int x, int y, int z, int value);
void makeGreedy();

extern Block allBlocks[BLOCK_COUNT];
extern NE_Camera *Camera;
extern NE_Material *Texture;
extern Vector3Int playerPosition;
extern Vector3Int LastPlayerChunckCollisionPosition;
extern NE_Model *playerModel;
extern Vector3 lookRotation;
extern int chunk[CHUNCK_X_SIZE][CHUNCK_Y_SIZE][CHUNCK_Z_SIZE];
extern GreedyFace greedyChunk[GREEDY_COUNT];
extern int greedyCount;

#endif // MAIN_H_
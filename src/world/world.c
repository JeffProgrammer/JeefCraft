//-----------------------------------------------------------------------------
// Copyright 2018 Jeff Hutchinson
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#include <stretchy_buffer.h>
#include "math/ray.h"
#include "world/cube.h"
#include "world/world.h"
#include "world/util.h"
#include "world/materials.h"
#include "world/terrainGenerator.h"
#include "world/worldTypes.h"
#include "render/worldRenderer.h"

/// ChunkWorld is a flat 2D array that represents the entire
/// world based upon
Chunk *gChunkWorld = NULL;

// Grid size but should be variable. This is the 'chunk distance'.
S32 worldSize = 2;

void initWorld() {
   initTerrainGenerator();

   gChunkWorld = (Chunk*)calloc((worldSize * 2) * (worldSize * 2), sizeof(Chunk));

   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         // World position calcuation before passing.
         generateWorld(x, z, x * CHUNK_WIDTH, z * CHUNK_WIDTH);
      }
   }

   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         generateCavesAndStructures(x, z, x * CHUNK_WIDTH, z * CHUNK_WIDTH);
      }
   }

   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         Chunk * chunk = getChunkAt(x, z);
         for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
            generateGeometryForRenderChunk(chunk, i);
         }
      }
   }

   // Upload to GL.
   uploadGeometryToGL();
}

void freeWorld() {
   freeTerrainGenerator();

   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         Chunk *c = getChunkAt(x, z);
         free(c->cubeData);

         // free from renderer
         freeChunkGL(c);
      }
   }

   free(gChunkWorld);
}

F32 getViewDistance() {
   // Give 1 chunk 'padding' looking forward.
   return worldSize * CHUNK_WIDTH + CHUNK_WIDTH;
}

bool isTransparent(Cube *cubeData, S32 x, S32 y, S32 z) {
   assert(cubeData);
   assert(getCubeAtLocalChunkPos(cubeData, x, y, z));
   return getCubeAtLocalChunkPos(cubeData, x, y, z)->material == Material_Air;
}

bool isTransparentAtCube(Cube *c) {
   if (c == NULL)
      return false;
   return c->material == Material_Air;
}

static void buildFace(Chunk *chunk, S32 index, S32 side, S32 material, Vec3 localPos) {
   // Vertex data first, then index data.

   RenderChunk *renderChunk = &chunk->renderChunks[index];

   for (S32 i = 0; i < 4; ++i) {
      GPUVertex v;
      v.position.x = cubes[side][i][0] + localPos.x;
      v.position.y = cubes[side][i][1] + localPos.y;
      v.position.z = cubes[side][i][2] + localPos.z;
      v.position.w = cubes[side][i][3];
      v.uvx = (F32)(cubeUVs[side][i][0] + ((F32)(material % TEXTURE_ATLAS_COUNT_I))) / TEXTURE_ATLAS_COUNT_F;
      v.uvy = (F32)(cubeUVs[side][i][1] + ((F32)(material / TEXTURE_ATLAS_COUNT_I))) / TEXTURE_ATLAS_COUNT_F;
      sb_push(renderChunk->vertexData, v);
   }
   renderChunk->vertexCount += 4;

   GPUIndex in = renderChunk->currentIndex;
   sb_push(renderChunk->indices, in);
   sb_push(renderChunk->indices, in + 2);
   sb_push(renderChunk->indices, in + 1);
   sb_push(renderChunk->indices, in);
   sb_push(renderChunk->indices, in + 3);
   sb_push(renderChunk->indices, in + 2);
   renderChunk->currentIndex += 4;
   renderChunk->indiceCount += 6;
}

void generateGeometryForRenderChunk(Chunk *chunk, S32 renderChunkId) {
   Cube *cubeData = chunk->cubeData;
   S32 chunkX = chunk->startX;
   S32 chunkZ = chunk->startZ;

   for (S32 x = 0; x < CHUNK_WIDTH; ++x) {
      for (S32 z = 0; z < CHUNK_WIDTH; ++z) {
         for (S32 j = 0; j < RENDER_CHUNK_HEIGHT; ++j) {
            S32 y = (RENDER_CHUNK_HEIGHT * renderChunkId) + j;
            Vec3 localPos;
            localPos.x = (F32)x;
            localPos.y = (F32)y;
            localPos.z = (F32)z;

            // skip if current block is transparent.
            if (isTransparent(cubeData, x, y, z))
               continue;

            // Cross chunk checking. Only need to check x and z axes.
            // If the next *chunk* over is is transparent then ya we have
            // to render regardless.
            bool isOpaqueNegativeX = false;
            bool isOpaquePositiveX = false;
            bool isOpaqueNegativeZ = false;
            bool isOpaquePositiveZ = false;

            if (x == 0 && chunkX > -worldSize) {
               Cube *behindData = getChunkAt(chunkX - 1, chunkZ)->cubeData;
               if (!isTransparent(behindData, CHUNK_WIDTH - 1, y, z)) {
                  // The cube behind us on the previous chunk is in fact
                  // transparent. We need to render this face.
                  isOpaqueNegativeX = true;
               }
            }
            if (x == (CHUNK_WIDTH - 1) && (chunkX + 1) < worldSize) {
               Cube *behindData = getChunkAt(chunkX + 1, chunkZ)->cubeData;
               if (!isTransparent(behindData, 0, y, z)) {
                  // The cube behind us on the previous chunk is in fact
                  // transparent. We need to render this face.
                  isOpaquePositiveX = true;
               }
            }
            if (z == 0 && chunkZ > -worldSize) {
               Cube *behindData = getChunkAt(chunkX, chunkZ - 1)->cubeData;
               if (!isTransparent(behindData, x, y, CHUNK_WIDTH - 1)) {
                  // The cube behind us on the previous chunk is in fact
                  // transparent. We need to render this face.
                  isOpaqueNegativeZ = true;
               }
            }
            if (z == (CHUNK_WIDTH - 1) && (chunkZ + 1) < worldSize) {
               Cube *behindData = getChunkAt(chunkX, chunkZ + 1)->cubeData;
               if (!isTransparent(behindData, x, y, 0)) {
                  // The cube behind us on the previous chunk is in fact
                  // transparent. We need to render this face.
                  isOpaquePositiveZ = true;
               }
            }

            // check all 6 directions to see if the cube is exposed.
            // If the cube is exposed in that direction, render that face.

            S32 material = getCubeAtLocalChunkPos(cubeData, x, y, z)->material;

            if (y >= (MAX_CHUNK_HEIGHT - 1) || isTransparent(cubeData, x, y + 1, z))
               buildFace(chunk, renderChunkId, CubeSides_Up, material, localPos);

            // If this is grass, bottom has to be dirt.

            if (y == 0 || isTransparent(cubeData, x, y - 1, z))
               buildFace(chunk, renderChunkId, CubeSides_Down, (material == Material_Grass ? Material_Dirt : material), localPos);

            // After we built the top, this is a special case for grass.
            // If we are actually building grass sides it has to be special.
            if (material == Material_Grass)
               material = Material_Grass_Side;

            if ((!isOpaqueNegativeX && x == 0) || (x > 0 && isTransparent(cubeData, x - 1, y, z)))
               buildFace(chunk, renderChunkId, CubeSides_West, material, localPos);

            if ((!isOpaquePositiveX && x >= (CHUNK_WIDTH - 1)) || (x < (CHUNK_WIDTH - 1) && isTransparent(cubeData, x + 1, y, z)))
               buildFace(chunk, renderChunkId, CubeSides_East, material, localPos);

            if ((!isOpaqueNegativeZ && z == 0) || (z > 0 && isTransparent(cubeData, x, y, z - 1)))
               buildFace(chunk, renderChunkId, CubeSides_South, material, localPos);

            if ((!isOpaquePositiveZ && z >= (CHUNK_WIDTH - 1)) || (z < (CHUNK_WIDTH - 1) && isTransparent(cubeData, x, y, z + 1)))
               buildFace(chunk, renderChunkId, CubeSides_North, material, localPos);
         }
      }
   }
}

void freeGenerateUpdate(Chunk *c, S32 renderChunkId) {
   assert(c);

   // TODO: split rendering from chunk generation.
   // this is especially important for multiplayer where worldgen
   // is server side and rendering is client side.
   freeRenderChunkGL(c, renderChunkId);
   generateGeometryForRenderChunk(c, renderChunkId);
   uploadRenderChunkToGL(c, renderChunkId);
}

static void remeshChunkGeometryAtGlobalPos(S32 x, S32 y, S32 z) {
   // Rebuild this *render chunk*
   S32 renderChunkId;
   Chunk *c = getChunkAtWorldSpacePosition(x, y, z);
   RenderChunk *r = getRenderChunkAtWorldSpacePosition(x, y, z, &renderChunkId);
   freeGenerateUpdate(c, renderChunkId);

   // Check x,y,z axes to see if they lay on render chunk boundaries.
   // If they do, we need to update the render chunk that is next to it.
   S32 localX, localY, localZ;
   worldPosToLocalPos(x, y, z, &localX, &localY, &localZ);

   if (localX == 0) {
      c = getChunkAtWorldSpacePosition(x - CHUNK_WIDTH, y, z);
      r = getRenderChunkAtWorldSpacePosition(x - CHUNK_WIDTH, y, z, &renderChunkId);
      freeGenerateUpdate(c, renderChunkId);
   } else if (localX >= (CHUNK_WIDTH - 1)) {
      c = getChunkAtWorldSpacePosition(x + CHUNK_WIDTH, y, z);
      r = getRenderChunkAtWorldSpacePosition(x + CHUNK_WIDTH, y, z, &renderChunkId);
      freeGenerateUpdate(c, renderChunkId);
   }

   if (localY == 0) {
      c = getChunkAtWorldSpacePosition(x, y - RENDER_CHUNK_HEIGHT, z);
      r = getRenderChunkAtWorldSpacePosition(x, y - RENDER_CHUNK_HEIGHT, z, &renderChunkId);
      freeGenerateUpdate(c, renderChunkId);
   } else if (localY >= (RENDER_CHUNK_HEIGHT - 1)) {
      c = getChunkAtWorldSpacePosition(x, y + RENDER_CHUNK_HEIGHT, z);
      r = getRenderChunkAtWorldSpacePosition(x, y + RENDER_CHUNK_HEIGHT, z, &renderChunkId);
      freeGenerateUpdate(c, renderChunkId);
   }

   if (localZ == 0) {
      c = getChunkAtWorldSpacePosition(x, y, z - CHUNK_WIDTH);
      r = getRenderChunkAtWorldSpacePosition(x, y, z - CHUNK_WIDTH, &renderChunkId);
      freeGenerateUpdate(c, renderChunkId);
   } else if (localZ >= (CHUNK_WIDTH - 1)) {
      c = getChunkAtWorldSpacePosition(x, y, z + CHUNK_WIDTH);
      r = getRenderChunkAtWorldSpacePosition(x, y, z + CHUNK_WIDTH, &renderChunkId);
      freeGenerateUpdate(c, renderChunkId);
   }
}

void removeCubeAtWorldPosition(Cube *cube, S32 x, S32 y, S32 z) {
   // Bounds check on removing cube if we are at a boundary.
   if (x <= -worldSize * CHUNK_WIDTH ||
      x >= worldSize * CHUNK_WIDTH ||
      z <= -worldSize * CHUNK_WIDTH ||
      z >= worldSize * CHUNK_WIDTH ||
      y <= 0 ||
      y >= MAX_CHUNK_HEIGHT) {
      printf("Cannot remove cube at %d %d %d. It is at a world edge boundary!\n", x, y, z);
      return;
   }

   cube->material = Material_Air;
   remeshChunkGeometryAtGlobalPos(x, y, z);
}


void addCubeAtGlobalPos(Vec3 position) {
   S32 x = (S32)position.x;
   S32 y = (S32)position.y;
   S32 z = (S32)position.z;

   // Bounds check on removing cube if we are at a boundary.
   if (x <= -worldSize * CHUNK_WIDTH ||
      x >= worldSize * CHUNK_WIDTH ||
      z <= -worldSize * CHUNK_WIDTH ||
      z >= worldSize * CHUNK_WIDTH ||
      y <= 0 ||
      y >= MAX_CHUNK_HEIGHT) {
      printf("Cannot remove cube at %d %d %d. It is at a world edge boundary!\n", x, y, z);
      return;
   }

   // Just add bedrock for now.
   getCubeAtWorldSpacePosition(x, y, z)->material = Material_Bedrock;
   remeshChunkGeometryAtGlobalPos(x, y, z);
}

// Cube xyz
void checkCubeAtLookAtCube(Vec3 cameraOrigin, Vec3 cameraDir, S32 x, S32 y, S32 z) {
   Vec3 cubePos = create_vec3((F32)x, (F32)y, (F32)z);

   Vec4 planes[6];
   planes[0] = create_vec4(cubeNormals[0][0], cubeNormals[0][1], cubeNormals[0][2], x + 1); // East
   planes[1] = create_vec4(cubeNormals[1][0], cubeNormals[1][1], cubeNormals[1][2], y + 1); // Up
   planes[2] = create_vec4(cubeNormals[2][0], cubeNormals[2][1], cubeNormals[2][2], -x);    // West
   planes[3] = create_vec4(cubeNormals[3][0], cubeNormals[3][1], cubeNormals[3][2], -y);    // Down
   planes[4] = create_vec4(cubeNormals[4][0], cubeNormals[4][1], cubeNormals[4][2], z + 1); // North
   planes[5] = create_vec4(cubeNormals[5][0], cubeNormals[5][1], cubeNormals[5][2], -z);    // South

                                                                                            // Get the face of the cube that we are touching.
   for (S32 i = 0; i < 6; ++i) {
      Vec3 norm = create_vec3(planes[i].x, planes[i].y, planes[i].z);
      if (glm_vec_dot(cameraDir.vec, norm.vec) < 0.0f) {
         Vec3 outPos;
         if (rayIntersectsPlane(cameraOrigin, cameraDir, planes[i], &outPos)) {
            // Cube test 'if cube contains point'
            if (outPos.x >= cubePos.x && outPos.x <= cubePos.x + 1 &&
               outPos.y >= cubePos.y && outPos.y <= cubePos.y + 1 &&
               outPos.z >= cubePos.z && outPos.z <= cubePos.z + 1) {
               // We found our plane!
               glm_vec_add(cubePos.vec, norm.vec, cubePos.vec);
               addCubeAtGlobalPos(cubePos);
               break;
            }
         }
      }
   }
}
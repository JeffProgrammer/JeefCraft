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

#include "world/util.h"
#include "world/material.h"
#include "world/worldMap.h"

extern S32 worldSize;

// Our map world
extern ChunkTable gChunkTable;

Cube* getCubeAt(Cube *cubeData, S32 x, S32 y, S32 z) {
   return &cubeData[x * (MAX_CHUNK_HEIGHT) * (CHUNK_WIDTH)+z * (MAX_CHUNK_HEIGHT)+y];
}

bool isTransparent(Cube *cubeData, S32 x, S32 y, S32 z) {
   assert(cubeData);
   assert(getCubeAt(cubeData, x, y, z));
   return getCubeAt(cubeData, x, y, z)->material == Material_Air;
}

bool isTransparentAtCube(Cube *c) {
   if (c == NULL)
      return false;
   return c->material == Material_Air;
}

Chunk* getChunkAtWorldSpacePosition(S32 x, S32 y, S32 z) {
   Chunk *chunk = chunktable_getAt(&gChunkTable, x, z);
   assert(chunk);
   return chunk;
}

RenderChunk* getRenderChunkAtWorldSpacePosition(S32 x, S32 y, S32 z, S32 *renderChunkIndex) {
   Chunk *chunk = getChunkAtWorldSpacePosition(x, y, z);
   if (chunk == NULL)
      return NULL;
   assert(y >= 0); // Ensure y is >= 0
   assert(y < MAX_CHUNK_HEIGHT); // Ensure chunk is < MAX_CHUNK_HEIGHT

   *renderChunkIndex = y / RENDER_CHUNK_HEIGHT;
   return &chunk->renderChunks[*renderChunkIndex];
}

void globalPosToLocalPos(S32 x, S32 y, S32 z, S32 *localX, S32 *localY, S32 *localZ) {
   // first calculate chunk based upon position.
   S32 chunkX = x < 0 ? ((x + 1) / CHUNK_WIDTH) - 1 : x / CHUNK_WIDTH;
   S32 chunkZ = z < 0 ? ((z + 1) / CHUNK_WIDTH) - 1 : z / CHUNK_WIDTH;
   S32 chunkY = y / RENDER_CHUNK_HEIGHT;

   *localX = x - (chunkX * CHUNK_WIDTH);
   *localZ = z - (chunkZ * CHUNK_WIDTH);
   *localY = y - (chunkY * RENDER_CHUNK_HEIGHT);

   assert(*localX >= 0);
   assert(*localZ >= 0);
   assert(*localY >= 0);
   assert(*localX < CHUNK_WIDTH);
   assert(*localZ < CHUNK_WIDTH);
   assert(*localY < RENDER_CHUNK_HEIGHT);
}

Cube* getGlobalCubeAtWorldSpacePosition(S32 x, S32 y, S32 z) {
   // first calculate chunk based upon position.
   S32 chunkX = x < 0 ? ((x + 1) / CHUNK_WIDTH) - 1 : x / CHUNK_WIDTH;
   S32 chunkZ = z < 0 ? ((z + 1) / CHUNK_WIDTH) - 1 : z / CHUNK_WIDTH;

   Chunk *chunk = chunktable_getAt(&gChunkTable, x, z);
   if (chunk == NULL)
      return NULL;

   S32 localChunkX = x - (chunkX * CHUNK_WIDTH);
   S32 localChunkZ = z - (chunkZ * CHUNK_WIDTH);

   assert(localChunkX >= 0);
   assert(localChunkZ >= 0);
   assert(localChunkX < CHUNK_WIDTH);
   assert(localChunkZ < CHUNK_WIDTH);

   return getCubeAt(chunk->cubeData, localChunkX, y, localChunkZ);
}
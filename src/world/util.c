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

/// ChunkWorld is a flat 2D array that represents the entire
/// world based upon
extern Chunk *gChunkWorld;

// Grid size but should be variable. This is the 'chunk distance'.
extern S32 worldSize;

Chunk* getChunkAt(S32 x, S32 z) {
   // Since x and z can go from -worldSize to worldSize,
   // we need to normalize them so that they are always positive.
   x += worldSize;
   z += worldSize;

   S32 index = (z * (worldSize * 2)) + x;
   return &gChunkWorld[index];
}

Cube* getCubeAtLocalChunkPos(Cube *cubeData, S32 x, S32 y, S32 z) {
   return &cubeData[x * (MAX_CHUNK_HEIGHT) * (CHUNK_WIDTH)+z * (MAX_CHUNK_HEIGHT)+y];
}

Chunk* getChunkAtWorldSpacePosition(S32 x, S32 y, S32 z) {
   // first calculate chunk based upon position.
   S32 chunkX = x < 0 ? ((x + 1) / CHUNK_WIDTH) - 1 : x / CHUNK_WIDTH;
   S32 chunkZ = z < 0 ? ((z + 1) / CHUNK_WIDTH) - 1 : z / CHUNK_WIDTH;

   // Don't go past.
   if (chunkX < -worldSize || chunkX >= worldSize || chunkZ < -worldSize || chunkZ >= worldSize)
      return NULL;

   Chunk *chunk = getChunkAt(chunkX, chunkZ);
   return chunk;
}

RenderChunk* getRenderChunkAtWorldSpacePosition(S32 x, S32 y, S32 z, S32 *outRenderChunkIndex) {
   Chunk *chunk = getChunkAtWorldSpacePosition(x, y, z);
   if (chunk == NULL)
      return NULL;
   assert(y >= 0); // Ensure y is >= 0
   assert(y < MAX_CHUNK_HEIGHT); // Ensure chunk is < MAX_CHUNK_HEIGHT

   *outRenderChunkIndex = y / RENDER_CHUNK_HEIGHT;
   return &chunk->renderChunks[*outRenderChunkIndex];
}

void worldPosToLocalPos(S32 x, S32 y, S32 z, S32 *outLocalX, S32 *outLocalY, S32 *outLocalZ) {
   // first calculate chunk based upon position.
   S32 chunkX = x < 0 ? ((x + 1) / CHUNK_WIDTH) - 1 : x / CHUNK_WIDTH;
   S32 chunkZ = z < 0 ? ((z + 1) / CHUNK_WIDTH) - 1 : z / CHUNK_WIDTH;
   S32 chunkY = y / RENDER_CHUNK_HEIGHT;

   *outLocalX = x - (chunkX * CHUNK_WIDTH);
   *outLocalZ = z - (chunkZ * CHUNK_WIDTH);
   *outLocalY = y - (chunkY * RENDER_CHUNK_HEIGHT);

   assert(*outLocalX >= 0);
   assert(*outLocalZ >= 0);
   assert(*outLocalY >= 0);
   assert(*outLocalX < CHUNK_WIDTH);
   assert(*outLocalZ < CHUNK_WIDTH);
   assert(*outLocalY < RENDER_CHUNK_HEIGHT);
}

Cube* getCubeAtWorldSpacePosition(S32 x, S32 y, S32 z) {
   // first calculate chunk based upon position.
   S32 chunkX = x < 0 ? ((x + 1) / CHUNK_WIDTH) - 1 : x / CHUNK_WIDTH;
   S32 chunkZ = z < 0 ? ((z + 1) / CHUNK_WIDTH) - 1 : z / CHUNK_WIDTH;

   // Don't go past.
   if (chunkX < -worldSize || chunkX >= worldSize || chunkZ < -worldSize || chunkZ >= worldSize)
      return NULL;

   Chunk *chunk = getChunkAt(chunkX, chunkZ);

   S32 localChunkX = x - (chunkX * CHUNK_WIDTH);
   S32 localChunkZ = z - (chunkZ * CHUNK_WIDTH);

   assert(localChunkX >= 0);
   assert(localChunkZ >= 0);
   assert(localChunkX < CHUNK_WIDTH);
   assert(localChunkZ < CHUNK_WIDTH);

   return getCubeAtLocalChunkPos(chunk->cubeData, localChunkX, y, localChunkZ);
}

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

#ifndef _WORLD_UTIL_H_
#define _WORLD_UTIL_H_

#include "world/worldTypes.h"

/**
 * @brief Gets the chunk at 'chunk' local positions 
 *  (i.e. chunk x = 2 is next to chunk x = 1)
 * @return Returns a pointer to the chunk. 
 */
Chunk* getChunkAt(S32 x, S32 z);

/** 
 * @brief Gets a specific cube from the cube data array.
 *  cubeData is a flattened 3D array that sits in Chunk.
 * @param x The local X position for the cube in a chunk.
 * @param y The local X position for the cube in a chunk.
 * @param z The local X position for the cube in a chunk.
 * @return Returns a Pointer to the cube.
 */
Cube* getCubeAtLocalChunkPos(Cube *cubeData, S32 x, S32 y, S32 z);

/**
 * @brief Gets a chunk at world space position.
 * @param x The x coordinate in worldspace.
 * @param y The y coordinate in worldspace.
 * @param z The z coordinate in worldspace.
 * @return Returns a pointer to the chunk.
 */
Chunk* getChunkAtWorldSpacePosition(S32 x, S32 y, S32 z);

/**
 * @brief Gets a RenderChunk at world space position.
 * @param x The x coordinate in worldspace.
 * @param y The y coordinate in worldspace.
 * @param z The z coordinate in worldspace.
 * @param outRenderChunkIndex The RenderChunk index within the renderChunks 
 * array inside of the Chunk structure if Return is not NULL.
 * @return Returns a pointer to the RenderChunk or NULL Chunk is not found at
 *  worldspace positon x y z.
 */
RenderChunk* getRenderChunkAtWorldSpacePosition(S32 x, S32 y, S32 z, S32 *outRenderChunkIndex);

/**
 * @brief Converts a worldspace position into a local chunkspace position.
 * @param x The x coordinate in worldspace.
 * @param y The y coordinate in worldspace.
 * @param z The z coordinate in worldspace.
 * @param outLocalX The x coordinate in local space.
 * @param outLocalY The y coordinate in local space.
 * @param outLocalZ The z coordinate in local space.
 */
void worldPosToLocalPos(S32 x, S32 y, S32 z, S32 *outLocalX, S32 *outLocalY, S32 *outLocalZ);

/**
 * @brief Gets a cube at the worldspace position.
 * @param x The x coordinate in worldspace.
 * @param y The y coordinate in worldspace.
 * @param z The z coordinate in worldspace.
 * @return Returns the cube at worldspace position x y z.
 */
Cube* getCubeAtWorldSpacePosition(S32 x, S32 y, S32 z);

#endif
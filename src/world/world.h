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

#ifndef _WORLD_WORLD_H_
#define _WORLD_WORLD_H_

#include "base/types.h"
#include "math/math.h"

struct Cube;
struct Chunk;

#define CHUNK_WIDTH 16
#define MAX_CHUNK_HEIGHT 256
#define RENDER_CHUNK_HEIGHT 16
#define CHUNK_SIZE (S32)(MAX_CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_WIDTH)
#define CHUNK_SPLITS (S32)(MAX_CHUNK_HEIGHT / RENDER_CHUNK_HEIGHT)

void initWorld();

void freeWorld();

F32 getViewDistance();

bool isTransparent(Cube *cubeData, S32 x, S32 y, S32 z);

bool isTransparentAtCube(Cube *c);

void generateGeometryForRenderChunk(Chunk *chunk, S32 renderChunkId);

void generateGeometryForAllChunkSplits(Chunk *chunk);

void removeCubeAtWorldPosition(Cube *cube, S32 x, S32 y, S32 z);

void checkCubeAtLookAtCube(Vec3 cameraOrigin, Vec3 cameraDir, S32 x, S32 y, S32 z);

#endif
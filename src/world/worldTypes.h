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

#ifndef _WORLD_WORLDTYPES_H_
#define _WORLD_WORLDTYPES_H_

#include "base/types.h"
#include "render/renderTypes.h"
#include "world/world.h"

typedef struct Cube {
   U16 material : 10; // 1024 material types
   U16 light : 4;     // 0-15 light level
   U16 flag1 : 1;     // 1-bit extra flag
   U16 flag2 : 1;     // 1-bit extra flag
} Cube;

typedef struct Chunk {
   S32 startX;
   S32 startZ;
   Cube *cubeData;                         // Cube data for full chunk
   RenderChunk renderChunks[CHUNK_SPLITS]; // Per-render chunk data.
   void *graphicsData; // Per Graphics API user data.
} Chunk;


#endif
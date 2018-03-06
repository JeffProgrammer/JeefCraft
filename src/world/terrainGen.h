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

#ifndef _WORLD_TERRAINGEN_H_
#define _WORLD_TERRAINGEN_H_

#include "base/types.h"
#include "world/world.h"

void initTerrainGen();

void freeTerrainGen();

void generateWorld(S32 worldX, S32 worldZ);

void generateCavesAndStructures(S32 worldX, S32 worldZ);

void generateGeometryForRenderChunk(Chunk *chunk, S32 renderChunkId);

#endif
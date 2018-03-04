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

#ifndef _WORLD_TERRAINGENERATOR_H_
#define _WORLD_TERRAINGENERATOR_H_

#include "base/types.h"

void initTerrainGenerator();

void freeTerrainGenerator();

void generateWorld(S32 chunkX, S32 chunkZ, S32 worldX, S32 worldZ);

void generateCavesAndStructures(S32 chunkX, S32 chunkZ, S32 worldX, S32 worldZ);

#endif
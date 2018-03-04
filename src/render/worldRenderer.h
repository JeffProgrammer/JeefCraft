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

#ifndef _RENDER_WORLDRENDERER_H_
#define _RENDER_WORLDRENDERER_H_

#include "base/types.h"

struct Chunk;

void initWorldRenderer();

void freeWorldRenderer();

void renderWorld(F32 dt);

void uploadChunkToGL(Chunk *chunk);

void uploadGeometryToGL();

void uploadRenderChunkToGL(Chunk *chunk, S32 renderChunkIndex);

void freeChunkGL(Chunk *chunk);

void freeRenderChunkGL(Chunk *chunk, S32 renderChunkIndex);

#endif
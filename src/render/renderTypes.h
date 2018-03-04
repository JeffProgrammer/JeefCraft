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

#ifndef _RENDER_RENDERTYPES_H_
#define _RENDER_RENDERTYPES_H_

#include "base/types.h"
#include "math/math.h"

typedef struct GPUVertex {
   Vec4 position;
   F32 uvx;
   F32 uvy;
} GPUVertex;

typedef U32 GPUIndex;

typedef struct RenderChunk {
   GPUVertex *vertexData; /// stretchy buffer
   GPUIndex *indices;          /// stretchy buffer
   GPUIndex currentIndex;      /// Current index offset
   GPUIndex indiceCount;       /// Indice Size
   S32 vertexCount;       /// VertexData Count 

   //GLuint vbo;            /// OpenGL Vertex Buffer Object
   //GLuint ibo;            /// OpenGL Index Buffer Object
} RenderChunk;

#endif
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

#include <GL/glew.h>
#include <stretchy_buffer.h>
#include <assert.h>
#include <string.h>
#include "base/types.h"
#include "game/camera.h"
#include "graphics/shader.h"
#include "graphics/texture2d.h"
#include "math/math.h"
#include "math/frustum.h"
#include "math/aabb.h"
#include "math/screenWorld.h"
#include "platform/input.h"
#include "render/worldRenderer.h"
#include "world/cube.h"
#include "world/materials.h"
#include "world/worldTypes.h"
#include "world/util.h"
#include "world/world.h"

GLuint projMatrixLoc;
GLuint modelMatrixLoc;
GLuint textureLoc;
U32 program;
Texture2D textureAtlas;

GLuint singleBufferCubeVBO;
GLuint singleBufferCubeIBO;

int gVisibleChunks = 0;
int gTotalVisibleChunks = 0;
int gTotalChunks = 0;


S32 pickerShaderProjMatrixLoc;
S32 pickerShaderModelMatrixLoc;
GLuint pickerProgram;

// status of picking so we can't pick more than 1 in same keypress/mousepress
int pickerStatus = RELEASED;

// status of placing blocks so we can't spam place blocks
int placingStatus = RELEASED;

extern S32 worldSize;

typedef struct RenderChunkGL {
   GLuint vbo;
   GLuint ibo;
} RenderChunkGL;

typedef struct ChunkGL {
   RenderChunkGL r[CHUNK_SPLITS];
} ChunkGL;

void uploadChunkToGL(Chunk *chunk) {
   if (chunk->graphicsData)
      free(chunk->graphicsData);
   // TODO: come up with a better way for holding the vbo/ibo/etc
   chunk->graphicsData = malloc(sizeof(ChunkGL));
   memset(chunk->graphicsData, 0, sizeof(ChunkGL));

   for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
      uploadRenderChunkToGL(chunk, i);
   }
}

void uploadRenderChunkToGL(Chunk *chunk, S32 renderChunkIndex) {
   RenderChunk *r = &chunk->renderChunks[renderChunkIndex];
   RenderChunkGL *gl = &((ChunkGL*)chunk->graphicsData)->r[renderChunkIndex];
   memset(gl, 0, sizeof(RenderChunkGL));

   if (r->vertexCount > 0) {
      glGenBuffers(1, &gl->vbo);
      glGenBuffers(1, &gl->ibo);

      glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GPUVertex) * r->vertexCount, r->vertexData, GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ibo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GPUIndex) * r->indiceCount, r->indices, GL_STATIC_DRAW);
   }

   // Free right after uploading to the GL. We don't need gpu data
   // in both system and gpu ram.
   sb_free(r->vertexData);
   sb_free(r->indices);
}

void freeRenderChunkGL(Chunk *chunk, S32 renderChunkIndex) {
   RenderChunk *r = &chunk->renderChunks[renderChunkIndex];
   RenderChunkGL *gl = &((ChunkGL*)chunk->graphicsData)->r[renderChunkIndex];

   if (r->vertexCount > 0) {
      glDeleteBuffers(1, &gl->vbo);
      glDeleteBuffers(1, &gl->ibo);
   }
   memset(r, 0, sizeof(RenderChunk));
   memset(gl, 0, sizeof(RenderChunkGL));
}

void freeChunkGL(Chunk *chunk) {
   for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
      freeRenderChunkGL(chunk, i);
   }
   free(chunk->graphicsData);
   chunk->graphicsData = NULL;
}

void uploadGeometryToGL() {
   // *Single threaded*
   //
   // Upload to the GL
   // Now that we generated all of the geometry, upload to the GL.
   // Note if a chunk has no geometry we don't create a vbo
   //
   // TODO: use VAO if extension is supported??
   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         uploadChunkToGL(getChunkAt(x, z));
      }
   }

   // Single buffer cube vbo/ibo
   glGenBuffers(1, &singleBufferCubeVBO);
   glBindBuffer(GL_ARRAY_BUFFER, singleBufferCubeVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(F32) * 6 * 4 * 4, cubes, GL_STATIC_DRAW);

   glGenBuffers(1, &singleBufferCubeIBO);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, singleBufferCubeIBO);

   GPUIndex indices[36];
   S32 in = 0;
   for (S32 i = 0; i < 36; i += 6) {
      indices[i] = in;
      indices[i + 1] = in + 2;
      indices[i + 2] = in + 1;
      indices[i + 3] = in;
      indices[i + 4] = in + 3;
      indices[i + 5] = in + 2;

      in += 4;
   }
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GPUIndex) * 36, indices, GL_STATIC_DRAW);
}

void initWorldRenderer() {
   // Only 2 mip levels.
   bool ret = createTexture2D("Assets/block_atlas.png", 4, 2, &textureAtlas);
   if (!ret) {
      exit(-3);
   }

   // Create shader
   generateShaderProgram("Shaders/basic.vert", "Shaders/basic.frag", &program);
   projMatrixLoc = glGetUniformLocation(program, "projViewMatrix");
   modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
   textureLoc = glGetUniformLocation(program, "textureAtlas");

   // Create shader for picker
   generateShaderProgram("Shaders/red.vert", "Shaders/red.frag", &pickerProgram);
   pickerShaderProjMatrixLoc = glGetUniformLocation(pickerProgram, "projViewMatrix");
   pickerShaderModelMatrixLoc = glGetUniformLocation(pickerProgram, "modelMatrix");

   gTotalChunks = worldSize * 2 * worldSize * 2 * CHUNK_SPLITS;
}

void freeWorldRenderer() {

}

bool orthoFlag = false;

void renderWorld(F32 delta) {
   // Set GL State
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   glUseProgram(program);

   // proj/view matrix
   mat4 proj, view, projView;

   if (inputGetKeyStatus(KEY_V) == PRESSED)
      orthoFlag = true;
   else
      orthoFlag = false;

   // For debugging culling
   if (!orthoFlag) {
      getCurrentProjMatrix(&proj);
      getCurrentViewMatrix(&view);
   }
   else {
      glm_ortho(-256.0f, 256.0f, -256.0f / (1440.f / 900.f), 256.0f / (1440.f / 900.f), -200.0f, 200.0f, proj);
      Vec3 eye = create_vec3(0.0f, 0.0f, 0.0f);
      Vec3 center = create_vec3(0.0f, -1.0f, 0.0f);
      Vec3 up = create_vec3(1.0f, 0.0f, 0.0f);
      glm_lookat(eye.vec, center.vec, up.vec, view);
   }
   glm_mat4_mul(proj, view, projView);
   glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &(projView[0][0]));

   // Bind our texture atlas to texture unit 0
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, textureAtlas.glId);
   glUniform1i(textureLoc, 0);

   gVisibleChunks = 0;
   gTotalVisibleChunks = 0;

   Frustum frustum;
   getCameraFrustum(&frustum);

   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         Chunk *c = getChunkAt(x, z);
         for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
            if (c->renderChunks[i].vertexCount > 0) {
               RenderChunkGL *gl = &((ChunkGL*)c->graphicsData)->r[i];

               gTotalVisibleChunks++;

               // Set position.
               // Center y pos should actually be RENDER_CHUNK_HEIGHT * i
               // but pos should always be 0 for y since the pos is baked into the y coord.
               Vec3 pos = create_vec3(x * CHUNK_WIDTH, 0, z * CHUNK_WIDTH);
               Vec3 center;
               Vec3 halfExtents = create_vec3(CHUNK_WIDTH / 2.0f, RENDER_CHUNK_HEIGHT / 2.0f, CHUNK_WIDTH / 2.0f);
               glm_vec_add(pos.vec, halfExtents.vec, center.vec);
               center.y += (F32)(i * RENDER_CHUNK_HEIGHT); // We add since we already have RENDER_CHUNK_HEIGHT / 2.0

               if (FrustumCullSquareBox(&frustum, center, CHUNK_WIDTH / 2.0f)) {
                  mat4 modelMatrix;
                  glm_mat4_identity(modelMatrix);
                  glm_translate(modelMatrix, pos.vec);
                  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &(modelMatrix[0][0]));
                  glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
                  glEnableVertexAttribArray(0);
                  glEnableVertexAttribArray(1);
                  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GPUVertex), (void*)offsetof(GPUVertex, position));
                  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GPUVertex), (void*)offsetof(GPUVertex, uvx));
                  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ibo);
                  glDrawElements(GL_TRIANGLES, (GLsizei)c->renderChunks[i].indiceCount, GL_UNSIGNED_INT, (void*)0);
                  glDisableVertexAttribArray(0);
                  glDisableVertexAttribArray(1);

                  gVisibleChunks++;
               }
            }
         }
      }
   }

   return;

   // Do our raycast to screen world.
   Vec3 rayOrigin;
   Vec4 rayDir;
   screenRayToWorld(view, &rayOrigin, &rayDir);

   // Check to see if we have something within 8 blocks away.
   Vec3 point = rayOrigin;
   Vec3 scalar;
   glm_vec_scale(rayDir.vec, 0.01f, scalar.vec);
   for (S32 i = 0; i < 400; ++i) {
      glm_vec_add(point.vec, scalar.vec, point.vec);

      Vec3 pos = create_vec3(floorf(point.x), floorf(point.y), floorf(point.z));

      // Calculate chunk at point.
      Cube *c = getCubeAtWorldSpacePosition((S32)pos.x, (S32)pos.y, (S32)pos.z);
      if (c != NULL && c->material != Material_Air) {
         glUseProgram(pickerProgram);

         glUniformMatrix4fv(pickerShaderProjMatrixLoc, 1, GL_FALSE, &(projView[0][0]));
         mat4 modelMatrix;
         glm_mat4_identity(modelMatrix);
         glm_translate(modelMatrix, pos.vec);

         Vec3 scale = create_vec3(1.2f, 1.2f, 1.2f);
         Vec3 trans = create_vec3(-0.1f, -0.1f, -0.1f);
         glm_scale(modelMatrix, scale.vec);
         glm_translate(modelMatrix, trans.vec);

         glUniformMatrix4fv(pickerShaderModelMatrixLoc, 1, GL_FALSE, &(modelMatrix[0][0]));

         glBindBuffer(GL_ARRAY_BUFFER, singleBufferCubeVBO);
         glEnableVertexAttribArray(0);
         glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(F32) * 4, (void*)0);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, singleBufferCubeIBO);
         glDrawElements(GL_TRIANGLES, (GLsizei)36, GL_UNSIGNED_INT, (void*)0);
         glDisableVertexAttribArray(0);

         // TODO: Have mouse click. For now hit the G key.
         if (pickerStatus == RELEASED && inputGetKeyStatus(KEY_G) == PRESSED) {
            removeCubeAtWorldPosition(c, (S32)pos.x, (S32)pos.y, (S32)pos.z);
            pickerStatus = PRESSED;
         }
         else if (inputGetKeyStatus(KEY_G) == RELEASED) {
            // set picker status to released
            pickerStatus = RELEASED;
         }

         if (placingStatus == RELEASED && inputGetKeyStatus(KEY_H) == PRESSED) {
            Vec3 rayDirection = create_vec3(rayDir.x, rayDir.y, rayDir.z);
            checkCubeAtLookAtCube(rayOrigin, rayDirection, (S32)pos.x, (S32)pos.y, (S32)pos.z);
            placingStatus = PRESSED;
         }
         else if (inputGetKeyStatus(KEY_H) == RELEASED) {
            placingStatus = RELEASED;
         }

         break;
      }
   }
}
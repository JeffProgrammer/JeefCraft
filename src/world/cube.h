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

#ifndef _WORLD_CUBE_H_
#define _WORLD_CUBE_H_

#include "base/types.h"

// Taken from std_voxel_render.h, from the public domain
static F32 cubes[6][4][4] = {
   { { 1,0,1,0 },{ 1,1,1,0 },{ 1,1,0,0 },{ 1,0,0,0 } }, // east
   { { 1,1,1,1 },{ 0,1,1,1 },{ 0,1,0,1 },{ 1,1,0,1 } }, // up
   { { 0,1,1,2 },{ 0,0,1,2 },{ 0,0,0,2 },{ 0,1,0,2 } }, // west
   { { 0,0,1,3 },{ 1,0,1,3 },{ 1,0,0,3 },{ 0,0,0,3 } }, // down
   { { 0,1,1,4 },{ 1,1,1,4 },{ 1,0,1,4 },{ 0,0,1,4 } }, // north
   { { 0,0,0,5 },{ 1,0,0,5 },{ 1,1,0,5 },{ 0,1,0,5 } }, // south
};

static F32 cubeUVs[6][4][2] = {
   { { 0, 1 },{ 0, 0 },{ 1, 0 },{ 1, 1 } }, // East
   { { 1, 1 },{ 0, 1 },{ 0, 0 },{ 1, 0 } }, // up
   { { 1, 0 },{ 1, 1 },{ 0, 1 },{ 0, 0 } }, // west
   { { 0, 1 },{ 1, 1 },{ 1, 0 },{ 0, 0 } }, // down
   { { 0, 0 },{ 1, 0 },{ 1, 1 },{ 0, 1 } }, // north
   { { 1, 1 },{ 0, 1 },{ 0, 0 },{ 1, 0 } }  // south
};

static float cubeNormals[6][3] = {
   { 1.0f,0.0f,0.0f },  // East
   { 0.0f,1.0f,0.0f },  // Up
   { -1.0f,0.0f,0.0f }, // West
   { 0.0f,-1.0f,0.0f }, // Down
   { 0.0f,0.0f,1.0f },  // North
   { 0.0f,0.0f,-1.0f } // South
};

typedef enum CubeSides {
   CubeSides_East,
   CubeSides_Up,
   CubeSides_West,
   CubeSides_Down,
   CubeSides_North,
   CubeSides_South,
} CubeSides;

#endif
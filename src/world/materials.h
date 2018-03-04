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

#ifndef _WORLD_MATERIAL_H_
#define _WORLD_MATERIAL_H_

typedef enum Materials {
   Material_Air,
   Material_Bedrock,
   Material_Dirt,
   Material_Grass,      // Also note that bottoms of grass have dirt blocks.
   Material_Grass_Side, // Sides of grass have a special texture.
   Material_Wood_Trunk,
   Material_Leaves
} Material;

// How many textures go across in the main texture atlas row
#define TEXTURE_ATLAS_COUNT_I 32
#define TEXTURE_ATLAS_COUNT_F 32.0f

#endif
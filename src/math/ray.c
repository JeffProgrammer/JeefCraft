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

#include "math/ray.h"

bool rayIntersectsPlane(Vec3 rayOrigin, Vec3 rayDir, Vec4 plane, Vec3 *outPos) {
   // t = -(ray_origin dot plane_normal + distance_from_plane) / (ray_dir dot normal)
   // point_of_intersection = ray_origin + (t * ray_dir)

   Vec3 normal = create_vec3(plane.x, plane.y, plane.z);
   F32 rayNormal = glm_vec_dot(rayDir.vec, normal.vec);

   // No divide by 0
   if (isFloatZero(rayNormal))
      return false;

   Vec3 center;
   glm_vec_scale(normal.vec, plane.w, center.vec);

   Vec3 diff;
   glm_vec_sub(center.vec, rayOrigin.vec, diff.vec);

   F32 t = /*-*/(glm_vec_dot(diff.vec, normal.vec)/* + plane.w*/) / rayNormal;
   if (isFloatZero(t))
      return false;

   // Get point of intersection
   Vec3 scale;
   glm_vec_scale(rayDir.vec, t, scale.vec);
   glm_vec_add(rayOrigin.vec, scale.vec, outPos->vec);
   return true;
}
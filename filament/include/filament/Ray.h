/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//! \file

#ifndef TNT_FILAMENT_RAY_H
#define TNT_FILAMENT_RAY_H

#include <utils/compiler.h>
#include <utils/EntityManager.h>

#include <math/vec3.h>

#include <array>

namespace filament {

struct UTILS_PUBLIC Intersection {

	// same as entity id
	using Type = uint32_t;

	Type id = 0;
	math::float3 position = std::numeric_limits<float>::max();
	
	// possibly get renderable id
	inline constexpr bool is_valid() {
		return id != 0;
	}

	inline void set_position(const math::float3& pos) {
		position = pos;
	}
};

struct UTILS_PUBLIC Ray {

	math::float3 origin = std::numeric_limits<float>::max();
	math::float3 direction = std::numeric_limits<float>::max();
	float time = 0;

	//
	// ray - triangle intersection
	// from https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html
	inline bool intersect(const std::array<math::float3, 3>& v, math::float3& p) {
		
		auto const v0v1 = v[0] - v[1];
		auto const v0v2 = v[2] - v[0];
		auto const n = cross(v0v1, v0v2);

		// check parallel
		if(dot(n, direction) < std::numeric_limits<float>::epsilon()) 
			return false;

		p = origin + time * direction;

		//TODO currrently no optimized
		auto const e0 = v[1] - v[0];
		auto const e1 = v[2] - v[1];
		auto const e2 = v[0] - v[1];

		auto c = cross(e0, p - v[0]);
		if(dot(n, c) < 0) return false;

		c = cross(e1, p - v[1]);
		if(dot(n, c) < 0) return false;

		c = cross(e1, p - v[2]);
		if(dot(n, c) < 0) return false;

		return true;
	}
};

} // namespace filament

#endif // TNT_FILAMENT_BOX_H

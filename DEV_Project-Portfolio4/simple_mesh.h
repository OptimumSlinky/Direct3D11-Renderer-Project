#pragma once

#include <array>

namespace end
{
	class alignas(8) float2 : std::array<float, 2> {};

	class alignas(16) float4 : std::array<float, 4> {};

	struct simple_vert
	{
		float4 pos;
		float4 norm;
		float4 color;
		float2 tex_coord;
	};

	struct simple_mesh
	{
		uint32_t vert_count = 0;
		uint32_t index_count = 0;
		simple_vert* verts = nullptr;
		uint32_t* indices = nullptr;
	};
}

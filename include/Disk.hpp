#pragma once
#include <stdint.h>
#include <limits>
#include <vector>
#include "meshgrid.hpp"

using uint = uint32_t;
using int32 = int32_t;



class Disk
{
private:
	constexpr static float def_cen[3]{ 0.f }; // for default value pass to constructor - argument center 
	constexpr static float PI = 3.14159265359f;
private:
	float cen[3];
	float rad;
	bool set_hole = false;
	float hole_rad;
public:
	Disk(const float radius, const float center[3] = def_cen) : rad(radius), hole_rad(0.f)
	{
		cen[0] = center[0];
		cen[1] = center[1];
		cen[2] = center[2];
	}

	bool generate_mesh(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices) {
		set_hole = false;
		return generate_mesh_(max_size_vert, vertices, indices);
	}

	bool generate_mesh_hole(const float max_size_vert, const float hole_radius, std::vector<float>& vertices, std::vector<uint>& indices) {
		set_hole = true;
		hole_rad = hole_radius;
		return generate_mesh_(max_size_vert, vertices, indices);
	}

protected:
	bool generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices) {

		if (rad <= 0.f)
			return false;

		vertices.clear();

		indices.clear();

		/*
			prepare mesh
		*/
		auto r = meshgen::linspace(0.f, rad, max_size_vert); // min_size = 0.f, max_size = 100.f, 
		auto theta = meshgen::linspace(0.f, 2 * PI, max_size_vert);
		auto mesh_pair = meshgen::meshgrid(r, theta);

		// Convert to rectangular coordinates
		auto R = std::get<0>(mesh_pair);
		auto THETA = std::get<1>(mesh_pair);
		size_t grid_size = R.size();
		std::vector<float> x_grid;
		std::vector<float> z_grid;

		for (int i = 0; i < grid_size; ++i) {
			for (int j = 0; j < grid_size; ++j) {
				x_grid.push_back(R(i, j) * cos(THETA(i, j)));
				z_grid.push_back(R(i, j) * sin(THETA(i, j)));
			}
		}


		for (int i = 0; i < grid_size; ++i) {
			for (int j = 0; j < grid_size; ++j) {
				auto x = x_grid[j + i * grid_size];
				auto z = z_grid[j + i * grid_size];
				auto y = 0;
				if (set_hole) {
					auto skip = is_hole(x, z, hole_rad);
					if (skip)
						continue;
				}
				vertices.push_back(x + cen[0]);
				vertices.push_back(y);
				vertices.push_back(z + cen[2]);
			}
		}
		int32 last_vert = vertices.size() / 3;
		int32 vert_size = vertices.size() / 3;
		

		/*
			build indices
		*/
		bool oddRow = false;
		uint y = 0;
		while (vert_size > 0) {
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (uint x = 0; x <= grid_size; ++x)
				{
					vert_size--;
					auto current = y * grid_size + x;
					auto next = (y + 1) * grid_size + x;
					if (set_hole && next >= last_vert)
						continue;
					indices.push_back(current);
					indices.push_back(next);
					if (vert_size <= 0)
						break;
				}
			}
			else
			{
				for (int x = grid_size; x >= 0; --x)
				{
					vert_size--;
					auto current = (y + 1) * grid_size + x;
					auto prev = y * grid_size + x;
					if (set_hole && current >= last_vert)
						continue;
					indices.push_back(current);
					indices.push_back(prev);
					if (vert_size <= 0)
						break;
				}
			}
			oddRow = !oddRow;
			y++;
		}
		
		return true;
	}

private:
		bool is_hole(const float x, const float z, const float hole_radius) {
			auto r1 = pow((x - cen[0]), 2);
			auto r2 = pow((z - cen[2]), 2);
			auto skip = ((r1 + r2) <= pow(hole_radius, 2));
			return skip;
		}
};
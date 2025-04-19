#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <vector>
#include <cstdint>
#include "matrix.hpp"

struct Vertex {
    Vec3 pos;
    Vec3 color;
    Vec2 texCoord;
};

bool loadOBJ(const char* path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

#endif
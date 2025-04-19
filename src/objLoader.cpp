#include "objLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>

struct PackedVertex {
    Vec3 pos;
    Vec3 normal;
    Vec2 texCoord;
    bool operator==(const PackedVertex& other) const {
        return pos == other.pos && texCoord == other.texCoord && normal == other.normal;
    }
};

namespace std {
    template<> struct hash<PackedVertex> {
        size_t operator()(PackedVertex const& packed) const {
            auto h1 = hash<float>()(packed.pos.x) ^ (hash<float>()(packed.pos.y)<<1) ^ (hash<float>()(packed.pos.z)<<2);
            auto h2 = hash<float>()(packed.normal.x) ^ (hash<float>()(packed.normal.y)<<1) ^ (hash<float>()(packed.normal.z)<<2);
            auto h3 = hash<float>()(packed.texCoord.x) ^ (hash<float>()(packed.texCoord.y)<<1);
            return h1 ^ h2 ^ h3;
        }
    };
}

bool loadOBJ(const char* path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    std::vector<Vec3> temp_pos;
    std::vector<Vec2> temp_uv;
    std::vector<Vec3> temp_norm;
    std::vector<uint32_t> posIdx, uvIdx, normIdx;

    std::ifstream in(path);
    if (!in.is_open()) { std::cerr << "Cannot open " << path << "\n"; return false; }
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            Vec3 v; iss >> v.x >> v.y >> v.z; temp_pos.push_back(v);
        } else if (prefix == "vt") {
            Vec2 uv; iss >> uv.x >> uv.y; temp_uv.push_back(uv);
        } else if (prefix == "vn") {
            Vec3 n; iss >> n.x >> n.y >> n.z; temp_norm.push_back(n);
        } else if (prefix == "f") {
            for (int i=0;i<3;i++) {
                std::string vert; iss >> vert;
                std::replace(vert.begin(), vert.end(), '/', ' ');
                std::istringstream viss(vert);
                uint32_t v, t, n; viss >> v >> t >> n;
                posIdx.push_back(v-1);
                uvIdx.push_back(t-1);
                normIdx.push_back(n-1);
            }
        }
    }

    std::unordered_map<PackedVertex, uint32_t> vertexMap;
    for (size_t i=0;i<posIdx.size();i++) {
        PackedVertex packed;
        packed.pos = temp_pos[posIdx[i]];
        packed.texCoord = temp_uv[uvIdx[i]];
        packed.normal = temp_norm[normIdx[i]];
        uint32_t index;
        auto it = vertexMap.find(packed);
        if (it != vertexMap.end()) {
            index = it->second;
        } else {
            Vertex vert;
            vert.pos = packed.pos;
            vert.texCoord = packed.texCoord;
            vert.color = { static_cast<float>(rand())/RAND_MAX,
                           static_cast<float>(rand())/RAND_MAX,
                           static_cast<float>(rand())/RAND_MAX };
            index = vertices.size();
            vertices.push_back(vert);
            vertexMap[packed] = index;
        }
        indices.push_back(index);
    }
    return true;
}
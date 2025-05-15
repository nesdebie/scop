/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:31:13 by nesdebie          #+#    #+#             */
/*   Updated: 2025/05/15 10:08:09 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj_loader.h"


bool operator==(const Vertex& a, const Vertex& b) {
    return a.position == b.position &&
           a.normal == b.normal &&
           a.texCoord == b.texCoord;
}

struct VertexHash {
    size_t operator()(const Vertex& v) const {
        return ((std::hash<float>()(v.position.x) ^
                (std::hash<float>()(v.position.y) << 1)) >> 1) ^
               (std::hash<float>()(v.position.z) << 1) ^
               (std::hash<float>()(v.texCoord.x) << 1) ^
               (std::hash<float>()(v.texCoord.y) << 1);
    }
};

static bool ends_with(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size()-suffix.size(), suffix.size(), suffix) == 0;
}

bool loadOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::string& textureFile) {
    if (filename.empty() || ends_with(filename, ".obj") == false || ends_with(filename, "/.obj") == true || (filename.size() == 4 && ends_with(filename, ".obj") == true)) {
        std::cerr << "Filename is invalid." << std::endl;
        return false;
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return false;
    }
    std::string mtlFilename;
    textureFile.clear();

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texcoords;
    std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        } else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            uv.y = 1.0f - uv.y;  // Flip V for Vulkan
            temp_texcoords.push_back(uv);
        } else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        } else if (prefix == "f") {
            std::vector<uint32_t> faceIndices;
        
            std::string vertexString;
            while (iss >> vertexString) {
                std::istringstream vss(vertexString);
                std::string idxPos, idxTex, idxNorm;
                std::getline(vss, idxPos, '/');
                std::getline(vss, idxTex, '/');
                std::getline(vss, idxNorm, '/');
        
                int posIdx = idxPos.empty() ? -1 : std::stoi(idxPos) - 1;
                int texIdx = idxTex.empty() ? -1 : std::stoi(idxTex) - 1;
                int normIdx = idxNorm.empty() ? -1 : std::stoi(idxNorm) - 1;
        
                if (posIdx < 0 || posIdx >= (int)temp_positions.size()) continue;
        
                Vertex vertex{};
                vertex.position = temp_positions[posIdx];
                vertex.normal = (normIdx >= 0 && normIdx < (int)temp_normals.size()) ? temp_normals[normIdx] : glm::vec3(0.0f, 0.0f, 1.0f);
                vertex.texCoord = (texIdx >= 0 && texIdx < (int)temp_texcoords.size()) ? temp_texcoords[texIdx] : glm::vec2(0.0f);
        
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                faceIndices.push_back(uniqueVertices[vertex]);
            }
        
            // Triangulate face (fan from vertex 0)
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i + 1]);
            }
        } else if (prefix == "mtllib") {
            iss >> mtlFilename;
        
            std::ifstream mtlFile("models/" + mtlFilename);
            if (!mtlFile.is_open() && ends_with(mtlFilename, ".mtl")) {
                mtlFile.open("models/mtl/" + mtlFilename);
                if (!mtlFile.is_open() && ends_with(mtlFilename, ".mtl")) {
                    std::cerr << "Failed to open MTL file: " << mtlFilename << std::endl;
                    return false;
                }
            }
        
            std::string mtlLine;
            while (std::getline(mtlFile, mtlLine)) {
                std::istringstream mtlIss(mtlLine);
                std::string mtlPrefix;
                mtlIss >> mtlPrefix;
        
                if (mtlPrefix == "map_Kd") {
                    std::string texCandidate;
                    mtlIss >> texCandidate;
        
                    // Try models/ first
                    std::ifstream texFile("models/" + texCandidate);
                    if (texFile.good() && ends_with(texCandidate, ".png")) {
                
                        textureFile = texCandidate;
                        break;
                    }
        
                    // Then models/tex/
                    texFile.open("models/tex/" + texCandidate);
                    if (texFile.good() && ends_with(texCandidate, ".png")) {
                        textureFile = "tex/" + texCandidate;
                        break;
                    }
        
                    std::cerr << "Warning: Texture not found in models/ or models/tex/: " << texCandidate << std::endl;
                }
            }
        }
        
        
    }
    return true;
}
    

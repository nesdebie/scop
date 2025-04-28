/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:31:13 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/28 11:22:23 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

struct VertexHash {
    size_t operator()(const Vertex& v) const {
        return ((std::hash<float>()(v.position.x) ^
                (std::hash<float>()(v.position.y) << 1)) >> 1) ^
               (std::hash<float>()(v.position.z) << 1);
    }
};

bool operator==(const Vertex& a, const Vertex& b) {
    return a.position == b.position && a.normal == b.normal;
}

bool loadOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;
    std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices;

    std::string line;
    while (std::getline(file, line)) {
        if (file.eof())
            break;
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
    
        if (prefix.empty())
            continue; // Skip if no prefix was read
    
        if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        } else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        } else if (prefix == "f") {
            std::string vertexString;
            int verticesInFace = 0;
            while (iss >> vertexString) {
                verticesInFace++;
        
                std::istringstream vss(vertexString);
                std::string idxPos, idxTex, idxNorm;
                std::getline(vss, idxPos, '/');
                std::getline(vss, idxTex, '/');
                std::getline(vss, idxNorm, '/');
        
                if (idxPos.empty())
                    continue; // No vertex position? skip
        
                int posIdx = -1;
                try {
                    posIdx = std::stoi(idxPos) - 1;
                } catch (...) {
                    std::cerr << "Invalid position index: " << idxPos << std::endl;
                    continue;
                }
        
                int normIdx = -1;
                if (!idxNorm.empty()) {
                    try {
                        normIdx = std::stoi(idxNorm) - 1;
                    } catch (...) {
                        std::cerr << "Invalid normal index: " << idxNorm << std::endl;
                        normIdx = -1;
                    }
                }
        
                if (posIdx < 0 || posIdx >= (int)temp_positions.size()) {
                    std::cerr << "Position index out of bounds: " << posIdx << std::endl;
                    continue;
                }
        
                Vertex vertex = {};
                vertex.position = temp_positions[posIdx];
                vertex.normal = (normIdx >= 0 && normIdx < (int)temp_normals.size()) ?
                                 temp_normals[normIdx] : glm::vec3(0.0f, 0.0f, 1.0f);
        
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        
            if (verticesInFace != 3) {
                std::cerr << "Warning: Non-triangle face detected, vertices: " << verticesInFace << std::endl;
            }
        }
    }
    return true;
}
    

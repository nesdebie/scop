/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 08:19:38 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/16 11:34:47 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Model.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

bool Model::loadFromOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<glm::vec3> temp_normals;

    std::string currentMaterial; // To track the current material
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
            std::cout << "Vertex: " << vertex.x << ", " << vertex.y << ", " << vertex.z << std::endl;
        } else if (prefix == "vt") {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            temp_texCoords.push_back(uv);
            std::cout << "Texture Coord: " << uv.x << ", " << uv.y << std::endl;
        } else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
            std::cout << "Normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
        } else if (prefix == "f") {
            unsigned int vi[3], ti[3], ni[3];
            char slash;
            for (int i = 0; i < 3; ++i) {
                ss >> vi[i] >> slash >> ti[i] >> slash >> ni[i];
                if (vi[i] - 1 >= temp_vertices.size() || 
                    ti[i] - 1 >= temp_texCoords.size() || 
                    ni[i] - 1 >= temp_normals.size()) {
                    std::cerr << "Error: Invalid face indices in file: " << path << std::endl;
                    return false;
                }
        
                vertices.push_back(temp_vertices[vi[i] - 1]);
                texCoords.push_back(temp_texCoords[ti[i] - 1]);
                normals.push_back(temp_normals[ni[i] - 1]);
                indices.push_back(indices.size());
            }
            std::cout << "Face parsed successfully with material: " << currentMaterial << std::endl;
        } else if (prefix == "usemtl") {
            ss >> currentMaterial;
            std::cout << "Using material: " << currentMaterial << std::endl;
        } else if (prefix == "mtllib") {
            std::string materialFile;
            ss >> materialFile;
            std::cout << "Material library: " << materialFile << std::endl;
        } else if (prefix == "s") {
            std::string shading;
            ss >> shading;
            if (shading == "off") {
                std::cout << "Smoothing group: off" << std::endl;
            } else {
                std::cout << "Smoothing group: " << shading << std::endl;
            }
        } else {
            std::cout << "Skipping unrecognized line: " << line << std::endl;
        }
    }

    return true;
}

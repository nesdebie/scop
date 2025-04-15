/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:44:32 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/15 20:44:33 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj_loader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

bool loadOBJ(const std::string& path,
             std::vector<Vertex>& outVertices,
             std::vector<unsigned int>& outIndices)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << "\n";
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> vertexIndices, normalIndices;

    std::string line;
    while (getline(file, line)) {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            glm::vec3 v;
            ss >> v.x >> v.y >> v.z;
            positions.push_back(v);
        } else if (type == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        } else if (type == "f") {
            unsigned int p[3], n[3];
            char dummy;
            for (int i = 0; i < 3; ++i) {
                ss >> p[i] >> dummy >> dummy >> n[i];
                vertexIndices.push_back(p[i] - 1);
                normalIndices.push_back(n[i] - 1);
            }
        }
    }

    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        Vertex vert;
        vert.position = positions[vertexIndices[i]];
        vert.normal = normals[normalIndices[i]];
        outVertices.push_back(vert);
        outIndices.push_back(i);
    }

    return true;
}

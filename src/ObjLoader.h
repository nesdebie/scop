/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ObjLoader.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:30:14 by nesdebie          #+#    #+#             */
/*   Updated: 2025/06/16 09:33:14 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJ_LOADER_H
# define OBJ_LOADER_H
# include "vertex.h"

# include <vector>
# include <string>
# include <fstream>
# include <sstream>
# include <iostream>
# include <unordered_map>
# include <map>

struct SubMesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string textureFile;
    my_glm::vec3 diffuseColor = my_glm::vec3(1.0f);
    my_glm::vec3 ambientColor = my_glm::vec3(1.0f);
    my_glm::vec3 specularColor = my_glm::vec3(0.0f);
    my_glm::vec3 emissiveColor = my_glm::vec3(0.0f);
    float specularExponent = 1.0f;
    float dissolve = 1.0f;
    float refractionIndex = 1.0f;
    int illumModel = 2;
    bool hasMapKdInitially = false;
};


bool loadOBJ(const std::string& filename, std::vector<SubMesh>& submeshes);

#endif

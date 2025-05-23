/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ObjLoader.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:30:14 by nesdebie          #+#    #+#             */
/*   Updated: 2025/05/23 11:21:40 by nesdebie         ###   ########.fr       */
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
};

bool loadOBJ(const std::string& filename, std::vector<SubMesh>& submeshes);

#endif

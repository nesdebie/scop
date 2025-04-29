/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_loader.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:30:14 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/29 10:22:01 by nesdebie         ###   ########.fr       */
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

bool loadOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_loader.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:30:14 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/28 08:42:00 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "vertex.h"
#include <vector>
#include <string>

bool loadOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:45:30 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/15 20:45:32 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <GLFW/glfw3.h>

GLuint loadShader(const std::string& vertexPath, const std::string& fragmentPath);

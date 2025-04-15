/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bmp_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:49:20 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/15 20:49:47 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bmp_loader.hpp"
#include <fstream>
#include <iostream>

unsigned char* loadBMP(const char* filename, int& width, int& height) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return nullptr;

    unsigned char header[54];
    file.read((char*)header, 54);
    width = *(int*)&header[18];
    height = *(int*)&header[22];
    int offset = *(int*)&header[10];

    int imageSize = 3 * width * height;
    unsigned char* data = new unsigned char[imageSize];

    file.seekg(offset, std::ios::beg);
    file.read((char*)data, imageSize);
    return data;
}

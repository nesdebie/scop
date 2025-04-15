/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix_map.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:48:14 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/15 20:48:16 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cmath>

struct Mat4 {
    float data[16];
};

inline Mat4 identity() {
    Mat4 mat = {};
    for (int i = 0; i < 4; i++) mat.data[i * 5] = 1.0f;
    return mat;
}

inline Mat4 perspective(float fov, float aspect, float near, float far) {
    float tanHalfFOV = tanf(fov / 2.0f);
    Mat4 mat = {};
    mat.data[0] = 1.0f / (aspect * tanHalfFOV);
    mat.data[5] = 1.0f / tanHalfFOV;
    mat.data[10] = -(far + near) / (far - near);
    mat.data[11] = -1.0f;
    mat.data[14] = -(2.0f * far * near) / (far - near);
    return mat;
}

inline Mat4 translate(const Mat4& m, float x, float y, float z) {
    Mat4 res = m;
    res.data[12] += x;
    res.data[13] += y;
    res.data[14] += z;
    return res;
}

inline Mat4 rotateY(const Mat4& m, float angle) {
    Mat4 res = identity();
    float c = cos(angle), s = sin(angle);
    res.data[0] = c;
    res.data[2] = s;
    res.data[8] = -s;
    res.data[10] = c;

    // Multiply with m
    Mat4 out = {};
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            for (int i = 0; i < 4; i++)
                out.data[col + row * 4] += m.data[i + row * 4] * res.data[col + i * 4];
    return out;
}

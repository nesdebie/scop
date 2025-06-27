/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_glm.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 09:33:55 by nesdebie          #+#    #+#             */
/*   Updated: 2025/06/27 09:36:05 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MY_GLM_H
#define MY_GLM_H

#include <cmath>
#include <cassert>
#include <algorithm>
#include <cfloat>

namespace my_glm {

    /* ----- VEC 2 ----- */
    struct vec2 {
        float x = 0, y = 0;

        constexpr vec2() = default;
        constexpr vec2(float x_, float y_) : x(x_), y(y_) {}
        bool operator==(const vec2& o) const { return x == o.x && y == o.y; }
    };

    /* ----- VEC 3 ----- */
    struct vec3 {
        float x = 0, y = 0, z = 0;

        constexpr vec3() = default;
        constexpr vec3(float f) : x(f), y(f), z(f) {}
        constexpr vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

        vec3 operator+(const vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
        vec3 operator-(const vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
        vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
        vec3 operator/(float s) const { return {x / s, y / s, z / s}; }

        vec3& operator+=(const vec3& o) {
            x += o.x; y += o.y; z += o.z;
            return *this;
        }

        
        friend vec3 operator*(float s, const vec3& v) { return v * s; }

        bool operator==(const vec3& o) const { return x == o.x && y == o.y && z == o.z; }
    };

    inline float length(const vec3& v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    inline vec3 normalize(const vec3& v) {
        float len = length(v);
        assert(len != 0);
        return v / len;
    }

    inline float dot(const vec3& a, const vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline vec3 cross(const vec3& a, const vec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
    inline vec3 min(const vec3& a, const vec3& b) {
        return { std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) };
    }

    inline vec3 max(const vec3& a, const vec3& b) {
        return { std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) };
    }

    /* ----- VEC 4 ----- */
    struct vec4 {
        float x = 0, y = 0, z = 0, w = 0;

        constexpr vec4() = default;
        constexpr vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
        constexpr vec4(const vec3& v, float w_) : x(v.x), y(v.y), z(v.z), w(w_) {}
    };

    /* ----- MAT 4 ----- */
    struct mat4 {
        float data[16] = {0};

        constexpr mat4() = default;
        constexpr mat4(float diag) {
            for (int i = 0; i < 16; ++i) data[i] = 0;
            data[0] = data[5] = data[10] = data[15] = diag;
        }

        float* operator[](int row) { return &data[row * 4]; }
        const float* operator[](int row) const { return &data[row * 4]; }

        static mat4 identity() { return mat4(1.0f); }
    };

    inline mat4 translate(const mat4& m, const vec3& v) {
        mat4 result = m;
        result.data[12] += v.x;
        result.data[13] += v.y;
        result.data[14] += v.z;
        return result;
    }

    inline mat4 perspective(float fovY, float aspect, float zNear, float zFar) {
        mat4 m;
        float tanHalfFovy = std::tan(fovY / 2);
        m.data[0] = 1 / (aspect * tanHalfFovy);
        m.data[5] = 1 / tanHalfFovy;
        m.data[10] = -(zFar + zNear) / (zFar - zNear);
        m.data[11] = -1;
        m.data[14] = -(2 * zFar * zNear) / (zFar - zNear);
        return m;
    }

    inline mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
        vec3 f = normalize(center - eye);
        vec3 s = normalize(cross(f, up));
        vec3 u = cross(s, f);

        mat4 m = mat4::identity();
        m.data[0] = s.x; m.data[4] = s.y; m.data[8]  = s.z;
        m.data[1] = u.x; m.data[5] = u.y; m.data[9]  = u.z;
        m.data[2] =-f.x; m.data[6] =-f.y; m.data[10] =-f.z;
        m.data[12] = -dot(s, eye);
        m.data[13] = -dot(u, eye);
        m.data[14] =  dot(f, eye);
        return m;
    }

    inline mat4 rotate(const mat4& m, float angle, const vec3& axis) {
        mat4 r = mat4::identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        float omc = 1.0f - c;

        vec3 a = normalize(axis);
        float x = a.x, y = a.y, z = a.z;

        r.data[0] = c + x * x * omc;
        r.data[1] = x * y * omc + z * s;
        r.data[2] = x * z * omc - y * s;

        r.data[4] = y * x * omc - z * s;
        r.data[5] = c + y * y * omc;
        r.data[6] = y * z * omc + x * s;

        r.data[8] = z * x * omc + y * s;
        r.data[9] = z * y * omc - x * s;
        r.data[10] = c + z * z * omc;
        r.data[15] = 1.0f;

        mat4 result;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                result[row][col] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result[row][col] += m[row][k] * r[k][col];
                }
            }
        }
        return result;
    }

    /* ----- MATH UTILS ----- */
    inline float radians(float degrees) {
        return degrees * static_cast<float>(M_PI) / 180.0f;
    }

    inline float half_pi() {
        return static_cast<float>(M_PI_2);
    }

    inline float clamp(float x, float a, float b) {
        return std::min(std::max(x, a), b);
    }

}

#endif

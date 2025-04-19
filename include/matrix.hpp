#ifndef MATRIX_HPP
#define MATRIX_HPP

struct Vec2 { float x, y; bool operator==(const Vec2& o) const { return x==o.x && y==o.y; } };
struct Vec3 { float x, y, z; bool operator==(const Vec3& o) const { return x==o.x && y==o.y && z==o.z; } };
struct Vec4 { float x, y, z, w; };

struct Mat4 {
    float m[4][4];
    static Mat4 identity();
    static Mat4 perspective(float fovy, float aspect, float znear, float zfar);
    static Mat4 translate(const Vec3& v);
    static Mat4 rotate(const Vec3& axis, float angle);
    Mat4 operator*(const Mat4& o) const;
};

#endif
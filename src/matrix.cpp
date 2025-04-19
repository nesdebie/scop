#include "matrix.hpp"
#include <cmath>

Mat4 Mat4::identity() {
    Mat4 I = {};
    for(int i=0;i<4;i++) I.m[i][i] = 1.0f;
    return I;
}

Mat4 Mat4::perspective(float fovy, float aspect, float znear, float zfar) {
    Mat4 P = {};
    float tanHalfFovy = tanf(fovy / 2.0f);
    P.m[0][0] = 1.0f / (aspect * tanHalfFovy);
    P.m[1][1] = 1.0f / tanHalfFovy;
    P.m[2][2] = -(zfar + znear) / (zfar - znear);
    P.m[2][3] = -1.0f;
    P.m[3][2] = -(2.0f * zfar * znear) / (zfar - znear);
    return P;
}

Mat4 Mat4::translate(const Vec3& v) {
    Mat4 T = identity();
    T.m[3][0] = v.x;
    T.m[3][1] = v.y;
    T.m[3][2] = v.z;
    return T;
}

Mat4 Mat4::rotate(const Vec3& axis, float angle) {
    Mat4 R = {};
    float c = cosf(angle);
    float s = sinf(angle);
    Vec3 a = axis; // assume normalized
    R.m[0][0] = a.x*a.x*(1-c) + c;
    R.m[0][1] = a.x*a.y*(1-c) - a.z*s;
    R.m[0][2] = a.x*a.z*(1-c) + a.y*s;
    R.m[1][0] = a.y*a.x*(1-c) + a.z*s;
    R.m[1][1] = a.y*a.y*(1-c) + c;
    R.m[1][2] = a.y*a.z*(1-c) - a.x*s;
    R.m[2][0] = a.z*a.x*(1-c) - a.y*s;
    R.m[2][1] = a.z*a.y*(1-c) + a.x*s;
    R.m[2][2] = a.z*a.z*(1-c) + c;
    R.m[3][3] = 1.0f;
    return R;
}

Mat4 Mat4::operator*(const Mat4& o) const {
    Mat4 R = {};
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            for(int k=0;k<4;k++)
                R.m[i][j] += m[i][k] * o.m[k][j];
    return R;
}
#pragma once
#include <cmath>

// this will typically display certain parts of the client/character
struct Vector {
    float x, y, z;

    Vector() : x(0.f), y(0.f), z(0.f) {}
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector operator+(const Vector& o) const { return { x + o.x, y + o.y, z + o.z }; }
    Vector operator-(const Vector& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vector operator*(float f) const { return { x * f, y * f, z * f }; }

    Vector& operator+=(const Vector& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vector& operator-=(const Vector& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }


    float Length() const { return std::sqrt(x * x + y * y + z * z); }
    bool IsZero() const { return x == 0.f && y == 0.f && z == 0.f; }
};

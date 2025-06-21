#ifndef TYPES_HPP
#define TYPES_HPP
#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <iostream>

using scalar_t = float;

using id_t = uint32_t;

struct Vec3{
    scalar_t x, y ,z;   //coordinates of the vector
    // Default constructor initializes to zero
    Vec3(scalar_t x = 0, scalar_t y = 0, scalar_t z = 0) : x(x), y(y), z(z) {}

    // Copy constructor
    Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}

    // Assignment operator
    Vec3& operator= (const Vec3& other);

    // Arithmetic operators
    Vec3& operator+= (const Vec3& other);
    Vec3& operator-= (const Vec3& other);
    Vec3& operator*= (scalar_t scalar);
    Vec3 operator+ (const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator- (const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z);}
    Vec3 operator- () { return Vec3(-x, -y, -z); }
    Vec3 operator* (scalar_t scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/ (scalar_t scalar) const;

    // Comparison operators
    bool operator== (const Vec3& other) const { return x == other.x && y == other.y && z == other.z; }

    // Norm and normalization
    scalar_t norm() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalize() const;

    // Change the norm of the vector
    void changeNorm(scalar_t amount);

    // if the norm of the vector is more than maxNorm, set the norm to maxNorm
    void limitNorm(scalar_t maxNorm);
};

std::ostream& operator<<(std::ostream& os, const Vec3& vec);

#endif 
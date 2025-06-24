#include "types.hpp"




Vec3& Vec3::operator= (const Vec3& other) {
        if (this != &other) { // self-assignment check
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }
Vec3& Vec3::operator+= (const Vec3& other) { 
        x += other.x; 
        y += other.y; 
        z += other.z; 
        return *this;
    }


Vec3& Vec3::operator-= (const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

Vec3& Vec3::operator*= (scalar_t scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
}

Vec3& Vec3::operator/= (scalar_t scalar) {
        if (scalar == 0) {
            throw std::runtime_error("Division by zero in Vec3 operator/");
        }
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
}

Vec3 Vec3::operator/ (scalar_t scalar) const {
        if (scalar == 0) {
            throw std::runtime_error("Division by zero in Vec3 operator/");
        }
        return Vec3(x / scalar, y / scalar, z / scalar);
}   

Vec3 Vec3::normalize() const {
        scalar_t n = norm();
        return n > 0 ? Vec3(x / n, y / n, z / n) : Vec3(0, 0, 0);
}

Vec3 Vec3::changeNorm(scalar_t amount) const {
    scalar_t n = norm();
    if (n == 0) {
        throw std::runtime_error("Cannot change norm of a zero vector");
    }
    scalar_t coeff = amount / n;
    return Vec3(x * coeff, y * coeff, z * coeff);
}


Vec3 Vec3::limitNorm(scalar_t maxNorm) const {
    scalar_t n = norm();
    if (n > maxNorm) {
        return changeNorm(maxNorm);
    }
    return *this;
}


std::ostream& operator<<(std::ostream& os, const Vec3& vec){
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}
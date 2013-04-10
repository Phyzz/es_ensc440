#include <math.h>
#include "es_3dVect.hpp"

es_3dVect::es_3dVect(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

es_3dVect & es_3dVect::operator=(const es_3dVect &rhs) {
    if (this == &rhs) {
        return *this;
    }
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    
    return *this;    
}

es_3dVect & es_3dVect::operator+=(const es_3dVect &rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    
    return *this;
}

es_3dVect & es_3dVect::operator-=(const es_3dVect &rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    
    return *this;
}

es_3dVect & es_3dVect::operator*=(const float &rhs) {
    this->x *= rhs;
    this->y *= rhs;
    this->z *= rhs;
    
    return *this;
}

es_3dVect & es_3dVect::operator/=(const float &rhs) {
    this->x /= rhs;
    this->y /= rhs;
    this->z /= rhs;
    
    return *this;
}

es_3dVect es_3dVect::operator+(const es_3dVect &rhs) {
    return es_3dVect(*this) += rhs;
}

es_3dVect es_3dVect::operator-(const es_3dVect &rhs) {
    return es_3dVect(*this) -= rhs;
}

es_3dVect es_3dVect::operator*(const float &rhs) {
    return es_3dVect(*this) *= rhs;
}

es_3dVect es_3dVect::operator/(const float &rhs) {
    return es_3dVect(*this) /= rhs;
}

float es_3dVect::length() {
    return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

float es_3dVect::dot(const es_3dVect other) {
    return this->x * other.x + this->y * other.y + this->z * other.z;
}
es_3dVect es_3dVect::cross(const es_3dVect other) {
    return es_3dVect(this->y*other.z - this->z*other.y, this->z*other.x - this->x*other.z, this->x*other.y - this->y*other.x);
}

std::ostream & operator<<(std::ostream & os, const es_3dVect &rhs) {
    os << "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
    return os;
}

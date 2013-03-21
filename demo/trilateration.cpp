#include <iostream>
#include <math.h>

class vect3d {
    public:
        vect3d() { x = 0; y = 0; z = 0; }
        vect3d(float x, float y, float z);

        vect3d & operator=(const vect3d &rhs);    
        vect3d & operator+=(const vect3d &rhs); 
        vect3d & operator-=(const vect3d &rhs);
        vect3d & operator*=(const float &rhs);
        vect3d & operator/=(const float &rhs);
        vect3d operator+(const vect3d &rhs); 
        vect3d operator-(const vect3d &rhs);
        vect3d operator*(const float &rhs);
        vect3d operator/(const float &rhs);    
        vect3d cross(const vect3d other);        
        float dot(const vect3d other);
        float length();
        
//        std::ostream & operator<<(std::ostream & os, const vect3d &rhs);

        float x;
        float y;
        float z;
};

vect3d::vect3d(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

vect3d & vect3d::operator=(const vect3d &rhs) {
    if (this == &rhs) {
        return *this;
    }
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    
    return *this;    
}

vect3d & vect3d::operator+=(const vect3d &rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    
    return *this;
}

vect3d & vect3d::operator-=(const vect3d &rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    
    return *this;
}

vect3d & vect3d::operator*=(const float &rhs) {
    this->x *= rhs;
    this->y *= rhs;
    this->z *= rhs;
    
    return *this;
}

vect3d & vect3d::operator/=(const float &rhs) {
    this->x /= rhs;
    this->y /= rhs;
    this->z /= rhs;
    
    return *this;
}

vect3d vect3d::operator+(const vect3d &rhs) {
    return vect3d(*this) += rhs;
}

vect3d vect3d::operator-(const vect3d &rhs) {
    return vect3d(*this) -= rhs;
}

vect3d vect3d::operator*(const float &rhs) {
    return vect3d(*this) *= rhs;
}

vect3d vect3d::operator/(const float &rhs) {
    return vect3d(*this) /= rhs;
}

float vect3d::length() {
    return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

float vect3d::dot(const vect3d other) {
    return this->x * other.x + this->y * other.y + this->z * other.z;
}
vect3d vect3d::cross(const vect3d other) {
    return vect3d(this->y*other.z - this->z*other.y, this->z*other.x - this->x*other.z, this->x*other.y - this->y*other.x);
}

std::ostream & operator<<(std::ostream & os, const vect3d &rhs) {
    os << "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
    return os;
}

vect3d* new_basis(vect3d & b1, vect3d & b2, vect3d & b3){
    vect3d * ret = new vect3d[3];
    
    //ex
    ret[0] = b2-b1;
    ret[0] = ret[0]/(ret[0].length());
    
    //ey
    ret[1] = (b3 - b1) - ret[0]*(ret[0].dot(b3-b1));
    ret[1] = ret[1]/ret[1].length();

    //ez
    ret[2] = ret[0].cross(ret[1]);

    return ret;
}

void trilaterate(vect3d & b1, vect3d & b2, vect3d & b3, float r1, float r2, float r3) {

    vect3d* basis = new_basis(b1,b2,b3);

    float h = (b2 - b1).length();
    float i = basis[0].dot(b3-b1);
    float j = basis[1].dot(b3-b1);

    float X = (r1 * r1 - r2*r2 + h * h)/(2*h);
    float Y = (r1 * r1 - r3 * r3 + i * i + j* j -2*i*X)/(2*j);
    float Z = r1 * r1 - X * X - Y * Y;
    if (Z < 0) {
        std::cout << "Can't solve!";
        delete[] basis;
        return;
    }
    Z = sqrt(Z);     

    vect3d soln1 = b1 + basis[0]*X + basis[1]*Y + basis[2]*Z;
    vect3d soln2 = b1 + basis[0]*X + basis[1]*Y + basis[2]*Z*(-1);

    std::cout << "Solution 1: " << soln1 << std::endl;
    std::cout << "Solution 2: " << soln2 << std::endl;
    delete[] basis;
}

float get_r(vect3d p1, vect3d p2) {
    return (p2-p1).length();
}

int main(int argc, char* argv[]) {

    vect3d b1 = vect3d(0,0,27);
    vect3d b2 = vect3d(20,0,15);
    vect3d b3 = vect3d(10,15,25);
    vect3d b4 = vect3d(13,18, 4);

    vect3d point = vect3d(18,6,4);
    
    std::cout << "Trilaterating point: " << point << std::endl;
    
    float r1 = get_r(b1,point);
    float r2 = get_r(b2,point);
    float r3 = get_r(b3,point);
    float r4 = get_r(b4,point);

    trilaterate(b1,b2,b3,r1,r2,r3);
    std::cout << std::endl;
    trilaterate(b1,b2,b4,r1,r2,r4);

    return 0;
}


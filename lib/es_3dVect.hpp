#include <iostream>

class es_3dVect {
    public:
        es_3dVect() { x = 0; y = 0; z = 0; }
        es_3dVect(float x, float y, float z);

        es_3dVect & operator=(const es_3dVect &rhs);    
        es_3dVect & operator+=(const es_3dVect &rhs); 
        es_3dVect & operator-=(const es_3dVect &rhs);
        es_3dVect & operator*=(const float &rhs);
        es_3dVect & operator/=(const float &rhs);
        es_3dVect operator+(const es_3dVect &rhs); 
        es_3dVect operator-(const es_3dVect &rhs);
        es_3dVect operator*(const float &rhs);
        es_3dVect operator/(const float &rhs);    
        es_3dVect cross(const es_3dVect other);        
        float dot(const es_3dVect other);
        float length();

        float x;
        float y;
        float z;
};

std::ostream & operator<<(std::ostream & os, const es_3dVect &rhs);

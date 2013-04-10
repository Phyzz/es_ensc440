#include <iostream>
#include <math.h>
#include "../lib/es_3dVect.hpp"

es_3dVect* new_basis(es_3dVect & b1, es_3dVect & b2, es_3dVect & b3){
    es_3dVect * ret = new es_3dVect[3];
    
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

void trilaterate(es_3dVect & b1, es_3dVect & b2, es_3dVect & b3, float r1, float r2, float r3) {

    es_3dVect* basis = new_basis(b1,b2,b3);

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

    es_3dVect soln1 = b1 + basis[0]*X + basis[1]*Y + basis[2]*Z;
    es_3dVect soln2 = b1 + basis[0]*X + basis[1]*Y + basis[2]*Z*(-1);

    std::cout << "Solution 1: " << soln1 << std::endl;
    std::cout << "Solution 2: " << soln2 << std::endl;
    delete[] basis;
}

float get_r(es_3dVect p1, es_3dVect p2) {
    return (p2-p1).length();
}

int main(int argc, char* argv[]) {

    es_3dVect b1 = es_3dVect(0,0,27);
    es_3dVect b2 = es_3dVect(20,0,15);
    es_3dVect b3 = es_3dVect(10,15,25);
    es_3dVect b4 = es_3dVect(13,18, 4);

    es_3dVect point = es_3dVect(18,6,4);
    
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


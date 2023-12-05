#include "Cylinder.h"
#include <math.h>

/**
* Sheared Cylinder's intersection method and normal.
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{

    float a = (dir.x) * (dir.x) + dir.z * dir.z;
    float b = 2 * ((dir.x) * (posn.x - center.x) + (dir.z * (posn.z - center.z)));
    float c = (posn.x - center.x) * (posn.x - center.x) + (posn.z - center.z) * (posn.z - center.z) - radius * radius;
    float delta = b * b - 4 * a * c;

    if(delta < 0.001){
        return -1.0;
    }
//    ******
    float t1 = (-b + sqrt(delta))/(2 * a);
    float t2 = (-b - sqrt(delta))/(2 * a);

    float low;
    float high;

    if(t1 > t2){
        low = t2;
        high = t1;
    } else {
        low = t1;
        high = t2;
    }

    float t1_h = posn.y +  low * dir.y;
    float t2_h = posn.y + high * dir.y;

    if((t1_h > (center.y + height)) && (t2_h > (center.y + height))){
        return -1.0;
    } else if((t1_h > (center.y + height)) && (t2_h < (center.y + height))){
        return high;
    } else {
        return low;
    }

}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 pt)
{
    glm::vec3 n = glm::vec3((pt.x-center.x)/radius, 0., (pt.z - center.z)/radius);

    return n;
}

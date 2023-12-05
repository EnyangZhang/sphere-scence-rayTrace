#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir).
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
   float a = dir.x * dir.x + dir.z * dir.z - ((radius / height) * (radius / height))*(dir.y * dir.y);
   float b = 2 * ((posn.x - center.x) * dir.x + (posn.z - center.z) * dir.z + ((radius / height) * (radius / height)) * (height - posn.y + center.y) * dir.y);
   float c = (posn.x - center.x) * (posn.x - center.x) + (posn.z - center.z) * (posn.z - center.z) - ((radius / height) * (radius / height)) * (height - posn.y + center.y) * (height - posn.y + center.y);
   float delta = b*b - 4*a*c;

   if(delta < 0.001){
       return -1;
   }

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
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    float alpha = atan((p.x - center.x)/(p.z-center.z));
    float theta = atan(radius/height);
    glm::vec3 n = glm::vec3 (sin(alpha) * cos(theta), sin(theta), cos(alpha) * cos(theta));
    return n;
}

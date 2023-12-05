/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer (assignmnet 2)
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "TextureBMP.h"
#include <GL/glut.h>
#include "Shader.h"
#include "Cylinder.h"
#include "Cone.h"

using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float u1 = 1;
const float u2 = 1.01;
const float ETA = u1/u2;

TextureBMP crystalPlanet;




vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);

    //double light sources
    glm::vec3 light(15, 40, -3);
    glm::vec3 lightSecondary(-15, 40, -70);
	glm::vec3 ambientCol(0.2);   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);

    //double lightVector
    glm::vec3 lightVector = glm::normalize(light - ray.xpt);
    glm::vec3 lightVectorSecondary = glm::normalize(lightSecondary - ray.xpt);

    //double shadow
    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);

    Ray shadowSecondary(ray.xpt, lightVectorSecondary);
    shadowSecondary.closestPt(sceneObjects);

    //double reflVector
    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
    glm::vec3 reflVectorSecondary = glm::reflect(-lightVectorSecondary , normalVector);

    glm::vec3 viewVector = -ray.dir;

    //double lDotn
    float lDotn = glm::dot(lightVector,normalVector);
    float lDotnSecondary = glm::dot(lightVectorSecondary, normalVector);

    float specularTerm;
    float specularTermSecondary;

    //double rDotv
    float rDotv = glm::dot(reflVector, viewVector);
    float rDotvSecondary = glm::dot(reflVectorSecondary, viewVector);

    //double specularTerm
    if(rDotv < 0) specularTerm = 0.0;
    else specularTerm = pow(rDotv, 20)*(1,1,1);

    if(rDotvSecondary < 0) specularTermSecondary = 0.0;
    else specularTermSecondary = pow(rDotvSecondary, 20)*(1,1,1);


    glm::vec3 colorSum;
    float decreaseBrightness = 0.6;

    //shadow
    if(lDotn <= 0 || (shadow.xindex > -1 && shadow.xdist < glm::length(light - ray.xpt))){
        colorSum = ambientCol*materialCol;
    } else {
        colorSum = ambientCol*materialCol + decreaseBrightness * (lDotn*materialCol + specularTerm);
    }

    if(lDotnSecondary <= 0 || (shadowSecondary.xindex > -1 && shadowSecondary.xdist < glm::length(lightSecondary - ray.xpt))){
        colorSum += ambientCol*materialCol;
    } else {
        colorSum += ambientCol*materialCol + decreaseBrightness * (lDotnSecondary*materialCol + specularTermSecondary);
    }

    //sphere1 has reflaction
    if(ray.xindex == 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1); //Recursion!
        colorSum = colorSum + (0.8f*reflectedCol);
    }


    //sphere3 has refraciton and transparent
    if((ray.xindex == 2) && (step < MAX_STEPS)){
        glm::vec3 g = glm::refract(ray.dir, normalVector, ETA);
        Ray refrRay(ray.xpt, g);
        refrRay.closestPt(sceneObjects);

        glm::vec3 m = sceneObjects[refrRay.xindex]->normal(refrRay.xpt);
        glm::vec3 h = glm::refract(g, -m, u2);
        Ray refrRayOut(refrRay.xpt, h);
        refrRayOut.closestPt(sceneObjects);

        glm::vec3 refractedCol = trace(refrRayOut, step + 1);
        colorSum = colorSum  + refractedCol;
        return colorSum;

            }



    //sphere2 texture
    if(ray.xindex == 1){
        float patternX = asin(normalVector.x) / M_PI + 0.5;
        float patternY = asin(normalVector.y) / M_PI + 0.5;
        colorSum += crystalPlanet.getColorAt(patternX, patternY);
    }

    //sphere4 partten
    if(ray.xindex == 3){
         int patternX = (int)((ray.xpt.x)) % 2;
         int patternZ = (int)((ray.xpt.z) * 2) % 2;

        if((patternX && patternZ) || (!patternX && !patternZ)){
            colorSum += glm::vec3(1., 0., 0.);}
        else{
            colorSum += glm::vec3(0., 0., 1);}
     }

    //chequered floor
    if(ray.xindex == 4){
         int patternX = (int)(((ray.xpt.x) + 128) /4) % 2;
         int patternZ = (int)((ray.xpt.z) /4) % 2;

        if((patternX && patternZ) || (!patternX && !patternZ)){
            colorSum += glm::vec3(0.,0.,0.5);}
        else{
            colorSum += glm::vec3(0.5, 0.3, 0.0);}
     }


    return colorSum;

}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{


	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
            yp = YMIN + j*cellY;

            //-- implement super-sampling
            //left Bottom
            glm::vec3 dirlb(xp+0.25*cellX, yp+0.25*cellY, -EDIST);	//direction of the primary ray
            Ray raylb = Ray(eye, dirlb);		//Create a ray originating from the camera in the direction 'dir'
            raylb.normalize();				//Normalize the direction of the ray to a unit vector
            glm::vec3 collb = trace (raylb, 1); //Trace the primary ray and get the colour value
            //right bottom
            glm::vec3 dirrb(xp+0.75*cellX, yp+0.25*cellY, -EDIST);	//direction of the primary ray
            Ray rayrb = Ray(eye, dirrb);		//Create a ray originating from the camera in the direction 'dir'
            rayrb.normalize();				//Normalize the direction of the ray to a unit vector
            glm::vec3 colrb = trace (rayrb, 1); //Trace the primary ray and get the colour value
            //left top
            glm::vec3 dirlt(xp+0.25*cellX, yp+0.75*cellY, -EDIST);	//direction of the primary ray
            Ray raylt = Ray(eye, dirlt);		//Create a ray originating from the camera in the direction 'dir'
            raylt.normalize();				//Normalize the direction of the ray to a unit vector
            glm::vec3 collt = trace (raylt, 1); //Trace the primary ray and get the colour value
            //right top
            glm::vec3 dirrt(xp+0.75*cellX, yp+0.75*cellY, -EDIST);	//direction of the primary ray
            Ray rayrt = Ray(eye, dirrt);		//Create a ray originating from the camera in the direction 'dir'
            rayrt.normalize();				//Normalize the direction of the ray to a unit vector
            glm::vec3 colrt = trace (rayrt, 1); //Trace the primary ray and get the colour value

            glColor3f((collb.r + collt.r + colrb.r + colrt.r)/4, (collb.g + collt.g + colrb.g + colrt.g)/4, (collb.b + collt.b + colrb.b + colrt.b)/4);

			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
    glutSwapBuffers();

}

//box transformation using rotaion along z-axis 30 degree
glm::vec3 rotate(glm::vec3 vec)
{
    vec.x = vec.x * sqrt(3)/2 - vec.y * 1/2;
    vec.y = vec.x * 1/2 + vec.y * sqrt(3)/2;
    vec.z = vec.z;

    return vec;
}

//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
    //--Load the image for sphere2
    crystalPlanet = TextureBMP ((char*)"crystalPlanet.bmp");

	//-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -90.0), 10., glm::vec3(0, 0, 1));//0
    Sphere *sphere2 = new Sphere(glm::vec3(5.0, 5.5, -70.0), 2.5, glm::vec3(0, 0, 0));//1
    Sphere *sphere3 = new Sphere(glm::vec3(7.0, -15.0, -80.0), 2.5, glm::vec3(0, 0, 0));//2
    Sphere *sphere4 = new Sphere(glm::vec3(-7.0, -13.0, -80.0), 2.5, glm::vec3(0, 0, 0));//3



	//--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(sphere4);



    //--Create four point for plane
    Plane *plane = new Plane//4
            (
            glm::vec3(-20., -20, -40),//Point A
            glm::vec3(20., -20, -40),//Point B
            glm::vec3(20., -20, -200),//Point C
            glm::vec3(-20., -20, -200),//Point D
            glm::vec3(0.5, 0.5, 0)//Colour
            );

    sceneObjects.push_back(plane);



    //--Create box(cube)
    Plane *top = new Plane(rotate(glm::vec3(10, -15, -100)), rotate(glm::vec3(15, -15, -100)), rotate(glm::vec3(15, -15, -105)), rotate(glm::vec3(10, -15, -105)), glm::vec3(1, 1, 0));
    Plane *front = new Plane(rotate(glm::vec3(10, -15, -100)), rotate(glm::vec3(10, -20, -100)), rotate(glm::vec3(15, -20, -100)), rotate(glm::vec3(15, -15, -100)), glm::vec3(0, 1, 0));
    Plane *back = new Plane(rotate(glm::vec3(10, -15, -105)), rotate(glm::vec3(15, -15, -105)), rotate(glm::vec3(15, -20, -100)), rotate(glm::vec3(10, -20, -105)),glm::vec3(0, 1, 0));
    Plane *left = new Plane(rotate(glm::vec3(10, -20, -105)), rotate(glm::vec3(10, -20, -100)), rotate(glm::vec3(10, -15, -100)), rotate(glm::vec3(10, -15, -105)), glm::vec3(0, 1, 0));
    Plane *right = new Plane(rotate(glm::vec3(15, -20, -100)), rotate(glm::vec3(15, -20, -105)), rotate(glm::vec3(15, -15, -105)), rotate(glm::vec3(15, -15, -100)), glm::vec3(0, 1, 0));

    //--Push to sceneObjects for box and rotate it along z-axis 30 degree
    sceneObjects.push_back(top);
    sceneObjects.push_back(front);
    sceneObjects.push_back(back);
    sceneObjects.push_back(left);
    sceneObjects.push_back(right);

    //--create a cylinder
    Cylinder* cylinder = new Cylinder(glm::vec3(15, -15, -90), 2., 1., glm::vec3(1, 0, 0));
    sceneObjects.push_back(cylinder);

    //--create a cone
    Cone* cone = new Cone(glm::vec3(0, -15, -85), 0.5, 3., glm::vec3(1, 0, 0));
    sceneObjects.push_back(cone);

    //create tetrahedron
    glm::vec3 a = glm::vec3(0,-10,-80);
    glm::vec3 b = glm::vec3(4,-10,-80);
    glm::vec3 c = glm::vec3(2, -10, -80 + sqrt(3) * 2);
    glm::vec3 d = glm::vec3(2, -10 + (sqrt(6) * 4)/3, -80 + sqrt(3));
    glm::vec3 e = glm::vec3(2,-10,-80);
    glm::vec3 f = glm::vec3((c.x+d.x)/2,(c.y+d.y)/2,(c.z+d.z)/2);

    Plane* side1 = new Plane(e,b,c,a,glm::vec3(0, 1, 1));
    Plane* side2 = new Plane(b,d,f,c,glm::vec3(0, 1, 1));
    Plane* side3 = new Plane(d,a,c,f,glm::vec3(0, 1, 1));
    Plane* side4 = new Plane(a,d,b,e,glm::vec3(0, 1, 1));

    sceneObjects.push_back(side1);
    sceneObjects.push_back(side2);
    sceneObjects.push_back(side3);
    sceneObjects.push_back(side4);

}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");
    initialize();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

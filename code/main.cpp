#include <math.h>
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#include "PointLight.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "Lambert.h"
#include "Marbre.hpp"
#include "Glass.hpp"
#include "Mirror.hpp"
#include "Stone.hpp"
#include "SquareLight.hpp"
#include "MiroWindow.h"
#include "PFMLoader.h"

static Vector3 bg = Vector3(0.0f);

void
makeSpiralScene(int width, int height)
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(width, height);

    // set up the camera
    g_camera->setBGColor(bg);
    g_camera->setEye(Vector3(-5, 2, 3));
    g_camera->setLookAt(Vector3(0, 0, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(5000);
    g_scene->addLight(light);

    // create a spiral of spheres
    const int maxI = 200;
    const float a = 0.15f;
    for (int i = 1; i < maxI; ++i)
    {
        float t = i/float(maxI);
        float theta = 4*PI*t;
        float r = a*theta;
        float x = r*cos(theta);
        float y = r*sin(theta);
        float z = 2*(2*PI*a - r);
        Sphere * sphere = new Sphere;
        sphere->setCenter(Vector3(x,y,z));
        sphere->setRadius(r/10);
        sphere->setMaterial(new Lambert(Vector3(1.0f * t, 1.0f * (1.0f - t), 0.0f)));
        g_scene->addObject(sphere);
    }

    // let objects do pre-calculations if needed
    g_scene->preCalc();
}


void
makeLorenzScene(int width, int height)
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(width, height);

    // set up the camera
    g_camera->setBGColor(bg);
    g_camera->setEye(Vector3(-2, 1, 0));
    g_camera->setLookAt(Vector3(0, 0.1, 0.8));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(5000);
    g_scene->addLight(light);

    // create a spiral of spheres
    const int maxI = 2000;

    const float sigma = 10.0f;
    const float rho = 28.0f;
    const float beta = 8.0f / 3.0f;

    float x = 1.0f;
    float y = 1.0f;
    float z = 1.0f;

    for (int i = 1; i < maxI; ++i)
    {

        float nx = sigma * (y - x);
        float ny = x * (rho - z) - y;
        float nz = x * y - beta * z;

        x = x + nx * 0.005f;
        y = y + ny * 0.005f;
        z = z + nz * 0.005f;

        float t = i/float(maxI);

        Sphere * sphere = new Sphere;
        sphere->setCenter(Vector3(x * 0.03f, y * 0.03f, z * 0.03f));
        sphere->setRadius(0.02);
        sphere->setMaterial(new Lambert(Vector3(1.0f * t, 1.0f * (1.0f - t), 0.0f)));
        g_scene->addObject(sphere);
    }
    g_scene->preCalc();
}

void
makeEmptyCornellBoxScene(int width, int height)
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(width, height);

    // set up the camera
    g_camera->setBGColor(bg);
    g_camera->setEye(Vector3(0, -4, 1));
    g_camera->setLookAt(Vector3(0, 0, 1));
    g_camera->setUp(Vector3(0, 0, 1));
    g_camera->setFOV(40);


    Material    *m_light = new SquareLight();

    TriangleMesh *light = new TriangleMesh;
    light->createSingleTriangle();

    light->setV1(Vector3(-0.1f,  0.1f,  1.98f));
    light->setV2(Vector3(-0.1f, -0.1f,  1.98f));
    light->setV3(Vector3( 0.1f, -0.1f,  1.98f));

    light->setN1(Vector3( 0.0f,  0.0f,  1.0f));
    light->setN2(Vector3( 0.0f,  0.0f,  1.0f));
    light->setN3(Vector3( 0.0f,  0.0f,  1.0f));

    Triangle* t = new Triangle;
    t->setIndex(0);
    t->setMesh(light);
    t->setMaterial(m_light);
    g_scene->addObject(t);

    TriangleMesh *light2 = new TriangleMesh;
    light2->createSingleTriangle();

    light2->setV1(Vector3( 0.1f, -0.1f,  1.98f));
    light2->setV2(Vector3( 0.1f,  0.1f,  1.98f));
    light2->setV3(Vector3(-0.1f,  0.1f,  1.98f));

    light2->setN1(Vector3( 0.0f,  0.0f,  1.0f));
    light2->setN2(Vector3( 0.0f,  0.0f,  1.0f));
    light2->setN3(Vector3( 0.0f,  0.0f,  1.0f));

    Triangle* t2 = new Triangle;
    t2->setIndex(0);
    t2->setMesh(light2);
    t2->setMaterial(m_light);
    g_scene->addObject(t2);










    Material *mat = new Lambert(1.0f);

    TriangleMesh *floor1 = new TriangleMesh;
    floor1->createSingleTriangle();

    floor1->setV1(Vector3(-1.0f, -1.0f,  0.0f));
    floor1->setV2(Vector3( 1.0f, -1.0f,  0.0f));
    floor1->setV3(Vector3(-1.0f,  1.0f,  0.0f));

    floor1->setN1(Vector3( 0.0f,  0.0f,  1.0f));
    floor1->setN2(Vector3( 0.0f,  0.0f,  1.0f));
    floor1->setN3(Vector3( 0.0f,  0.0f,  1.0f));

    t = new Triangle;
    t->setIndex(0);
    t->setMesh(floor1);
    t->setMaterial(mat);
    g_scene->addObject(t);

    TriangleMesh *floor2 = new TriangleMesh;
    floor2->createSingleTriangle();

    floor2->setV1(Vector3( 1.0f, -1.0f,  0.0f));
    floor2->setV2(Vector3( 1.0f,  1.0f,  0.0f));
    floor2->setV3(Vector3(-1.0f,  1.0f,  0.0f));

    floor2->setN1(Vector3( 0.0f,  0.0f,  1.0f));
    floor2->setN2(Vector3( 0.0f,  0.0f,  1.0f));
    floor2->setN3(Vector3( 0.0f,  0.0f,  1.0f));

    t2 = new Triangle;
    t2->setIndex(0);
    t2->setMesh(floor2);
    t2->setMaterial(mat);
    g_scene->addObject(t2);


    TriangleMesh *left1 = new TriangleMesh;
    left1->createSingleTriangle();

    left1->setV1(Vector3(-1.0f, -1.0f,  0.0f));
    left1->setV2(Vector3(-1.0f,  1.0f,  0.0f));
    left1->setV3(Vector3(-1.0f, -1.0f,  2.0f));

    left1->setN1(Vector3( 1.0f,  0.0f,  0.0f));
    left1->setN2(Vector3( 1.0f,  0.0f,  0.0f));
    left1->setN3(Vector3( 1.0f,  0.0f,  0.0f));

    Triangle* t3 = new Triangle;
    t3->setIndex(0);
    t3->setMesh(left1);
    t3->setMaterial(mat);
    g_scene->addObject(t3);

    TriangleMesh *left2 = new TriangleMesh;
    left2->createSingleTriangle();

    left2->setV1(Vector3(-1.0f,  1.0f,  0.0f));
    left2->setV2(Vector3(-1.0f,  1.0f,  2.0f));
    left2->setV3(Vector3(-1.0f, -1.0f,  2.0f));

    left2->setN1(Vector3( 1.0f,  0.0f,  0.0f));
    left2->setN2(Vector3( 1.0f,  0.0f,  0.0f));
    left2->setN3(Vector3( 1.0f,  0.0f,  0.0f));

    Triangle* t4 = new Triangle;
    t4->setIndex(0);
    t4->setMesh(left2);
    t4->setMaterial(mat);
    g_scene->addObject(t4);

    TriangleMesh *back1 = new TriangleMesh;
    back1->createSingleTriangle();

    back1->setV1(Vector3(-1.0f,  1.0f,  0.0f));
    back1->setV2(Vector3( 1.0f,  1.0f,  0.0f));
    back1->setV3(Vector3(-1.0f,  1.0f,  2.0f));

    back1->setN1(Vector3( 0.0f, -1.0f,  0.0f));
    back1->setN2(Vector3( 0.0f, -1.0f,  0.0f));
    back1->setN3(Vector3( 0.0f, -1.0f,  0.0f));

    Triangle* t5 = new Triangle;
    t5->setIndex(0);
    t5->setMesh(back1);
    //t5->setMaterial(new Lambert(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.2f, 0.2f, 0.2f)));
    //t5->setMaterial(new Mirror());
    t5->setMaterial(mat);
    g_scene->addObject(t5);

    TriangleMesh *back2 = new TriangleMesh;
    back2->createSingleTriangle();

    back2->setV1(Vector3( 1.0f,  1.0f,  0.0f));
    back2->setV2(Vector3( 1.0f,  1.0f,  2.0f));
    back2->setV3(Vector3(-1.0f,  1.0f,  2.0f));

    back2->setN1(Vector3( 0.0f, -1.0f,  0.0f));
    back2->setN2(Vector3( 0.0f, -1.0f,  0.0f));
    back2->setN3(Vector3( 0.0f, -1.0f,  0.0f));

    Triangle* t6 = new Triangle;
    t6->setIndex(0);
    t6->setMesh(back2);
    //t6->setMaterial(new Lambert(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.2f, 0.2f, 0.2f)));
    //t6->setMaterial(new Mirror());
    t6->setMaterial(mat);
    g_scene->addObject(t6);

    TriangleMesh *right1 = new TriangleMesh;
    right1->createSingleTriangle();

    right1->setV1(Vector3( 1.0f,  1.0f,  0.0f));
    right1->setV2(Vector3( 1.0f, -1.0f,  0.0f));
    right1->setV3(Vector3( 1.0f, -1.0f,  2.0f));

    right1->setN1(Vector3(-1.0f,  0.0f,  0.0f));
    right1->setN2(Vector3(-1.0f,  0.0f,  0.0f));
    right1->setN3(Vector3(-1.0f,  0.0f,  0.0f));

    Triangle* t7 = new Triangle;
    t7->setIndex(0);
    t7->setMesh(right1);
    t7->setMaterial(mat);
    g_scene->addObject(t7);

    TriangleMesh *right2 = new TriangleMesh;
    right2->createSingleTriangle();

    right2->setV1(Vector3( 1.0f,  1.0f,  0.0f));
    right2->setV2(Vector3( 1.0f, -1.0f,  2.0f));
    right2->setV3(Vector3( 1.0f,  1.0f,  2.0f));

    right2->setN1(Vector3(-1.0f,  0.0f,  0.0f));
    right2->setN2(Vector3(-1.0f,  0.0f,  0.0f));
    right2->setN3(Vector3(-1.0f,  0.0f,  0.0f));

    Triangle* t8 = new Triangle;
    t8->setIndex(0);
    t8->setMesh(right2);
    t8->setMaterial(mat);
    g_scene->addObject(t8);

    TriangleMesh *roof1 = new TriangleMesh;
    roof1->createSingleTriangle();

    roof1->setV1(Vector3(-1.0f,  1.0f,  2.0f));
    roof1->setV2(Vector3( 1.0f, -1.0f,  2.0f));
    roof1->setV3(Vector3( 1.0f,  1.0f,  2.0f));

    roof1->setN1(Vector3( 0.0f,  0.0f, -1.0f));
    roof1->setN2(Vector3( 0.0f,  0.0f, -1.0f));
    roof1->setN3(Vector3( 0.0f,  0.0f, -1.0f));

    Triangle* t9 = new Triangle;
    t9->setIndex(0);
    t9->setMesh(roof1);
    //t9->setMaterial(new Lambert(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.2f)));
    t9->setMaterial(mat);
    g_scene->addObject(t9);

    TriangleMesh *roof2 = new TriangleMesh;
    roof2->createSingleTriangle();

    roof2->setV1(Vector3(-1.0f,  1.0f,  2.0f));
    roof2->setV2(Vector3(-1.0f, -1.0f,  2.0f));
    roof2->setV3(Vector3( 1.0f, -1.0f,  2.0f));

    roof2->setN1(Vector3( 0.0f,  0.0f, -1.0f));
    roof2->setN2(Vector3( 0.0f,  0.0f, -1.0f));
    roof2->setN3(Vector3( 0.0f,  0.0f, -1.0f));

    Triangle* t10 = new Triangle;
    t10->setIndex(0);
    t10->setMesh(roof2);
    t10->setMaterial(mat);
    g_scene->addObject(t10);

    // let objects do pre-calculations if needed
    g_scene->preCalc();
}


void
makeCornellBoxScene(int width, int height)
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(width, height);

    // set up the camera
    g_camera->setBGColor(bg);
    g_camera->setEye(Vector3(0, -4, 1));
    g_camera->setLookAt(Vector3(0, 0, 1));
    g_camera->setUp(Vector3(0, 0, 1));
    g_camera->setFOV(40);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(0.80f, -0.90f, 1.0f));
    light->setColor(Vector3(1.0f, 1.0f, 1.0f));
    light->setWattage(5);
    g_scene->addLight(light);

    light = new PointLight;
    light->setPosition(Vector3(-0.80f, -0.90f, 1.0f));
    light->setColor(Vector3(1.0f, 1.0f, 1.0f));
    light->setWattage(5);
    g_scene->addLight(light);

/*    light = new PointLight;*/
    //light->setPosition(Vector3(0.0f, -0.60f, 1.2f));
    //light->setColor(Vector3(1.0f, 1.0f, 1.0f));
    //light->setWattage(5);
    //g_scene->addLight(light);


    TriangleMesh *floor1 = new TriangleMesh;
    floor1->createSingleTriangle();

    floor1->setV1(Vector3(-1.0f, -1.0f,  0.0f));
    floor1->setV2(Vector3( 1.0f, -1.0f,  0.0f));
    floor1->setV3(Vector3(-1.0f,  1.0f,  0.0f));

    floor1->setN1(Vector3( 0.0f,  0.0f,  1.0f));
    floor1->setN2(Vector3( 0.0f,  0.0f,  1.0f));
    floor1->setN3(Vector3( 0.0f,  0.0f,  1.0f));

    Triangle* t = new Triangle;
    t->setIndex(0);
    t->setMesh(floor1);
    t->setMaterial(new Marbre());
    g_scene->addObject(t);

    TriangleMesh *floor2 = new TriangleMesh;
    floor2->createSingleTriangle();

    floor2->setV1(Vector3( 1.0f, -1.0f,  0.0f));
    floor2->setV2(Vector3( 1.0f,  1.0f,  0.0f));
    floor2->setV3(Vector3(-1.0f,  1.0f,  0.0f));

    floor2->setN1(Vector3( 0.0f,  0.0f,  1.0f));
    floor2->setN2(Vector3( 0.0f,  0.0f,  1.0f));
    floor2->setN3(Vector3( 0.0f,  0.0f,  1.0f));

    Triangle* t2 = new Triangle;
    t2->setIndex(0);
    t2->setMesh(floor2);
    t2->setMaterial(new Marbre());
    g_scene->addObject(t2);


    TriangleMesh *left1 = new TriangleMesh;
    left1->createSingleTriangle();

    left1->setV1(Vector3(-1.0f, -1.0f,  0.0f));
    left1->setV2(Vector3(-1.0f,  1.0f,  0.0f));
    left1->setV3(Vector3(-1.0f, -1.0f,  2.0f));

    left1->setN1(Vector3( 1.0f,  0.0f,  0.0f));
    left1->setN2(Vector3( 1.0f,  0.0f,  0.0f));
    left1->setN3(Vector3( 1.0f,  0.0f,  0.0f));

    Triangle* t3 = new Triangle;
    t3->setIndex(0);
    t3->setMesh(left1);
    t3->setMaterial(new Lambert(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.2f, 0.0f, 0.0f)));
    g_scene->addObject(t3);

    TriangleMesh *left2 = new TriangleMesh;
    left2->createSingleTriangle();

    left2->setV1(Vector3(-1.0f,  1.0f,  0.0f));
    left2->setV2(Vector3(-1.0f,  1.0f,  2.0f));
    left2->setV3(Vector3(-1.0f, -1.0f,  2.0f));

    left2->setN1(Vector3( 1.0f,  0.0f,  0.0f));
    left2->setN2(Vector3( 1.0f,  0.0f,  0.0f));
    left2->setN3(Vector3( 1.0f,  0.0f,  0.0f));

    Triangle* t4 = new Triangle;
    t4->setIndex(0);
    t4->setMesh(left2);
    t4->setMaterial(new Lambert(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.2f, 0.0f, 0.0f)));
    g_scene->addObject(t4);

    TriangleMesh *back1 = new TriangleMesh;
    back1->createSingleTriangle();

    back1->setV1(Vector3(-1.0f,  1.0f,  0.0f));
    back1->setV2(Vector3( 1.0f,  1.0f,  0.0f));
    back1->setV3(Vector3(-1.0f,  1.0f,  2.0f));

    back1->setN1(Vector3( 0.0f, -1.0f,  0.0f));
    back1->setN2(Vector3( 0.0f, -1.0f,  0.0f));
    back1->setN3(Vector3( 0.0f, -1.0f,  0.0f));

    Triangle* t5 = new Triangle;
    t5->setIndex(0);
    t5->setMesh(back1);
    //t5->setMaterial(new Lambert(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.2f, 0.2f, 0.2f)));
    //t5->setMaterial(new Mirror());
    t5->setMaterial(new Stone());
    g_scene->addObject(t5);

    TriangleMesh *back2 = new TriangleMesh;
    back2->createSingleTriangle();

    back2->setV1(Vector3( 1.0f,  1.0f,  0.0f));
    back2->setV2(Vector3( 1.0f,  1.0f,  2.0f));
    back2->setV3(Vector3(-1.0f,  1.0f,  2.0f));

    back2->setN1(Vector3( 0.0f, -1.0f,  0.0f));
    back2->setN2(Vector3( 0.0f, -1.0f,  0.0f));
    back2->setN3(Vector3( 0.0f, -1.0f,  0.0f));

    Triangle* t6 = new Triangle;
    t6->setIndex(0);
    t6->setMesh(back2);
    //t6->setMaterial(new Lambert(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.2f, 0.2f, 0.2f)));
    //t6->setMaterial(new Mirror());
    t6->setMaterial(new Stone());
    g_scene->addObject(t6);

    TriangleMesh *right1 = new TriangleMesh;
    right1->createSingleTriangle();

    right1->setV1(Vector3( 1.0f,  1.0f,  0.0f));
    right1->setV2(Vector3( 1.0f, -1.0f,  0.0f));
    right1->setV3(Vector3( 1.0f, -1.0f,  2.0f));

    right1->setN1(Vector3(-1.0f,  0.0f,  0.0f));
    right1->setN2(Vector3(-1.0f,  0.0f,  0.0f));
    right1->setN3(Vector3(-1.0f,  0.0f,  0.0f));

    Triangle* t7 = new Triangle;
    t7->setIndex(0);
    t7->setMesh(right1);
    t7->setMaterial(new Lambert(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.2f, 0.0f)));
    g_scene->addObject(t7);

    TriangleMesh *right2 = new TriangleMesh;
    right2->createSingleTriangle();

    right2->setV1(Vector3( 1.0f,  1.0f,  0.0f));
    right2->setV2(Vector3( 1.0f, -1.0f,  2.0f));
    right2->setV3(Vector3( 1.0f,  1.0f,  2.0f));

    right2->setN1(Vector3(-1.0f,  0.0f,  0.0f));
    right2->setN2(Vector3(-1.0f,  0.0f,  0.0f));
    right2->setN3(Vector3(-1.0f,  0.0f,  0.0f));

    Triangle* t8 = new Triangle;
    t8->setIndex(0);
    t8->setMesh(right2);
    t8->setMaterial(new Lambert(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.2f, 0.0f)));
    g_scene->addObject(t8);

    TriangleMesh *roof1 = new TriangleMesh;
    roof1->createSingleTriangle();

    roof1->setV1(Vector3(-1.0f,  1.0f,  2.0f));
    roof1->setV2(Vector3( 1.0f, -1.0f,  2.0f));
    roof1->setV3(Vector3( 1.0f,  1.0f,  2.0f));

    roof1->setN1(Vector3( 0.0f,  0.0f, -1.0f));
    roof1->setN2(Vector3( 0.0f,  0.0f, -1.0f));
    roof1->setN3(Vector3( 0.0f,  0.0f, -1.0f));

    Triangle* t9 = new Triangle;
    t9->setIndex(0);
    t9->setMesh(roof1);
    //t9->setMaterial(new Lambert(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.2f)));
    t9->setMaterial(new Stone(1));
    g_scene->addObject(t9);

    TriangleMesh *roof2 = new TriangleMesh;
    roof2->createSingleTriangle();

    roof2->setV1(Vector3(-1.0f,  1.0f,  2.0f));
    roof2->setV2(Vector3(-1.0f, -1.0f,  2.0f));
    roof2->setV3(Vector3( 1.0f, -1.0f,  2.0f));

    roof2->setN1(Vector3( 0.0f,  0.0f, -1.0f));
    roof2->setN2(Vector3( 0.0f,  0.0f, -1.0f));
    roof2->setN3(Vector3( 0.0f,  0.0f, -1.0f));

    Triangle* t10 = new Triangle;
    t10->setIndex(0);
    t10->setMesh(roof2);
    //t10->setMaterial(new Lambert(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.2f)));
    t10->setMaterial(new Stone(1));
    g_scene->addObject(t10);

    Sphere * sphere = new Sphere;
    sphere->setCenter(Vector3(0.40f, -0.20f, 0.301f));
    sphere->setRadius(0.3);
    sphere->setMaterial(new Glass(Vector3(0.5f, 0.0f, 0.0f)));
    g_scene->addObject(sphere);

    sphere = new Sphere;
    sphere->setCenter(Vector3(-0.5f, 0.50f, 0.401f));
    sphere->setRadius(0.4);
    sphere->setMaterial(new Mirror());
    g_scene->addObject(sphere);

    sphere = new Sphere;
    sphere->setCenter(Vector3(-0.4f, -0.30f, 0.0f));
    sphere->setRadius(0.2);
    sphere->setMaterial(new Marbre());
    g_scene->addObject(sphere);



    // let objects do pre-calculations if needed
    g_scene->preCalc();
}

void
makeBunnyScene(int width, int height)
{

    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(width, height);

    // set up the camera
    g_camera->setBGColor(bg);
    g_camera->setEye(Vector3(0.15, 2, -6));
    g_camera->setLookAt(Vector3(0.15, 1, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(40);

/*    int w, h;*/
    //Vector3 *img = readPFMImage("./grace_probe.pfm", &w, &h);

    //g_camera->environment_map(img, Camera::SPHERICAL_MAPPING, w, h);



    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3.0f, 8.0f, -8.0f));
    light->setColor(Vector3(1.0f, 1.0f, 1.0f));
    light->setWattage(300);
    g_scene->addLight(light);

    light = new PointLight;
    light->setPosition(Vector3(3.0f, 8.0f, -8.0f));
    light->setColor(Vector3(1.0f, 1.0f, 1.0f));
    light->setWattage(300);
    g_scene->addLight(light);

    Material* mat = new Lambert(Vector3(1.0f));
    //Material* mat1 = new Lambert(1.0f);
    Material* mat1 = new Mirror();
    //Material* mat1 = new Glass(1.0f);
    //Material* mat1 = new Marbre();
    //Material* mat1 = new Stone(0);

    TriangleMesh * bunny = new TriangleMesh;
    //bunny->load("bunny_smooth.obj");
    //bunny->load("sponza.obj");
    //bunny->load("cornel_box.obj");
    //bunny->load("sphere.obj");
    bunny->load("smooth_sphere.obj");
    //bunny->load("./models/teapot.obj");

     //create all the triangles in the bunny mesh and add to the scene
    for (int i = 0; i < bunny->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(bunny);
        t->setMaterial(mat1);
        g_scene->addObject(t);
    }


  /*  Sphere * sphere = new Sphere;*/
    //sphere->setCenter(Vector3(0.0f, -0.5f, 8.0f));
    //sphere->setRadius(1.0);
    //sphere->setMaterial(mat);
    //g_scene->addObject(sphere);


    TriangleMesh *floor1 = new TriangleMesh;
    floor1->createSingleTriangle();

    floor1->setV1(Vector3(-10.0f,  -2.0f, -10.0f));
    floor1->setV2(Vector3( 10.0f,  -2.0f, -10.0f));
    floor1->setV3(Vector3(-10.0f,  -2.0f,  10.0f));

    floor1->setN1(Vector3( 0.0f,  1.0f,  0.0f));
    floor1->setN2(Vector3( 0.0f,  1.0f,  0.0f));
    floor1->setN3(Vector3( 0.0f,  1.0f,  0.0f));

    Triangle* t = new Triangle;
    t->setIndex(0);
    t->setMesh(floor1);
    t->setMaterial(mat);
    g_scene->addObject(t);

    TriangleMesh *floor2 = new TriangleMesh;
    floor2->createSingleTriangle();

    floor2->setV1(Vector3( 10.0f, -2.0f, -10.0f));
    floor2->setV2(Vector3( 10.0f, -2.0f,  10.0f));
    floor2->setV3(Vector3(-10.0f, -2.0f,  10.0f));

    floor2->setN1(Vector3( 0.0f,  1.0f,  0.0f));
    floor2->setN2(Vector3( 0.0f,  1.0f,  0.0f));
    floor2->setN3(Vector3( 0.0f,  1.0f,  0.0f));

    Triangle* t2 = new Triangle;
    t2->setIndex(0);
    t2->setMesh(floor2);
    t2->setMaterial(mat);
    g_scene->addObject(t2);

    g_scene->preCalc();
}

void
makeMirrorSphereScene(int width, int height)
{

    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;


    int w, h;
    Vector3 *img = readPFMImage("./grace_probe.pfm", &w, &h);

    g_camera->environment_map(img, Camera::SPHERICAL_MAPPING, w, h);


    g_image->resize(width, height);

    // set up the camera
    g_camera->setBGColor(bg);
    g_camera->setEye(Vector3(0, 1, 6));
    g_camera->setLookAt(Vector3(0, 0, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(40);

    Material* mat = new Mirror();
    Sphere * sphere = new Sphere;
    sphere->setCenter(Vector3(0.0f));
    sphere->setRadius(1);
    sphere->setMaterial(mat);
    g_scene->addObject(sphere);

    g_scene->preCalc();
}

int
main(int argc, char*argv[])
{
    //const int width = 128, height = 128;
    const int width = 512, height = 512;
    //const int width = 1024, height = 1024;

    //makeSpiralScene(width, height);
    //makeLorenzScene(width, height);
    makeCornellBoxScene(width, height);
    //makeEmptyCornellBoxScene(width, height);
    //makeBunnyScene(width, height);
    //makeMirrorSphereScene(width, height);

#ifdef  NO_OPENGL

    g_camera->setRenderer(Camera::RENDER_RAYTRACE);
    g_camera->click(g_scene, g_image);
    char str[1024];
    sprintf(str, "miro_%d.ppm", time(0));
    g_image->writePPM(str);

#else

    MiroWindow miro(&argc, argv);
    miro.mainLoop();

#endif

    return 0; // never executed
}


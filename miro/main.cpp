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
#include "PointLightShading.h"
#include "MiroWindow.h"

void
makeSpiralScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(512, 512);
    
    // set up the camera
    g_camera->setBGColor(Vector3(1.0f, 1.0f, 1.0f));
    g_camera->setEye(Vector3(-5, 2, 3));
    g_camera->setLookAt(Vector3(0, 0, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(1000);
    g_scene->addLight(light);

    // create a spiral of spheres
    Material* mat = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
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
        sphere->setMaterial(mat);
        g_scene->addObject(sphere);
    }
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}


void
makeBunnyScene()
{
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 3, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 5, 10));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(500);
	g_scene->addLight(light);
	PointLight * light2 = new PointLight;
	light2->setPosition(Vector3(9, 10, 20));
	light2->setColor(Vector3(1, 1, 1));
	light2->setWattage(600);
	g_scene->addLight(light2);

	Material* mat = new Lambert(Vector3(1.0f));

	TriangleMesh * bunny = new TriangleMesh;
	TriangleMesh * bunny2 = new TriangleMesh;
	bunny->load("teapot.obj");
	bunny2->load("teapot.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < bunny->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(bunny);
		t->setMaterial(mat);
		g_scene->addObject(t);
	}

	/*
	for (int i = 0; i < bunny2->numTris(); ++i)
	{
	Triangle* t = new Triangle;
	t->setIndex(i);
	t->setMesh(bunny2);
	t->setMaterial(mat);
	g_scene->addObject(t);
	}
	*/
	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(mat);
	g_scene->addObject(t);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

void
makeTeapotScene()
{
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 3, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 5, 10));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(500);
	g_scene->addLight(light);
	PointLight * light2 = new PointLight;
	light2->setPosition(Vector3(9, 10, 20));
	light2->setColor(Vector3(1, 1, 1));
	light2->setWattage(600);
	g_scene->addLight(light2);

	Material* mat = new Lambert(Vector3(1.0f));

	TriangleMesh * bunny = new TriangleMesh;
	TriangleMesh * bunny2 = new TriangleMesh;
	bunny->load("teapot.obj");
	bunny2->load("teapot.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < bunny->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(bunny);
		t->setMaterial(mat);
		g_scene->addObject(t);
	}

	/*
	for (int i = 0; i < bunny2->numTris(); ++i)
	{
	Triangle* t = new Triangle;
	t->setIndex(i);
	t->setMesh(bunny2);
	t->setMaterial(mat);
	g_scene->addObject(t);
	}
	*/
	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(mat);
	g_scene->addObject(t);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

void
makeSphereScene()
{
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 3, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 5, 10));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(200);
	g_scene->addLight(light);
	PointLight * light2 = new PointLight;
	light2->setPosition(Vector3(15, 10, 5));
	light2->setColor(Vector3(1, 1, 1));
	light2->setWattage(100);
	g_scene->addLight(light2);

	Material* mat = new PointLightShading(Vector3(1.0f));
	int r = 10;
	float x1 = 1;
	float y1 = 1;
	float z1 = 0;
	float x2 = -2;
	float y2 = 1;
	float z2 = 0;
	float x3 = -2;
	float y3 = 1;
	float z3 = 2;

	Sphere * sphere1 = new Sphere;
	sphere1->setCenter(Vector3(x1, y1, z1));
	sphere1->setRadius(r / 10);
	sphere1->setMaterial(mat);
	g_scene->addObject(sphere1);

	Sphere * sphere2 = new Sphere;
	sphere2->setCenter(Vector3(x2, y2, z2));
	sphere2->setRadius(r / 10);
	sphere2->setMaterial(mat);
	g_scene->addObject(sphere2);

	Sphere * sphere3 = new Sphere;
	sphere3->setCenter(Vector3(x3, y3, z3));
	sphere3->setRadius(r / 10);
	sphere3->setMaterial(mat);
	g_scene->addObject(sphere3);

	/*
	for (int i = 0; i < bunny2->numTris(); ++i)
	{
	Triangle* t = new Triangle;
	t->setIndex(i);
	t->setMesh(bunny2);
	t->setMaterial(mat);
	g_scene->addObject(t);
	}
	*/
	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(mat);
	g_scene->addObject(t);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}


int
main(int argc, char*argv[])
{
    // create a scene
	makeSphereScene();

    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}


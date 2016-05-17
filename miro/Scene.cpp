#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#define MINSAMPLES 10
#define MAXSAMPLES 100

Scene * g_scene = 0;

void
Scene::openGL(Camera *cam)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam->drawGL();

    // draw objects
    for (size_t i = 0; i < m_objects.size(); ++i)
        m_objects[i]->renderGL();
	
    glutSwapBuffers();
}

void
Scene::preCalc()
{
    Objects::iterator it;
    for (it = m_objects.begin(); it != m_objects.end(); it++)
    {
        Object* pObject = *it;
        pObject->preCalc();
    }
    Lights::iterator lit;
    for (lit = m_lights.begin(); lit != m_lights.end(); lit++)
    {
        PointLight* pLight = *lit;
        pLight->preCalc();
    }

    m_bvh.build(&m_objects);
}

void
Scene::raytraceImage(Camera *cam, Image *img)
{
    Ray ray;
    HitInfo hitInfo;
    Vector3 shadeResult;
    
// loop over all pixels in the image
#pragma omp parallel for
    for (int j = 0; j < img->height(); ++j) // loop over every y value
    {

#pragma omp parallel for
        for (int i = 0; i < img->width(); ++i) // loop over every x value
        {
			Vector3 sampleResult(0.0f);
			// for # of samples/pixel ranging from (10->100)
			for (int k = 0; k < MINSAMPLES; ++k)
			{
				Vector3 res;
				Ray ray;
				//ray = cam->eyeRay(i, j, img->width(), img->height());
				ray = cam->eyeRayRand(i, j, img->width(), img->height());
				if (trace(hitInfo, ray))
				{
					//sampleResult += hitInfo.material->shade(ray, hitInfo, *this);
					//img->setPixel(i, j, shadeResult);
				}
				if (trace(ray, res, 0))
				{
					sampleResult += res;
				}
			}
			sampleResult = sampleResult*5 / (float)MINSAMPLES;
			img->setPixel(i, j, sampleResult);
		}
        img->drawScanline(j);
        glFinish();
        printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
        fflush(stdout);
    }
    
    printf("Rendering Progress: 100.000%\n");
    debug("done Raytracing!\n");
	//std::cout << "Number of Rays: " << m_bvh.getCounter()->getBoxI() << "rayBoxIntersections: " << m_bvh.getCounter()->getBoxI() << " rayTriangleIntersections: " << m_bvh.getCounter()->getTriI() << std::endl;
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    return m_bvh.intersect(minHit, ray, tMin, tMax);
}

bool 
Scene::trace(const Ray& ray, Vector3& res, int bounces) {
	res = Vector3(0.0f);
	HitInfo hit;
	if (bounces < 32) {
		if (trace(hit, ray))
		{
			// if a hit then trace
			res = hit.material->shade(ray, hit, *this);
			// check diffuse
			if (hit.material->isDiffuse()) 
			{
				if ((float)rand() / (float)RAND_MAX < .5f)
					res = 2.0f * hit.material->shade(ray, hit, *this);
				else
				{
					Ray diff = ray.diffuse(hit);
					Vector3 diff_res;
					if (trace(diff, diff_res, bounces+1))
					{
						// recurse for diffusion
						res += diff_res * hit.material->getDiffuse() * .7f;
					}
				}
			}
			// check specular reflection
			if (hit.material->isReflection()) 
			{
				Ray reflection = ray.reflect(hit);
				Vector3 reflectionRes;
				// recurse on reflection ray
				if (trace(reflection, reflectionRes, bounces+1)) 
				{
					res += reflectionRes * hit.material->getReflection();
				}
			}
			if (hit.material->isRefraction()) 
			{ // check specular refraction
				Ray refraction = ray.refract(hit);
				Vector3 refractionRes;
				// recurse on refraction ray
				if (trace(refraction, refractionRes, bounces+1)) 
				{
					res += refractionRes * hit.material->getRefraction();
				}
			}
		}
		return true;
	}
	return false;
}

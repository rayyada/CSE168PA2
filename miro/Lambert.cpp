#define NOMINMAX
#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>


Lambert::Lambert(const Vector3 & kd, const Vector3 & ka, const Vector3 &ks) :
    m_kd(kd), m_ka(ka), m_ks(ks)
{

}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_ks = { 0.0,0.5,1.0 };
    
    const Vector3 viewDir = -ray.d; // d is a unit vector
    
    const Lights *lightlist = scene.lights();
	Ray tempRay;
    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
		/*
        PointLight* pLight = *lightIter;

		Vector3 l = pLight->position() - hit.P;
        
        // the inverse-squared falloff
        float falloff = l.length2();
        
        // normalize the light direction
        l /= sqrt(falloff);

        // get the diffuse component
        float nDotL = dot(hit.N, l);
        Vector3 result = pLight->color();
        result *= m_kd;
        
        L += (std::max)(0.0f, nDotL/falloff * pLight->wattage() / PI) * result;

		// specular stuff
		Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;
		tempRay.d = l;
		tempRay.o = hit.P;
		Vector3 result_spec_ref = pLight->color() * m_ks;

		L += (std::max)(0.0f, (nDotL / falloff) * (pLight->wattage() / PI)) * result_spec_ref;
		*/
		PointLight* pLight = *lightIter;

		Vector3 l = pLight->position() - hit.P;

		// the inverse-squared falloff 
		float falloff = 1.0f / (4.0f * PI * l.length2());

		// normalize the light direction
		l.normalize();

		// get the diffuse component
		float nDotL = dot(hit.N, l);

		Vector3 result = pLight->color();
		result *= Vector3(m_kd);

		// E = (phi * (n dot l)) / 4 * PI * r^2
		float irradiance = pLight->wattage() * nDotL * falloff;

		L += (std::max(0.0f, irradiance)) * result;


		// get the specular component
			Vector3 r = (-l + 2 * dot(l, hit.N) * hit.N).normalized();

			float eDotR = dot(viewDir, r);
			eDotR = 0.0f > eDotR ? 0.0f : 1.0f < eDotR ? 1.0f : eDotR; // clamp it to [0..1]
			eDotR = pow(eDotR, 10);
			//L += std::max(0.0f, eDotR * falloff * pLight->wattage());
    }
    
    // add the ambient component
    L += m_ka;
    
	Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;
	Ray tempRay2; HitInfo tempHit2;
	tempRay2.d = wr;
	tempRay2.o = hit.P;
	scene.trace(tempHit2, tempRay2);
	//if (tempHit2.t >= 0.0001f)
	//{
		//L += reflection(tempRay2, tempHit2, scene);
	//}

    return L;
}

Vector3
Lambert::reflection(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	Vector3 m_ks = { 0.0,0.5,1.0 };
	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
	HitInfo tempHit;
	Ray tempRay;
	const Vector3 viewDir = -ray.d; // d is a unit vector

	const Lights *lightlist = scene.lights();

	// loop over all of the lights
	Lights::const_iterator lightIter;
	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
	{
		PointLight* pLight = *lightIter;

		Vector3 l = pLight->position() - hit.P;

		// the inverse-squared falloff
		float falloff = l.length2();

		// normalize the light direction
		l /= sqrt(falloff);

		// get the diffuse component
		float nDotL = dot(hit.N, l);
		Vector3 result = pLight->color();
		result *= m_kd;

		L += (std::max(0.0f, (nDotL / falloff) * (pLight->wattage() / PI)) * result);
	}
	L += m_ka;

	return L;
}
Vector3
Lambert::refraction(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	return Vector3(1);
}
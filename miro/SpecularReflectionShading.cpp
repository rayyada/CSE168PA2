#define NOMINMAX
#include "SpecularReflectionShading.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

SpecularReflectionShading::SpecularReflectionShading(const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka)
{

}

SpecularReflectionShading::~SpecularReflectionShading()
{
}

Vector3
SpecularReflectionShading::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	
	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_kr = { 0.0,0.5,1.0 };
	Vector3 m_ks = { 0.4f,0.4f,0.4f };
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
		Vector3 resultks = pLight->color();
		resultks *= m_ks;
		L += std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * result;

		// get the reflection component
		Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;
		HitInfo tempHit2;
		Ray tempRay2;
		tempRay2.d = l;
		tempRay2.o = hit.P;
		scene.trace(tempHit2, tempRay2);
		L += (std::max(0.0f, (nDotL / falloff) * (pLight->wattage() / PI)) * resultks);
	}

	// add the ambient component
	L += m_ka;

	return L;
}

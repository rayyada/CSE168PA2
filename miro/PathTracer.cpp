#include "PathTracer.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

#define MAXBOUNCES 36
int bounces = 0;


Pathtracer::Pathtracer(const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka)
{

}

Pathtracer::~Pathtracer()
{
}

Vector3
Pathtracer::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_ks = Vector3(1.0) - m_kd;
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

		Ray shadow(hit.P + (l * epsilon), l);
		HitInfo shadowHit;
		if (scene.trace(shadowHit, shadow, 0.0f, sqrt(falloff))) {
			continue;
		}
		// get the diffuse component
		float nDotL = dot(hit.N, l);
		Vector3 diffuse = pLight->color();
		Vector3 specular = pLight->color();
		Vector3 W_r = 2 * dot(l, hit.N)*hit.N - l; // E
		W_r.normalize();
		diffuse *= m_kd;
		specular *= m_ks;

		L += std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * diffuse;
		// highlights
		L += std::max(0.0f, powf(dot(viewDir, W_r), 100)) * specular;
	}

	// add the ambient component
	L += m_ka;
	return L;
}
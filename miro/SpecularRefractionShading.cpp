#define NOMINMAX
#include "SpecularRefractionShading.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

SpecularRefractionShading::SpecularRefractionShading(const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka)
{
}

SpecularRefractionShading::~SpecularRefractionShading()
{
}

Vector3
SpecularRefractionShading::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

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

		L += std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * result;

		Vector3 r = (-l + 2 * dot(l, hit.N) * hit.N).normalized();

		float eDotR = dot(viewDir, r);
		eDotR = 0.0f > eDotR ? 0.0f : 1.0f < eDotR ? 1.0f : eDotR; // clamp it to [0..1]
		eDotR = pow(eDotR, 3);
		L += std::max(0.0f, eDotR * falloff * pLight->wattage());
	}

	// add the ambient component
	L += m_ka;

	return L;
}

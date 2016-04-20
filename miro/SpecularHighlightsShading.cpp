
#include "Miro.h"
#include "SpecularHighlightsShading.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

SpecularHighlightsShading::SpecularHighlightsShading(const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka)
{

}

SpecularHighlightsShading::~SpecularHighlightsShading()
{
}

Vector3
SpecularHighlightsShading::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	Vector3 ia = Vector3(10.0f, 20.0f, 30.0f);

	Vector3 rayDir = ray.d; // d is a unit vector
	const Vector3 rayOrigin = ray.o; // d is a unit vector
	const Vector3 hitPoint = hit.P ; // d is a unit vector
	const Vector3 hitNormal = hit.N; // d is a unit vector
	rayDir.normalize();
	float ks = 10.0;
	float kd = 10.0;
	float ka = 4.0;
	float shininess = 160.0;


	Vector3 L = ka*ia;

	const Lights *lightlist = scene.lights();

	// loop over all of the lights
	Lights::const_iterator lightIter;
	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
	{
		PointLight* pLight = *lightIter;

		Vector3 l = pLight->position() - hit.P;
		l.normalize();
		Vector3 reflectance = 2 * (dot(l, hitNormal))*hitNormal - l;
		reflectance.normalize();
		// the inverse-squared falloff
		float falloff = l.length2();

		// normalize the light direction
		//l /= sqrt(falloff);

		// get the diffuse component
		float nDotL = dot(hit.N, l);
		Vector3 result = pLight->color();
		result *= m_kd;

		L += (kd * dot(l, hitNormal) * kd) + (ks * pow(dot(reflectance, rayDir), shininess) * ks);
	}

	// add the ambient component
	L += m_ka;

	return L;
}

#include "PointLightShading.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

PointLightShading::PointLightShading(const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka)
{

}

PointLightShading::~PointLightShading()
{
}

Vector3
PointLightShading::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{

	float reflectionRadians = acos(dot(ray.d, hit.N) / (ray.d.length() * hit.N.length()));
	// Let's assume for now that all rays go from the air to the object, which can be any material
	float refractionRadians = asin((1.00029 * sin(reflectionRadians)) / refractionIndex);

	Vector3 m_ks = { 0.0,0.5,1.0 };
	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
	HitInfo tempHit, tempHit2;
	Ray tempRay, tempRay2;
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

		Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;
		tempRay.d = l;
		tempRay.o = hit.P;

		//Vector3 reflectedVector = ray.d + (2 * dot(ray.d, hit.N) * hit.N);

		if (scene.trace(tempHit, tempRay) == true)
		{
			L += 0;
		}
		else
		{
			L += (std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * result);
		}
	}
	/*
	Ld(x, ~?) = Rd ? E
	L = Radiance
	E = Irradiance
	m_ka = ambiant coef
	m_kd = diffuse coef
	m_wattage = Phi

	Ediffuse = lightColor * diffuse coef

	ws = specular direction
	phi is the light source power
	Ls is color returned by the specular ray
	Lt is color returned by the transmitted ray
	*/
	// add the ambient component
	L += m_ka;

	//Reflective stuff
	Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;

	float c1 = -dot(hit.N, ray.d);

	tempRay2.d = wr;
	tempRay2.o = hit.P;
	scene.trace(tempHit2, tempRay2);
	if (tempHit2.t >= 0.0001f)
	{
		L += reflectionColor(tempRay2, tempHit2, scene);
	}


	//Refractive stuff
	float vacuum, air, ice, water, glass, diamond;
	vacuum = 1.0f;
	air = 1.00029f;
	ice = 1.31f;
	water = 1.33f;
	glass = 1.65f;
	diamond = 2.417;

	float n1, n2;
	n1 = air;
	n2 = air;
	//n1 = index of refraction of original medium
	//n2 = index of refraction of new medium
	float refraction = n1 / n2;
	float c2 = sqrt(1 - pow(refraction,2) * (1 - pow(c1, 2)));
	Vector3 Rr = (refraction * ray.d) + (refraction * c1 - c2) * hit.N;

	tempRay2.d = Rr;
	tempRay2.o = hit.P;
	scene.trace(tempHit2, tempRay2);
	if (tempHit2.t >= 0.0001f)
	{
		//L += reflectionColor(tempRay2, tempHit2, scene);
	}

	return L;
}


Vector3
PointLightShading::reflectionColor(const Ray& ray, const HitInfo& hit, const Scene& scene) const
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

		Vector3 resultks = pLight->color();
		resultks *= m_ks;

		Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;
		tempRay.d = l;
		tempRay.o = hit.P;

		L += (std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * resultks);
		
	}
	L += m_ka;

	return L;
}

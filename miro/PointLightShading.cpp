#define NOMINMAX
#include "PointLightShading.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

PointLightShading::PointLightShading(const Vector3 & kd, const Vector3 & ka, float rI) :
	m_kd(kd), m_ka(ka), refractionIndex(rI)
{
	// m_kd = diffusion
	// m_ka = ambient
	// m_kt = refraction
	// m_ks = reflection
	// indx = refraction index
	Vector3 split = Vector3(1.0f) - m_kd;
	m_ks = Vector3(split / 2);
	m_kr = Vector3(split / 2);
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

	//Vector3 m_ks = { 0.4f,0.4f,0.4f };
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

		// get the diffuse component  -- pow((ray.d * l),n.normalize)/ (n * l) * m_kd
		float nDotL = dot(hit.N, l); // E
		Vector3 result = pLight->color() * m_kd;              // R_d

		Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;

		tempRay2.d = wr;
		tempRay2.o = hit.P;
		scene.trace(tempHit2, tempRay2);
		if (tempHit2.t > 0.0001f)
		{
			L += (std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * result);
		}
		
		// highlights
		Vector3 resultks = pLight->color() * m_ks; // R_s
		Vector3 W_r = 2 * dot(l, hit.N)*hit.N - l; // E
		W_r.normalize();
		// higher the power, the sharper the reflection
		L += std::max(0.0f, powf(dot(viewDir, W_r), 200)) * resultks;
	}
		/*
		Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;
		tempRay.d = l;
		tempRay.o = hit.P; */
		/*
				//Vector3 reflectedVector = ray.d + (2 * dot(ray.d, hit.N) * hit.N);

				//if (scene.trace(tempHit, tempRay) == true) L += 0;
				//else L += (std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * result);

				// highlights
				Vector3 r = dot(ray.d, l);
				Vector3 h = dot(hit.N, l);
				Vector3 d = Vector3(pow(r.x, hit.N.length()), pow(r.y, hit.N.length()), pow(r.z, hit.N.length()));
				Vector3 a = d / h;

				Vector3 rv = (-l + 2 * dot(l, hit.N) * hit.N).normalized();

				float eDotR = dot(viewDir, rv);
				eDotR = 0.0f > eDotR ? 0.0f : 1.0f < eDotR ? 1.0f : eDotR; // clamp it to [0..1]
				eDotR = pow(eDotR, .5f);
				//L += std::max(0.0f, eDotR * falloff * pLight->wattage());

				if (scene.trace(tempHit, tempRay) == true)
					L += 0;
					//L += std::max(0.0f, eDotR / falloff * pLight->wattage()/PI);
					//L += std::max(0.0f, eDotR / falloff * pLight->wattage() / PI);
					//L += std::max(0.0f, (eDotR * falloff) * (pLight->wattage() / (4 * PI)));

				else
					//L += (std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * resultks * a);
					//L += std::max(0.0f, eDotR * falloff * pLight->wattage());
					//L += std::max(0.0f, (eDotR * falloff) * (pLight->wattage() / (4* PI)));
					L += 0;
		*/
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

	//Reflective stuff
	Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;

	tempRay2.d = wr;
	tempRay2.o = hit.P;
	scene.trace(tempHit2, tempRay2);
	//if (tempHit2.t >= 0.0001f)
	//{
		L += reflectionColor(tempRay2, tempHit2, scene);
	//}

	
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
	n2 = diamond;
	
	//n1 = index of refraction of original medium
	//n2 = index of refraction of new medium
	// w_t = -refr(ray.D-(do
	float refraction = n1/this->refractionIndex;
	float c1 = -dot(hit.N, ray.d);
	float c2 = sqrt(1 - pow(refraction,2) * (1 + pow(c1, 2)));
	Vector3 p1 = -refraction*(ray.d + c1*hit.N);
	Vector3 p2 = c2 * hit.N;
	//Vector3 wt = p1 - p2;
	//Vector3 Rr = (refraction * ray.d) + (refraction * c1 - c2) * hit.N;
	Vector3 Rr = -refraction*(ray.d + c1) - c2*hit.N;
	Vector3 wt = -(n1 / this->refractionIndex)*(ray.d - dot(ray.d, hit.N)*hit.N);
	wt = wt - (sqrt(1 - (pow(refraction, 2)*(1 - (pow(dot(ray.d, hit.N), 2)))))) * hit.N;
	//tempRay2.d = wt;
	tempRay2.d = -wt;
	tempRay2.o = hit.P;
	scene.trace(tempHit2, tempRay2);
	//if (tempHit2.t >= 0.0001f)
	//{
		L += refractionColor(tempRay2, tempHit2, scene);
	//} 

	// add the ambient component
	L += m_ka;

	return L;
}


Vector3
PointLightShading::reflectionColor(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{

	Vector3 m_ks = { 0.25,0.25,0.25 };
	Vector3 m_kr = { 0.25,0.25,0.25 };
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
		
		// diffuse result
		Vector3 resultkd = pLight->color() * m_kd;
		// specular reflection result
		Vector3 resultks = pLight->color() * m_ks;

		Vector3 wr = -2 * dot(ray.d, hit.N) * hit.N + ray.d;
		tempRay.d = l;
		tempRay.o = hit.P;
		//if (tempHit.t >= 0.0001f)
		L += (std::max(0.0f, (nDotL / falloff) * (pLight->wattage() / PI)) * resultks);
		
		Vector3 r = (-l + 2 * dot(l, hit.N) * hit.N).normalized();

		float eDotR = dot(viewDir, r);
		eDotR = 0.0f > eDotR ? 0.0f : 1.0f < eDotR ? 1.0f : eDotR; // clamp it to [0..1]
		eDotR = pow(eDotR, 10);
		//if (tempHit.t >= 0.0001f) {}
		L += std::max(0.0f, eDotR / falloff * pLight->wattage()/PI) * resultks;
		
	}
	L += m_ka;

	return L;
}

Vector3
PointLightShading::refractionColor(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	
	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

	const Vector3 viewDir = -ray.d; // d is a unit vector

	const Lights *lightlist = scene.lights();

	HitInfo tempHit;
	Ray tempRay;
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

		// diffusion
		Vector3 result = pLight->color() * m_kd;
		Vector3 resultr = pLight->color() * m_kr * m_kd;		

		float vacuum, air, ice, water, glass, diamond;
		vacuum = 1.0f;
		air = 1.00029f;
		ice = 1.31f;
		water = 1.33f;
		glass = 1.65f;
		diamond = 2.417;

		float n1, n2;
		n1 = air;
		n2 = glass;

		//n1 = index of refraction of original medium
		//n2 = index of refraction of new medium
		float refraction = air/this->refractionIndex;
		float c1 = -dot(ray.d, hit.N);
		float c2 = sqrt(1 - pow(refraction, 2) * (1 - pow(c1, 2)));
		Vector3 p1 = -refraction*(ray.d + c1*hit.N);
		Vector3 p2 = c2 * hit.N;
		//Vector3 wt = p1 - p2;
		//Vector3 Rr = (refraction * ray.d) + (refraction * c1 - c2) * hit.N;
		Vector3 wt = -(n1 / this->refractionIndex)*(ray.d - dot(ray.d, hit.N)*hit.N);
		wt = wt - (sqrt(1 - (pow(refraction, 2)*(1 - (pow(dot(ray.d, hit.N), 2))))))*hit.N;

		//Vector3 Rr = -refraction*(ray.d + c1) - c2*hit.N;
		
		tempRay.d = wt;
		tempRay.o = hit.P;
		scene.trace(tempHit, tempRay);
		//if (tempHit.t >= 0.0001f)
		//{
			//L += (std::max(0.0f, nDotL / falloff * pLight->wattage() / PI) * result);
			L += (std::max(0.0f, nDotL / falloff * pLight->wattage() / PI)) * resultr;

		//}
	
	}

	// add the ambient component
	//L += m_ka;
	return L;
	
	//return Vector3(1);
}

Vector3
PointLightShading::highlightColor(const Ray& ray, const HitInfo& hit, const Scene& scene) const {
	return Vector3(1.0f);
}

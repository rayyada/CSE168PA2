#include "Ray.h"
#include "Scene.h"
#include "Stone.h"
#include <algorithm>


void preCalc() {}

Vector3 StoneTexture::shade(const Ray& ray, const HitInfo& hit,
	const Scene& scene) const
{
	float at[3] = { hit.P.x, hit.P.y, hit.P.z };
	const long  maxOrder = 2;
	float F[maxOrder];
	float delta[maxOrder][3];
	unsigned long ID[maxOrder];

	WorleyNoise::noise3D(at, maxOrder, F, delta, ID);
	return Vector3(delta[maxOrder][0], delta[maxOrder][1], delta[maxOrder][2]);
	//debug("at: %f,%f,%f\t maxOrder: %ld\t F[0]: %f\t F[1]: %f\t delta[0]: %f, %f, %f\t ID[0]: %lu\t ID[1]: %lu\n", at[0], at[1], at[2], maxOrder, F[0], F[1], delta[0][0], delta[0][1], delta[0][2], ID[0], ID[1]);

}
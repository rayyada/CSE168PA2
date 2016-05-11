#pragma once
#include "Material.h"
#include "Worley.h"
#include "Perlin.h"
/* stone texture */

class StoneTexture : public Material {
public:
	StoneTexture() {}
	~StoneTexture() {}
	virtual void preCalc() {}

	virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
		const Scene& scene) const;
};

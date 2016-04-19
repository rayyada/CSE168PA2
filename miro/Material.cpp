#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

Vector3
Material::shade(const Ray& ray, const HitInfo& hit,
	const Scene& scene) const
{
    return Vector3(1.0f, 1.0f, 1.0f);
}

#ifndef CSE168_PATHTRACER_H_INCLUDED
#define CSE168_PATHTRACER_H_INCLUDED

#include "Material.h"

class Pathtracer : public Material
{
public:
	Pathtracer(const Vector3 & kd = Vector3(1),
		const Vector3 & ka = Vector3(0));
	virtual ~Pathtracer();

	const Vector3 & kd() const { return m_kd; }
	const Vector3 & ka() const { return m_ka; }

	void setKd(const Vector3 & kd) { m_kd = kd; }
	void setKa(const Vector3 & ka) { m_ka = ka; }

	virtual void preCalc() {}

	virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
		const Scene& scene) const;
	Vector3 RayTrace(const Ray& r, const HitInfo& h, const Scene& s) const;
protected:
	Vector3 m_kd;
	Vector3 m_ka;
};

#endif // CSE168_LAMBERT_H_INCLUDED
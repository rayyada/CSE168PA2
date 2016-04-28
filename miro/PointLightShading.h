#ifndef CSE168_POINTLIGHTSHADING_H_INCLUDED
#define CSE168_POINTLIGHTSHADING_H_INCLUDED

#include "Miro.h"
#include "Material.h"

class PointLightShading : public Material
{
public:
	PointLightShading(const Vector3 & kd = Vector3(1),
		const Vector3 & ka = Vector3(1), const float ri = 1.0f);
	virtual ~PointLightShading();

	const Vector3 & kd() const { return m_kd; }
	const Vector3 & ka() const { return m_ka; }

	void setKd(const Vector3 & kd) { m_kd = kd; }
	void setKa(const Vector3 & ka) { m_ka = ka; }
	void setRefractionIndex(const float index) { refractionIndex = index; }

	virtual void preCalc() {}

	virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
		const Scene& scene) const;
	Vector3 reflectionColor(const Ray& ray, const HitInfo& hit,
		const Scene& scene) const;
	Vector3 refractionColor(const Ray& ray, const HitInfo& hit,
		const Scene& scene) const;
	Vector3 highlightColor(const Ray& ray, const HitInfo& hit,
		const Scene& scene) const;
	float refractionIndex;
	float getRefractionIndex() { return refractionIndex; }
protected:
	Vector3 m_kd;
	Vector3 m_ka;
	Vector3 m_ks;
	Vector3 m_kr;
};

#endif 

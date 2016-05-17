#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"

class Material
{
public:
    Material();
    virtual ~Material();

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
	bool isDiffuse() const {
		if (diff.x >= 0.0) return true;
		if (diff.y >= 0.0) return true;
		if (diff.z >= 0.0) return true;
		return false;
	}
	bool isReflection() const {
		if (refl.x >= 0.0) return true;
		if (refl.y >= 0.0) return true;
		if (refl.z >= 0.0) return true;
		return false;
	}
	bool isRefraction() const {
		if (refr.x >= 0.0) return true;
		if (refr.y >= 0.0) return true;
		if (refr.z >= 0.0) return true;
		return false;
	}
	Vector3 getDiffuse() const {
		return diff;
	}
	Vector3 getReflection() const {
		return refl;
	}
	Vector3 getRefraction() const {
		return refr;
	}
	float getRefractionIndex() const {
		return refrI;
	}
protected:
	Vector3 diff = Vector3(.75f);
	Vector3 refl = Vector3(.25f);
	Vector3 refr = Vector3(.25f);
	float refrI = 1.0f;
};

#endif // CSE168_MATERIAL_H_INCLUDED

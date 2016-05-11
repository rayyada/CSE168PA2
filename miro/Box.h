#ifndef CSE168_BOX_H_INCLUDED
#define CSE168_BOX_H_INCLUDED

#include <vector>
#include "Object.h"

class Box : public Object
{
public:
	Box(Triangle * tri);
	Box(Vector3 boxmin, Vector3 boxmax);
	virtual ~Box();

    void setMaterial(const Material* m) {m_material = m;}

	virtual void renderGL();
    virtual void preCalc() {}


	virtual bool intersect(HitInfo& result, const Ray& ray,
		float tMin = 0.0f, float tMax = MIRO_TMAX);
	Vector3 getMin();
	Vector3 getMax();
	void setMin(Vector3 set);
	void setMax(Vector3 set);
	void setContainer(std::vector<Box*>* c);
	std::vector<Box*>* getContainer();
	char largestDimensionChar();
	float largestDimensionSize();
	Vector3 calcMidpoint();
	Vector3 getMidpoint();
	/*
	struct PointerCompare {
		bool operator()(const Box* l, const Box* r) {
			return *l < *r;
		}
	};

	bool operator<(const Box& node) const
	{
		return (this->_data.compare(node.getMidpoint()) == -1);
	}
	*/
protected:
	Vector3 min, max;
	std::vector<Box*>* container;
	Vector3 midpoint;
};

#endif // CSE168_OBJECT_H_INCLUDED

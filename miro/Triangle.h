#ifndef CSE168_TRIANGLE_H_INCLUDED
#define CSE168_TRIANGLE_H_INCLUDED

#include "Object.h"

/*
    The Triangle class stores a pointer to a mesh and an index into its
    triangle array. The mesh stores all data needed by this Triangle.
*/
class Triangle : public Object
{
public:
    Triangle(TriangleMesh * m = 0, unsigned int i = 0);
    virtual ~Triangle();

    void setIndex(unsigned int i) {m_index = i;}
    void setMesh(TriangleMesh* m) {m_mesh = m;}
	virtual void preCalc();
    virtual void renderGL();
    virtual bool intersect(HitInfo& result, const Ray& ray,
                           float tMin = 0.0f, float tMax = MIRO_TMAX);
	virtual void calcBBMinMax(Vector3& min, Vector3& max, int worldCheck);
	virtual Vector3 calcMidpoint();
	TriangleMesh* getMesh() { return m_mesh; }
	unsigned int getIndex() { return m_index; }
protected:
    TriangleMesh* m_mesh;
    unsigned int m_index;
	Vector3 midpoint;
};

#endif // CSE168_TRIANGLE_H_INCLUDED

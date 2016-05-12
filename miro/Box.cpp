#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"
#include "Lambert.h"
#include "Stone.h"
#include "Box.h"
#include <algorithm>

Box::Box(Triangle * tri, bool p)
{
	container = new std::vector<Box*>();

	// min = minium coordinate of the box
	// max = maxium coordinate of the box
	unsigned int index = tri->getIndex();
	TriangleMesh* tempMesh = tri->getMesh();
	TriangleMesh::TupleI3 ti3_v = tempMesh->vIndices()[index]; //get vertices
	const Vector3 & v0 = tempMesh->vertices()[ti3_v.x]; //vertex a of triangle
	const Vector3 & v1 = tempMesh->vertices()[ti3_v.y]; //vertex b of triangle
	const Vector3 & v2 = tempMesh->vertices()[ti3_v.z]; //vertex c of triangle

	Vector3 minTemp, maxTemp;
	minTemp.x = std::min(v0.x, v1.x);
	minTemp.x = std::min(minTemp.x, v2.x);
	min.x = std::min(min.x, minTemp.x);
	minTemp.y = std::min(v0.y, v1.y);
	minTemp.y = std::min(minTemp.y, v2.y);
	min.y = std::min(min.y, minTemp.y);
	minTemp.z = std::min(v0.z, v1.z);
	minTemp.z = std::min(minTemp.z, v2.z);
	min.z = std::min(min.z, minTemp.z);

	maxTemp.x = std::max(v0.x, v1.x);
	maxTemp.x = std::max(maxTemp.x, v2.x);
	max.x = std::max(max.x, maxTemp.x);
	maxTemp.y = std::max(v0.y, v1.y);
	maxTemp.y = std::max(maxTemp.y, v2.y);
	max.y = std::max(max.y, maxTemp.y);
	maxTemp.z = std::max(v0.z, v1.z);
	maxTemp.z = std::max(maxTemp.z, v2.z);
	max.z = std::max(max.z, maxTemp.z);
	calcMidpoint();
	primary = p;
}
Box::Box(Vector3 boxmin, Vector3 boxmax, bool p)
{
	container = new std::vector<Box*>();
	min = boxmin;
	max = boxmax;
	calcMidpoint();
	primary = p;
}
float Box::getSA()
{
	return (6 * pow((max.x - min.x), 2));
}
Box::~Box()
{

}

bool Box::primaryBox()
{
	return primary;
}
void Box::insertPrimary(Object* p)
{
	primaryOBJ = p;
}

Object* Box::getPrimary()
{
	return primaryOBJ;
}
void Box::setContainer(std::vector<Box*>* c)
{
	container = c;
}
std::vector<Box*>* Box::getContainer()
{
	return container;
}

Vector3 Box::getMin()
{
	return min;
}
Vector3 Box::getMax()
{
	return max;
}
void Box::setMin(Vector3 set)
{
	min = set;
}
void Box::setMax(Vector3 set)
{
	max = set;
}

void
Box::renderGL()
{

}
Vector3 Box::getMidpoint()
{
	return midpoint;
}
Vector3 Box::calcMidpoint()
{
	Vector3 add = max - min;
	midpoint = min + add;
	return midpoint;
}

char Box::largestDimensionChar()
{
	float xSize, ySize, zSize;
	float largestD;
	xSize = max.x - min.x;
	ySize = max.y - min.y;
	zSize = max.z - min.z;
	largestD = std::max(std::max(xSize, ySize), zSize);
	if (largestD == xSize)
	{
		return 'x';
	}
	if (largestD == ySize)
	{
		return 'y';
	}
	if (largestD == zSize)
	{
		return 'z';
	}
}

float Box::largestDimensionSize()
{
	float xSize, ySize, zSize;
	float largestD;
	xSize = max.x - min.x;
	ySize = max.y - min.y;
	zSize = max.z - min.z;
	largestD = std::max(std::max(xSize, ySize), zSize);

	return largestD;
}

bool
Box::intersect(HitInfo& result, const Ray& ray,
	float tMin, float tMax)
{
	// Since we use floats, we cover cases where inverse direction vectors are still valid e.g. 1.0/0.0 = infinity
	// box intersection calc with 1/r.dir already calculated to optimize calculation further
	float t1 = (min.x - ray.o.x)*ray.invd.x;
	float t2 = (max.x - ray.o.x)*ray.invd.x;

	// Initial calculation of xmin/xmax because we need some initial tmin/tmax for later
	float tmin = std::min(t1, t2);
	float tmax = std::max(t1, t2);

	for (int i = 1; i < 3; ++i) {
		t1 = (min[i] - ray.o[i])*ray.invd[i];
		t2 = (max[i] - ray.o[i])*ray.invd[i];

		// Do necessary swapping and getting the max value for tmin, min value for tmax
		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));
	}

	// Intersection is false when tmax < tmin or 0
	if (tmin < tMin || tmax > tMax)
	{
		return false;
	}
	return tmax > std::max(tmin, 0.0f);
}
#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"


Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{

}


Triangle::~Triangle()
{

}


void
Triangle::renderGL()
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}



bool
Triangle::intersect(HitInfo& result, const Ray& r,float tMin, float tMax)
{
	TriangleMesh::TupleI3 ti3_v = m_mesh->vIndices()[m_index]; //get vertices
	TriangleMesh::TupleI3 ti3_n = m_mesh->nIndices()[m_index]; //get normals
	const Vector3 & v0 = m_mesh->vertices()[ti3_v.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3_v.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3_v.z]; //vertex c of triangle
	const Vector3 & n0 = m_mesh->normals()[ti3_n.x]; //normal a of triangle
	const Vector3 & n1 = m_mesh->normals()[ti3_n.y]; //normal b of triangle
	const Vector3 & n2 = m_mesh->normals()[ti3_n.z]; //normal c of triangle

	Vector3 rayOrigin = r.o;
	Vector3 rayDir = r.d;
	//Solve for normal of the plane. In this case, the triangle
	Vector3 triangleNormal = (n0 + n1 + n2) / 3;
	triangleNormal.normalize();
	float D = dot(triangleNormal, v0);

	// Check if dot(n, d) == 0. If it's 0, the ray is parallel to the plane, and would give a divide by zero when caluclating t
	float parallelCheck = dot(triangleNormal, rayDir);
	if (parallelCheck == 0)
	{
		return false;
	}

	// P = o + tR
	// Ax + By + Cz + D = 0
	float t = -(dot(triangleNormal, rayOrigin) + D) / parallelCheck;
    
	// Calculate point where ray intersects plane. Note that this does not mean it intersects the triangle
	Vector3 intersection = rayOrigin + (t*rayDir);

	//Inside-outside test
	Vector3 edge0 = v1 - v0;
	Vector3 edge1 = v2 - v1;
	Vector3 edge2 = v0 - v2;
	Vector3 check0 = intersection - v0;
	Vector3 check1 = intersection - v1;
	Vector3 check2 = intersection - v2;
	if (dot(triangleNormal, cross(edge0, check0)) > 0 && dot(triangleNormal, cross(edge1, check1)) > 0 && dot(triangleNormal, cross(edge2, check2)) > 0)
	{
		return true;
	}
	return false;
}

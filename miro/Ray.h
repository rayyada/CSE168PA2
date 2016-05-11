#ifndef CSE168_RAY_H_INCLUDED
#define CSE168_RAY_H_INCLUDED

#include "Vector3.h"

class Ray
{
public:
	Vector3 o,      //!< Origin of ray
		d,      //!< Direction of rayz
		invd;
    Ray() : o(), d(Vector3(0.0f,0.0f,1.0f))
    {
		invd.x = 1.0f / d.x;
		invd.y = 1.0f / d.y;
		invd.z = 1.0f / d.z;
    }

    Ray(const Vector3& o, const Vector3& d) : o(o), d(d)
    {
		invd.x = 1.0f / d.x;
		invd.y = 1.0f / d.y;
		invd.z = 1.0f / d.z;
    }
};


//! Contains information about a ray hit with a surface.
/*!
    HitInfos are used by object intersection routines. They are useful in
    order to return more than just the hit distance.
*/
class HitInfo
{
public:
    float t;                            //!< The hit distance
    Vector3 P;                          //!< The hit point
    Vector3 N;                          //!< Shading normal vector
    const Material* material;           //!< Material of the intersected object

    //! Default constructor.
    explicit HitInfo(float t = 0.0f,
                     const Vector3& P = Vector3(),
                     const Vector3& N = Vector3(0.0f, 1.0f, 0.0f)) :
        t(t), P(P), N(N), material (0)
    {
        // empty
    }
};

#endif // CSE168_RAY_H_INCLUDED

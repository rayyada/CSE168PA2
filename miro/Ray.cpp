#include "Ray.h"
#include "Miro.h"

Ray Ray::diffuse(const HitInfo& hit) const {
	float theta = asin(sqrt((float)rand() / (float)RAND_MAX));
	float phi = 2.0f * PI * ((float)rand() / (float)RAND_MAX);

	Vector3 t_1 = cross(Vector3(0, 0, 1), hit.N);
	if (t_1.length() < epsilon) {
		// if cross product equals 0, hit.N is parallel to the z-axis --> use y-axis instead
		t_1 = cross(Vector3(0, 1, 0), hit.N);
	}
	Vector3 t_2 = cross(t_1, hit.N);

	// convert spherical coordinates to vector
	Vector3 dir = cos(phi) * sin(theta) * t_1 +
		sin(phi) * sin(theta) * t_2 +
		cos(theta) * hit.N;

	dir.normalize();

	Vector3 origin = hit.P + (dir * epsilon);
	return Ray(origin, dir);
}

Ray Ray::reflect(const HitInfo& hit) const {
	// mirror reflection ray
	Vector3 refl = (this->d - 2 * dot(d, hit.N) * hit.N).normalized();
	// calculates hitpoint -> origin
	Vector3 orig = hit.P + (refl * epsilon);
	return Ray(orig, refl);
}

Ray Ray::refract(const HitInfo& hit) const {

	Vector3 n;

	// indices of refraction
	float n1 = 1.0f;
	float n2 = 1.0f;

	if (dot(hit.N, this->d) < 0) { // entering object
		n2 = hit.material->getRefractionIndex();
		n = hit.N;
	}
	else { // leaving object
		n1 = hit.material->getRefractionIndex();
		n = -hit.N;
	}

	// compute energy of refracted ray ( cos^2 (theta2) )
	float cosTheta1 = dot(this->d, n); // NOTE: should this be n or hit.N?
	float e = 1 - ((n1*n1) / (n2*n2)) * (1 - cosTheta1*cosTheta1);

	// total internal reflection
	if (e < 0.0f) return reflect(hit);

	// create refraction ray
	Vector3 dir = (n1 / n2) * (this->d - n * cosTheta1) - n * (sqrt(e));
	Vector3 origin = hit.P + (dir * epsilon);
	return Ray(origin, dir);
}

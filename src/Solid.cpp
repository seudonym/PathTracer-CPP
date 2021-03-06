#include "Solid.h"

#define EPSILON 0.0001

// Materials:
//=============================================================================================
Lambertian::Lambertian(const vec3& _albedo) : albedo(_albedo) {}
bool Lambertian::scatter(const Ray& ray, const Intersection& intersection, vec3& attenuation, Ray& scattered) {
	vec3 target = intersection.point + intersection.normal + rndDirection();
	scattered = Ray(intersection.point + intersection.normal * EPSILON, target - intersection.point);
	attenuation = albedo;
	return true;
}

Reflective::Reflective(const vec3& _albedo, float _fuzziness) : albedo(_albedo), fuzziness(_fuzziness) {}
bool Reflective::scatter(const Ray& ray, const Intersection& intersection, vec3& attenuation, Ray& scattered) {
	vec3 target = reflect(normalize(ray.direction), intersection.normal);
	scattered = Ray(intersection.point + intersection.normal * EPSILON, target + rndDirection() * fuzziness);
	attenuation = albedo;
	return dot(scattered.direction, intersection.normal);
}

Transparent::Transparent(float _ior) : ior(_ior) {}
bool Transparent::scatter(const Ray& ray, const Intersection& intersection, vec3& attenuation, Ray& scattered) {
	vec3 normal;
	vec3 reflected = reflect(ray.direction, intersection.normal);
	float eta;		// eta = eta1 / eta2	1 is the incident medium, 2 is the refracted medium
	attenuation = vec3(1.0);

	if (dot(ray.direction, intersection.normal) > 0) {
		// If ray inside, flip normal. eta1 / eta2 = ior
		normal = intersection.normal * -1;
		eta = ior;
	}
	else {
		// If outside, dont change normal, eta1/eta2 = 1/ior
		normal = intersection.normal;
		eta = 1.0 / ior;
	}

	vec3 refracted;
	if (refract(ray.direction, normal, eta, refracted)) scattered = Ray(intersection.point - normal * EPSILON, refracted);
	else {
		scattered = Ray(intersection.point, reflected);
		return false;
	}
	return true;
}


// Solids:
//=============================================================================================
Sphere::Sphere() : center(vec3(0.0)), radius(1.0f) {}
Sphere::Sphere(const vec3& _center, float _radius, Material* _material) : center(_center), radius(_radius), material(_material) {}

bool Sphere::intersect(const Ray& ray, float tMin, float tMax, Intersection& intersection) {
	vec3 OC = ray.origin - center;
	float a = dot(ray.direction, ray.direction);
	float b = 2.0 * dot(ray.direction, OC);
	float c = dot(OC, OC) - radius * radius;
	float discriminant = b * b - 4.0 * a * c;

	if (discriminant > 0) {
		float sqrtDiscriminant = sqrt(discriminant);
		float temp = (-b - sqrtDiscriminant) / (2.0 * a);

		intersection.material = material;

		if (temp < tMax && temp > tMin) {
			intersection.t = temp;
			intersection.point = pointOnRay(ray, temp);
			intersection.normal = (intersection.point - center) / radius;
			return true;
		}

		temp = (-b + sqrtDiscriminant) / (2.0 * a);
		if (temp < tMax && temp > tMin) {
			intersection.t = temp;
			intersection.point = pointOnRay(ray, temp);
			intersection.normal = (intersection.point - center) / radius;
			return true;
		}
	}

	return false;
}


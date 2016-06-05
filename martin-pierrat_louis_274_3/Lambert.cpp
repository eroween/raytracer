#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include "Camera.h"
#include "Random.hpp"
#include <algorithm>


Lambert::Lambert(const Vector3 & kd, const Vector3 & ka) :
    m_kd(kd), m_ka(ka)
{

}

Lambert::~Lambert()
{
}

Vector3
Lambert::path_shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    float russian_roulette = 1.0f;
    float emittance = 0.0f;
    float reflectance = 0.8f;

    if (ray.bounce > 10) {
        if (Random::linear_bounded(0.0f, 1.0f) < 0.9f)
            return Vector3(0.0f);
        russian_roulette = 0.9f;
    }

    HitInfo h;
    Ray r;
    r.o = hit.P;
    r.d = Random::hemispherical(hit.N);
    r.bounce = ray.bounce + 1;

    if (scene.trace(h, r, 0.0001f) == false) {
        return 0.0f;
    }

    Vector3 Le = emittance;
    Vector3 Li = h.material->path_shade(r, h, scene);


    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

    const Vector3 viewDir = -ray.d; // d is a unit vector

    const Lights *lightlist = scene.lights();

    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;
        Vector3 l = pLight->position() - hit.P;

/*        Ray shadow_ray;*/
        //HitInfo shadow_hit;
        //shadow_ray.o = hit.P;
        //shadow_ray.d = l;
        //shadow_ray.d.normalize();
        //if (scene.trace(shadow_hit, shadow_ray, 0.0001f, l.length()) == true) {
           //continue;
        //}


        // the inverse-squared falloff
        float falloff = l.length2();

        // normalize the light direction
        l /= sqrt(falloff);

        Vector3 result = pLight->color();
        result *= m_kd;
        L += std::max(0.0f, (pLight->wattage() * (dot(hit.N, l))) / (4.0f * PI * falloff)) * result;
    }

    Li += L;

    // Lambertian reflectance.
    float p = 1.0f;
    float BRDF = dot(-r.d, h.N);
    if (std::isnan(BRDF)) return 0.0f;
    return (Le * emittance + (p * BRDF * Li) * reflectance) / russian_roulette;
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

    const Vector3 viewDir = -ray.d; // d is a unit vector

    const Lights *lightlist = scene.lights();

    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;
        Vector3 l = pLight->position() - hit.P;

/*        Ray shadow_ray;*/
        //HitInfo shadow_hit;
        //shadow_ray.o = hit.P;
        //shadow_ray.d = l;
        //shadow_ray.d.normalize();
        //if (scene.trace(shadow_hit, shadow_ray, 0.0001f, l.length()) == true) {
           //continue;
        //}


        // the inverse-squared falloff
        float falloff = l.length2();
        // normalize the light direction
        l /= sqrt(falloff);

        Vector3 result = pLight->color();
        result *= m_kd;
        L += std::max(0.0f, (pLight->wattage() * (dot(hit.N, l))) / (4.0f * PI * falloff)) * result;
    }
    L += m_ka;
    return L;
}

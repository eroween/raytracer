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

    virtual Vector3 path_shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const;
    virtual Vector3 emittance_per_point(void) const
    {
        return 0.0f;
    }

    virtual float reflectance(const Vector3 &I = 0.0f, const Vector3 &N = 0.0f, float n1 = 0.0f, float n2 = 0.0f) const {
       return 1.0f;
    }

    virtual float       pdf(const Vector3 &N, const Vector3 &dir) const {
        return std::max(0.0f, dot(dir, N)); // lambertian
    }


    virtual float   transmittance(const Vector3 &I, const Vector3 &N, float n1, float n2) const {
        return 0.0f;
    }

    virtual float   refractive_index(void) const {
        return 1.0f;
    }

    virtual Vector3 diffuse(void) const {
         return 1.0f;
    }

};

#endif // CSE168_MATERIAL_H_INCLUDED

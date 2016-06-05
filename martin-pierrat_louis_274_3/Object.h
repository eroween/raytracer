#ifndef CSE168_OBJECT_H_INCLUDED
#define CSE168_OBJECT_H_INCLUDED

#include <vector>
#include "Miro.h"
#include "Material.h"
#include "Photon.hpp"
#include "AABB.hpp"

class Object
{
public:
    Object() : m_material(nullptr), m_aabb(nullptr) {}
    virtual ~Object() {}

    void setMaterial(const Material* m) {m_material = m;}

    virtual void renderGL() {}
    virtual void preCalc() {}

    virtual bool intersect(HitInfo& result, const Ray& ray,
                           float tMin = 0.0f, float tMax = MIRO_TMAX) = 0;

    virtual Photon  random_photon(void) const
    {
        throw std::logic_error("Not Implemented.");
    };

    AABB    *aabb(void) const { return this->m_aabb; }
    void    aabb(AABB *aabb) { this->m_aabb = aabb; }

protected:
    const Material* m_material;

    AABB    *m_aabb;

};

typedef std::vector<Object*> Objects;

#endif // CSE168_OBJECT_H_INCLUDED

#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

Vector3
Material::shade(const Ray&, const HitInfo&, const Scene&) const
{
    return Vector3(1.0f, 1.0f, 1.0f);
}

Vector3
Material::path_shade(const Ray &, const HitInfo &, const Scene &) const
{
    return Vector3(1.0f);
}

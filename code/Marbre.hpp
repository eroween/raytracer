#pragma once

#include    "Material.h"

class   Marbre : public Material
{
    public:
        Marbre(void);
        virtual ~Marbre(void);

    public:
        virtual void    preCalc(void);
        virtual Vector3     shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const;

    private:

        virtual Vector3     shade_specular(const Ray &ray, const HitInfo &hit, const Scene &scene) const;

    private:
        ///
        /// \brief Reflected light value.
        ///
        Vector3 m_kd;
        Vector3 m_ks;
        Vector3 m_kt;
        Vector3 m_ka;
};

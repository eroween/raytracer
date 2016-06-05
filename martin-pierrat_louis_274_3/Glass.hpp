//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2016 Martin-Pierrat Louis (louismartinpierrat@gmail.com)
//

#pragma     once

#include "Material.h"

class   Glass : public Material
{
    public:
        Glass(const Vector3 &color);
        virtual ~Glass(void);

    public:
        virtual void    preCalc(void);
        virtual Vector3     shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const;

        virtual float       pdf(const Vector3 &N, const Vector3 &dir) const {
            return 1.0f; // specular
        }


    virtual float reflectance(const Vector3 &I = 0.0f, const Vector3 &N = 0.0f, float n1 = 0.0f, float n2 = 0.0f) const {
        const float n = n1 / n2;
        const float cosI = -dot(N, I);
        const float sinT2 = n * n * (1.0 - cosI * cosI);
        if (sinT2 > 1.0f) return 1.0f;
        const float cosT = sqrt(1.0f - sinT2);
        const float rOrth = (n1 * cosI - n2 * cosT) / (n1 * cosI +  n2 * cosT);
        const float rPar = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
        return (rOrth * rOrth + rPar * rPar) / 2.0f;
    }

    virtual float   transmittance(const Vector3 &I, const Vector3 &N, float n1, float n2) const {
        return 1.0f - reflectance(I, N, n1, n2);
    }

    virtual float   refractive_index(void) const {
        return m_n;
    }




    private:
        virtual Vector3     shade_reflection(const Ray &ray, const HitInfo &hit, const Scene &scene) const;
        virtual Vector3     shade_refraction(const Ray &ray, const HitInfo &hit, const Scene &scene) const;
        virtual Vector3     shade_reflectance(const Ray &ray, const HitInfo &hit, const Scene &scene) const;

    private:
        Vector3 m_kd;
        Vector3 m_ka;

        // indice.
        float   m_n;
};

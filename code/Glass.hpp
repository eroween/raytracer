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

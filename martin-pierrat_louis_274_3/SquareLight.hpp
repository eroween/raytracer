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

#pragma once

#include "Material.h"

class   SquareLight : public Material
{
    public:
        SquareLight(void);
        virtual ~SquareLight(void);

        virtual void    preCalc(void) {}

        virtual Vector3     shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const {}
        virtual Vector3     path_shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const;

    public:

        float   wattage(void) const {return this->m_wattage;}
        void    wattage(float wattage) {this->m_wattage = wattage;}

        Vector3 color(void) const {return this->m_color;}
        void    color(const Vector3 &color) {this->m_color = color;}

        float   area(void) const {return this->m_area;}
        void    area(float area) {this->m_area = area;}

        virtual Vector3 emittance_per_point(void) const
        {
            return m_wattage / m_area * m_color;
        }

    private:
        float   m_wattage;
        Vector3 m_color;
        float   m_area;
};

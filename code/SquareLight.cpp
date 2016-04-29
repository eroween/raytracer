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

#include "SquareLight.hpp"
#include "Ray.h"

SquareLight::SquareLight(void) :
    m_wattage(100),
    m_color(1.0f),
    m_area(0.2f * 0.2f)
{

}

SquareLight::~SquareLight(void)
{

}

Vector3
SquareLight::path_shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const
{
    return ((this->wattage() * std::max(0.0f, dot(-ray.d, hit.N))) / this->area()) * this->color();
}

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

#include    "AABB.hpp"
#include    "Statistic.hpp"

#include <limits>

AABB::AABB(void) :
    m_box_min(std::numeric_limits<float>::infinity()),
    m_box_max(-std::numeric_limits<float>::infinity())
{

}

AABB::AABB(const Vector3 &box_min, const Vector3 &box_max) :
    m_box_min(box_min),
    m_box_max(box_max)
{

}

bool
AABB::intersect(const Ray &r, float tmin, float tmax)
{
    Statistic::number_ray_box_intersection += 1;
    for (int dim = 0 ; dim < 3 ; ++dim)
    {
        float inv_dir = 1.0f / r.d[dim];
        float t0 = (m_box_min[dim] - r.o[dim]) * inv_dir;
        float t1 = (m_box_max[dim] - r.o[dim]) * inv_dir;
        if (inv_dir < 0.0f) std::swap(t0, t1);
        tmin = std::max(t0, tmin);
        tmax = std::min(t1, tmax);
        if (tmax < tmin) return false;
    }
    //*out_tmin = tmin;
    return true;
}

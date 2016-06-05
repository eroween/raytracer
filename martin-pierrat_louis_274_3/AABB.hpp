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

#include "Miro.h"
#include "Vector3.h"
#include "Ray.h"

class   AABB
{
    public:
        AABB(void);
        AABB(const Vector3 &box_min, const Vector3 &box_max);
        ~AABB(void) {}

    public:

        virtual bool    intersect(const Ray &r, float tmin = 0.0f, float tmax = MIRO_TMAX);

        const Vector3   &box_min(void) const { return this->m_box_min; };
        const Vector3   &box_max(void) const { return this->m_box_max; };
        const Vector3   centroid(void) const {
/*            std::cout << m_box_min.x << ' ' << m_box_min.y << ' ' << m_box_min.z << std::endl;*/
            //std::cout << m_box_max.x << ' ' << m_box_max.y << ' ' << m_box_max.z << std::endl;

            return m_box_min * 0.5f + m_box_max * 0.5f; }

    public:

        void    merge(AABB *other)
        {
            m_box_min.x = std::min(other->m_box_min.x, m_box_min.x);
            m_box_min.y = std::min(other->m_box_min.y, m_box_min.y);
            m_box_min.z = std::min(other->m_box_min.z, m_box_min.z);

            m_box_max.x = std::max(other->m_box_max.x, m_box_max.x);
            m_box_max.y = std::max(other->m_box_max.y, m_box_max.y);
            m_box_max.z = std::max(other->m_box_max.z, m_box_max.z);
        }

       void    merge(const Vector3 &point)
        {
            m_box_min.x = std::min(m_box_min.x, point.x);
            m_box_min.y = std::min(m_box_min.y, point.y);
            m_box_min.z = std::min(m_box_min.z, point.z);

            m_box_max.x = std::max(m_box_max.x, point.x);
            m_box_max.y = std::max(m_box_max.y, point.y);
            m_box_max.z = std::max(m_box_max.z, point.z);
        }

       int  maximum_extent(void)
       {
            Vector3     d = m_box_max - m_box_min;
            if (d.x > d.y && d.x > d.z) return 0;
            else if (d.y > d.z) return 1;
            return 2;
       }

       float    surface_area(void)
       {
            float a = m_box_max.x - m_box_min.x;
            float b = m_box_max.y - m_box_min.y;
            float c = m_box_max.z - m_box_min.z;
            return 2.0f * a * b + 2.0f * b * c + 2.0f * a * c; // 2ab + 2bc + 2ac
       }

    private:

        Vector3     m_box_min;
        Vector3     m_box_max;

};

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

#include "Ray.h"

class Photon : public Ray
{
    public:
        Photon(void) : Ray(), m_spectrum(0.0f)
        {

        }

        Photon(const Vector3 &o, const Vector3 &d, const Vector3 &spectrum) : Ray(o, d), m_spectrum(spectrum)
        {

        }

        float   refractive_index(void) const {
             return n;
        }

    public:
        Vector3     m_spectrum;
};

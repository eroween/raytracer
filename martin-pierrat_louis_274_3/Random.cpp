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

#include "Random.hpp"

#include <random>
#include <cmath>
#include <chrono>
#include <cstdlib>

/*inline Vector3*/
//Vector3::hemispherical_random(const Vector3 &normal)
//{
    //const float phi = random(0.0f, 6.283185307179586476925286766559f);
    //const float rho = acos(sqrt(random(0.0f, 1.0f)));
    //const float theta = rho - 3.1415926535897f / 2.0f
    //const float x = r * cos(a);
    //const float y = r * sin(a);
    //const Vector3 base_v = Vector3(normal.y, -normal.x, 0.0f);
    //return Vector3(x, y, z).rotated(a, cross(normal, base_v));
//}

float
Random::linear_bounded(float min, float max)
{
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 generator (seed);
    std::uniform_real_distribution<double> uniform01(min, max);
    return uniform01(generator);;
}

Vector3
Random::hemispherical(const Vector3 &n)
{
/*    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();*/
    //static std::mt19937 generator (seed);
    //static std::uniform_real_distribution<double> uniform01(0.0, 1.0);

    //double theta = 2 * M_PI * uniform01(generator);
    //double phi = acos(1 - 2 * uniform01(generator));
    //double x = sin(phi) * cos(theta);
    //double y = sin(phi) * sin(theta);
    //double z = abs(cos(phi));

    float Xi1 = (float)rand()/(float)RAND_MAX;
    float Xi2 = (float)rand()/(float)RAND_MAX;

    //float Xi1 = Random::linear_bounded(0.0f, 1.0f);
    //float Xi2 = Random::linear_bounded(0.0f, 1.0f);
    //float Xi2 = (float)rand()/(float)RAND_MAX;



    float  theta = acos(sqrt(1.0-Xi1));
    float  phi = 2.0 * 3.1415926535897932384626433832795 * Xi2;

    float xs = sinf(theta) * cosf(phi);
    float ys = cosf(theta);
    float zs = sinf(theta) * sinf(phi);

    Vector3 y(n.x, n.y, n.z);
    Vector3 h = y;
    if (fabs(h.x)<=fabs(h.y) && fabs(h.x)<=fabs(h.z))
        h.x= 1.0;
    else if (fabs(h.y)<=fabs(h.x) && fabs(h.y)<=fabs(h.z))
        h.y= 1.0;
    else
        h.z= 1.0;


    Vector3 x = cross(h, y).normalize();
    Vector3 z = cross(x, y).normalize();

    Vector3 direction = xs * x + ys * y + zs * z;

    if (std::isnan(direction.x) || std::isnan(direction.y) || std::isnan(direction.z)) {
        return Vector3(-0.0737933, 0.997213, -0.0109931);
    }
    return direction.normalize();
}

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

#include "Stone.hpp"
#include "Ray.h"
#include "Scene.h"
#include "Perlin.hpp"
#include "Worley.h"

Stone::Stone(int v) :
    m_ka(0.08, 0.08, 0.08),
    m_v(v)
{

}

Stone::~Stone(void)
{

}


void
Stone::preCalc(void)
{

}

float cellular_noise(const Vector3 &pos)
{
    float at[3] = {pos.x, pos.y, pos.z};
    float F[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    unsigned long ID[4] = {0, 0, 0, 0};
    float delta[][3] = {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
    WorleyNoise::noise3D(at, 4, F, delta, ID);
    return 1.0f + F[0] - F[3];
}

float fractal_cellular_noise(const Vector3 &pos, int size)
{
    float persistency = 0.5f; // between 0.0f && 1.0f
    float amplitude = pow(2.0f, size - 1); // octave
    float frequencies = 1.0f;
    float noise = 0.0f;

    for (unsigned int i = 0 ; i < size ; ++i)
    {
        noise += amplitude * cellular_noise(pos * frequencies);
        amplitude = persistency * amplitude;
        frequencies = 2.0f * frequencies;
    }
    noise /= pow(2.0f, size) - 1;
    return std::min(1.0f, std::max(0.0f, noise));
}

Vector3
Stone::shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const
{

    const auto &lights = *scene.lights();
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

    for (const auto &light : lights)
    {
        Vector3     l = light->position() - hit.P;

        // Shadow check
        Ray shadow_ray;
        HitInfo shadow_hit;
        shadow_ray.o = hit.P;
        shadow_ray.d = l;
        shadow_ray.d.normalize();
        if (scene.trace(shadow_hit, shadow_ray, 0.0001f, l.length()) == true) {
           continue;
        }
        // !Shadow check

        float falloff = l.length2();
        l /= sqrt(falloff);
        Vector3 ws = Vector3::reflect(ray.d, hit.N);

        // Texturing

        // !Texturing
        //

        float s = 7.0f;
        Vector3 pos = hit.P * s;
        Vector3 c1;
        Vector3 c2;
        float fnoise;
        if (this->m_v == 0) {
            fnoise = fractal_cellular_noise(pos, 4);
            c1 = Vector3(90.0f / 255.0f, 60.0f / 255.0f, 10.0f / 255.0f);
            c2 = Vector3(255.0f / 255.0f, 160.0f / 255.0f, 30.0f / 255.0f);
            c1 *= Vector3(0.2);
            c2 *= Vector3(0.6);
        } else {
            fnoise = fractal_cellular_noise(pos, 4);
            c1 = Vector3(0.0f);
            c2 = Vector3(0.4f);
        }
        Vector3 worley = fnoise * c1 + (1.0f - fnoise) * c2;
        L += worley * light->color() * ((light->wattage() * dot(hit.N, l)) / (4.0f * PI * falloff));
    }
        float s = 7.0f;
        Vector3 pos = hit.P * s;
        Vector3 c1;
        Vector3 c2;
        float fnoise;
        if (this->m_v == 0) {
            fnoise = fractal_cellular_noise(pos, 4);
            c1 = Vector3(90.0f / 255.0f, 60.0f / 255.0f, 10.0f / 255.0f);
            c2 = Vector3(255.0f / 255.0f, 160.0f / 255.0f, 30.0f / 255.0f);
            c1 *= Vector3(0.2);
            c2 *= Vector3(0.6);
        } else {
            fnoise = fractal_cellular_noise(pos, 4);
            c1 = Vector3(0.0f);
            c2 = Vector3(0.4f);
        }
        Vector3 worley = fnoise * c1 + (1.0f - fnoise) * c2;
    L += this->m_ka * worley;
    return L;
}

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

#include    <cmath>
#include    "Marbre.hpp"
#include    "Ray.h"
#include    "Scene.h"
#include    "Camera.h"
#include    "Perlin.hpp"

Marbre::Marbre(void) :
    Material(),
    m_kd(0.9f, 0.9f, 0.9f),
    m_ka(0.1f, 0.1f, 0.1f)
{

}

Marbre::~Marbre(void)
{

}

void
Marbre::preCalc(void)
{

}

Vector3
Marbre::shade_specular(const Ray &ray, const HitInfo &hit, const Scene &scene) const
{
    if (ray.bounce > 10) return 0.0f;
    Ray reflect_ray;
    HitInfo reflect_hit;
    reflect_ray.o = hit.P;
    reflect_ray.d = -2 * (dot(ray.d, hit.N) * hit.N) + ray.d;
    reflect_ray.d.normalize();
    reflect_ray.bounce = ray.bounce + 1;
    if (scene.trace(reflect_hit, reflect_ray, 0.01f) == false) {
        return g_camera->bgColor();
    }
    if (reflect_hit.material == 0) return 0.0f;
    return reflect_hit.material->shade(reflect_ray, reflect_hit, scene);
}

float turbulence(float x, float y, float z, float size)
{
    float value = 0.0, initialSize = size;

    while (size >= 1)
    {
        value += PerlinNoise::noise(x / size, y / size, z / size) * size;
        size /= 2;
    }
    return value / size;
}

Vector3
Marbre::shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const
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

        const float s = 30.0f;
        const float f = 0.01f; // pattern_frequency
        const float a = 100.0f; // noise amplitude
        Vector3 pos = hit.P * s;
        float noise = turbulence(pos.x, pos.y, pos.z, 4) + 0.5f;
        float kd = (1.0f + sin(f * (pos.x + a * noise))) / 2.0f;
        kd = std::max(0.0f, std::min(1.0f, kd));

        Vector3     color1(1.0f, 1.0, 1.0f);
        Vector3     color2(0.1f, 0.1f, 0.5f);

        kd = kd * kd * kd * kd;
        Vector3 color = color2 * kd + color1 * (1.0f - kd);

        // !Texturing

        L += color *light->color() * ((light->wattage() * dot(hit.N, l)) / (4.0f * PI * falloff));
    }

    const float s = 30.0f;
    const float f = 0.01f; // pattern_frequency
    const float a = 100.0f; // noise amplitude
    Vector3 pos = hit.P * s;
    float noise = turbulence(pos.x, pos.y, pos.z, 4) + 0.5f;
    float kd = (1.0f + sin(f * (pos.x + a * noise))) / 2.0f;
    kd = std::max(0.0f, std::min(1.0f, kd));

    Vector3     color1(1.0f, 1.0, 1.0f);
    Vector3     color2(0.1f, 0.1f, 0.5f);

    kd = kd * kd * kd * kd;
    Vector3 color = color2 * kd + color1 * (1.0f - kd);

    L += this->m_ka * color;
    return L;
}

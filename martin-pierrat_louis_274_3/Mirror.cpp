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

#include "Mirror.hpp"
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

Mirror::Mirror(void)
{

}

Mirror::~Mirror(void)
{

}


void
Mirror::preCalc(void)
{

}

Vector3
Mirror::shade_reflection(const Ray &ray, const HitInfo &hit, const Scene &scene) const
{
    if (ray.bounce > 10) return 0.0f;
    HitInfo reflect_hit;
    Ray reflect_ray;

    reflect_ray.o = hit.P;
    reflect_ray.d = Vector3::reflect(ray.d, hit.N);
    reflect_ray.bounce = ray.bounce + 1;
    if (scene.trace(reflect_hit, reflect_ray, 0.0001f) == false) {
        // we don't hit anything, so we need to use the background color of the camera.
        return g_camera->bgColor(reflect_ray.d);
    }
    return reflect_hit.material->shade(reflect_ray, reflect_hit, scene);
}

Vector3
Mirror::shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const
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

        float rs = 1.0f;
        float s = rs * (pow(dot(ws, l), 50) / dot(hit.N, l));
        L += s * ((light->wattage() * dot(hit.N, l)) / (4.0f * PI * falloff));

    }
    L += this->shade_reflection(ray, hit, scene);
    return L;
}

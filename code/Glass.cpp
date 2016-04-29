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

#include    "Glass.hpp"
#include    "Ray.h"
#include    "Scene.h"
#include "Camera.h"

float   reflectance(const Vector3 &I, const Vector3 &N, float n1, float n2)
{
    const float n = n1 / n2;
    const float cosI = -dot(N, I);
    const float sinT2 = n * n * (1.0 - cosI * cosI);
    if (sinT2 > 1.0f) return 1.0f;
    const float cosT = sqrt(1.0f - sinT2);
    const float rOrth = (n1 * cosI - n2 * cosT) / (n1 * cosI +  n2 * cosT);
    const float rPar = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
    return (rOrth * rOrth + rPar * rPar) / 2.0f;
}

Glass::Glass(const Vector3 &color) :
    m_kd(0.0f, 0.0f, 0.0f),
    m_ka(0.0f, 0.0f, 0.0f),
    //m_n(2.490f)
    m_n(1.02f)
{

}

Glass::~Glass(void)
{

}

void
Glass::preCalc(void)
{

}

Vector3
Glass::shade_reflection(const Ray &ray, const HitInfo &hit, const Scene &scene) const
{
    HitInfo reflect_hit;
    Ray reflect_ray;

    reflect_ray.o = hit.P;
    reflect_ray.d = Vector3::reflect(ray.d, hit.N);
    reflect_ray.bounce = ray.bounce + 1;
    if (scene.trace(reflect_hit, reflect_ray, 0.001f) == false) {
        // we don't hit anything, so we need to use the background color of the camera.
        return g_camera->bgColor();
    }
    return reflect_hit.material->shade(reflect_ray, reflect_hit, scene);
}

Vector3
Glass::shade_refraction(const Ray &ray, const HitInfo &hit, const Scene &scene) const
{
    HitInfo refract_hit;
    Ray refract_ray;
    float n1 = ray.n;
    float n2 = this->m_n; // can fail.

    refract_ray.o = hit.P;
    //refract_ray.n = n2;
    refract_ray.d = Vector3::refract(hit.N, ray.d, n1 / n2);
    if (refract_ray.d == Vector3(0.0f)) {
        return Vector3(0.0f);
    }
    if (scene.trace(refract_hit, refract_ray, 0.0001f) == false) {
        return g_camera->bgColor();
    }


    refract_ray.o = refract_hit.P;
    refract_ray.d = Vector3::refract(-refract_hit.N, refract_ray.d, n2 / n1);
    refract_ray.bounce = ray.bounce + 1;
    HitInfo h2;
    if (refract_ray.d == Vector3(0.0f)) {
        return Vector3(0.0f);
    }
    if (scene.trace(h2, refract_ray, 0.0001f) == false) {
        return g_camera->bgColor();
    }
    return h2.material->shade(refract_ray, h2, scene);
}

Vector3
Glass::shade_reflectance(const Ray &ray, const HitInfo &hit, const Scene &scene) const
{
    float   r = 1.0f;
    Vector3 reflect_color;
    Vector3 refract_color;

    if (ray.bounce > 10) {
        return 0.0f;
    }
    r = reflectance(ray.d, hit.N, ray.n, this->m_n);
    reflect_color = this->shade_reflection(ray, hit, scene);
    if (r < 1.0f) {
        refract_color = this->shade_refraction(ray, hit, scene);
    } else {
        refract_color = 0.0f;
    }
    return r * reflect_color + (1.0f - r) * refract_color;
}

Vector3
Glass::shade(const Ray &ray, const HitInfo &hit, const Scene &scene) const
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

        Vector3 kd = this->m_kd * light->color();
        float rs = reflectance(l, hit.N, ray.n, this->m_n);
        float s = rs * (pow(dot(ws, l), 1000) / dot(hit.N, l));
        L += (kd + s) * ((light->wattage() * dot(hit.N, l)) / (4.0f * PI * falloff));
    }
    L += this->shade_reflectance(ray, hit, scene);
    L += this->m_ka;
    return L;
}

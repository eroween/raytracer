#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include "Path.hpp"

#include <atomic>
#include <thread>
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <limits>

Scene * g_scene = 0;

inline float   reflectance(const Vector3 &I, const Vector3 &N, float n1, float n2)
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

void
Scene::openGL(Camera *cam)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam->drawGL();

    // draw objects
    for (size_t i = 0; i < m_objects.size(); ++i)
        m_objects[i]->renderGL();

    glutSwapBuffers();
}

void
Scene::preCalc()
{
    Objects::iterator it;
    for (it = m_objects.begin(); it != m_objects.end(); it++)
    {
        Object* pObject = *it;
        pObject->preCalc();
    }
    Lights::iterator lit;
    for (lit = m_lights.begin(); lit != m_lights.end(); lit++)
    {
        PointLight* pLight = *lit;
        pLight->preCalc();
    }

    m_bvh.build(&m_objects);
}

void
Scene::pathtraceImage(Camera *cam, Image *img)
{
    int MAX_SAMPLE = 100;
    Ray ray;

    std::vector<std::pair<int, int>> tasks;
    tasks.reserve(img->height() * img->width());
    for (int j = 0; j < img->height(); ++j)
    {
        for (int i = 0 ; i < img->width() ; ++i)
        {
            tasks.push_back(std::make_pair(i, j));
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tasks.begin(), tasks.end(), g);

    std::atomic<bool>  f1;
    std::atomic<bool>  f2;
    std::atomic<bool>  f3;
    std::atomic<bool>  f4;

    f1 = false;
    f2 = false;
    f3 = false;
    f4 = false;

    std::atomic<int>    n;
    n = 0;

    const auto &worker_routine = [=, &n](int id, const std::vector<std::pair<int, int>> &tasks, int start, int end, std::atomic<bool> &f) {
        int max_sample = MAX_SAMPLE;
        for (int t = start ; t < end ; ++t) {
            int i = tasks[t].first;
            int j = tasks[t].second;

            if (i == 256 && j == 256) {
                max_sample = std::max(10000, MAX_SAMPLE);
            }
            Vector3 shadeResult(0.0f);
            for (int s = 0 ; s < max_sample ; ++s)
            {
                Ray ray = cam->eyeRay(i, j, img->width(), img->height());
                HitInfo hitInfo;
                if (trace(hitInfo, ray)) {
                    shadeResult += hitInfo.material->path_shade(ray, hitInfo, *this) / (float)(max_sample);
                } else {
                    shadeResult += g_camera->bgColor(ray.d) / (float)(max_sample);
                }
            }
            if (i == 256 && j == 256) {
                max_sample = MAX_SAMPLE;
                std::cout << "Point(256, 256) => (" << shadeResult.x << "," << shadeResult.y << "," << shadeResult.z << ")" << std::endl;
            }
            img->setPixel(i, j, shadeResult);
            ++n;
        }
        f = true;
    };

    std::thread t1(worker_routine, 0, std::ref(tasks), 0.0f / 4.0f * tasks.size(), 1.0f / 4.0f * tasks.size(), std::ref(f1));
    std::thread t2(worker_routine, 1, std::ref(tasks), 1.0f / 4.0f * tasks.size(), 2.0f / 4.0f * tasks.size(), std::ref(f2));
    std::thread t3(worker_routine, 2, std::ref(tasks), 2.0f / 4.0f * tasks.size(), 3.0f / 4.0f * tasks.size(), std::ref(f3));
    std::thread t4(worker_routine, 3, std::ref(tasks), 3.0f / 4.0f * tasks.size(), 4.0f / 4.0f * tasks.size(), std::ref(f4));

    while (f1 == false || f2 == false || f3 == false || f4 == false)
    {
        printf("Rendering Progress: %.3f%%\r", n/float(tasks.size())*100.0f);
        fflush(stdout);
        img->draw();
        glFinish();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    img->draw();
    glFinish();
    printf("Rendering Progress: 100.000%%\n");
    debug("done Pathtracing!\n");
}

#include "Random.hpp"

void
Scene::photontraceImage(Camera *cam, Image *img)
{
    std::cout << "photon trace" << std::endl;
    const unsigned int max_photons = 1e4; // 10.000.000
    const unsigned int max_bounce = 10;
    Vector3 surface[512][512] = {};
    unsigned int j = 0;

    for (Object *emitter : m_emitter_objects)
    {
        for (unsigned int i = 0 ; i < max_photons / m_emitter_objects.size() ; ++i)
        {
            //float   probability = 1.0f;
            Photon p = emitter->random_photon();
            for (unsigned int b = 0 ; b < max_bounce /*|| Random::linear_bounded(0.0f, 1.0f) < 0.9f */; ++b)
            {
                HitInfo h, h2;
                //if (b < max_bounce == false) probability = 0.9f;
                if (trace(h, p, std::numeric_limits<float>::epsilon()) == false) break;
                Ray s_r(h.P, (cam->eye() - h.P).normalize());
                if (trace(h2, s_r, std::numeric_limits<float>::epsilon()) == false)
                {
                    auto sp = g_camera->world_point_to_camera_point(h.P);
                    //if (sp.first >= 0 && sp.first < img->width() && sp.second >= 0 && sp.second < img->height()) {
                        surface[sp.second][sp.first] += p.m_spectrum * std::max(0.0f, dot(h.N, s_r.d));
                    //}
                }
                p.o = h.P;
                p.d = Random::hemispherical(h.N);
                p.m_spectrum = (p.m_spectrum * h.material->reflectance() + h.material->emittance_per_point()) * std::max(0.0f, dot(p.d, h.N));
            }
            std::cout << (float)(++j) / (float)(max_photons) << "           " << "\r" << std::flush;
        }
    }
    for (int y = 0 ; y < 512 ; ++y)
    {
        for (int x = 0 ; x < 512 ; ++x)
        {
            img->setPixel(x, y, surface[y][x] / 50000.0f);
            if (x == 256 && y == 256)
                std::cout << surface[y][x] << std::endl;
        }
    }
}

Path
Scene::compute_path(Photon &ph, float prob, int max_bounce) // TODO
{
    HitInfo h;
    Path    p;

    p.m_path.push_back(std::make_pair(ph, prob));
    if (max_bounce < 1) return p;
    if (trace(h, ph, std::numeric_limits<float>::epsilon()) == true) {
        Photon nph;
        nph.o = h.P;
        nph.d = Random::hemispherical(h.N);
        nph.m_spectrum = (ph.m_spectrum + h.material->emittance_per_point()) * h.material->reflectance() * dot(h.N, nph.d);
        float nprob = dot(h.N, nph.d);
        if (h.material->emittance_per_point().x > 0.0f) return p;
        Path np = this->compute_path(nph, nprob, max_bounce - 1);
        p.m_path.insert(p.m_path.end(), np.m_path.begin(), np.m_path.end());
    }
    return p;
}

Path
Scene::compute_eye_path(Camera *cam, Image *img, int x, int y) // TODO
{
    Ray     eye_ray = cam->eyeRay(x, y, img->width(), img->height());

    Photon eye_photon;
    eye_photon.o = eye_ray.o;
    eye_photon.d = eye_ray.d;
    eye_photon.m_spectrum = 0.0f;
    return compute_path(eye_photon, 1.0f, 5);
}

Path
Scene::compute_light_path(void)
{
    Photon  pr;
    int     ln = Random::linear_bounded(0.0f, m_emitter_objects.size());
    int i = 0;
    float prob = 0.0f;
    for (Object *obj : m_emitter_objects)
    {
        if (i == ln)
        {
            pr = obj->random_photon();
            prob = dot(Vector3(0.0f, 0.0f, 1.0f), pr.d);
            break;
        }
        ++i;
    }
    return compute_path(pr, prob, 5);
}

Vector3
Scene::mix_path(const Path &p1, int i, const Path &p2, int j)
{
    Vector3     c = 0.0f;
    Vector3     eli = 0.0f;

    if (i >= p1.m_path.size()) return c;
    if (i < p1.m_path.size()) c += mix_path(p1, i + 1, p2, j) * p1.m_path[i].second;

    for (auto l : p2.m_path)
    {
        Photon r = l.first;
        HitInfo     h;

        Ray     s_r;
        s_r.o = p1.m_path[i].first.o;
        s_r.d = (r.o - s_r.o).normalize();
        if (dot(s_r.d, r.d) < 0 && trace(h, s_r) == false)
        {
            float prob = dot(h.N, s_r.d);
            s_r.o = h.P;
            s_r.d = -s_r.d;
            trace(h, s_r);
            c += r.m_spectrum/* * dot(s_r.d, h.N) * prob*/ * l.second;
        }
    }
    return c;
}

void
Scene::bidirectional_pathtrace_image(Camera *cam, Image *img)
{
    const unsigned int  max_samples = 100;

    for (unsigned int y = 0 ; y < img->height() ; ++y)
    {
        for (unsigned int x = 0 ; x < img->width() ; ++x)
        {
            Vector3 color = 0.0f;
            for (unsigned int s = 0 ; s < max_samples ; ++s)
            {
                Path ep = this->compute_eye_path(cam, img, x, y);
                Path lp = this->compute_light_path();
                color += this->mix_path(ep, 1, lp, 0) / (float)max_samples;
            }
            img->setPixel(x, y, color / 100.0f);
        }
        printf("Rendering Progress: %.3f%%\r", y/float(img->height())*100.0f);
        img->drawScanline(y);
        glFinish();
        fflush(stdout);
    }
    printf("Rendering Progress: 100.000%%\n");
    debug("done Bidirectional Pathtracing !\n");
}

#include "photonmap.h"
#include <functional>

void
Scene::photonmap_image(Camera *cam, Image *img)
{
    unsigned int max_photons = 1e7 * 4; // 1 000 000
    unsigned int max_bounce = 6;
    unsigned int max_photons_m = max_photons * max_bounce;
    unsigned int j = 0;
    Vector3 *surface[img->height()] = {};

    for (int i = 0 ; i < img->height() ; ++i)
    {
        surface[i] = new Vector3[img->width()]();
    }

    float radius = 0.1f;
    float caustic_radius = 0.05f;

    bool first = true;

    float pass = 1.0f;

    for (;pass < 20.0f;)
    {

        Photon_map  m(max_photons_m);
        Photon_map  caustic_map(max_photons_m);
        std::cout << "photonmap" << std::endl;

        for (Object *emitter : m_emitter_objects)
        {
            for (unsigned int i = 0 ; i < max_photons / m_emitter_objects.size() ; ++i)
            {
                bool    caustic = false;
                //float   probability = 1.0f;
                Photon p = emitter->random_photon();
                p.d.normalize();
                for (unsigned int b = 0 ; b < max_bounce /*|| Random::linear_bounded(0.0f, 1.0f) < 0.9f */; ++b)
                {
                    HitInfo h, h2;
                    //if (b < max_bounce == false) probability = 0.9f;
                    if (trace(h, p, std::numeric_limits<float>::epsilon()) == true)
                    {


                        float r = Random::linear_bounded(0.0f, 1.0f);
                        p.o = h.P;

                        float n1 = 1.00029f;
                        float n2 = 1.00029f;

                        float material_reflectance = 0.0f;
                        float material_transmittance = 0.0f;


                        Vector3 N = 0.0f;

                        if (dot(-p.d, h.N) < 0.0f) {
                            n1 = h.material->refractive_index();
                            material_reflectance = h.material->reflectance(p.d, -h.N, n1, n2);
                            material_transmittance = h.material->transmittance(p.d, -h.N, n1, n2);
                            N = -h.N;
                        } else {
                            n2 = h.material->refractive_index();
                            material_reflectance = h.material->reflectance(p.d, h.N, n1, n2);
                            material_transmittance = h.material->transmittance(p.d, h.N, n1, n2);
                            N = h.N;
                        }


                        if (r < material_reflectance)
                        {
                            if (caustic == true) {
                                caustic_map.store(&p.m_spectrum.x, &h.P.x, &p.d.x);
                            } else {
                                m.store(&p.m_spectrum.x, &h.P.x, &p.d.x);
                            }
                            if (h.material->refractive_index() == 1.0f) {
                                p.d = Random::hemispherical(h.N); // lambertian
                            } else
                                p.d = Vector3::reflect(p.d, N); // specular
                            p.d.normalize();
                            p.m_spectrum = (p.m_spectrum * material_reflectance + h.material->emittance_per_point()) * h.material->diffuse() * h.material->pdf(h.N, p.d);
                        }
                        else if (r < material_reflectance + material_transmittance)
                        {
                            //break;
                            caustic = true;
                            Vector3 refract_ray = 0.0f;
                            if (dot(-p.d, h.N) < 0.0f) {
                                refract_ray = Vector3::refract(-h.N, p.d, n1 / n2);
                            } else {
                                refract_ray = Vector3::refract(h.N, p.d, n1 / n2);
                            }
                            if (refract_ray.x == 0.0f && refract_ray.y == 0.0f && refract_ray.z == 0.0f) {
                                if (dot(-p.d, h.N) < 0.0f) {
                                    p.d = Vector3::reflect(p.d, -h.N); // specular
                                } else {
                                    p.d = Vector3::reflect(p.d, h.N); // specular
                                }
                                p.m_spectrum = (p.m_spectrum + h.material->emittance_per_point()) * h.material->diffuse();
                            } else {
                                p.m_spectrum = (p.m_spectrum * material_transmittance + h.material->emittance_per_point()) * h.material->diffuse();
                                p.d = refract_ray;
                            }
                        } else {
                            break;
                        }
                    }
                }
                std::cout << (float)(++j) / (float)(max_photons) << "           " << "\r" << std::flush;
            }
        }
        std::cout << "photonmap building done" << std::endl;

        caustic_map.balance();
        m.balance();
        caustic_map.scale_photon_power(1.0f / (max_photons * max_bounce * 10.0f));
        m.scale_photon_power(1.0f / (max_photons * max_bounce * 10.0f));

        j = 0;
        for (unsigned int y = 0 ; y < img->height() ; ++y)
        {
            for (unsigned int x = 0 ; x < img->width() ; ++x)
            {

                Ray     ray = cam->eyeRay(x, y, img->width(), img->height());

                HitInfo hitInfo;

                int photon_hit = 0;


                Vector3 shadeResult;
                if (trace(hitInfo, ray)) {

                    int h = 0;


                    if (hitInfo.material->emittance_per_point().x > 0.0f) {
                        shadeResult = g_camera->bgColor(ray.d);
                    } else {


                        std::function<Vector3 (const Ray &, int)>    launch_ray = [&, this](const Ray &r, int bounce) -> Vector3 {

                            if (bounce > max_bounce) {
                                return 1.0f;
                            }

                            HitInfo h;
                            Vector3 shadeResult = 0.0f;

                            if (trace(h, r, 0.001f)) {

                                float n1 = 1.00029f;
                                float n2 = 1.00029f;
                                float material_reflectance = 0.0f;
                                float material_transmittance = 0.0f;

                                Vector3     N = h.N;

                                if (dot(-ray.d, h.N) < 0.0f) {
                                    n1 = h.material->refractive_index();
                                    material_reflectance = h.material->reflectance(ray.d, -h.N, n1, n2);
                                    material_transmittance = h.material->transmittance(ray.d, -h.N, n1, n2);
                                    N = -h.N;
                                } else {
                                    n2 = h.material->refractive_index();
                                    material_reflectance = h.material->reflectance(ray.d, h.N, n1, n2);
                                    material_transmittance = h.material->transmittance(ray.d, h.N, n1, n2);
                                }

                                if (h.material->refractive_index() == 1.0f) {

                                    Vector3 c = 0.0f;
                                    caustic_map.irradiance_estimate(&c.x, &h.P.x, &h.N.x, caustic_radius, 50);
                                    photon_hit = m.irradiance_estimate(&shadeResult.x, &h.P.x, &h.N.x, radius, 50);
                                    shadeResult += c;
                                    shadeResult *= material_reflectance;


                                } else {

                                    if (material_reflectance > 0.0f) {
                                        Ray         reflect_ray;
                                        HitInfo     reflect_hit;

                                        reflect_ray.o = h.P;
                                        reflect_ray.d = Vector3::reflect(ray.d, N);

                                        shadeResult += launch_ray(reflect_ray, bounce + 1) * material_reflectance;
                                    }

                                    if (material_transmittance > 0.0f) {
                                        Ray     refract_ray;
                                        HitInfo refract_hit;

                                        refract_ray.o = h.P;
                                        refract_ray.d = Vector3::refract(N, ray.d, n1 / n2);
                                        shadeResult += launch_ray(refract_ray, bounce + 1) * material_transmittance;
                                    }

                                }

                            } else {
                                shadeResult = g_camera->bgColor(r.d);
                            }
                            return shadeResult;
                        };

                        HitInfo h = hitInfo;

                        float n1 = 1.00029f;
                        float n2 = 1.00029f;
                        float material_reflectance = 0.0f;
                        float material_transmittance = 0.0f;

                        Vector3     N = h.N;

                        if (dot(-ray.d, h.N) < 0.0f) {
                            n1 = h.material->refractive_index();
                            material_reflectance = h.material->reflectance(ray.d, -h.N, n1, n2);
                            material_transmittance = h.material->transmittance(ray.d, -h.N, n1, n2);
                            N = -h.N;
                        } else {
                            n2 = h.material->refractive_index();
                            material_reflectance = h.material->reflectance(ray.d, h.N, n1, n2);
                            material_transmittance = h.material->transmittance(ray.d, h.N, n1, n2);
                        }


                        if (h.material->refractive_index() == 1.0f) {
                            Vector3 c = 0.0f;
                            caustic_map.irradiance_estimate(&c.x, &hitInfo.P.x, &hitInfo.N.x, caustic_radius, 50);
                            photon_hit = m.irradiance_estimate(&shadeResult.x, &hitInfo.P.x, &hitInfo.N.x, radius, 50);
                            shadeResult += c;
                            shadeResult *= material_reflectance;
                        } else {
                            if (material_reflectance > 0.0f) {
                                Ray         reflect_ray;
                                HitInfo     reflect_hit;

                                reflect_ray.o = h.P;
                                reflect_ray.d = Vector3::reflect(ray.d, N);

                                shadeResult += launch_ray(reflect_ray, 0) * material_reflectance;
                            }


                            if (material_transmittance > 0.0f) {

                                Ray     refract_ray;
                                HitInfo refract_hit;

                                refract_ray.o = h.P;
                                refract_ray.d = Vector3::refract(N, ray.d, n1 / n2);

                                shadeResult += launch_ray(refract_ray, 0) * material_transmittance;
                            }
                        }

                        shadeResult *= h.material->diffuse();
                    }

                } else {
                    shadeResult = g_camera->bgColor(ray.d);
                }
                /*        if (first)*/
                //surface[y][x] = shadeResult;
                surface[y][x] = surface[y][x] * ((pass - 1.0f) / pass) + shadeResult * (1.0f / pass);
                //std::cout << shadeResult << std::endl;
                img->setPixel(x, y, surface[y][x]);
                ++j;
            }
            std::cout << (float)(j) / (float)(img->height() * img->width()) << "           " << "\r" << std::flush;
            img->drawScanline(y);
        }
        glFinish();
        pass += 1.0f;
        first = false;
        radius *= 0.95f;
        j = 0;
    }
}


void
Scene::raytraceImage(Camera *cam, Image *img)
{
    Ray ray;
    HitInfo hitInfo;
    Vector3 shadeResult;

     std::atomic<bool>  f1;
     std::atomic<bool>  f2;
     std::atomic<bool>  f3;
     std::atomic<bool>  f4;

     f1 = false;
     f2 = false;
     f3 = false;
     f4 = false;

    std::atomic<int>    n;
    n = 0;

/*    const auto &worker_routine = [=, &n](int id, const std::vector<std::pair<int, int>> &tasks, int start, int end, std::atomic<bool> &f) {*/

        //std::vector<ray> rays;
        //std::vector<hitinfo> hitinfos;

        //hitinfos.emplace_back();
        //hitinfos.emplace_back();
        //hitinfos.emplace_back();
        //hitinfos.emplace_back();

        //for (int t = start ; t < end ; t += 4) {

            //rays.push_back(cam->eyeray(tasks[t + 0].first, tasks[t + 0].second, img->width(), img->height()));
            //rays.push_back(cam->eyeray(tasks[t + 1].first, tasks[t + 1].second, img->width(), img->height()));
            //rays.push_back(cam->eyeray(tasks[t + 2].first, tasks[t + 2].second, img->width(), img->height()));
            //rays.push_back(cam->eyeray(tasks[t + 3].first, tasks[t + 3].second, img->width(), img->height()));

            //vector3 shaderesult;

            //const std::vector<bool> &results = trace(hitinfos, rays);

            //if (!results.empty())
            //{
                //if (results[0])
                //{
                    //shaderesult = hitinfos[0].material->shade(ray, hitinfos[0], *this);
                    //if (std::isnan(shaderesult.x) || std::isnan(shaderesult.y) || std::isnan(shaderesult.z))
                        //std::cerr << "nan value" << std::endl;
                    //img->setpixel(tasks[t + 0].first, tasks[t + 0].second, shaderesult);
                //} else {
                    //img->setpixel(tasks[t + 0].first, tasks[t + 0].second, g_camera->bgcolor(ray.d));
                //}

                //if (results[1])
                //{
                    //shaderesult = hitinfos[1].material->shade(ray, hitinfos[1], *this);
                    //if (std::isnan(shaderesult.x) || std::isnan(shaderesult.y) || std::isnan(shaderesult.z))
                        //std::cerr << "nan value" << std::endl;
                    //img->setpixel(tasks[t + 1].first, tasks[t + 1].second, shaderesult);
                //} else {
                    //img->setpixel(tasks[t + 1].first, tasks[t + 1].second, g_camera->bgcolor(ray.d));
                //}

                //if (results[2])
                //{
                    //shaderesult = hitinfos[2].material->shade(ray, hitinfos[2], *this);
                    //if (std::isnan(shaderesult.x) || std::isnan(shaderesult.y) || std::isnan(shaderesult.z))
                        //std::cerr << "nan value" << std::endl;
                    //img->setpixel(tasks[t + 2].first, tasks[t + 2].second, shaderesult);
                //} else {
                    //img->setpixel(tasks[t + 2].first, tasks[t + 2].second, g_camera->bgcolor(ray.d));
                //}

                //if (results[3])
                //{
                    //shaderesult = hitinfos[3].material->shade(ray, hitinfos[3], *this);
                    //if (std::isnan(shaderesult.x) || std::isnan(shaderesult.y) || std::isnan(shaderesult.z))
                        //std::cerr << "nan value" << std::endl;
                    //img->setpixel(tasks[t + 3].first, tasks[t + 3].second, shaderesult);
                //} else {
                    //img->setpixel(tasks[t + 3].first, tasks[t + 3].second, g_camera->bgcolor(ray.d));
                //}
            //}

            //n += 4;
            //rays.clear();

        //}
        //f = true;
    //};



    const auto &worker_routine = [=, &n](int id, const std::vector<std::pair<int, int>> &tasks, int start, int end, std::atomic<bool> &f) {
        for (int t = start ; t < end ; ++t) {
            int i = tasks[t].first;
            int j = tasks[t].second;
            Ray ray = cam->eyeRay(i, j, img->width(), img->height());
            HitInfo hitInfo;
            Vector3 shadeResult;
            if (trace(hitInfo, ray))
            {
                shadeResult = hitInfo.material->shade(ray, hitInfo, *this);
                if (std::isnan(shadeResult.x) || std::isnan(shadeResult.y) || std::isnan(shadeResult.z))
                    std::cerr << "NAN VALUE" << std::endl;
                img->setPixel(i, j, shadeResult);
            } else {
                 img->setPixel(i, j, g_camera->bgColor(ray.d));
            }
            ++n;
        }
        f = true;
    };

    std::vector<std::pair<int, int>> tasks;

    // loop over all pixels in the image
    tasks.reserve(img->height() * img->width());
    for (int j = 0; j < img->height(); ++j)
    {
        for (int i = 0 ; i < img->width() ; ++i)
        {
            tasks.push_back(std::make_pair(i, j));
        }
    }

/*     std::random_device rd;*/
     //std::mt19937 g(rd());
     //std::shuffle(tasks.begin(), tasks.end(), g);

     std::thread t1(worker_routine, 0, std::ref(tasks), 0.0f / 4.0f * tasks.size(), 1.0f / 4.0f * tasks.size(), std::ref(f1));
     std::thread t2(worker_routine, 1, std::ref(tasks), 1.0f / 4.0f * tasks.size(), 2.0f / 4.0f * tasks.size(), std::ref(f2));
     std::thread t3(worker_routine, 2, std::ref(tasks), 2.0f / 4.0f * tasks.size(), 3.0f / 4.0f * tasks.size(), std::ref(f3));
     std::thread t4(worker_routine, 3, std::ref(tasks), 3.0f / 4.0f * tasks.size(), 4.0f / 4.0f * tasks.size(), std::ref(f4));

     while (f1 == false || f2 == false || f3 == false || f4 == false)
     {
         printf("Rendering Progress: %.3f%%\r", n/float(tasks.size())*100.0f);
         fflush(stdout);
         img->draw();
         glFinish();
         std::this_thread::sleep_for(std::chrono::milliseconds(10));
     }

     t1.join();
     t2.join();
     t3.join();
     t4.join();

    printf("Rendering Progress: 100.000%%\n");
    debug("done Raytracing!\n");
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    return m_bvh.intersect(minHit, ray, tMin, tMax);
}

std::vector<bool>
Scene::trace(std::vector<HitInfo>& minHit, const std::vector<Ray>& ray, float tMin, float tMax) const
{
    return m_bvh.intersect(minHit, ray, tMin, tMax);
}

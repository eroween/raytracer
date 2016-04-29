#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#include <atomic>
#include <thread>
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>

Scene * g_scene = 0;

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

/*Vector3*/
//Scene::pathtrace(const Ray &ray, int depth)
/*{*/
/*    const int MAX_DEPTH = 10;*/
    //HitInfo     hit;
    //Vector3     emittance;

    //if (depth > MAX_DEPTH) {
        //return 0.0f;
    //}

    //if (trace(hit, ray, 0.0001f) == false) {
        //return g_camera->bgColor(ray.d);
    //}

    //emittance = hit.material->pathtrace();

/*    Ray     newRay;*/
    //newRay.o = hit.P;
    //newRay.d = Vector3::hemispherical_random(hit.N);

    //float costheta = dot(newRay.d, hit.N);
    //Vector3 BRDF = 2.0f * hit.material->reflectance() * costheta;
    //Vector3 reflected = pathtrace(newRay, depth + 1);

    //return emittance;
/*    return 0.0f;*/
//}

void
Scene::pathtraceImage(Camera *cam, Image *img)
{
    int MAX_SAMPLE = 1000;
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
                if (trace(hitInfo, ray))
                {
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
    printf("Rendering Progress: 100.000%\n");
    debug("done Pathtracing!\n");
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
                if (isnan(shadeResult.x) || isnan(shadeResult.y) || isnan(shadeResult.z))
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

     std::random_device rd;
     std::mt19937 g(rd());
     std::shuffle(tasks.begin(), tasks.end(), g);

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

    printf("Rendering Progress: 100.000%\n");
    debug("done Raytracing!\n");
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    return m_bvh.intersect(minHit, ray, tMin, tMax);
}

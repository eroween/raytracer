#ifndef CSE168_SCENE_H_INCLUDED
#define CSE168_SCENE_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "PointLight.h"
#include "BVH.h"
#include "Path.hpp"

#include <list>

class Camera;
class Image;

class Scene
{
public:
    void addObject(Object* pObj)        {m_objects.push_back(pObj);}
    const Objects* objects() const      {return &m_objects;}

    void addLight(PointLight* pObj)     {m_lights.push_back(pObj);}
    const Lights* lights() const        {return &m_lights;}

    void add_emitter_object(Object *emitter) {
        m_emitter_objects.push_back(emitter);
    }

    void preCalc();
    void openGL(Camera *cam);

    void raytraceImage(Camera *cam, Image *img);
    void pathtraceImage(Camera *cam, Image *img);
    void photontraceImage(Camera *cam, Image *img);
    void bidirectional_pathtrace_image(Camera *cam, Image *img);
    void photonmap_image(Camera *cam, Image *img);

private:
    Path compute_eye_path(Camera *cam, Image *img, int x, int y);
    Path compute_light_path(void);
    Path compute_path(Photon &r, float, int);
    Vector3 mix_path(const Path &p1, int i, const Path &p2, int j);

public:

    bool trace(HitInfo& minHit, const Ray& ray, float tMin = 0.0f, float tMax = MIRO_TMAX) const;
    std::vector<bool> trace(std::vector<HitInfo> &minHit, const std::vector<Ray> &ray, float tMin = 0.0f, float tMax = MIRO_TMAX) const;

protected:
    Objects m_objects;
    BVH m_bvh;
    Lights m_lights;

    std::list<Object *>     m_emitter_objects;

};

extern Scene * g_scene;

#endif // CSE168_SCENE_H_INCLUDED

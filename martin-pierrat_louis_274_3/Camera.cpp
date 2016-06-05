#include <stdio.h>
#include <stdlib.h>
#include "Miro.h"
#include "Camera.h"
#include "Image.h"
#include "Scene.h"
#include "Console.h"
#include "OpenGL.h"
#include "Matrix4x4.h"

#include "Statistic.hpp"

#include <chrono>

Camera * g_camera = 0;

static bool firstRayTrace = true;

const float HalfDegToRad = DegToRad/2.0f;

Camera::Camera() :
    m_map_type(SPHERICAL_MAPPING),
    m_img(0),
    m_bgColor(0,0,0),
    m_renderer(RENDER_OPENGL),
    m_eye(0,0,0),
    m_viewDir(0,0,-1),
    m_up(0,1,0),
    m_lookAt(FLT_MAX, FLT_MAX, FLT_MAX),
    m_fov((45.)*(PI/180.))
{
    calcLookAt();
}


Camera::~Camera()
{
    if (this->m_img != 0)
    {
        delete[] this->m_img;
    }
}


void
Camera::click(Scene* pScene, Image* pImage)
{
    calcLookAt();
#ifdef  NO_OPENGL
    static bool firstRayTrace = true;
#else
    static bool firstRayTrace = false;
#endif


    Statistic::number_ray = 0;
    Statistic::number_ray_box_intersection = 0;
    Statistic::number_ray_triangle_intersection = 0;

    if (m_renderer == RENDER_OPENGL)
    {
        glDrawBuffer(GL_BACK);
        pScene->openGL(this);
        firstRayTrace = true;
    }
    else if (m_renderer == RENDER_RAYTRACE)
    {


        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDrawBuffer(GL_FRONT);
        if (firstRayTrace)
        {
            pImage->clear(bgColor());


            std::chrono::time_point<std::chrono::system_clock> start, end;
            start = std::chrono::system_clock::now();




            pScene->raytraceImage(this, g_image);


            end = std::chrono::system_clock::now();

            std::chrono::duration<double> elapsed_seconds = end-start;
            std::time_t end_time = std::chrono::system_clock::to_time_t(end);

            std::cout << "finished computation at " << std::ctime(&end_time)
                << "elapsed time: " << elapsed_seconds.count() << "s\n";



            firstRayTrace = false;
        }



        g_image->draw();


    }
    else if (this->m_renderer == RENDER_PATHTRACE)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDrawBuffer(GL_FRONT);
        if (firstRayTrace)
        {
            pImage->clear(bgColor());
            pScene->pathtraceImage(this, g_image);
            firstRayTrace = false;
        }
        g_image->draw();
    }
    else if (this->m_renderer == RENDER_PHOTONTRACE)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDrawBuffer(GL_FRONT);
        if (firstRayTrace)
        {
            pImage->clear(bgColor());
            pScene->photontraceImage(this, g_image);
            firstRayTrace = false;
        }
        g_image->draw();
    }
    else if (this->m_renderer == RENDER_BIDIRECTIONAL_PATHTRACER)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDrawBuffer(GL_FRONT);
        if (firstRayTrace)
        {
            pImage->clear(bgColor());
            pScene->bidirectional_pathtrace_image(this, g_image);
            firstRayTrace = false;
        }
        g_image->draw();
    }
    else if (this->m_renderer == RENDER_PHOTON_MAPPING)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDrawBuffer(GL_FRONT);
        if (firstRayTrace)
        {
            pImage->clear(bgColor());
            pScene->photonmap_image(this, g_image);
            firstRayTrace = false;
        }
        g_image->draw();
    }


    if (m_renderer != RENDER_OPENGL)
    {
        std::cout << "--------------statistics---------------" << std::endl;
        std::cout << "BVH Node : " << Statistic::number_bvh_nodes << std::endl;
        std::cout << "BVH Leaf Node : " << Statistic::number_bvh_leaf_nodes << std::endl;
        std::cout << "Ray number : " << Statistic::number_ray << std::endl;
        std::cout << "Ray/Box intersection : " << Statistic::number_ray_box_intersection << std::endl;
        std::cout << "Ray/Triangle intersection : " << Statistic::number_ray_triangle_intersection << std::endl;
        std::cout << "---------------------------------------" << std::endl;
    }
}


void
Camera::calcLookAt()
{
    // this is true when a "lookat" is not used in the config file
    if (m_lookAt.x != FLT_MAX)
    {
        setLookAt(m_lookAt);
        m_lookAt.set(FLT_MAX, FLT_MAX, FLT_MAX);
    }
}


void
Camera::drawGL()
{
    // set up the screen with our camera parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov(), g_image->width()/(float)g_image->height(), 0.01, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector3 vCenter = eye() + viewDir();
    gluLookAt(eye().x, eye().y, eye().z, vCenter.x, vCenter.y, vCenter.z, up().x, up().y, up().z);
}


Ray
Camera::eyeRay(int x, int y, int imageWidth, int imageHeight)
{
    // first compute the camera coordinate system
    // ------------------------------------------

    // wDir = e - (e+m_viewDir) = -m_vView
    const Vector3 wDir = Vector3(-m_viewDir).normalize();
    const Vector3 uDir = cross(m_up, wDir).normalize();
    const Vector3 vDir = cross(wDir, uDir);



    // next find the corners of the image plane in camera space
    // --------------------------------------------------------

    const float aspectRatio = (float)imageWidth/(float)imageHeight;


    const float top     = tan(m_fov*HalfDegToRad);
    const float right   = aspectRatio*top;

    const float bottom  = -top;
    const float left    = -right;


    // transform x and y into camera space
    // -----------------------------------

    const float imPlaneUPos = left   + (right - left)*(((float)x+0.5f)/(float)imageWidth); // you can replace +0.5f by random::linear(0, 1)
    const float imPlaneVPos = bottom + (top - bottom)*(((float)y+0.5f)/(float)imageHeight); // you can replace +0.5f by random::linear(0, 1)

    return Ray(m_eye, (imPlaneUPos*uDir + imPlaneVPos*vDir - wDir).normalize());
}

Vector3
Camera::bgColor(const Vector3 &ray) const
{
    if (this->m_img == 0) return this->bgColor();
    float m = 2.0f * sqrt(ray.x * ray.x + ray.z * ray.z + (ray.y + 1.0f) * (ray.y + 1.0f));
    float u = ray.x / m + 0.5f;
    float v = ray.z / m + 0.5f;
    return this->m_img[(int)(this->m_h * (int)(this->m_h * v) + this->m_w * u)];
}

Matrix4x4   lookat(const Vector3 &origin, const Vector3 &position, const Vector3 &up)
{
    Vector3 f = (position - origin).normalize();
    Vector3 s = (cross(up, f)).normalize();
    Vector3 u = cross(f, s);
    Matrix4x4 r(
            -s.x, u.x, -f.x, 0.0f,
            s.y, u.y, -f.y, 0.0f,
            s.z, u.z, -f.z, 0.0f,
            -dot(s, origin), -dot(u, origin), dot(f, origin), 1.0f
        );
    r.transpose();
    return r;
}

Matrix4x4   perspective(float fov, float aspect, float znear, float zfar)
{
    Matrix4x4 result;
    const float f = 1.0f / tan((fov * PI / 180.0f) / 2.0f);
    result.setIdentity();
    result.setColumn1(f / aspect, 0.0f,                                   0.0f,  0.0f);
    result.setColumn2(      0.0f,    f,                                   0.0f,  0.0f);
    result.setColumn3(      0.0f, 0.0f,        (zfar + znear) / (znear - zfar), -1.0f);
    result.setColumn4(      0.0f, 0.0f, (2.0f * zfar * znear) / (znear - zfar),  1.0f);
    return result;
}

/*std::pair<int, int>*/
//Camera::world_point_to_camera_point(const Vector3 &point)
//{
    //Matrix4x4 view_matrix = lookat(m_eye, m_eye + m_viewDir, m_up);
    //Matrix4x4 projection_matrix = perspective(m_fov, g_image->width() / (float)g_image->height(), 0.01f, 10000.0f);
    //Matrix4x4 mvp = projection_matrix * view_matrix;
    //Vector4 p = (mvp * Vector4(point, 1.0f)).normalize();
    //return std::make_pair((p.x + 0.5f) * g_image->width() + 0.5f, (p.y + 0.5f) * g_image->height() + 0.5f);
//}

std::pair<int, int>
Camera::world_point_to_camera_point(const Vector3 &point)
{
    const Vector3 wDir = Vector3(-m_viewDir).normalize();
    const Vector3 uDir = cross(m_up, wDir).normalize();
    const Vector3 vDir = cross(wDir, uDir);

    // next find the corners of the image plane in camera space
    // --------------------------------------------------------

    const float aspectRatio = (float)g_image->width()/(float)g_image->height();


    const float top     = tan(m_fov*HalfDegToRad);
    const float right   = aspectRatio*top;

    const float bottom  = -top;
    const float left    = -right;

    const Vector3   eye_to_point = point - m_eye;

    float u = dot(eye_to_point, uDir);
    float v = dot(eye_to_point, vDir);
    float w = dot(eye_to_point, wDir);

    u /= fabs(w);
    v /= fabs(w);
    w /= fabs(w);

    float   px = g_image->width() * ( u - left ) / (right - left) - 0.5f;
    float   py = g_image->height() * ( v - bottom ) / (top - bottom) - 0.5f;

    //auto r = std::make_pair((ax + 20.0f) / 40.0f * g_image->width(), (ay + 20.0f) / 40.0f * g_image->height());

    return std::make_pair((int)px, (int)py);
}

/*std::pair<int, int>*/
//Camera::world_point_to_camera_point(const Vector3 &point)
//{
    //const Vector3   eye_point = point - m_eye;
    //const Vector3   t1 = Vector3(eye_point.x, eye_point.y, 0.0f).normalize();
    //const Vector3   t2 = Vector3(0.0f, eye_point.y, eye_point.z).normalize();
    //const Vector3   vdir = (m_viewDir).normalize();
    //float ax = acos(dot(t1, vdir)) * 180.0f / PI;
    //float ay = acos(dot(t2, vdir)) * 180.0f / PI;
    //if (t1.x < 0.0f) ax *= -1;
    //if (t2.z < 0.0f) ay *= -1;
    //return std::make_pair((ax + 20.0f) / 40.0f * g_image->width(), (ay + 20.0f) / 40.0f * g_image->height());
/*}*/

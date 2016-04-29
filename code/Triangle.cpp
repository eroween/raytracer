#include <cmath>

#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"


Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{

}


Triangle::~Triangle()
{

}


void
Triangle::renderGL()
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}



bool
Triangle::intersect(HitInfo& result, const Ray& r, float tMin, float tMax)
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & vert0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & vert1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & vert2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    ti3 = m_mesh->nIndices()[m_index];
    const Vector3 &norm0 = m_mesh->normals()[ti3.x];
    const Vector3 &norm1 = m_mesh->normals()[ti3.y];
    const Vector3 &norm2 = m_mesh->normals()[ti3.z];

    // find vectors for two edges sharing vert0
    const Vector3 edge1 = vert1 - vert0;
    const Vector3 edge2 = vert2 - vert0;

    // begin calculating determinant  also used to calculate U parameter
    const Vector3 pvec = cross(r.d, edge2);

    // if determinant is near zero ray lies in plane of triangle
    float det = dot(edge1, pvec);

    if (det > -0.000001f && det < 0.000001) {
        return false;
    }

    float inv_det = 1.0f / det;

    // calculate distance from vert to ray origin

    const Vector3 tvec = r.o - vert0;

    // calculate U parameter and test bounds

    float u = dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
         return false;
    }

    const Vector3   qvec = cross(tvec, edge1);

    // calculate V parameter and test bounds

    float v = dot(r.d, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) {
         return false;
    }

    float t = dot(edge2, qvec) * inv_det;

    if (t < tMin || t > tMax) {
         return false;
    }

    result.t = t;
    result.P = (1.0f - u - v) * vert0 + u * vert1 + v * vert2;
    result.N = (1.0f - u - v) * norm0 + u * norm1 + v * norm2;
    //result.N = 1.0f/3.0f * norm0 + 1.0f/3.0f * norm1 + 1.0f/3.0f * norm2;
    result.N.normalize();
    result.material = this->m_material;

    return true;
}

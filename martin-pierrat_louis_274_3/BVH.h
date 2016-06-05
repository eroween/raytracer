#ifndef CSE168_BVH_H_INCLUDED
#define CSE168_BVH_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "AABB.hpp"

#include <list>

class BVH
{
    public:
        class   Node
        {
            public:
                Node(void);
                Node(AABB *aabb);
                ~Node(void);

            public:
                AABB    *aabb(void) { return this->m_aabb; }
                void    aabb(AABB *aabb) { this->m_aabb = aabb; }

                Objects *objects(void) const { return m_objects; }
                void    objects(Objects *objects) { this->m_objects = objects; }

                std::list<Object *>     *list_objects(void) const { return m_list_objects; }
                void    list_objects(std::list<Object *> *objects) { this->m_list_objects = objects; }

                Node    *right(void) const { return m_right; }
                Node    *left(void) const { return m_left; }

                bool    leaf(void) const { return m_left == nullptr && m_right == nullptr; }

                int     axis(void) const { return m_axis; }

            public:

                void    precompute();

            private:
                Node    *m_left;
                Node    *m_right;

                AABB    *m_aabb;
                Objects     *m_objects;
                std::list<Object *>     *m_list_objects;
                int     m_axis;
        };

    public:
        BVH(void);
        ~BVH(void);

    public:
        void build(Objects * objs);

        bool intersect(HitInfo& result, const Ray& ray, float tMin = 0.0f, float tMax = MIRO_TMAX) const;
        bool intersect(BVH::Node *n, HitInfo& re, const Ray& r, float tMin = 0.0f, float tMax = MIRO_TMAX) const;

        std::vector<bool> intersect(BVH::Node *n, std::vector<HitInfo> &re, const std::vector<Ray> &r, float tMin = 0.0f, float tMax = MIRO_TMAX) const;
        std::vector<bool> intersect(std::vector<HitInfo> &results, const std::vector<Ray> &rays, float tMin = 0.0f, float tMax = MIRO_TMAX) const;


    protected:
        Objects * m_objects;

        BVH::Node   *m_root_node;

};

#endif // CSE168_BVH_H_INCLUDED

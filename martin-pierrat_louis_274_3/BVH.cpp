#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include "Statistic.hpp"
#include <limits>

BVH::Node::Node(void) :
    m_left(nullptr),
    m_right(nullptr),
    m_aabb(nullptr),
    m_objects(nullptr)
{
    Statistic::number_bvh_nodes += 1;
}

BVH::Node::Node(AABB *aabb) :
    m_left(nullptr),
    m_right(nullptr),
    m_aabb(aabb),
    m_objects(nullptr)
{
    Statistic::number_bvh_nodes += 1;
}

BVH::Node::~Node(void)
{

}

void
BVH::Node::precompute()
{
    this->m_aabb = new AABB();

    for (Object *obj : *m_list_objects) {
        this->m_aabb->merge(obj->aabb());
    }


    //if (m_list_objects->size() < 4) { // totally wrong

            //Statistic::number_bvh_leaf_nodes += 1;
        //return;
    //}
        // split
        AABB    centroid_aabb;

        for (Object *obj : *m_list_objects) {
            centroid_aabb.merge(obj->aabb()->centroid());
        }

        int dim = centroid_aabb.maximum_extent();
        m_axis = dim;

        if (centroid_aabb.box_min()[dim] == centroid_aabb.box_max()[dim])
        {
            Statistic::number_bvh_leaf_nodes += 1;
            return;
        }

        struct  Bucket
        {
            Bucket() : count(0) {}
            int     count;
            AABB    aabb;
        };

        Bucket      buckets[12];

        for (Object *obj : *m_list_objects)
        {
            int     b = 12 * ((obj->aabb()->centroid()[dim] - centroid_aabb.box_min()[dim]) / (centroid_aabb.box_max()[dim] - centroid_aabb.box_min()[dim]));
            if (b == 12) b = 11;
            buckets[b].count++;
            buckets[b].aabb.merge(obj->aabb());
        }


        float   cost[11];

        for (int i = 0 ; i < 11 ; ++i)
        {
            AABB    b0, b1;
            int     c0, c1;
            c0 = c1 = 0;
            for (int j = 0 ; j < i + 1 ; ++j)
            {
                b0.merge(&buckets[j].aabb);
                c0 += buckets[j].count;
            }

            for (int j = i + 1 ; j < 12 ; ++j)
            {
                b1.merge(&buckets[j].aabb);
                c1 += buckets[j].count;
            }
            cost[i] = 0.125 + (c0 * b0.surface_area() + c1 * b1.surface_area()) / m_aabb->surface_area();
        }

        float min_cost = cost[0];
        std::size_t n = 0;
        for (int i = 1 ; i < 11 ; ++i)
        {
            if (cost[i] < min_cost)
            {
                min_cost = cost[i];
                n = i;
            }
        }


        if (min_cost > m_list_objects->size() && m_list_objects->size() < 9) {
            Statistic::number_bvh_leaf_nodes += 1;
            //std::cout << m_list_objects->size() << std::endl;
        //std::cout << min_cost << std::endl;
            return;
        }


        std::list<Object *>     *left = new std::list<Object *>();
        std::list<Object *>     *right = new std::list<Object *>();

        float pmid = 0.5f * (centroid_aabb.box_min()[dim] + centroid_aabb.box_max()[dim]);

        for (Object *obj : *m_list_objects)
        {

            int     b = 12 * ((obj->aabb()->centroid()[dim] - centroid_aabb.box_min()[dim]) / (centroid_aabb.box_max()[dim] - centroid_aabb.box_min()[dim]));
            if (b == 12) b = 11;

            if (b <= n) {
                left->push_back(obj);
            } else {
                right->push_back(obj);
            }


/*            if (obj->aabb()->centroid()[dim] < pmid) {*/
                //left->push_back(obj);
            //} else {
                //right->push_back(obj);
            //}



        }


        m_list_objects->clear();

        m_left = new BVH::Node();
        m_right = new BVH::Node();

        m_left->list_objects(left);
        m_right->list_objects(right);

        m_left->precompute();
        m_right->precompute();
}


BVH::BVH(void) :
    m_objects(nullptr),
    m_root_node(nullptr)
{

}


void
BVH::build(Objects * objs)
{
    std::list<Object *>     *left = new std::list<Object *>();
    for (Object *obj : *objs) {
        left->push_back(obj);
    }
    this->m_root_node = new BVH::Node();
    this->m_root_node->list_objects(left);
    this->m_root_node->precompute();
    // construct the bounding volume hierarchy
    m_objects = objs;
}

bool
BVH::intersect(BVH::Node *node, HitInfo &h, const Ray &r, float tmin, float tmax) const
{
    if (node == nullptr) return false;

    Vector3 min = node->aabb()->box_min();
    Vector3 max = node->aabb()->box_max();

    if (node->aabb()->intersect(r, tmin, tmax))
    {

        if (node->leaf()) {


            std::list<Object *>     *objects = node->list_objects();
            bool hit = false;
            HitInfo tempMinHit;
            h.t = MIRO_TMAX;

            for (Object *o : *objects)
            {
                if (o->intersect(tempMinHit, r, tmin, tmax))
                {
                    hit = true;
                    if (tempMinHit.t < h.t)
                        h = tempMinHit;
                }
            }
            return hit;
        } else {

            HitInfo hr;
            HitInfo hl;

            bool rr = false;
            bool rl = false;

/*            rr = intersect(node->right(), hr, r, tmin, tmax);*/
            //rl = intersect(node->left(), hl, r, tmin, tmax);


            if (r.d[node->axis()] < 0) {

                rr = intersect(node->right(), hr, r, tmin, tmax);
                if (rr == true)
                    rl = intersect(node->left(), hl, r, tmin, hr.t);
                else
                    rl = intersect(node->left(), hl, r, tmin, tmax);


            } else {
                rl = intersect(node->left(), hl, r, tmin, tmax);
                if (rl == true)
                    rr = intersect(node->right(), hr, r, tmin, hl.t);
                else
                    rr = intersect(node->right(), hr, r, tmin, tmax);
            }


            if (rr == true && rl == true) {
                if (hr.t < hl.t)
                    h = hr;
                else
                    h = hl;
            } else if (rr == true) {
                h = hr;
            } else {
                h = hl;
            }

            return rr || rl;

        }
    }
    return false;
}

std::vector<bool>
BVH::intersect(BVH::Node *node, std::vector<HitInfo> &h, const std::vector<Ray> &r, float tmin, float tmax) const
{
    if (node == nullptr) return std::vector<bool>();

    Vector3 min = node->aabb()->box_min();
    Vector3 max = node->aabb()->box_max();

    std::vector<bool>   intersected;

    for (const Ray &ri : r) {
        intersected.push_back(node->aabb()->intersect(ri, tmin, tmax));
    }

    for (int i = 0 ; i < r.size() ; ++i)
    {
        if (node->leaf()) {

            std::list<Object *>     *objects = node->list_objects();
            bool hit = false;
            HitInfo tempMinHit;

            HitInfo &ha = h[i];

            ha.t = MIRO_TMAX;

            for (Object *o : *objects)
            {
                if (o->intersect(tempMinHit, r[i], tmin, tmax))
                {
                    hit = true;
                    if (tempMinHit.t < ha.t)
                        ha = tempMinHit;
                }
            }
            intersected[i] = hit;
        } else {

            HitInfo hr;
            HitInfo hl;

            bool rr = false;
            bool rl = false;

/*            rr = intersect(node->right(), hr, r, tmin, tmax);*/
            //rl = intersect(node->left(), hl, r, tmin, tmax);


            if (r[i].d[node->axis()] < 0) {

                rr = intersect(node->right(), hr, r[i], tmin, tmax);
                if (rr == true)
                    rl = intersect(node->left(), hl, r[i], tmin, hr.t);
                else
                    rl = intersect(node->left(), hl, r[i], tmin, tmax);


            } else {
                rl = intersect(node->left(), hl, r[i], tmin, tmax);
                if (rl == true)
                    rr = intersect(node->right(), hr, r[i], tmin, hl.t);
                else
                    rr = intersect(node->right(), hr, r[i], tmin, tmax);
            }


            if (rr == true && rl == true) {
                if (hr.t < hl.t)
                    h[i] = hr;
                else
                    h[i] = hl;
            } else if (rr == true) {
                h[i] = hr;
            } else {
                h[i] = hl;
            }
            intersected[i] = rr || rl;
        }
    }
    return intersected;
}

std::vector<bool>
BVH::intersect(std::vector<HitInfo> &results, const std::vector<Ray> &rays, float tMin, float tMax) const
{
    return intersect(m_root_node, results, rays, tMin, tMax);
}

bool
BVH::intersect(HitInfo &h, const Ray &r, float tmin, float tmax) const
{
    return intersect(m_root_node, h, r, tmin, tmax);
}

//bool
//BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
//{
    //// Here you would need to traverse the BVH to perform ray-intersection
    //// acceleration. For now we just intersect every object.

    //bool hit = false;
    //HitInfo tempMinHit;
    //minHit.t = MIRO_TMAX;

    //for (size_t i = 0; i < m_objects->size(); ++i)
    //{

        //if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
        //{
            //hit = true;
            //if (tempMinHit.t < minHit.t)
                //minHit = tempMinHit;
        //}
    //}

    //return hit;
/*}*/

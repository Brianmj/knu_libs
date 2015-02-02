//
//  geometrics.hpp
//  Geometrics
//
//  Created by Brian Jones on 10/14/13.
//  Copyright (c) 2013 Brian Jones. All rights reserved.
//

#ifndef Geometrics_geometrics_hpp
#define Geometrics_geometrics_hpp

#include <knu/mathlibrary5.hpp>
#include <iostream>

using namespace knu::math;
using namespace knu::math::utility;

const float epsilon = 0.0004f;

template<typename VecType>
class Ray
{
    VecType beginPoint;
    VecType endPoint;
    VecType dir;
    
    void calculate_direction()
    {
        dir = endPoint - beginPoint;
    }
    
public:
    Ray():beginPoint(), endPoint(), dir()
    {}
    
    Ray(VecType b, VecType e):
    beginPoint(b), endPoint(e), dir()
    {
        calculate_direction();
    }
    
    VecType begin() const { return beginPoint; }
    
    void begin(VecType b) { beginPoint = b; calculate_direction(); }
    
    VecType end() const { return endPoint; }
    
    void end(VecType e) { endPoint = e; calculate_direction(); }
    
    void ray(VecType b, VecType e) { begin(b); end(e); }
    
    VecType direction() const { return dir; }
    
    float length() const {return dir.length();}
    VecType position_at(float t)
    {
        return begin() + (direction() * t);
    }
};

class Plane
{
    Vector3f n;
    Vector3f p;
    float d;
    
    void make_plane_from_points(Vector3f p1, Vector3f p2, Vector3f p3)
    {
        auto e3 = p2 - p1;
        auto e1 = p3 - p2;
        
        n = e3.cross(e1);
        n.normalize();
        p = p1;
        d = p1.dot(n);
    }
    
    void create_from_points(std::vector<knu::math::Vector3f> pts)
    {
        
    }
    
public:
    Plane():
    n(), p(), d(0.0f) {}
    
    Plane(Vector3f p1, Vector3f p2, Vector3f p3):
    n(), p(), d(0.0f)
    {
        make_plane_from_points(p1, p2, p3);
    }
    
    Vector3f point() const { return p; }
    Vector3f normal() const { return n; }
};

class Sphere
{
    Vector3f c;
    Vector3f origCen;        // original center point for restoration
    float r;
    float origRad;           // original radius for restoration;
    
    
public:
    Sphere():c(),
    origCen(),
    r(0.0f),
    origRad()
    {}
    
    void create_from_points(const std::vector<Vector3f> &pts)
    {
        Vector3f min(1.0f, 1.0f, 1.0f), max(-1.0f, -1.0f, -1.0f);
        
        for(const auto &p : pts)
        {
            if(p.x < min.x)
                min.x = p.x;
            else
                if(p.x > max.x)
                    max.x = p.x;
            
            if(p.y < min.y)
                min.y = p.y;
            else
                if(p.y > max.y)
                    max.y = p.y;
            if(p.z < min.z)
                min.z = p.z;
            else
                if(p.z > max.z)
                    max.z = p.z;
        }
        
        c = (min + max) * 0.5f;
        r = max.length();
        origCen = c;
        origRad = r;
    }
    
    inline Vector3f center() const { return c; }
    inline float radius() const { return r; }
    inline bool intersects(const Sphere &s) const
    {
        auto centerDiff = c - s.c;
        float radiusSum = r + s.r;
        
        return centerDiff.dot(centerDiff) <= (radiusSum * radiusSum);
    }
    
	inline void transform_from_current(const knu::math::m4f &m)
	{
		c = (knu::math::v4f(c, 1.0f) * m).get_vec3();
	}

	inline void transform_from_original(const knu::math::m4f &m)
	{
		c = (knu::math::v4f(origCen, 1.0f) * m).get_vec3();
	}
    
    inline bool inside_sphere(const knu::math::Vector3f &point)
    {
        auto d = point - c;
        auto l = d.length();
        return l <= r;
    }
    
    inline void restore()
    {
        c = origCen;
        r = origRad;
    }

};
#endif

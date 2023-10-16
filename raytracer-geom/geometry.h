#pragma once

#include <vector.h>
#include <sphere.h>
#include <intersection.h>
#include <triangle.h>
#include <ray.h>

#include <optional>
#include <math.h>
#include <cmath>

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    double eps = 1e-9;
    const Vector& ray_direction = ray.GetDirection();
    const Vector margin = ray.GetOrigin() - sphere.GetCenter();
    double r = sphere.GetRadius();
    // (x-x0)**2 + (y-y0)**2 + (z-z0)**2 = r**2
    // x = x1 + t * vx
    // y = y1 + t * vy
    // z = z1 + t * vz

    // (t * vx + x1 - x0)**2 + (t * vy + y1 - y0)**2 + (t * vz + z1 - z0)**2 = r**2

    // (t * vx)**2 + 2 * t * vx * (x1 - x0) + (x1 - x0)**2 + (t * vy)**2 + 2 * t * vy * (y1 - y0) +
    // + (y1 - y0)**2 + (t * vz)**2 + 2 * t * vz * (z1 - z0) + (z1 - z0)**2 = r**2

    // t**2 * (vx**2 + vy**2 + vz**2) + 2 * t * (vx * (x1 - x0) + vy * (y1 - y0) + vz * (z1 - z0)) +
    // + (x1 - x0)**2 + (y1 - y0)**2 + (z1 - z0)**2 - r**2 = 0

    double a, b, c;
    a = DotProduct(ray_direction, ray_direction);
    b = 2 * DotProduct(ray_direction, margin);
    c = DotProduct(margin, margin) - r * r;

    const double descriminant = b * b - 4 * a * c;
    if (descriminant < -eps) {
        return std::nullopt;
    } else if (descriminant < eps) {
        const double t = -b / (2 * a);
        if (t > eps) {
            Vector intersect_pos = ray.GetOrigin() + t * ray_direction;
            Vector normal = intersect_pos - sphere.GetCenter();
            return Intersection(intersect_pos, normal, t);
        } else {
            return std::nullopt;
        }
    } else {
        const double t1 = (-b - std::sqrt(descriminant)) / (2 * a);
        const double t2 = (-b + std::sqrt(descriminant)) / (2 * a);
        if (t1 > eps) {
            Vector intersect_pos = ray.GetOrigin() + t1 * ray_direction;
            Vector normal = intersect_pos - sphere.GetCenter();
            return Intersection(intersect_pos, normal, t1);
        } else if (t2 > eps) {
            Vector intersect_pos = ray.GetOrigin() + t2 * ray_direction;
            Vector normal = sphere.GetCenter() - intersect_pos;
            return Intersection(intersect_pos, normal, t2);
        } else {
            return std::nullopt;
        }
    }
}

std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
    double eps = 1e-9;
    Vector edge1 = triangle[1] - triangle[0];
    Vector edge2 = triangle[2] - triangle[0];
    const Vector& ray_direct = ray.GetDirection();
    const Vector& ray_origin = ray.GetOrigin();

    Vector pvec = CrossProduct(ray_direct, edge2);
    double det = DotProduct(pvec, edge1);

    if (std::fabs(det) < eps) {
        return std::nullopt;
    } else {
        double inv_det = 1 / det;
        Vector tvec = ray_origin - triangle[0];

        double u = inv_det * DotProduct(tvec, pvec);
        if (u < -eps || u > 1 + eps) {
            return std::nullopt;
        }

        Vector qvec = CrossProduct(tvec, edge1);
        double v = inv_det * DotProduct(ray_direct, qvec);
        if (v < -eps || u + v > 1 + eps) {
            return std::nullopt;
        }

        double t = inv_det * DotProduct(edge2, qvec);
        if (t > eps) {
            Vector intersect_pos = ray_origin + t * ray_direct;
            Vector normal = det > 0 ? CrossProduct(edge1, edge2) : CrossProduct(edge2, edge1);
            return Intersection(intersect_pos, normal, t);
        } else {
            return std::nullopt;
        }
    }
}

Vector Reflect(const Vector& ray, const Vector& normal) {
    double proj = -DotProduct(ray, normal);
    Vector reflect = ray + 2 * proj * normal;
    return reflect;
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    double eps = 1e-9;
    double proj = -DotProduct(ray, normal);
    double cos_phi2_sqr = 1 - eta * eta * (1 - proj * proj);
    if (cos_phi2_sqr < -eps) {
        return std::nullopt;
    } else {
        Vector refract = eta * ray + (eta * proj - std::sqrt(cos_phi2_sqr)) * normal;
        return refract;
    }
}

Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    Vector res;
    Vector r = triangle[0] - point;
    Vector edge1 = triangle[1] - triangle[0];
    Vector edge2 = triangle[2] - triangle[0];
    Vector comp0 = Vector(edge1[0], edge2[0], r[0]);
    Vector comp1 = Vector(edge1[1], edge2[1], r[1]);
    Vector normal = CrossProduct(comp0, comp1);
    normal /= normal[2];

    res[1] = normal[0];
    res[2] = normal[1];
    res[0] = 1.0 - res[1] - res[2];
    return res;
}

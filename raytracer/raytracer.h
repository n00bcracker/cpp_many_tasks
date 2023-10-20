#pragma once

#include <image.h>
#include <options/camera_options.h>
#include <options/render_options.h>
#include <scene.h>
#include <geometry.h>
#include <screen.h>

#include <filesystem>
#include <cmath>
#include <array>
#include <optional>

class LookAtCamera {
public:
    LookAtCamera(const CameraOptions& camera_options) {
        double eps = 1e-9;
        shift_ = camera_options.look_from;

        Vector up(0, 1, 0);
        Vector forward = camera_options.look_from - camera_options.look_to;
        forward.Normalize();
        Vector right;
        if (fabs(DotProduct(up, forward) - 1.0) < eps ||
            fabs(DotProduct(up, forward) + 1.0) < eps) {
            right = {1, 0, 0};
        } else {
            right = CrossProduct(up, forward);
        }

        right.Normalize();
        Vector newup = CrossProduct(forward, right);

        x_rotate_ = Vector(right[0], newup[0], forward[0]);
        y_rotate_ = Vector(right[1], newup[1], forward[1]);
        z_rotate_ = Vector(right[2], newup[2], forward[2]);
    }

    Ray RayTransform(const Ray& ray) const {
        Vector new_origin = ray.GetOrigin() + shift_;
        const Vector& direction = ray.GetDirection();
        Vector new_direction(DotProduct(direction, x_rotate_), DotProduct(direction, y_rotate_),
                             DotProduct(direction, z_rotate_));
        Ray new_ray(new_origin, new_direction);
        return new_ray;
    }

private:
    Vector x_rotate_;
    Vector y_rotate_;
    Vector z_rotate_;
    Vector shift_;
};

Screen CreateScreen(const CameraOptions& camera_options) {
    Screen screen(camera_options.screen_width, camera_options.screen_height);
    double height = 2 * std::tan(camera_options.fov / 2);
    double pixel_size = height / screen.GetHeight();
    double width = pixel_size * screen.GetWidth();

    double x, y, z = -1;
    for (size_t i = 0; i < screen.GetHeight(); ++i) {
        for (size_t j = 0; j < screen.GetWidth(); ++j) {
            x = -width / 2 + pixel_size * (j + 0.5);
            y = height / 2 - pixel_size * (i + 0.5);
            screen[i][j].SetCenterPosition({x, y, z});
        }
    }

    return screen;
}

std::optional<Vector> ComputeObjectNormal(const Ray& ray, const Object& object,
                                          const Vector& position) {
    double eps = 1e-9;
    Vector normal1 = *object.GetNormal(0);
    Vector normal2 = *object.GetNormal(1);
    Vector normal3 = *object.GetNormal(2);

    if (Length(normal1) > eps && Length(normal2) > eps && Length(normal3) > eps) {
        Vector bar_coord = GetBarycentricCoords(object.polygon, position);
        Vector normal = normal1 * bar_coord[0] + normal2 * bar_coord[1] + normal2 * bar_coord[2];
        if (DotProduct(ray.GetDirection(), normal) > eps) {
            normal = -1 * normal;
        }
        return normal;
    } else {
        return std::nullopt;
    }
}

std::optional<std::array<double, 3>> ComputeColor(const Ray& ray, const Scene& scena,
                                                  const RenderOptions& render_options) {
    const auto& objects = scena.GetObjects();
    const auto& spheres = scena.GetSphereObjects();

    std::optional<Intersection> nearest_intersection = std::nullopt;
    const Object* nearest_object = nullptr;
    const SphereObject* nearest_sphere = nullptr;

    for (const auto& object : objects) {
        auto intersection = GetIntersection(ray, object.polygon);
        if (intersection.has_value() &&
            (!nearest_intersection.has_value() ||
             (intersection->GetDistance() < nearest_intersection->GetDistance()))) {
            nearest_intersection = *intersection;
            nearest_object = &object;
        }
    }

    for (const auto& sphere : spheres) {
        auto intersection = GetIntersection(ray, sphere.sphere);
        if (intersection.has_value() &&
            (!nearest_intersection.has_value() ||
             (intersection->GetDistance() < nearest_intersection->GetDistance()))) {
            nearest_intersection = *intersection;
            nearest_sphere = &sphere;
        }
    }

    if (render_options.mode == RenderMode::kDepth) {
        if (nearest_intersection.has_value()) {
            double distance = nearest_intersection->GetDistance();
            return std::array<double, 3>{distance, distance, distance};
        } else {
            return std::nullopt;
        }
    } else if (render_options.mode == RenderMode::kNormal) {
        if (nearest_intersection.has_value()) {
            if (nearest_sphere) {
                const auto& normal = nearest_intersection->GetNormal();
                return std::array<double, 3>{normal[0], normal[1], normal[2]};
            } else if (nearest_object) {
                auto normal =
                    ComputeObjectNormal(ray, *nearest_object, nearest_intersection->GetPosition());
                if (!normal.has_value()) {
                    normal = nearest_intersection->GetNormal();
                }

                return std::array<double, 3>{(*normal)[0], (*normal)[1], (*normal)[2]};
            }
        } else {
            return std::nullopt;
        }
    }

    return std::nullopt;
}

Image Render(const std::filesystem::path& path, const CameraOptions& camera_options,
             const RenderOptions& render_options) {
    Scene scene = ReadScene(path);
    Screen screen = CreateScreen(camera_options);
    LookAtCamera camera(camera_options);

    for (size_t i = 0; i < screen.GetHeight(); ++i) {
        for (size_t j = 0; j < screen.GetWidth(); ++j) {
            Ray ray({0, 0, 0}, screen[i][j].GetCenterPosition());
            ray = camera.RayTransform(ray);
            auto color = ComputeColor(ray, scene, render_options);
            if (color.has_value()) {
                screen[i][j].SetColor(*color);
            }
        }
    }

    screen.PostProcessing(render_options.mode);
    Image image(screen.GetWidth(), screen.GetHeight());
    for (size_t i = 0; i < screen.GetHeight(); ++i) {
        for (size_t j = 0; j < screen.GetWidth(); ++j) {
            const auto& color = screen[i][j].GetColor();
            RGB rgb{static_cast<int>(color[0] * 255), static_cast<int>(color[1] * 255),
                    static_cast<int>(color[2] * 255)};
            image.SetPixel(rgb, i, j);
        }
    }

    return image;
}

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
#include <algorithm>

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

std::optional<Vector> ComputeObjectNormal(const Object& object, const Vector& position) {
    double eps = 1e-9;
    Vector normal1 = *object.GetNormal(0);
    Vector normal2 = *object.GetNormal(1);
    Vector normal3 = *object.GetNormal(2);

    if (Length(normal1) > eps && Length(normal2) > eps && Length(normal3) > eps) {
        Vector bar_coord = GetBarycentricCoords(object.polygon, position);
        Vector normal = normal1 * bar_coord[0] + normal2 * bar_coord[1] + normal2 * bar_coord[2];
        return normal;
    } else {
        return std::nullopt;
    }
}

Vector ComputeNormal(const Intersection& intersect, const Object* object = nullptr) {
    std::optional<Vector> normal = std::nullopt;
    if (object) {
        normal = ComputeObjectNormal(*object, intersect.GetPosition());
    }

    if (!normal.has_value()) {
        normal = intersect.GetNormal();
    }

    return *normal;
}

std::optional<Intersection> NearestIntersection(const Ray& ray, const Scene& scene,
                                                const SphereObject** nearest_sphere,
                                                const Object** nearest_object) {
    const auto& objects = scene.GetObjects();
    const auto& spheres = scene.GetSphereObjects();

    std::optional<Intersection> nearest_intersection = std::nullopt;
    *nearest_sphere = nullptr;
    *nearest_object = nullptr;

    for (const auto& sphere : spheres) {
        auto intersection = GetIntersection(ray, sphere.sphere);
        if (intersection.has_value() &&
            (!nearest_intersection.has_value() ||
             (intersection->GetDistance() < nearest_intersection->GetDistance()))) {
            nearest_intersection = *intersection;
            *nearest_sphere = &sphere;
        }
    }

    for (const auto& object : objects) {
        auto intersection = GetIntersection(ray, object.polygon);
        if (intersection.has_value() &&
            (!nearest_intersection.has_value() ||
             (intersection->GetDistance() < nearest_intersection->GetDistance()))) {
            nearest_intersection = *intersection;
            *nearest_object = &object;
        }
    }

    return nearest_intersection;
}

std::optional<std::array<double, 3>> ComputeColor(const Ray& ray, const Scene& scene,
                                                  const RenderOptions& render_options,
                                                  int cur_depth, int max_depth) {
    double eps = 1e-9;
    const auto& lights = scene.GetLights();
    const SphereObject* nearest_sphere;
    const Object* nearest_object;

    std::optional<Intersection> nearest_intersection =
        NearestIntersection(ray, scene, &nearest_sphere, &nearest_object);

    if (render_options.mode == RenderMode::kDepth) {
        if (nearest_intersection.has_value()) {
            double distance = nearest_intersection->GetDistance();
            return std::array<double, 3>{distance, distance, distance};
        } else {
            return std::nullopt;
        }
    } else if (render_options.mode == RenderMode::kNormal) {
        if (nearest_intersection.has_value()) {
            const Vector normal = ComputeNormal(*nearest_intersection, nearest_object);
            return std::array<double, 3>{normal[0], normal[1], normal[2]};
        } else {
            return std::nullopt;
        }
    } else if (render_options.mode == RenderMode::kFull) {
        Vector color;
        if (nearest_intersection.has_value()) {
            const Material* material;
            const Vector normal = ComputeNormal(*nearest_intersection, nearest_object);
            const Ray reflected_ray(nearest_intersection->GetPosition() + 1.5 * eps * normal,
                                    Reflect(ray.GetDirection(), normal));
            if (nearest_object) {
                material = nearest_object->material;
            } else {
                material = nearest_sphere->material;
            }

            color += material->ambient_color + material->intensity;

            for (const auto& light : lights) {
                const Ray light_ray =
                    Ray(light.position, nearest_intersection->GetPosition() - light.position);
                const SphereObject* dummy_sphere;
                const Object* dummy_object;
                auto light_intersection =
                    NearestIntersection(light_ray, scene, &dummy_sphere, &dummy_object);
                const Vector distance_vector =
                    light_intersection->GetPosition() - nearest_intersection->GetPosition();
                if (Length(distance_vector) < eps) {
                    Vector ld = std::max(DotProduct(-1 * light_ray.GetDirection(), normal), 0.0) *
                                light.intensity;
                    for (auto i = 0; i < 3; ++i) {
                        ld[i] *= material->diffuse_color[i];
                    }
                    color += material->albedo[0] * ld;

                    Vector ls = std::pow(std::max(DotProduct(-1 * light_ray.GetDirection(),
                                                             reflected_ray.GetDirection()),
                                                  0.0),
                                         material->specular_exponent) *
                                light.intensity;
                    for (auto i = 0; i < 3; ++i) {
                        ls[i] *= material->specular_color[i];
                    }
                    color += material->albedo[0] * ls;
                }
            }

            if (cur_depth < max_depth) {
                if (nearest_object ||
                    Length(reflected_ray.GetOrigin() - nearest_sphere->sphere.GetCenter()) >
                        nearest_sphere->sphere.GetRadius() + eps) {
                    const auto reflected_color = ComputeColor(reflected_ray, scene, render_options,
                                                              cur_depth + 1, max_depth);
                    if (reflected_color.has_value()) {
                        color += material->albedo[1] * Vector((*reflected_color)[0],
                                                              (*reflected_color)[1],
                                                              (*reflected_color)[2]);
                    }
                }

                std::optional<Vector> refract_direction;
                if (nearest_object == nullptr &&
                    Length(reflected_ray.GetOrigin() - nearest_sphere->sphere.GetCenter()) <
                        nearest_sphere->sphere.GetRadius() - eps) {
                    refract_direction =
                        Refract(ray.GetDirection(), normal, material->refraction_index);
                    if (refract_direction.has_value()) {
                        Ray refracted_ray(nearest_intersection->GetPosition() - 1.5 * eps * normal,
                                          *refract_direction);
                        const auto refracted_color = ComputeColor(
                            refracted_ray, scene, render_options, cur_depth + 1, max_depth);
                        if (refracted_color.has_value()) {
                            color += Vector((*refracted_color)[0], (*refracted_color)[1],
                                            (*refracted_color)[2]);
                        }
                    }
                } else {
                    refract_direction =
                        Refract(ray.GetDirection(), normal, 1 / material->refraction_index);
                    if (refract_direction.has_value()) {
                        Ray refracted_ray(nearest_intersection->GetPosition() - 1.5 * eps * normal,
                                          *refract_direction);
                        const auto refracted_color = ComputeColor(
                            refracted_ray, scene, render_options, cur_depth + 1, max_depth);
                        if (refracted_color.has_value()) {
                            color += material->albedo[2] * Vector((*refracted_color)[0],
                                                                  (*refracted_color)[1],
                                                                  (*refracted_color)[2]);
                        }
                    }
                }
            }
        }

        return std::array<double, 3>{color[0], color[1], color[2]};
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
            auto color = ComputeColor(ray, scene, render_options, 0, render_options.depth);
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

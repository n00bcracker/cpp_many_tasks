#pragma once

#include <material.h>
#include <vector.h>
#include <object.h>
#include <light.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <ranges>
#include <string_view>
#include <utility>
#include <tuple>
#include <optional>
#include <algorithm>
#include <cctype>
#include <regex>

class Scene {
public:
    void AddObject(Object&& object) {
        objects_.push_back(object);
    }

    void AddSphere(SphereObject&& sphere) {
        spheres_.push_back(sphere);
    }

    void AddLight(Light&& light) {
        lights_.push_back(light);
    }

    void SetMaterials(std::unordered_map<std::string, Material>&& materials) {
        materials_ = materials;
    }

    const std::vector<Object>& GetObjects() const {
        return objects_;
    }

    const std::vector<SphereObject>& GetSphereObjects() const {
        return spheres_;
    }

    const std::vector<Light>& GetLights() const {
        return lights_;
    }

    const std::unordered_map<std::string, Material>& GetMaterials() const {
        return materials_;
    }

private:
    std::vector<Object> objects_;
    std::vector<SphereObject> spheres_;
    std::vector<Light> lights_;
    std::unordered_map<std::string, Material> materials_;
};

std::vector<std::string> SplitString(const std::string& str, const std::string_view& delim = " ") {
    std::vector<std::string> res;
    const std::regex whitespace_pattern("\\s+");
    std::string trimmed = std::regex_replace(str, whitespace_pattern, " ");
    for (const auto word : std::views::split(trimmed, delim)) {
        if (word.begin() != word.end()) {
            res.emplace_back(word.begin(), word.end());
        }
    }

    return res;
}

Vector ReadVector(const std::vector<std::string>& params) {
    Vector res;
    for (size_t i = 0; i < 3; ++i) {
        res[i] = std::stod(params[i + 1]);
    }

    return res;
}

std::pair<int, std::optional<int>> ParseVertex(const std::string& vertex) {
    std::pair<int, std::optional<int>> res;
    for (size_t i = 0; const auto part : std::views::split(vertex, std::string_view("/"))) {
        std::string_view tmp(part.begin(), part.end());
        if (i == 0) {
            res.first = std::stoi(tmp.data());
        } else if (i == 2) {
            res.second = std::stoi(tmp.data());
        }
        ++i;
    }
    return res;
}

template <typename T>
const T& GetItem(const std::vector<T>& vector, int index) {
    if (index >= 0) {
        return vector[index - 1];
    } else {
        return vector[vector.size() + index];
    }
}

void FillPolygonVertex(const std::string& vertex, size_t vertex_idx,
                       const std::vector<Vector>& vertices,
                       const std::vector<Vector>& vertex_normals,
                       std::array<Vector, 3>& polygon_vertices,
                       std::array<Vector, 3>& polygon_normals) {
    auto [v_idx, vn_idx] = ParseVertex(vertex);
    polygon_vertices[vertex_idx] = GetItem(vertices, v_idx);
    if (vn_idx.has_value()) {
        polygon_normals[vertex_idx] = GetItem(vertex_normals, *vn_idx);
    } else {
        polygon_normals[vertex_idx] = Vector{};
    }
}

std::vector<Object> ReadFigure(const std::vector<std::string>& params,
                               const std::vector<Vector>& vertices,
                               const std::vector<Vector>& vertex_normals,
                               const Material* material) {
    std::vector<Object> res;
    std::array<Vector, 3> polygon_vertices;
    std::array<Vector, 3> polygon_normals;
    for (size_t i = 1; i < params.size(); ++i) {
        if (!params[i].empty()) {
            if (i == 1 || i == 2) {
                FillPolygonVertex(params[i], i - 1, vertices, vertex_normals, polygon_vertices,
                                  polygon_normals);
            } else {
                FillPolygonVertex(params[i], 2, vertices, vertex_normals, polygon_vertices,
                                  polygon_normals);
                res.emplace_back(
                    material,
                    Triangle(polygon_vertices[0], polygon_vertices[1], polygon_vertices[2]),
                    polygon_normals);
                polygon_vertices[1] = polygon_vertices[2];
                polygon_normals[1] = polygon_normals[2];
            }
        }
    }

    return res;
}

SphereObject ReadSphere(const std::vector<std::string>& params, const Material* material) {
    Vector center;
    for (size_t i = 0; i < 3; ++i) {
        center[i] = std::stod(params[i + 1]);
    }
    double r = std::stod(params[4]);

    return SphereObject{material, Sphere(center, r)};
}

Light ReadLight(const std::vector<std::string>& params) {
    Vector position;
    for (size_t i = 0; i < 3; ++i) {
        position[i] = std::stod(params[i + 1]);
    }

    Vector intensity;
    for (size_t i = 0; i < 3; ++i) {
        intensity[i] = std::stod(params[i + 4]);
    }
    return Light{position, intensity};
}

std::unordered_map<std::string, Material> ReadMaterials(const std::filesystem::path& path) {
    std::ifstream mtlfile(path);
    std::string buffer;
    std::unordered_map<std::string, Material> res;
    Material material;

    if (mtlfile.is_open()) {
        while (std::getline(mtlfile, buffer)) {
            std::vector<std::string> params = SplitString(buffer);

            if (!params.empty()) {
                const std::string& attr_name = params[0];
                if (attr_name == "newmtl") {
                    if (!material.name.empty()) {
                        res.emplace(material.name, material);
                    }

                    material = Material();
                    material.name = params[1];
                } else if (attr_name == "Ka") {
                    material.ambient_color = ReadVector(params);
                } else if (attr_name == "Kd") {
                    material.diffuse_color = ReadVector(params);
                } else if (attr_name == "Ks") {
                    material.specular_color = ReadVector(params);
                } else if (attr_name == "Ke") {
                    material.intensity = ReadVector(params);
                } else if (attr_name == "Ns") {
                    material.specular_exponent = std::stod(params[1]);
                } else if (attr_name == "Ni") {
                    material.refraction_index = std::stod(params[1]);
                } else if (attr_name == "al") {
                    material.albedo = ReadVector(params);
                }
            }
        }

        if (!material.name.empty()) {
            res.emplace(material.name, material);
        }

        mtlfile.close();
    }

    return res;
}

Scene ReadScene(const std::filesystem::path& path) {
    std::ifstream wavefile(path);
    std::string buffer;

    std::vector<Vector> vertices;
    std::vector<Vector> vertex_normals;
    const Material* current_material;
    Scene scena;

    if (wavefile.is_open()) {
        while (std::getline(wavefile, buffer)) {
            std::vector<std::string> params = SplitString(buffer);

            if (!params.empty()) {
                const std::string& object_type = params[0];
                if (object_type == "v") {
                    vertices.push_back(ReadVector(params));
                } else if (object_type == "vn") {
                    vertex_normals.push_back(ReadVector(params));
                } else if (object_type == "f") {
                    for (auto& object :
                         ReadFigure(params, vertices, vertex_normals, current_material)) {
                        scena.AddObject(std::move(object));
                    }
                } else if (object_type == "S") {
                    scena.AddSphere(ReadSphere(params, current_material));
                } else if (object_type == "P") {
                    scena.AddLight(ReadLight(params));
                } else if (object_type == "usemtl") {
                    current_material = &scena.GetMaterials().at(params[1]);
                } else if (object_type == "mtllib") {
                    std::filesystem::path materials_path = path;
                    materials_path.replace_filename(params[1]);
                    scena.SetMaterials(ReadMaterials(materials_path));
                }
            }
        }
        wavefile.close();
    }

    return scena;
}

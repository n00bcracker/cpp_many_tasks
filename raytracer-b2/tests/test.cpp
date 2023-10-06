#include <options/camera_options.h>
#include <options/render_options.h>
#include <tests/commons.h>
#include <raytracer.h>
#include <util.h>

#include <cmath>
#include <string_view>
#include <optional>
#include <filesystem>

#include <catch2/catch_test_macros.hpp>

void CheckImage(std::string_view obj_filename, std::string_view result_filename,
                const CameraOptions& camera_options, const RenderOptions& render_options,
                const std::optional<std::filesystem::path>& output_path = std::nullopt) {
    static const auto kTestsDir = GetFileDir(__FILE__);
    auto image = Render(kTestsDir / obj_filename, camera_options, render_options);
    if (output_path) {
        image.Write(*output_path);
    }
    Compare(image, Image{kTestsDir / result_filename});
}

TEST_CASE("Dragon", "[no_asan]") {
    CameraOptions camera_opts{.screen_width = 1024,
                              .screen_height = 768,
                              .fov = std::numbers::pi / 4,
                              .look_from = {-1.1, .6, -.5},
                              .look_to = {.20702, -.26424, .214467}};
    CheckImage("dragon/dragon.obj", "dragon/result.png", camera_opts, {8});
}

TEST_CASE("Bunny") {
    CameraOptions camera_opts{.screen_width = 1024,
                              .screen_height = 768,
                              .look_from = {-0.8, 1., 1.4},
                              .look_to = {-.2, .9, .0}};
    CheckImage("bunny/bunny.obj", "bunny/result.png", camera_opts, {1});
}

TEST_CASE("Figure", "[.][no_asan]") {
    CameraOptions camera_opts{.screen_width = 1920,
                              .screen_height = 1080,
                              .look_from = {0., 1., 1.3},
                              .look_to = {0., 1., -1.}};
    CheckImage("sculpture/Kayle_sculpt1.OBJ", "sculpture/result.png", camera_opts, {8});
}

#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <cmath>

#include <geometry.h>
#include <options/render_options.h>

class Pixel {
public:
    Pixel() : color_{0, 0, 0}, center_position_(0, 0, -1), is_colored_(false) {
    }

    void SetColor(const std::array<double, 3>& color) {
        color_ = color;
        is_colored_ = true;
    }

    const std::array<double, 3>& GetColor() const {
        return color_;
    }

    void SetCenterPosition(const Vector& position) {
        center_position_ = position;
    }

    const Vector& GetCenterPosition() const {
        return center_position_;
    }

    bool IsColored() const {
        return is_colored_;
    }

private:
    std::array<double, 3> color_;
    Vector center_position_;
    bool is_colored_;
};

class Screen {
public:
    Screen(size_t width, size_t height)
        : width_(width),
          height_(height),
          screen_(std::vector<std::vector<Pixel>>(height, std::vector<Pixel>(width))) {
    }

    size_t GetWidth() const {
        return width_;
    }

    size_t GetHeight() const {
        return height_;
    }

    std::vector<Pixel>& operator[](size_t i) {
        return screen_[i];
    }

    void PostProcessing(RenderMode render_mode) {
        double eps = 1e-9;
        if (render_mode == RenderMode::kDepth) {
            double max_dist = -1;
            for (size_t i = 0; i < height_; ++i) {
                for (size_t j = 0; j < width_; ++j) {
                    if (screen_[i][j].IsColored() && screen_[i][j].GetColor()[0] > max_dist) {
                        max_dist = screen_[i][j].GetColor()[0];
                    }
                }
            }

            for (size_t i = 0; i < height_; ++i) {
                for (size_t j = 0; j < width_; ++j) {
                    if (screen_[i][j].IsColored()) {
                        auto dist = screen_[i][j].GetColor()[0];
                        screen_[i][j].SetColor({dist / max_dist, dist / max_dist, dist / max_dist});
                    } else {
                        screen_[i][j].SetColor({1, 1, 1});
                    }
                }
            }
        } else if (render_mode == RenderMode::kNormal) {
            for (size_t i = 0; i < height_; ++i) {
                for (size_t j = 0; j < width_; ++j) {
                    if (screen_[i][j].IsColored()) {
                        auto normal = screen_[i][j].GetColor();
                        screen_[i][j].SetColor(
                            {normal[0] / 2 + 0.5, normal[1] / 2 + 0.5, normal[2] / 2 + 0.5});
                    } else {
                        screen_[i][j].SetColor({0, 0, 0});
                    }
                }
            }
        } else if (render_mode == RenderMode::kFull) {
            double max_intensity = -1;
            for (size_t i = 0; i < height_; ++i) {
                for (size_t j = 0; j < width_; ++j) {
                    if (screen_[i][j].IsColored()) {
                        const auto& color = screen_[i][j].GetColor();
                        max_intensity = std::max({max_intensity, color[0], color[1], color[2]});
                    }
                }
            }

            for (size_t i = 0; i < height_; ++i) {
                for (size_t j = 0; j < width_; ++j) {
                    if (screen_[i][j].IsColored()) {
                        const auto& color = screen_[i][j].GetColor();
                        std::array<double, 3> new_color;
                        for (auto i = 0; i < 3; ++i) {
                            new_color[i] = color[i] *
                                           (color[i] / std::pow(max_intensity + eps, 2) + 1) /
                                           (1 + color[i]);
                            new_color[i] = std::pow(new_color[i], 1.0 / 2.2);
                        }
                        screen_[i][j].SetColor(new_color);
                    }
                }
            }
        }
    }

private:
    size_t width_;
    size_t height_;
    std::vector<std::vector<Pixel>> screen_;
};

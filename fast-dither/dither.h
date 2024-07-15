#pragma once

#include <kd_tree.h>
#include <image.h>

#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

Point<3> RGBToDouble(const RGB& color) {
    Point<3> double_color;
    double_color[0] = color.r;
    double_color[1] = color.g;
    double_color[2] = color.b;

    return double_color;
}

std::vector<Points<3>> CopyImage(const Image& image) {
    size_t width = image.Width();
    size_t height = image.Height();

    std::vector<Points<3>> copy_image;

    for (size_t i = 0; i < height; ++i) {
        Points<3> image_row;
        for (size_t j = 0; j < width; ++j) {
            image_row.emplace_back(RGBToDouble(image.GetPixel(i, j)));
        }

        copy_image.emplace_back(std::move(image_row));
    }

    return copy_image;
}

Points<3> ConvertRGBPaletteToDouble(const std::vector<RGB>& colors) {
    Points<3> double_palette;
    for (const RGB& color : colors) {
        Point<3> double_color = RGBToDouble(color);
        double_palette.emplace_back(std::move(double_color));
    }

    return double_palette;
}

Point<3> ComputeError(const RGB& new_pixel, const Point<3>& old_pixel) {
    Point<3> error;
    error[0] = old_pixel[0] - static_cast<double>(new_pixel.r);
    error[1] = old_pixel[1] - static_cast<double>(new_pixel.g);
    error[2] = old_pixel[2] - static_cast<double>(new_pixel.b);

    return error;
}

Point<3> PointScalarMult(const Point<3>& point, double coeff) {
    Point<3> res;
    for (size_t i = 0; i < point.size(); ++i) {
        res[i] = point[i] * coeff;
    }

    return res;
}

Point<3> PointAdd(const Point<3>& point1, const Point<3>& point2) {
    Point<3> res;
    for (size_t i = 0; i < point1.size(); ++i) {
        res[i] = point1[i] + point2[i];
    }

    return res;
}

void FloydSteinbergDithering(std::vector<Points<3>>& image, size_t i, size_t j,
                             const Point<3>& error) {
    size_t height = image.size();
    size_t width = image[0].size();

    if (j + 1 < width) {
        image[i][j + 1] = PointAdd(PointScalarMult(error, 7.f / 16), image[i][j + 1]);
    }

    if (i + 1 < height && j > 0) {
        image[i + 1][j - 1] = PointAdd(PointScalarMult(error, 3.f / 16), image[i + 1][j - 1]);
    }

    if (i + 1 < height) {
        image[i + 1][j] = PointAdd(PointScalarMult(error, 5. / 16), image[i + 1][j]);
    }

    if (i + 1 < height && j + 1 < width) {
        image[i + 1][j + 1] = PointAdd(PointScalarMult(error, 1. / 16), image[i + 1][j + 1]);
    }
}

Image Dither(const Image& input, const std::vector<RGB>& colors) {
    std::vector<Points<3>> double_image = CopyImage(input);
    size_t width = input.Width();
    size_t height = input.Height();
    Image dithered_image_image = Image(width, height);

    KdTree<3> kd_tree(ConvertRGBPaletteToDouble(colors));

    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            const Point<3>& old_pixel = double_image[i][j];
            const RGB new_pixel = colors[kd_tree.GetNearest(old_pixel)];
            const Point<3> error = ComputeError(new_pixel, old_pixel);
            FloydSteinbergDithering(double_image, i, j, error);
            dithered_image_image.SetPixel(new_pixel, i, j);
        }
    }

    return dithered_image_image;
}

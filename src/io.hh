#pragma once

#include <filesystem>
#include <opencv2/core.hpp>
#include <vector>

namespace panorama
{

    namespace fs = std::filesystem;

    std::vector<cv::Mat> loadImages(const std::vector<fs::path>& paths);
    void saveImage(const fs::path& path, const cv::Mat& image);

} // namespace panorama

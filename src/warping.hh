#pragma once

#include <opencv2/core.hpp>
#include <vector>

namespace panorama
{

    struct WarpedImage
    {
        cv::Mat image;
        cv::Mat mask;
    };

    std::vector<WarpedImage>
    warpImages(const std::vector<cv::Mat>& images,
               const std::vector<cv::Mat>& homographies);

} // namespace panorama

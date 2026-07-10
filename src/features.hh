#pragma once

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

namespace panorama
{

    struct Features
    {
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
    };

    Features detectFeatures(const cv::Mat& image);

} // namespace panorama

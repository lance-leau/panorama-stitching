#pragma once

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

#include "features.hh"

namespace panorama
{
    struct HomographyResult
    {
        cv::Mat homography;
        std::vector<uint8_t> inliersMask;
    };

    HomographyResult estimateHomography(const Features& first,
                                        const Features& second,
                                        const std::vector<cv::DMatch>& matches);

} // namespace panorama

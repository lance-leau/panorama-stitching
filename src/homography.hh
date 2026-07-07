#pragma once

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

#include "features.hh"

namespace panorama
{

    cv::Mat estimateHomography(const Features& first, const Features& second,
                               const std::vector<cv::DMatch>& matches);

} // namespace panorama

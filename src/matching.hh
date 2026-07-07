#pragma once

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

#include "features.hh"

namespace panorama
{

    std::vector<cv::DMatch> matchFeatures(const Features& first,
                                          const Features& second);

} // namespace panorama

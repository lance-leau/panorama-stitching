#pragma once

#include <opencv2/core.hpp>
#include <vector>

#include "warping.hh"

namespace panorama
{

    cv::Mat blendImages(const std::vector<WarpedImage>& warpedImages);

} // namespace panorama

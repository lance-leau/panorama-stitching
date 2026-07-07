#pragma once

#include <filesystem>
#include <opencv2/core.hpp>
#include <vector>

#include "blend.hh"
#include "features.hh"
#include "homography.hh"
#include "io.hh"
#include "matching.hh"
#include "warping.hh"

namespace panorama
{
    namespace fs = std::filesystem;

    struct PipelineResult
    {
        cv::Mat panorama;
        int imagesCount = 0;
    };

    PipelineResult runPipeline(const std::vector<fs::path>& inputPaths,
                               const fs::path& outputPath);

} // namespace panorama

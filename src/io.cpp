#include "io.hh"

#include <stdexcept>

namespace panorama
{

    std::vector<cv::Mat> loadImages(const std::vector<fs::path>&)
    {
        throw std::logic_error("TODO");
    }

    void saveImage(const fs::path&, const cv::Mat&)
    {
        throw std::logic_error("TODO");
    }

} // namespace panorama

#include "warping.hh"

#include <stdexcept>

namespace panorama
{

    std::vector<WarpedImage> warpImages(const std::vector<cv::Mat>&,
                                        const std::vector<cv::Mat>&)
    {
        throw std::logic_error("TODO");
    }

} // namespace panorama

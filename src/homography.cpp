#include "homography.hh"

#include <stdexcept>

namespace panorama
{

    cv::Mat estimateHomography(const Features&, const Features&,
                               const std::vector<cv::DMatch>&)
    {
        throw std::logic_error("TODO");
    }

} // namespace panorama

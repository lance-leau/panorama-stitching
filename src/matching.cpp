#include "matching.hh"

#include <stdexcept>

namespace panorama
{

    std::vector<cv::DMatch> matchFeatures(const Features&, const Features&)
    {
        throw std::logic_error("TODO");
    }

} // namespace panorama

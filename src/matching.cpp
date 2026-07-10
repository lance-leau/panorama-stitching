#include "matching.hh"

#include <stdexcept>

namespace panorama
{

    std::vector<cv::DMatch> matchFeatures(const Features&, const Features&)
    {
        cv::BFMatcher matcher(cv::NORM_L2);

        std::vector<std::vector<cv::DMatch>> rawMatches;
        matcher.knnMatch(first.descriptors, second.descriptors, rawMatches, 2);

        std::vector<cv::DMatch> goodMatches;
        for (const auto& pair : rawMatches)
        {
            if (pair.size() < 2)
                continue;

            if (pair[0].distance < kRatioThreshold * pair[1].distance)
                goodMatches.push_back(pair[0]);
        }

        std::sort(goodMatches.begin(), goodMatches.end(),
                  [](const cv::DMatch& a, const cv::DMatch& b) { return a.distance < b.distance; });

        return goodMatches;
    }

} // namespace panorama

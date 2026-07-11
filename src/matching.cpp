#include "matching.hh"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace panorama
{

    constexpr float kRatioThreshold = 0.75f;

    float l2Distance(const float* a, const float* b, int dims)
    {
        float sum = 0.0f;
        for (int i = 0; i < dims; ++i)
        {
            const float diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }

    struct BestTwo
    {
        float bestDist = std::numeric_limits<float>::max();
        int bestIdx = -1;
        float secondDist = std::numeric_limits<float>::max();
    };

    BestTwo findTwoNearest(const float* query, const cv::Mat& targetDescriptors,
                           int dims)
    {
        BestTwo result;

        for (int j = 0; j < targetDescriptors.rows; ++j)
        {
            const float* candidate = targetDescriptors.ptr<float>(j);
            const float dist = l2Distance(query, candidate, dims);

            if (dist < result.bestDist)
            {
                result.secondDist = result.bestDist;
                result.bestDist = dist;
                result.bestIdx = j;
            }
            else if (dist < result.secondDist)
            {
                result.secondDist = dist;
            }
        }

        return result;
    }

    std::vector<cv::DMatch> matchFeatures(const Features& first,
                                          const Features& second)
    {
        if (first.descriptors.empty() || second.descriptors.empty())
            throw std::runtime_error("matchFeatures: descripteurs vides");

        const int dims = first.descriptors.cols;
        if (second.descriptors.cols != dims)
            throw std::runtime_error(
                "matchFeatures: dimensions de descripteurs incoherentes");

        std::vector<cv::DMatch> goodMatches;

        for (int i = 0; i < first.descriptors.rows; ++i)
        {
            const float* query = first.descriptors.ptr<float>(i);
            BestTwo nearest = findTwoNearest(query, second.descriptors, dims);

            if (nearest.bestIdx < 0)
                continue;

            if (nearest.bestDist < kRatioThreshold * nearest.secondDist)
                goodMatches.emplace_back(i, nearest.bestIdx, nearest.bestDist);
        }

        std::sort(goodMatches.begin(), goodMatches.end(),
                  [](const cv::DMatch& a, const cv::DMatch& b) {
                      return a.distance < b.distance;
                  });

        return goodMatches;
    }

} // namespace panorama
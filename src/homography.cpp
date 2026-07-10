#include "homography.hh"

#include <opencv2/calib3d.hpp>
#include <stdexcept>

#define kMinMatches 4
#define kRansacThreshold 4.0

namespace panorama
{

    cv::Mat estimateHomography(const Features& first, const Features& second,
                               const std::vector<cv::DMatch>& matches)
    {
        if (matches.size() < kMinMatches)
        {
            std::cout << "estimateHomography: pas assez de correspondances ("
                      << matches.size() << " < " << kMinMatches << ")\n";
            return cv::Mat();
        }

        std::vector<cv::Point2f> pointsFirst;
        std::vector<cv::Point2f> pointsSecond;
        pointsFirst.reserve(matches.size());
        pointsSecond.reserve(matches.size());

        for (const auto& match : matches)
        {
            pointsFirst.push_back(first.keypoints[match.queryIdx].pt);
            pointsSecond.push_back(second.keypoints[match.trainIdx].pt);
        }

        cv::Mat homography = cv::findHomography(pointsSecond, pointsFirst,
                                                cv::RANSAC, kRansacThreshold);
        if (homography.empty())
            throw std::runtime_error("estimateHomography: estimation echouee");

        return homography;
    }

} // namespace panorama
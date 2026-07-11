#include "blend.hh"

#include <opencv2/imgproc.hpp>
#include <stdexcept>

namespace panorama
{

    namespace
    {
        cv::Mat distanceWeight(const cv::Mat& mask)
        {
            cv::Mat weight;
            cv::distanceTransform(mask, weight, cv::DIST_L2, 5);

            double maxVal = 0.0;
            cv::minMaxLoc(weight, nullptr, &maxVal);
            if (maxVal > 0.0)
                weight /= maxVal;

            return weight;
        }
    } // namespace

    cv::Mat blendImages(const std::vector<WarpedImage>& warpedImages)
    {
        if (warpedImages.empty())
            throw std::runtime_error("blendImages: aucune image fournie");

        const cv::Size canvasSize = warpedImages.front().image.size();

        cv::Mat accumulator = cv::Mat::zeros(canvasSize, CV_32FC3);
        cv::Mat weightSum = cv::Mat::zeros(canvasSize, CV_32FC1);

        for (const auto& warped : warpedImages)
        {
            if (warped.image.size() != canvasSize)
                throw std::runtime_error(
                    "blendImages: tailles d'images incoherentes");

            cv::Mat weight = distanceWeight(warped.mask);

            cv::Mat imageFloat;
            warped.image.convertTo(imageFloat, CV_32FC3);

            std::vector<cv::Mat> weightChannels(3, weight);
            cv::Mat weight3;
            cv::merge(weightChannels, weight3);

            accumulator += imageFloat.mul(weight3);
            weightSum += weight;
        }

        cv::Mat safeWeightSum;
        cv::max(weightSum, 1e-6f, safeWeightSum);

        std::vector<cv::Mat> safeWeightChannels(3, safeWeightSum);
        cv::Mat safeWeightSum3;
        cv::merge(safeWeightChannels, safeWeightSum3);

        cv::Mat blended = accumulator / safeWeightSum3;

        cv::Mat result;
        blended.convertTo(result, CV_8UC3);
        return result;
    }

} // namespace panorama
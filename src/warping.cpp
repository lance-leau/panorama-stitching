#include "warping.hh"

#include <algorithm>
#include <limits>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <stdexcept>

namespace panorama
{

    std::vector<cv::Point2f> _imageCorners(const cv::Mat& image)
    {
        const float w = static_cast<float>(image.cols);
        const float h = static_cast<float>(image.rows);
        return { { 0.f, 0.f }, { w, 0.f }, { w, h }, { 0.f, h } };
    }

    std::vector<WarpedImage>
    warpImages(const std::vector<cv::Mat>& images,
               const std::vector<cv::Mat>& homographies)
    {
        std::vector<std::vector<cv::Point2f>> allCorners;
        allCorners.reserve(images.size());
        for (size_t i = 0; i < images.size(); ++i)
        {
            std::vector<cv::Point2f> corners;
            cv::perspectiveTransform(_imageCorners(images[i]), corners,
                                     homographies[i]);
            allCorners.push_back(std::move(corners));
        }

        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float maxY = std::numeric_limits<float>::lowest();
        for (const auto& corners : allCorners)
            for (const auto& pt : corners)
            {
                minX = std::min(minX, pt.x);
                minY = std::min(minY, pt.y);
                maxX = std::max(maxX, pt.x);
                maxY = std::max(maxY, pt.y);
            }

        const int width = static_cast<int>(std::ceil(maxX - minX));
        const int height = static_cast<int>(std::ceil(maxY - minY));

        cv::Mat translation =
            (cv::Mat_<double>(3, 3) << 1, 0, -minX, 0, 1, -minY, 0, 0, 1);

        std::vector<WarpedImage> warped;
        warped.reserve(images.size());
        for (size_t i = 0; i < images.size(); ++i)
        {
            cv::Mat fullHomography = translation * homographies[i];

            WarpedImage result;
            cv::warpPerspective(images[i], result.image, fullHomography,
                                cv::Size(width, height));

            cv::Mat sourceMask(images[i].size(), CV_8UC1, cv::Scalar(255));
            cv::warpPerspective(sourceMask, result.mask, fullHomography,
                                cv::Size(width, height));

            warped.push_back(std::move(result));
        }

        return warped;
    }

} // namespace panorama
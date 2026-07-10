#include "features.hh"

#include <opencv2/imgproc.hpp>
#include <stdexcept>

namespace panorama
{

    Features detectFeatures(const cv::Mat& image)
    {
        if (image.empty())
        {
            std::cout << "detectFeatures() : image is empty\n";
            return Features();
        }

        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        static const auto detector = cv::SIFT::create(3000);

        Features features;
        detector->detectAndCompute(gray, cv::noArray(), features.keypoints,
                                   features.descriptors);
        return features;
    }

} // namespace panorama
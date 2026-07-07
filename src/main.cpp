#include <iostream>

#include "features.hh"
#include "io.hh"

int main()
{
    const std::filesystem::path imagePath = "/image01.jpg";

    auto images = panorama::loadImages({ imagePath });
    const cv::Mat& image = images.front();
    std::cout << "image chargee: " << image.cols << "x" << image.rows << "\n";

    auto features = panorama::detectFeatures(image);
    std::cout << "POIs detected: " << features.keypoints.size() << "\n";

    return 0;
}
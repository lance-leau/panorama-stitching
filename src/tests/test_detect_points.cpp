#include "test_framework.hpp"
#include "../features.hh"
#include <opencv2/opencv.hpp>

static cv::Mat make_checkerboard(int size = 300, int cell = 20) {
    cv::Mat img(size, size, CV_8UC3);
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x) {
            bool w = ((x / cell) + (y / cell)) % 2 == 0;
            img.at<cv::Vec3b>(y, x) = w ? cv::Vec3b(255,255,255) : cv::Vec3b(0,0,0);
        }
    return img;
}

TEST(detectFeatures_retourne_keypoints_sur_image_texturee) {
    cv::Mat img = make_checkerboard();
    auto f = panorama::detectFeatures(img);
    ASSERT_FALSE(f.keypoints.empty());
    ASSERT_FALSE(f.descriptors.empty());
}

TEST(detectFeatures_descripteurs_dimension_128) {
    cv::Mat img = make_checkerboard();
    auto f = panorama::detectFeatures(img);
    // SIFT produit des descripteurs de 128 dimensions
    ASSERT_EQ(f.descriptors.cols, 128);
}

TEST(detectFeatures_autant_de_descripteurs_que_de_keypoints) {
    cv::Mat img = make_checkerboard();
    auto f = panorama::detectFeatures(img);
    ASSERT_EQ((int)f.keypoints.size(), f.descriptors.rows);
}

TEST(detectFeatures_image_uniforme_peu_de_points) {
    cv::Mat img(200, 200, CV_8UC3, cv::Scalar(128, 128, 128));
    auto f = panorama::detectFeatures(img);
    ASSERT_TRUE((int)f.keypoints.size() < 10);
}

TEST(detectFeatures_keypoints_dans_les_bornes_image) {
    cv::Mat img = make_checkerboard(300);
    auto f = panorama::detectFeatures(img);
    for (const auto& kp : f.keypoints) {
        ASSERT_TRUE(kp.pt.x >= 0 && kp.pt.x < img.cols);
        ASSERT_TRUE(kp.pt.y >= 0 && kp.pt.y < img.rows);
    }
}

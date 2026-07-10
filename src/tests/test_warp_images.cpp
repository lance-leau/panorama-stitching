#include "test_framework.hpp"
#include "../warping.hh"
#include <opencv2/opencv.hpp>

static cv::Mat solid(int w, int h, cv::Scalar color = {100, 150, 200}) {
    return cv::Mat(h, w, CV_8UC3, color);
}

static cv::Mat translation_H(double tx, double ty) {
    return (cv::Mat_<double>(3,3) << 1,0,tx, 0,1,ty, 0,0,1);
}

TEST(warpImages_retourne_autant_dimages_que_dentrees) {
    std::vector<cv::Mat> images = {solid(150, 100), solid(150, 100), solid(150, 100)};
    std::vector<cv::Mat> Hs = {
        cv::Mat::eye(3, 3, CV_64F),
        translation_H(120, 0),
        translation_H(240, 0),
    };
    auto warped = panorama::warpImages(images, Hs);
    ASSERT_EQ((int)warped.size(), 3);
}

TEST(warpImages_toutes_les_images_ont_le_meme_canvas) {
    std::vector<cv::Mat> images = {solid(150, 100), solid(150, 100), solid(150, 100)};
    std::vector<cv::Mat> Hs = {
        cv::Mat::eye(3, 3, CV_64F),
        translation_H(120, 0),
        translation_H(240, 0),
    };
    auto warped = panorama::warpImages(images, Hs);
    for (size_t i = 1; i < warped.size(); ++i)
        ASSERT_EQ(warped[i].image.size(), warped[0].image.size());
}

TEST(warpImages_masques_non_vides) {
    std::vector<cv::Mat> images = {solid(100, 80), solid(100, 80)};
    std::vector<cv::Mat> Hs = {
        cv::Mat::eye(3, 3, CV_64F),
        translation_H(50, 0),
    };
    auto warped = panorama::warpImages(images, Hs);
    for (auto& wi : warped)
        ASSERT_TRUE(cv::countNonZero(wi.mask) > 0);
}

TEST(warpImages_canvas_assez_grand_pour_translation) {
    std::vector<cv::Mat> images = {solid(200, 100), solid(200, 100)};
    std::vector<cv::Mat> Hs = {
        cv::Mat::eye(3, 3, CV_64F),
        translation_H(180, 0),
    };
    auto warped = panorama::warpImages(images, Hs);
    // Canvas doit contenir les deux images décalées de 180 px
    ASSERT_TRUE(warped[0].image.cols >= 380);
}

TEST(warpImages_identite_preserve_pixels) {
    cv::Mat img(100, 100, CV_8UC3, cv::Scalar(42, 84, 126));
    std::vector<cv::Mat> images = {img};
    std::vector<cv::Mat> Hs = {cv::Mat::eye(3, 3, CV_64F)};
    auto warped = panorama::warpImages(images, Hs);

    ASSERT_FALSE(warped[0].image.empty());
    cv::Vec3b px = warped[0].image.at<cv::Vec3b>(50, 50);
    ASSERT_EQ((int)px[0], 42);
    ASSERT_EQ((int)px[1], 84);
    ASSERT_EQ((int)px[2], 126);
}

TEST(warpImages_masque_et_image_meme_taille) {
    std::vector<cv::Mat> images = {solid(120, 90), solid(120, 90)};
    std::vector<cv::Mat> Hs = {cv::Mat::eye(3,3,CV_64F), translation_H(60,0)};
    auto warped = panorama::warpImages(images, Hs);
    for (auto& wi : warped)
        ASSERT_EQ(wi.image.size(), wi.mask.size());
}

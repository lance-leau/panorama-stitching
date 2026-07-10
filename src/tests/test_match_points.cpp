#include "test_framework.hpp"
#include "../features.hh"
#include "../matching.hh"
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

TEST(matchFeatures_meme_image_donne_beaucoup_de_matches) {
    cv::Mat img = make_checkerboard();
    auto f = panorama::detectFeatures(img);
    auto matches = panorama::matchFeatures(f, f);
    ASSERT_GE((int)matches.size(), 10);
}

TEST(matchFeatures_images_differentes_moins_de_matches) {
    cv::Mat img1 = make_checkerboard(300, 20);
    cv::Mat img2 = make_checkerboard(300, 40);
    auto f1 = panorama::detectFeatures(img1);
    auto f2 = panorama::detectFeatures(img2);
    auto matches_same = panorama::matchFeatures(f1, f1);
    auto matches_diff = panorama::matchFeatures(f1, f2);
    ASSERT_TRUE((int)matches_diff.size() <= (int)matches_same.size());
}

TEST(matchFeatures_tries_par_distance_croissante) {
    cv::Mat img = make_checkerboard();
    auto f = panorama::detectFeatures(img);
    auto matches = panorama::matchFeatures(f, f);
    for (size_t i = 1; i < matches.size(); ++i)
        ASSERT_TRUE(matches[i].distance >= matches[i-1].distance);
}

TEST(matchFeatures_indices_valides) {
    cv::Mat img = make_checkerboard();
    auto f = panorama::detectFeatures(img);
    auto matches = panorama::matchFeatures(f, f);
    int n = static_cast<int>(f.keypoints.size());
    for (const auto& m : matches) {
        ASSERT_TRUE(m.queryIdx >= 0 && m.queryIdx < n);
        ASSERT_TRUE(m.trainIdx >= 0 && m.trainIdx < n);
    }
}

TEST(matchFeatures_retourne_vecteur_non_vide_sur_images_similaires) {
    cv::Mat img = make_checkerboard(400, 15);
    auto f = panorama::detectFeatures(img);
    auto matches = panorama::matchFeatures(f, f);
    ASSERT_FALSE(matches.empty());
}

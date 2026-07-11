#include "test_framework.hpp"
#include "../features.hh"
#include "../matching.hh"
#include "../homography.hh"
#include <opencv2/opencv.hpp>
#include <cmath>

static void make_translation_features(double tx, double ty,
                                      panorama::Features& fa,
                                      panorama::Features& fb,
                                      std::vector<cv::DMatch>& matches) {
    fa.keypoints.clear(); fb.keypoints.clear(); matches.clear();
    int N = 20;
    fa.descriptors = cv::Mat(N, 128, CV_32F);
    fb.descriptors = cv::Mat(N, 128, CV_32F);
    cv::randn(fa.descriptors, 0, 1);
    fb.descriptors = fa.descriptors.clone();

    for (int i = 0; i < N; ++i) {
        float x = 50.f + i * 20.f;
        float y = 100.f + i * 10.f;
        fa.keypoints.push_back(cv::KeyPoint(x, y, 1.f));
        fb.keypoints.push_back(cv::KeyPoint(x + (float)tx, y + (float)ty, 1.f));
        cv::DMatch m;
        m.trainIdx = i; m.queryIdx = i; m.distance = 0.f;
        matches.push_back(m);
    }
}


TEST(estimateHomography_type_float64) {
    panorama::Features fa, fb;
    std::vector<cv::DMatch> matches;
    make_translation_features(10.0, 5.0, fa, fb, matches);

    auto result = panorama::estimateHomography(fa, fb, matches);
    cv::Mat H = result.homography;
    ASSERT_EQ(H.type(), CV_64F);
}

TEST(estimateHomography_rmse_faible_sur_correspondances_parfaites) {
    panorama::Features fa, fb;
    std::vector<cv::DMatch> matches;
    make_translation_features(80.0, 40.0, fa, fb, matches);

    auto result = panorama::estimateHomography(fa, fb, matches);
    cv::Mat H = result.homography;

    std::vector<cv::Point2f> src, dst;
    for (const auto& m : matches) {
        src.push_back(fb.keypoints[m.queryIdx].pt);
        dst.push_back(fa.keypoints[m.trainIdx].pt);
    }
    std::vector<cv::Point2f> proj;
    cv::perspectiveTransform(src, proj, H);

    double rmse = 0;
    for (size_t i = 0; i < proj.size(); ++i) {
        double dx = proj[i].x - dst[i].x;
        double dy = proj[i].y - dst[i].y;
        rmse += dx*dx + dy*dy;
    }
    rmse = std::sqrt(rmse / proj.size());
    ASSERT_NEAR(rmse, 0.0, 1.0);
}

TEST(estimateHomography_moins_de_4_matches_leve_exception) {
    panorama::Features fa, fb;
    std::vector<cv::DMatch> matches;
    make_translation_features(10.0, 5.0, fa, fb, matches);
    matches.resize(3); 

    ASSERT_THROWS(panorama::estimateHomography(fa, fb, matches));
}

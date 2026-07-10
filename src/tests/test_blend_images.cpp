#include "test_framework.hpp"
#include "../blend.hh"
#include "../warping.hh"
#include <opencv2/opencv.hpp>

static panorama::WarpedImage make_warped(int w, int h, cv::Scalar color) {
    panorama::WarpedImage wi;
    wi.image = cv::Mat(h, w, CV_8UC3, color);
    wi.mask  = cv::Mat(h, w, CV_8U,   cv::Scalar(255));
    return wi;
}

TEST(blendImages_taille_identique_aux_entrees) {
    std::vector<panorama::WarpedImage> warped = {
        make_warped(200, 100, {100, 0, 0}),
        make_warped(200, 100, {0, 100, 0}),
    };
    cv::Mat result = panorama::blendImages(warped);
    ASSERT_EQ(result.rows, 100);
    ASSERT_EQ(result.cols, 200);
    ASSERT_EQ(result.channels(), 3);
}

TEST(blendImages_type_uint8) {
    std::vector<panorama::WarpedImage> warped = {
        make_warped(50, 50, {200, 100, 50}),
        make_warped(50, 50, {50, 100, 200}),
    };
    cv::Mat result = panorama::blendImages(warped);
    ASSERT_EQ(result.type(), CV_8UC3);
}

TEST(blendImages_pas_de_valeur_hors_borne) {
    std::vector<panorama::WarpedImage> warped = {
        make_warped(80, 80, {255, 255, 255}),
        make_warped(80, 80, {255, 255, 255}),
    };
    cv::Mat result = panorama::blendImages(warped);
    double mn, mx;
    cv::minMaxLoc(result, &mn, &mx);
    ASSERT_TRUE(mn >= 0 && mx <= 255);
}

TEST(blendImages_image_seule_retourne_cette_image) {
    panorama::WarpedImage wi;
    wi.image = cv::Mat(60, 60, CV_8UC3, cv::Scalar(123, 45, 67));
    wi.mask  = cv::Mat(60, 60, CV_8U,   cv::Scalar(255));

    cv::Mat result = panorama::blendImages({wi});
    cv::Vec3b px = result.at<cv::Vec3b>(30, 30);
    ASSERT_NEAR((int)px[0], 123, 5);
    ASSERT_NEAR((int)px[1],  45, 5);
    ASSERT_NEAR((int)px[2],  67, 5);
}

TEST(blendImages_trois_images_canvas_preservé) {
    std::vector<panorama::WarpedImage> warped = {
        make_warped(300, 100, {80,  120, 160}),
        make_warped(300, 100, {160, 80,  120}),
        make_warped(300, 100, {120, 160, 80}),
    };
    cv::Mat result = panorama::blendImages(warped);
    ASSERT_EQ(result.rows, 100);
    ASSERT_EQ(result.cols, 300);
}

TEST(blendImages_masque_vide_ignore_limage) {
    panorama::WarpedImage wi_valid = make_warped(50, 50, {200, 50, 50});
    panorama::WarpedImage wi_empty;
    wi_empty.image = cv::Mat(50, 50, CV_8UC3, cv::Scalar(0, 200, 0));
    wi_empty.mask  = cv::Mat(50, 50, CV_8U,   cv::Scalar(0)); // masque vide

    cv::Mat result = panorama::blendImages({wi_valid, wi_empty});
    cv::Vec3b px = result.at<cv::Vec3b>(25, 25);
    // Le pixel doit venir de wi_valid, pas wi_empty
    ASSERT_NEAR((int)px[0], 200, 10);
}

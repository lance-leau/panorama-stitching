#include "test_framework.hpp"
#include "../pipeline.hh"
#include "../io.hh"
#include <opencv2/opencv.hpp>
#include <filesystem>

namespace fs = std::filesystem;

static cv::Mat make_checkerboard(int w, int h, int cell = 20) {
    cv::Mat img(h, w, CV_8UC3);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            bool white = ((x / cell) + (y / cell)) % 2 == 0;
            img.at<cv::Vec3b>(y, x) = white ? cv::Vec3b(230,230,230) : cv::Vec3b(25,25,25);
        }
    return img;
}

static fs::path save_tmp(const std::string& name, const cv::Mat& img) {
    fs::path p = fs::temp_directory_path() / name;
    cv::imwrite(p.string(), img);
    return p;
}


TEST(runPipeline_trois_images_fonctionne) {
    cv::Mat base = make_checkerboard(700, 300);
    auto p1 = save_tmp("tri1.jpg", base(cv::Rect(0,   0, 300, 300)).clone());
    auto p2 = save_tmp("tri2.jpg", base(cv::Rect(200, 0, 300, 300)).clone());
    auto p3 = save_tmp("tri3.jpg", base(cv::Rect(400, 0, 300, 300)).clone());
    fs::path out = fs::temp_directory_path() / "tri_out.jpg";

    auto result = panorama::runPipeline({p1, p2, p3}, out);
    ASSERT_FALSE(result.panorama.empty());
    ASSERT_EQ(result.imagesCount, 3);
}

TEST(runPipeline_une_seule_image_leve_exception) {
    auto p1 = save_tmp("solo.jpg", make_checkerboard(300, 200));
    fs::path out = fs::temp_directory_path() / "solo_out.jpg";
    ASSERT_THROWS(panorama::runPipeline({p1}, out));
}

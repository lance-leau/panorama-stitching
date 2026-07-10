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

TEST(runPipeline_deux_images_produit_un_fichier_de_sortie) {
    cv::Mat base = make_checkerboard(500, 300);
    auto p1 = save_tmp("pipe_img1.jpg", base(cv::Rect(0,   0, 350, 300)).clone());
    auto p2 = save_tmp("pipe_img2.jpg", base(cv::Rect(150, 0, 350, 300)).clone());
    fs::path out = fs::temp_directory_path() / "pipe_out.jpg";
    if (fs::exists(out)) fs::remove(out);

    panorama::runPipeline({p1, p2}, out);
    ASSERT_TRUE(fs::exists(out));
    ASSERT_TRUE(fs::file_size(out) > 0);
}

TEST(runPipeline_panorama_plus_large_quune_image) {
    cv::Mat base = make_checkerboard(600, 300);
    auto p1 = save_tmp("wide1.jpg", base(cv::Rect(0,   0, 350, 300)).clone());
    auto p2 = save_tmp("wide2.jpg", base(cv::Rect(250, 0, 350, 300)).clone());
    fs::path out = fs::temp_directory_path() / "wide_out.jpg";

    auto result = panorama::runPipeline({p1, p2}, out);
    ASSERT_FALSE(result.panorama.empty());
    ASSERT_TRUE(result.panorama.cols > 350);
}

TEST(runPipeline_compte_les_images_traitees) {
    cv::Mat base = make_checkerboard(500, 300);
    auto p1 = save_tmp("cnt1.jpg", base(cv::Rect(0,   0, 300, 300)).clone());
    auto p2 = save_tmp("cnt2.jpg", base(cv::Rect(100, 0, 300, 300)).clone());
    fs::path out = fs::temp_directory_path() / "cnt_out.jpg";

    auto result = panorama::runPipeline({p1, p2}, out);
    ASSERT_EQ(result.imagesCount, 2);
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

TEST(runPipeline_sortie_est_une_image_couleur) {
    cv::Mat base = make_checkerboard(500, 300);
    auto p1 = save_tmp("col1.jpg", base(cv::Rect(0,   0, 300, 300)).clone());
    auto p2 = save_tmp("col2.jpg", base(cv::Rect(100, 0, 300, 300)).clone());
    fs::path out = fs::temp_directory_path() / "col_out.jpg";

    auto result = panorama::runPipeline({p1, p2}, out);
    ASSERT_EQ(result.panorama.channels(), 3);
    ASSERT_EQ(result.panorama.type(), CV_8UC3);
}

#include "test_framework.hpp"
#include "../io.hh"
#include <opencv2/opencv.hpp>
#include <filesystem>

namespace fs = std::filesystem;

static fs::path make_tmp_image(const std::string& name, int w = 200, int h = 150) {
    fs::path p = fs::temp_directory_path() / name;
    cv::Mat img(h, w, CV_8UC3, cv::Scalar(100, 150, 200));
    cv::imwrite(p.string(), img);
    return p;
}

TEST(loadImages_image_valide) {
    auto p = make_tmp_image("test_load.jpg");
    auto imgs = panorama::loadImages({p});
    ASSERT_EQ((int)imgs.size(), 1);
    ASSERT_FALSE(imgs[0].empty());
    ASSERT_EQ(imgs[0].channels(), 3);
}

TEST(loadImages_charge_plusieurs_images) {
    auto p1 = make_tmp_image("batch1.jpg", 100, 80);
    auto p2 = make_tmp_image("batch2.jpg", 120, 90);
    auto imgs = panorama::loadImages({p1, p2});
    ASSERT_EQ((int)imgs.size(), 2);
    ASSERT_FALSE(imgs[0].empty());
    ASSERT_FALSE(imgs[1].empty());
}

TEST(loadImages_liste_vide_retourne_vide) {
    auto imgs = panorama::loadImages({});
    ASSERT_EQ((int)imgs.size(), 0);
}

TEST(saveImage_cree_le_fichier) {
    fs::path out = fs::temp_directory_path() / "test_save_out.jpg";
    if (fs::exists(out)) fs::remove(out);
    cv::Mat img(50, 50, CV_8UC3, cv::Scalar(42, 42, 42));
    panorama::saveImage(out, img);
    ASSERT_TRUE(fs::exists(out));
    ASSERT_TRUE(fs::file_size(out) > 0);
}

TEST(saveImage_cree_dossiers_parents) {
    fs::path out = fs::temp_directory_path() / "sous_dossier_test_xyz" / "panorama.jpg";
    if (fs::exists(out.parent_path())) fs::remove_all(out.parent_path());
    cv::Mat img(50, 50, CV_8UC3, cv::Scalar(10, 20, 30));
    panorama::saveImage(out, img);
    ASSERT_TRUE(fs::exists(out));
}

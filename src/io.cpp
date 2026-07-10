#include "io.hh"

namespace panorama
{

    std::vector<cv::Mat> loadImages(const std::vector<fs::path>& paths)
    {
        std::vector<cv::Mat> images;
        images.reserve(paths.size());
        for (const auto& path : paths)
        {
            cv::Mat image = cv::imread(path.string(), cv::IMREAD_COLOR);
            if (image.empty())
            {
                std::cout << "img was not found : " << path.string() << "\n";
                return std::vector<cv::Mat>();
            }
            images.push_back(std::move(image));
        }
        return images;
    }

    void saveImage(const fs::path& path, const cv::Mat& image)
    {
        std::error_code ec;
        fs::create_directories(path.parent_path(), ec);
        if (ec)
        {
            std::cout << "saveImage() : could not create directory : "
                    + path.parent_path().string() + ", error :" + ec.message()
                    + "\n";
            return;
        }
        if (!cv::imwrite(path.string(), image))
        {
            std::cout << "could not save image : " << path.string() << "\n";
            return;
        }
    }

} // namespace panorama

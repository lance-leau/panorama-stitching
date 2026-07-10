#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "pipeline.hh"

namespace
{
    namespace fs = std::filesystem;

    fs::path nextOutputPath()
    {
        const fs::path outputDir = "outputs";
        fs::create_directories(outputDir);

        int existing = 0;
        for (const auto& entry : fs::directory_iterator(outputDir))
            if (entry.path().extension() == ".jpg"
                && entry.path().filename().string().rfind("panorama", 0) == 0)
                ++existing;

        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "panorama%02d.jpg", existing + 1);
        return outputDir / buffer;
    }

    std::vector<fs::path> collectPaths(const std::vector<std::string>& inputs)
    {
        static const std::vector<std::string> extensions = { ".jpg", ".jpeg",
                                                             ".png", ".bmp" };
        std::vector<fs::path> paths;

        for (const auto& item : inputs)
        {
            fs::path path(item);
            if (fs::is_directory(path))
            {
                for (const auto& entry : fs::directory_iterator(path))
                {
                    if (!entry.is_regular_file())
                        continue;
                    std::string ext = entry.path().extension().string();
                    std::transform(
                        ext.begin(), ext.end(), ext.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    if (std::find(extensions.begin(), extensions.end(), ext)
                        != extensions.end())
                        paths.push_back(entry.path());
                }
            }
            else
            {
                paths.push_back(path);
            }
        }

        std::sort(paths.begin(), paths.end());
        return paths;
    }
} // namespace

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " <images ou dossier>\n";
        return 1;
    }

    std::vector<std::string> inputs(argv + 1, argv + argc);

    try
    {
        auto paths = collectPaths(inputs);
        auto output = nextOutputPath();

        panorama::PipelineResult result = panorama::runPipeline(paths, output);

        std::cout << "panorama sauvegarde: " << output.string() << "\n";
        std::cout << "taille: " << result.panorama.cols << "x"
                  << result.panorama.rows << "\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "erreur: " << e.what() << "\n";
        return 1;
    }
}
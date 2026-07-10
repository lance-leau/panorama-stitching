#include "pipeline.hh"

#include <opencv2/calib3d.hpp>
#include <stdexcept>

namespace panorama
{

    namespace
    {
        std::vector<cv::Mat>
        buildAbsoluteHomographies(const std::vector<cv::Mat>& pairwiseH,
                                  int ref)
        {
            int n = static_cast<int>(pairwiseH.size()) + 1;
            std::vector<cv::Mat> absH(n);
            absH[ref] = cv::Mat::eye(3, 3, CV_64F);

            for (int i = ref - 1; i >= 0; --i)
                absH[i] = absH[i + 1] * pairwiseH[i].inv();

            for (int i = ref + 1; i < n; ++i)
                absH[i] = absH[i - 1] * pairwiseH[i - 1];

            return absH;
        }
    } // namespace

    PipelineResult runPipeline(const std::vector<fs::path>& inputPaths,
                               const fs::path& outputPath)
    {
        std::vector<cv::Mat> images = loadImages(inputPaths);
        const int n = static_cast<int>(images.size());
        if (n < 2)
            throw std::runtime_error(
                "runPipeline: il faut au moins deux images");

        // features de chaque image, calculees une seule fois
        std::vector<Features> features;
        features.reserve(n);
        for (const auto& img : images)
            features.push_back(detectFeatures(img));

        cv::Mat panorama;

        if (n == 2)
        {
            auto matches = matchFeatures(features[0], features[1]);
            cv::Mat H = estimateHomography(features[0], features[1], matches);
            cv::Mat identity = cv::Mat::eye(3, 3, CV_64F);

            auto warped = warpImages({ images[0], images[1] }, { identity, H });
            panorama = blendImages(warped);
        }
        else
        {
            std::vector<cv::Mat> pairwiseH;
            pairwiseH.reserve(n - 1);
            for (int i = 0; i < n - 1; ++i)
            {
                auto matches = matchFeatures(features[i], features[i + 1]);
                pairwiseH.push_back(
                    estimateHomography(features[i], features[i + 1], matches));
            }

            int ref = n / 2;
            auto absH = buildAbsoluteHomographies(pairwiseH, ref);
            auto warped = warpImages(images, absH);
            panorama = blendImages(warped);
        }

        saveImage(outputPath, panorama);

        PipelineResult result;
        result.panorama = panorama;
        result.imagesCount = n;
        return result;
    }

} // namespace panorama
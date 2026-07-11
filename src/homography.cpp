#include "homography.hh"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

namespace panorama
{

    constexpr int kMinMatches = 4;
    constexpr double kRansacThreshold = 4.0;
    constexpr int kRansacIterations = 2000;

    void solveLinearSystem(std::vector<std::vector<double>>& A,
                           std::vector<double>& b)
    {
        const int n = static_cast<int>(b.size());

        for (int col = 0; col < n; ++col)
        {
            int pivotRow = col;
            double pivotVal = std::abs(A[col][col]);
            for (int row = col + 1; row < n; ++row)
            {
                if (std::abs(A[row][col]) > pivotVal)
                {
                    pivotVal = std::abs(A[row][col]);
                    pivotRow = row;
                }
            }

            if (pivotVal < 1e-12)
                throw std::runtime_error(
                    "solveLinearSystem: systeme singulier");

            std::swap(A[col], A[pivotRow]);
            std::swap(b[col], b[pivotRow]);

            for (int row = col + 1; row < n; ++row)
            {
                const double factor = A[row][col] / A[col][col];
                for (int k = col; k < n; ++k)
                    A[row][k] -= factor * A[col][k];
                b[row] -= factor * b[col];
            }
        }

        for (int row = n - 1; row >= 0; --row)
        {
            double sum = b[row];
            for (int k = row + 1; k < n; ++k)
                sum -= A[row][k] * b[k];
            b[row] = sum / A[row][row];
        }
    }

    cv::Mat computeHomographyDLT(const std::vector<cv::Point2f>& src,
                                 const std::vector<cv::Point2f>& dst)
    {
        const int n = static_cast<int>(src.size());

        std::vector<std::vector<double>> AtA(8, std::vector<double>(8, 0.0));
        std::vector<double> Atb(8, 0.0);

        for (int i = 0; i < n; ++i)
        {
            const double x = src[i].x, y = src[i].y;
            const double xp = dst[i].x, yp = dst[i].y;

            double row1[8] = { x, y, 1, 0, 0, 0, -x * xp, -y * xp };
            double row2[8] = { 0, 0, 0, x, y, 1, -x * yp, -y * yp };

            for (int a = 0; a < 8; ++a)
            {
                Atb[a] += row1[a] * xp + row2[a] * yp;
                for (int b = 0; b < 8; ++b)
                {
                    AtA[a][b] += row1[a] * row1[b] + row2[a] * row2[b];
                }
            }
        }

        std::vector<double> h = Atb;
        solveLinearSystem(AtA, h);

        return (cv::Mat_<double>(3, 3) << h[0], h[1], h[2], h[3], h[4], h[5],
                h[6], h[7], 1.0);
    }

    double reprojectionError(const cv::Mat& H, const cv::Point2f& srcPt,
                             const cv::Point2f& dstPt)
    {
        const double* h = H.ptr<double>();
        const double wx = h[0] * srcPt.x + h[1] * srcPt.y + h[2];
        const double wy = h[3] * srcPt.x + h[4] * srcPt.y + h[5];
        const double w = h[6] * srcPt.x + h[7] * srcPt.y + h[8];

        if (std::abs(w) < 1e-12)
            return std::numeric_limits<double>::max();

        const double px = wx / w;
        const double py = wy / w;
        const double dx = px - dstPt.x;
        const double dy = py - dstPt.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    HomographyResult ransacHomography(const std::vector<cv::Point2f>& src,
                                      const std::vector<cv::Point2f>& dst)
    {
        const int n = static_cast<int>(src.size());

        std::mt19937 rng(
            42); // seed fixe: resultats reproductibles d'un run a l'autre
        std::uniform_int_distribution<int> pick(0, n - 1);

        cv::Mat bestH;
        std::vector<uint8_t> bestInliers;
        int bestInlierCount = -1;

        for (int iter = 0; iter < kRansacIterations; ++iter)
        {
            // 4 indices distincts
            std::vector<int> sample;
            while (sample.size() < 4)
            {
                int idx = pick(rng);
                if (std::find(sample.begin(), sample.end(), idx)
                    == sample.end())
                    sample.push_back(idx);
            }

            std::vector<cv::Point2f> sampleSrc, sampleDst;
            for (int idx : sample)
            {
                sampleSrc.push_back(src[idx]);
                sampleDst.push_back(dst[idx]);
            }

            cv::Mat H;
            try
            {
                H = computeHomographyDLT(sampleSrc, sampleDst);
            }
            catch (const std::runtime_error&)
            {
                continue; // echantillon degenere, on retire
            }

            std::vector<uint8_t> inliers(n, 0);
            int inlierCount = 0;
            for (int i = 0; i < n; ++i)
            {
                if (reprojectionError(H, src[i], dst[i]) < kRansacThreshold)
                {
                    inliers[i] = 1;
                    ++inlierCount;
                }
            }

            if (inlierCount > bestInlierCount)
            {
                bestInlierCount = inlierCount;
                bestInliers = inliers;
                bestH = H;
            }
        }

        if (bestInlierCount < kMinMatches)
            throw std::runtime_error("estimateHomography: RANSAC n'a "
                                     "trouve aucun consensus valable");

        std::vector<cv::Point2f> inlierSrc, inlierDst;
        for (int i = 0; i < n; ++i)
        {
            if (bestInliers[i])
            {
                inlierSrc.push_back(src[i]);
                inlierDst.push_back(dst[i]);
            }
        }

        cv::Mat refined = computeHomographyDLT(inlierSrc, inlierDst);
        return HomographyResult{ refined, bestInliers };
    }

    HomographyResult estimateHomography(const Features& first,
                                        const Features& second,
                                        const std::vector<cv::DMatch>& matches)
    {
        if (matches.size() < static_cast<size_t>(kMinMatches))
            throw std::runtime_error(
                "estimateHomography: pas assez de correspondances ("
                + std::to_string(matches.size()) + " < "
                + std::to_string(kMinMatches) + ")");

        std::vector<cv::Point2f> pointsFirst;
        std::vector<cv::Point2f> pointsSecond;
        pointsFirst.reserve(matches.size());
        pointsSecond.reserve(matches.size());

        for (const auto& match : matches)
        {
            pointsFirst.push_back(first.keypoints[match.queryIdx].pt);
            pointsSecond.push_back(second.keypoints[match.trainIdx].pt);
        }

        return ransacHomography(pointsSecond, pointsFirst);
    }

} // namespace panorama
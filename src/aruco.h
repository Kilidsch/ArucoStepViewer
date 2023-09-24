#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <vector>

struct ArucoImages
{
    std::vector<cv::Mat> threshold;
    std::vector<cv::Mat> contours;
    std::vector<cv::Mat> contoursRaw;
    std::vector<cv::Mat> contoursPerimeter;
    std::vector<cv::Mat> contoursSquareConvex;
    std::vector<cv::Mat> contoursMinDist;
    // no extra thing for close to image border by now
};

void simulateDetectMarkers(cv::InputArray _image, const cv::Ptr<cv::aruco::Dictionary> &_dictionary,
                           cv::OutputArrayOfArrays _corners, cv::OutputArray _ids,
                           const cv::Ptr<cv::aruco::DetectorParameters> &_params,
                           cv::OutputArrayOfArrays _rejectedImgPoints, cv::InputArrayOfArrays camMatrix = cv::noArray(),
                           cv::InputArrayOfArrays distCoeff = cv::noArray());

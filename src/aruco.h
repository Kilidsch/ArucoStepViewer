#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>

struct ArucoImages{
    std::vector<cv::Mat> threshold;
    std::vector<cv::Mat> contours;
};

ArucoImages simulateDetectMarkers(cv::InputArray _image, const cv::Ptr<cv::aruco::Dictionary> &_dictionary, cv::OutputArrayOfArrays _corners,
                                           cv::OutputArray _ids, const cv::Ptr<cv::aruco::DetectorParameters> &_params,
                                           cv::OutputArrayOfArrays _rejectedImgPoints, cv::InputArrayOfArrays camMatrix = cv::noArray(), cv::InputArrayOfArrays distCoeff = cv::noArray());

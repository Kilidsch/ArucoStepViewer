/**
 * To large extend code copied from aruco module of OpenCV
 */

#include "aruco.h"

#include "opencv2/aruco.hpp"
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>


using namespace cv;
using namespace cv::aruco;
using namespace std;

/**
 * @brief Convert input image to gray if it is a 3-channels image
 */
static void _convertToGrey(InputArray _in, OutputArray _out)
{

    CV_Assert(_in.type() == CV_8UC1 || _in.type() == CV_8UC3);

    if (_in.type() == CV_8UC3)
        cvtColor(_in, _out, COLOR_BGR2GRAY);
    else
        _in.copyTo(_out);
}

/**
 * @brief Threshold input image using adaptive thresholding
 */
static void _threshold(InputArray _in, OutputArray _out, int winSize, double constant)
{

    CV_Assert(winSize >= 3);
    if (winSize % 2 == 0)
        winSize++; // win size must be odd
    adaptiveThreshold(_in, _out, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, winSize, constant);
}

/**
 * @brief Given a tresholded image, find the contours, calculate their polygonal approximation
 * and take those that accomplish some conditions
 */
static void _findMarkerContours(InputArray _in, vector<vector<Point2f>> &candidates,
                                vector<vector<Point>> &contoursOut, double minPerimeterRate,
                                double maxPerimeterRate, double accuracyRate,
                                double minCornerDistanceRate, int minDistanceToBorder)
{

    CV_Assert(minPerimeterRate > 0 && maxPerimeterRate > 0 && accuracyRate > 0 &&
              minCornerDistanceRate >= 0 && minDistanceToBorder >= 0);

    // calculate maximum and minimum sizes in pixels
    unsigned int minPerimeterPixels =
        (unsigned int)(minPerimeterRate * max(_in.getMat().cols, _in.getMat().rows));
    unsigned int maxPerimeterPixels =
        (unsigned int)(maxPerimeterRate * max(_in.getMat().cols, _in.getMat().rows));

    Mat contoursImg;
    _in.getMat().copyTo(contoursImg);
    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_LIST, CHAIN_APPROX_NONE);
    // now filter list of contours
    for (unsigned int i = 0; i < contours.size(); i++)
    {
        // check perimeter
        if (contours[i].size() < minPerimeterPixels || contours[i].size() > maxPerimeterPixels)
            continue;

        // check is square and is convex
        vector<Point> approxCurve;
        approxPolyDP(contours[i], approxCurve, double(contours[i].size()) * accuracyRate, true);
        if (approxCurve.size() != 4 || !isContourConvex(approxCurve))
            continue;

        // check min distance between corners
        double minDistSq =
            max(contoursImg.cols, contoursImg.rows) * max(contoursImg.cols, contoursImg.rows);
        for (int j = 0; j < 4; j++)
        {
            double d = (double)(approxCurve[j].x - approxCurve[(j + 1) % 4].x) *
                           (double)(approxCurve[j].x - approxCurve[(j + 1) % 4].x) +
                       (double)(approxCurve[j].y - approxCurve[(j + 1) % 4].y) *
                           (double)(approxCurve[j].y - approxCurve[(j + 1) % 4].y);
            minDistSq = min(minDistSq, d);
        }
        double minCornerDistancePixels = double(contours[i].size()) * minCornerDistanceRate;
        if (minDistSq < minCornerDistancePixels * minCornerDistancePixels)
            continue;

        // check if it is too near to the image border
        bool tooNearBorder = false;
        for (int j = 0; j < 4; j++)
        {
            if (approxCurve[j].x < minDistanceToBorder || approxCurve[j].y < minDistanceToBorder ||
                approxCurve[j].x > contoursImg.cols - 1 - minDistanceToBorder ||
                approxCurve[j].y > contoursImg.rows - 1 - minDistanceToBorder)
                tooNearBorder = true;
        }
        if (tooNearBorder)
            continue;

        // if it passes all the test, add to candidates vector
        vector<Point2f> currentCandidate;
        currentCandidate.resize(4);
        for (int j = 0; j < 4; j++)
        {
            currentCandidate[j] = Point2f((float)approxCurve[j].x, (float)approxCurve[j].y);
        }
        candidates.push_back(currentCandidate);
        contoursOut.push_back(contours[i]);
    }
}

/**
 * @brief Assure order of candidate corners is clockwise direction
 */
static void _reorderCandidatesCorners(vector<vector<Point2f>> &candidates)
{

    for (unsigned int i = 0; i < candidates.size(); i++)
    {
        double dx1 = candidates[i][1].x - candidates[i][0].x;
        double dy1 = candidates[i][1].y - candidates[i][0].y;
        double dx2 = candidates[i][2].x - candidates[i][0].x;
        double dy2 = candidates[i][2].y - candidates[i][0].y;
        double crossProduct = (dx1 * dy2) - (dy1 * dx2);

        if (crossProduct < 0.0)
        { // not clockwise direction
            swap(candidates[i][1], candidates[i][3]);
        }
    }
}

/**
 * @brief to make sure that the corner's order of both candidates (default/white) is the same
 */
static vector<Point2f> alignContourOrder(Point2f corner, vector<Point2f> candidate)
{
    uint8_t r = 0;
    double min = cv::norm(Vec2f(corner - candidate[0]), NORM_L2SQR);
    for (uint8_t pos = 1; pos < 4; pos++)
    {
        double nDiff = cv::norm(Vec2f(corner - candidate[pos]), NORM_L2SQR);
        if (nDiff < min)
        {
            r = pos;
            min = nDiff;
        }
    }
    std::rotate(candidate.begin(), candidate.begin() + r, candidate.end());
    return candidate;
}

/**
 * @brief Check candidates that are too close to each other, save the potential candidates
 *        (i.e. biggest/smallest contour) and remove the rest
 */
static void _filterTooCloseCandidates(const vector<vector<Point2f>> &candidatesIn,
                                      vector<vector<vector<Point2f>>> &candidatesSetOut,
                                      const vector<vector<Point>> &contoursIn,
                                      vector<vector<vector<Point>>> &contoursSetOut,
                                      double minMarkerDistanceRate, bool detectInvertedMarker)
{

    CV_Assert(minMarkerDistanceRate >= 0);

    vector<int> candGroup;
    candGroup.resize(candidatesIn.size(), -1);
    vector<vector<unsigned int>> groupedCandidates;
    for (unsigned int i = 0; i < candidatesIn.size(); i++)
    {
        for (unsigned int j = i + 1; j < candidatesIn.size(); j++)
        {

            int minimumPerimeter = min((int)contoursIn[i].size(), (int)contoursIn[j].size());

            // fc is the first corner considered on one of the markers, 4 combinations are possible
            for (int fc = 0; fc < 4; fc++)
            {
                double distSq = 0;
                for (int c = 0; c < 4; c++)
                {
                    // modC is the corner considering first corner is fc
                    int modC = (c + fc) % 4;
                    distSq += (candidatesIn[i][modC].x - candidatesIn[j][c].x) *
                                  (candidatesIn[i][modC].x - candidatesIn[j][c].x) +
                              (candidatesIn[i][modC].y - candidatesIn[j][c].y) *
                                  (candidatesIn[i][modC].y - candidatesIn[j][c].y);
                }
                distSq /= 4.;

                // if mean square distance is too low, remove the smaller one of the two markers
                double minMarkerDistancePixels = double(minimumPerimeter) * minMarkerDistanceRate;
                if (distSq < minMarkerDistancePixels * minMarkerDistancePixels)
                {

                    // i and j are not related to a group
                    if (candGroup[i] < 0 && candGroup[j] < 0)
                    {
                        // mark candidates with their corresponding group number
                        candGroup[i] = candGroup[j] = (int)groupedCandidates.size();

                        // create group
                        vector<unsigned int> grouped;
                        grouped.push_back(i);
                        grouped.push_back(j);
                        groupedCandidates.push_back(grouped);
                    }
                    // i is related to a group
                    else if (candGroup[i] > -1 && candGroup[j] == -1)
                    {
                        int group = candGroup[i];
                        candGroup[j] = group;

                        // add to group
                        groupedCandidates[group].push_back(j);
                    }
                    // j is related to a group
                    else if (candGroup[j] > -1 && candGroup[i] == -1)
                    {
                        int group = candGroup[j];
                        candGroup[i] = group;

                        // add to group
                        groupedCandidates[group].push_back(i);
                    }
                }
            }
        }
    }

    // save possible candidates
    candidatesSetOut.clear();
    contoursSetOut.clear();

    vector<vector<Point2f>> biggerCandidates;
    vector<vector<Point>> biggerContours;
    vector<vector<Point2f>> smallerCandidates;
    vector<vector<Point>> smallerContours;

    // save possible candidates
    for (unsigned int i = 0; i < groupedCandidates.size(); i++)
    {
        unsigned int smallerIdx = groupedCandidates[i][0];
        unsigned int biggerIdx = smallerIdx;
        double smallerArea = contourArea(candidatesIn[smallerIdx]);
        double biggerArea = smallerArea;

        // evaluate group elements
        for (unsigned int j = 1; j < groupedCandidates[i].size(); j++)
        {
            unsigned int currIdx = groupedCandidates[i][j];
            double currArea = contourArea(candidatesIn[currIdx]);

            // check if current contour is bigger
            if (currArea >= biggerArea)
            {
                biggerIdx = currIdx;
                biggerArea = currArea;
            }

            // check if current contour is smaller
            if (currArea < smallerArea && detectInvertedMarker)
            {
                smallerIdx = currIdx;
                smallerArea = currArea;
            }
        }

        // add contours and candidates
        biggerCandidates.push_back(candidatesIn[biggerIdx]);
        biggerContours.push_back(contoursIn[biggerIdx]);
        if (detectInvertedMarker)
        {
            smallerCandidates.push_back(alignContourOrder(candidatesIn[biggerIdx][0], candidatesIn[smallerIdx]));
            smallerContours.push_back(contoursIn[smallerIdx]);
        }
    }
    // to preserve the structure :: candidateSet< defaultCandidates, whiteCandidates >
    // default candidates
    candidatesSetOut.push_back(biggerCandidates);
    contoursSetOut.push_back(biggerContours);
    // white candidates
    candidatesSetOut.push_back(smallerCandidates);
    contoursSetOut.push_back(smallerContours);
}

/**
 * @brief Initial steps on finding square candidates
 */
static void _detectInitialCandidates(const Mat &grey, vector<vector<Point2f>> &candidates,
                                     vector<vector<Point>> &contours,
                                     const Ptr<DetectorParameters> &params,
                                     ArucoImages& previewImages)
{

    CV_Assert(params->adaptiveThreshWinSizeMin >= 3 && params->adaptiveThreshWinSizeMax >= 3);
    CV_Assert(params->adaptiveThreshWinSizeMax >= params->adaptiveThreshWinSizeMin);
    CV_Assert(params->adaptiveThreshWinSizeStep > 0);

    // number of window sizes (scales) to apply adaptive thresholding
    int nScales = (params->adaptiveThreshWinSizeMax - params->adaptiveThreshWinSizeMin) /
                      params->adaptiveThreshWinSizeStep +
                  1;

    vector<vector<vector<Point2f>>> candidatesArrays((size_t)nScales);
    vector<vector<vector<Point>>> contoursArrays((size_t)nScales);
    previewImages.threshold.resize(nScales);
    previewImages.contours.resize(nScales);

    ////for each value in the interval of thresholding window sizes
    parallel_for_(Range(0, nScales), [&](const Range &range)
                  {
        const int begin = range.start;
        const int end = range.end;

        for (int i = begin; i < end; i++) {
            int currScale = params->adaptiveThreshWinSizeMin + i * params->adaptiveThreshWinSizeStep;
            // threshold
            Mat& thresh = previewImages.threshold[i]; // cv::Mat -> shared_ptr
            _threshold(grey, thresh, currScale, params->adaptiveThreshConstant);
            // TODO: How to show this? Parallel and number of images is dependent on parameters...

            // detect rectangles
            _findMarkerContours(thresh, candidatesArrays[i], contoursArrays[i],
                                params->minMarkerPerimeterRate, params->maxMarkerPerimeterRate,
                                params->polygonalApproxAccuracyRate, params->minCornerDistanceRate,
                                params->minDistanceToBorder);
            // TODO: Draw contours! Save in Output
            cv::cvtColor(thresh, previewImages.contours[i], cv::COLOR_GRAY2BGR);
            cv::drawContours(previewImages.contours[i], contoursArrays[i], -1, cv::Scalar(255,0,0), 2);
        } });

    // join candidates
    for (int i = 0; i < nScales; i++)
    {
        for (unsigned int j = 0; j < candidatesArrays[i].size(); j++)
        {
            candidates.push_back(candidatesArrays[i][j]);
            contours.push_back(contoursArrays[i][j]);
        }
    }
}

/**
 * @brief Detect square candidates in the input image
 */
static void _detectCandidates(InputArray _image, vector<vector<vector<Point2f>>> &candidatesSetOut,
                              vector<vector<vector<Point>>> &contoursSetOut, const Ptr<DetectorParameters> &_params,
                              ArucoImages& result)
{

    Mat image = _image.getMat();
    CV_Assert(image.total() != 0);

    /// 1. CONVERT TO GRAY
    Mat grey;
    _convertToGrey(image, grey);

    // TODO: gray image as debug

    vector<vector<Point2f>> candidates;
    vector<vector<Point>> contours;
    /// 2. DETECT FIRST SET OF CANDIDATES
    _detectInitialCandidates(grey, candidates, contours, _params, result);
    /// 3. SORT CORNERS
    _reorderCandidatesCorners(candidates);

    /// 4. FILTER OUT NEAR CANDIDATE PAIRS
    // save the outter/inner border (i.e. potential candidates)
    _filterTooCloseCandidates(candidates, candidatesSetOut, contours, contoursSetOut,
                              _params->minMarkerDistanceRate, _params->detectInvertedMarker);
}

/**
 * @brief Given an input image and candidate corners, extract the bits of the candidate, including
 * the border bits
 */
static Mat _extractBits(InputArray _image, InputArray _corners, int markerSize,
                        int markerBorderBits, int cellSize, double cellMarginRate,
                        double minStdDevOtsu)
{

    CV_Assert(_image.getMat().channels() == 1);
    CV_Assert(_corners.total() == 4);
    CV_Assert(markerBorderBits > 0 && cellSize > 0 && cellMarginRate >= 0 && cellMarginRate <= 1);
    CV_Assert(minStdDevOtsu >= 0);

    // number of bits in the marker
    int markerSizeWithBorders = markerSize + 2 * markerBorderBits;
    int cellMarginPixels = int(cellMarginRate * cellSize);

    Mat resultImg; // marker image after removing perspective
    int resultImgSize = markerSizeWithBorders * cellSize;
    Mat resultImgCorners(4, 1, CV_32FC2);
    resultImgCorners.ptr<Point2f>(0)[0] = Point2f(0, 0);
    resultImgCorners.ptr<Point2f>(0)[1] = Point2f((float)resultImgSize - 1, 0);
    resultImgCorners.ptr<Point2f>(0)[2] =
        Point2f((float)resultImgSize - 1, (float)resultImgSize - 1);
    resultImgCorners.ptr<Point2f>(0)[3] = Point2f(0, (float)resultImgSize - 1);

    // remove perspective
    Mat transformation = getPerspectiveTransform(_corners, resultImgCorners);
    warpPerspective(_image, resultImg, transformation, Size(resultImgSize, resultImgSize),
                    INTER_NEAREST);

    // output image containing the bits
    Mat bits(markerSizeWithBorders, markerSizeWithBorders, CV_8UC1, Scalar::all(0));

    // check if standard deviation is enough to apply Otsu
    // if not enough, it probably means all bits are the same color (black or white)
    Mat mean, stddev;
    // Remove some border just to avoid border noise from perspective transformation
    Mat innerRegion = resultImg.colRange(cellSize / 2, resultImg.cols - cellSize / 2)
                          .rowRange(cellSize / 2, resultImg.rows - cellSize / 2);
    meanStdDev(innerRegion, mean, stddev);
    if (stddev.ptr<double>(0)[0] < minStdDevOtsu)
    {
        // all black or all white, depending on mean value
        if (mean.ptr<double>(0)[0] > 127)
            bits.setTo(1);
        else
            bits.setTo(0);
        return bits;
    }

    // now extract code, first threshold using Otsu
    threshold(resultImg, resultImg, 125, 255, THRESH_BINARY | THRESH_OTSU);

    // for each cell
    for (int y = 0; y < markerSizeWithBorders; y++)
    {
        for (int x = 0; x < markerSizeWithBorders; x++)
        {
            int Xstart = x * (cellSize) + cellMarginPixels;
            int Ystart = y * (cellSize) + cellMarginPixels;
            Mat square = resultImg(Rect(Xstart, Ystart, cellSize - 2 * cellMarginPixels,
                                        cellSize - 2 * cellMarginPixels));
            // count white pixels on each cell to assign its value
            size_t nZ = (size_t)countNonZero(square);
            if (nZ > square.total() / 2)
                bits.at<unsigned char>(y, x) = 1;
        }
    }

    return bits;
}

/**
 * @brief Return number of erroneous bits in border, i.e. number of white bits in border.
 */
static int _getBorderErrors(const Mat &bits, int markerSize, int borderSize)
{

    int sizeWithBorders = markerSize + 2 * borderSize;

    CV_Assert(markerSize > 0 && bits.cols == sizeWithBorders && bits.rows == sizeWithBorders);

    int totalErrors = 0;
    for (int y = 0; y < sizeWithBorders; y++)
    {
        for (int k = 0; k < borderSize; k++)
        {
            if (bits.ptr<unsigned char>(y)[k] != 0)
                totalErrors++;
            if (bits.ptr<unsigned char>(y)[sizeWithBorders - 1 - k] != 0)
                totalErrors++;
        }
    }
    for (int x = borderSize; x < sizeWithBorders - borderSize; x++)
    {
        for (int k = 0; k < borderSize; k++)
        {
            if (bits.ptr<unsigned char>(k)[x] != 0)
                totalErrors++;
            if (bits.ptr<unsigned char>(sizeWithBorders - 1 - k)[x] != 0)
                totalErrors++;
        }
    }
    return totalErrors;
}

/**
 * @brief Tries to identify one candidate given the dictionary
 * @return candidate typ. zero if the candidate is not valid,
 *                           1 if the candidate is a black candidate (default candidate)
 *                           2 if the candidate is a white candidate
 */
static uint8_t _identifyOneCandidate(const Ptr<Dictionary> &dictionary, InputArray _image,
                                     vector<Point2f> &_corners, int &idx,
                                     const Ptr<DetectorParameters> &params, int &rotation)
{
    CV_Assert(_corners.size() == 4);
    CV_Assert(_image.getMat().total() != 0);
    CV_Assert(params->markerBorderBits > 0);

    uint8_t typ = 1;
    // get bits
    Mat candidateBits =
        _extractBits(_image, _corners, dictionary->markerSize, params->markerBorderBits,
                     params->perspectiveRemovePixelPerCell,
                     params->perspectiveRemoveIgnoredMarginPerCell, params->minOtsuStdDev);

    // analyze border bits
    int maximumErrorsInBorder =
        int(dictionary->markerSize * dictionary->markerSize * params->maxErroneousBitsInBorderRate);
    int borderErrors =
        _getBorderErrors(candidateBits, dictionary->markerSize, params->markerBorderBits);

    // check if it is a white marker
    if (params->detectInvertedMarker)
    {
        // to get from 255 to 1
        Mat invertedImg = ~candidateBits - 254;
        int invBError = _getBorderErrors(invertedImg, dictionary->markerSize, params->markerBorderBits);
        // white marker
        if (invBError < borderErrors)
        {
            borderErrors = invBError;
            invertedImg.copyTo(candidateBits);
            typ = 2;
        }
    }
    if (borderErrors > maximumErrorsInBorder)
        return 0; // border is wrong

    // take only inner bits
    Mat onlyBits =
        candidateBits.rowRange(params->markerBorderBits,
                               candidateBits.rows - params->markerBorderBits)
            .colRange(params->markerBorderBits, candidateBits.cols - params->markerBorderBits);

    // try to indentify the marker
    if (!dictionary->identify(onlyBits, idx, rotation, params->errorCorrectionRate))
        return 0;

    return typ;
}

/**
 * @brief Copy the contents of a corners vector to an OutputArray, settings its size.
 */
static void _copyVector2Output(vector<vector<Point2f>> &vec, OutputArrayOfArrays out)
{
    out.create((int)vec.size(), 1, CV_32FC2);

    if (out.isMatVector())
    {
        for (unsigned int i = 0; i < vec.size(); i++)
        {
            out.create(4, 1, CV_32FC2, i);
            Mat &m = out.getMatRef(i);
            Mat(Mat(vec[i]).t()).copyTo(m);
        }
    }
    else if (out.isUMatVector())
    {
        for (unsigned int i = 0; i < vec.size(); i++)
        {
            out.create(4, 1, CV_32FC2, i);
            UMat &m = out.getUMatRef(i);
            Mat(Mat(vec[i]).t()).copyTo(m);
        }
    }
    else if (out.kind() == _OutputArray::STD_VECTOR_VECTOR)
    {
        for (unsigned int i = 0; i < vec.size(); i++)
        {
            out.create(4, 1, CV_32FC2, i);
            Mat m = out.getMat(i);
            Mat(Mat(vec[i]).t()).copyTo(m);
        }
    }
    else
    {
        CV_Error(cv::Error::StsNotImplemented,
                 "Only Mat vector, UMat vector, and vector<vector> OutputArrays are currently supported.");
    }
}

/**
 * @brief rotate the initial corner to get to the right position
 */
static void correctCornerPosition(vector<Point2f> &_candidate, int rotate)
{
    std::rotate(_candidate.begin(), _candidate.begin() + 4 - rotate, _candidate.end());
}

/**
 * @brief Identify square candidates according to a marker dictionary
 */
static void _identifyCandidates(InputArray _image, vector<vector<vector<Point2f>>> &_candidatesSet,
                                vector<vector<vector<Point>>> &_contoursSet, const Ptr<Dictionary> &_dictionary,
                                vector<vector<Point2f>> &_accepted, vector<vector<Point>> &_contours, vector<int> &ids,
                                const Ptr<DetectorParameters> &params,
                                OutputArrayOfArrays _rejected = noArray())
{

    int ncandidates = (int)_candidatesSet[0].size();
    vector<vector<Point2f>> accepted;
    vector<vector<Point2f>> rejected;

    vector<vector<Point>> contours;

    CV_Assert(_image.getMat().total() != 0);

    Mat grey;
    _convertToGrey(_image.getMat(), grey);

    vector<int> idsTmp(ncandidates, -1);
    vector<int> rotated(ncandidates, 0);
    vector<uint8_t> validCandidates(ncandidates, 0);

    //// Analyze each of the candidates
    parallel_for_(Range(0, ncandidates), [&](const Range &range)
                  {
        const int begin = range.start;
        const int end = range.end;

        vector< vector< Point2f > >& candidates = params->detectInvertedMarker ? _candidatesSet[1] : _candidatesSet[0];

        for(int i = begin; i < end; i++) {
            int currId;
            validCandidates[i] = _identifyOneCandidate(_dictionary, grey, candidates[i], currId, params, rotated[i]);

            if(validCandidates[i] > 0)
                idsTmp[i] = currId;
        } });

    for (int i = 0; i < ncandidates; i++)
    {
        if (validCandidates[i] > 0)
        {
            // to choose the right set of candidates :: 0 for default, 1 for white markers
            uint8_t set = validCandidates[i] - 1;

            // shift corner positions to the correct rotation
            correctCornerPosition(_candidatesSet[set][i], rotated[i]);

            if (!params->detectInvertedMarker && validCandidates[i] == 2)
                continue;

            // add valid candidate
            accepted.push_back(_candidatesSet[set][i]);
            ids.push_back(idsTmp[i]);

            contours.push_back(_contoursSet[set][i]);
        }
        else
        {
            rejected.push_back(_candidatesSet[0][i]);
        }
    }

    // parse output
    _accepted = accepted;

    _contours = contours;

    if (_rejected.needed())
    {
        _copyVector2Output(rejected, _rejected);
    }
}

/**
 * Line fitting  A * B = C :: Called from function refineCandidateLines
 * @param nContours, contour-container
 */
static Point3f _interpolate2Dline(const std::vector<cv::Point2f> &nContours)
{
    float minX, minY, maxX, maxY;
    minX = maxX = nContours[0].x;
    minY = maxY = nContours[0].y;

    for (unsigned int i = 0; i < nContours.size(); i++)
    {
        minX = nContours[i].x < minX ? nContours[i].x : minX;
        minY = nContours[i].y < minY ? nContours[i].y : minY;
        maxX = nContours[i].x > maxX ? nContours[i].x : maxX;
        maxY = nContours[i].y > maxY ? nContours[i].y : maxY;
    }

    Mat A = Mat::ones((int)nContours.size(), 2, CV_32F); // Coefficient Matrix (N x 2)
    Mat B((int)nContours.size(), 1, CV_32F);             // Variables   Matrix (N x 1)
    Mat C;                                               // Constant

    if (maxX - minX > maxY - minY)
    {
        for (unsigned int i = 0; i < nContours.size(); i++)
        {
            A.at<float>(i, 0) = nContours[i].x;
            B.at<float>(i, 0) = nContours[i].y;
        }

        solve(A, B, C, DECOMP_NORMAL);

        return Point3f(C.at<float>(0, 0), -1., C.at<float>(1, 0));
    }
    else
    {
        for (unsigned int i = 0; i < nContours.size(); i++)
        {
            A.at<float>(i, 0) = nContours[i].y;
            B.at<float>(i, 0) = nContours[i].x;
        }

        solve(A, B, C, DECOMP_NORMAL);

        return Point3f(-1., C.at<float>(0, 0), C.at<float>(1, 0));
    }
}

/**
 * Find the Point where the lines crosses :: Called from function refineCandidateLines
 * @param nLine1
 * @param nLine2
 * @return Crossed Point
 */
static Point2f _getCrossPoint(Point3f nLine1, Point3f nLine2)
{
    Matx22f A(nLine1.x, nLine1.y, nLine2.x, nLine2.y);
    Vec2f B(-nLine1.z, -nLine2.z);
    return Vec2f(A.solve(B).val);
}

static void _distortPoints(vector<cv::Point2f> &in, const Mat &camMatrix, const Mat &distCoeff)
{
    // trivial extrinsics
    Matx31f Rvec(0, 0, 0);
    Matx31f Tvec(0, 0, 0);

    // calculate 3d points and then reproject, so opencv makes the distortion internally
    vector<cv::Point3f> cornersPoints3d;
    for (unsigned int i = 0; i < in.size(); i++)
    {
        float x = (in[i].x - float(camMatrix.at<double>(0, 2))) / float(camMatrix.at<double>(0, 0));
        float y = (in[i].y - float(camMatrix.at<double>(1, 2))) / float(camMatrix.at<double>(1, 1));
        cornersPoints3d.push_back(Point3f(x, y, 1));
    }
    cv::projectPoints(cornersPoints3d, Rvec, Tvec, camMatrix, distCoeff, in);
}

/**
 * Refine Corners using the contour vector :: Called from function detectMarkers
 * @param nContours, contour-container
 * @param nCorners, candidate Corners
 * @param camMatrix, cameraMatrix input 3x3 floating-point camera matrix
 * @param distCoeff, distCoeffs vector of distortion coefficient
 */
static void _refineCandidateLines(std::vector<Point> &nContours, std::vector<Point2f> &nCorners, const Mat &camMatrix, const Mat &distCoeff)
{
    vector<Point2f> contour2f(nContours.begin(), nContours.end());

    if (!camMatrix.empty() && !distCoeff.empty())
    {
        undistortPoints(contour2f, contour2f, camMatrix, distCoeff);
    }

    /* 5 groups :: to group the edges
     * 4 - classified by its corner
     * extra group - (temporary) if contours do not begin with a corner
     */
    vector<Point2f> cntPts[5];
    int cornerIndex[4] = {-1};
    int group = 4;

    for (unsigned int i = 0; i < nContours.size(); i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            if (nCorners[j] == contour2f[i])
            {
                cornerIndex[j] = i;
                group = j;
            }
        }
        cntPts[group].push_back(contour2f[i]);
    }

    // saves extra group into corresponding
    if (!cntPts[4].empty())
    {
        CV_CheckLT(group, 4, "FIXIT: avoiding infinite loop: implementation should be revised: https://github.com/opencv/opencv_contrib/issues/2738");
        for (unsigned int i = 0; i < cntPts[4].size(); i++)
            cntPts[group].push_back(cntPts[4].at(i));
        cntPts[4].clear();
    }

    // Evaluate contour direction :: using the position of the detected corners
    int inc = 1;

    inc = ((cornerIndex[0] > cornerIndex[1]) && (cornerIndex[3] > cornerIndex[0])) ? -1 : inc;
    inc = ((cornerIndex[2] > cornerIndex[3]) && (cornerIndex[1] > cornerIndex[2])) ? -1 : inc;

    // calculate the line :: who passes through the grouped points
    Point3f lines[4];
    for (int i = 0; i < 4; i++)
    {
        lines[i] = _interpolate2Dline(cntPts[i]);
    }

    /*
     * calculate the corner :: where the lines crosses to each other
     * clockwise direction		no clockwise direction
     *      0                           1
     *      .---. 1                     .---. 2
     *      |   |                       |   |
     *    3 .___.                     0 .___.
     *          2                           3
     */
    for (int i = 0; i < 4; i++)
    {
        if (inc < 0)
            nCorners[i] = _getCrossPoint(lines[i], lines[(i + 1) % 4]); // 01 12 23 30
        else
            nCorners[i] = _getCrossPoint(lines[i], lines[(i + 3) % 4]); // 30 01 12 23
    }

    if (!camMatrix.empty() && !distCoeff.empty())
    {
        _distortPoints(nCorners, camMatrix, distCoeff);
    }
}

/**
 * This is a modified version of "detectMarkers", which creates debug images
 */
ArucoImages simulateDetectMarkers(InputArray _image, const Ptr<Dictionary> &_dictionary, OutputArrayOfArrays _corners,
                           OutputArray _ids, const Ptr<DetectorParameters> &_params,
                           OutputArrayOfArrays _rejectedImgPoints, InputArrayOfArrays camMatrix, InputArrayOfArrays distCoeff)
{

    CV_Assert(!_image.empty());

    Mat grey;
    _convertToGrey(_image.getMat(), grey);

    /// STEP 1: Detect marker candidates
    vector<vector<Point2f>> candidates;
    vector<vector<Point>> contours;
    vector<int> ids;

    vector<vector<vector<Point2f>>> candidatesSet;
    vector<vector<vector<Point>>> contoursSet;
    /// STEP 1.a Detect marker candidates :: using AprilTag
    // NOTE: This little application works explicitly with aruco, not apriltag
    // if(_params->cornerRefinementMethod == CORNER_REFINE_APRILTAG){
    //     _apriltag(grey, _params, candidates, contours);

    //     candidatesSet.push_back(candidates);
    //     contoursSet.push_back(contours);
    // }

    /// STEP 1.b Detect marker candidates :: traditional way
    // else
    ArucoImages result;
    _detectCandidates(grey, candidatesSet, contoursSet, _params, result);

    /// STEP 2: Check candidate codification (identify markers)
    _identifyCandidates(grey, candidatesSet, contoursSet, _dictionary, candidates, contours, ids, _params,
                        _rejectedImgPoints);

    // copy to output arrays
    _copyVector2Output(candidates, _corners);
    Mat(ids).copyTo(_ids);

    /// STEP 3: Corner refinement :: use corner subpix
    if (_params->cornerRefinementMethod == CORNER_REFINE_SUBPIX)
    {
        CV_Assert(_params->cornerRefinementWinSize > 0 && _params->cornerRefinementMaxIterations > 0 &&
                  _params->cornerRefinementMinAccuracy > 0);

        //// do corner refinement for each of the detected markers
        parallel_for_(Range(0, _corners.cols()), [&](const Range &range)
                      {
            const int begin = range.start;
            const int end = range.end;

            for (int i = begin; i < end; i++) {
                cornerSubPix(grey, _corners.getMat(i),
                             Size(_params->cornerRefinementWinSize, _params->cornerRefinementWinSize),
                             Size(-1, -1),
                             TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS,
                                          _params->cornerRefinementMaxIterations,
                                          _params->cornerRefinementMinAccuracy));
            } });
    }

    /// STEP 3, Optional : Corner refinement :: use contour container
    if (_params->cornerRefinementMethod == CORNER_REFINE_CONTOUR)
    {

        if (!_ids.empty())
        {

            // do corner refinement using the contours for each detected markers
            parallel_for_(Range(0, _corners.cols()), [&](const Range &range)
                          {
                for (int i = range.start; i < range.end; i++) {
                    _refineCandidateLines(contours[i], candidates[i], camMatrix.getMat(),
                                          distCoeff.getMat());
                } });

            // copy the corners to the output array
            _copyVector2Output(candidates, _corners);
        }
    }



    return result;
}

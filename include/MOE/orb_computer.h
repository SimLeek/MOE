#ifndef ORB_COMPUTER_H
#define ORB_COMPUTER_H

#include "MultiPlatform.h"

#include <opencv2/features2d.hpp>
#if defined __has_include
    #if __has_include(<opencv2/cudafeatures2d.hpp>)
        # include <opencv2/cudafeatures2d.hpp>
        # define CUDA_AVAILABLE 1
    #else
        # define CUDA_AVAILABLE 0
    #endif
#endif

#include<zmq.hpp>

class OrbComputer
{
public:
#if CUDA_AVAILABLE
    DLLEXPORT OrbComputer(
        int nfeatures = 500,
        double scaleFactor = 1.2f,
        int nlevels = 8,
        int edgeThreshold = 31,
        int firstLevel = 0,
        int WTA_K = 2,
        cv::ORB::ScoreType scoreType = cv::ORB::HARRIS_SCORE,
        int patchSize = 31,
        int fastThreshold = 20) : the_orb(std::move(cv::cuda::ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize, fastThreshold, false))) {}

protected:
    cv::Ptr<cv::cuda::ORB> the_orb;
#else
    OrbComputer(
        int nfeatures = 500,
        double scaleFactor = 1.2f,
        int nlevels = 8,
        int edgeThreshold = 31,
        int firstLevel = 0,
        int WTA_K = 2,
        cv::ORB::ScoreType scoreType = cv::ORB::HARRIS_SCORE,
        int patchSize = 31,
        int fastThreshold = 20) : the_orb(std::move(cv::ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize, fastThreshold))) {}

protected:
    cv::Ptr<cv::ORB> the_orb;
#endif
public:
    void DLLEXPORT detect_and_compute(cv::InputArray _image, cv::InputArray _mask, std::vector<cv::KeyPoint>& _keypoints, cv::OutputArray _descriptors);

    void DLLEXPORT set_orb(int nfeatures = -1,
        double scaleFactor = -1.0f,
        int nlevels = -1,
        int edgeThreshold = -1,
        int firstLevel = -1,
        int WTA_K = -1,
        cv::ORB::ScoreType scoreType = (cv::ORB::ScoreType)-1,
        int patchSize = -1,
        int fastThreshold = -1);

    void DLLEXPORT get_orb(int& nfeatures,
        float& scaleFactor,
        int& nlevels,
        int& edgeThreshold,
        int& firstLevel,
        int& WTA_K,
        cv::ORB::ScoreType& scoreType,
        int& patchSize,
        int& fastThreshold);



};

void DLLEXPORT orb_loop(zmq::context_t* ctx=NULL);

#endif

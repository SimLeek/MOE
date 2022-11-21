#ifndef ORB_IO_H
#define ORB_IO_H

#include "MultiPlatform.h"

#include <opencv2/opencv.hpp>
#include <zmq.hpp>
#include <vector>

void DLLEXPORT write_mat(zmq::socket_t& publisher, cv::Mat& im);
void DLLEXPORT read_kps(zmq::socket_t& subscriber,
    std::vector<cv::KeyPoint>& kpts,
    cv::Mat& desc);
cv::Mat DLLEXPORT read_mat(zmq::socket_t& subscriber);
void DLLEXPORT write_kps(
    zmq::socket_t& publisher,
    std::vector<cv::KeyPoint>& kpts,
    cv::Mat& desc
);

#endif
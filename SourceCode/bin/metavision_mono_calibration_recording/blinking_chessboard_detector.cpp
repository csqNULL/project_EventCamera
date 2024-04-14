/**********************************************************************************************************************
 * Copyright (c) Prophesee S.A. - All Rights Reserved                                                                 *
 *                                                                                                                    *
 * Subject to Prophesee Metavision Licensing Terms and Conditions ("License T&C's").                                  *
 * You may not use this file except in compliance with these License T&C's.                                           *
 * A copy of these License T&C's is located in the "licensing" folder accompanying this file.                         *
 **********************************************************************************************************************/

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#if CV_MAJOR_VERSION >= 4
#include <opencv2/calib3d/calib3d_c.h>
#endif

#include <metavision/sdk/base/events/event_cd.h>
#include <metavision/sdk/calibration/utils/calibration_detection_frame_generator.h>
#include <metavision/sdk/calibration/utils/points_on_grid.h>

#include "blinking_chessboard_detector.h"

namespace Metavision {

BlinkingChessBoardDetector::BlinkingChessBoardDetector(int width, int height, int cols, int rows,
                                                       const BlinkingFrameGeneratorAlgorithmConfig &config,
                                                       timestamp skip_time_us, bool debug) :
    width_(width),
    height_(height),
    cols_(cols),
    rows_(rows),
    grid_size_(cols, rows),
    use_inverted_gray_(true),
    cos_max_angle_(std::cos(3.14159 / 9)),
    last_ts_(-skip_time_us - 1), // Avoid making the algorithm wait until ts=skip_time_us_
    skip_time_us_(skip_time_us),
    debug_(debug) {
    algo_ = std::make_unique<BlinkingFrameGeneratorAlgorithm>(width, height, config);
    algo_->set_output_callback([this](timestamp ts, cv::Mat &frame) { on_blinking_frame(ts, frame); });
    new_frame_.create(height, width, CV_8UC1);
    frame_.create(height, width, CV_8UC1);
}

BlinkingChessBoardDetector::~BlinkingChessBoardDetector() {}

template<typename InputIt>
void BlinkingChessBoardDetector::process_events(InputIt it_begin, InputIt it_end) {
    algo_->process_events(it_begin, it_end);
}

void BlinkingChessBoardDetector::set_output_callback(const OutputCb &output_cb) {
    output_cb_ = output_cb;
}

void BlinkingChessBoardDetector::on_blinking_frame(timestamp ts, cv::Mat &cb_frame) {
    const int calibration_flags = CV_CALIB_CB_FILTER_QUADS;

    if (ts < last_ts_ + skip_time_us_)
        return;

    if (cb_frame.empty())
        return;

    if (use_inverted_gray_)
        cb_frame.convertTo(new_frame_, CV_8UC1, -1., 255.); // map linearly 0 to 255 and 255 to 0
    else
        cb_frame.convertTo(new_frame_, CV_8UC1); // keep the pixel intensity

    cv::bitwise_xor(new_frame_, frame_, dst_);
    if (!cv::countNonZero(dst_))
        return;

    if (debug_) {
        cv::imshow("[DEBUG] Blinking frame", new_frame_);
        cv::waitKey(1);
    }

    output_calib_results_.reset(height_, width_, CV_8UC3);
    auto &keypoints = output_calib_results_.keypoints_;

    cv::swap(frame_, new_frame_);
    if (!cv::findChessboardCorners(frame_, grid_size_, keypoints, calibration_flags))
        return;

    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 40, 0.001);
    cv::cornerSubPix(frame_, keypoints, cv::Size(11, 11), cv::Size(-1, -1), criteria);

    if (!Metavision::are_points_on_grid_radial_distortion(keypoints, cols_, rows_, cos_max_angle_))
        return;

    auto &output_mat = output_calib_results_.frame_;
    cv::cvtColor(frame_, output_mat, cv::COLOR_GRAY2BGR);
    output_cb_(ts, output_calib_results_);
    last_ts_ = ts;
}

// Template instantiation
template void BlinkingChessBoardDetector::process_events(std::vector<Metavision::EventCD>::const_iterator first,
                                                         std::vector<Metavision::EventCD>::const_iterator last);
template void BlinkingChessBoardDetector::process_events(const Metavision::EventCD *first,
                                                         const Metavision::EventCD *last);

} // namespace Metavision

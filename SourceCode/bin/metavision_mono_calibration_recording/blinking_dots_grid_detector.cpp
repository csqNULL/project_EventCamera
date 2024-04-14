/**********************************************************************************************************************
 * Copyright (c) Prophesee S.A. - All Rights Reserved                                                                 *
 *                                                                                                                    *
 * Subject to Prophesee Metavision Licensing Terms and Conditions ("License T&C's").                                  *
 * You may not use this file except in compliance with these License T&C's.                                           *
 * A copy of these License T&C's is located in the "licensing" folder accompanying this file.                         *
 **********************************************************************************************************************/

#include <opencv2/core/types.hpp>
#include <metavision/sdk/base/events/event_cd.h>
#include <metavision/sdk/calibration/utils/calibration_detection_frame_generator.h>

#include "blinking_dots_grid_detector.h"

namespace Metavision {

BlinkingDotsGridDetector::BlinkingDotsGridDetector(int width, int height,
                                                   const BlinkingDotsGridDetectorAlgorithmConfig &config,
                                                   timestamp skip_time_us) :
    last_ts_(-skip_time_us - 1), // Avoid making the algorithm wait until ts=skip_time_us_
    skip_time_us_(skip_time_us) {
    algo_ = std::make_unique<BlinkingDotsGridDetectorAlgorithm>(width, height, config);
    algo_->set_output_callback([this](timestamp ts, cv::Mat &frame, std::vector<cv::Point2f> &grid_points) {
        on_blinking_pattern(ts, frame, grid_points);
    });
}

BlinkingDotsGridDetector::~BlinkingDotsGridDetector() {}

template<typename InputIt>
void BlinkingDotsGridDetector::process_events(InputIt it_begin, InputIt it_end) {
    algo_->process_events(it_begin, it_end);
}

void BlinkingDotsGridDetector::set_output_callback(const OutputCb &output_cb) {
    output_cb_ = output_cb;
}

void BlinkingDotsGridDetector::on_blinking_pattern(timestamp ts, cv::Mat &frame,
                                                   std::vector<cv::Point2f> &grid_points) {
    if (grid_points.empty() || ts < last_ts_ + skip_time_us_)
        return;

    cv::swap(frame, output_calib_results_.frame_);
    std::swap(grid_points, output_calib_results_.keypoints_);
    output_cb_(ts, output_calib_results_);
    last_ts_ = ts;
}

// Template instantiation
template void BlinkingDotsGridDetector::process_events(std::vector<Metavision::EventCD>::const_iterator first,
                                                       std::vector<Metavision::EventCD>::const_iterator last);

} // namespace Metavision

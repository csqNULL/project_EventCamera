/**********************************************************************************************************************
 * Copyright (c) Prophesee S.A. - All Rights Reserved                                                                 *
 *                                                                                                                    *
 * Subject to Prophesee Metavision Licensing Terms and Conditions ("License T&C's").                                  *
 * You may not use this file except in compliance with these License T&C's.                                           *
 * A copy of these License T&C's is located in the "licensing" folder accompanying this file.                         *
 **********************************************************************************************************************/

#ifndef METAVISION_APPS_BLINKING_DOTS_GRID_DETECTOR_H
#define METAVISION_APPS_BLINKING_DOTS_GRID_DETECTOR_H

#include <functional>
#include <memory>
#include <opencv2/core/core.hpp>
#include <metavision/sdk/base/utils/timestamp.h>
#include <metavision/sdk/calibration/algorithms/blinking_dots_grid_detector_algorithm.h>

namespace Metavision {

struct CalibrationDetectionResult;

/// @brief Class that detects a grid of blinking dots with specific frequencies.
/// Uses the class @ref BlinkingDotsGridDetectorAlgorithm.
///
class BlinkingDotsGridDetector {
public:
    /// @brief Type for callback called after each asynchronous process.
    using OutputCb = std::function<void(timestamp, CalibrationDetectionResult &)>;

    /// @brief Constructor
    /// @param width Sensor's width
    /// @param height Sensor's height
    /// @param config Blinking frame generator configuration, see @ref BlinkingDotsGridDetectorAlgorithmConfig
    /// @param skip_time_us Minimum time interval between two produced detections. 2s by default
    BlinkingDotsGridDetector(int width, int height, const BlinkingDotsGridDetectorAlgorithmConfig &config,
                             timestamp skip_time_us = 2e6);

    /// @brief Destructor.
    ~BlinkingDotsGridDetector();

    /// @brief Processes a buffer of events
    /// @tparam InputIt Read-Only input event iterator type. Works for iterators over buffers of @ref EventCD
    /// or equivalent
    /// @param it_begin Iterator to the first input event
    /// @param it_end Iterator to the past-the-end event
    template<typename InputIt>
    void process_events(InputIt it_begin, InputIt it_end);

    /// @brief Sets a callback to get the detection results.
    /// The callback provides:
    /// * Timestamp of grid detection.
    /// * An image showing the location of the events used to detect the grid.
    /// * An std::vector with the points in the grid (empty if the grid was not correctly detected).
    /// @note The generated struct will be passed to the callback as a non constant
    /// reference, the user is free to copy or swap it.
    /// @param output_cb Function to be set as callback.
    void set_output_callback(const OutputCb &output_cb);

private:
    /// @brief Adapts the output from @ref BlinkingDotsGridDetectorAlgorithm into a suitable @ref
    /// CalibrationdDetectionResult.
    void on_blinking_pattern(timestamp ts, cv::Mat &frame, std::vector<cv::Point2f> &grid_points);

    /// Algorithm to get the blinking frame
    std::unique_ptr<BlinkingDotsGridDetectorAlgorithm> algo_;

    /// Output result.
    CalibrationDetectionResult output_calib_results_;
    OutputCb output_cb_;
    timestamp last_ts_;
    const timestamp skip_time_us_;
};

} // namespace Metavision

#endif // METAVISION_APPS_BLINKING_DOTS_GRID_DETECTOR_H
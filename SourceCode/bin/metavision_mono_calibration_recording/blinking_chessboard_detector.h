/**********************************************************************************************************************
 * Copyright (c) Prophesee S.A. - All Rights Reserved                                                                 *
 *                                                                                                                    *
 * Subject to Prophesee Metavision Licensing Terms and Conditions ("License T&C's").                                  *
 * You may not use this file except in compliance with these License T&C's.                                           *
 * A copy of these License T&C's is located in the "licensing" folder accompanying this file.                         *
 **********************************************************************************************************************/

#ifndef METAVISION_APPS_BLINKING_CHESSBOARD_DETECTOR_H
#define METAVISION_APPS_BLINKING_CHESSBOARD_DETECTOR_H

#include <functional>
#include <memory>
#include <opencv2/core/core.hpp>
#include <metavision/sdk/base/utils/timestamp.h>
#include <metavision/sdk/calibration/algorithms/blinking_frame_generator_algorithm.h>

namespace Metavision {

/// @brief Class that detects a blinking chessboard on events using the class @ref BlinkingFrameGeneratorAlgorithm
/// to produce a binary image of the blinking chessboard and OpenCV to find the corners on it.
///
class BlinkingChessBoardDetector {
public:
    /// Type for callback called after each asynchronous process.
    using OutputCb = std::function<void(timestamp, CalibrationDetectionResult &)>;

    /// @brief Constructor
    /// @param sensor_width Sensor's width
    /// @param sensor_height Sensor's height
    /// @param cols Number of horizontal lines
    /// @param rows Number of vertical lines
    /// @param config Blinking frame generator configuration
    /// @param skip_time_us Minimum time interval between two produced detections. 2s by default
    /// @param debug Enable debug mode to display intermediate blinking images
    BlinkingChessBoardDetector(int sensor_width, int sensor_height, int cols, int rows,
                               const BlinkingFrameGeneratorAlgorithmConfig &config, timestamp skip_time_us = 2e6,
                               bool debug = false);

    /// @brief Destructor.
    ~BlinkingChessBoardDetector();

    /// @brief Processes a buffer of events
    /// @tparam InputIt Read-Only input event iterator type. Works for iterators over buffers of @ref EventCD
    /// or equivalent
    /// @param it_begin Iterator to the first input event
    /// @param it_end Iterator to the past-the-end event
    template<typename InputIt>
    void process_events(InputIt it_begin, InputIt it_end);

    /// @brief Sets a callback to get the detection results.
    /// @param output_cb Function to be set as callback.
    /// @note The generated struct will be passed to the callback as a non constant reference, meaning that the
    /// user is free to copy it or swap it using std::swap. In case of a swap with a struct containing a non initialized
    /// image, it will be automatically initialized.
    void set_output_callback(const OutputCb &output_cb);

private:
    /// @brief Finds the corners using OpenCV and calls the output callback if the chessboard has been successfully
    /// found (Binary frame with 0 or 255)
    /// @note The generated image will be passed to the callback as a non constant reference, meaning that the
    /// class is free to copy it or swap it. In case of a swap with a non initialized image, it will be automatically
    /// initialized
    void on_blinking_frame(timestamp ts, cv::Mat &cb_frame);

    /// Algorithm to get the blinking frame
    std::unique_ptr<BlinkingFrameGeneratorAlgorithm> algo_;

    // Camera's Geometry
    const int width_;
    const int height_;

    // Pattern's Geometry
    const int cols_;
    const int rows_;
    const cv::Size grid_size_;

    // Parameters
    const bool use_inverted_gray_;
    const float cos_max_angle_;

    // Temporary image containers
    cv::Mat frame_, new_frame_;
    cv::Mat1b dst_;

    // Output Result
    CalibrationDetectionResult output_calib_results_;
    OutputCb output_cb_;
    timestamp last_ts_;
    const timestamp skip_time_us_;

    bool debug_;
};

} // namespace Metavision

#endif // METAVISION_APPS_BLINKING_CHESSBOARD_DETECTOR_H
/**********************************************************************************************************************
 * Copyright (c) Prophesee S.A. - All Rights Reserved                                                                 *
 *                                                                                                                    *
 * Subject to Prophesee Metavision Licensing Terms and Conditions ("License T&C's").                                  *
 * You may not use this file except in compliance with these License T&C's.                                           *
 * A copy of these License T&C's is located in the "licensing" folder accompanying this file.                         *
 **********************************************************************************************************************/

// Tool for intrinsics camera calibration from a blinking Chessboard, using Metavision Calibration SDK.

#include <functional>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <metavision/sdk/base/utils/log.h>
#include <metavision/sdk/core/pipeline/pipeline.h>
#include <metavision/sdk/core/pipeline/frame_generation_stage.h>
#include <metavision/sdk/core/pipeline/frame_composition_stage.h>
#include <metavision/sdk/driver/pipeline/camera_stage.h>
#include <metavision/sdk/cv/algorithms/trail_filter_algorithm.h>
#include <metavision/sdk/calibration/utils/calibration_grid_pattern.h>
#include <metavision/sdk/calibration/utils/calibration_detection_frame_generator.h>
#include <metavision/sdk/calibration/utils/recorded_pattern_serializer.h>
#include <metavision/sdk/ui/utils/event_loop.h>
#include <metavision/sdk/ui/pipeline/frame_display_stage.h>

#include "blinking_chessboard_detector.h"
#include "blinking_dots_grid_detector.h"

namespace po  = boost::program_options;
namespace bfs = boost::filesystem;

using EventBuffer    = std::vector<Metavision::EventCD>;
using EventBufferPtr = Metavision::SharedObjectPool<EventBuffer>::ptr_type;

using FramePool = Metavision::SharedObjectPool<cv::Mat>;
using FramePtr  = FramePool::ptr_type;
using FrameData = std::pair<Metavision::timestamp, FramePtr>;

using CalibResultsPool = Metavision::SharedObjectPool<Metavision::CalibrationDetectionResult>;
using CalibResultsPtr  = CalibResultsPool::ptr_type;
using CalibResultsData = std::pair<Metavision::timestamp, CalibResultsPtr>;

// Enum defining the type of pattern to use.
enum class PatternType { BlinkingChessBoard, BlinkingDots };

// These two operator overloads are required for the enum to work with boost::program_options.
std::istream &operator>>(std::istream &in, PatternType &pattern_type) {
    std::string token;
    in >> token;
    if (token == "CHESSBOARD")
        pattern_type = PatternType::BlinkingChessBoard;
    else if (token == "DOTS")
        pattern_type = PatternType::BlinkingDots;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}

std::ostream &operator<<(std::ostream &out, const PatternType &pattern_type) {
    if (pattern_type == PatternType::BlinkingChessBoard)
        out << "CHESSBOARD";
    else
        out << "DOTS";
    return out;
}

/// @brief Stage that detects a blinking chessboard on events and produces a binary image of the blinking chessboard and
/// a vector of its corner coordinates.
///   - Input : buffer of events    : EventBufferPtr
///   - Output: calibration results : CalibResultsData
class BlinkingChessBoardDetectorStage : public Metavision::BaseStage {
public:
    BlinkingChessBoardDetectorStage(int width, int height, int cols, int rows,
                                    const Metavision::BlinkingFrameGeneratorAlgorithmConfig &blinking_config,
                                    Metavision::timestamp skip_time_us, bool debug) :
        calib_pool_(CalibResultsPool::make_bounded()) {
        algo_ = std::make_unique<Metavision::BlinkingChessBoardDetector>(width, height, cols, rows, blinking_config,
                                                                         skip_time_us, debug);

        set_consuming_callback([this](const boost::any &data) {
            try {
                auto buffer = boost::any_cast<EventBufferPtr>(data);
                if (buffer->empty())
                    return;
                successful_cb_ = false;
                algo_->process_events(buffer->cbegin(), buffer->cend());
                if (!successful_cb_)
                    produce(std::make_pair(buffer->crbegin()->t, CalibResultsPtr())); // Temporal marker
            } catch (boost::bad_any_cast &c) { MV_LOG_ERROR() << c.what(); }
        });

        algo_->set_output_callback(
            [this](Metavision::timestamp ts, Metavision::CalibrationDetectionResult &pattern_detection) {
                auto output_ptr = calib_pool_.acquire();
                std::swap(*output_ptr, pattern_detection);
                successful_cb_ = true;
                produce(std::make_pair(ts, output_ptr));
            });
    }

private:
    CalibResultsPool calib_pool_;
    std::unique_ptr<Metavision::BlinkingChessBoardDetector> algo_;
    bool successful_cb_;
};

/// @brief Stage that detects a blinking grid of dots.
///   - Input : buffer of events    : EventBufferPtr
///   - Output: calibration results : CalibResultsData
class BlinkingDotsGridDetectorStage : public Metavision::BaseStage {
public:
    BlinkingDotsGridDetectorStage(int width, int height,
                                  const Metavision::BlinkingDotsGridDetectorAlgorithmConfig &config,
                                  Metavision::timestamp skip_time_us) {
        algo_ = std::make_unique<Metavision::BlinkingDotsGridDetector>(width, height, config, skip_time_us);

        /// [PATTERN_DETECTOR_SET_CONSUMING_CALLBACK_BEGIN]
        set_consuming_callback([this](const boost::any &data) {
            try {
                auto buffer = boost::any_cast<EventBufferPtr>(data);
                if (buffer->empty())
                    return;
                successful_cb_ = false;
                algo_->process_events(buffer->cbegin(), buffer->cend());
                if (!successful_cb_)
                    produce(std::make_pair(buffer->crbegin()->t, CalibResultsPtr())); // Temporal marker
            } catch (boost::bad_any_cast &c) { MV_LOG_ERROR() << c.what(); }
        });
        /// [PATTERN_DETECTOR_SET_CONSUMING_CALLBACK_END]

        /// [PATTERN_DETECTOR_SET_OUTPUT_CALLBACK_BEGIN]
        algo_->set_output_callback(
            [this](Metavision::timestamp ts, Metavision::CalibrationDetectionResult &pattern_detection) {
                auto output_ptr = calib_pool_.acquire();
                std::swap(*output_ptr, pattern_detection);
                successful_cb_ = true;
                produce(std::make_pair(ts, output_ptr));
            });
        /// [PATTERN_DETECTOR_SET_OUTPUT_CALLBACK_END]
    }

private:
    CalibResultsPool calib_pool_;
    std::unique_ptr<Metavision::BlinkingDotsGridDetector> algo_;
    bool successful_cb_;
};

/// @brief Stage that stores the 2D detections of the rigid 3D pattern to compute the intrinsic camera parameters
///
/// It outputs the 2D detections and the 3D pattern in a JSON file at the end of the pipeline.
///   - Input  : calibration results : CalibResultsData
class DetectionsLoggerStage : public Metavision::BaseStage {
public:
    DetectionsLoggerStage(int width, int height, const Metavision::CalibrationGridPattern &pattern,
                          PatternType pattern_type, const std::string &output_dir_path, bool use_saved_points) :
        img_size_(width, height), pattern_3d_(pattern), pattern_type_(pattern_type) {
        json_path_ = (bfs::path(output_dir_path) / "recorded_pattern.json").string();

        if (use_saved_points)
            load_dumped_points();

        /// [DETECTIONS_LOGGER_STAGE_SET_CONSUMING_CALLBACK_BEGIN]
        set_consuming_callback([this](const boost::any &data) {
            try {
                auto ts_calib_results     = boost::any_cast<CalibResultsData>(data);
                auto &input_calib_results = ts_calib_results.second;
                if (input_calib_results) {
                    pts_2d_.emplace_back(input_calib_results->keypoints_);
                    if (pts_2d_.size() == 50) {
                        MV_LOG_INFO() << "Got 50 calibration patterns... This is enough to calibrate, press 'q' to"
                                         "exit the application or continue acquiring more patterns.";
                    } else {
                        MV_LOG_INFO() << "Got" << pts_2d_.size() << Metavision::Log::no_space << "calibration pattern"
                                      << (pts_2d_.size() > 1 ? "s" : "") << " ...";
                    }
                }
            } catch (boost::bad_any_cast &c) { MV_LOG_ERROR() << c.what(); }
        });
        /// [DETECTIONS_LOGGER_STAGE_SET_CONSUMING_CALLBACK_END]
    }

    ~DetectionsLoggerStage() {
        if (pts_2d_.empty()) {
            MV_LOG_WARNING() << "No patterns have been detected. Here are some suggestions on how to perform a"
                                "successful detection:";
            if (pattern_type_ == PatternType::BlinkingChessBoard) {
                MV_LOG_WARNING() << "Run with --debug option to visualize the intermediate blinking frames.";
                MV_LOG_WARNING() << " 1) Specify the correct pattern geometry (--cols, --rows, --pattern-type)";
                MV_LOG_WARNING() << " 2) The chessboard must be in the field of view of the camera.";
                MV_LOG_WARNING() << " 3) The chessboard must blink. Don't juste move a static chessboard"
                                 << "in front of the camera.";
                MV_LOG_WARNING() << " 4) The camera must be static at each view. Ideally with the help of a tripod.";
                MV_LOG_WARNING() << " 5) Increase the accumulation time (-a). Decrease the minimum number "
                                    "of pixels (-m). Increase the on/off ratios (--ratio-on, -ratio-off) to 1.0.";
            } else {
                MV_LOG_WARNING() << " 1) Specify the correct pattern geometry (--cols, --rows, --pattern-type)";
                MV_LOG_WARNING() << " 2) The LED grid must be in the field of view of the camera.";
                MV_LOG_WARNING() << " 3) The camera must be static at each view. Ideally with the help of a tripod.";
                MV_LOG_WARNING() << " 4) Specify the correct frequency properties (--normal-freq, --special-freq)";
                MV_LOG_WARNING() << " 5) Decrease the minimum size of a blinking dot (--min-dot-size).";
            }
        } else if (Metavision::write_patterns_to_file(json_path_, img_size_, pts_2d_, pattern_3d_))
            MV_LOG_INFO() << "The 3D pattern geometry and the 2D detections have been saved in" << json_path_;
        else
            MV_LOG_WARNING() << "Failed to dump 2D observations to json file.";
    }

    void load_dumped_points() {
        cv::Size previous_img_size;
        std::vector<std::vector<cv::Point2f>> previous_pts_2d;
        Metavision::CalibrationGridPattern previous_pattern;
        if (!Metavision::read_patterns_from_file(json_path_, previous_img_size, previous_pts_2d, previous_pattern))
            throw std::invalid_argument("Failed to get 2D observations from json file.");

        if (previous_img_size != img_size_) {
            std::stringstream ss;
            ss << "Invalid image size in the json to load. " << previous_img_size << " instead of " << img_size_ << ".";
            throw std::invalid_argument(ss.str());
        }

        if (previous_pattern.n_cols_ != pattern_3d_.n_cols_ || previous_pattern.n_rows_ != pattern_3d_.n_rows_ ||
            previous_pattern.n_pts_ != pattern_3d_.n_pts_) {
            std::stringstream ss;
            ss << "Invalid pattern geometry in the json to load. Must be "
               << cv::Size(pattern_3d_.n_cols_, pattern_3d_.n_rows_) << ".";
            throw std::invalid_argument(ss.str());
        }

        if (previous_pts_2d.end() !=
            std::find_if(previous_pts_2d.begin(), previous_pts_2d.end(),
                         [&](const std::vector<cv::Point2f> &vec) { return vec.size() != pattern_3d_.n_pts_; })) {
            std::stringstream ss;
            ss << "Invalid 2D observations in the json to load. Must be vectors of size " << pattern_3d_.n_pts_ << ".";
            throw std::invalid_argument(ss.str());
        }

        // Use 2d observations
        std::swap(previous_pts_2d, pts_2d_);
    }

private:
    const cv::Size img_size_;
    const Metavision::CalibrationGridPattern pattern_3d_;
    const PatternType pattern_type_;
    std::vector<std::vector<cv::Point2f>> pts_2d_;
    std::string json_path_;
};

/// @brief Stage that generates images from calibration results.
///
/// It combines the image of the calibration pattern, the keypoints and a colored overlay that shows which regions
/// have been well covered during the calibration.
///
///   - Input : calibration results                   : CalibResultsData
///   - Output: timestamped frame  (Chessboard Frame) : FrameData
class PatternFrameGeneratorStage : public Metavision::BaseStage {
public:
    PatternFrameGeneratorStage(int width, int height, int cols, int rows, const std::string &output_images_dir_path,
                               bool overlay_convex_hull) :
        frame_pool_(FramePool::make_bounded()) {
        frame_id_      = 1;
        export_frames_ = (output_images_dir_path != "");
        if (export_frames_) {
            base_frames_path_ = (bfs::path(output_images_dir_path) / "pattern_").string();
        }

        display_algo_ = std::make_unique<Metavision::CalibrationDetectionFrameGenerator>(
            width, height, cols, rows, Metavision::CalibrationDetectionFrameGenerator::PatternMode::Chessboard,
            overlay_convex_hull);

        /// [PATTERN_FRAME_GENERATOR_SET_CONSUMING_CALLBACK_BEGIN]
        set_consuming_callback([this](const boost::any &data) {
            try {
                auto ts_calib_results = boost::any_cast<CalibResultsData>(data);
                auto output_frame_ptr = frame_pool_.acquire();

                const auto &input_ts            = ts_calib_results.first;
                const auto &input_calib_results = ts_calib_results.second;
                if (!input_calib_results) {
                    produce(std::make_pair(input_ts, FramePtr())); // Temporal marker
                    return;
                }
                if (export_frames_) {
                    std::stringstream ss;
                    ss << base_frames_path_ << frame_id_++ << ".png";
                    cv::imwrite(ss.str(), input_calib_results->frame_);
                }

                display_algo_->generate_bgr_img(*output_frame_ptr, *input_calib_results);
                produce(std::make_pair(input_ts, output_frame_ptr));
            } catch (boost::bad_any_cast &c) { MV_LOG_ERROR() << c.what(); }
        });
        /// [PATTERN_FRAME_GENERATOR_SET_CONSUMING_CALLBACK_END]
    }

    ~PatternFrameGeneratorStage() {
        if (export_frames_) {
            MV_LOG_INFO() << Metavision::Log::no_space << "Images of the 2D detections have been saved as "
                          << base_frames_path_ << "*.png";
        }
    }

private:
    bool export_frames_;
    std::string base_frames_path_;
    int frame_id_;
    cv::Mat export_mat_;

    std::unique_ptr<Metavision::CalibrationDetectionFrameGenerator> display_algo_;
    FramePool frame_pool_;
};

// Application's parameters.
struct Config {
    // General parameters.
    std::string raw_file_path_;
    std::string biases_file_;
    std::string output_dir_path_;
    Metavision::timestamp accumulation_time_;
    Metavision::timestamp skip_time_;
    PatternType pattern_type_;
    bool use_saved_points_;
    bool overlay_convex_hull_;
    bool disable_images_export_;
    std::string output_images_dir_path_ = ""; // output_dir_path_/pattern_images

    // Grid Geometry
    int cols_;
    int rows_;
    float distance_between_cols_;
    float distance_between_rows_;

    // Blinking frame generator algorithm's parameters.
    int min_num_blinking_pixels_;
    float blinking_pixels_ratios_on_;
    float blinking_pixels_ratios_off_;

    // Blinking dots grid parameters.
    float normal_freq_;
    float special_freq_;
    int period_diff_thresh_us_;
    int frequency_filter_length_;
    float max_freq_diff_;
    int min_dot_size_;
    bool use_fisheye_;

    bool debug_;
};

bool get_pipeline_configuration(int argc, char *argv[], Config &config) {
    const std::string short_program_desc("Tool showing how to use Metavision Calibration SDK to collect 2D detections "
                                         "of a rigid 3D pattern (blinking chessboard or blinking dots grid) for the "
                                         "calibration of the intrinsic parameters of the camera.\n");
    const std::string long_program_desc(short_program_desc + "Press 'q' or Escape key to leave the program.\n");

    po::options_description options_desc;
    po::options_description base_options("Base options");
    // clang-format off
    base_options.add_options()
        ("help,h", "Produce help message.")
        ("input-raw-file,i",      po::value<std::string>(&config.raw_file_path_), "Path to input RAW file. If not specified, the camera live stream is used.")
        ("biases,b",              po::value<std::string>(&config.biases_file_), "Path to a biases file. If not specified, the camera will be configured with the default biases.")
        ("output-dir,o",          po::value<std::string>(&config.output_dir_path_)->default_value("/tmp/mono_calibration"), "Path to the folder where the 2D detections will be saved. If the folder does not exists, it will be created.")
        ("accumulation-time,a",   po::value<Metavision::timestamp>(&config.accumulation_time_)->default_value(1e5), "Defines the period (in us) for detection of the blinking pattern (chessboard or dots grid).")
        ("skip-time",             po::value<Metavision::timestamp>(&config.skip_time_)->default_value(2e6), "Minimum time interval (in us) between two produced detections (chessboard or dots grid).")
        ("pattern-type",          po::value<PatternType>(&config.pattern_type_)->default_value(PatternType::BlinkingChessBoard), "Type of calibration pattern to use: CHESSBOARD or DOTS.")
        ("use-saved-points,u",    po::bool_switch(&config.use_saved_points_)->default_value(false), "Whether or not we should use the saved points from previous recording.")
        ("disable-images-export", po::bool_switch(&config.disable_images_export_)->default_value(false), "Whether or not we should export the images of the detected patterns.")
        ("overlay-convex-hull",   po::bool_switch(&config.overlay_convex_hull_)->default_value(false), "If specified, overlay a shadow over the convex hull covering all the dots "
                                                                                                        "in the pattern. Otherwise, draw only a circle over each dot.")
        ("debug",                 po::bool_switch(&config.debug_)->default_value(false), "Enable debug mode to display intermediate images in case of a blinking chessboard.")
        ;
    // clang-format on

    po::options_description grid_geometry("Grid geometry options");
    // clang-format off
    grid_geometry.add_options()
        ("cols",    po::value<int>(&config.cols_)->default_value(9), "Number of targets (LEDs or checkerboard corners) in width (by convention the largest number).")
        ("rows",    po::value<int>(&config.rows_)->default_value(6), "Number of targets (LEDs or checkerboard corners) in height (by convention the smallest number).")
        ("distance-between-cols", po::value<float>(&config.distance_between_cols_)->default_value(0.02f),
                                  "Distance between two consecutive columns in the LED grid or chessboard pattern (in meters).")
        ("distance-between-rows", po::value<float>(&config.distance_between_rows_)->default_value(0.02f),
                                  "Distance between two consecutive rows in the LED grid or chessboard pattern (in meters).")
        ("fisheye", po::bool_switch(&config.use_fisheye_)->default_value(false), "Allows the detection of strongly distorted grids, as when using a fisheye lens.")
        ;
    // clang-format on

    po::options_description blinking_frame_generator_options("Blinking Frame Generator options");
    // clang-format off
    blinking_frame_generator_options.add_options()
        ("min-blink-pix,m", po::value<int>(&config.min_num_blinking_pixels_)->default_value(100), "Minimum number of pixels needed to be detected before outputting a frame.")
        ("ratio-on",        po::value<float>(&config.blinking_pixels_ratios_on_)->default_value(0.15f), "The acceptable ratio of pixels that received only positive events over the number of pixels that received both during the accumulation window.")
        ("ratio-off",       po::value<float>(&config.blinking_pixels_ratios_off_)->default_value(0.15f),  "The acceptable ratio of pixels that received only negative events over the number of pixels that received both during the accumulation window.")
        ;
    // clang-format on

    po::options_description blinking_dots_grid_options("Blinking dots grid options");
    // clang-format off
    blinking_dots_grid_options.add_options()
        ("normal-freq",             po::value<float>(&config.normal_freq_)->default_value(125.f), "Blinking frequency of the dots in the grid.")
        ("special-freq",            po::value<float>(&config.special_freq_)->default_value(166.f), "Blinking frequency of the dots in the first row of the grid.")
        ("period-diff-threshold",   po::value<int>(&config.period_diff_thresh_us_)->default_value(2000), "For a given pixel, maximum difference between successive periods to be considered the same, in us.")
        ("frequency-filter-length", po::value<int>(&config.frequency_filter_length_)->default_value(7),  "Number of successive blinks with the same frequency to validate a blinking pixel.")
        ("max-freq-diff",           po::value<float>(&config.max_freq_diff_)->default_value(10),  "Maximum frequency difference between pixels of the same dot.")
        ("min-dot-size",            po::value<int>(&config.min_dot_size_)->default_value(20),  "Minimum size of a blinking dot, in pixels.")
        ;
    // clang-format on

    options_desc.add(base_options)
        .add(grid_geometry)
        .add(blinking_frame_generator_options)
        .add(blinking_dots_grid_options);

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(options_desc).run(), vm);
        po::notify(vm);
    } catch (po::error &e) {
        MV_LOG_ERROR() << short_program_desc;
        MV_LOG_ERROR() << options_desc;
        MV_LOG_ERROR() << "Parsing error:" << e.what();
        return false;
    }

    if (vm.count("help")) {
        MV_LOG_INFO() << short_program_desc;
        MV_LOG_INFO() << options_desc;
        return false;
    }

    if (!bfs::exists(config.output_dir_path_)) {
        try {
            bfs::create_directories(config.output_dir_path_);
        } catch (bfs::filesystem_error &e) {
            MV_LOG_ERROR() << "Unable to create folder" << config.output_dir_path_;
            return false;
        }
    }

    if (!config.disable_images_export_) {
        config.output_images_dir_path_ = (bfs::path(config.output_dir_path_) / "pattern_images").string();
        if (!bfs::exists(config.output_images_dir_path_)) {
            try {
                bfs::create_directories(config.output_images_dir_path_);
            } catch (bfs::filesystem_error &e) {
                MV_LOG_ERROR() << "Unable to create folder" << config.output_images_dir_path_;
                return false;
            }
        }
    }

    MV_LOG_INFO() << long_program_desc;

    return true;
}

int main(int argc, char *argv[]) {
    Config conf_;

    if (!get_pipeline_configuration(argc, argv, conf_))
        return 1;

    const auto start = std::chrono::high_resolution_clock::now();

    Metavision::Pipeline p(true);

    Metavision::Camera camera;
    if (conf_.raw_file_path_.empty()) {
        try {
            camera = Metavision::Camera::from_first_available();
            if (!conf_.biases_file_.empty()) {
                MV_LOG_INFO() << "Setting camera biases from" << conf_.biases_file_;
                camera.biases().set_from_file(conf_.biases_file_);
            }
        } catch (const Metavision::CameraException &e) {
            MV_LOG_ERROR() << e.what();
            return 1;
        }
    } else {
        camera = Metavision::Camera::from_file(conf_.raw_file_path_, false);
    }

    const unsigned short width  = camera.geometry().width();
    const unsigned short height = camera.geometry().height();

    const Metavision::timestamp event_buffer_duration_ms = 100;
    const int display_fps                                = 10;

    // Pipeline
    //
    //  0 (Camera) --->--- 1 (Trail) ------------>------------ 2 (Pattern detector, either:
    //                     |                                   |  A. blinking chessboard
    //                     v                                   v  B. blinking dot grid)
    //                     |                                   |
    //                     v                                   |------------>-----------|
    //                     |                                   |                        |
    //                     v                                   v                        v
    //                     |                                   |                        |
    //                     5 (Frame Gen)                       4 (Pattern Frame Gen)    3 (Detections Logger)
    //                     |                                   |
    //                     v                                   v
    //                     |---->--  6 (Frame Composer) ---<---|
    //                               |
    //                               v
    //                               |
    //                               7 (Display)

    // 0) Camera stage
    auto &cam_stage =
        p.add_stage(std::make_unique<Metavision::CameraStage>(std::move(camera), event_buffer_duration_ms));

    // 1) Trail Filter Stage
    auto &trail_stage =
        p.add_algorithm_stage(std::make_unique<Metavision::TrailFilterAlgorithm>(width, height, 1e6), cam_stage);

    // 2) Calibration pattern detection stage.
    std::unique_ptr<Metavision::BaseStage> pattern_detection_stage_ptr;

    // 2A) Blinking pattern detector algorithm stage, or...
    if (conf_.pattern_type_ == PatternType::BlinkingChessBoard) {
        Metavision::BlinkingFrameGeneratorAlgorithmConfig blinking_config(
            conf_.accumulation_time_, conf_.min_num_blinking_pixels_, conf_.blinking_pixels_ratios_on_,
            conf_.blinking_pixels_ratios_off_);

        pattern_detection_stage_ptr = std::make_unique<BlinkingChessBoardDetectorStage>(
            width, height, conf_.cols_, conf_.rows_, blinking_config, conf_.skip_time_, conf_.debug_);
    }
    // 2B) ...blinking dots pattern detector algorithm stage.
    else {
        Metavision::BlinkingDotsGridDetectorAlgorithmConfig blinking_config;
        blinking_config.processing_timestep        = conf_.accumulation_time_;
        blinking_config.num_rows                   = conf_.rows_;
        blinking_config.num_cols                   = conf_.cols_;
        blinking_config.distance_between_cols      = conf_.distance_between_cols_;
        blinking_config.distance_between_rows      = conf_.distance_between_rows_;
        blinking_config.special_freq               = conf_.special_freq_;
        blinking_config.normal_freq                = conf_.normal_freq_;
        blinking_config.period_diff_thresh_us      = conf_.period_diff_thresh_us_;
        blinking_config.frequency_filter_length    = conf_.frequency_filter_length_;
        blinking_config.max_cluster_frequency_diff = conf_.max_freq_diff_;
        blinking_config.min_cluster_size           = conf_.min_dot_size_;
        blinking_config.fisheye                    = conf_.use_fisheye_;

        pattern_detection_stage_ptr =
            std::make_unique<BlinkingDotsGridDetectorStage>(width, height, blinking_config, conf_.skip_time_);
    }
    auto &pattern_detection_stage = p.add_stage(std::move(pattern_detection_stage_ptr), trail_stage);

    // 3) Detections Logger stage
    Metavision::CalibrationGridPattern calib_pattern(conf_.cols_, conf_.rows_, conf_.distance_between_rows_,
                                                     conf_.distance_between_cols_);
    auto &logger_stage =
        p.add_stage(std::make_unique<DetectionsLoggerStage>(width, height, calib_pattern, conf_.pattern_type_,
                                                            conf_.output_dir_path_, conf_.use_saved_points_),
                    pattern_detection_stage);

    // 4) Pattern frame generator stage
    auto &pattern_frame_stage = p.add_stage(
        std::make_unique<PatternFrameGeneratorStage>(width, height, conf_.cols_, conf_.rows_,
                                                     conf_.output_images_dir_path_, conf_.overlay_convex_hull_),
        pattern_detection_stage);

    // 5) Events frame stage
    auto &events_frame_stage = p.add_stage(
        std::make_unique<Metavision::FrameGenerationStage>(width, height, event_buffer_duration_ms, display_fps),
        trail_stage);

    // 6) Frame composer stage
    auto &full_frame_stage = p.add_stage(std::make_unique<Metavision::FrameCompositionStage>(display_fps, 0));
    full_frame_stage.add_previous_frame_stage(events_frame_stage, 0, 0, width, height);
    full_frame_stage.add_previous_frame_stage(pattern_frame_stage, width + 10, 0, width, height);

    // 7) Stage displaying the combined frame
    const std::string display_title =
        (conf_.pattern_type_ == PatternType::BlinkingChessBoard ? "CD & Blinking Chessboard" :
                                                                  "CD & Blinking Dots Grid");
    auto &disp_stage = p.add_stage(std::make_unique<Metavision::FrameDisplayStage>(display_title, width * 2, height),
                                   full_frame_stage);

    disp_stage.set_key_callback([&](Metavision::UIKeyEvent key, int scancode, Metavision::UIAction action, int mods) {
        if (action == Metavision::UIAction::RELEASE)
            if (key == Metavision::UIKeyEvent::KEY_ESCAPE || key == Metavision::UIKeyEvent::KEY_Q)
                p.cancel();
    });

    // Run the pipeline and wait for its completion
    p.run();

    const auto end     = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    MV_LOG_INFO() << "Ran in" << static_cast<float>(elapsed.count()) / 1000.f << "s" << std::endl;

    MV_LOG_INFO() << "Now run metavision_mono_calibration to calibrate the camera from the pattern detections."
                  << std::endl;

    return 0;
}

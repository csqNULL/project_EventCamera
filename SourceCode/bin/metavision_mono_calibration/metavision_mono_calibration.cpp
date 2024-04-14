/**********************************************************************************************************************
 * Copyright (c) Prophesee S.A. - All Rights Reserved                                                                 *
 *                                                                                                                    *
 * Subject to Prophesee Metavision Licensing Terms and Conditions ("License T&C's").                                  *
 * You may not use this file except in compliance with these License T&C's.                                           *
 * A copy of these License T&C's is located in the "licensing" folder accompanying this file.                         *
 **********************************************************************************************************************/

// Tool for intrinsics camera calibration from a blinking Chessboard, using Metavision Calibration SDK.

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#if CV_MAJOR_VERSION >= 4
#include <opencv2/calib3d/calib3d_c.h>
#endif
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <metavision/sdk/base/utils/log.h>
#include <metavision/sdk/calibration/utils/calibrate_mono_camera.h>
#include <metavision/sdk/calibration/utils/recorded_pattern_serializer.h>
#include <metavision/sdk/cv/utils/camera_geometry_helpers.h>
#include <metavision/sdk/cv/utils/camera_geometry.h>
#include <metavision/sdk/cv/utils/pinhole_camera_model.h>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

enum class CalibrationRefinementMode { NONE, REFINE, REFINE_AND_SHOW_IMAGES };

std::istream &operator>>(std::istream &in, CalibrationRefinementMode &mode) {
    std::string token;
    in >> token;
    if (token == "NONE")
        mode = CalibrationRefinementMode::NONE;
    else if (token == "REFINE")
        mode = CalibrationRefinementMode::REFINE;
    else if (token == "REFINE_AND_SHOW_IMAGES")
        mode = CalibrationRefinementMode::REFINE_AND_SHOW_IMAGES;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}

std::ostream &operator<<(std::ostream &out, const CalibrationRefinementMode &mode) {
    if (mode == CalibrationRefinementMode::REFINE)
        out << "REFINE";
    else if (mode == CalibrationRefinementMode::REFINE_AND_SHOW_IMAGES)
        out << "REFINE_AND_SHOW_IMAGES";
    else
        out << "NONE";
    return out;
}

class Pipeline {
public:
    Pipeline() = default;

    /// @brief Utility function to parse command line attributes
    bool parse_command_line(int argc, char *argv[]);

    /// @brief Runs the calibration pipeline
    bool run();

private:
    /// @brief Loads serialized detections and performs the calibration
    bool calibrate();

    /// @brief Refines calibration by using intrinsics to undistort and unproject images to redetect keypoints
    bool refine_calibration();

    /// @brief Finds corners that are too close from the edge to avoid incorrect subpix refinements
    /// @param input_pts Keypoints detected in the input image
    /// @param fronto_pts Keypoints detected in the undistorted fronto-parallel image
    /// @param view_idx View index
    /// @param corners_close_to_edge Coordinates of the corners considered close to the image edge
    /// @param corners_close_to_edge_indices Indices of corners in @p corners_close_to_edge w.r.t @p fronto_pts
    void find_corners_close_to_the_edge(const std::vector<cv::Point2f> &input_pts,
                                        const std::vector<cv::Point2f> &fronto_pts, int view_idx,
                                        std::vector<cv::Point2f> &corners_close_to_edge,
                                        std::vector<int> &corners_close_to_edge_indices);

    /// @brief Computes reprojection errors and displays them
    void compute_reprojection_errors();

    /// @brief Exports JSON file containing intrinsics, extrinsics and per view reprojection errors
    void export_calibration_results();

    std::string input_dir_path_;
    std::string input_json_path_;             // Deduced from input_dir_path_
    std::string output_calibration_path_;     // Deduced from input_dir_path_
    std::string input_images_base_path_ = ""; // Deduced from input_dir_path_

    float outlier_ths_;
    bool show_reprojection_errors_;
    int error_magnification_;
    std::string errors_window_title_;

    // 2D-3D correspondences
    cv::Size img_size_;
    std::vector<std::vector<cv::Point2f>> pts_2d_;
    Metavision::CalibrationGridPattern pattern_3d_;

    // Calibration
    std::vector<bool> selected_views_;
    std::vector<cv::Vec3d> rvecs_;
    std::vector<cv::Vec3d> tvecs_;
    cv::Mat K_, d_;
    std::vector<float> per_view_rms_reprojection_errors_;
    float overall_rms_reprojection_error_;

    CalibrationRefinementMode refinement_mode_;
    int blur_radius_refinement_;

    Metavision::MonoCalibration::Model camera_model_ = Metavision::MonoCalibration::Model::Pinhole;

    int cv_flag_first_pass_;
    int cv_flag_refinement_;

    bool use_fisheye_;
};

bool Pipeline::parse_command_line(int argc, char *argv[]) {
    const std::string short_program_desc(
        "Tool showing how to use Metavision Calibration SDK to calibrate the intrinsic parameters of the camera.\n");
    const std::string long_program_desc(short_program_desc + "Press 'q' or Escape key to leave the program.\n");

    bpo::options_description options_desc;
    // clang-format off
    options_desc.add_options()
        ("help,h", "Produce help message.")
        ("input-dir,i",                bpo::value<std::string>(&input_dir_path_)->default_value("/tmp/mono_calibration"), "Path to input directory containing the calibration detections.")
        ("outlier-ths,o",              bpo::value<float>(&outlier_ths_)->default_value(2), "Remove the views for which the reprojection error is more than a certain number of times the standard deviation away from the mean."
                                                                                           "Default value of 2 means that views with error above (mean+2*std) are removed. A negative threshold can be used to specify that all views must be kept.")
        ("show-reprojection-errors,e", bpo::bool_switch(&show_reprojection_errors_)->default_value(false), "Whether or not we should show the reprojection errors on top of the images of the detected patterns.")
        ("error-magnification,m",      bpo::value<int>(&error_magnification_)->default_value(10), "Magnification factor used to display reprojection errors that are superimposed on each view.")
        ("refine-calibration,r",       bpo::value<CalibrationRefinementMode>(&refinement_mode_)->default_value(CalibrationRefinementMode::NONE), "Whether or not we should refine the calibration. This approach uses the obtained parameters as a first guess to "
                                       "perform undistortion and unprojection of calibration images to a canonical fronto-parallel plane. This canonical plane is then used to localize the calibration pattern keypoints more accurately and recompute the camera "
                                       "parameters. There are 3 possible modes: NONE, REFINE, REFINE_AND_SHOW_IMAGES. Warning: This method only works for blinking chessboards, not for blinking dots.")
        ("blur-radius-refinement,b",   bpo::value<int>(&blur_radius_refinement_)->default_value(5), "Radius used to blur the images during the refinement step. By default it's 5pixels, i.e. a diameter of 11pixels.")
        ("fisheye,f",                  bpo::bool_switch(&use_fisheye_)->default_value(false), "Whether or not to use the fisheye model instead of the pinhole.")
        ;
    // clang-format on
    bpo::variables_map vm;
    try {
        bpo::store(bpo::command_line_parser(argc, argv).options(options_desc).run(), vm);
        bpo::notify(vm);
    } catch (bpo::error &e) {
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

    const std::string extension = bfs::path(input_dir_path_).extension().string();
    if (extension != "") {
        MV_LOG_ERROR() << "Invalid directory path. Remove the extension" << extension;
        if (extension == ".raw") {
            MV_LOG_ERROR()
                << "This tool uses serialized calibration views and does not directly process RAW recordings.";
            MV_LOG_ERROR() << "Use metavision_mono_calibration_recording instead to acquire pattern detections.";
        }
        return false;
    }

    if (!bfs::exists(input_dir_path_)) {
        MV_LOG_ERROR() << "Input directory doesn't exist.\n" << input_dir_path_;
        return false;
    }

    input_json_path_         = (bfs::path(input_dir_path_) / "recorded_pattern.json").string();
    output_calibration_path_ = (bfs::path(input_dir_path_) / "intrinsics.json").string();

    if (!bfs::exists(input_json_path_)) {
        MV_LOG_ERROR() << "Input directory must contain a \"recorded_pattern.json\" file.";
        return false;
    }

    if (show_reprojection_errors_ || refinement_mode_ != CalibrationRefinementMode::NONE) {
        bfs::path input_images_dir_path = bfs::path(input_dir_path_) / "pattern_images";
        if (!bfs::exists(input_images_dir_path)) {
            MV_LOG_ERROR() << "Input directory must contain a \"pattern_images\" folder.";
            return false;
        }
        input_images_base_path_ = (input_images_dir_path / "pattern_").string();
    }

    if (error_magnification_ < 1) {
        MV_LOG_ERROR() << "The magnification factor used to display reprojection errors must be greater than 1.";
        return false;
    }

    if (blur_radius_refinement_ < 1) {
        MV_LOG_ERROR() << "The radius used to blur the images during the refinement step must be greater than 1.";
        return false;
    }

    if (use_fisheye_) {
        camera_model_       = Metavision::MonoCalibration::Model::Fisheye;
        cv_flag_first_pass_ = cv::fisheye::CALIB_FIX_SKEW | cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
        cv_flag_refinement_ = cv::fisheye::CALIB_FIX_SKEW | cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
    } else {
        camera_model_       = Metavision::MonoCalibration::Model::Pinhole;
        cv_flag_first_pass_ = cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_FIX_K3;
        cv_flag_refinement_ = cv::CALIB_FIX_ASPECT_RATIO;
    }

    std::stringstream ss;
    ss << "Reprojection error (magnified by " << error_magnification_ << ")";
    errors_window_title_ = ss.str();

    MV_LOG_INFO() << long_program_desc;

    return true;
}

bool Pipeline::calibrate() {
    if (!Metavision::read_patterns_from_file(input_json_path_, img_size_, pts_2d_, pattern_3d_)) {
        MV_LOG_ERROR() << "Failed to load the serialized 2d detections" << std::endl;
        return false;
    }

    MV_LOG_INFO() << "Starting Calibration...";
    // First calibration: Fix K3 anf filter out outliers
    overall_rms_reprojection_error_ = Metavision::MonoCalibration::calibrate_opencv(
        pattern_3d_.base_3D_points_, pts_2d_, img_size_, camera_model_, cv_flag_first_pass_, K_, d_, &selected_views_,
        &rvecs_, &tvecs_, outlier_ths_);

    if (refinement_mode_ != CalibrationRefinementMode::NONE) {
        MV_LOG_INFO() << "Starting Calibration Refinement...";
        MV_LOG_INFO() << "(Make sure the calibration images contain blinking chessboards instead of blinking dots)";
        refine_calibration();
    }

    MV_LOG_INFO() << "Calibration done.";
    const int num_kept_views = std::count(selected_views_.begin(), selected_views_.end(), true);
    MV_LOG_INFO() << Metavision::Log::no_space << "Kept " << num_kept_views << "/" << pts_2d_.size() << " views.";
    MV_LOG_INFO() << "RMS reprojection error:" << overall_rms_reprojection_error_ << "pix" << std::endl;

    MV_LOG_INFO() << "Camera matrix" << std::endl << K_;
    MV_LOG_INFO() << "Distortion coefficients" << std::endl << d_ << std::endl;

    return true;
}

bool Pipeline::refine_calibration() {
    using PinholeModel32f    = Metavision::PinholeCameraModel<float>;
    using PinholeGeometry32f = Metavision::CameraGeometry<PinholeModel32f>;

    std::vector<float> K_vec;
    std::vector<float> d_vec;
    for (auto it = K_.begin<double>(); it != K_.end<double>(); ++it)
        K_vec.push_back(*it);
    for (auto it = d_.begin<double>(); it != d_.end<double>(); ++it)
        d_vec.push_back(*it);

    PinholeModel32f pinhole_model(img_size_.width, img_size_.height, K_vec, d_vec);
    PinholeGeometry32f cam_geometry(pinhole_model);

    // Determine the target corners corresponding to the canonical fronto-parallel plane
    const float margin_y = 0.1 * img_size_.height;
    const float margin_x = 0.1 * img_size_.width;

    const std::vector<cv::Point2f> fronto_undist_inner_corners{
        {margin_x, margin_y},                                              // Top left
        {img_size_.width - 1 - margin_x, margin_y},                        // Top right
        {img_size_.width - 1 - margin_x, img_size_.height - 1 - margin_y}, // Bottom right
        {margin_x, img_size_.height - 1 - margin_y}};                      // Bottom left

    cv::Mat_<float> mapx, mapy;
    mapx.create(img_size_);
    mapy.create(img_size_);

    cv::Mat img, img_fronto_undist, img_bgr, img_undist_bgr, concat_bgr;

    std::vector<std::vector<cv::Point2f>> refined_pts_2d;
    using SizeType = std::vector<bool>::size_type;

    for (SizeType i = 0; i < selected_views_.size(); ++i) {
        const auto &input_pts = pts_2d_[i];
        // 1) Compute undistorted inner corners of the pattern
        const std::vector<cv::Point2f> inner_corners{
            input_pts.front(),                                    // Top left
            input_pts[pattern_3d_.n_cols_ - 1],                   // Top right
            input_pts.back(),                                     // Bottom right
            input_pts[pattern_3d_.n_pts_ - pattern_3d_.n_cols_]}; // Bottom left

        std::vector<cv::Point2f> undist_inner_corners(4);
        for (size_t i = 0; i < 4; ++i) {
            Eigen::Vector2f v1, v2;
            v1(0) = inner_corners[i].x;
            v1(1) = inner_corners[i].y;
            cam_geometry.img_to_undist_norm(v1, v2);
            cam_geometry.undist_norm_to_undist_img(v2, v1);

            undist_inner_corners[i] = {v1(0), v1(1)};
        }

        // 2) Compute the maps that will be used to both undistort and unproject to the fronto-parallel plane
        // H: fronto undist plane -> undist image
        const cv::Mat_<float> H = cv::findHomography(fronto_undist_inner_corners, undist_inner_corners, 0);
        Metavision::get_homography_and_distortion_maps(cam_geometry, H, mapx, mapy);

        // 3) Load grayscale image and map it to the undistorted fronto-parallel view
        std::stringstream ss;
        ss << input_images_base_path_ << i + 1 << ".png";
        if (!bfs::exists(ss.str())) {
            MV_LOG_ERROR() << "A pattern detection image is missing.\n" << ss.str();
            return false;
        }
        img = cv::imread(ss.str(), cv::IMREAD_GRAYSCALE);
        cv::remap(img, img_fronto_undist, mapx, mapy, cv::INTER_CUBIC, 0,
                  255); // Set a white background around the chessboard
        cv::GaussianBlur(img_fronto_undist, img_fronto_undist,
                         cv::Size(2 * blur_radius_refinement_ + 1, 2 * blur_radius_refinement_ + 1), 2);

        if (refinement_mode_ == CalibrationRefinementMode::REFINE_AND_SHOW_IMAGES) {
            cv::cvtColor(img, img_bgr, cv::COLOR_GRAY2BGR);
            cv::cvtColor(img_fronto_undist, img_undist_bgr, cv::COLOR_GRAY2BGR);
        }

        // 4) Find chessboard corners
        // Project previous corner detections to the current undistorted fronto parallel view
        // and update the subpix refinement
        const cv::Size chessboard_size(pattern_3d_.n_cols_, pattern_3d_.n_rows_);
        std::vector<cv::Point2f> keypoints;
        cv::undistortPoints(input_pts, keypoints, K_, d_, cv::noArray(), K_); // keypoints on undist image
        cv::perspectiveTransform(keypoints, keypoints, H.inv());              // keypoints now on fronto plane

        const float dcol = cv::norm(keypoints[0] - keypoints[1]);                   // distance between 2 cols
        const float drow = cv::norm(keypoints[0] - keypoints[pattern_3d_.n_cols_]); // distance between 2 rows
        const float search_rad_y =
            std::round(0.5 * (0.9 * drow - 1)); // window size of cornerSubPix will be (0.9 dcol, 0.9 drow)
        const float search_rad_x = std::round(0.5 * (0.9 * dcol - 1));

        cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 200, 1e-6);
        cv::cornerSubPix(img_fronto_undist, keypoints, cv::Size(search_rad_x, search_rad_y), cv::Size(1, 1), criteria);

        // Refine with a smaller search window the corners that are too close from the edge to avoid incorrect
        // refinements
        std::vector<cv::Point2f> corners_close_to_edge;
        std::vector<int> corners_close_to_edge_indices;
        find_corners_close_to_the_edge(
            input_pts, keypoints, i, corners_close_to_edge,
            corners_close_to_edge_indices); // corners_close_to_edge in the same coordinate system as keypoints

        // Overwrite possibly wrong subpix refinement for corners too close from the edge
        if (!corners_close_to_edge.empty()) {
            cv::cornerSubPix(img_fronto_undist, corners_close_to_edge, cv::Size(11, 11), cv::Size(1, 1), criteria);
            using SizeType = std::vector<cv::Point2f>::size_type;
            for (SizeType k = 0; k < corners_close_to_edge.size(); ++k)
                keypoints[corners_close_to_edge_indices[k]] = corners_close_to_edge[k];
        }

        if (refinement_mode_ == CalibrationRefinementMode::REFINE_AND_SHOW_IMAGES)
            cv::drawChessboardCorners(img_undist_bgr, chessboard_size, keypoints, true);

        // Undo transformations on the keypoints
        cv::perspectiveTransform(keypoints, keypoints, H); // keypoints now on undist image
        for (cv::Point2f &pt : keypoints) {
            cv::Point2f tmp;
            Metavision::undist_img_to_undist_norm(cam_geometry, pt, tmp);
            Metavision::undist_norm_to_img(cam_geometry, tmp, pt);
        } // keypoints now on dist image

        if (refinement_mode_ == CalibrationRefinementMode::REFINE_AND_SHOW_IMAGES) {
            cv::drawChessboardCorners(img_bgr, chessboard_size, keypoints, true);
            if (!selected_views_[i]) {
                cv::putText(img_bgr, "Outlier during initial calibration.", cv::Point(0, 10), cv::FONT_HERSHEY_DUPLEX,
                            0.5, cv::Vec3b(0, 0, 255));
            }
        }

        refined_pts_2d.push_back(keypoints); // Use refined detections

        if (refinement_mode_ == CalibrationRefinementMode::REFINE_AND_SHOW_IMAGES) {
            cv::hconcat(img_bgr, img_undist_bgr, concat_bgr);
            cv::imshow("distorted - undistorted", concat_bgr);
            cv::waitKey(0);
        }
    }

    std::swap(refined_pts_2d, pts_2d_);

    // Recalibrate and allow K3 to vary
    overall_rms_reprojection_error_ = Metavision::MonoCalibration::calibrate_opencv(
        pattern_3d_.base_3D_points_, pts_2d_, img_size_, camera_model_, cv_flag_refinement_, K_, d_, &selected_views_,
        &rvecs_, &tvecs_, outlier_ths_);

    return true;
}

void Pipeline::find_corners_close_to_the_edge(const std::vector<cv::Point2f> &input_pts,
                                              const std::vector<cv::Point2f> &fronto_pts, int view_idx,
                                              std::vector<cv::Point2f> &corners_close_to_edge,
                                              std::vector<int> &corners_close_to_edge_indices) {
    // Return true if the symmetric of point "pt_idx" w.r.t. the corner "corner_idx" is still in the image
    // This allows to extrapolate the position of the external edges of the board from the internal corners
    const auto is_valid = [&](int corner_idx, int pt_idx) {
        static constexpr float kThs = 0.9;
        const cv::Point2f pt        = input_pts[corner_idx] + kThs * (input_pts[corner_idx] - input_pts[pt_idx]);
        return (pt.x >= 0 && pt.x < img_size_.width && pt.y >= 0 && pt.y < img_size_.height);
    };

    const int tl_idx = 0;
    const int tr_idx = pattern_3d_.n_cols_ - 1;
    const int br_idx = pattern_3d_.n_rows_ * pattern_3d_.n_cols_ - 1;   // pattern_3d_.n_pts_ -1
    const int bl_idx = (pattern_3d_.n_rows_ - 1) * pattern_3d_.n_cols_; // pattern_3d_.n_pts_ - pattern_3d_.n_cols_

    const int shift_down  = pattern_3d_.n_cols_;
    const int shift_up    = -pattern_3d_.n_cols_;
    const int shift_right = 1;
    const int shift_left  = -1;

    std::stringstream ss;
    ss << "View " << view_idx + 1 << " is close to the edge. Smaller subpix search window:";

    // First row
    if (!is_valid(tl_idx, tl_idx + shift_down) || !is_valid(tr_idx, tr_idx + shift_down)) {
        ss << " First row,";
        for (int i = tl_idx; i <= tr_idx; i += shift_right) {
            corners_close_to_edge.emplace_back(fronto_pts[i]);
            corners_close_to_edge_indices.emplace_back(i);
        }
    }
    // Last row
    if (!is_valid(bl_idx, bl_idx + shift_up) || !is_valid(br_idx, br_idx + shift_up)) {
        ss << " Last row,";
        for (int i = bl_idx; i <= br_idx; i += shift_right) {
            corners_close_to_edge.emplace_back(fronto_pts[i]);
            corners_close_to_edge_indices.emplace_back(i);
        }
    }
    // First col
    if (!is_valid(tl_idx, tl_idx + shift_right) || !is_valid(bl_idx, bl_idx + shift_right)) {
        ss << " First column,";
        for (int i = tl_idx; i <= bl_idx; i += shift_down) {
            corners_close_to_edge.emplace_back(fronto_pts[i]);
            corners_close_to_edge_indices.emplace_back(i);
        }
    }
    // Last col
    if (!is_valid(tr_idx, tr_idx + shift_left) || !is_valid(br_idx, br_idx + shift_left)) {
        ss << " Last column,";
        for (int i = tr_idx; i <= br_idx; i += shift_down) {
            corners_close_to_edge.emplace_back(fronto_pts[i]);
            corners_close_to_edge_indices.emplace_back(i);
        }
    }

    if (!corners_close_to_edge_indices.empty()) {
        ss.seekp(-1, std::ios_base::end);
        ss << ' '; // Remove last comma
        MV_LOG_INFO() << ss.str();
    }
}

void Pipeline::compute_reprojection_errors() {
    {
        per_view_rms_reprojection_errors_.clear();
        Metavision::MonoCalibration::compute_reprojection_errors_opencv(pattern_3d_.base_3D_points_, pts_2d_, K_, d_,
                                                                        selected_views_, rvecs_, tvecs_, camera_model_,
                                                                        per_view_rms_reprojection_errors_);
        int i_selected = 0;
        using SizeType = std::vector<bool>::size_type;
        for (SizeType i = 0; i < selected_views_.size(); ++i) {
            std::stringstream ss;
            ss << "Pattern " << i + 1 << ": ";
            if (selected_views_[i]) {
                ss << per_view_rms_reprojection_errors_[i_selected] << " pix";
                i_selected++;
            } else {
                ss << "skipped";
            }
            MV_LOG_INFO() << ss.str();
        }
    }

    // Error visualization
    if (show_reprojection_errors_) {
        std::vector<cv::Point2f> reprojected_pts_2d;
        const cv::Mat skip_overlay(img_size_.height, img_size_.width, CV_8UC3, cv::Vec3b(0, 0, 255));
        const cv::Mat normal_overlay(img_size_.height, img_size_.width, CV_8UC3, cv::Vec3b(0, 0, 0));
        const std::string skipped_view_str = "SKIPPED VIEW";
        const cv::Size str_size            = cv::getTextSize(skipped_view_str, cv::FONT_HERSHEY_SIMPLEX, 1, 1, 0);
        const cv::Point text_pos           = cv::Point((img_size_.width - str_size.width) / 2, img_size_.height / 2);
        const cv::Size chessboard_size(pattern_3d_.n_cols_, pattern_3d_.n_rows_);

        cv::Mat img(img_size_.height, img_size_.width, CV_8UC3);
        int i_selected = 0;
        using SizeType = std::vector<bool>::size_type;
        for (SizeType i = 0; i < selected_views_.size(); ++i) {
            {
                std::stringstream ss;
                ss << input_images_base_path_ << i + 1 << ".png";
                if (bfs::exists(ss.str())) {
                    img = cv::imread(ss.str());
                } else {
                    img.setTo(cv::Vec3b::all(255));
                    MV_LOG_ERROR() << "A pattern detection image is missing. A blank image is used instead.\n"
                                   << ss.str();
                }
            }

            cv::putText(img, std::to_string(i), cv::Point(img.cols / 2, 40), cv::FONT_HERSHEY_SIMPLEX, 1,
                        cv::Vec3b(0, 0, 255), 2);
            cv::drawChessboardCorners(img, chessboard_size, pts_2d_[i], true);

            if (selected_views_[i]) {
                cv::addWeighted(img, 0.4, normal_overlay, 0.4, 0, img);

                Metavision::MonoCalibration::project_points_opencv(pattern_3d_.base_3D_points_, K_, d_,
                                                                   rvecs_[i_selected], tvecs_[i_selected],
                                                                   camera_model_, reprojected_pts_2d);

                // Magnified reprojection errors
                using SizeType = std::vector<cv::Point2f>::size_type;
                for (SizeType k = 0; k < reprojected_pts_2d.size(); ++k) {
                    cv::arrowedLine(img, pts_2d_[i][k],
                                    pts_2d_[i][k] + error_magnification_ * (reprojected_pts_2d[k] - pts_2d_[i][k]),
                                    cv::Vec3b(0, 0, 255), 2);
                }

                std::stringstream ss;
                ss << "RMS reprojection error: " << per_view_rms_reprojection_errors_[i_selected] << " pix";
                cv::putText(img, ss.str(), cv::Point(0, 10), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Vec3b(0, 0, 255));

                i_selected++;
            } else {
                cv::addWeighted(img, 0.4, skip_overlay, 0.4, 0, img);
                cv::putText(img, skipped_view_str, text_pos, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Vec3b::all(255), 2);
            }

            cv::imshow(errors_window_title_, img);
            cv::waitKey(0);
        }
    }
}

void Pipeline::export_calibration_results() {
    cv::FileStorage fs(output_calibration_path_, cv::FileStorage::WRITE);
    fs << "n_kept_views" << int(per_view_rms_reprojection_errors_.size());
    fs << "image_size" << img_size_;

    fs << "pattern"
       << "{"
       << "n_rows" << static_cast<int>(pattern_3d_.n_rows_) << "n_cols" << static_cast<int>(pattern_3d_.n_cols_)
       << "square_height" << pattern_3d_.square_height_ << "square_width" << pattern_3d_.square_width_ << "}";
    fs << "camera_matrix" << K_;
    fs << "distortion_coefficients" << d_;

    std::vector<int> export_selected_views;
    for (const bool b : selected_views_)
        export_selected_views.emplace_back(b);
    fs << "selected_views" << export_selected_views;

    fs << "overall_rms_reprojection_error" << overall_rms_reprojection_error_;
    fs << "per_view_rms_reprojection_errors" << per_view_rms_reprojection_errors_;

    cv::Mat export_rvecs;
    cv::vconcat(rvecs_, export_rvecs);
    fs << "rvecs" << export_rvecs;

    cv::Mat export_tvecs;
    cv::vconcat(tvecs_, export_tvecs);
    fs << "tvecs" << export_tvecs;
    fs.release();

    MV_LOG_INFO() << std::endl
                  << "Intrinsics, extrinsics and per view RMS reprojection errors have been saved in"
                  << output_calibration_path_;
}

bool Pipeline::run() {
    if (!calibrate())
        return false;

    compute_reprojection_errors();

    export_calibration_results();
    return true;
}

int main(int argc, char *argv[]) {
    Pipeline p;

    if (!p.parse_command_line(argc, argv))
        return 1;

    if (!p.run())
        return 1;

    return 0;
}

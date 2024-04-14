# Copyright (c) Prophesee S.A. - All Rights Reserved
#
# Subject to Prophesee Metavision Licensing Terms and Conditions ("License T&C's").
# You may not use this file except in compliance with these License T&C's.
# A copy of these License T&C's is located in the "licensing" folder accompanying this file.

project(metavision_mono_calibration_recording)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

find_package(MetavisionSDK COMPONENTS core driver cv calibration ui REQUIRED)
find_package(Boost COMPONENTS filesystem program_options REQUIRED)
find_package(OpenCV COMPONENTS core highgui imgproc calib3d REQUIRED)

set(sample metavision_mono_calibration_recording)
set(source_files ${sample}.cpp blinking_chessboard_detector.cpp blinking_dots_grid_detector.cpp)
add_executable(${sample} ${source_files})
target_link_libraries(${sample}
    MetavisionSDK::core
    MetavisionSDK::base
    MetavisionSDK::driver
    MetavisionSDK::cv
    MetavisionSDK::calibration
    MetavisionSDK::ui
    Boost::filesystem
    Boost::program_options
    ${OpenCV_LIBS})

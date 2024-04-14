# Copyright (c) Prophesee S.A. - All Rights Reserved
#
# Subject to Prophesee Metavision Licensing Terms and Conditions ("License T&C's").
# You may not use this file except in compliance with these License T&C's.
# A copy of these License T&C's is located in the "licensing" folder accompanying this file.

project(metavision_mono_calibration)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

find_package(MetavisionSDK COMPONENTS base calibration REQUIRED)
find_package(Boost COMPONENTS filesystem program_options REQUIRED)
find_package(OpenCV COMPONENTS core highgui imgproc imgcodecs calib3d REQUIRED)

set(sample metavision_mono_calibration)
set(source_files ${sample}.cpp)
add_executable(${sample} ${source_files})
target_link_libraries(${sample}
    MetavisionSDK::core
    MetavisionSDK::cv
    MetavisionSDK::calibration
    Boost::filesystem
    Boost::program_options
    ${OpenCV_LIBS})

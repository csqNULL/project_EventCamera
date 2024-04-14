import os
import numpy as np
from metavision_sdk_core import BaseFrameGenerationAlgorithm
from metavision_core.event_io import EventsIterator
import metavision_sdk_cv
import metavision_hal
import cv2


from ipywidgets import interact
from PIL import Image

def display_sequence(images):
    def _show(frame=(0, len(images)-1)):
        return Image.fromarray(images[frame])
    interact(_show)

from metavision_core.utils import get_sample

# 定义存储事件点和物体点的列表
image_points_list = []
object_points_list = []

camera_matrix = np.array([[1000.0, 0.0, 320.0], [0.0, 1000.0, 240.0], [0.0, 0.0, 1.0]], dtype=np.float32)
dist_coeffs = np.zeros((4, 1))

sequence_filename = "recording_1234k_fast.raw"

# get_sample(sequence_filename, folder=".")
# assert(os.path.isfile(sequence_filename))

# mv_it = EventsIterator(sequence_filename, start_ts=7500000, mode="mixed", delta_t=500, n_events=500000)
mv_it = EventsIterator(sequence_filename, start_ts=1000000, mode="mixed", delta_t=200, n_events=5000000)
height, width = mv_it.get_size()

# bias_bool = metavision_hal.I_LL_Biases('bias_fo',55)
frequency_filter = metavision_sdk_cv.FrequencyAlgorithm(width=width, height=height, min_freq=3200, max_freq=4800)
frequency_clustering_filter = metavision_sdk_cv.FrequencyClusteringAlgorithm(width=width, height=height,
                                                                             min_cluster_size=1)
# , max_time_diff=100

freq_buffer = frequency_filter.get_empty_output_buffer()
cluster_buffer = frequency_clustering_filter.get_empty_output_buffer()

im = np.zeros((height, width, 3), dtype=np.uint8)


for idx, ev in enumerate(mv_it):

    if idx >= 1000:
        break

    BaseFrameGenerationAlgorithm.generate_frame(ev, im)

    frequency_filter.process_events(ev, freq_buffer)
    frequency_clustering_filter.process_events(freq_buffer, cluster_buffer)

    for cluster in cluster_buffer.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        # if (300 < x0 < 650) & (200 < y0 < 300):
        #     print(x0+10, y0+10)
        # else:
        #     continue
        # print(x0+10, y0+10)
        cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
                    1, (0, 255, 0), 1)
        image_points = np.array([x0, y0])


        # 添加事件点和对应的物体点
        image_points_list.append((x0, y0))
        object_points_list.append((x0, y0, 0))  # 在这里我们只是用事件点作为物体点的代替

         # 每处理一定数量的事件，进行一次位姿估计
        if len(image_points_list) >= 4:
            # 将列表转换为 NumPy 数组
            image_points = np.array(image_points_list, dtype=np.float32)
            object_points = np.array(object_points_list, dtype=np.float32)

            # 使用 solvePnPRansac 进行位姿估计
            retval, rvec_est, tvec_est, inliers = cv2.solvePnPRansac(object_points, image_points, camera_matrix, dist_coeffs)
            if retval:
                print("Pose estimation successful!")
                print("Estimated rotation vector (rvec):", rvec_est)
                print("Estimated translation vector (tvec):", tvec_est)
                print("Inliers:", inliers)
            else:
                print("Pose estimation failed!")

            # 清空列表以进行下一轮位姿估计
            image_points_list.clear()
            object_points_list.clear()
        # if cluster["frequency"] > 4800:
        # print(float(cluster["frequency"]))
        # print(ev)

        # print(ev)
    cv2.imshow("Events", im[...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cv2.destroyAllWindows()

import matplotlib.pyplot as plt
# %matplotlib inline
fig = plt.figure(figsize=(15,15))
plt.imshow(im, aspect="equal")

# camera_matrix = np.array([[1000.0, 0.0, 320.0], [0.0, 1000.0, 240.0], [0.0, 0.0, 1.0]], dtype=np.float32)
# dist_coeffs = np.zeros((4, 1))
# object_points = np.random.randint(1, 10, [1,3])
# # 生成一些随机的三维点，动捕的实际坐标点

# # 使用 solvePnPRansac 进行位姿估计
# retval, rvec_est, tvec_est, inliers = cv2.solvePnPRansac(object_points, image_points, camera_matrix, dist_coeffs)
# print(retval)
# print(rvec_est)
# print(tvec_est)
# print(inliers)
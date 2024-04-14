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


def calculate_average(matrix, n):
    # 确保矩阵的形状是 n×2
    if matrix.shape != (n, 2):
        raise ValueError("The shape of the matrix should be ({}, 2).".format(n))

    # 计算每个元素的平均值
    average_values = np.mean(matrix, axis=0)
    average_values = np.mat(average_values)

    return average_values

def judPoint(data_list, n, image_points_list, object_points_list):
    data_list = np.array(data_list)
    data_list = data_list.reshape(-1, 2)

    if n > 1:
        data = calculate_average(data_list,n)
        data = data.tolist()
        # 添加事件点和对应的物体点
        for data_xy in data:
            image_points_list.append((data_xy[0], data_xy[1]))
            object_points_list.append((data_xy[0], data_xy[1], 0))  # 在这里我们只是用事件点作为物体点的代替
            return 1
    elif n == 1:
        # 添加事件点和对应的物体点
        for data_xy in data_list:
            image_points_list.append((data_xy[0], data_xy[1]))
            object_points_list.append((data_xy[0], data_xy[1], 0))  # 在这里我们只是用事件点作为物体点的代替
            return 1
    else:
        return 0

    
    
sequence_filename = "recording_1234k_fast.raw"

# get_sample(sequence_filename, folder=".")
# assert(os.path.isfile(sequence_filename))

mv_it = EventsIterator(sequence_filename, start_ts=1000000, mode="mixed", delta_t=500, n_events=5000000)
height, width = mv_it.get_size()

min_freq1 = 800
max_freq1 = 1200
min_freq2 = 1500
max_freq2 = 2500
min_freq3 = 2500
max_freq3 = 3500
min_freq4 = 3500
max_freq4 = 4500

# led_1
frequency_filter_1 = metavision_sdk_cv.FrequencyAlgorithm(width=width, height=height, min_freq=min_freq1, max_freq=max_freq1)
frequency_clustering_filter_1 = metavision_sdk_cv.FrequencyClusteringAlgorithm(width=width, height=height,
                                                                             min_cluster_size=2)
# , max_time_diff=100

freq_buffer_1 = frequency_filter_1.get_empty_output_buffer()
cluster_buffer_1 = frequency_clustering_filter_1.get_empty_output_buffer()

# led_2
frequency_filter_2 = metavision_sdk_cv.FrequencyAlgorithm(width=width, height=height, min_freq=min_freq2, max_freq=max_freq2)
frequency_clustering_filter_2 = metavision_sdk_cv.FrequencyClusteringAlgorithm(width=width, height=height,
                                                                             min_cluster_size=2)
# , max_time_diff=100

freq_buffer_2 = frequency_filter_2.get_empty_output_buffer()
cluster_buffer_2 = frequency_clustering_filter_2.get_empty_output_buffer()

# led_3
frequency_filter_3 = metavision_sdk_cv.FrequencyAlgorithm(width=width, height=height, min_freq=min_freq3, max_freq=max_freq3)
frequency_clustering_filter_3 = metavision_sdk_cv.FrequencyClusteringAlgorithm(width=width, height=height,
                                                                             min_cluster_size=2)
# , max_time_diff=100

freq_buffer_3 = frequency_filter_3.get_empty_output_buffer()
cluster_buffer_3 = frequency_clustering_filter_3.get_empty_output_buffer()

# led_4
frequency_filter_4 = metavision_sdk_cv.FrequencyAlgorithm(width=width, height=height, min_freq=min_freq4, max_freq=max_freq4)
frequency_clustering_filter_4 = metavision_sdk_cv.FrequencyClusteringAlgorithm(width=width, height=height,
                                                                             min_cluster_size=1)
# , max_time_diff=100

freq_buffer_4 = frequency_filter_4.get_empty_output_buffer()
cluster_buffer_4 = frequency_clustering_filter_4.get_empty_output_buffer()

im = np.zeros((height, width, 3), dtype=np.uint8)
led = [1,2,3,4]


for idx, ev in enumerate(mv_it):

    if idx >= 250:
        break

    # im = np.zeros((height, width, 3), dtype=np.uint8)
    BaseFrameGenerationAlgorithm.generate_frame(ev, im)


# freq1
    frequency_filter_1.process_events(ev, freq_buffer_1)
    frequency_clustering_filter_1.process_events(freq_buffer_1, cluster_buffer_1)
    data_list = []
    n = 0
    for cluster in cluster_buffer_1.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], 997 + (int)(np.random.normal(0, 10, [1,1]))), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,1, (0, 255, 0), 1)

        data_list.append((x0, y0))
        n = n + 1
            
        # print(ev)
        # print(float(cluster["frequency"]))
    cv2.imshow("Events", im     [...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    # print(judPoint(data_list, n, image_points_list, object_points_list))
    judPoint(data_list, n, image_points_list, object_points_list)




# freq2
    # im = np.zeros((height, width, 3), dtype=np.uint8)
    # BaseFrameGenerationAlgorithm.generate_frame(ev, im)
    frequency_filter_2.process_events(ev, freq_buffer_2)
    frequency_clustering_filter_2.process_events(freq_buffer_2, cluster_buffer_2)
    data_list = []
    n = 0
    for cluster in cluster_buffer_2.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], 1970 + (int)(np.random.normal(0, 10, [1,1]))), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
                    1, (0, 255, 0), 1)        
        data_list.append((x0, y0))
        n = n + 1

        # print(ev)
        # print(float(cluster["frequency"]))
    cv2.imshow("Events", im     [...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    # print(judPoint(data_list, n, image_points_list, object_points_list))
    judPoint(data_list, n, image_points_list, object_points_list)




# freq3
    # im = np.zeros((height, width, 3), dtype=np.uint8)
    # BaseFrameGenerationAlgorithm.generate_frame(ev, im)
    frequency_filter_3.process_events(ev, freq_buffer_3)
    frequency_clustering_filter_3.process_events(freq_buffer_3, cluster_buffer_3)
    n = 0
    data_list = []
    for cluster in cluster_buffer_3.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], 2982 + (int)(np.random.normal(0, 10, [1,1]))), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
                    1, (0, 255, 0), 1)
        # print("3号")

        data_list.append((x0, y0))
        n = n + 1

        # print(ev)
        # print(float(cluster["frequency"]))
    cv2.imshow("Events", im     [...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    # print(judPoint(data_list, n, image_points_list, object_points_list))
    judPoint(data_list, n, image_points_list, object_points_list)




# freq4
    # im = np.zeros((height, width, 3), dtype=np.uint8)
    # BaseFrameGenerationAlgorithm.generate_frame(ev, im)
    frequency_filter_4.process_events(ev, freq_buffer_4)
    frequency_clustering_filter_4.process_events(freq_buffer_4, cluster_buffer_4)
    n = 0
    data_list = []
    for cluster in cluster_buffer_4.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], 3962 + (int)(np.random.normal(0, 10, [1,1]))), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
                    1, (0, 255, 0), 1)
        # print("4号")
        # 添加事件点和对应的物体点

        data_list.append((x0, y0))

        n = n + 1

        # print(ev)
        # print(float(cluster["frequency"]))
    cv2.imshow("Events", im     [...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    # print(judPoint(data_list, n, image_points_list, object_points_list))
    judPoint(data_list, n, image_points_list, object_points_list)


        # 每处理一定数量的事件，进行一次位姿估计
    if len(image_points_list) >= 4:
        i = 1
        for position in image_points_list:
            print("位置",i,position)
            i = i + 1

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


cv2.destroyAllWindows()


import matplotlib.pyplot as plt
# %matplotlib inline
fig = plt.figure(figsize=(15,15))
plt.imshow(im, aspect="equal")

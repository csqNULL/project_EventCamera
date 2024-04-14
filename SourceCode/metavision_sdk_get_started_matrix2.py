import os
import numpy as np
from metavision_sdk_core import BaseFrameGenerationAlgorithm
from metavision_core.event_io import EventsIterator
import metavision_sdk_cv
import metavision_hal
import cv2
import xlsxwriter as xw

from ipywidgets import interact
from PIL import Image

fileName = '测试.xlsx'

def xw_toExcel(data, fileName):  # xlsxwriter库储存数据到excel
    workbook = xw.Workbook(fileName)  # 创建工作簿
    worksheet1 = workbook.add_worksheet("sheet1")  # 创建子表
    worksheet1.activate()  # 激活表
    title = ['x0', 'y0']  # 设置表头
    worksheet1.write_row('A1', title)  # 从A1单元格开始写入表头
    i = 2  # 从第二行开始写入数据
    for j in range(len(data)):
        insertData = [data[j]["id"], data[j]["name"], data[j]["price"]]
        row = 'A' + str(i)
        worksheet1.write_row(row, insertData)
        i += 1
    workbook.close()  # 关闭表


def display_sequence(images):
    def _show(frame=(0, len(images)-1)):
        return Image.fromarray(images[frame])
    interact(_show)

from metavision_core.utils import get_sample


# 定义存储事件点和物体点的列表
image_points_list = []
object_points_list = []

cameraMatrix = np.array([[1.3376667907724209e+03, 0., 6.2504931658445537e+02],
                            [0., 1.3376667907724209e+03, 3.7959956600073838e+02],
                            [0., 0., 1.]], np.float32)
distCoeffs = np.array([-1.0885556876987361e-01, 4.2649835956791732e-01,
                        -1.2981244349286130e-03, 3.4821656936060650e-04, 0.])


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
                                                                             min_cluster_size=1)
# , max_time_diff=100

freq_buffer_1 = frequency_filter_1.get_empty_output_buffer()
cluster_buffer_1 = frequency_clustering_filter_1.get_empty_output_buffer()

# led_2
frequency_filter_2 = metavision_sdk_cv.FrequencyAlgorithm(width=width, height=height, min_freq=min_freq2, max_freq=max_freq2)
frequency_clustering_filter_2 = metavision_sdk_cv.FrequencyClusteringAlgorithm(width=width, height=height,
                                                                             min_cluster_size=1)
# , max_time_diff=100

freq_buffer_2 = frequency_filter_2.get_empty_output_buffer()
cluster_buffer_2 = frequency_clustering_filter_2.get_empty_output_buffer()

# led_3
frequency_filter_3 = metavision_sdk_cv.FrequencyAlgorithm(width=width, height=height, min_freq=min_freq3, max_freq=max_freq3)
frequency_clustering_filter_3 = metavision_sdk_cv.FrequencyClusteringAlgorithm(width=width, height=height,
                                                                             min_cluster_size=1)
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

test = 0
for idx, ev in enumerate(mv_it):

    if idx >= 250:
        break

    # im = np.zeros((height, width, 3), dtype=np.uint8)
    BaseFrameGenerationAlgorithm.generate_frame(ev, im)

    pointNum = 0

# freq1
    frequency_filter_1.process_events(ev, freq_buffer_1)
    frequency_clustering_filter_1.process_events(freq_buffer_1, cluster_buffer_1)
    data_list = []
    n = 0
    for cluster in cluster_buffer_1.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        # cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], 997 + (int)(np.random.normal(0, 10, [1,1]))), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,1, (0, 255, 0), 1)

        data_list.append((x0, y0))
        n = n + 1
        pointNum = pointNum + 1            
        # print(ev)
        # print(float(cluster["frequency"]))
    cv2.imshow("Events", im     [...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    # print(judPoint(data_list, n, image_points_list, object_points_list))





# freq2
    # im = np.zeros((height, width, 3), dtype=np.uint8)
    # BaseFrameGenerationAlgorithm.generate_frame(ev, im)
    frequency_filter_2.process_events(ev, freq_buffer_2)
    frequency_clustering_filter_2.process_events(freq_buffer_2, cluster_buffer_2)
    n = 0
    for cluster in cluster_buffer_2.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        # cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], 1970 + (int)(np.random.normal(0, 10, [1,1]))), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)        
        data_list.append((x0, y0))
        n = n + 1
        pointNum = pointNum + 1
        # print(ev)
        # print(float(cluster["frequency"]))
    cv2.imshow("Events", im     [...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    # print(judPoint(data_list, n, image_points_list, object_points_list))





# freq3
    # im = np.zeros((height, width, 3), dtype=np.uint8)
    # BaseFrameGenerationAlgorithm.generate_frame(ev, im)
    frequency_filter_3.process_events(ev, freq_buffer_3)
    frequency_clustering_filter_3.process_events(freq_buffer_3, cluster_buffer_3)
    n = 0
    for cluster in cluster_buffer_3.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], 2982 + (int)(np.random.normal(0, 10, [1,1]))), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        # print("3号")

        data_list.append((x0, y0))
        n = n + 1
        pointNum = pointNum + 1

        # print(ev)
        # print(float(cluster["frequency"]))
    cv2.imshow("Events", im     [...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    # print(judPoint(data_list, n, image_points_list, object_points_list))





# freq4
    # im = np.zeros((height, width, 3), dtype=np.uint8)
    # BaseFrameGenerationAlgorithm.generate_frame(ev, im)
    frequency_filter_4.process_events(ev, freq_buffer_4)
    frequency_clustering_filter_4.process_events(freq_buffer_4, cluster_buffer_4)
    n = 0
    for cluster in cluster_buffer_4.numpy():
        x0 = int(cluster["x"]) - 10
        y0 = int(cluster["y"]) - 10
        # cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        # cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], 3962 + (int)(np.random.normal(0, 10, [1,1]))), (x0, y0-10), cv2.FONT_HERSHEY_PLAIN,
        #             1, (0, 255, 0), 1)
        # print("4号")
        # 添加事件点和对应的物体点

        data_list.append((x0, y0))

        n = n + 1
        pointNum = pointNum + 1

        # print(ev)
        # print(float(cluster["frequency"]))
    cv2.imshow("Events", im     [...,::-1])
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 判断四个角点是否都被检测到
    if pointNum <= 4:
        continue

    data_list = np.array(data_list)
    data_list = data_list.reshape(-1, 2)
    # print(data_list)
    # print(data_list.shape)
    # 定义停止条件
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)

    # 使用cv2.kmeans函数执行聚类，centers为中心点
    data_list = data_list.astype(np.float32)
    ret1, labels1, centers1 = cv2.kmeans(data_list, 1, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
    ret2, labels2, centers2 = cv2.kmeans(data_list, 2, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
    ret3, labels3, centers3 = cv2.kmeans(data_list, 3, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
    ret4, labels4, centers4 = cv2.kmeans(data_list, 4, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
    # print(centers)
    # print("ret")
    # print(ret1, ret2, ret3, ret4)
    
# 判断是否真检测了四个点，用方差表达离群度
    if (ret4 > ret3) and (ret4 > ret2) and (ret4 > ret1):
        continue
    centers = centers4

    # print(np.var(centers))
    test = test + 1
    
    for position in centers:
        x0 = int(position[0]) - 10
        y0 = int(position[1]) - 10
        # print(x0, y0)
        cv2.rectangle(im, (x0, y0), (x0+20, y0+20), color=(0, 255, 0))
    # print("我是分隔符")

    i = 1
    for position in image_points_list:
        print("位置",i,position)
        i = i + 1

    # 将列表转换为 NumPy 数组
    # image_points = np.array(centers, dtype=np.float32)
    image_points = centers
    # object_points = np.array(object_points_list, dtype=np.float32)

    # 使用 solvePnPRansac 进行位姿估计
    new_column = np.zeros((4,1))
    object_points = np.hstack((image_points, new_column))
    retval, rvec_est, tvec_est = cv2.solvePnP(object_points, image_points, cameraMatrix, distCoeffs)
    # retval, rvec_est, tvec_est, inliers = cv2.solvePnPRansac(object_points, image_points, cameraMatrix, distCoeffs)
    if retval:
        # 将相机坐标系中的点转换为世界坐标系中的点
        rvec_mat, _ = cv2.Rodrigues(rvec_est)
        world_points_est = np.dot(rvec_mat.T, object_points.T - tvec_est)
        world_points_est = world_points_est.T
    # 打印估计的世界坐标点
        print(world_points_est)
        # print("Estimated world points:", world_points_est)
        # print("Pose estimation successful!")
        # print("Estimated rotation vector (rvec):", rvec_est)
        # print("Estimated translation vector (tvec):", tvec_est)
        # # print("Inliers:", inliers)
        # world_coords, _ = cv2.projectPoints(object_points, rvec_est, tvec_est, cameraMatrix, distCoeffs)
        # print("三维坐标")
        # print(world_coords)
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

# 显示聚类结果

# plt.scatter(group1[:,0], group1[:,1], c='r')
# plt.scatter(group2[:,0], group2[:,1], c='b')
# plt.scatter(centers[:,0], centers[:,1], s=100, c='y', marker='s')
# plt.xlabel('Feature 1')
# plt.ylabel('Feature 2')
# plt.title('K-means Clustering')
# plt.show()
import numpy as np
from matplotlib import pyplot as plt
import time
# import pandas as pd
import cv2.aruco as aruco
import sys
import cv2
from PIL import Image
import matplotlib as plt
import csv
import os
# import scipy.misc
from pylab import plot
import matplotlib.pyplot as plt
# %matplotlib inline
import cv2
import numpy as np
import csv
import os
from enum import Enum
from metavision_core.event_io import EventsIterator
from metavision_core.event_io import LiveReplayEventsIterator, is_live_camera
from metavision_sdk_core import PeriodicFrameGenerationAlgorithm
from metavision_sdk_cv import ActivityNoiseFilterAlgorithm, TrailFilterAlgorithm, SpatioTemporalContrastAlgorithm
from metavision_sdk_ui import EventLoop, BaseWindow, MTWindow, UIAction, UIKeyEvent



np.set_printoptions(threshold=np.inf)

from metavision_sdk_core import PeriodicFrameGenerationAlgorithm,ColorPalette,BaseFrameGenerationAlgorithm
from metavision_sdk_ui import EventLoop, BaseWindow, MTWindow, UIAction, UIKeyEvent
from metavision_core.event_io import EventsIterator,LiveReplayEventsIterator, is_live_camera
import metavision_sdk_cv
import numba as nb
import metavision_sdk_ml
from metavision_sdk_core import EventBbox

activity_time_ths = 20  # Length of the time window for activity filtering (in us)
activity_ths = 2  # Minimum number of events in the neighborhood for activity filtering
activity_trail_ths = 1000  # Length of the time window for trail filtering (in us)

trail_filter_ths = 100  # Length of the time window for activity filtering (in us)

stc_filter_ths = 10000  # Length of the time window for filtering (in us)
stc_cut_trail = True  # If true, after an event goes through, it removes all events until change of polarity


class Filter(Enum):
    NONE = 0,
    ACTIVITY = 1,
    STC = 2,
    TRAIL = 3

def filter():
    """ Main """
    args = parse_args()

    print("Code sample showing how to create a simple application testing different noise filtering strategies.")
    print("Available keyboard options:\n"
          "  - A: Filter events using the activity noise filter algorithm\n"
          "  - T: Filter events using the trail filter algorithm\n"
          "  - S: Filter events using the spatio temporal contrast algorithm\n"
          "  - E: Show all events\n"
          "  - Q/Escape: Quit the application\n")

    # Events iterator on Camera or RAW file
    mv_iterator = EventsIterator(input_path=args.input_path, delta_t=1000)
    if args.replay_factor > 0 and not is_live_camera(args.input_path):
        mv_iterator = LiveReplayEventsIterator(mv_iterator, replay_factor=args.replay_factor)
    height, width = mv_iterator.get_size()  # Camera Geometry

    filters = {Filter.ACTIVITY: ActivityNoiseFilterAlgorithm(width, height, activity_time_ths),
               Filter.TRAIL: TrailFilterAlgorithm(width, height, trail_filter_ths),
               Filter.STC: SpatioTemporalContrastAlgorithm(width, height, stc_filter_ths, stc_cut_trail)
               }

    events_buf = ActivityNoiseFilterAlgorithm.get_empty_output_buffer()
    filter_type = Filter.NONE

    # Event Frame Generator
    event_frame_gen = PeriodicFrameGenerationAlgorithm(width, height, accumulation_time_us=10000)

    # Window - Graphical User Interface (Display filtered events and process keyboard events)
    with MTWindow(title="Metavision Noise Filtering", width=width, height=height, mode=BaseWindow.RenderMode.BGR) as window:
        def on_cd_frame_cb(ts, cd_frame):
            window.show_async(cd_frame)

        event_frame_gen.set_output_callback(on_cd_frame_cb)

        def keyboard_cb(key, scancode, action, mods):
            nonlocal filter_type

            if action != UIAction.RELEASE:
                return
            if key == UIKeyEvent.KEY_ESCAPE or key == UIKeyEvent.KEY_Q:
                window.set_close_flag()
            elif key == UIKeyEvent.KEY_E:
                # Show all events
                filter_type = Filter.NONE
            elif key == UIKeyEvent.KEY_A:
                # Filter events using the activity filter algorithm
                filter_type = Filter.ACTIVITY
            elif key == UIKeyEvent.KEY_T:
                # Filter events using the trail filter algorithm
                filter_type = Filter.TRAIL
            elif key == UIKeyEvent.KEY_S:
                # Filter events using the spatio temporal contrast algorithm
                filter_type = Filter.STC

        window.set_keyboard_callback(keyboard_cb)

        # Process events
        for evs in mv_iterator:
            # Dispatch system events to the window
            EventLoop.poll_and_dispatch()

            # Process events
            if filter_type in filters:
                filters[filter_type].process_events(evs, events_buf)
                event_frame_gen.process_events(events_buf)
            else:
                event_frame_gen.process_events(evs)

            if window.should_close():
                break



def noise_filter(ev):
    # apply trail filter
    trail.process_events(ev, ev_filtered_buffer)
    return ev_filtered_buffer.numpy()



def set_array(evs, array_1):
    for point in evs:
        if point[2] == 1:
            array_1[point[1], point[0]] = 255
    return

def parse_args():
    import argparse
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Metavision SDK Get Started sample.',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        '-i', '--input-raw-file', dest='input_path', default="",
        help="Path to input RAW file. If not specified, the live stream of the first available camera is used. "
        "If it's a camera serial number, it will try to open that camera instead.")
    args = parser.parse_args()
    return args


def boxAngle(box):
    time_start = time.time()

    """
    得到旋转中心坐标，与旋转角度
    :return angle: 平行四边形转为矩形需要的角度
    :return cx, cy: 中心坐标
    """
    x1, y1, x2, y2, x3, y3, x4, y4 = box[:8]
    cx = (x1 + x3 + x2 + x4) / 4.0
    cy = (y1 + y3 + y4 + y2) / 4.0
    w = (np.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2) + np.sqrt((x3 - x4) ** 2 + (y3 - y4) ** 2)) / 2
    h = (np.sqrt((x2 - x3) ** 2 + (y2 - y3) ** 2) + np.sqrt((x1 - x4) ** 2 + (y1 - y4) ** 2)) / 2

    sinA = (h * (x1 - cx) - w * (y1 - cy)) * 1.0 / (h * h + w * w) * 2
    angle = np.arcsin(sinA)
    time_end = time.time()
    # print('def1cost', time_end - time_start, 's')
    return angle, w, h, cx, cy


def rotate(image, box, leftAdjustAlph=0.0, rightAdjustAlph=0.0):
    """
    需要用到boxAngle函数，得到旋转中心与旋转角度
    """
    time_start = time.time()

    angle, w, h, cx, cy = boxAngle(box)
    im = Image.fromarray(image)
    degree_ = angle * 180.0 / np.pi

    box = (max(1, cx - w / 2 - leftAdjustAlph * (w / 2))  ##xmin
           , cy - h / 2,  ##ymin
           min(cx + w / 2 + rightAdjustAlph * (w / 2), im.size[0] - 1)  ##xmax
           , cy + h / 2)  ##ymax
    newW = box[2] - box[0]
    newH = box[3] - box[1]
    # 新box的信息

    box = {'cx': cx, 'cy': cy, 'w': newW, 'h': newH, 'degree': degree_, }

    im = im.rotate(degree_, center=(cx, cy))  # .crop(box)
    time_end = time.time()
    # print('def2cost', time_end - time_start, 's')
    return im


def once_select_corner(corner_x, corner_y, average_x, average_y, distance):
    time_start = time.time()
    i = 0
    x1 = []
    y1 = []
    t_x = 0
    t_y = 0
    x1_flag = True
    while True:
        if i < len(corner_x):
            if pow((corner_x[i] - average_x), 2) + pow((corner_y[i] - average_y), 2) <= distance:
                x1.append(corner_x[i])
                y1.append(corner_y[i])
                t_x += corner_x[i]
                t_y += corner_y[i]
            i += 1
        else:
            break
    if len(x1) > 0:
        t_x = round(t_x / len(x1))
        t_y = round(t_y / len(y1))
    else:
        x1_flag = False
    time_end = time.time()
    # print('def3cost', time_end - time_start, 's')
    return x1_flag, x1, y1, t_x, t_y


def select_corners(img,corners):
    time_start = time.time()
    corners = cv2.goodFeaturesToTrack(img, 100, 0.05, 10)  # 返回的结果是 [[ 311., 250.]] 两层括号的数组。
    # corners = [(0,0),(1,1),(2,2),(3,3)]
    time_end = time.time()
    # print("harriscost = ",time_end-time_start)

    # print(corners.shape)
    # corners =
    # corners = None
    x11 = []
    y11 = []
    if corners is None:
        corner_flag = False
    else:
        corner_flag = True
        corners = np.int64(corners)
        # print(corners)
        corner_x = []
        corner_y = []
        average_x = 0
        average_y = 0
        for i in corners:
            x, y = i.ravel()
            corner_x.append(x)
            corner_y.append(y)
            average_x += x
            average_y += y

        average_x = round(average_x / len(corner_x))
        average_y = round(average_y / len(corner_y))
        corner_flag, x1, y1, average_x, average_y = once_select_corner(corner_x, corner_y, average_x, average_y, 80000)

        # tempimg1 = cv2.cvtColor(img.copy(), cv2.COLOR_GRAY2RGB)
        # for i in range(len(x1) - 1):
        #     cv2.circle(tempimg1, (x1[i], y1[i]), 5, 255, -1)
        # cv2.putText(tempimg1, "average(x1,y1): ", (average_x, average_y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1,
        #             cv2.LINE_AA)
        # cv2.imshow('2', tempimg1)

        if len(x1) > 0:
            corner_flag, x11, y11, average_x, average_y = once_select_corner(x1, y1, average_x, average_y, 50000)
        else:
            corner_flag = False

        # tempimg = cv2.cvtColor(img.copy(), cv2.COLOR_GRAY2RGB)
        # for i in range(len(x11) - 1):
        #     cv2.circle(tempimg, (x11[i], y11[i]), 5, 255, -1)
        # cv2.putText(tempimg, "average(x2,y2): ", (average_x, average_y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1,
        #             cv2.LINE_AA)
        # cv2.imshow('3', tempimg)
        # cv2.waitKey(0)
    time_end = time.time()
    # print('def4cost', time_end - time_start, 's')
    return corner_flag, x11, y11, img, corners


def select_the_four_corners(x11, y11, img):
    time_start = time.time()
    x = []
    y = []
    x.append(min(x11))
    y.append(y11[x11.index(min(x11))])
    x.append(max(x11))
    y.append(y11[x11.index(max(x11))])
    y.append(max(y11))
    x.append(x11[y11.index(max(y11))])
    y.append(min(y11))
    x.append(x11[y11.index(min(y11))])
    box = [x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3]]

    cx = round((x[0] + x[1] + x[2] + x[3]) / 4)
    cy = round((y[0] + y[1] + y[2] + y[3]) / 4)

    distance = []
    tempt_distance = 0
    for i in range(4):
        for j in range(4):
            tempt_distance += (x[j] - x[i]) ** 2 + (y[j] - y[i]) ** 2
        distance.append(tempt_distance)
    error_corner = distance.index(max(distance))
    temp_x1 = round((x[0] + x[3]) / 2)
    temp_y1 = round((y[0] + y[3]) / 2)

    temp_x2 = round((x[1] + x[3]) / 2)
    temp_y2 = round((y[1] + y[3]) / 2)

    temp_x3 = round((x[1] + x[2]) / 2)
    temp_y3 = round((y[1] + y[2]) / 2)

    temp_x4 = round((x[0] + x[2]) / 2)
    temp_y4 = round((y[0] + y[2]) / 2)

    temp_height = 0
    temp_width = 0

    if error_corner == 0:
        temp_height = 1.1 * (cy - temp_y2 + temp_x3 - cx) / 2
        temp_width = 1.1 * (temp_x2 - cx + temp_y3 - cy) / 2

    if error_corner == 1:
        temp_height = 1.1 * (cx - temp_x1 + temp_y4 - cy) / 2
        temp_width = 1.1 * (cy - temp_y1 + cx - temp_x4) / 2

    if error_corner == 2:
        temp_height = 1.1 * (cx - temp_x1 + cy - temp_y2) / 2
        temp_width = 1.1 * (cy - temp_y1 + temp_x2 - cx) / 2

    if error_corner == 3:
        temp_height = 1.1 * (temp_x3 - cx + temp_y4 - cy) / 2
        temp_width = 1.1 * (temp_y3 - cy + cx - temp_x4) / 2

    x1 = round(cx - temp_height - temp_width)
    y1 = round(cy + temp_height - temp_width)

    x2 = round(cx + temp_height + temp_width)
    y2 = round(cy - temp_height + temp_width)

    x4 = round(cx - temp_height + temp_width)
    y4 = round(cy - temp_height - temp_width)

    x3 = round(cx + temp_height - temp_width)
    y3 = round(cy + temp_height + temp_width)

    # print('corners=', box)
    corner = np.array([[[x2, y2], [x4, y4], [x1, y1], [x3, y3]]], dtype=float)
    temp_img = img.copy()
    # cv2.putText(temp_img, "x1: ", (x1, y1), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA)
    # cv2.putText(temp_img, "x2: ", (x2, y2), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA)
    # cv2.putText(temp_img, "x3: ", (x3, y3), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA)
    # cv2.putText(temp_img, "x4: ", (x4, y4), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA)
    # cv2.circle(temp_img, (x1, y1), 5, 0, -1)
    # cv2.circle(temp_img, (x3, y3), 5, 0, -1)
    # cv2.circle(temp_img, (x2, y2), 5, 0, -1)
    # cv2.circle(temp_img, (x4, y4), 5, 0, -1)
    # cv2.imshow("img111", temp_img)
    time_end = time.time()
    # print('def5cost', time_end - time_start, 's')
    return x1, y1, x2, y2, x3, y3, x4, y4, corner


def affine_transformation(x1, y1, x2, y2, x3, y3, x4, y4, img):
    time_start = time.time()
    pts = np.array([[x1, y1], [x3, y3], [x2, y2], [x4, y4]], np.int32)
    # -1表示该纬度靠后面的纬度自动计算出来，实际上是4
    # print("pts = ",pts)

    pts = pts.reshape((-1, 1, 2,))
    cv2.polylines(img, [pts], isClosed=True, color=(0, 0, 0), thickness=2)
    # cv2.imshow('img', eve)

    pts = np.float32([[x1, y1], [x4, y4], [x3, y3], [x2, y2]])
    pts2 = np.float32([[0, 0], [300, 0], [0, 300], [300, 300]])
    M = cv2.getPerspectiveTransform(pts, pts2)
    dst = cv2.warpPerspective(img, M, (300, 300))
    # cv2.imshow('img2',dst)
    # cv2.waitKey(0)
    (mean, stddv) = cv2.meanStdDev(dst)
    # print("mean = ", mean)
    # print("stddv = ", stddv)
    time_end = time.time()
    # print('def6cost', time_end - time_start, 's')
    return dst


def cell_judgment(dst):
    time_start = time.time()
    imgwidth = dst.shape[0]
    imgheight = dst.shape[1]
    npim = np.zeros((imgheight, imgwidth), dtype=int)
    npim[:] = dst[:] / 255
    index = np.zeros((8, 8), dtype=int)

    x1 = 0
    y1 = 0
    M = imgwidth // 8
    N = imgheight // 8
    # print(M)
    # standard = (imgwidth*imgheight)/100
    skip = round(M/5)

    standard = 20
    # standard = round((M*N)/70)

    # print(standard)

    for x in range(0, imgwidth, M):
        for y in range(0, imgheight, N):
            x1 = x + M
            y1 = y + N
            tiles = dst[x:x + M, y:y + N]
            # cv2.rectangle(dst, (x, y), (x1, y1), (0, 0, 255))

    # cv2.imshow("dst2", dst)
    # cv2.waitKey(0)
    for i in range(0, 8, 1):
        for j in range(0, 8, 1):
            for x in range(i * M + skip, i * M + M - skip, 1):
                for y in range(j * N + skip, j * N + N - skip, 1):
                    if npim[x, y] == 1:
                        temp = 1
                    else:
                        temp = 0
                    index[i, j] += temp
            if index[i, j] >= standard or i == 0 or i == 7 or j == 0 or j == 7:
                index[i, j] = 1
            else:
                index[i, j] = 0

    # cv2.imshow("dst2", dst)
    time_end = time.time()
    # print('def7cost', time_end - time_start, 's')
    return index


# flag = -1   true
# flag = -2   false
# flag >= 0   ID
def detect(index, times):
    time_start = time.time()
    for i in range(0, 30, 1):
        countgrid = 0
        ID = -1
        flag = True
        f = open("E:/Desktop/Code/ARUCOTXT/" + str(i) + ".txt")
        matrix = f.read()
        # print(matrix)
        # print(index)
        y = 0
        for j in range(0, 8, 1):
            for k in range(0, 8, 1):
                count = j * 8 + k + y
                if str(matrix[count]) == str(index[j, k]):
                    countgrid = countgrid + 1

                # if str(matrix[count]) != str(index[j, k]):
                #     flag = False
                #     break
            y += 1

        if j == 7 and countgrid <= 60:
            flag = False
            # print(index)

        if flag:
            break

    if flag:
        # print("顺时针旋转", times * 90, "度识别成功，识别对应txt文件id为", i)
        ID = i
        print("Indentified successfully, ID=", ID)
    # else:
    #     print("顺时针旋转", times * 90, "度识别失败")
    # else:
    #     print("Failed")

    return flag, ID


def identify(dst, img):
    time_start = time.time()
    index = cell_judgment(dst)
    for i in range(0, 4, 1):
        flag, ID = detect(index, i)
        if flag:
            times = i
            break
        index = np.rot90(index, 1)


    # if flag:
    #     cv2.putText(img, "times:" + str(times) + " ID:" + str(ID), (30, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255),
    #                 1, cv2.LINE_AA)
    # time_end = time.time()
    # # print('def8cost', time_end - time_start, 's')
    return flag


def detect_part(img,corners):
    time_start = time.time()
    # 5.显示结果
    # cv2.imshow("img_src", fanse)
    #
    # cv2.waitKey()
    # cv2.destroyAllWindows()

    # img = GaussianFilter(img)
    # img = cv2.blur (img,(8,8))
    # ret, thresh1 = cv2.threshold(img, 188, 255, cv2.THRESH_TOZERO)  # binary （黑白二值）
    # cv2.imshow('thresh',thresh1)

    # img = cv2.blur (fanse,(3,3))
    # eve = cv2.blur (fanse,(3,3))

    # cv2.imshow('123',img)
    temp_img = img
    # img = cv2.resize(img, None, fx=1, fy=1, interpolation=cv2.INTER_CUBIC)
    # cv2.imshow('111', img)
    # cv2.waitKey(0)
    corner_flag, x11, y11, temp_img, corners = select_corners(temp_img,corners)

    # plt.imshow(img1), plt.show()

    #
    # img1 = cv2.imread("ARUCO21.jpg")
    # img2 = cv2.imread("ARUCO22.jpg")

    # eve = cv2.absdiff(img1,img2)

    # eve=cv2.resize(eve,None,fx=1,fy=1,interpolation=cv2.INTER_CUBIC)

    # eve2 = cv2.blur (eve,(30,30))
    # eve2 = eve
    #
    # gray = eve2
    # gray = cv2.cvtColor(eve2, cv2.COLOR_BGR2GRAY)
    # ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    # #print("threshold value %s" % ret)  #打印阈值，超过阈值显示为白色，低于该阈值显示为黑色
    # #cv2.imshow("threshold", binary) #显示二值化图像
    # gray = 255 - binary
    # cv2.imshow("123", gray) #显示二值化图像
    # cv2.imshow('dst', dst)

    # cv2.imwrite("test21.jpg",dst)

    # cv2.waitKey(0)
    # detect
    # 读取图片
    # frame=cv2.imread('test22.jpg')
    # 调整图片大小
    # frame = dst;

    # frame=cv2.resize(frame,None,fx=1,fy=1,interpolation=cv2.INTER_CUBIC)

    # -----------------------------------------

    # img = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    ##

    # 图像转二值图

    # x11 = []
    # y11 = []

    # for c in corners:
    #     # 找到边界坐标
    #     x, y, w, h = cv2.boundingRect(c)  # 计算点集最外面的矩形边界
    #     # 因为这里面包含了，图像本身那个最大的框，所以用了if，来剔除那个图像本身的值。
    #     if x != 0 and y != 0 and w != frame.shape[1] and h != frame.shape[0]:
    #         # 左上角坐标和右下角坐标
    #         # 如果执行里面的这个画框，就是分别来画的，
    #         # cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 0), 1)
    #         x11.append(x)
    #         y11.append(y)
    # x11.append(x + w)
    # y11.append(y + h)
    # x1 = min(x11)
    # x11.pop(x1)
    # x2 = max(x11)
    # x11.pop(x2)
    # y3 = max(y11)
    # y11.pop(y3)
    # y4 = min(y11)
    # y11.pop(y4)
    # image = rotate(img, box)
    dst = img
    corner = 0
    if corner_flag:
        x1, y1, x2, y2, x3, y3, x4, y4, corner = select_the_four_corners(x11, y11, temp_img)
        dst = affine_transformation(x1, y1, x2, y2, x3, y3, x4, y4, img)
    time_end = time.time()
    # print('def9cost', time_end - time_start, 's')
    return corner_flag, dst, corner, corners


def get_contour(array):
    """获取连通域

    :param img: 输入图片
    :return: 最大连通域
    """
    time_start = time.time()
    blurred = img
    # blurred = cv2.pyrMeanShiftFiltering(img, 10, 80)
    # gray = cv2.cvtColor(blurred, cv2.COLOR_BGR2GRAY)
    gray = img

    # cv2.imshow("gray", gray)
    # t, binary = cv2.threshold(gray, 20, 255, cv2.THRESH_TOZERO)

    standard = 80

    # t, binary1 = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    # t, temp_binary = cv2.threshold(gray, 150, 255, cv2.THRESH_TOZERO_INV)
    # t, binary2 = cv2.threshold(temp_binary, standard, 255, cv2.THRESH_TOZERO)
    # t, binary2 = cv2.threshold(temp_binary, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)

    # cv2.imshow("binary2",binary2)
    # contours1, hierarchy = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # binary = cv2.drawContours(binary, contours1, -1, (255, 255, 255), 2)
    img1 = cv2.bitwise_not(binary1)
    img2 = cv2.bitwise_not(binary2)

    # # 灰度化, 二值化, 连通域分析
    # gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # # cv2.imshow("gray", gray)
    # # t, binary = cv2.threshold(gray, 20, 255, cv2.THRESH_TOZERO)
    #
    # standard = 80
    #
    # t, binary1 = cv2.threshold(gray, 150, 255, cv2.THRESH_BINARY)
    # t, temp_binary = cv2.threshold(gray, 150, 255, cv2.THRESH_TOZERO_INV)
    # t, binary2 = cv2.threshold(temp_binary, standard, 255, cv2.THRESH_TOZERO)
    # # t, binary2 = cv2.threshold(temp_binary, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    #
    # # contours1, hierarchy = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # # binary = cv2.drawContours(binary, contours1, -1, (255, 255, 255), 2)
    # img1 = cv2.bitwise_not(binary1)
    # img2 = cv2.bitwise_not(binary2)
    # img1 = cv2.cvtColor(img1, cv2.COLOR_GRAY2RGB)
    # img2 = cv2.cvtColor(img2, cv2.COLOR_GRAY2RGB)
    # img1 = cv2.pyrMeanShiftFiltering(img1, 10, 50)
    # img2 = cv2.pyrMeanShiftFiltering(img2, 10, 50)
    # t, img1 = cv2.threshold(img1, 150, 255, cv2.THRESH_TOZERO)
    # t, img2 = cv2.threshold(img2, 150, 255, cv2.THRESH_TOZERO)
    # kernel = np.ones((3, 3), np.uint8)
    # img1 = cv2.erode(img1, kernel, iterations=1)
    # img2 = cv2.erode(img2, kernel, iterations=1)
    # cv2.imshow("1", img1)
    # cv2.imshow("2", img2)
    # cv2.waitKey(0)
    time_end = time.time()
    # print('def10cost', time_end - time_start, 's')
    return img1, img2


def poseestimate(corner):
    cameraMatrix = np.array([[1.3376667907724209e+03, 0., 6.2504931658445537e+02],
                             [0.,1.3376667907724209e+03, 3.7959956600073838e+02],
                             [0., 0., 1.]], np.float32)
    distCoeffs = np.array([ -1.0885556876987361e-01, 4.2649835956791732e-01,
            -1.2981244349286130e-03, 3.4821656936060650e-04, 0.])
    rvec, tvec, _ = aruco.estimatePoseSingleMarkers(corner, 0.05, cameraMatrix, distCoeffs)
    print("rvec=",rvec,"tvec=",tvec)
    return 1

def rvectvec(corner):
    cameraMatrix = np.array([[1.3376667907724209e+03, 0., 6.2504931658445537e+02],
                             [0.,1.3376667907724209e+03, 3.7959956600073838e+02],
                             [0., 0., 1.]], np.float32)
    distCoeffs = np.array([ -1.0885556876987361e-01, 4.2649835956791732e-01,
            -1.2981244349286130e-03, 3.4821656936060650e-04, 0.])
    rvec, tvec, _ = aruco.estimatePoseSingleMarkers(corner, 0.05, cameraMatrix, distCoeffs)
    print("rvec=",rvec,"tvec=",tvec)
    return rvec,tvec

import numpy as np



def aruco_print(corner, corners, chief_img):
    time_start = time.time()

    # aruco_dict = aruco.Dictionary_get(cv2.aruco.DICT_6X6_250)
    # 使用默认值初始化检测器参数
    # parameters = aruco.DetectorParameters_create()
    # #使用aruco.detectMarkers()函数可以检测到marker，返回ID和标志板的4个角点坐标
    #
    # corners, ids, rejectedImgPoints = aruco.detectMarkers(chief_img, aruco_dict, parameters=parameters)

    # print("corners=", corners)
    # print(corners)
    # markerCenter = corners[0][0].sum(0) / 4.0
    # print(markerCenter)

    # objpixel = np.array([[0],[0],[0]])
    # cameraMatrix = np.array([[4841.96379551005,0,0,0,4847.77602188844,0,2669.54210074893,2052.20584545245,1]])
    # distCoeffs = np.array([[-0.184722445690629,0.150813162901775,0,0]])
    # retval,rvec,tvec = cv2.solvePnP(objpixel,markerCenter,cameraMatrix,distCoeffs)
    # print(retval)
    # print(rvec)
    # print(tvec)

    # mtx = [4841.96379551005,0,0,0,4847.77602188844,0,2669.54210074893,2052.20584545245,1]
    # dist = [-0.184722445690629,0.150813162901775,0,0]
    # rvec, tvec, _ = aruco.estimatePoseSingleMarkers(corner, 0.05, mtx, dist)
    # print(rvec,tvec)
    cameraMatrix = np.array([[1.3376667907724209e+03, 0., 6.2504931658445537e+02],
                             [0., 1.3376667907724209e+03, 3.7959956600073838e+02],
                             [0., 0., 1.]], np.float32)
    distCoeffs = np.array([-1.0885556876987361e-01, 4.2649835956791732e-01,
                           -1.2981244349286130e-03, 3.4821656936060650e-04, 0.])
    #
    # print("corner=", corner)
    # print("corners=", corners)

    rvec, tvec, _ = aruco.estimatePoseSingleMarkers(corner, 0.05, cameraMatrix, distCoeffs)
    print('rvec=', rvec, 'tvec=', tvec)
    print('')
    return tvec
    #
    # rotation_m = cv2.Rodrigues(rvec)
    # rotation_t = np.hstack([rotation_m[0], tvec])
    # return np.round(rotation_t[:, -1:], decimals=3)
    # print("rvec=",rvec,"tvec=",tvec)

    # cv2.drawFrameAxes(chief_img, cameraMatrix, distCoeffs, rvec, tvec, 0.03)
    # print(rvec, tvec)

    #
    # cv2.putText(chief_img, "rvec: " + str(rvec), (30, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA)
    # cv2.putText(chief_img, "tvec: " + str(tvec), (30, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA)


    # cv2.putText(chief_img, " ID:" + str(1), (410, 320), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1,cv2.LINE_AA)
    # aruco.drawDetectedMarkers(eve, corners)

    #
    #
    # ##
    # #画出标志位置
    #

    # img = GetApprox(img)

    # print(corners[1])
    # size = corners.size
    # i=0
    # for i in range(size):
    #     cv2.circle(chief_img, [1,1], 5, 255, -1)

    # cv2.imshow("Aruco_detect", chief_img)
    # cv2.waitKey(0)

    # cv2.imshow("ID20.jpg",eve)

    # cv2.imwrite("ID20.jpg",eve)
    time_end = time.time()
    # print('def11cost', time_end - time_start, 's')





def detect_img(img,corners):
    time_start = time.time()
    # img1, img2 = get_contour(img)
    # cv2.imshow("img1",img1)
    # cv2.imshow("img2",img2)
    #
    # cv2.imshow("img1", img1)
    corner1_flag, dst1, corner1, corners1 = detect_part(img,corners)
    flag = False
    if corner1_flag:
        # temp_dst2 = cv2.bitwise_not(dst1)
        # temp_dst1 = cv2.bitwise_not(dst2)
        # temp_dst_total = cv2.add(temp_dst1, temp_dst2)
        # dst_total = cv2.bitwise_not(temp_dst_total)
        # cv2.imshow("dst1", dst1)
        # cv2.imshow("dst2", dst2)
        # cv2.imshow("dst_total", dst_total)
        if identify(dst1, img):
            flag = True
            tvec = aruco_print(corner1, corners1, img)
            time_end = time.time()
            timecost = time_end - time_start
            # print(timecost)
            # csv_writer.writerow([tvec[0][0][0], tvec[0][0][1], tvec[0][0][2], timecost])

        # else:
        #     if identify(dst2, img):
        #         flag = True
        #     else:
        #         if identify(dst_total, img):
        #             flag = True
        #     aruco_print(corner2, corners2, img)
    # aruco_print(corner,corners)

    # 建立一个与图像尺寸相同的全零数组

    # 将图像3个通道相加赋值给空数组

    #####2023 1 13
    # for i in range(8,0,-1):
    #     for j in range (8,0,-1):
    #         for x in range(i*M, i*M+M, 1):
    #             for y in range(j*N,j*N+N, 1):
    #                 index[i, j] += npim[x, y];
    #         if(index[i,j]>1000):
    #             index[i,j] = 0;
    #         else:
    #             index[i,j] = 1;
    #
    # for i in range(0,8,1):
    #     for j in range (8,0,-1):
    #         for x in range(i*M, i*M+M, 1):
    #             for y in range(j*N,j*N+N, 1):
    #                 index[i, j] += npim[x, y];
    #         if(index[i,j]>1000):
    #             index[i,j] = 0;
    #         else:
    #             index[i,j] = 1;
    #
    # for i in range(8,0,-1):
    #     for j in range (0,8,1):
    #         for x in range(i*M, i*M+M, 1):
    #             for y in range(j*N,j*N+N, 1):
    #                 index[i, j] += npim[x, y];
    #         if(index[i,j]>1000):
    #             index[i,j] = 0;
    #         else:
    #             index[i,j] = 1;

    ####xiazhou zaishuo !

    # matplotShow(image)
    # cv2.imshow("123",image)

    # box = [20,30,70,30,100,50,50,50]
    # img = rotate(img, box)

    # 图片显示方法二选一

    # cv2.imshow("12345",img)

    ##-----------------------------------------

    # 灰度图
    # gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # 设置预定义的字典

    # cv2.waitKey(0)
    #
    # cv2.destroyAllWindows()
    time_end = time.time()
    timecost = time_end - time_start
    print("time cost = ",timecost)

    return flag






# if __name__ == '__main__':
#     sum = 160
#     success = 0
#     totalsum = 0
#     for i in range(sum):
#         time_start = time.time()
#         j = 501 + i
#         imgpath = 'E:/Desktop/Code/static30/'+str(j)+'.jpg'
#
#         chief_img = cv2.imread(imgpath)
#         # axis(chief_img)
#         # pola(chief_img)
#
#         if detect_img(chief_img):
#             print("图片" + str(j) + "识别成功")
#             totalsum += 1
#         else:
#             print("图片" + str(j) + "识别失败")
#         time_end = time.time()
#
#         print("total cost = ",time_end - time_start,'s')
#     print("识别成功" + str(totalsum) + "次")

# if __name__ == '__main__':
#     successnum = 0
#     totalnum = 0
#     videopath = 'Dynamic_30cm_cut_10ms.avi'
#
#     videocapture = cv2.VideoCapture(videopath)
#     # videocapture = cv2.VideoCapture(0) #摄像头方式
#     success, frame = videocapture.read()
#     acc = float()
#
#     while True:
#         if not success:
#             break
#         time_start = time.time()
#         totalnum = totalnum + 1
#         # print("framenum：", totalnum)
#         if (detect_img(frame)):
#             successnum = successnum + 1
#         success, frame = videocapture.read()
#         # cv2.imshow("123",frame)
#         # cv2.waitKey(0)
#         if (totalnum != 0):
#             acc = successnum / totalnum
#         else:
#             acc = 0
#         time_end = time.time()
#     print("Succeed " + str(successnum) + " times " + "out of " + str(totalnum)  + ", the accuracy is: " + str(acc))
#     # print("totally cost ",time_end - time_start,'s')
#         # cv2.imshow("123",frame)
#         # cv2.waitKey(20)
#     # cv2.destroyAllWindows()


def main():
    success = 0
    totalsum = 0
    args = parse_args()


    print("Code sample showing how to create a simple application testing different noise filtering strategies.")
    print("Available keyboard options:\n"
          "  - A: Filter events using the activity noise filter algorithm\n"
          "  - T: Filter events using the trail filter algorithm\n"
          "  - S: Filter events using the spatio temporal contrast algorithm\n"
          "  - E: Show all events\n"
          "  - Q/Escape: Quit the application\n")
    # csv_file = open('E:/desktop/code/marker_location100.csv','w',encoding='utf-8',newline='')
    # csv_writer = csv.writer(csv_file)
    # csv_writer.writerow(['x','y','z','processing_time'])

    # timestart = time.time()

    # Events iterator on Camera or RAW file
    mv_iterator = EventsIterator(input_path='D:/DaChuang_Prophese/Dynamic_30cm.raw', delta_t=20000)
    # mv_iterator = EventsIterator(input_path='C:/Users/bjtuy/Dynamic_30cm_cut.raw')
    # mv_iterator = EventsIterator(input_path=args.input_path,delta_t=10000)
    height, width = mv_iterator.get_size()  # Camera Geometry



    # #1.2KHz
    frequency_filter = metavision_sdk_cv.FrequencyAlgorithm(width=width, height=height, min_freq=1., max_freq=2000.)
    frequency_clustering_filter = metavision_sdk_cv.FrequencyClusteringAlgorithm(width=width, height=height,
                                                                                 min_cluster_size=1,
                                                                                 max_time_diff=1500)
    freq_buffer = frequency_filter.get_empty_output_buffer()
    cluster_buffer = frequency_clustering_filter.get_empty_output_buffer()

    TRAIL_THRESHOLD = 1000
    trail = metavision_sdk_cv.TrailFilterAlgorithm(width=width, height=height, threshold=TRAIL_THRESHOLD)


    im = np.zeros((height,width,3),dtype=np.uint8)

    if not is_live_camera(args.input_path):
         mv_iterator = LiveReplayEventsIterator(mv_iterator)

    global_counter = 0  # This will track how many events we processed
    global_max_t = 0  # This will track the highest timestamp we processed

    count = 0
    kernel_1 = np.ones((2, 2), np.uint8)
    kernel_2 = np.ones((3, 3), np.uint8)
    filters = {Filter.ACTIVITY: ActivityNoiseFilterAlgorithm(width, height, activity_time_ths),
               Filter.TRAIL: TrailFilterAlgorithm(width, height, trail_filter_ths),
               Filter.STC: SpatioTemporalContrastAlgorithm(width, height, stc_filter_ths, stc_cut_trail)
               }

    events_buf = ActivityNoiseFilterAlgorithm.get_empty_output_buffer()

    filter_type = Filter.NONE

    event_frame_gen = PeriodicFrameGenerationAlgorithm(sensor_width=width, sensor_height=height,
                                                       accumulation_time_us=5000, fps=300.0, palette=ColorPalette.Dark)
    #
    # Window - Graphical User Interface
    with MTWindow(title="Metavision SDK Get Started", width=width, height=height,
                mode=BaseWindow.RenderMode.BGR) as window:

        def keyboard_cb(key, scancode, action, mods):
            nonlocal filter_type

            if action != UIAction.RELEASE:
                return
            if key == UIKeyEvent.KEY_ESCAPE or key == UIKeyEvent.KEY_Q:
                window.set_close_flag()
            elif key == UIKeyEvent.KEY_E:
                # Show all events
                filter_type = Filter.NONE
            elif key == UIKeyEvent.KEY_A:
                # Filter events using the activity filter algorithm
                filter_type = Filter.ACTIVITY
            elif key == UIKeyEvent.KEY_T:
                # Filter events using the trail filter algorithm
                filter_type = Filter.TRAIL
            elif key == UIKeyEvent.KEY_S:
                # Filter events using the spatio temporal contrast algorithm
                filter_type = Filter.STC

        window.set_keyboard_callback(keyboard_cb)

        def on_cd_frame_cb(ts,cd_frame):
            window.show_async(cd_frame)

        event_frame_gen.set_output_callback(on_cd_frame_cb)

    # event_frame_gen = PeriodicFrameGenerationAlgorithm(sensor_width=width, sensor_height=height,
    #                                                        accumulation_time_us=5000)

        # Process events
        for evs in mv_iterator:
            # print(evs)
            timestart = time.time()
            EventLoop.poll_and_dispatch()
            # ev_filtered_buffer = trail.get_empty_output_buffer()
            # evs = noise_filter(evs)

            if filter_type in filters:
                filters[filter_type].process_events(evs, events_buf)
                event_frame_gen.process_events(events_buf)
                BaseFrameGenerationAlgorithm.generate_frame(events_buf, im)

            else:
                event_frame_gen.process_events(evs)
                BaseFrameGenerationAlgorithm.generate_frame(evs, im)

            # cv2.imshow("im",im)
            # cv2.waitKey(0)


            #
            # BaseFrameGenerationAlgorithm.generate_frame(evs, im)



            times = time.time()
            frequency_filter.process_events(evs, freq_buffer)
            frequency_clustering_filter.process_events(freq_buffer, cluster_buffer)
            corners = np.vstack((cluster_buffer.numpy()['x'], cluster_buffer.numpy()['y'])).T

            # print(corners.shape)

            for cluster in cluster_buffer.numpy():
                x0 = int(cluster["x"]) - 10
                y0 = int(cluster["y"]) - 10
                cv2.rectangle(im, (x0, y0), (x0 + 20, y0 + 20), color=(0, 255, 0))
                cv2.putText(im, "id_{}: {} Hz".format(cluster["id"], int(cluster["frequency"])), (x0, y0 - 10),
                            cv2.FONT_HERSHEY_PLAIN,
                            1, (0, 255, 0), 1)
            # cv2.imshow("Events", im[..., ::-1])
            # cv2.waitKey(0)
            timee = time.time()
            # print("frecost=",timee-times)

            img = cv2.cvtColor(im, cv2.COLOR_RGB2GRAY)
            # img = cv2.erode(im, kernel_1, iterations=1)
            # print(img)
            # cv2.Waitkey(0)
            # cv2.imshow("123",img)
            timeend = time.time()
            timecost = timeend - timestart


            timestart1 = time.time()
            # detect_img(img,corners)
            if (detect_img(img,corners)):
                totalsum += 1
                # print("success")
            # else:
             #    print("failed")
                count += 1
                # EventLoop.poll_and_dispatch()
            # timeend = time.time()
            # print("cost = ",timeend - timestart)
            # event_frame_gen.process_events(evs)
            if window.should_close():
                break
        print(count)
                # timeend1 = time.time()
                # print('cost1=',timeend1 - timestart1)

        #     # Print the global statistics
        # duration_seconds = global_max_t / 1.0e6
        # # print(f"There were {global_counter} events in total.")
        # # print(f"The total duration was {duration_seconds:.2f} seconds.")
        # if duration_seconds >= 1:  # No need to print this statistics if the video was too short
        #     print(f"There were {global_counter / duration_seconds :.2f} events per second on average.")

        # for evs in mv_iterator:
        #
        #     # # Dispatch system events to the window
        #     EventLoop.poll_and_dispatch()
        #     event_frame_gen.process_events(evs)
            # print(evs);

            # cv2.waitKey(0)
            # print(evs)
            # if detect_img(evs):
            #     print("图片识别成功")
            #     totalsum += 1
            # else:
            #     print("图片识别失败")

        # print("识别成功" + str(totalsum) + "次")
        # timeend = time.time()
        # print('cost=', timeend - timestart)

        # cv2.waitKey(0)

# if __name__ == '__main__':
#     success = 0
#     totalsum = 0
#     args = parse_args()
#
#     csv_file = open('E:/desktop/code/marker_location100.csv','w',encoding='utf-8',newline='')
#     csv_writer = csv.writer(csv_file)
#     csv_writer.writerow(['x','y','z','processing_time'])
#
#     # timestart = time.time()
#
#     # Events iterator on Camera or RAW file
#     # mv_iterator = EventsIterator(input_path='C:/Users/bjtuy/dynamic_30cm_cut.raw', delta_t=10000)
#     # mv_iterator = EventsIterator(input_path='C:/Users/bjtuy/Dynamic_30cm_cut.raw')
#     mv_iterator = EventsIterator(input_path=args.input_path,delta_t=10000)
#     height, width = mv_iterator.get_size()  # Camera Geometry
#     global_counter = 0  # This will track how many events we processed
#     global_max_t = 0  # This will track the highest timestamp we processed
#     count = 0
#     kernel_1 = np.ones((2, 2), np.uint8)
#     kernel_2 = np.ones((3, 3), np.uint8)
#
#     event_frame_gen = PeriodicFrameGenerationAlgorithm(sensor_width=width,sensor_height=height,accumulation_time_us=10000)
#     im = np.zeros((height, width, 3), dtype=np.uint8)
#
#
#     for evs in mv_iterator:
#          # print(evs)
#         timestart = time.time()
#         BaseFrameGenerationAlgorithm.generate_frame(evs, im)
#         cv2.imshow("123",im)
#         # cv2.waitKey(0)
#         # array_0 = np.zeros((height, width), int)
#         # array_1 = np.zeros((height, width), int)
#         #  # print("----- New event buffer! -----")
#         # if evs.size == 0:
#         #     print("The current event buffer is empty.")
#         # else:
#         #      # min_t = evs['t'][0]  # Get the timestamp of the first event of this callback
#         #      # max_t = evs['t'][-1]  # Get the timestamp of the last event of this callback
#         #      # global_max_t = max_t  # Events are ordered by timestamp, so the current last event has the highest timestamp
#         #     #
#         #      # counter = evs.size  # Local counter
#         #      # global_counter += counter  # Increase global counter
#         #      #
#         #     # print(f"There were {counter} events in this event buffer.")
#         #      # print(f"There were {global_counter} total events up to now.")
#         #      # print(f"The current event buffer included events from {min_t} to {max_t} microseconds.")
#         #      # print("----- End of the event buffer! -----")
#         #
#         #     for point in evs:
#         #          #
#         #          # if point[2] == 0:
#         #          #     array_0[point[1]][point[0]] = 255
#         #          if point[2] == 1:
#         #             array_1[point[1]][point[0]] = 255
#         #
#         #     set_array(evs, array_1)
#         #
#         #     #
#         #     # cv2.imwrite('E:/Desktop/0/' + str(count) + '.png', array_0)
#         #     # cv2.imwrite('E:/Desktop/1/' + str(count) + '.png', array_1)
#         #
#         #     # array_2 = cv2.imread('E:/Desktop/1/' + str(count_1) + '.png', cv2.IMREAD_UNCHANGED)
#         #     # array_2 = cv2.erode(array_2, kernel_1, iterations=1)
#         #     # # array_2 = cv2.dilate(array_2, kernel_2, iterations=1)
#         #     # cv2.imwrite('E:/Desktop/2/' + str(count_1) + '.png', array_2)
#         #     img = np.array(array_1, np.uint8)
#
#         img = cv2.erode(im, kernel_1, iterations=1)
#         # cv2.imshow("img",img)
#         # cv2.waitKey(0)
#         timeend = time.time()
#         timecost = timeend - timestart
#
#         # print("cost = ",timeend - timestart)
#         # timestart1 = time.time()
#         if (detect_img(img)):
#             totalsum += 1
#             # print("success")
#         # else:
#         #   print("failed")
#             count += 1
#
#         # print("识别成功" + str(totalsum) + "次")
#         # timeend = time.time()
#         # print('cost=', timeend - timestart)
#
#         # cv2.waitKey(0)


if __name__ == "__main__":
    main()

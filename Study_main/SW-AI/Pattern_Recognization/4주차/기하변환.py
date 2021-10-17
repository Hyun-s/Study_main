import cv2
import numpy as np
import matplotlib.pyplot as plt
from math import cos, sin, pi

plt.figure(figsize=(17, 5))

img = cv2.imread('hand.jpg')
img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
height = img.shape[0]
width = img.shape[1]
plt.subplot(1, 3, 1)
plt.imshow(img)
plt.axis('off')
plt.title('original')


# OpenCV를 이용한 변환 행렬 도출
center = (width / 2, height / 2)
cv_M = cv2.getRotationMatrix2D(center, 90, 1.0)  # 회전 방향이 반시계방향(CCW; Counter Clock-Wise)
cv_result = cv2.warpAffine(img, cv_M, (width, height))
print('>> OpenCV Rotation matrix')
print(cv_M, end='\n\n')

plt.subplot(1, 3, 2)
plt.imshow(cv_result)
plt.axis('off')
plt.title('cv_result')


# 행렬 구하기 위한 변수
h,w,_ = img.shape
center = (height/2,width/2) # y,x
theta = -pi/2

# 원점 복귀 행렬
m1 = np.array([[1, 0, center[1]],
               [0, 1, center[0]],
               [0, 0, 1]])

# 회전 변환 행렬
m2 = np.array([[cos(theta), -sin(theta), 0],
               [sin(theta),  cos(theta), 0],
               [0        ,          0, 1]])

# 원점 이동 행렬
m3 = np.array([[1, 0, -center[1]],
               [0, 1, -center[0]],
               [0, 0, 1]])

my_M = m1@m2@m3

# 출력 결과
print('>> My matrix')
print(my_M)
my_result = cv2.warpAffine(img, my_M[:2,], (width, height))

plt.subplot(1, 3, 3)
plt.imshow(my_result)
plt.axis('off')
plt.title('my_result')

# figure 출력
plt.tight_layout()
plt.show()
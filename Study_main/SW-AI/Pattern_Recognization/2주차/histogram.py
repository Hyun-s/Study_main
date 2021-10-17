import cv2
import matplotlib.pyplot as plt
#%matplotlib inline

# 이미지 읽기
img = cv2.imread('img1.jpg')
plt.imshow(img)
plt.title('RGB')
plt.show()

# 이미지를 grayscale로 변환
img_gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
plt.imshow(img_gray,cmap='gray')
plt.title('gray_scale')
plt.show()
print(img_gray.shape)

# 2중 for문을 활용한 histogram 계산
hist = [0]*256
h,w = img_gray.shape
for x in range(w):
    for y in range(h):
        hist[img_gray[y][x]]+=1
plt.title('for')
plt.bar(range(256),hist)
plt.show()

# cv2를 활용한 histogram 계산
hist_cv2 = cv2.calcHist([img_gray],[0],None,[256],[0,256])
hist_cv2 = hist_cv2.reshape(-1)
plt.bar(range(256),hist_cv2)
plt.title('cv2')
plt.show()
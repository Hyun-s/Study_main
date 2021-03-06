import numpy as np
import cv2


def set_region_of_interest(img, vertices):
    """

    :param img:       대상 이미지
    :param vertices:  이미지에서 남기고자 하는 영역의 꼭짓점 좌표 리스트
    :return:
    관심 영역만 마스킹 된 이미지
    """

    mask = np.zeros_like(img)
    cv2.fillPoly(mask, vertices, 255)

    return cv2.bitwise_and(img, mask)


def run(img):
    height, width = img.shape[:2]

    vertices = np.array([[(50, height),
                          (width // 2 - 45, height // 2 + 60),
                          (width // 2 + 45, height // 2 + 60),
                          (width - 50, height)]])

    # 1) BGR -> GRAY 영상으로 색 변환
    gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # 2) 이미지 내 노이즈를 완화시키기 위해 blur 효과 적용
    blur_img = cv2.GaussianBlur(gray_img, (15, 15), 0)

    # 3) 캐니 엣지 검출을 사용하여 엣지 영상 검출
    edge_img = cv2.Canny(blur_img,130,145)

    # 4) 관심 영역(ROI; Region Of Interest)을 설정하여 배경 영역 제외
    roi_img = set_region_of_interest(edge_img,vertices)

    # 5) 허프 변환을 사용하여 조건을 만족하는 직선 검출
    line_list = cv2.HoughLinesP(roi_img,1,np.pi/180,30, maxLineGap=40)
    try:
        line_list = line_list.reshape(len(line_list),2,2)
    except:
        print('Line not found')
        return img
    # 6) 찾은 직선들을 입력 이미지에 그리기
    for p1, p2 in line_list:
        img = cv2.line(img,p1,p2,(0,0,255),3)
    return img

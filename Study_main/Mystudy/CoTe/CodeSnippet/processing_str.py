# -*- coding: utf-8 -*-
"""
Created on Tue Mar 23 16:21:37 2021

@author: user
"""

# 특수문자 빼고 영어, 숫자 전부 소문자로
def str_to_list(st):
    s = s.lower()
    list_str = []
    for ch in s:
        if('a'<= ch  and ch <= 'z') or ('0'<= ch and ch<='9'):
            list_str.append(ch)
    return list_str

def list_reverse(ls):
    return ls[::-1]
# -*- coding: utf-8 -*-
"""
Created on Mon Mar 29 14:43:44 2021

@author: user
"""
from collections import defaultdict
from typing import List
import  re

class Solution:
    def mostCommonWord(self, paragraph: str, banned: List[str]) -> str:
        word_dict = defaultdict(int)
        words = [word for word in re.sub(r'[^\w]',' ',paragraph).lower().split()
                if  word not in banned]
        for word in words:
            word_dict[word] +=1
        
        return max(word_dict,key=word_dict.get)
    
paragraph = "Bob hit a ball, the hit BALL flew far after it was hit."

banned = ["hit"]
sol = Solution()
print(sol.mostCommonWord(paragraph, banned))
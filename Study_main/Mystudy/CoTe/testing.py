# -*- coding: utf-8 -*-
"""
Created on Mon Mar 29 14:15:33 2021

@author: user
"""
from collections import defaultdict
output=defaultdict(list)
strs = ["eat","tea","tan","ate","nat","bat"]
class MySolution:
    def groupAnagrams(self, strs: List[str]) -> List[List[str]]:
        output=defaultdict(list)
        for word in strs:
            tmp = ','.join(word).split(',')
            tmp.sort()
            output[''.join(tmp)].append(word)
        return list(output.values())
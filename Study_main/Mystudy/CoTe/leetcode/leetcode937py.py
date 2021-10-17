# -*- coding: utf-8 -*-
"""
Created on Mon Mar 29 14:33:29 2021

@author: user
"""

class Solution:
    def reorderLogFiles(self, logs: List[str]) -> List[str]:
        digls = []
        letls = []
        for log in logs:
            if log.split()[1].isdigit():
                digls.append(log)
            else:
                letls.append(log)
        letls.sort(key = lambda x : (x.split()[1:],x.split()[0]))
        return letls + digls
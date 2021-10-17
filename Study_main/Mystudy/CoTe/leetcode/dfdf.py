# -*- coding: utf-8 -*-
"""
Created on Mon Mar 22 15:15:43 2021

@author: user
"""

#1 ~ 100까지의 숫자를 출력하되, 3의 배수들은 “Fizz”, 5의 배수들을 “Buzz”,
# 3과 5의 배수들은 “FizzBuzz”를 출력하고 그 외는 그냥 숫자를 출력하는 
# 프로그램을 만들라.

for x in range(1,100+1):
    if x%3 ==0 and x%5 == 0:
        print("FizzBuzz",end='')
    elif x % 3 == 0:
        print('Fizz',end='')
    elif x % 5 == 0:
        print('Buzz', end='')
    else:
        print(x,end='')
    print()
    
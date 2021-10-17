# my solution

class Solution_my:
    def isPalindrome(self, s: str) -> bool:
        s = s.lower()
        list_str = []
        for ch in s:
            if('a'<= ch  and ch <= 'z') or ('0'<= ch and ch<='9'):
                list_str.append(ch)
        for x in range(int(len(list_str)/2)):
            if list_str[x] != list_str[-x-1]:
                return False
        return True
    
# book solution
        
class Solution_book_base:
    def isPalindrome(self, s: str) -> bool:
        strs = []
        for char in s:
            if char.isalnum(): # isalnum 영문자 숫자일경우 true return
                strs.append(char.lower())
                
        while len(strs) > 1:
            if strs.pop(0) != strs.pop():
                return False
        return True
    
class Solution_book_deque:
    def isPalindrome(self, s: str) -> bool:
        strs: Deque = collections.deque()
        
        for char in s:
            if char.isalnum():
                strs.append(char.lower())
                
        while(len(strs)>1):
            if strs.popleft() != strs.pop():
                return False
        return True
    
class Solution_slicing:
    def isPalindrome(self, s: str) -> bool:
        s = s.lower()
        
        s = re.sub('[^a-z0-9]','',s)
        return s == s[::-1]
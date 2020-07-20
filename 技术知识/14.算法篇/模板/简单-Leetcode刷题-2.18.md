#### 1. Two sum
https://www.acwing.com/solution/LeetCode/content/47/  
https://leetcode.com/problems/two-sum/submissions/

题目描述
给定一个整型数组，要求返回两个数的下标，使得两数之和等于给定的目标值，要求同一个下标不能使用两次。
数据保证有且仅有一组解。  

样例  
给定数组 nums = [2, 7, 11, 15]，以及目标值 target = 9，

由于 nums[0] + nums[1] = 2 + 7 = 9,
所以 return [0, 1].

#### 代码：
```python
""""
循环一遍 nums 数组，在每步循环中我们做两件事：

1. 判断 target−nums[i] 是否在哈希表中；
2. 将nums[i]插入哈希表中
"""
class Solution(object):
    def twoSum(self, nums, target):
        """
        :type nums: List[int]
        :type target: int
        :rtype: List[int]
        """
        pre_num = {}
        for i in range(len(nums)):
            num = target - nums[i]
            if pre_num.has_key(num):
                # pre_num[num]始终小于i
                return [pre_num[num], i]
            else:
                pre_num[nums[i]] = i
```
#### 2.Reverse Integer 
https://www.acwing.com/solution/LeetCode/content/97/  
https://leetcode.com/problems/reverse-integer/description/  

题目描述
给定一个int型整数，请将各位数字反转。
如果结果大于INT_MAX或小于INT_MIN，请返回0.

样例1  
输入： 123  
输出： 321  
样例2  
输入：-123  
输出：-321  
样例3  
输入：120  
输出：21  

```python
""""
最大的整数2^31 -1
最小的整数-2^31
""""
class Solution(object):
    def reverse(self, x):
        """
        :type x: int
        :rtype: int
        """
        MAX_INT = 2**31 -1
        MIN_INT = -2**31
        tag = 1
        if x < 0:
            tag = -1
        x *= tag
        res = 0
        while(x):
            num = x %10
            x = x/10
            res = res*10 + num
        res *= tag
        if res > MAX_INT or res < MIN_INT:
            return 0
        else:
            return res
```

#### 3.Palindrome Number
https://www.acwing.com/solution/LeetCode/content/99/  
https://leetcode.com/problems/palindrome-number/description/

题目描述
判断一个整数的字符串表示是否是回文串。
提示：

负数因为负号的存在，不是回文串；
如果用Reverse Integer 题目中的做法，需要避免整数溢出的问题；

```Python
class Solution(object):
    def isPalindrome(self, x):
        """
        :type x: int
        :rtype: bool
        """
        res = True
        if x < 0:
            return False
        x_to_str = str(x)
        x_len = len(x_to_str)
        for i in range(x_len):
            if i <= x_len-i-1:
                if x_to_str[i] != x_to_str[x_len-i-1]:
                    res = False
                    break
            else:
                break
        return res
```

#### 4. Roman to Integer
https://www.acwing.com/solution/LeetCode/content/116/  
https://leetcode.com/problems/roman-to-integer/description/  

给出一个罗马数字（不超过3999），将其转化为阿拉伯数字。
关于罗马数字的定义参见Integer to Roman。  
样例  
Input: "III"
Output: 3

Input: "IV"
Output: 4

Input: "IX"
Output: 9

Input: "LVIII"
Output: 58

Input: "MCMXCIV"
Output: 1994

定义映射，将单一字母映射到数字。  
从前往后扫描，如果发现s[i+1]的数字比s[i]的数字大，那么累计s[i+1]-s[i]差值即可，并将i多向后移动一位；否则直接累计s[i]的值。

```python
# 要用while实现
# 对于 for loop, i+=1 不work
class Solution(object):
    def romanToInt(self, s):
        """
        :type s: str
        :rtype: int
        """
        sum = 0
        i = 0
        n = len(s)
        r_to_i = {'I':1, 'V': 5, 'X': 10, 'L': 50, 'C':100, 'D': 500, 'M': 1000}
        while (i < n):
            if i != n-1 and r_to_i[s[i]] < r_to_i[s[i+1]]:
                sum += r_to_i[s[i+1]] - r_to_i[s[i]]
                i += 1
            else:
                sum += r_to_i[s[i]]
            i += 1
        return sum 
```

#### 5.Longest Common Prefix

https://www.acwing.com/solution/LeetCode/content/58/

题目描述  
给定一个字符串(string)数组，要求返回所有字符串的最长公共前缀。

样例  
给定字符串数组 ["aaca", "aacb", "aaba"] ，返回最长公共前缀 "aa"。

解题思路：  
如果输入为空，返回空字符串  
如果不为空，以第一个字符串作为基准进行遍历，如果遇到第i个字符与其他字符串的第i个字符不等，我们直接通过用substr的方法直接取出共同前缀的子字符串。如果遍历完没有返回结果的话，第一个字符串就是公共前缀。
```python
class Solution(object):
    def longestCommonPrefix(self, strs):
        """
        :type strs: List[str]
        :rtype: str
        """
        if not strs:
            return ""
        for i in range(len(strs[0])):
            for j in range(len(strs)):
                # i>=len(strs[j]),此处应为>=。因为i是下标，len(strs[j])是字符串的长度
                if i >= len(strs[j]) or strs[0][i] != strs[j][i]:
                    return strs[0][:i]
        return strs[0]
```
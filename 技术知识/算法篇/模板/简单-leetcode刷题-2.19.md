#### 1. Valid Parentheses  
https://www.acwing.com/solution/LeetCode/content/68/

题目描述  
给定一个嵌套的括号序列，含有 ( ) [ ] { } 三种括号，判断序列是否合法。

样例  
"()" 和 "()[]{}" 是合法的；  
"(]" 和 "([)]" 不是合法的。


使用栈数据结构：
1. 遇到左括号，需要压栈。
2. 遇到右括号，判断栈顶是否和当前右括号匹配；若不匹配则返回false，否则匹配弹出栈顶。
3. 最后判断栈是否为空；若为空则合法，否则不合法。

```python
class Solution(object):
    def isValid(self, s):
        """
        :type s: str
        :rtype: bool
        """
        left_dict = {'(': 0, '{': 1, '[': 2}
        right_dict = {')':0, '}':1, ']':2}
        left_key = left_dict.keys()
        right_key = right_dict.keys()
        stack = []
        for x in s:
            if x in left_key:
                stack.append(x)
            if x in right_key:
                if not stack:
                    return False
                tmp = stack.pop()
                if right_dict[x] != left_dict[tmp]:
                    return False
        return not stack
```

#### 2.Merge Two Sorted Lists
https://www.acwing.com/solution/LeetCode/content/69/  
题目描述  
给出两个排好序的单向链表，返回合并排序后新的单向链表。

链表结点的数据结构：

struct ListNode {  
    int val;  
    ListNode *next;  
    ListNode(int x) : val(x), next(NULL) {}  
};  
样例  
输入 1->2->4, 1->3->4  
返回 1->1->2->3->4->4

算法  
(线性合并) O(n)O(n)
1. 新建头部的保护结点dummy，设置cur指针指向dummy。
2. 若当前l1指针指向的结点的值val比l2指针指向的结点的值val小，则令cur的next指针指向l1，且l1后移；否则指向l2，且l2后移。
3. 然后cur指针按照上一部设置好的位置后移。
4. 循环以上步骤直到l1或l2为空。
5. 将剩余的l1或l2接到cur指针后边。

时间复杂度  
两个链表各遍历一次，所以时间复杂度为O(n)。  

```python
# Definition for singly-linked list.
# class ListNode(object):
#     def __init__(self, x):
#         self.val = x
#         self.next = None

class Solution(object):
    def mergeTwoLists(self, l1, l2):
        """
        :type l1: ListNode
        :type l2: ListNode
        :rtype: ListNode
        """
        dummy = ListNode(0)
        cur = dummy
        while (l1 and l2):
            if l1.val <= l2.val:
                cur.next = l1
                l1 = l1.next
            else:
                cur.next = l2
                l2 = l2.next
            cur = cur.next
        cur.next = l1 if l1 else l2
        return dummy.next
```

#### 3.Remove Duplicates from Sorted Array

https://www.acwing.com/solution/LeetCode/content/77/  

题目描述  
给定一个有序的数组，设计原地算法移除数组中重复的数字。在原数组的基础上修改后，返回新数组的长度。
额外要求
不得占用额外空间去申请另一个数组，必须使用常数的空间。  
样例
输入：  
nums = [1, 1, 2]

输出：  
length = 2  
其中原数组已经修改为[1, 2]，在length = 2之后的数字无关紧要。  
算法  
(双指针移动) O(n)O(n)  
1. 如果nums的长度是0，直接返回0。
2. 初始令k为0，i从位置1开始遍历，若发现nums[i]和nums[k]不相等，则说明找到新的元素，并且nums[++k]赋值为nums[i]。
3. i向后移动直到末尾。

```python
class Solution(object):
    def removeDuplicates(self, nums):
        """
        :type nums: List[int]
        :rtype: int
        """
        k = 0
        if len(nums) == 0:
            return 0
        for i in range(1, len(nums)):
            if nums[i] != nums[k]:
                k += 1
                nums[k] = nums[i]
        # k是新数组的下标，所以应该返回k+1
        return k+1
```
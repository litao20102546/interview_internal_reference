
vector (向量)
    vector<int> vec;
    empty(), push_back(), pop_back(),begin(), end(), front(), back(), size() vec[i]
    vector<char>::iterator iter1; iter1 = v1.end()-1; //注意v1.end()指向的是最后一个元素的下一个位置，所以访问最后一个元素
                        //的正确操作为：v1.end() - 1;

stack 
     stack<int> s
     top(), empty(), size(), pop(), push()

queue  
    queue<int> q1
    front(), back(), empty(), size(), pop(), push()

list （Lists将元素按顺序储存在链表中. 与 向量(vectors)相比, 它允许快速的插入和删除，但是随机访问却比较慢）
    front() 返回第一个元素                 back() 返回最后一个元素 
    begin() 返回指向第一个元素的迭代器          end() 返回末尾的迭代器
    pop_back() 删除最后一个元素             pop_front() 删除第一个元素 
    push_back() 在list的末尾添加一个元素    push_front() 在list的头部添加一个元素
    reverse() 把list的元素倒转                sort() 给list排序
    splice() 合并两个list                 size() 返回list中的元素个数 

set
    set<int> s;
    begin()返回set容器的第一个元素            end()返回set容器的最后一个元素
    clear()删除set容器中的所有的元素        empty()判断set容器是否为空
    size()返回当前set容器中的元素个数        max_size()返回set容器可能包含的元素最大个数
    rend()返回的值和rbegin()相同            rbegin返回的值和end()相同

unordered_map
    begin        返回指向容器起始位置的迭代器
    end            返回指向容器末尾位置的迭代器
    size        返回有效元素个数
    max_size    返回 unordered_map 支持的最大元素个数
    empty        判断是否为空
    insert        插入元素
    find        通过给定主键查找元素
    count       
unordered_set
    t.insert();
    t.erase();
    t.find();
    t.count();
string
    str.size()  "" 返回0
    str.empty() "" 返回1

总结：
    需要注意的点：
    1 双指针不断向后或者向前的走时候，判断条件要加上不能超出边界的限制

    对make_heap(), pop_heap(), push_heap()的用法做个总结：
    可以用伪函数less()和greater()来生成大顶堆和小顶堆
    push_heap()是在堆的基础上进行数据的插入操作，参数与make_heap()相同，需要注意的是，只有make_heap（）和push_heap（）同为大顶堆或小顶堆，才能插入。
    pop_heap()是在堆的基础上，弹出堆顶元素。这里需要注意的是，pop_heap()并没有删除元素，而是将堆顶元素和数组最后一个元素进行了替换，如果要删除这个元素，还需要对数组进行pop_back()操作。 

快速取余:int pos = (n - 1) & hash，也就是hash%n
//1 在有序矩阵中寻找一个值是否存在
    //(原一维数组中下标为i的元素将出现在二维数组中的[i/n][i%n]的位置)
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>

    bool findNum(int array[], int rows, int columns, int number){
        bool found = false;
        if(array == NULL || rows <= 0 || columns <= 0){
            return found;
        }
        int row = 0;
        int column = columns - 1;
        while(row < rows && column >= 0){
            if(array[row * columns + column] == number){
                found = true;
                break;
            }
            else if(array[row * columns + column] < number){
                row++;
            }
            else {
                column--;
            }
        }
        return found;
    }

    int main(){
        int array[16] = {1,2,8,9,2,4,9,12,4,7,10,13,6,8,11,15};
        printf("%d", findNum(array, 4, 4, 7));
        return 0;
    }

//2 替换字符串空格为%20

    #include <stdlib.h>
    #include <stdio.h>

    void ReplaceStr(char str[], int length){
        if (str == NULL){
            return;
        }

        //get number of blank
        int num_blank = 0;
        int i = 0;
        while(str[i] != '\0'){
            if (str[i] == ' '){
                num_blank++;
            }
            i++;
        }

        int index_last = length + 2 * num_blank - 1;
        int index_before = length - 1;
        while(index_before <= index_last){
            if(str[index_before] != ' '){
                str[index_last--] = str[index_before--];
            }
            else {
                str[index_last--] = '0';
                str[index_last--] = '2';
                str[index_last--] = '%';
                index_before--;
            }
        }
        return;
    }

    int main(){
        char arr[] = "We are happy.";
        printf("%s", arr);
        ReplaceStr(arr, strlen(arr));
        printf("%s", arr);
        return 0;
    }

//3 插入节点
    #include <stdlib.h>
    #include <stdio.h>

    struct ListNode{
        int value;
        ListNode* next;
    }

    void AddToTail(ListNode** pHead, int num){
        struct ListNode* tail = (ListNode* )malloc(sizeof(ListNode));
        //申明struct的时候指明struct ListNode{...} Node,则可以Node* tail,或者struct ListNode{...}* Node,则可以Node tail.
        //相当于typedef struct ListNode Node或者typedef struct ListNode* Node;
        if(!tail){
            printf("internal error\n");
            return;
        }
        tail->value = num;
        tail->next = NULL;

        if(*pHead == NULL){
            *pHead = tail;
        }
        else {
            ListNode* head = *pHead;
            while(head->next != NULL){
                head = head->next;
            }
            head->next = tail;
        }
        return;
    }

//4 找到第一个是某值的节点并删除

    #include <stdlib.h>
    #include <stdio.h>

    struct ListNode {
        int value;
        ListNode* next;
    }

    void RemoveOneNode(ListNode** pHead, int num) 
    {
        if(pHead == NULL || *pHead == NULL) {
            return;
        } 
        ListNode* toDeleteNode = NULL;
        if((*pHead)->value == num)
        {
            toDeleteNode = *pHead;
            *pHead = (*pHead)->next;//必须加这一句，否则删除头结点之后后面节点也断了
        }    
        else
        {
            ListNode* pNode = *pHead;
            while(pNode->next != NULL) 
            {
                if(pNode->next->value == num)
                {
                    toDeleteNode = pNode->next;
                    pNode->next = pNode->next->next;
                }
                else 
                {
                    pNode = pNdoe->next;
                }

            }        
        }
        
        if(!toDeleteNode)
        {
            delete toDeleteNode;
            toDeleteNode = NULL;
        }
        return;
    }

//5 从后打印链表节点值
    //C++栈应用：std::stack<ListNode*> nodes;
    void PrintNodeValue(ListNode* pHead)
    {
        if(pHead == NULL)
        {
            return;
        }
        std::stack<ListNode*> nodes;
        ListNode* pNode = pHead;
        while(pNode->next != NULL)
        {
            nodes.push(pNode);
        }
        while(!nodes.empty())//不能用nodes.top()判断
        {
            printf("%d \t", (nodes.top()->value);
            nodes.pop();
        }
        return;
    }

    //利用栈增加了空间对内存的消耗，可以利用递归实现：
    void PrintNodeValue(ListNode* pHead)
    {
        if(pHead != NULL)
        {
            ListNode* pNode = pHead;
            PrintNodeValue(pNode->next);
            printf("%d \t", pHead->value);
        }
    }
    //但是这种递归方法在列表很长的时候可能会造成函数调用栈溢出，这样的话显示用栈基于循环的做法鲁棒性更好一些

//6 构建二叉树
    struct BinTreeNode
    {
        int value;
        BinTreeNode* left;
        BinTreeNode* right;
    }

    BinTreeNode* Construct(int* preorder, int* inorder, int length)
    {
        if(preorder == NULL || inorder == NULL || length <= 0)
        {
            return NULL;
        }
        return ConstructCore(preorder, preorder + length -1, inorder, inorder + length -1);
    }

    BinTreeNode* ConstructCore(int* startPreorder, int* endPreorder, int* startInorder, int* endInorder)
    {
        //前序遍历根节点的值
        BinTreeNode* root = new BinTreeNode();
        int rootvalue = startPreorder[0]
        root->value = rootvalue;
        root->left = root->right = NULL;

        if(startPreorder == endPreorder)
        {
            if((startInorder == endInorder) && (*startPreorder == *startInorder))
            {
                return root;
            }
            else
            {
                throw std::exception("Invalid input\n")
            }
        }

        //中序序列中寻找根节点位置e
        int* rootInorder = startInorder;
        while((rootInorder <= endInorder) && (*rootInorder != rootvalue))
        {
            rootInorder++;
        }
        if((rootInorder == endInorder) && (*rootInorder != rootvalue)){
            throw std::exception("Invalid input\n");
        }

        int leftlength = rootInorder - startInorder;
        int leftPreorderEnd = startPreorder + leftlength;
        if(leftlength > 0)
        {
            //构建左子树
            root->left = ConstructCore(startPreorder + 1, leftPreorderEnd, startInorder, rootInorder -1);
        }
        if(leftlength < endPreorder - startPreorder)
        {
            //构建右子树
            root->right = ConstructCore(leftPreorderEnd + 1, endPreorder, rootInorder + 1, endInorder);
        }
        return root;
    }

//7 排序 （稳定性是看两个相等的数，一次排序后相对位置是否变化）
    //https://blog.csdn.net/u010305706/article/details/51207444
    //九大排序 :1）插入排序：
    //             直接插入排序（从后往前，稳定，最好n,最差n的平方，平均时间复杂度n的平方），
    //             希尔排序，（逐渐分组排序，最后增量为1，不稳定，平均时间复杂度nlogn）
    //             二分法插入排序 (从后往前，稳定，与正反序无关，移动和直接插入相同，查询位置平均少，平均时间复杂度n的平方)
    //        2）交换排序：
    //             冒泡排序 （大数往下沉，小数往上冒，稳定，正序复杂度n，反序n的平方，平均时间复杂度n的平方）
    //             快速排序 【递归调用导致nlogn空间复杂度】（基数，小于基数放左边，大于基数放右边，不稳定，时间复杂度为nlogn,数比较大的时候用比较合适，基本正序的时候反而不好用）
    //        3) 选择排序：
    //             简单选择排序 (选择最小的放到最前面，不稳定，时间复杂度n的平方)
    //             堆排序 （建立堆，不断剔除最大数，不稳定，最坏nlogn,建立堆需要多判断，不宜记录数小的文件）
    //        4) 归并排序：【空间复杂度n】（分组，子序列先排序再合并，稳定，时间复杂度nlogn，常用于总体无序，子项相对有序, 实现merge时需要用到辅助数组）
    //        5) 基数排序：（从最低位，按照位排序，稳定，稳定性d(n+r)，d位数，r基数）
    //
    //堆排序：https://www.cnblogs.com/chengxiao/p/6129630.html
    //其中构建初始堆经推导复杂度为O(n)，在交换并重建堆的过程中，需交换n-1次，而重建堆的过程中，根据完全二叉树的性质，
    //[log2(n-1),log2(n-2)...1]逐步递减，近似为nlogn。所以堆排序时间复杂度一般认为就是O(nlogn)级。
    //建堆从最后一个非叶子结点开始length/2-1
    //
    //
    //
    //
    //
    //
    //    
    //#define left(x) 2*x+1;//获得左节点在数组中的下标
    //#define right(x) 2*(x+1);//获得右节点在数组中的下标
    // void MaxHeapify(int* a,int i,int low,int high)//输入为要被排序的数组和根节点，数组a当中被维护的那一部分的下标low，high
    // {
    //     int l = left(i);//计算下标为i的节点的左子节点
    //     int r = right(i);//计算下标为i的节点的右子节点
    //     int largest;//保存i,l,r(即i和它的左右子节点)之间的最大数的下标
    //     int temp;//交互数组中的数所使用的临时变量
    //     //找到三个数当中最大的那个数，将最大的那个数和i进行互换
    //     if (l<=high && a[l]>a[i])
    //     {
    //         largest = l;
    //     }
    //     else{
    //         largest = i;
    //     }

    //     if (r<=high && a[r]>a[largest])
    //     {
    //         largest = r;
    //     }
    //     if (largest != i)
    //     {
    //         temp = a[i];
    //         a[i] = a[largest];
    //         a[largest] = temp;
    //         MaxHeapify(a, largest,low,high);//交换有可能破坏子树的最大堆性质，所以对所交换的那个子节点进行一次维护，而未交换的那个子节点，根据我们的假设，是保持着最大堆性质的。
    //     }
    // }
    // void BuildMaxHeap(int* a,int length)
    // {
    //     for (int i = length/2 - 1; i >= 0; i--)
    //     {
    //         MaxHeapify(a, i, 0, length - 1);
    //     }
    // }
    //堆排序函数
    // void HeapSort(int a[],int length)
    // {
    //     int temp;
    //     BuildMaxHeap(a,length);
    //     for (int i = length - 1; i >= 1; i--)
    //     {
    //         //交换根节点和数组的最后一个节点
    //         temp = a[i];
    //         a[i] = a[0];
    //         a[0] = temp;
    //         MaxHeapify(a, 0, 0, i-1);//维护从下标为i-1到0的子数组
    //     }
    // }

    //归并排序
    void merge_sort(int *data, int left, int right)
    {
        if(left<right){
             int middle = (left+right)/2;
             //对左边进行递归
             mergeSort(a, left, middle);
             //对右边进行递归
             mergeSort(a, middle+1, right);
             //合并
             merge(a,left,middle,right);
    }

    void merge(int *data,int start,int end)
    {
        int tmpArr[a.length];
        int mid = middle+1; //右边的起始位置
         int tmp = left;
         int third = left;
         while(left<=middle && mid<=right){
             //从两个数组中选取较小的数放入中间数组
             if(a[left]<=a[mid]){
                 tmparr[third++] = a[left++];
             }else{
                 tmparr[third++] = a[mid++];
             }
         }
         //将剩余的部分放入中间数组
         while(left<=middle){
             tmparr[third++] = a[left++];
         }
         while(mid<=right){
             tmparr[third++] = a[mid++];
         }
         //将中间数组复制回原数组
         while(tmp<=right){
             a[tmp] = tmparr[tmp++];
         }
    }
//8 快排 (选一个基数，通常选第一个或者最后一个，利用前后双指针将小于基数的数放到左边，大于基数的数放到右边，再把基数放到对应的位置，利用迭代完成，递归次数log2n，第一次partition 为n)
    #include<stdio.h>
    #include<string.h>
    int partition(int data[], int low, int high)
    {
        if(data == NULL || low < 0)
        {
            printf("internal error\n");
        }
        int temp = data[low];
        while(low < high){
            while((low < high) && (data[high] >= temp)){
                high--;
            }
            data[low] = data[high];
            while((low < high) && (data[low] <= temp)){
                low++;
            }
            data[high] = data[low];

        }
        data[low] = temp;
        return low;

    }

    void QuickSort(int data[], int start, int end)
    {
        if(start < end){
            int index = partition(data, start, end);
            QuickSort(data, start, index - 1);
            QuickSort(data, index + 1, end);
        }
    }

    int main()
    {
        int a[14] = {49,38,65,97,76,13,27,49,78,34,12,64,1,8};
        int i;
        printf("quick sort before\n");
        for(i = 0;i<14;i++){
                printf("%d\t",a[i]);
        }

        QuickSort(a, 0, 13);
        printf("quick sort after\n");
        for(i = 0;i<14;i++){
                printf("%d\t",a[i]);
        }

    }

//9 旋转数组找最小值
    int findMinNum(int* data, int length)
    {
        if(data == NULL && length < 0){
            throw std:: exception("Invalid parameter\n");
        }

        int start = 0;
        int end = length - 1;
        int Midindex = start;
        int result = data[start];
        while(data[start] > data[end])
        {
            if(end - start == 1)
            {    
                result = data[end];
                break;//容易丢掉
            }
            Midindex = (start + end)/2;
            if(data[start] == data[end] && data[end] == data[Midindex])
            {
                return findMin(data, start, end);//解决start,Midindex, end下标的值相同的情况
            }
            if(data[start] <= data[Midindex])
            {
                start = Midindex;
            }
            else if(data[end] >= data[Midindex])
            {
                end = Midindex;
            }
        }
        return result;
    }

    int findMin(int* data, int start, int end)
    {
        int res = data[start];
        int i;
        for(i = start + 1; i <= end; i++)
        {
            if(data[i] < res)
            {
                res = data[i];
            }
        }
        return res;
    }

//10 左边放奇数，右边放偶数 (扩展性，加分项)
  void SortArray(int* data, int length, bool (*func)(int))
  {
      //判断初始参数有效性
      int start = 0;
      int end = length - 1;
      while(start < end)
      {
          while(start < end && !func(data[start]))
          {
              start++;
          }
          while(start < end && func(data[end]))
          {
              end--;
          }
          swap(data[start], data[end]);
      }
      return;
  }

  bool RightValue(int n)
  {
      return (n & 1) == 0;
  }
  SortArray(data, length, RightValue)

//11 链表中倒数第k个节点
  ListNode* findKthNode(ListNode* pHead, unsigned int k)
  {
      if(pHead == NULL || k <= 0)
      {
          return NULL;
      }

      int i;
      ListNode* firstNode = pHead;
      ListNode* secondNode = pHead;
      for(i = 0;i < k; i++)
      {
          if(firstNode == NULL){
              throw std:: exception("error input parameter\n");
          }
          else {
              firstNode = firstNode->next;
          }
      }
      while(firstNode != NULL)
      {
          firstNode = firstNode->next;
          secondNode = secondNode->next;
      }
      return secondNode;
  }

//12 链表的反转
  ListNode* reverseList(ListNode* pHead)
  {
      ListNode* reverseHead = NULL;
      ListNode* pNode = pHead;
      ListNode* preNode = NULL;
      while(pNode != NULL)
      {
          ListNode* pNext = pNode->next;
          if(pNext == NULL)
          {
              reverseHead = pNode;
          }
          pNode->next = preNode;
          preNode = pNode;
          pNode = pNext;
      }
      return reverseHead;
  }

//13 链表的合并
  ListNode* merge(ListNode* pHead1, ListNode* pHead2)
  {
      if(pHead1 == NULL)
      {
          return pHead2;
      }
      else if(pHead2 == NULL)
      {
          return PHead1;
      }
      ListNode* head = NULL;
      if(pHead1->value > pHead2->value)
      {
          head = pHead2;
          head->next = merge(pHead1, pHead2->next);
      }
      else
      {
          head = pHead1;
          head->next = merge(pHead1->next, pHead2);
      }
      return head;
  }

//14 按照层打印节点值，即广度优先遍历，必须用到队列
    /*
    #include <iostream>
    #include <queue>
    #include <assert.h>

    调用的时候要有头文件： #include<stdlib.h> 或 #include<cstdlib> +
    #include<queue>       #include<queue>
    详细用法:
    定义一个queue的变量     queue<Type> M
    查看是否为空范例        M.empty()    是的话返回1，不是返回0;
    从已有元素后面增加元素   M.push()
    输出现有元素的个数      M.size()
    显示第一个元素          M.front()
    显示最后一个元素        M.back()
    清除第一个元素          M.pop()
    */
    void PrintFromTopToBottom(BinTreeNode* root)
    {
        if(root == NULL)
        {
            return;
        }
        queue<BinTreeNode*> bintreelist;
        bintreelist.push(root);
        while(bintreelist.size())
        {
            ListNode* temp = bintreelist.front();
            bintreelist.pop();
            printf("%d\t", temp->value);
            if(temp->left)
            {
                bintreelist.push(temp->left);
            }
            if(temp->right)
            {
                bintreelist.push(temp->right);
            }
        }
    }

//15 二叉搜索树后序遍历，给定序列，判断是否为二叉搜索树后序遍历
    bool IsSquenceofBST(int data[], int length)
    {
        // 边界条件
        if(data == NULL || length <= 0)
        {
            return false;
        }
        // 划分左右子树，并判断左右子树和根节点的关系
        int i = 0;
        int root = data[length - 1];
        for(; i < length - 1; i++)
        {
            if(data[i] > root)
            {
                break;
            }
        }
        int j = i;
        for(; j < length - 1; j++)
        {
            if(data[j] < root)
            {
                return false;
            }
        }
        //遍历二叉树左子树
        bool left = true;
        if(i > 0)
            left = IsSquenceofBST(data, i);
        //遍历二叉树右子树
        bool right = true;
        if(i < length - 1)
            right = IsSquenceofBST(data + i, length - i + 1);
        return left && right;
    }

//16 打印二叉树满足和为某值的路径 (利用vector保存路径)
    void PrintPath(BinTreeNode* pRoot, int total)
    {
        if(BinTreeNode == NULL)
            return;
        std::vector(int) path;
        int cursum = 0;
        FindNum(pRoot, path, total, cursum);
    }

    void FindNum(BinTreeNode* pRoot, std::vector(int)& path, int total, int cursum)
    {
        path.push_back(pRoot->value);
        cursum += pRoot->value;
        bool isLeaf = pRoot->left == NULL && pRoot->right == NULL;
        if(isLeaf && (cursum == total))
        {
            //print
            std::vector<int>::iterator iter = path.begain();
            for(;iter != path.end(); iter++)
            {
                printf("%d\t", iter):
            }
            printf("\n");

        }
        if(pRoot->left != NULL)
        {
            FindNum(pRoot->left, path, total, cursum);
        }
        if(pRoot->right != NULL)
        {
            FindNum(pRoot->right, path, total, cursum);
        }
        path.pop_back();
        return;    
    }

//17 字符串的排列 (第二个swap是为了还原字符串顺序， pbegin+1)
    void swap(char* c1,char* c2){
        char tmp=*c1;
        *c1=*c2;
        *c2=tmp;
    }
     
    void permutation(char* pstr,char* pbegin){
        if(*pbegin=='\0')
            printf("%s\n",pstr);
        else{
            for(char* pCh=pbegin;*pCh!='\0';pCh++){
                swap(pbegin,pCh);
                permutation(pstr,pbegin+1);
                swap(pbegin,pCh);
            }
        }
    }
     
    void permutationStr(char* pstr){
        if(pstr==NULL)
            return;
        permutation(pstr,pstr);
    }

//18 复杂链表的复制

    struct ComplexListNode {
        int value;
        ComplexListNode *next, 
        ComplexListNode *sibling;
    };

    void CloneNodes(ComplexListNode* pHead)
    {
        ComplexListNode* pNode = pHead;
        while(pNode != NULL)
        {
            ComplexListNode* pCloned = new ComplexListNode();
            pCloned->value = pNode->value;
            pCloned->next = pNode->next;
            pCloned->sibling = NULL;
            pNode->next = pCloned;
            pNode = pCloned->next;
        }
    }

    void ConnectSiblingNodes(ComplexListNode* pHead)
    {
        ComplexListNode* pNode = pHead;
        while(pNode ! = NULL)
        {
            ComplexListNode* pCloned = pNode->next;
            if(pNode->sibling != NULL)
            {
                pCloned->sibling = pNode->sibling->next;
            }    
            pNode = pCloned->next;                
        }
    }

    ComplexListNode* ReconnectNodes(ComplexListNode* pHead)
    {
        ComplexListNode* pNode = pHead;
        ComplexListNode* pClonedHead = NULL;
        ComplexListNode* pClonedNode = NULL;

        if(pNode->next != NULL)
        {
            pClonedHead = pClonedNode = pNode->next;
            pNode->next = pClonedNode->next;
            pNode = pNode->next;
        }

        while(pNode != NULL)
        {
            pClonedNode->next = pNode->next;
            pClonedNode = pClonedNode->next;
            pNode->next = pClonedNode->next;
            pNode = pNode->next;
        }

        return pClonedHead;
    }

    ComplexListNode* Clone(ComplexListNode* pHead)
    {
        CloneNodes(pHead);
        ConnectSiblingNodes(pHead);
        return ReconnectNodes(pHead);
    }

//19 二叉树与双向链表（中序遍历）
    struct BinaryTreeNode
    {
        int value;
        BinaryTreeNode* left;
        BinaryTreeNode* right;
    }

    BinaryTreeNode* Convert(BinaryTreeNode* pRootOfTree)
    {
        BinaryTreeNode* pLastNodeInList = NULL;
        ConvertNodes(pRootOfTree, &pLastNodeInList);
        BinaryTreeNode* pHeadNodeInList = pLastNodeInList;
        while(pHeadNodeInList != NULL && pHeadNodeInList->left != NULL)
        {
            pHeadNodeInList = pHeadNodeInList->left;
        }
        return pHeadNodeInList;
    }

    void ConvertNodes(BinaryTreeNode* pNode, BinaryTreeNode** pLastNodeInList)
    {
        if(pNode == NULL)
            return;
        BinaryTreeNode* current = pNode;
        // BinaryTreeNode* pNodeList = *pLastNodeInList;
        if(current->left != NULL)
            ConvertNodes(current->left, pLastNodeInList);

        current->left = *pLastNodeInList;
        if(*pLastNodeInList != NULL)
            (*pLastNodeInList)->right = current;
        *pLastNodeInList = current;

        if(current->right != NULL)
            ConvertNodes(current->right, pLastNodeInList);
    }

//20 第一个只出现一次的字符
    char FirstOneChar(char* string)
    {
        if(string == NULL)
            return '\0';
        const int TableSize = 256;
        unsigned int hashTable[TableSize];
        int i;
        for(i = 0; i < TableSize; i++)
        {
            hashTable[i] = 0;
        }
        char* strIndex = string;
        while(*strIndex != '\0')
        {
            hashTable[*(strIndex++)]++;
        }
        strIndex = string;
        while(*strIndex != '\0')
        {
            if(hashTable[*strIndex] == 1)
                return *strIndex;
            strIndex++;
        }
        return '\0';

    }

//21 2sum (暴力两次遍历，n2, 快排+双指针，nlogn, hashmap n, map.count(temp)查找元素的个数，unordered_map)
    //note: 找的是index
    class Solution {
    public:
        vector<int> twoSum(vector<int>& nums, int target) {
            vector<int> result;
            unordered_map<int, int> map;
            int numsSize = nums.size();
            int i;
            for(i = 0; i < numsSize; i++) {
                int temp = target - nums[i];
                if(map.count(temp)) 
                {
                    result.push_back(map[temp]);
                    result.push_back(i);
                    break;
                }
                map[nums[i]] = i;
            }
            return result;
        }
    };

//22 3sum (注意每一个边界值及其增减)
    //     第二种方法是先取数组中一个数，问题就转化为2 sum的问题求解，当数组中所有的数都被选定过后，问题也就解决了。例如“abcdf”，先取a，那么问题就是在bcdf中寻找两个数的和为target-a，然后取b,这个时候问题
    //  转化为cdf中寻找和为target-b的2 sum问题，a就不必再算入找寻找集合中，为什么？简单的证明即可：
    //      第一次取a，问题转化为bcdf重寻找和为target-a的2 sum问题，假设其中一个数是b,那么有：
    //       1.如果找不一个数加上a+b等于target，当选定b为第一个数时，如果第二个数选到了a，依旧不会有数满足该条件；
    //       2.如果能找到一个数加上a+b等于target，那么这个数时唯一确定的，当选定b为第一个数时，如果第二个数又选到a,那么这个数也唯一确定，且和选定a为第一个数，b为第二个数的最后一个数相等，那么结果集重复。
    //  再回想选取第一个数和第二个数的方式，就可以知道，在选定一个数为第一个数后，这个数后面的数都会依次被选取，如果在后面的选取中，有选择了前面的数，那么结果集肯定会重复，这样就增加了不必要的选取和计算，影响效率。
    //
    //  利用hash还是排序搜索？单从方法上讲，前者肯定优于后者，但有一个问题，如果用hash，选取第一个数时，需要遍历一次数组，再存入hash，再检查hash，
    //  而C++提供的map不支持改变KEY，相对比较麻烦，排序搜索的方式，只需要排序、移动首尾指针，再检查。
    class Solution {
    public:
        vector<vector<int>> threeSum(vector<int>& nums) {
            vector<vector<int>> result;
            int numsSize = nums.size();
            sort(nums.begin(), nums.end());
            int index_start, index_end;
            for(int i = 0; i < numsSize - 2; i++) {
                if(i > 0 && nums[i] == nums[i - 1])
                    continue;
                index_start = i + 1;
                index_end = numsSize - 1;
                int target = 0 - nums[i];
                while(index_start < index_end) {
                    if(nums[index_start] + nums[index_end] == target) {
                        result.push_back({nums[i], nums[index_start], nums[index_end]});
                        while(nums[index_start] == nums[index_start + 1] && index_start < index_end )
                            index_start++;
                        while(nums[index_end] == nums[index_end - 1] && index_start < index_end )
                            index_end--; 
                        index_start++;
                        index_end--;
                    } else if(nums[index_start] + nums[index_end] < target) {
                        index_start++;
                    } else {
                        index_end--;
                    }
                }
            }
            return result;
        }
    };

//23 3sum smaller(满足和小于target的组合个数 http://www.cnblogs.com/grandyang/p/5235086.html)
    // O(n^2)
    class Solution {
    public:
        int threeSumSmaller(vector<int>& nums, int target) {
            if (nums.size() < 3) return 0;
            int res = 0, n = nums.size();
            sort(nums.begin(), nums.end());
            for (int i = 0; i < n - 2; ++i) {
                if(i > 0 && nums[i] == nums[i - 1])
                    continue;                
                int left = i + 1, right = n - 1;
                while (left < right) {
                    if (nums[i] + nums[left] + nums[right] < target) {
                        res += right - left;
                        ++left;
                    } else {
                        --right;
                    }
                }
            }
            return res;
        }
    };

//24 3sum closest (http://www.cnblogs.com/grandyang/p/4510984.html)
    class Solution {
    public:
        int threeSumClosest(vector<int>& nums, int target) {
            if (nums.size() < 3) return 0;
            int closest = nums[0] + nums[1] + nums[2];
            int diff = abs(closest - target);
            sort(nums.begin(), nums.end());
            for (int i = 0; i < nums.size() - 2; ++i) {
                int left = i + 1, right = nums.size() - 1;
                while (left < right) {
                    int sum = nums[i] + nums[left] + nums[right];
                    int newDiff = abs(sum - target);
                    if (diff > newDiff) {
                        diff = newDiff;
                        closest = sum;
                    }
                    if (sum < target) ++left;
                    else --right;
                }
            }
            return closest;
        }
    };

//25 Two Sum III  (Data structure design) for(auto a : m)
    class TwoSum {
    public:
        void add(int number) {
            ++m[number];
        }
        bool find(int value) {
            for (auto a : m) {
                int t = value - a.first;
                if ((t != a.first && m.count(t)) || (t == a.first && a.second > 1)) {
                    return true;
                }
            }
            return false;
        }
    private:
        unordered_map<int, int> m;
    };

//26 Add Two Numbers (返回res->next, )
    class Solution {
    public:
        ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
            ListNode* result = new ListNode(0);
            ListNode* res = result;
            int sum = 0;
            while(l1 != NULL || l2 != NULL) {
                if(l1 != NULL) {
                    sum += l1->val;
                    l1 = l1->next;
                }
                if(l2 != NULL) {
                    sum += l2->val;
                    l2 = l2->next;
                }
                result = (result->next = new ListNode(sum % 10));
                sum = sum / 10;
            }
            if(sum) {
                result->next = new ListNode(1);
            }
            return res->next;
        }
    };

//27 PathSum III (找到二叉树中的子路径条数使得该子路径权值之和与给定和相等 利用hashmap更快 on)
    //若sum为从根节点到某x节点的路径权值和，则遍历至节点x时，当前的路径和curSum恰好与sum相等，则res = m[curSum - sum] = m[0] = 1;
    //若sum为某段子路径权值和，如：x1->x2->x3->x4......中sum等于节点x3与节点x4的权值和，即sum = sumx3+x4。
    //则遍历至x2时， m[curSum]++; 处已经记录了m[curSum] = m[sumx1+x2] = 1,遍历至x4时curSum = sumx1+x2+x3+x4,
    //则res = m[curSum - sum] = m[sumx1+x2+x3+x4 - sumx3+x4] = m[sumx1+x2] = 1。
    class Solution {
    public:
        int pathSum(TreeNode* root, int sum) {
            unordered_map<int, int> m;
            m[0] = 1;
            return helper(root, sum, 0, m);
        }
        int helper(TreeNode* node, int sum, int curSum, unordered_map<int, int>& m) {
            if (!node) return 0;
            curSum += node->val;
            int res = m[curSum - sum];
            ++m[curSum];
            res += helper(node->left, sum, curSum, m) + helper(node->right, sum, curSum, m);
            --m[curSum];
            return res;
        }
    };
//28 Path Sum IV (http://bookshadow.com/weblog/2017/08/30/leetcode-path-sum-iv/)

//29  Binary Tree Paths (int 转 string to_string(value))   [DFS]
    class Solution {
    public:
        void findPath(TreeNode* root, string path, vector<string> &result) {
            if(root->left == NULL && root->right == NULL){
                result.push_back(path);
            } 
            if(root->left != NULL) {
                findPath(root->left, path + "->" + to_string(root->left->val), result);
            }
            if(root->right != NULL) {
                findPath(root->right, path + "->" + to_string(root->right->val), result);
            }
        }
        vector<string> binaryTreePaths(TreeNode* root) {
            vector<string> result;
            if(root == NULL) {
                return result;
            }
            string path = to_string(root->val);
            findPath(root, path, result);
            return result;
        }
    };    

//*//30 Binary Tree Maximum Path Sum(保证左子树和右子树大于或者等于零，比较加上root和以前不加root最大的max)
    class Solution {
    public:
        int res_max = INT_MIN;;
        int findMaxPath(TreeNode* root) {
            if(root == NULL)
                return 0;
            int lmax = max(findMaxPath(root->left), 0);
            int rmax = max(findMaxPath(root->right), 0);
            res_max = max(res_max, lmax + rmax + root->val);
            return max(lmax, rmax) + root->val;
        }
        int maxPathSum(TreeNode* root) {
            if(root == NULL) {
                    return 0;
            }
            findMaxPath(root);
            return res_max;
        }
    };

//31 LRU (注意先更新map的node的value值，然后移动该node到List头，再用map指定该node)
    struct CacheNode{
        int key;
        int value;
        CacheNode(int k, int v):key(k), value(v){}
    };

    class LRUCache {
    private:
        list<CacheNode> cacheList;
        unordered_map<int, list<CacheNode>::iterator> cacheMap;
        int size;
    public:
        LRUCache(int capacity) {
            size = capacity;
        }
        
        int get(int key) {
            if(cacheMap.find(key) == cacheMap.end()) {
                return -1;
            } else {
                cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);//move的作用，move后cacheMap[key]为空需要指定
                cacheMap[key] = cacheList.begin();
                return cacheMap[key]->value;
            }
        }
        
        void put(int key, int value) {
            if(cacheMap.find(key) == cacheMap.end()) {
                if(cacheList.size() == size) {
                    cacheMap.erase(cacheList.back().key);
                    cacheList.pop_back();
                } 
                cacheList.push_front(CacheNode(key, value));
                cacheMap[key] = cacheList.begin();
            } else {
                cacheMap[key]->value = value;
                cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);
                cacheMap[key] = cacheList.begin();
            }
        }
    };

//32 Palindrome Number()
    public class Solution {
        bool isPalindrome(int x) {
            if(x<0) return false;
            int len=0;
            int temp=x;
            int large=1;
            while(temp>=10){
                large = large*10;
                temp = temp/10;
            }
            while(x>0){
                if(x/large!=x%10) return false;
                else{
                    x = x % large;
                    x = x / 10;
                    large = large /100;
                }
            }
            return true;
        }
    }

//33 Palindrome Permutation 回文全排列(auto map i->second)
    //Given a string, determine if a permutation of the string could form a palindrome.
    //For example,
    //"code" -> False, "aab" -> True, "carerac" -> True.
    class solution{
    public:
        bool isPalindrome(string str) {
            int count = 0;
            unordered_map<char, int> map;
            for(auto s : str) {
                map[s]++;
            }
            for(auto i = map.begin(); i != map.end(); i++) {
                if(i->second % 2) {
                    count++;
                }
            }
            return count == 0 || (str.size() % 2 == 1 && count == 1);
        }
    }

//34 Palindrome Permutation II 回文全排列之二
    //Given a string s, return all the palindromic permutations (without duplicates) of it. Return an empty list if no palindromic permutation could be form.
    //For example:
    //Given s = "aabb", return ["abba", "baab"].
    //Given s = "abc", return [].
    class Solution{
    public:
        vector<string> findAllPalindrome(string str) {
            unordered_map<char, int> map;
            unodered_set<string> res;
            string t = "", mid = "";
            for(auto s : str) {
                map[s]++;
            }
            for(auto i = map.begin(); i != map.end(); i++) {
                if(i->second % 2 == 1) {
                    mid += i->first;
                }
                t += string(i->second / 2, i->first);
            }
            if(mid.size() > 1) return {};
            permute(t, 0, mid, res);
            return vector<string>(res.begin(), res.end()); 
        }
        void permute(string &t, int start, string mid, unodered_set<string> &res) {
            if(start >= t.size()) {
                res.insert(t + mid + string(t.rbegin(), t.rend()));
            }
            for(int i = start; i < t.size(); i++) {
                if(i != start && t[i] == t[start]) {
                    continue;
                }
                swap(t[i], t[start]);
                permute(t, start + 1, mid, res);
                swap(t[i], t[start]);
            }
        }
    };

//35 Palindrome Linked List(if(fast) { slow = slow->next;}为了适应长度为奇数的列表)
    /**
     * Definition for singly-linked list.
     * struct ListNode {
     *     int val;
     *     ListNode *next;
     *     ListNode(int x) : val(x), next(NULL) {}
     * };
     */
    class Solution {
    public:
        ListNode* reverse(ListNode* node) {
            ListNode* temp = NULL;
            ListNode* pre = NULL;
            while(node) {
                temp = node->next;
                node->next = pre;
                pre = node;
                node = temp;
            }
            return pre;
        }
        bool isPalindrome(ListNode* head) {
            if(head == NULL || head->next == NULL)
                return true;
            ListNode* fast = head;
            ListNode* slow = head;
            while(fast && fast->next) {
                fast = fast->next->next;
                slow = slow->next;
            }
            //考虑到奇数个节点
            if(fast) {
                slow = slow->next;
            }
            slow = reverse(slow);
            
            while(slow) {
                if(head->val == slow->val) {
                    head = head->next;
                    slow = slow->next;
                } else {
                    return false;
                }
            }
            return true;
        }
    };

//36 Longest Palindrome(奇数个字符只能有一个是奇数)unordered_map
    class Solution {
    public:
        int longestPalindrome(string s) {
            unordered_map<char, int> map;
            bool flag = false;
            int res = 0;
            for(auto c : s) {
                map[c]++;
            }
            for(auto i = map.begin(); i != map.end(); i++) {
                res += i->second;
                if(i->second % 2 == 1) {
                    res -= 1;
                    flag = true;
                }
            }
            return flag ? res + 1: res;
        }
    };

//37 Ugly Number（只能被2，3，5整除的数）num >= 2
    class Solution {
    public:
        bool isUgly(int num) {
            while(num >= 2 ) {
                if(num % 2 == 0) {
                    num = num / 2;
                } else if(num % 3 == 0) {
                    num = num / 3;
                } else if(num % 5 == 0) {
                    num = num / 5;
                } else {
                    return false;
                }
            }
            return num == 1;
        }
    };

//38 Ugly Number II(找出第n个丑数)
    class Solution {
    public:
        int Min(int i, int j, int k) {
            int m = i > j ? j : i;
            return m > k ? k : m;
        }
        int nthUglyNumber(int n) {
            int min = 0;
            int uglyNums[n];
            int m2=0, m3=0, m5=0;
            int next = 1;
            uglyNums[0] = 1;
            while(next < n) {
                min = Min(uglyNums[m2]*2, uglyNums[m3]*3, uglyNums[m5]*5);
                uglyNums[next++] = min;
                if(min == uglyNums[m2]*2)
                    m2++;
                if(min == uglyNums[m3]*3)
                    m3++;
                if(min == uglyNums[m5]*5)
                    m5++;
            }
            return uglyNums[n - 1];
        }
    };

//39 Super Ugly Number
    class Solution {
    public:
        int nthSuperUglyNumber(int n, vector<int>& primes) {
            int min = 0;
            int next = 1;
            int uglyNums[n];
            uglyNums[0] = 1;
            int length = primes.size();
            int index[length];
            for(int i = 0; i < length; i++) {
                index[i] = 0;
            }
            while(next < n) {
                min = INT_MAX;
                for(int i = 0; i< length; i++) {
                    if(uglyNums[index[i]] * primes[i] < min) {
                        min = uglyNums[index[i]] * primes[i];
                    }
                }

                for(int i = 0; i< length; i++) {
                    if(uglyNums[index[i]] * primes[i] == min) {
                        index[i]++;
                    }
                }
                uglyNums[next++] = min;
            }
            return uglyNums[n - 1];

        }
    };

//40 Merge k Sorted Lists(利用优先队列排序 https://blog.csdn.net/c20182030/article/details/70757660)
    //priority_queue<Type, Container, Functional>，其中Type 为数据类型，Container为保存数据的容器，Functional 为元素比较方式
    struct cmp {
        bool operator () (ListNode *a, ListNode *b) {
            return a->val > b->val;
        }
    };
     
    class Solution {  
    public:  
        ListNode *mergeKLists(vector<ListNode *> &lists) {  
            priority_queue<ListNode*, vector<ListNode*>, cmp> q;
            for (int i = 0; i < lists.size(); ++i) {
                if (lists[i]) q.push(lists[i]);
            }
            ListNode *head = NULL, *pre = NULL, *tmp = NULL;
            while (!q.empty()) {
                tmp = q.top();
                q.pop();
                if (!pre) head = tmp;
                else pre->next = tmp;
                pre = tmp;
                if (tmp->next) q.push(tmp->next);
            }
            return head;
        }  
    };

//41 Kth Smallest Element in a Sorted Matrix
    //1)(利用最大堆而不是最小堆)
        class Solution {
        public:
            int kthSmallest(vector<vector<int>>& matrix, int k) {
                int rows = matrix.size();
                int columns = matrix[0].size();
                priority_queue<int> max_heap;
                for(int i = 0; i < rows; i++) {
                    for(int j = 0; j < columns; j++) {
                        max_heap.push(matrix[i][j]);
                        if(max_heap.size() > k) {
                            max_heap.pop();
                        }
                    }
                }
                return max_heap.top();
            }
        };

    //2)利用二分法（log(Max-Min)*n*log(n)）我们使用upper_bound，这个函数是查找第一个大于目标数的元素，
        //如果目标数在比该行的尾元素大，则upper_bound返回该行元素的个数，如果目标数比该行首元素小，则upper_bound返回0, 
        class Solution {
        public:
            int kthSmallest(vector<vector<int>>& matrix, int k) {
                int n = matrix.size(), left=matrix[0][0], right=matrix[n-1][n-1];
                while(left < right)
                {
                    long mid = (left+right)/2, num = 0;
                    for(int i = 0; i < n; i++)
                    {
                        auto it = upper_bound(matrix[i].begin(), matrix[i].end(), mid);
                        num += it - matrix[i].begin();
                    }
                    if(num < k) left = mid+1;
                    else right = mid;
                }
                return left;
            }
        }
    //3)利用列有序进行优化upper_bound，时间复杂度（nlog(Max-Min)）
        class Solution {
        public:
            int kthSmallest(vector<vector<int>>& matrix, int k) {
                int left = matrix[0][0], right = matrix.back().back();
                while (left < right) {
                    int mid = left + (right - left) / 2;
                    int cnt = search_less_equal(matrix, mid);
                    if (cnt < k) left = mid + 1;
                    else right = mid;
                }
                return left;
            }
            int search_less_equal(vector<vector<int>>& matrix, int target) {
                int n = matrix.size(), i = n - 1, j = 0, res = 0;
                while (i >= 0 && j < n) {
                    if (matrix[i][j] <= target) {
                        res += i + 1;
                        ++j;
                    } else {
                        --i;
                    }
                }
                return res;
            }
        };
//42 String to Integer (atoi) 注意：字符串为空，字符串空格，字符串符号， 字符串数字后非数字，边界值(INT_MAX = 2147483647)
    class Solution {
    public:
        int myAtoi(string str) {
            if(str.empty())
                return 0;
            int i = 0, flag = 1, base = 0, n = str.size();
            while(i < n && str[i] == ' ') {
                i++;
            }
            if(str[i] == '-' || str[i] == '+') {
                flag = (str[i++] == '-') ? -1 : 1;
            }
            while(i < n && str[i] >= '0' && str[i] <= '9') {
                if(base > INT_MAX / 10 || (base == INT_MAX / 10 && str[i] > '7')) {
                    return flag == -1 ? INT_MIN : INT_MAX;
                }
                base = base * 10 + str[i++] - '0';
            }
            return base * flag;
        }
    };

//43 Valid Parentheses (不能忘记最后st.empty())
    class Solution {
    public:
        bool check(char c1, char c2) {
            switch(c1) {
                case '}':
                    return c2 == '{';
                case ']':
                    return c2 == '[';
                case ')':
                    return c2 == '(';
                default:
                    return false;
            }
        }
        bool isValid(string s) {
            stack<char> st;
            int n = s.size();
            for(int i = 0; i < n; i++) {
                char c1 = s[i];
                if(c1 == '(' || c1 == '{' || c1 == '[') {
                    st.push(c1);
                } else {
                    if(st.empty()) return false;
                    if(!check(c1, st.top())) return false;
                    st.pop();
                }
            }
            return st.empty();
        }
    };

//44 Implement strStr() i 和 j都不能设置终点值，或者设置为长度+ 1
    class Solution {
    public:
        int strStr(string haystack, string needle) {
            for(int i = 0; ; i++) {
                for(int j = 0;; j++) {
                    if(j == needle.size()) 
                        return i;
                    if(i + j >= haystack.size())
                        return -1;
                    if(needle[j] != haystack[i+j])
                        break;
                }
            }
        }
    };

//45 Implement Stack using Queues pop动作除了back都先出后进
    class MyStack {
    private:
        queue<int> q;
        int size;
    public:
        /** Initialize your data structure here. */
        MyStack() {
            size = 0;
        }
        
        /** Push element x onto stack. */
        void push(int x) {
            q.push(x);
            size++;
        }
        
        /** Removes the element on top of the stack and returns that element. */
        int pop() {
            for(int i = 1; i < size; i++) {
                q.push(q.front());
                q.pop();
            }
            int res = q.front();
            q.pop();
            size--;
            return res;
        }
        
        /** Get the top element. */
        int top() {
            return q.back();
        }
        
        /** Returns whether the stack is empty. */
        bool empty() {
            return q.empty();
        }
    };

//46 Implement Queue using Stacks 两个堆栈，只有pop和top在s2 空的时候从s1移动到s2.
    class MyQueue {
    private:
        stack<int> s1, s2;
    public:
        /** Initialize your data structure here. */
        MyQueue() {
            
        }
        
        /** Push element x to the back of queue. */
        void push(int x) {
            s1.push(x);
        }
        
        /** Removes the element from in front of queue and returns that element. */
        int pop() {
            int res;
            if(s2.empty()) {
                while(!s1.empty()) {
                    int data = s1.top();
                    s2.push(data);
                    s1.pop();
                }
            }
            res = s2.top();
            s2.pop();
            return res;
        }
        
        /** Get the front element. */
        int peek() {
            int res;
            if(s2.empty()) {
                while(!s1.empty()) {
                    int data = s1.top();
                    s2.push(data);
                    s1.pop();
                }
            }
            res = s2.top();
            return res;        
        }
        
        /** Returns whether the queue is empty. */
        bool empty() {
            return s1.empty() && s2.empty();
        }
    };

//47 Word Ladder(同源最短，Dijkstra，此题权值均为1，用BFS实现) 往队列添加元素一定要把set中元素删除
    class Solution {
    public:
        int ladderLength(string beginWord, string endWord, unordered_set<string>& wordList) {
            wordList.insert(endWord);
            queue<pair<string,int>> que;
            que.push(make_pair(beginWord, 1));
            wordList.erase(wordList.find(beginWord));
            while(!que.empty()){
                auto val = que.front();
                que.pop();
                if(val.first == endWord) return val.second;
                for(int i = 0; i < val.first.size(); i++) {
                    string str = val.first;
                    for(int j = 0; j < 26; j++) {
                        str[i] = 'a' + j;
                        if(wordList.count(str) == 1) {
                            que.push(make_pair(str, val.second + 1));
                            wordList.erase(str);
                        }
                    }
                }
            }
            return 0;
        }
    };

//48 Word Ladder II (们设置一个hash表用来存储一个字符串的父结点集合, 这样C不在字典中再去查hash表看C是否在hash表中,
    // 如果在的话并且C的父结点层次和B一样, 那么就将B也加入到C的父结点结合中去. 可以知道, 一个字符串的父结点集合的距离起点的
    //距离必然是相等的, 也就是说他们都是最短距离.最后遍历完所有的点之后, 再用DFS从终点往前找出所有集合即可.)
    class Solution {
    public:
        void DFS(set<pair<string, int>> st, vector<string> vec)
        {
            for(auto val: st)
            {
                vec.push_back(val.first);
                if(hash.count(val.first)==0)
                {
                    reverse(vec.begin(), vec.end());
                    return result.push_back(vec);
                }
                DFS(hash[val.first], vec);
                vec.pop_back();
            }
        }
        
        vector<vector<string>> findLadders(string beginWord, string endWord, unordered_set<string> &wordList) {
            wordList.insert(endWord);
            wordList.erase(beginWord);
            que.push(make_pair(beginWord, 1));
            while(!que.empty())
            {
                auto val = que.front();
                que.pop();
                for(int i = 0; i < val.first.size(); i++)
                {
                    string tem = val.first;
                    for(int j =0; j < 26; j ++)
                    {
                        tem[i] = 'a' + j;
                        if(wordList.count(tem))
                        {
                            que.push(make_pair(tem, val.second+1));
                            wordList.erase(tem);
                            hash[tem].insert(val);
                        }
                        else if(hash.count(tem)&&hash[tem].begin()->second==val.second)
                            hash[tem].insert(val);
                    }
                }
            }
            DFS(hash[endWord], vector<string>{endWord});
            return result;
        }
        vector<vector<string>> result;
        unordered_map<string, set<pair<string, int>>> hash;
        queue<pair<string, int>> que;
    };

//49  Merge Intervals (考虑两个区间比较的三种情况，其中一种不需要操作，注意空值) sort 应用
    /**
     * Definition for an interval.
     * struct Interval {
     *     int start;
     *     int end;
     *     Interval() : start(0), end(0) {}
     *     Interval(int s, int e) : start(s), end(e) {}
     * };
     */
    class Solution {
    public:
        static bool cmp(const Interval& interval1, const Interval& interval2) {
            return interval1.start < interval2.start;
        }
        vector<Interval> merge(vector<Interval>& intervals) {
            if(intervals.empty()) return intervals;
            vector<Interval> result;
            sort(intervals.begin(), intervals.end(), cmp);
            result.push_back(intervals[0]);
            for(int i = 1; i < intervals.size(); i++) {
                Interval preIn = result.back();
                Interval curIn = intervals[i];
                if(preIn.end >= curIn.start && preIn.end <= curIn.end) {
                    preIn.end = curIn.end;
                    result.pop_back();
                    result.push_back(preIn);
                } else if (preIn.end <= curIn.start) {
                    result.push_back(curIn);
                }
            }
            return result;
        }
    };

//50 Insert Interval
    /**
     * Definition for an interval.
     * struct Interval {
     *     int start;
     *     int end;
     *     Interval() : start(0), end(0) {}
     *     Interval(int s, int e) : start(s), end(e) {}
     * };
     */
    class Solution {
    public:
        vector<Interval> insert(vector<Interval>& intervals, Interval newInterval) {
            vector<Interval> res;
            int cur = 0;
            for(int i = 0; i < intervals.size(); i++) {
                if(intervals[i].end < newInterval.start) {
                    res.push_back(intervals[i]);
                    cur++;
                } else if(intervals[i].start > newInterval.end) {
                    res.push_back(intervals[i]);
                } else {
                    newInterval.start = min(intervals[i].start, newInterval.start);
                    newInterval.end = max(intervals[i].end, newInterval.end);
                }
            }
            res.insert(res.begin() + cur, newInterval);
            return res;
        }
    };

//51 Number of Islands (将已经遍历的节点值更改为0，查看四个方向的DFS)
    class Solution {
    public:
        void search(vector<vector<char>>& grid, int x, int y) {
            if(x < 0 || x >= grid.size() || y < 0 || y >= grid[0].size() || grid[x][y] != '1') return;
            grid[x][y] = '0';
            search(grid, x - 1, y);
            search(grid, x + 1, y);
            search(grid, x, y - 1);
            search(grid, x, y + 1);
        }
        int numIslands(vector<vector<char>>& grid) {
            int count = 0;
            if(grid.empty()) return count;
            int rows = grid.size();
            int columns = grid[0].size();
            for(int i = 0; i < rows; i++) {
                for(int j = 0; j < columns; j++) {
                    if(grid[i][j] == '1') {
                        search(grid, i, j);
                        count++;
                    }
                    
                }
            }
            return count;
        }
    };

//52 Pow(x, n) 注意n < 0时的处理
    class Solution {
    public:
        double myPow(double x, int n) {
            if(n == 0) {
                return 1.0;
            } 
            if(n < 0) {
                return 1 / x * myPow(1 / x, - (n + 1));
            } 
            return n & 1 == 1 ? x * myPow(x * x, n / 2) : myPow(x * x, n / 2);
        }
    };

//53 Set Matrix Zeroes
    class Solution {
    public:
        void setZeroes(vector<vector<int>>& matrix) {
            int row = matrix.size();
            int col = matrix[0].size();
            bool row_flag = false;
            bool col_flag = false;
            
            for(int i = 0; i < row; i++) {
                if(matrix[i][0]== 0) {
                    row_flag = true;
                    break;
                }
            }
            for(int j = 0; j < col; j++) {
                if(matrix[0][j]== 0) {
                    col_flag = true;
                    break;
                }
            }

            for(int i = 1; i < row; i++) {
                for(int j = 1; j < col; j++) {
                    if(matrix[i][j] == 0) {
                        matrix[i][0] = 0;
                        matrix[0][j] = 0;
                    }
                }
            }

            for(int i = 1; i < row; i++) {
                if(matrix[i][0]== 0) {
                    for(int j = 1; j < col; j++) {
                        matrix[i][j] = 0;
                    }
                }
            } 
            for(int j = 1; j < col; j++) {
                if(matrix[0][j]== 0) {
                    for(int i = 1; i < row; i++) {
                        matrix[i][j] = 0;
                    }
                }
            }

            if(row_flag) {
                for(int i = 0; i < row; i++) {
                    matrix[i][0] = 0;
                }
            }
            if(col_flag) {
                for(int j = 0; j < col; j++) {
                    matrix[0][j] = 0;
                }
            }        
        }
    };

//54 Validate Binary Search Tree
    /**
     * Definition for a binary tree node.
     * struct TreeNode {
     *     int val;
     *     TreeNode *left;
     *     TreeNode *right;
     *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
     * };
     */
    class Solution {
    public:
        bool isBST(TreeNode* root, TreeNode* min, TreeNode* max) {
            if(root == NULL) {
                return true;
            }
            if((min != NULL && min->val >= root->val) ||(max != NULL) && max->val <= root->val) {
                return false;
            }
            return isBST(root->left, min, root) && isBST(root->right, root, max);
        }
        bool isValidBST(TreeNode* root) {
            return root == NULL ? true : isBST(root, NULL, NULL);
        }
    };

//55 Sum Root to Leaf Numbers
    /**
     * Definition for a binary tree node.
     * struct TreeNode {
     *     int val;
     *     TreeNode *left;
     *     TreeNode *right;
     *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
     * };
     */
    class Solution {
    private:
        int total = 0;
    public:
        void sumRootToLeaf(TreeNode* root, int sum) {
            if(root == NULL) {
                return;
            }
            sum = sum * 10 + root->val;
            if(root->left == NULL && root->right == NULL) {
                total += sum;
            }
            sumRootToLeaf(root->left, sum);
            sumRootToLeaf(root->right, sum);
            
        }
        int sumNumbers(TreeNode* root) {
            sumRootToLeaf(root, 0);
            return total;
        }
    };

//56 Binary Tree Level Order Traversal
    /**
     * Definition for a binary tree node.
     * struct TreeNode {
     *     int val;
     *     TreeNode *left;
     *     TreeNode *right;
     *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
     * };
     */
    class Solution {
    public:
        vector<vector<int>> levelOrder(TreeNode* root) {
            vector<vector<int>> result;
            if(root == NULL) return result;
            queue<TreeNode*> nodes;
            nodes.push(root);
            while(!nodes.empty()) {
                int size = nodes.size();
                vector<int> res;
                for(int i = 0; i< size; i++) {
                    TreeNode* node= nodes.front();
                    res.push_back(node->val);
                    nodes.pop();
                    if(node->left) {
                        nodes.push(node->left);
                    }
                    if(node->right) {
                        nodes.push(node->right);
                    }
                }
                result.push_back(res);
            }
            return result;
        }
    };

//57 Binary Tree Zigzag Level Order Traversal
    /**
     * Definition for a binary tree node.
     * struct TreeNode {
     *     int val;
     *     TreeNode *left;
     *     TreeNode *right;
     *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
     * };
     */
    class Solution {
    public:
        vector<vector<int>> zigzagLevelOrder(TreeNode* root) {
            vector<vector<int>> result;
            if(root == NULL) return result;
            queue<TreeNode*> nodes;
            nodes.push(root);
            bool level = true;
            while(!nodes.empty()) {
                int size = nodes.size();
                vector<int> res;
                for(int i = 0; i< size; i++) {
                    TreeNode* node= nodes.front();
                    res.push_back(node->val);
                    nodes.pop();
                    if(node->left) {
                        nodes.push(node->left);
                    }
                    if(node->right) {
                        nodes.push(node->right);
                    }
                }
                if(!level) {
                    reverse(res.begin(), res.end());
                }
                level = !level;
                result.push_back(res);
            }
            return result;
        }
    };

//58 Binary Tree Vertical Order Traversal 二叉树的竖直遍历
    class Solution {
    public:
        vector<vector<int>> verticalOrder(TreeNode* root) {
            vector<vector<int>> res;
            if (!root) return res;
            map<int, vector<int>> m;
            queue<pair<int, TreeNode*>> q;
            q.push({0, root});
            while (!q.empty()) {
                auto a = q.front(); 
                q.pop();
                m[a.first].push_back(a.second->val);
                if (a.second->left) 
                    q.push({a.first - 1, a.second->left});
                if (a.second->right) 
                    q.push({a.first + 1, a.second->right});
            }
            for (auto a : m) {
                res.push_back(a.second);
            }
            return res;
        }
    };

//59 Maximal SquareGiven a 2D binary matrix filled with 0's and 1's, find the largest 
    //square containing only 1's and return its area.
    class Solution {
    public:
        int maximalSquare(vector<vector<char>>& matrix) {
         if (matrix.empty())
             return 0;
     
         int rows = matrix.size();//行数
         int cols = matrix[0].size(); //列数
         vector<vector<int> > dp(rows+1, vector<int>(cols+1, 0));
         int result = 0; //return result 
         for (int i = 0; i < rows; i ++) {
             for (int j = 0; j < cols; j ++) {
                 if (matrix[i][j] == '1') {
                     int temp = min(dp[i][j], dp[i][j+1]);
                     dp[i+1][j+1] = min(temp, dp[i+1][j]) + 1;
                 }
                 else 
                     dp[i+1][j+1] = 0;
                 
                 result = max(result, dp[i+1][j+1]);
             }
         }
         return result * result; //get the area of square;
        }
    };

//60 Largest Plus Sign
    //(https://blog.csdn.net/qq_26410101/article/details/80902687)
    //https://www.cnblogs.com/grandyang/p/8679286.html
    class Solution {
    public:
        int orderOfLargestPlusSign(int N, vector<vector<int>>& mines) {
            int res = 0;
            vector<vector<int>> dp(N, vector<int>(N, N));
            for (auto mine : mines) dp[mine[0]][mine[1]] = 0;
            for (int i = 0; i < N; ++i) {
                int l = 0, r = 0, u = 0, d = 0;
                for (int j = 0, k = N - 1; j < N; ++j, --k) {
                    dp[i][j] = min(dp[i][j], l = (dp[i][j] ? l + 1 : 0));
                    dp[j][i] = min(dp[j][i], u = (dp[j][i] ? u + 1 : 0));
                    dp[i][k] = min(dp[i][k], r = (dp[i][k] ? r + 1 : 0));
                    dp[k][i] = min(dp[k][i], d = (dp[k][i] ? d + 1 : 0));
                }
            }
            for (int k = 0; k < N * N; ++k) res = max(res, dp[k / N][k % N]);
            return res;
        }
    };

// 61 和17都属于全排列(leetcode 46. Permutations)
    class Solution {
    public:
        vector<vector<int>> permute(vector<int>& nums) {
            vector<vector<int>> res;
            if(nums.size() == 0) {
                return res;
            } else {
                permutation(nums, 0, res);
            }
            return res;
        }
        
        void permutation(vector<int>& nums, int begin, vector<vector<int>>& res) {
            if(nums.size() <= begin) {
                res.push_back(nums);
                return;
            }
            for(int i = begin; i < nums.size(); i++) {
                swap(nums, begin, i);
                permutation(nums, begin + 1, res);
                swap(nums, begin, i);
            }
        }
        void swap(vector<int>& nums, int i, int j) {
            int temp = nums[i];
            nums[i] = nums[j];
            nums[j] = temp;
        }
    };

// 62 (leetcode 47. Permutations II,有重复元素全排列，vector<vector<int>> (res.begin(), res.end()) )
    class Solution {
    public:
        vector<vector<int>> permuteUnique(vector<int>& nums) {
            set<vector<int>> res;
            permutation(nums, 0, res);
            return vector<vector<int>> (res.begin(), res.end());
        }
        
        void permutation(vector<int>& nums, int begin, set<vector<int>>& res) {
            if(nums.size() <= begin) {
                res.insert(nums);
                return;
            }
            for(int i = begin; i < nums.size(); i++) {
                if(i != begin && nums[i] == nums[begin]) {
                    continue;
                }
                swap(nums, begin, i);
                permutation(nums, begin + 1, res);
                swap(nums, begin, i);
            }
        }
        void swap(vector<int>& nums, int i, int j) {
            int temp = nums[i];
            nums[i] = nums[j];
            nums[j] = temp;
        }
    };        

// 63 (leetcode 31. Next Permutation, 数字内组合得到下一个比该数大的数)
    //从后至前的找第一对逆序位置，交换逆序位置的元素，并且将前逆序位置后的数字进行顺序的重排
    class Solution {
    public:
        void nextPermutation(vector<int>& nums) {
            int j,len=nums.size(),i=len-1,tmp;
            while(i>0 &&  nums[i]<=nums[i-1])i--;
            if(i>0){
                j=len-1;
                while(j>=i && nums[j]<=nums[i-1])j--;
                tmp=nums[i-1];
                nums[i-1]=nums[j];
                nums[j]=tmp;
            }
            j=len-1;
            while(i<j){
                tmp=nums[i];
                nums[i]=nums[j];
                nums[j]=tmp;
                i++;j--;
            }
        }
    };

// 64 min函数的栈
    class Solution
    {
        public:
            stack<int> stack1,stack2;    //stack2是一个辅助栈，用于存放当前最小值
            void push(int value)
            {
                stack1.push(value);
                if(stack2.empty())
                    stack2.push(value);
                else if (value<stack2.top())
                    stack2.push(value);
                else {
                    stack2.push(stack2.top());
                }
            }
            void pop()
            {
                stack1.pop();
                stack2.pop();
            }

            int top()
            {
                return stack1.top();
            }
            int min()
            {
                return stack2.top();
            }
    };

// 63 (leetcode 53. Maximum Subarray, 最大子数组，largest sum)
    // class Solution {
    // public:
    //     int maxSubArray(vector<int>& nums) {
    //         if(nums.size()==0)return 0;
    //         return helper(nums,0,nums.size()-1);
    //     }
    //     int helper(vector<int>& nums,int l,int r){
    //         if(l>=r)return nums[l];
    //         int mid=(l+r)/2;
    //         int left=helper(nums,l,mid-1);
    //         int right=helper(nums,mid+1,r);
            
    //         int t=nums[mid];
    //         int max_num=nums[mid];
    //         for(int i=mid-1;i>=l;i--){
    //             t+=nums[i];
    //             max_num=max(max_num,t);
    //         }
    //         t=max_num;
    //         for(int i=mid+1;i<=r;i++){
    //             t+=nums[i];
    //             max_num=max(max_num,t);
    //         }
            
    //         return max(max(left,right),max_num);
    //     }
    // };
    //------------------------------------------
    class Solution {
    public:
        int maxSubArray(vector<int>& nums) {
            int size = nums.size();
            if(size == 0) return 0;
            int resmax = INT_MIN;
            int sum = 0;
            int i=0;
            while(i < size) {
                sum += nums[i++];
                if(resmax < sum){
                    resmax = sum;
                }
                if(sum < 0) {
                    sum = 0;
                }
            }
            return resmax;
        }
    };

// 64 数组中最小的K个数
    // 1 建堆或者优先队列，建立k个元素最大堆nlogk，n个元素的最小堆klogn, https://blog.csdn.net/liqinzhe11/article/details/79277155
        //(建立n个元素的最小堆，每次去除堆顶的最小值，然后弹出堆顶，重新构成最小堆。O(klogn))
        //建立k个元素的最大堆(借助multiset来完成)O(nlogk) 适合海量数据
    // 2 基于Partition的思路
    // 3 红黑树
    // 4 排序
    class Solution {
    public:
        vector<int> GetLeastNumbers_Solution(vector<int> input, int k)
        {
            vector<int> ret;
            if (input.empty() || k > input.size())
                return ret;
            
            for (int i = 0; i < k; i++)
                ret.push_back(input[i]);
            make_heap(ret.begin(), ret.end(), less<int>());
            
            
            for (int i = k; i < input.size(); i++)
            {
                if (input[i] < ret[0])
                {
                    pop_heap(ret.begin(), ret.end(), less<int>());//pop_heap()并没有删除元素，而是将堆顶元素和数组最后一个元素进行了替换
                    ret.pop_back();
                    ret.push_back(input[i]);
                    push_heap(ret.begin(), ret.end(), less<int>());
                }
            }
            return ret;
        }
    };

// 65 (leetcode 160. Intersection of Two Linked Lists, 链表长度相减， 剑指offer193页)

// 66 (leetcode 3. Longest Substring Without Repeating Characters最长无重复字符的子串)
    //如果在滑动窗口就从left一直向右删除字符，直到把相同的字符也删除
    //子串（substring）——在字符串中是连续的
    //子序列（subsequence）——在字符串中可以不连续，也可以连续
    class Solution {
    public:
        int lengthOfLongestSubstring(string s) {
            int res = 0, left = 0, i = 0, n = s.size();
            unordered_set<char> t;
            while (i < n) {
                if (!t.count(s[i])) {
                    t.insert(s[i++]);
                    res = max(res, (int)t.size());
                }  else {
                    t.erase(s[left++]);
                }
            }
            return res;
        }
    };

// 67 (leetcode 226. Invert Binary Tree, 二叉树的镜像)
    class Solution {
    public:
        TreeNode* invertTree(TreeNode* root) {
            if(root == NULL) {
                return root;
            }
            TreeNode* temp = root->left;
            root->left = invertTree(root->right);
            root->right = invertTree(temp);
            return root;
        }
    };

// 68 (每次从这个圆圈中删除第m 个数字,求出在这个圆圈中剩下的最后一个数字)
    int LastNumberOfCircle(int n, int m)
    {
        int last = 0;
        for(int i=2; i<=n; ++i)
        {
            last = (last + m) % i;
        }
        return last;
    }    
    
// 69 (leetcode 657. Robot Return to Origin)
    class Solution {
        public boolean judgeCircle(String moves) {
            int hDirection = 0; //horizontal direction
            int vDirection = 0; //vertical direction
            for (int i = 0; i < moves.length(); i++) {
                switch (moves.charAt(i)) {
                    case 'U':
                        vDirection++;
                        break;
                    case 'D':
                        vDirection--;
                        break;
                    case 'L':
                        hDirection--;
                        break;
                    case 'R':
                        hDirection++;
                    default:
                        break;
                }
            }
            return vDirection == 0 && hDirection == 0 ? true : false;
        }
    }

// 70 (leetcode 141. Linked List Cycle, 是否有环)
    class Solution {
    public:
        bool hasCycle(ListNode *head) {
            if(head == NULL || head->next == NULL) {
                return false;
            } else {
                ListNode *slow = head;
                ListNode *fast = head;
                while(fast != NULL && fast->next != NULL) {
                    slow = slow->next;
                    fast = fast->next->next;
                    if(slow == fast) {
                        return true;
                    }
                }
            }
            return false;
        }
    };

// 71 (leetcode 142. Linked List Cycle II, 求环的入口，如果求环的长度，从追上的节点继续走一圈)
    class Solution {
    public:
        ListNode *detectCycle(ListNode *head) {
            bool cycle = false;
            ListNode *slow = head;
            ListNode *fast = head;
            if(head != NULL && head->next == NULL) {
                while(fast != NULL && fast->next != NULL) {
                    slow = slow->next;
                    fast = fast->next->next;
                    if(slow == fast) {
                        cycle = true;
                        break;
                    }
                }
            }
            if(cycle = false) {
                return NULL;
            } else {
                slow = head;
                while(slow != fast) {
                    slow = slow->next;
                    fast = fast->next;
                }
                return slow;
            }
        }
    };

// 72( Fibonacci（O(log n)求解 ）分析https://blog.csdn.net/jxy859/article/details/6685700)
    //{f(n), f(n-1), f(n-1), f(n-2)} ={1, 1, 1,0}n-1次方
    //     an/2*an/2            n为偶数时
    // an=
    //     a(n-1)/2*a(n-1)/2    n为奇数时
    struct matrix22
        {
            int v11,v12,v21,v22;
            matrix22(int a,int b,int c,int d)
            {
                v11 = a; v12 = b; v21 = c; v22 = d;
            }
            matrix22(){}
        };
        matrix22 matMult(const matrix22 &a, const matrix22 &b)//矩阵乘法
        {
            matrix22 res;
            res.v11 = a.v11*b.v11 + a.v12*b.v21;
            res.v12 = a.v11*b.v12 + a.v12*b.v22;
            res.v21 = a.v21*b.v11 + a.v22*b.v21;
            res.v22 = a.v21*b.v12 + a.v22*b.v22;
            return res;
        }
        matrix22 matPow(const matrix22 &a, int exp)//矩阵求幂
        {
            matrix22 res(1,0,0,1);//初始化结果为单位矩阵
            matrix22 tmp = a;
            for(; exp; exp >> 1)
            {
                if(exp & 1)
                    res = matMult(res, tmp);
                tmp = matMult(tmp, tmp);
            }
            return res;
        }

    class Solution {
    public:
        int climbStairs(int n) {
            matrix22 A(1,1,1,0);
            A = matPow(A, n-1);
            return A.v11 + A.v21;//fn(n-1) + fn(n-2)
        }
    };

//*// 73 (K sum)

// 74 (最长的数字串)
    int main()
    {
        string str;//定义str为某个要输入的字符串
        cin>>str;

        string tmp;//定义一个临时字符串数组用来存放数字串
        string maxstr;//只存放最长的数字串
        int maxlength=0;

        for(int i=0;i<str.length();i++)
        {
            if(str[i]>='0' && str[i]<='9')
            {
                //次数的while循环和if中的判断语句一样，但是千万不可以少
                //while是为了将所有的数字串都加进tmp当中
                while(str[i]>='0' && str[i]<='9')
                {
                    tmp+=str[i++];
                }
                if(tmp.size()>maxlength)
                {
                    maxstr=tmp;
                    maxlength=tmp.size();
                }
            }
            tmp.clear();
        }
        cout<<maxstr;
        return 0;
    }

// 75 (leetcode 191. Number of 1 Bits)
    class Solution {
    public:
        int hammingWeight(uint32_t n) {
            int count = 0;
            while(n != 0) {
                n = n & (n - 1);
                count++;
            }
            return count;
        }
    };

// 76 最长公共子序列（LCS）(https://blog.csdn.net/coolwriter/article/details/79916029)
    //当我们要求table[ i ][ j ]，我们要先判断A[ i ]和B[ j ]是否相同，
    //如果相同他就是table[ i - 1 ][ j - 1 ] + 1，相当于在两个字符串都去掉一个字符时的最长公共字串再加 1；
    //否则最长公共字串取table[ i ][ j - 1 ] 和table[ i - 1 ][ j ] 中大者。
    //table[ i ][ j ]为字符串A的第一个字符到第 i 个字符串和字符串B的第一个字符串到第 j 个字符串的最长公共子序列
    class LCS {
    public:
        int findLCS(string A, int n, string B, int m) {
            int table[n + 1][m + 1];
            for(int i = 0;i <= n;++i)
                table[i][0] = 0;
            for(int i = 0;i <= m;++i)
                table[0][i] = 0;

            for(int i = 1;i <= n;++i){
                for(int j = 1;j <= m;++j){
                    if(A[i-1] == B[j-1])
                        table[i][j] = table[i-1][j-1] + 1;
                    else {
                        table[i][j] = max(table[i][j - 1],table[i - 1][j]);
                    }
                }
            }
            return table[n][m];
        }
    };
    
// 77 最长公共子串(dp)子串是要求更严格的一种子序列，要求在母串中连续地出现。
    class LongestSubstring {
    public:
        int findLongest(string A, int n, string B, int m) {
            int dp[1000][1000] = {0};
            int max_len = 0;
            for(int i = 0;i < n;i++) {
                for(int j = 0;j < m;j++) {
                    if(A[i] == B[j]) {
                        if(i >0 && j>0) {
                            dp[i][j] = dp[i-1][j-1] +1;
                        }
                        else {
                            dp[i][j] = 1;   // i= j= 0  的时候，不能使用上面那个公式，所以要单独写出来
                        }
                    }
                    if(max_len < dp[i][j]) {
                        max_len = dp[i][j];
                    }
                }
            }
            return max_len ;
        }
    };

// 78 对称字符串长度 (dp法/Manacher法) https://www.cnblogs.com/mini-coconut/p/9074315.html
//对于字符串str，假设dp[i,j]=1表示str[i...j]是回文子串，那个必定存在dp[i+1,j-1]=1。
//当str[i]=str[j]时，如果str[i+1...j-1]是回文串，则str[i...j]也是回文串；如果str[i+1...j-1]不是回文串，则str[i...j]不是回文串
    string longestPalindrome(string s)
    {
        if (s.empty()) return "";
        int len = s.size();
        if (len == 1)return s;
        int longest = 1;
        int start=0;
        vector<vector<int> > dp(len,vector<int>(len));
        for (int i = 0; i < len; i++) {
            dp[i][i] = 1;
            if(i<len-1) {
                if (s[i] == s[i + 1]) {
                    dp[i][i + 1] = 1;
                    start=i;
                    longest=2;
                }
            }
        }
        for (int l = 3; l <= len; l++) {//子串长度
            for (int i = 0; i+l-1 < len; i++) {//枚举子串的起始点
                int j=l+i-1;//终点
                if (s[i] == s[j] && dp[i+1][j-1]==1){
                    dp[i][j] = 1;
                    start=i;
                    longest = l;
                }
            }
        }
        return s.substr(start,longest);
    }

// 79  二叉树中任意两节点的最低共同父节点（看剑指offer252页）
    * struct TreeNode {
    *     int val;
    *     TreeNode *left;
    *     TreeNode *right;
    *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
    * };
    class Solution {
    public:
        bool GetNodePath(TreeNode* pRoot, TreeNode* pNode, list<TreeNode*> & path) {  
            if(pRoot == pNode) {     
                path.push_back(pRoot);  
                return true;  
            }  
            // if(pRoot == NULL)  
            //     return false;  
            path.push_back(pRoot);  
            bool found = false;  
            found = GetNodePath(pRoot->left, pNode, path);  
            if(!found)  
                found = GetNodePath(pRoot->right, pNode, path);  
            if(!found)  
                path.pop_back();  
            return found;  
        }  

        TreeNode* GetLastCommonNode(const list<TreeNode*>& path1, const list<TreeNode*>& path2) 
        {
            TreeNode* pLast = NULL;  
            list<TreeNode*>::const_iterator iter1 = path1.begin();  
            list<TreeNode*>::const_iterator iter2 = path2.begin();  
            while(iter1 != path1.end() && iter2 != path2.end())  
            {  
                if(*iter1 == *iter2)  
                    pLast = *iter1;  
                else  
                    break;  
                iter1++;  
                iter2++;  
            }  
            return pLast; 
        }

        TreeNode* GetLastCommonParent(TreeNode* pRoot, TreeNode* pNode1, TreeNode* pNode2) {  
            if(pRoot == NULL || pNode1 == NULL || pNode2 == NULL)  
                return NULL;  
            list<TreeNode*> path1;  
            bool bResult1 = GetNodePath(pRoot, pNode1, path1);  
            list<TreeNode*> path2;  
            bool bResult2 = GetNodePath(pRoot, pNode2, path2);  
            if(!bResult1 || !bResult2)   
                return NULL;
            return GetLastCommonNode(path1, path2);   
        }
    };

// 80 (722. Remove Comments 消除注释，注意break，str清空)
    class Solution {
    public:
        vector<string> removeComments(vector<string>& source) {
            vector<string> res;
            bool inBlock = false;
            string str;
            for (auto& cur : source) {
                for (int i = 0; i < cur.size();) {
                    if (!inBlock) {
                        if (i + 1 == cur.size()) {
                            str += cur[i++];
                        }
                        else {
                            string tmp = cur.substr(i, 2);
                            if (tmp == "/*") {
                                inBlock = true;
                                i = i + 2;
                            }
                            else if (tmp == "//") {
                                break;
                            }
                            else {
                                str += cur[i++];
                            }
                        }
                    }
                    else {
                        if (i + 1 == cur.size()) {
                            i++;
                        }
                        else {
                            string tmp = cur.substr(i, 2);
                            if (tmp == "*/") {
                                inBlock = false;
                                i = i + 2;
                            }
                            else {
                                i++;
                            }
                        }
                    }
                }
                if (str.size() && !inBlock) {
                    res.push_back(str);
                    str = "";
                }
            }
            return res;
        }
    };

// 81 KMP
    void getNext(const string& needle,int next[])
    {
        const int n = needle.size();
        int i = 0;
        int j = -1;
        next[i] = j;
        while (i < n)
        {
            if (j == -1 || needle[i] == needle[j])
            {
                ++i;
                ++j;
                next[i] = j;
            }
            else
            {
                j = next[j];
            }
        }
    }
    int kmp(const string& hay, const string& needle)
    {
        
        const int n = hay.size();
        const int m = needle.size();
        int* next = new int[m];
        getNext(needle, next);
        int i = 0;
        int j = 0;
        while (i < n&&j < m)
        {
            if (hay[i] == needle[j])
            {
                ++i;
                ++j;
            }
            else
            {
                j = next[j];
            }
        }
        //delete next; 
        if (j == m)
            return i - j;
        else
            return -1;
    }

// 82 (给定一棵树,找到一个点，使得与其他节点距离总和最小值)构造最小生成树的算法主要有：克鲁斯卡尔（Kruskal）算法和普利姆（Prim）算法
/*边的定义*/
    struct edge
    {
           int u, v;                                //边的两个端点编号
           int cost;                                //边权
           edge(int x,int y, int c):u(x),v(y),cost(c){}
    };
     
    /*边的比较函数*/
    bool cmp(edge a, edge b)
    {
           return a.cost < b.cost;
    }
     
    /*并查集查询函数，返回x所在集合的根结点,并查集可以通过查询两个结点所在集合的根结点是否相同来判断它们是否在同一个集合中*/
    int findFather(vector<int> father, int x)
    {
       int a = x;
       while (x != father[x])
              x = father[x];
       //路径压缩
       while (a != father[a]) {
              int z = a;
              a = father[a];
              father[z] = x;
       }
       return x;
    }
     
    /*Kruskal算法求无向图的最小生成树*/
    int Kruskal(int n, int m, vector<edge>& E)
    {
       /*
       param
       n:                         图的顶点个数
       m:                         图中边的个数
       E:                         边的集合
       */
       vector<int> father(n);                                 //并查集数组
       int ans = 0;                                           //所求边权之和
       int NumEdge = 0;                                       //记录最小生成树边数
       for (int i = 0; i < n; i++)                            //初始化并查集
            father[i] = i;
       sort(E.begin(), E.end(), cmp);                         //所有边按边权从小到大排序
       for (int i = 0; i < m; ++i)                            //枚举所有边
       {
          int faU = findFather(father, E[i].u);           //查询端点u所在集合的根结点
          int faV = findFather(father, E[i].v);           //查询端点v所在集合的根结点
          if (faU != faV) {                               //如果不在一个集合中
             father[faU] = faV;                       //合并集合（相当于把测试边加入到最小生成树）
             ans += E[i].cost;
             NumEdge++;                               //当前生成树边数加1
             if (NumEdge == n - 1)                    //边数等于顶点数减1，算法结束
                   break;
          }
       }
       if (NumEdge != n - 1)                                  //无法连通时返回-1
            return -1;    
       else
            return ans;                                     //返回最小生成树边权之和
    }
     
    int main()
    {
           vector<edge> E = { edge(0,1,4),edge(1,2,1),edge(2,3,6),edge(3,4,5),edge(0,4,1),
                              edge(0,5,2),edge(1,5,3),edge(2,5,5),edge(3,5,4),edge(4,5,3) };
           int n = 6;
           int m = 10;
           int res = Kruskal(n, m, E);
           cout << res << endl;
    }

// 83 判断麻将胡没胡(正则的状态机实现方式）

//*// 84 把一堆数字分成两堆，使两堆的和尽可能接近
    //设总和为sum，最大部分和为partsum，则两堆数最小的差值为 ( sum - partsum ) - partsum
    #include <iostream>
    #include <vector>
    #include <numeric>
    using namespace std;
    struct Node
    {
        bool reach;
        int parent; 
        int value;
        
        Node(): reach( false ), parent( 0 ), value( 0 )
        {
        }
    };
    int main()
    {
        vector<int> seq;
        for( int i; (cin >> i) && i; ) seq.push_back( i );
        
        int allsum = accumulate( seq.begin(), seq.end(), 0 );
        int partsum = allsum / 2;
        
        vector<Node> f( partsum + 1 );
        f[0].reach = true;
        int rmpos = 0;
        
        for( vector<int>::iterator it = seq.begin(); it != seq.end(); it++ )
        {
            bool rm = true;
            for( int i = min(rmpos, partsum - *it); i >= 0; i-- )
            {
                if( !f[i].reach ) continue;
                Node& next = f[i + *it];
                next.reach = true;
                if( !next.parent )
                {
                    next.parent = i;
                    next.value = *it;
                }
                if( rm )
                {
                    rmpos = max( rmpos, i + *it );
                    rm = false;
                }
            }
        }
        
        while( !f[partsum].reach ) partsum--;
        cout << "The minimum difference of sums of two parts is " << allsum - partsum * 2 << endl;
        cout << "The smaller part is: ";
        for( int p = partsum; p != 0; p = f[p].parent ) cout << f[p].value << " ";
        cout << endl;
        return 0;
    }

//https://www.jianshu.com/p/a9abf3425291

// 85 (leetcode 121. Best Time to Buy and Sell Stock, 只买卖一次)
    class Solution {
    public:
        int maxProfit(vector<int>& prices) {
            if(prices.size() < 1)
                return 0;
            int ret = 0;
            int min_cur = prices[0];
            for(int i = 1; i< prices.size(); i++)
            {
                ret = max(ret, prices[i] - min_cur);
                min_cur = min(min_cur, prices[i]);
            }
            return ret;
        }
    };

// 86 (122. Best Time to Buy and Sell Stock II, 可以多次买卖)
    class Solution {
    public:
        int maxProfit(vector<int>& prices) {
            if(prices.size() < 1)
                return 0;
            int ret = 0;
            for(int i = 0; i< prices.size() - 1; i++)
            {
                if(prices[i+1] - prices[i] > 0)
                    ret += prices[i+1] - prices[i];
            }
            return ret;
        }
    };

// 87 (123. Best Time to Buy and Sell Stock III 只允许买卖2次 http://www.cnblogs.com/grandyang/p/4281975.html)
    //定义local[i][j]为在到达第i天时最多可进行j次交易并且最后一次交易在最后一天卖出的最大利润，此为局部最优。
    //然后我们定义global[i][j]为在到达第i天时最多可进行j次交易的最大利润，此为全局最优。它们的递推式为：
    //local[i][j] = max(global[i - 1][j - 1] + max(diff, 0), local[i - 1][j] + diff)
    //global[i][j] = max(local[i][j], global[i - 1][j])
    class Solution {
    public:
        int maxProfit(vector<int> &prices) {
            if (prices.empty()) return 0;
            int n = prices.size(), g[n][3] = {0}, l[n][3] = {0};
            for (int i = 1; i < prices.size(); ++i) {
                int diff = prices[i] - prices[i - 1];
                for (int j = 1; j <= 2; ++j) {
                    l[i][j] = max(g[i - 1][j - 1] + max(diff, 0), l[i - 1][j] + diff);
                    g[i][j] = max(l[i][j], g[i - 1][j]);
                }
            }
            return g[n - 1][2];
        }
    };

// 88 (309. Best Time to Buy and Sell Stock with Cooldown https://www.cnblogs.com/grandyang/p/4997417.html)
    class Solution {
    public:
        int maxProfit(vector<int>& prices) {
            int buy = INT_MIN, pre_buy = 0, sell = 0, pre_sell = 0;
            for (int price : prices) {
                pre_buy = buy;
                buy = max(pre_sell - price, pre_buy);
                pre_sell = sell;
                sell = max(pre_buy + price, pre_sell);
            }
            return sell;
        }
    };

// 89（LeetCode 4. Median of Two Sorted Arrays https://blog.csdn.net/u014591781/article/details/52893664）
    //https://leetcode.com/problems/median-of-two-sorted-arrays/discuss/2471/very-concise-ologminmn-iterative-solution-with-detailed-explanation
    class Solution {
    public:
        double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
            int m = nums1.size(), n = nums2.size();
            if (m < n) return findMedianSortedArrays(nums2, nums1);
            if (n == 0) return ((double)nums1[(m - 1) / 2] + (double)nums1[m / 2]) / 2.0;
            int left = 0, right = n * 2;
            while (left <= right) {
                int mid2 = (left + right) / 2;
                int mid1 = m + n - mid2;
                double L1 = mid1 == 0 ? INT_MIN : nums1[(mid1 - 1) / 2];
                double L2 = mid2 == 0 ? INT_MIN : nums2[(mid2 - 1) / 2];
                double R1 = mid1 == m * 2 ? INT_MAX : nums1[mid1 / 2];
                double R2 = mid2 == n * 2 ? INT_MAX : nums2[mid2 / 2];
                if (L1 > R2) left = mid2 + 1;//A1's lower half is too big; need to move C1 left (C2 right)
                else if (L2 > R1) right = mid2 - 1;//// A2's lower half too big; need to move C2 left.
                else return (max(L1, L2) + min(R1, R2)) / 2;
            }
            return -1;
        }
    };

// 90 (平衡二叉树， 剑指offer 210页)
    bool IsBalanced_1(TreeNode* pRoot,int& depth){
        if(pRoot==NULL){
            depth=0;
            return true;
        }
        int left,right;
        int diff;
        if(IsBalanced_1(pRoot->left,left) && IsBalanced_1(pRoot->right,right)){
            diff=left-right;
            if(diff<=1 && diff>=-1){
                depth=left>right?left+1:right+1;
                return true;
            }
        }
        return false;
    }
     
    bool IsBalancedTree(TreeNode* pRoot){
        int depth=0;
        return IsBalanced_1(pRoot,depth);
    }

// 92 构建乘积数组 https://www.acwing.com/solution/AcWing/content/759/（vector初始化）
    class Solution {
    public:
        vector<int> multiply(const vector<int>& A) {
            vector<int>left(A.size(),1);
            vector<int>right(A.size(),1);
            for(int i = 1;i<A.size();i++){
                left[i] = A[i-1]*left[i-1];
            }
            for(int i = A.size()-2;i>=0;i--){
                right[i] = A[i+1]*right[i+1];
            }
            vector<int>B(A.size(),0);
            for(int i = 0;i<A.size();i++){
                B[i] = left[i]*right[i];
            }
            return B;
        }
    };

// 93 剑指offer(47)：礼物的最大值 https://blog.csdn.net/dugudaibo/article/details/79678890
    int getMaxValue(const int *val,int rows,int cols) 
    {                     
        if((val == NULL)||(rows <= 0)||(cols <= 0))return 0;

        //只用一行缓存来缓存上一行的最大值
        int *buffer = new int[cols];
        for(int i = 0;i != rows;i++){ 
            for(int j = 0;j != cols;j++){ 
                int up = 0,left = 0;
                //取上面一格的最大值
                if(i != 0){ 
                    up = buffer[j]; 
                }         
                //因为左边一格的最大值会及时更新到缓存，所以这里取的是左边一格的数据                                                                                                                                                            
                if(j != 0){ 
                    left = buffer[j-1]; 
                }         
                int tmp = max(up,left)+val[i*cols+j]; 
                //对当前行的数据实时更新到缓存
                buffer[j] = tmp;
            }             
        }                 
        int resu = buffer[cols-1];
        delete[] buffer;  
        return resu;      
    } 

// 94 面试题46：把数字翻译成字符串 https://blog.csdn.net/xy_cpp/article/details/79000901
    int GetTranslationCount(int num)
    {
        if(num < 0)
            return -1;
        string strNum = to_string(num);
        int result = GetTranslationCount(strNum);
        return result;
    }

    int GetTranslationCount(const string& num)
    {
        int length = num.size();
        int* counts = new int[length];
        int count = 0;
        for(int i = length - 1; i >= 0; --i)
        {
            if(i < length - 1)
                count = counts[i+1];
            else
                count = 1;

            if(i < length - 1)
            {
                int digit1 = num[i] - '0';
                int digit2 = num[i+1] - '0';
                int twoNum = digit1 * 10 + digit2;
                if(twoNum <= 25 && twoNum >= 10)
                {
                    if(i < length - 2)
                        count += counts[i+2];
                    else
                        count += 1;
                }
            }

            counts[i] = count;
        }
        count = counts[0];
        delete[] counts;
        return count;
    }
//*//95 求数组中的逆序对的数量 https://www.cnblogs.com/coffy/p/5896541.html
    class Solution {
    public:
        int InversePairs(vector<int> data) {
            if(data.size() == 0){
                return 0;
            }
            // 排序的辅助数组
            vector<int> copy;
            for(int i = 0; i < data.size(); ++i){
                copy.push_back(data[i]);
            }
            return InversePairsCore(data, copy, 0, data.size() - 1) % 1000000007;
        }
        long InversePairsCore(vector<int> &data, vector<int> &copy, int begin, int end){
            // 如果指向相同位置，则没有逆序对。
            if(begin == end){
                copy[begin] = data[end];
                int i = mid; // i初始化为前半段最后一个数字的下标
                int j = end; // j初始化为后半段最后一个数字的下标
                int indexcopy = end; // 辅助数组
                    return 0;
            }
            // 求中点
            int mid = (end + begin) >> 1;
            // 使data左半段有序，并返回左半段逆序对的数目
            long leftCount = InversePairsCore(copy, data, begin, mid);
            // 使data右半段有序，并返回右半段逆序对的数目
            long rightCount = InversePairsCore(copy, data,mid + 1, end);//复制的数组的最后一个数字的下标
            long count = 0; // 计数，逆序对的个数，注意类型

            while(i >= begin && j >= mid + 1){
                if(data[i] > data[j]){
                    copy[indexcopy--] = data[i--];
                    count += j - mid;
                }
                else{
                    copy[indexcopy--] = data[j--];
                }
            }
            for(;i >= begin; --i){
                copy[indexcopy--] = data[i];
            }
            for(;j >= mid + 1; --j){
                copy[indexcopy--] = data[j];
            }
            return leftCount + rightCount + count;
        }
    };    

//96 顺时针打印矩阵，C++实现
    class Solution {
    public:
        vector<int> printMatrix(vector<vector<int> > matrix) {
            // 存储结果
            vector<int> result;
            // 边界条件
            if(matrix.empty())
                return result;
            // 二维矩阵行列
            int rows = matrix.size();
            int cols = matrix[0].size();
            // 圈的四个角标
            int left = 0;
            int right = cols-1;
            int top = 0;
            int btm = rows-1;
            // 循环打印圈
            while(left <= right && top <= btm){             // 循环条件：
                // 圈的第一步
                for(int i=left;i<=right;++i)                // 第一步循环条件
                    result.push_back(matrix[top][i]);       // 第一步坐标
                // 圈的第二步
                if(top<btm)                                 // 第二步边界条件
                    for(int i=top+1;i<=btm;++i)             // 第二步循环条件
                        result.push_back(matrix[i][right]); // 第二步坐标
                // 圈的第三步
                if(top<btm && left<right)                   // 第三步边界条件
                    for(int i=right-1;i>=left;--i)          // 第三步循环条件
                        result.push_back(matrix[btm][i]);   // 第三步坐标
                // 圈的第四步
                if(top+1<btm && left<right)                 // 第四步边界条件
                    for(int i=btm-1;i>=top+1;--i)           // 第四步循环条件
                        result.push_back(matrix[i][left]);  // 第四步坐标

                ++left;--right;++top;--btm;
            }
            return result;
        }
    };

// 97 剪绳子
    class Solution{
    public:
        int integerBreak(int n) {
            if( n<= 3)
                return 1 * (n - 1);
            int res = 1;
            if(n % 3 == 1) {
                res = 4;
                n -= 4;
            } else if(n % 3 == 2) {
                res = 2;
                n -= 2;
            }
            while(n){
                res *= 3;
                n -= 3;
            }
            return res;    
        }
    }

// 98 面试题33：把数组排成最小的数
    class Solution {
    public:
        static bool compare(int a,int b){
            string strNum1=to_string(a);
            string strNum2=to_string(b);
            return (strNum1+strNum2)<(strNum2+strNum1);
        }
         
        string PrintMinNumber(vector<int> numbers) {
            string result;
            if(numbers.empty())
                return result;
            sort(numbers.begin(),numbers.end(),compare);
            for(unsigned int i=0;i<numbers.size();i++)
                result+=to_string(numbers[i]);
            return result;
        }
    };

// 99 数字序列中某一位的数字

    寻找规律

    以n=1001为例，首先是一位数，有10个（0~9），1001>10，所以肯定不在这10个里面，从后面的数字开始找第1001-10=991位的数字。
    接着是两位数，有90个（10~99），991>2*90，所以肯定不在这90个数里面，从后面的数字开始找第991-2*90=811位的数字。
    再接下来是三位数，有900个（100~999），811<3*900，所以肯定在这90个数里面。现在就是找从100开始的第811位的数字，
    每个数字都是三位，811=3*270+1，因此第811位就是从100开始的第270个数字（370）的中间一位。

    规律是先找到第n位的是属于几位数，然后在digit位数中找到对应的那一位数字。
    int numberAtIndex(int index)
    {
        if(index<0)
            return -1;
        int digit=1;
        while(true)
        {
            int count=countOfDigit(digit);
            if(index<count*digit)
            {
                return numAtIndexDigit(index,digit);
            }
            else
            {
                index=index-count*digit;
                digit++;
            }
        }
    }
            
     
    //位数为digit的数字一共多少个，1位数有10个（0~9），2位数有90个（10~99），3位数有900个（100~999）
    int countOfDigit(int digit)
    {
        if(digit==1)
            return 10;
        int result=9;
        while(digit>1)
        {
            result*=10;
            digit--;
        }
        return result;
    }
     
    //在位数为digit的数字中，第index个数字是多少
    int numAtIndexDigit(int index,int digit)
    {
        int begin=beginOfDigit(digit);
        int temp=begin+index/digit;
        int yu=index%digit;
        for(int i=1;i<digit-yu;i++)
        {
            temp=temp/10;
        }
        return temp%10;
    }
     
    //digit位数的第一个数字，如1位数的是0，2位数的是10，3位数的是100
    int beginOfDigit(int digit)
    {
        if(digit==1)
            return 0;
        int result=1;
        while(digit>1)
        {
            result*=10;
            digit--;
        }
        return result;
    }


字典树，又称为单词查找树，Tire数，是一种树形结构，它是一种哈希树的变种

基本性质
    根节点不包含字符，除根节点外的每一个子节点都包含一个字符
    从根节点到某一节点。路径上经过的字符连接起来，就是该节点对应的字符串
    每个节点的所有子节点包含的字符都不相同
    class TrieNode // 字典树节点
        {
            private int num;// 有多少单词通过这个节点,即由根至该节点组成的字符串模式出现的次数
            private TrieNode[] son;// 所有的儿子节点
            private boolean isEnd;// 是不是最后一个节点
            private char val;// 节点的值

            TrieNode()
            {
                num = 1;
                son = new TrieNode[SIZE];
                isEnd = false;
            }
        }   

      1、HashMap通过hashcode对其内容进行快速查找，而TreeMap中所有的元素都保持着某种固定的顺序，如果你需要得到一个有序的结果你就应该使用TreeMap（HashMap中元素的排列顺序是不固定的）。HashMap中元素的排列顺序是不固定的）。
      2、  HashMap通过hashcode对其内容进行快速查找，而TreeMap中所有的元素都保持着某种固定的顺序，如果你需要得到一个有序的结果你就应该使用TreeMap（HashMap中元素的排列顺序是不固定的）。集合框架”提供两种常规的Map实现：HashMap和TreeMap (TreeMap实现SortedMap接口)。
      3、在Map 中插入、删除和定位元素，HashMap 是最好的选择。但如果您要按自然顺序或自定义顺序遍历键，那么TreeMap会更好。使用HashMap要求添加的键类明确定义了hashCode()和 equals()的实现。 这个TreeMap没有调优选项，因为该树总处于平衡状态。



//二叉堆树位置 k 的结点的父结点的位置是 k/2，而他的子结点的位置是 2k 和 2k+1。

//题2：topK，求最大的前K个数
//思路：利用K长度的最大堆， 或者利用快排的partition= K，前面的数即为所求

//题3：取全排列的第K大的数
//思路：https://segmentfault.com/a/1190000007367589

//题4： LeetCode 109 Convert Sorted List to Binary Search Tree（链表，数组，二叉树互相转换）

//C++ 在继承中虚函数、纯虚函数、普通函数，三者的区别
https://www.cnblogs.com/xudong-bupt/p/3570304.html

虚函数(impure virtual)
　　C++的虚函数主要作用是“运行时多态”，父类中提供虚函数的实现，为子类提供默认的函数实现。
　　子类可以重写父类的虚函数实现子类的特殊化。
纯虚函数(pure virtual)
　　C++中包含纯虚函数的类，被称为是“抽象类”。抽象类不能使用new出对象，只有实现了这个纯虚函数的子类才能new出对象。
　　C++中的纯虚函数更像是“只提供申明，没有实现”，是对子类的约束，是“接口继承”。
　　C++中的纯虚函数也是一种“运行时多态”。
　　如下面的类包含纯虚函数，就是“抽象类”： 

内存管理：
    栈区
        先进后出，自动分配，函数参数（从右向走），局部变量
        和函数及变量生命周期一样，同时释放

    堆区
        堆区需要程序员申请，指定大小，手动释放
        malloc, realloc, calloc, free
        32位默认4G,实际2G以内，64位128G以内

    堆栈区别
        堆 手动分配释放  空间大  不连续的内存区域    申请效率慢，容易产生碎片
        栈 自动分配释放  空间小  一块连续的内存区域   申请效率高

    全局静态区
        全局变量和静态变量存放在一起，初始化的全局变量和静态变量在一块区域
        全局变量和静态变量在整个程序的运行生存期都存在，程序结束时才释放

    常量区
        常量：初始化就开辟一段空间保存常量，相同的常量用同一个地址， char p[] = “A”不是同一个地址

    malloc，realloc，calloc
    realloc如果当前内存块足够的话，直接扩大原来的内存空间，并返回原来的指针地址，如果当前内存块不够的话，
           重新分配一块内存，并拷贝原来的内容到新内存空间，释放原来的内存空间，返回新指针地址。
    strcpy 必须拷贝以\0结束的字符串
    memcpy(des, src, count） 直接拷贝内存空间，指定拷贝大小,count为字节数，重叠拷贝避免使用
    memmove(des, src, count) 解决memcpy重复拷贝
    memset  初始化
    动态数组（realloc）
    代码段：存放程序执行代码
    数据段：存放初始化的全局变量
    BSS段：存放未初始化的全局变量


c++/c &引用
　　（1）传递引用给函数与传递指针的效果是一样的。这时，被调函数的形参就成为原来主调函数中的实参变量或对象的一个别名来使用，所以在被调函数中对形参变量的操作就是对其相应的目标对象（在主调函数中）的操作。

　　（2）使用引用传递函数的参数，在内存中并没有产生实参的副本，它是直接对实参操作；而使用一般变量传递函数的参数，当发生函数调用时，需要给 形参分配存储单元，形参变量是实参变量的副本；如果传递的是对象，还将调用拷贝构造函数。因此，当参数传递的数据较大时，用引用比用一般变量传递参数的效 率和所占空间都好。

　　（3）使用指针作为函数的参数虽然也能达到与使用引用的效果，但是，在被调函数中同样要给形参分配存储单元，且需要重复使用"*指针变量名"的 形式进行运算，这很容易产生错误且程序的阅读性较差；另一方面，在主调函数的调用点处，必须用变量的地址作为实参。而引用更容易使用，更清晰。

　　如果既要利用引用提高程序的效率，又要保护传递给函数的数据不在函数中被改变，就应使用常引用。
    1 
    void swap(int &p1, int &p2) //此处函数的形参p1, p2都是引用
    { int p; p=p1; p1=p2; p2=p; }

    　　为在程序中调用该函数，则相应的主调函数的调用点处，直接以变量作为实参进行调用即可，而不需要实参变量有任何的特殊要求。如：对应上面定义的swap函数，相应的主调函数可写为：

    main( )
    {
    　int a,b;
    　cin>>a>>b; //输入a,b两变量的值
    　swap(a,b); //直接以变量a和b作为实参调用swap函数
    　cout<<a<< ' ' <<b; //输出结果
    }
    2
     bool func1(int* depth){
        *depth = 1;
     }
     int a;
     func1(&a);
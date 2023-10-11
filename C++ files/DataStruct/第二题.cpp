#include<stdio.h>
#include<malloc.h>
#define LIST_INIT_SIZE 100
#define LISTINCREMENT 10
/*创建链表数据结构*/
typedef struct  Node{
	int data;//存储数据类型
	Node *next;//指向下一元素
} Node,*pNode;

pNode Creat_list();//创建一个单链表
void Traverse_list(pNode head);//遍历链表
pNode Insert_list(pNode end,int a);//在链表的第n个节点前插入一个新的节点，且新节点的值为n
pNode Reverse_list(pNode head);//反转单链表
void DestroyList(pNode head);  //销毁链表

int main(){
	int T;
	scanf("%d",&T);
	while(T--){
		pNode head,end;
		head=Creat_list();
		end=head;
		int n;
		scanf("%d",&n);
		while(n--){
			int a;
			scanf("%d",&a);	
			end=Insert_list(end,a);//尾部插入数据a
		}
		end->next=NULL;
		head=Reverse_list(head);
		Traverse_list(head);
		DestroyList(head);
	}
	return 0;
}
//创建一个单链表（不包括头结点）
pNode Creat_list()
{
	pNode head=(pNode)malloc(sizeof(Node));//创建链表的头结点
	head->next=NULL;
	return head;
}
//遍历链表
void Traverse_list(pNode head)
{
	pNode midnode=(pNode)malloc(sizeof(Node));
	midnode=head;
	while(midnode->next != NULL){
		midnode=midnode->next;
		printf("%d ",midnode->data);
	}
	return;
}


//在链表尾部插入一个值为a的新节点
pNode Insert_list(pNode end,int a)
{
	
	pNode ins=(pNode)malloc(sizeof(Node));//分配一个空间
	if(ins==NULL)
	{
		printf("分配失败\n");
	}
	ins->data=a;
	end->next = ins;
	ins->next = NULL;
	return ins;
}

//反转单链表
pNode Reverse_list(pNode head)
{
	
	pNode midnode1=(pNode)malloc(sizeof(Node));
	pNode midnode2=(pNode)malloc(sizeof(Node));
	if(midnode1==NULL || midnode2==NULL)
	{
		printf("分配失败\n");
	}
	midnode1=head->next;
	midnode2=midnode1->next;
	midnode1->next=NULL;
	
	
	while(midnode2->next != NULL){
		pNode X=midnode2->next;
		midnode2->next=midnode1;
		midnode1=midnode2;
		midnode2=X;
	}
	midnode2->next=midnode1;
	head->next=midnode2;
	return head;
}
/*销毁链表*/
void DestroyList(pNode head)  
{  
	pNode p;  
	if(head==NULL)  
		return;  
	while(head)  
	{  
		p=head->next;  
		free(head);  
		head=p;  
	}  
	return; 
}  


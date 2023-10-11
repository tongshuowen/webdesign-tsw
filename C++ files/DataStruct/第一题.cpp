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
pNode Insert_list(pNode head,int a);//链表尾部插入一个新的节点，且新节点的值为a
pNode Reverse_list(pNode head);//反转单链表
void DestroyList(pNode head);  //销毁链表

/*主函数*/
int main(){
	pNode head;
	head=Creat_list();
	int a;
	scanf("%d",&a);	
	while(a != -1){
		head=Insert_list(head,a);//尾部插入数据a
		scanf("%d",&a);	
	}//-1结束
	Traverse_list(head);
	DestroyList(head);
	return 0;
}
/*创建一个单链表*/
pNode Creat_list()
{
	pNode head=(pNode)malloc(sizeof(Node));//创建链表的头结点
	head->next=NULL;
	return head;
}
/*遍历链表*/
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
/*在链表头部插入一个值为a的新节点*/
pNode Insert_list(pNode head,int a)
{

	pNode ins=(pNode)malloc(sizeof(Node));//分配一个空间
	if(ins==NULL)
	{
		printf("分配失败\n");
	}
	ins->data=a;
	ins->next=head->next;
	head->next=ins;
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


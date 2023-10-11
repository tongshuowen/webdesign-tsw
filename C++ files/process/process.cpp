#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>//_getch()头文件，键入一个字符，不需要回车自动运行
#include<fstream>//写入文件
#include<windows.h>//Sleep(1000),1s延迟
using namespace std;

/*
*全局变量
*/
int   RunPoint;   // 运行进程指针，-1时为没有运行进程
int   WaitPoint;  // 阻塞队列指针，-1时为没有阻塞进程
int   ReadyPoint; // 就绪队列指针，-1时为没有就绪进程
long  ClockNumber;   // 系统时钟
int   ProcNumber;    // 系统中模拟产生的进程总数
int   FinishedProc;    // 系统中目前已执行完毕的进程总数
/*
*进程信息结构
*/
struct ProcStruct  
{
  int  p_pid;         // 进程的标识号
  char p_state;       // 进程的状态，C--运行  R--就绪  W--组塞  B--后备  F--完成
  int  p_rserial[10]; // 模拟的进程执行的CPU和I/O时间数据序列，间隔存储，第0项存储随后序列的长度（项数），以便知晓啥时该进程执行结束
  int  p_pos;         // 当前进程运行到的位置，用来记忆执行到序列中的哪项
  int  p_starttime;   // 进程建立时间
  int  p_endtime;     // 进程运行结束时间
  int  p_cputime;     // 当前运行时间段进程剩余的需要运行时间
  int  p_iotime;      // 当前I/O时间段进程剩余的I/O时间
  int  p_next;        // 进程控制块的链接指针
} proc[10];

/*
*功能函数
*/
void Create_ProcInfo(void);//随机创建进程√
void DisData();//在屏幕打印所创建的序列√
void Scheduler_FF(void);//进程模拟调度算法
/*
*调度功能
*/
void NewReadyProc(void);//判断是否有新进程到达，有则放入就绪队列
void Cpu_Sched(void);//新入进程只执行一个时间片
void NextRunProcess(void);//选择下一个可以运行的进程
void IO_Sched(void);//IO调度，排在队首的得到一个时间片
void Display_ProcInfo( );//显示系统当前状态
void Read_Process_Info(  );//生成进程文件
void DisResult(void);//显示模拟结果

int main( )
{
  char ch;  
  RunPoint=-1;   // 运行进程指针，-1时为没有运行进程
  WaitPoint=-1;  // 阻塞队列指针，-1时为没有阻塞进程
  ReadyPoint=-1; // 就绪队列指针，-1时为没有就绪进程
  ClockNumber=0;   // 系统时钟
  ProcNumber=0;    // 当前系统中的进程总数
  FinishedProc=0;// 系统中目前已执行完毕的进程总数为0

  system("cls") ;
  while ( true )                           
  {
    printf("***********************************\n");
    printf("     1: 建立进程调度数据序列 \n") ;
    printf("     2: 读进程信息，执行调度算法\n") ;  
    printf("***********************************\n");
    printf( "Enter your choice (1 ~ 2): ");  
    
	do{
        ch = (char)_getch() ;
    } while(ch != '1' && ch != '2');//如果输入信息不正确，继续输入
	 
 
	if(ch == '2')  Scheduler_FF();     // 选择2，进入模拟进程调度函数        
	else if(ch == '1') Create_ProcInfo();  // 选择1，随机生成数据序列        
	 _getch() ; 
     system("cls") ;
  }
  getch() ;
}

/*
*随机创建进程
*/
void Create_ProcInfo(void) {
    int s;//进程数据序列长度

    srand(GetTickCount());//随机种子
    ProcNumber=((float) rand() / 32767) * 5 + 5;  // 随机产生数量5~10
    for(int i=0;i<ProcNumber;i++)    // 生成进程的CPU--I/O时间数据序列
	{ 
        proc[i].p_pid=((float) rand() / 32767) * 1000;  // 初始化随机的进程ID号
        proc[i].p_state='B';   // 初始都为后备状态，可用其他表示符
        
        s=((float) rand() / 32767) *6 + 6; // 产生的进程数据序列长度在6~12间
        proc[i].p_rserial[0]=s; // 第一项用于记录序列的长度
        for(int j=1;j<=s;j++)  // 生成时间数据序列，数值在10~30间
            proc[i].p_rserial[j]=((float) rand() / 32767) *10 + 5;
        
        // 赋其他字段的值
        proc[i].p_pos=1;//进程运行到的位置
        proc[i].p_starttime=((float) rand() / 32767) *49+1;//进程建立时间
        proc[i].p_endtime=0;//进程结束时间
        proc[i].p_cputime=proc[i].p_rserial[1];//剩余服务时间
        proc[i].p_iotime=proc[i].p_rserial[2];//剩余IO时间
        proc[i].p_next=-1;//进程控制块链接指针无指向
     } 
    printf("\n---------------------------\n    建立了%2d 个进程数据序列\n\n", ProcNumber);
    DisData();  // 该函数为在屏幕上打印所创建的进程的具体信息，Dis是Display的缩写.
    printf("\nPress Any Key To Continue.......");
    _getch() ;
    return ;
}

/*
*在屏幕打印出所创建的序列
*/
 void DisData()
{
    ofstream  outFile;     //同时将该信息写入磁盘的某txt文件中，该函数要#include <fstream>

    outFile.open("./创建序列.txt") ;  //该txt文件名可自己命名，第1次open时磁盘中没有该文件，系统会创建一个空白的。
    for(int i=0; i<ProcNumber; i++) 
    { 
        printf("ID:%4d(len= %2d , start=%2d):",proc[i].p_pid ,proc[i].p_rserial[0],proc[i].p_starttime );
        outFile<<"ID:"<<proc[i].p_pid<<"(len="<<proc[i].p_rserial[0]<<" , start="<<proc[i].p_starttime<<"):  ";
        for(int j=1; j<=proc[i].p_rserial[0];j++){
            printf("%4d",proc[i].p_rserial[j]);
            outFile<<proc[i].p_rserial[j]<<"   ";
        }      
        printf("\n" );
        outFile<<endl;
        //写到txt文件中
        //打印到屏幕上     
    }
    outFile.close(); // 写入txt文件的流被冲刷，保存到磁盘上
}

/*
*进程调度模拟算法，选择2直接进入此方法
*/
void Scheduler_FF(void)  //调度模拟算法
{
  if(ProcNumber==0) //该值居然是0？ 只能说用户没创建进程
  { 
	  Read_Process_Info();//那么，从磁盘读取上次创建的进程信息，赋值给相应变量
  }
  ClockNumber=0;// 时钟开始计时, 开始调度模拟

  while(FinishedProc < ProcNumber) // 还有未完成进程，执行算法
  {     
	  ClockNumber++;  // 时钟前进1个单位
	  NewReadyProc(); // 判别新进程是否到达
	  Cpu_Sched();    // CPU调度
    IO_Sched();     // IO调度      
	  Display_ProcInfo(); //显示当前状态
	  Sleep(700); 
  }
  DisResult(); 
  getch();   
}
/*
*判断是否有新进程到达
*/
void NewReadyProc(void)
{ 
	for(int i=0; i<ProcNumber; i++) 
	{	
		if (proc[i].p_starttime == ClockNumber) // 进程进入时间达到系统时间，ClockNumber是当前的系统时间
		{   
      proc[i].p_state='R'; // 进程状态修改为就绪	  	  
		  proc[i].p_next=-1; // 该进行即将要挂在队列末尾，它肯定是尾巴，后面没人的，所以先设置next=-1

      if (ReadyPoint==-1) // 如果当前就绪队列无进程
        ReadyPoint=i;
		  else // 如果就绪队列有进程，放入队列尾
		  {	 
        int n=ReadyPoint; 		  
        while(proc[n].p_next!=-1)   n=proc[n].p_next; // 找到原来队伍中的尾巴
        proc[n].p_next=i; // 挂在这个尾巴后面
		  }
		}
	} 
}       
/*
*CPU调度，每次只执行一个时间片
*/
void Cpu_Sched(void)
{ 
	int n;

	if (RunPoint == -1)    // 没有进程在CPU上执行
	{ 		NextRunProcess(); 	return; 	}
	
	proc[RunPoint].p_cputime--;      // 进程CPU执行时间减少1个时钟单位
	
  if (proc[RunPoint].p_cputime > 0) return; // 还需要CPU时间，下次继续，这次就返回了
	
  // 如果不满足以上>0的条件，就意味着=0，即此项已经结束，就不会自动返回，接着做以下事情。	
  // 进程完成本次CPU后的处理
  if (proc[RunPoint].p_rserial[0]==proc[RunPoint].p_pos) //进程全部序列执行完成
	{  
    FinishedProc++;//完成进程数+1
    proc[RunPoint].p_state ='F';//该进程状态为“完成”
    proc[RunPoint].p_endtime = ClockNumber;	//进程结束时间是当前时钟时间
    RunPoint=-1;  //无进程执行  
    NextRunProcess(); //找分派程序去，接着做下一个
	}	
	else //进程未结束，下一项进行IO操作，进入阻塞队列
	{
    proc[RunPoint].p_pos++; // 序列项+1
    proc[RunPoint].p_state ='W'; // 进程设置为阻塞状态
    proc[RunPoint].p_iotime =proc[RunPoint].p_rserial[proc[RunPoint].p_pos];
		
		proc[n].p_next == -1; //标记下，就自己一个进程，没带尾巴一起来；否则，当p_next不为-1时，后面的那一串都是被阻塞者
    /*
    *进入阻塞队列的位置
    */
    n=WaitPoint;
		if(n == -1) //是阻塞队列第一个I/O进程
		  WaitPoint=RunPoint; 
		else
		{	
      do //找到队尾
			{
				if(proc[n].p_next == -1) 
				{ 
          proc[n].p_next = RunPoint;	//放入阻塞队列第尾			  
				  break;
				}
				n=proc[n].p_next;
			} while(n!=-1) ;
		}
      RunPoint=-1; // 无进程执行  
		  NextRunProcess(); //分配程序分配执行进程
	}
	return;
}

/*
*选择下一个可以运行的进程
*/
void NextRunProcess(void)
{
  if (ReadyPoint==-1) { RunPoint = -1; return;}  // 就绪队列也没有等待的进程

  proc[ReadyPoint].p_state ='C'; // 就绪指针指向进程，设置成“运行”状态
  RunPoint=ReadyPoint; //运行进程指针指向就绪进程
  proc[ReadyPoint].p_cputime =proc[ReadyPoint].p_rserial[proc[ReadyPoint].p_pos] ;
  ReadyPoint=proc[ReadyPoint].p_next; // 就绪指针后移
  proc[RunPoint].p_next = -1; //运行的进程不设置下一个运行的进程指向
}
/*
*排在队首的得到一个时间片服务
*/
void IO_Sched(void)
{
  int n;

  if (WaitPoint==-1) return;   // 没有等待I/O的进程，直接返回

  proc[WaitPoint].p_iotime--;  // 进行1个时钟的I/O时间
  if (proc[WaitPoint].p_iotime > 0) return; // 还没有完成本次I/O

  /*
  *进程的I/O完成处理
  */
  if (proc[WaitPoint].p_rserial[0]==proc[WaitPoint].p_pos) //进程全部任务执行完成
  {
    FinishedProc++;
    proc[WaitPoint].p_endtime = ClockNumber;
    proc[WaitPoint].p_state ='F';
    
    //如果阻塞队列未空，要开始下一个进程的I/O
    if(proc[WaitPoint].p_next==-1)//阻塞队列已空
      { WaitPoint=-1;return ;}
    else //调度下一个进程进行I/O操作
      {
        int n=proc[WaitPoint].p_next;//标记下一个阻塞进程
        proc[WaitPoint].p_next=-1;//结束的I/O的进程控制块指针清空
        WaitPoint=n;//即将I/O的进程等于队列中下个阻塞进程
        proc[WaitPoint].p_iotime =proc[WaitPoint].p_rserial[proc[WaitPoint].p_pos]; // 更新下个I/O进程时间段需要的时间
        return ;
      }
  }
  else //进程还有任务未完成，进行下次CPU操作，进就绪队列
  {
    int nn=WaitPoint; // 标记已经I/O完的进程，即将进入就绪队列
    if(proc[WaitPoint].p_next==-1)//阻塞队列已空
      WaitPoint=-1;
    else //调度下一个进程进行I/O操作
      {
        int bk=proc[WaitPoint].p_next;//标记下一个阻塞进程
        proc[WaitPoint].p_next=-1;//结束的I/O的进程控制块指针清空
        WaitPoint=bk;//即将I/O的进程等于队列中下个阻塞进程
        proc[WaitPoint].p_iotime =proc[WaitPoint].p_rserial[proc[WaitPoint].p_pos]; // 更新下个I/O进程时间段需要的时间
      }
    
    proc[nn].p_pos++; //此进程已经完成的时间段+1
    proc[nn].p_state ='R'; //进程状态为就绪
    proc[nn].p_next =-1;

    n=ReadyPoint;
    if(n == -1) //是就绪队列的第一个进程
    {  ReadyPoint=nn; return; }
    else//插入就绪队列队尾
    {  
      do
      {
        if(proc[n].p_next == -1) { proc[n].p_next = nn;  break ; }
        n=proc[n].p_next;
      }while(n!=-1);
    }
  }
  return ;
}

/*
显示系统当前状态
*/
void  Display_ProcInfo( )
{      
   system("cls") ;
   printf("\n        当前系统模拟%2d 个进程的运行    时钟：%ld\n\n", ProcNumber,ClockNumber);

   printf("        就绪指针=%d, 运行指针=%d, 阻塞指针=%d\n\n",ReadyPoint,RunPoint,WaitPoint );
   
  if(RunPoint!= -1)
  {
    printf(" ............... Running Process ...............\n No.%d ID:%d(len=%2d,start=%2d)", RunPoint,proc[RunPoint].p_pid,proc[RunPoint].p_rserial[0],proc[RunPoint].p_starttime);
    printf(" 总CPU时间=%d, 剩余CPU时间=%d\n",proc[RunPoint].p_rserial[proc[RunPoint].p_pos],proc[RunPoint].p_cputime);
    printf(" ////////////////////////////////////////////////\n");
  }
  else
      printf(" No Process Running !\n ////////////////////////////////////////////////\n");
    
  int n=ReadyPoint;
  printf("\n Ready Process ...... \n");
  while(n!=-1) // 显示就绪进程信息
  {
    printf(" No.%d ID:%d(len=%2d,start=%2d),第%d个/总时间=%d",n,proc[n].p_pid,proc[n].p_rserial[0],proc[n].p_starttime,proc[n].p_pos,proc[n].p_rserial[proc[n].p_pos] );
    printf("\t\t\tserial:");
    for(int j=1; j<=proc[n].p_rserial[0];j++)
      printf("%4d",proc[n].p_rserial[j]);
    printf("\n");
    n=proc[n].p_next;
  }
  printf(" ////////////////////////////////////////////////\n");

  n=WaitPoint;
  printf("\n Waiting Process ...... \n");
  while(n!=-1) // 显示阻塞进程信息
  {
    printf(" No.%d ID:%d(len=%2d,start=%2d),第%d个/总时间=%d,剩余I/O时间=%d",n,proc[n].p_pid,proc[n].p_rserial[0],proc[n].p_starttime,proc[n].p_pos,proc[n].p_rserial[proc[n].p_pos],proc[n].p_iotime);
    printf("\tserial:");
    for(int j=1; j<=proc[n].p_rserial[0];j++)
      printf("%4d",proc[n].p_rserial[j]);
    printf("\n");
    n=proc[n].p_next;
  }
  printf(" ////////////////////////////////////////////////\n");

  printf("\n=================== 后备进程 ====================\n");
  for(int i=0; i<ProcNumber; i++) 
    if (proc[i].p_state=='B')
      printf("     No.%d ID:%d(len=%2d,start=%2d)\n",i,proc[i].p_pid,proc[i].p_rserial[0],proc[i].p_starttime);
  printf("\n=================================================\n");
   
  printf("\n================ 已经完成的进程 =================\n");
  for(int i=0; i<ProcNumber; i++) 
    if (proc[i].p_state=='F')
      printf("     No.%d ID:%d(len=%2d,start=%2d),Endtime=%d\n",i,proc[i].p_pid,proc[i].p_rserial[0],proc[i].p_starttime,proc[i].p_endtime);
  printf("\n=================================================\n");
}

/*
*显示模拟执行结果
*/
void DisResult(void)
{ 
  int i;
  printf("\n=================================================\n");
  printf("标识号——时间序列-建立时间-结束时间-周转时间\n");
  for(i=0; i<ProcNumber; i++)
  printf("ID=%4d  %4d      %4d      %4d      %4d\n",proc[i].p_pid ,proc[i].p_rserial[0],proc[i].p_starttime,proc[i].p_endtime,proc[i].p_endtime-proc[i].p_starttime );
}
/*
*生成进程信息文件
*/
void Read_Process_Info(  )
{
  ifstream inFile;     // 定义打开文件的文件流
  char ch; 
  int i,j,k,tmp;		
  inFile.open("./创建序列.txt") ; // 打开上次写的txt进行信息文件流   
  
  i=0;

  /*
  *读取进程数据并初始化进程状态
  */
  while(inFile)
  { 
    inFile.get(ch);

    for(j=0;j<3;j++) inFile.get(ch);//扔掉3个字符，即“ID:”

    inFile>>proc[i].p_pid;//获取进程ID

    for(j=0;j<5;j++) inFile.get(ch);//继续读，扔掉5个字符，即“(len=”
    
    inFile>>proc[i].p_rserial[0];//获取此进程序列长度（项数）

    for(j=0;j<7;j++) inFile.get(ch);//继续读，扔掉7个字符，即“, start=”

    inFile>>proc[i].p_starttime;//获取进程到达时间

    for(j=0;j<2;j++) inFile.get(ch);//继续读，扔掉2个字符，即“): ”
                  
    for(k=1;k<=proc[i].p_rserial[0];k++)//获取此进程每个数据序列
    {
      inFile>>tmp;
                    
      proc[i].p_rserial[k]=tmp;
    }
    /*
    *初始化该进程
    */
    proc[i].p_state='B';//该进程进入后备队列
    proc[i].p_pos=1;//当前进程运行到的位置
    proc[i].p_endtime=0; //进程运行结束时间
    proc[i].p_next=-1; //进程控制块链接指针
    proc[i].p_cputime=proc[i].p_rserial[1];//当前运行时间段剩余运行时间
    proc[i].p_iotime=proc[i].p_rserial[2];//当前I/O时间段剩余运行时间 

    i++; //本行结束，一个进程信息读完，序号+1, 准备 next process
    
  }
		ProcNumber=i;  //给ProcNumber进程数赋值，i最后有++，下标从0~i-1
		inFile.close();//完工后，记得归位，关灯。
}



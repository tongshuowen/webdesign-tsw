#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>//_getch()ͷ�ļ�������һ���ַ�������Ҫ�س��Զ�����
#include<fstream>//д���ļ�
#include<windows.h>//Sleep(1000),1s�ӳ�
using namespace std;

/*
*ȫ�ֱ���
*/
int   RunPoint;   // ���н���ָ�룬-1ʱΪû�����н���
int   WaitPoint;  // ��������ָ�룬-1ʱΪû����������
int   ReadyPoint; // ��������ָ�룬-1ʱΪû�о�������
long  ClockNumber;   // ϵͳʱ��
int   ProcNumber;    // ϵͳ��ģ������Ľ�������
int   FinishedProc;    // ϵͳ��Ŀǰ��ִ����ϵĽ�������
/*
*������Ϣ�ṹ
*/
struct ProcStruct  
{
  int  p_pid;         // ���̵ı�ʶ��
  char p_state;       // ���̵�״̬��C--����  R--����  W--����  B--��  F--���
  int  p_rserial[10]; // ģ��Ľ���ִ�е�CPU��I/Oʱ���������У�����洢����0��洢������еĳ��ȣ����������Ա�֪��ɶʱ�ý���ִ�н���
  int  p_pos;         // ��ǰ�������е���λ�ã���������ִ�е������е�����
  int  p_starttime;   // ���̽���ʱ��
  int  p_endtime;     // �������н���ʱ��
  int  p_cputime;     // ��ǰ����ʱ��ν���ʣ�����Ҫ����ʱ��
  int  p_iotime;      // ��ǰI/Oʱ��ν���ʣ���I/Oʱ��
  int  p_next;        // ���̿��ƿ������ָ��
} proc[10];

/*
*���ܺ���
*/
void Create_ProcInfo(void);//����������̡�
void DisData();//����Ļ��ӡ�����������С�
void Scheduler_FF(void);//����ģ������㷨
/*
*���ȹ���
*/
void NewReadyProc(void);//�ж��Ƿ����½��̵����������������
void Cpu_Sched(void);//�������ִֻ��һ��ʱ��Ƭ
void NextRunProcess(void);//ѡ����һ���������еĽ���
void IO_Sched(void);//IO���ȣ����ڶ��׵ĵõ�һ��ʱ��Ƭ
void Display_ProcInfo( );//��ʾϵͳ��ǰ״̬
void Read_Process_Info(  );//���ɽ����ļ�
void DisResult(void);//��ʾģ����

int main( )
{
  char ch;  
  RunPoint=-1;   // ���н���ָ�룬-1ʱΪû�����н���
  WaitPoint=-1;  // ��������ָ�룬-1ʱΪû����������
  ReadyPoint=-1; // ��������ָ�룬-1ʱΪû�о�������
  ClockNumber=0;   // ϵͳʱ��
  ProcNumber=0;    // ��ǰϵͳ�еĽ�������
  FinishedProc=0;// ϵͳ��Ŀǰ��ִ����ϵĽ�������Ϊ0

  system("cls") ;
  while ( true )                           
  {
    printf("***********************************\n");
    printf("     1: �������̵����������� \n") ;
    printf("     2: ��������Ϣ��ִ�е����㷨\n") ;  
    printf("***********************************\n");
    printf( "Enter your choice (1 ~ 2): ");  
    
	do{
        ch = (char)_getch() ;
    } while(ch != '1' && ch != '2');//���������Ϣ����ȷ����������
	 
 
	if(ch == '2')  Scheduler_FF();     // ѡ��2������ģ����̵��Ⱥ���        
	else if(ch == '1') Create_ProcInfo();  // ѡ��1�����������������        
	 _getch() ; 
     system("cls") ;
  }
  getch() ;
}

/*
*�����������
*/
void Create_ProcInfo(void) {
    int s;//�����������г���

    srand(GetTickCount());//�������
    ProcNumber=((float) rand() / 32767) * 5 + 5;  // �����������5~10
    for(int i=0;i<ProcNumber;i++)    // ���ɽ��̵�CPU--I/Oʱ����������
	{ 
        proc[i].p_pid=((float) rand() / 32767) * 1000;  // ��ʼ������Ľ���ID��
        proc[i].p_state='B';   // ��ʼ��Ϊ��״̬������������ʾ��
        
        s=((float) rand() / 32767) *6 + 6; // �����Ľ����������г�����6~12��
        proc[i].p_rserial[0]=s; // ��һ�����ڼ�¼���еĳ���
        for(int j=1;j<=s;j++)  // ����ʱ���������У���ֵ��10~30��
            proc[i].p_rserial[j]=((float) rand() / 32767) *10 + 5;
        
        // �������ֶε�ֵ
        proc[i].p_pos=1;//�������е���λ��
        proc[i].p_starttime=((float) rand() / 32767) *49+1;//���̽���ʱ��
        proc[i].p_endtime=0;//���̽���ʱ��
        proc[i].p_cputime=proc[i].p_rserial[1];//ʣ�����ʱ��
        proc[i].p_iotime=proc[i].p_rserial[2];//ʣ��IOʱ��
        proc[i].p_next=-1;//���̿��ƿ�����ָ����ָ��
     } 
    printf("\n---------------------------\n    ������%2d ��������������\n\n", ProcNumber);
    DisData();  // �ú���Ϊ����Ļ�ϴ�ӡ�������Ľ��̵ľ�����Ϣ��Dis��Display����д.
    printf("\nPress Any Key To Continue.......");
    _getch() ;
    return ;
}

/*
*����Ļ��ӡ��������������
*/
 void DisData()
{
    ofstream  outFile;     //ͬʱ������Ϣд����̵�ĳtxt�ļ��У��ú���Ҫ#include <fstream>

    outFile.open("./��������.txt") ;  //��txt�ļ������Լ���������1��openʱ������û�и��ļ���ϵͳ�ᴴ��һ���հ׵ġ�
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
        //д��txt�ļ���
        //��ӡ����Ļ��     
    }
    outFile.close(); // д��txt�ļ���������ˢ�����浽������
}

/*
*���̵���ģ���㷨��ѡ��2ֱ�ӽ���˷���
*/
void Scheduler_FF(void)  //����ģ���㷨
{
  if(ProcNumber==0) //��ֵ��Ȼ��0�� ֻ��˵�û�û��������
  { 
	  Read_Process_Info();//��ô���Ӵ��̶�ȡ�ϴδ����Ľ�����Ϣ����ֵ����Ӧ����
  }
  ClockNumber=0;// ʱ�ӿ�ʼ��ʱ, ��ʼ����ģ��

  while(FinishedProc < ProcNumber) // ����δ��ɽ��̣�ִ���㷨
  {     
	  ClockNumber++;  // ʱ��ǰ��1����λ
	  NewReadyProc(); // �б��½����Ƿ񵽴�
	  Cpu_Sched();    // CPU����
    IO_Sched();     // IO����      
	  Display_ProcInfo(); //��ʾ��ǰ״̬
	  Sleep(700); 
  }
  DisResult(); 
  getch();   
}
/*
*�ж��Ƿ����½��̵���
*/
void NewReadyProc(void)
{ 
	for(int i=0; i<ProcNumber; i++) 
	{	
		if (proc[i].p_starttime == ClockNumber) // ���̽���ʱ��ﵽϵͳʱ�䣬ClockNumber�ǵ�ǰ��ϵͳʱ��
		{   
      proc[i].p_state='R'; // ����״̬�޸�Ϊ����	  	  
		  proc[i].p_next=-1; // �ý��м���Ҫ���ڶ���ĩβ�����϶���β�ͣ�����û�˵ģ�����������next=-1

      if (ReadyPoint==-1) // �����ǰ���������޽���
        ReadyPoint=i;
		  else // ������������н��̣��������β
		  {	 
        int n=ReadyPoint; 		  
        while(proc[n].p_next!=-1)   n=proc[n].p_next; // �ҵ�ԭ�������е�β��
        proc[n].p_next=i; // �������β�ͺ���
		  }
		}
	} 
}       
/*
*CPU���ȣ�ÿ��ִֻ��һ��ʱ��Ƭ
*/
void Cpu_Sched(void)
{ 
	int n;

	if (RunPoint == -1)    // û�н�����CPU��ִ��
	{ 		NextRunProcess(); 	return; 	}
	
	proc[RunPoint].p_cputime--;      // ����CPUִ��ʱ�����1��ʱ�ӵ�λ
	
  if (proc[RunPoint].p_cputime > 0) return; // ����ҪCPUʱ�䣬�´μ�������ξͷ�����
	
  // �������������>0������������ζ��=0���������Ѿ��������Ͳ����Զ����أ��������������顣	
  // ������ɱ���CPU��Ĵ���
  if (proc[RunPoint].p_rserial[0]==proc[RunPoint].p_pos) //����ȫ������ִ�����
	{  
    FinishedProc++;//��ɽ�����+1
    proc[RunPoint].p_state ='F';//�ý���״̬Ϊ����ɡ�
    proc[RunPoint].p_endtime = ClockNumber;	//���̽���ʱ���ǵ�ǰʱ��ʱ��
    RunPoint=-1;  //�޽���ִ��  
    NextRunProcess(); //�ҷ��ɳ���ȥ����������һ��
	}	
	else //����δ��������һ�����IO������������������
	{
    proc[RunPoint].p_pos++; // ������+1
    proc[RunPoint].p_state ='W'; // ��������Ϊ����״̬
    proc[RunPoint].p_iotime =proc[RunPoint].p_rserial[proc[RunPoint].p_pos];
		
		proc[n].p_next == -1; //����£����Լ�һ�����̣�û��β��һ���������򣬵�p_next��Ϊ-1ʱ���������һ�����Ǳ�������
    /*
    *�����������е�λ��
    */
    n=WaitPoint;
		if(n == -1) //���������е�һ��I/O����
		  WaitPoint=RunPoint; 
		else
		{	
      do //�ҵ���β
			{
				if(proc[n].p_next == -1) 
				{ 
          proc[n].p_next = RunPoint;	//�����������е�β			  
				  break;
				}
				n=proc[n].p_next;
			} while(n!=-1) ;
		}
      RunPoint=-1; // �޽���ִ��  
		  NextRunProcess(); //����������ִ�н���
	}
	return;
}

/*
*ѡ����һ���������еĽ���
*/
void NextRunProcess(void)
{
  if (ReadyPoint==-1) { RunPoint = -1; return;}  // ��������Ҳû�еȴ��Ľ���

  proc[ReadyPoint].p_state ='C'; // ����ָ��ָ����̣����óɡ����С�״̬
  RunPoint=ReadyPoint; //���н���ָ��ָ���������
  proc[ReadyPoint].p_cputime =proc[ReadyPoint].p_rserial[proc[ReadyPoint].p_pos] ;
  ReadyPoint=proc[ReadyPoint].p_next; // ����ָ�����
  proc[RunPoint].p_next = -1; //���еĽ��̲�������һ�����еĽ���ָ��
}
/*
*���ڶ��׵ĵõ�һ��ʱ��Ƭ����
*/
void IO_Sched(void)
{
  int n;

  if (WaitPoint==-1) return;   // û�еȴ�I/O�Ľ��̣�ֱ�ӷ���

  proc[WaitPoint].p_iotime--;  // ����1��ʱ�ӵ�I/Oʱ��
  if (proc[WaitPoint].p_iotime > 0) return; // ��û����ɱ���I/O

  /*
  *���̵�I/O��ɴ���
  */
  if (proc[WaitPoint].p_rserial[0]==proc[WaitPoint].p_pos) //����ȫ������ִ�����
  {
    FinishedProc++;
    proc[WaitPoint].p_endtime = ClockNumber;
    proc[WaitPoint].p_state ='F';
    
    //�����������δ�գ�Ҫ��ʼ��һ�����̵�I/O
    if(proc[WaitPoint].p_next==-1)//���������ѿ�
      { WaitPoint=-1;return ;}
    else //������һ�����̽���I/O����
      {
        int n=proc[WaitPoint].p_next;//�����һ����������
        proc[WaitPoint].p_next=-1;//������I/O�Ľ��̿��ƿ�ָ�����
        WaitPoint=n;//����I/O�Ľ��̵��ڶ������¸���������
        proc[WaitPoint].p_iotime =proc[WaitPoint].p_rserial[proc[WaitPoint].p_pos]; // �����¸�I/O����ʱ�����Ҫ��ʱ��
        return ;
      }
  }
  else //���̻�������δ��ɣ������´�CPU����������������
  {
    int nn=WaitPoint; // ����Ѿ�I/O��Ľ��̣����������������
    if(proc[WaitPoint].p_next==-1)//���������ѿ�
      WaitPoint=-1;
    else //������һ�����̽���I/O����
      {
        int bk=proc[WaitPoint].p_next;//�����һ����������
        proc[WaitPoint].p_next=-1;//������I/O�Ľ��̿��ƿ�ָ�����
        WaitPoint=bk;//����I/O�Ľ��̵��ڶ������¸���������
        proc[WaitPoint].p_iotime =proc[WaitPoint].p_rserial[proc[WaitPoint].p_pos]; // �����¸�I/O����ʱ�����Ҫ��ʱ��
      }
    
    proc[nn].p_pos++; //�˽����Ѿ���ɵ�ʱ���+1
    proc[nn].p_state ='R'; //����״̬Ϊ����
    proc[nn].p_next =-1;

    n=ReadyPoint;
    if(n == -1) //�Ǿ������еĵ�һ������
    {  ReadyPoint=nn; return; }
    else//����������ж�β
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
��ʾϵͳ��ǰ״̬
*/
void  Display_ProcInfo( )
{      
   system("cls") ;
   printf("\n        ��ǰϵͳģ��%2d �����̵�����    ʱ�ӣ�%ld\n\n", ProcNumber,ClockNumber);

   printf("        ����ָ��=%d, ����ָ��=%d, ����ָ��=%d\n\n",ReadyPoint,RunPoint,WaitPoint );
   
  if(RunPoint!= -1)
  {
    printf(" ............... Running Process ...............\n No.%d ID:%d(len=%2d,start=%2d)", RunPoint,proc[RunPoint].p_pid,proc[RunPoint].p_rserial[0],proc[RunPoint].p_starttime);
    printf(" ��CPUʱ��=%d, ʣ��CPUʱ��=%d\n",proc[RunPoint].p_rserial[proc[RunPoint].p_pos],proc[RunPoint].p_cputime);
    printf(" ////////////////////////////////////////////////\n");
  }
  else
      printf(" No Process Running !\n ////////////////////////////////////////////////\n");
    
  int n=ReadyPoint;
  printf("\n Ready Process ...... \n");
  while(n!=-1) // ��ʾ����������Ϣ
  {
    printf(" No.%d ID:%d(len=%2d,start=%2d),��%d��/��ʱ��=%d",n,proc[n].p_pid,proc[n].p_rserial[0],proc[n].p_starttime,proc[n].p_pos,proc[n].p_rserial[proc[n].p_pos] );
    printf("\t\t\tserial:");
    for(int j=1; j<=proc[n].p_rserial[0];j++)
      printf("%4d",proc[n].p_rserial[j]);
    printf("\n");
    n=proc[n].p_next;
  }
  printf(" ////////////////////////////////////////////////\n");

  n=WaitPoint;
  printf("\n Waiting Process ...... \n");
  while(n!=-1) // ��ʾ����������Ϣ
  {
    printf(" No.%d ID:%d(len=%2d,start=%2d),��%d��/��ʱ��=%d,ʣ��I/Oʱ��=%d",n,proc[n].p_pid,proc[n].p_rserial[0],proc[n].p_starttime,proc[n].p_pos,proc[n].p_rserial[proc[n].p_pos],proc[n].p_iotime);
    printf("\tserial:");
    for(int j=1; j<=proc[n].p_rserial[0];j++)
      printf("%4d",proc[n].p_rserial[j]);
    printf("\n");
    n=proc[n].p_next;
  }
  printf(" ////////////////////////////////////////////////\n");

  printf("\n=================== �󱸽��� ====================\n");
  for(int i=0; i<ProcNumber; i++) 
    if (proc[i].p_state=='B')
      printf("     No.%d ID:%d(len=%2d,start=%2d)\n",i,proc[i].p_pid,proc[i].p_rserial[0],proc[i].p_starttime);
  printf("\n=================================================\n");
   
  printf("\n================ �Ѿ���ɵĽ��� =================\n");
  for(int i=0; i<ProcNumber; i++) 
    if (proc[i].p_state=='F')
      printf("     No.%d ID:%d(len=%2d,start=%2d),Endtime=%d\n",i,proc[i].p_pid,proc[i].p_rserial[0],proc[i].p_starttime,proc[i].p_endtime);
  printf("\n=================================================\n");
}

/*
*��ʾģ��ִ�н��
*/
void DisResult(void)
{ 
  int i;
  printf("\n=================================================\n");
  printf("��ʶ�š���ʱ������-����ʱ��-����ʱ��-��תʱ��\n");
  for(i=0; i<ProcNumber; i++)
  printf("ID=%4d  %4d      %4d      %4d      %4d\n",proc[i].p_pid ,proc[i].p_rserial[0],proc[i].p_starttime,proc[i].p_endtime,proc[i].p_endtime-proc[i].p_starttime );
}
/*
*���ɽ�����Ϣ�ļ�
*/
void Read_Process_Info(  )
{
  ifstream inFile;     // ������ļ����ļ���
  char ch; 
  int i,j,k,tmp;		
  inFile.open("./��������.txt") ; // ���ϴ�д��txt������Ϣ�ļ���   
  
  i=0;

  /*
  *��ȡ�������ݲ���ʼ������״̬
  */
  while(inFile)
  { 
    inFile.get(ch);

    for(j=0;j<3;j++) inFile.get(ch);//�ӵ�3���ַ�������ID:��

    inFile>>proc[i].p_pid;//��ȡ����ID

    for(j=0;j<5;j++) inFile.get(ch);//���������ӵ�5���ַ�������(len=��
    
    inFile>>proc[i].p_rserial[0];//��ȡ�˽������г��ȣ�������

    for(j=0;j<7;j++) inFile.get(ch);//���������ӵ�7���ַ�������, start=��

    inFile>>proc[i].p_starttime;//��ȡ���̵���ʱ��

    for(j=0;j<2;j++) inFile.get(ch);//���������ӵ�2���ַ�������): ��
                  
    for(k=1;k<=proc[i].p_rserial[0];k++)//��ȡ�˽���ÿ����������
    {
      inFile>>tmp;
                    
      proc[i].p_rserial[k]=tmp;
    }
    /*
    *��ʼ���ý���
    */
    proc[i].p_state='B';//�ý��̽���󱸶���
    proc[i].p_pos=1;//��ǰ�������е���λ��
    proc[i].p_endtime=0; //�������н���ʱ��
    proc[i].p_next=-1; //���̿��ƿ�����ָ��
    proc[i].p_cputime=proc[i].p_rserial[1];//��ǰ����ʱ���ʣ������ʱ��
    proc[i].p_iotime=proc[i].p_rserial[2];//��ǰI/Oʱ���ʣ������ʱ�� 

    i++; //���н�����һ��������Ϣ���꣬���+1, ׼�� next process
    
  }
		ProcNumber=i;  //��ProcNumber��������ֵ��i�����++���±��0~i-1
		inFile.close();//�깤�󣬼ǵù�λ���صơ�
}



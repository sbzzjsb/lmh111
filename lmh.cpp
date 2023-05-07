#include <reg51.h>
#include <stdio.h>
#include <intrins.h> 
#define uchar unsigned char
#define uint unsigned int
#define key_pin P1

uchar code SEGCC_CODE[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71}; //七段译码 
uchar code SEGLOCAL_CODE[]={0x20,0x10,0x08,0x04,0x02,0x01};//数码管选择
uchar xdata *px;//片选px
uchar xdata *wx;//位选wx 
uchar data watch[]={59};//时间显示计数
uchar data score[]={00,00};//比分
uchar count1;//定时器1循环计数
uchar stopwatch_status;//秒表状态
bit watch_status=0;//显示状态切换
uchar count2;//锁
uchar key;
uchar flag;

void myinitinterrupt();
void myinitvar();
void DelayMS(uint x);
void Keyscan();

void main(void)
 { 
   myinitinterrupt();
   myinitvar();
   while (1)
   {
   Keyscan(); 
	 //甲队分数显示
	 *px=SEGLOCAL_CODE[0];
	 *wx=SEGCC_CODE[score[0]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[1];
	 *wx=SEGCC_CODE[score[0]%10];  
	 DelayMS(1);
	 //计时器 毫秒显示
	 if(flag==1||watch[0]!=59){
	 *px=SEGLOCAL_CODE[2];
	 *wx=SEGCC_CODE[watch[0]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[3];
	 *wx=SEGCC_CODE[watch[0]%10];
	 DelayMS(1);
	 }
	 //乙队分数显示
	 *px=SEGLOCAL_CODE[4];
	 *wx=SEGCC_CODE[score[1]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[5];
	 *wx=SEGCC_CODE[score[1]%10];
	 DelayMS(1);
	 if(count2==0){
			count2=150;
			key=1;
	 }
	 else
		 count2--;
      
   }//end while
 }
 //配置中断
 void myinitinterrupt()
{
	 count2=150;
	 key=1;
	 flag=0;
   EX0=1;
   IT0=1;       //下降沿触发
   //外部中断0负责秒表开始、暂停、结束
   TMOD=0x10;
   ET1=1;
   TH1=(65536-50000)/256; 
   TL1=(65536-50000)%256; 
   count1=20;
   //T1负责正常时间计时，1S一次  

   EA=1;
}
//初始化变量
void myinitvar()
{
   //stopwatch_status=0;//秒表状态 0:停止 1:运行 2:暂停 
   //watch_status=0;//显示状态切换 0:计时状态 1:时间显示状态
   px=0x8002;//片选地址
   wx=0x8004;//位选地址
}
 //ms延时 

void Keyscan()   {  //分数的控制
	uchar key_value=0; 
	if(key&&key_pin!=0xff)   //按键状态发生改变
	{
		_nop_(),_nop_(),_nop_(),_nop_();   //消抖
		key_value=P1;   //保存按键的状态
		if(key_pin!=0xff)
		{
			switch(key_value)
			{
				case 0x01:score[0]=0;break;
				case 0x02:score[0]+=1;break;
				case 0x04:score[0]+=2;break;
				case 0x08:score[0]+=3;break;
				case 0x10:score[1]=0;break;
				case 0x20:score[1]+=1;break;
				case 0x40:score[1]+=2;break;
				case 0x80:score[1]+=3;break;
				default:break;
			}
			key=0;
		}
		DelayMS(1);
	} 
}
void DelayMS(uint x)
 {  
   uchar i;
   while(x--)
      {
	 for(i=0;i<120;i++);
      }
 }
 
 void myint0() interrupt 0     //中断0控制倒计时的开始与暂停
 {
    if(watch_status==0)
    {
		flag=1;
	  watch_status=1;
	  TR1=1;
		}
    else
    {
	  watch_status=0;
	  TR1=0;
		flag=0;
    }
 }
 
 void mytime1() interrupt 3 
 {
    
    TH1=(65536-50000)/256; 
    TL1=(65536-50000)%256;   //50ms
    if(count1>0)
    {
       count1--;
       return;
    }
    count1=20;               //20次为1s
		if(watch[0]==0){
			return;
		}
    else
			 watch[0]--;
	  }

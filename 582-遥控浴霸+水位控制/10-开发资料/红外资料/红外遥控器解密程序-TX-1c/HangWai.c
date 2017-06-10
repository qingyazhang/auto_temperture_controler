#include <reg52.h>
#define uchar unsigned char 

//天祥板子的外部中断1
sbit dula=P2^6;
sbit wela=P2^7;
sbit LEDR=P2^2;
uchar code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,
                    0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

//时间计算
#define Imax 14000    //此处为晶振为11.0592时的取值, 
#define Imin 8000    //如用其它频率的晶振时,
#define Inum1 1450    //要改变相应的取值。
#define Inum2 700 
#define Inum3 3000 
//解码变量
unsigned char Im[4]={0x00,0x00,0x00,0x00};
uchar show[2]={0,0};

//全局变量
uchar f;
unsigned long m,Tc;
unsigned char IrOK;


//延时
void delay(uchar i)
{
  uchar j,k; 
  for(j=i;j>0;j--)
    for(k=125;k>0;k--);
}

//数码管显示
void display()
{
   dula=0;
   P0=table[show[0]];
   dula=1;
   dula=0;
   
   wela=0;
   P0=0xfe;
   wela=1;
   wela=0;
   delay(5);
   
   P0=table[show[1]];
   dula=1;
   dula=0;
   
   P0=0xfd;
   wela=1;
   wela=0;
   delay(5);
}


//外部中断解码程序_外部中断0
void intersvr1(void) interrupt 2 using 1
{
 TR0=1;
 Tc=TH0*256+TL0;//提取中断时间间隔时长
 TH0=0; 
 TL0=0;         //定时中断重新置零
 if((Tc>Imin)&&(Tc<Imax))
 { 
	m=0;
	f=1;
	return;
 }       //找到启始码
 if(f==1)
 {
    if(Tc>Inum1&&Tc<Inum3) 
    {
   	Im[m/8]=Im[m/8]>>1|0x80; m++; 
    }
    if(Tc>Inum2&&Tc<Inum1) 
    {
      Im[m/8]=Im[m/8]>>1; m++; //取码
  	 }
  	 if(m==32) 
    {
      m=0;  
      f=0;
      if(Im[2]==~Im[3]) 
      {
           IrOK=1;
			  TR0=0; 
   	}
      else IrOK=0;   //取码完成后判断读码是否正确
    }
               //准备读下一码
 }
 
}


/*演示主程序*/
void main(void)
{
 unsigned int  a;
   
 m=0;
 f=0;
 EA=1;//开启总中断
 
 IT1=1;//下降沿有效
 EX1=1;//外部中断1开
 
 TMOD=0x11;//定时器初始化  
 TH0=0;//T0赋初值
 TL0=0;
 TR0=0;//t0开始计时

 while(1)
 {
 
  if(IrOK==1) 
  {
      show[1]=Im[2] & 0x0F;     //取键码的低四位
      show[0]=Im[2] >> 4;  
      IrOK=0;
     LEDR=0;
  }
  for(a=100;a>0;a--)
   {
    //display();
   }
 
 }
}


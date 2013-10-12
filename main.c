
#include "iostm8l152c6.h"

#define STRINGVECTOR(x) #x
#define ISR( a, b ) \
  _Pragma( STRINGVECTOR( vector = (b) ) ) \
  __interrupt void (a)( void )

    
void delay( void )      //�������� 15 ������.
{
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");  
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
}

void delay_some( void )      //��������.
{
  for (unsigned long delay_count=0; delay_count<500; delay_count++);
}

void Init( void )
{
  CLK_CKDIVR = 0x01;    //������������ ����������� 2, ������� 8���
  
#define Data 0
#define Clock 2
#define Enable 4
  
  //PB_DDR = (1<<Data)|(1<<Clock)|(1<<Enable);
  PB_DDR_bit.DDR0 = 1;  //Data
  PB_DDR_bit.DDR2 = 1;  //Clock
  PB_DDR_bit.DDR4 = 1;  //Enable
  PB_CR1_bit.C10 = 1;
  PB_CR1_bit.C12 = 1;   //���������� � ����� push-pull, ����� �������� ���. 1
  PB_CR1_bit.C14 = 1; 
  PB_ODR_bit.ODR0 = 0;
  PB_ODR_bit.ODR2 = 0;   //��������� ������ � 0
  PB_ODR_bit.ODR4 = 0; 
}

void shiftreg( int Cmd, int RS, int HorL )
{
  if( HorL == 1 )       //������� � ������� ������� ��������
    {
      int Mask = 128;          //����� ��� �������� �����
      PB_ODR_bit.ODR0 = RS;     //������������ ��� �������/������
      PB_ODR_bit.ODR2 = 1;      //�������� � �������
      PB_ODR_bit.ODR2 = 0;      //������� ����� � �������
      for ( int i = 0; i < 4; i++)      //���� 4 �����, ������� �� ��������
        {
          if ( Cmd & Mask )     //���� ��������� ������� �� ����
            {
              PB_ODR_bit.ODR0 = 1;      //������������ ������� �� ����� ������
            }
            else
            {
              PB_ODR_bit.ODR0 = 0;      //����� ����
            }
        PB_ODR_bit.ODR2 = 1;      //�������� � �������
        PB_ODR_bit.ODR2 = 0;      //������� ����� � �������
        Mask = Mask / 2;          //���� ����� �� 2 �������� � �������� ����
        }
      PB_ODR_bit.ODR0 = 0;    //���� ���� �� ������� �� �����
      PB_ODR_bit.ODR2 = 1;    //�������� � ������� ��� ���
      PB_ODR_bit.ODR2 = 0;    //��� ���� ������, ��� ����� � ������� ����������
    }
 if( HorL == 0 )       //������� � ������� ������ ��������
    {
      int Mask = 8;          //����� ��� �������� �����
      PB_ODR_bit.ODR0 = RS;     //������������ ��� �������/������
      PB_ODR_bit.ODR2 = 1;      //�������� � �������
      PB_ODR_bit.ODR2 = 0;      //������� ����� � �������
      for ( int i = 0; i < 4; i++)      //���� 4 �����, ������� �� ��������
        {
          if ( Cmd & Mask )     //���� ��������� ������� �� ����
            {
              PB_ODR_bit.ODR0 = 1;      //������������ ������� �� ����� ������
            }
            else
            {
              PB_ODR_bit.ODR0 = 0;      //����� ����
            }
        PB_ODR_bit.ODR2 = 1;      //�������� � �������
        PB_ODR_bit.ODR2 = 0;      //������� ����� � �������
        Mask = Mask / 2;          //���� ����� �� 2 �������� � �������� ����
        }
      PB_ODR_bit.ODR0 = 0;    //���� ���� �� ������� �� �����
      PB_ODR_bit.ODR2 = 1;    //�������� � ������� ��� ���
      PB_ODR_bit.ODR2 = 0;    //��� ���� ������, ��� ����� � ������� ����������
    }
}

void LCD_cmd( int Cmd, int RS )
{
  PB_ODR_bit.ODR4 = 1;          //������ Enable �������
  shiftreg ( Cmd, RS, 1 );      //���� � ������� ������� ��������
  delay();                      //������ ����������
  PB_ODR_bit.ODR4 = 0;          //���������� ������� �������
  delay_some();                 //������ ����������
  PB_ODR_bit.ODR4 = 1;          //������ Enable �������
  shiftreg ( Cmd, RS, 0 );      //���� � ������� ������� ��������
  delay();                      //������ ����������
  PB_ODR_bit.ODR4 = 0;          //���������� ������� �������
  delay_some();                 //������ ����������
}

int main( void )
{    
  Init();
  
  LCD_cmd( 0x20, 0 );      //���������� ����� � 4-��� �����
  LCD_cmd( 0x28, 0 );      //���������� ����������� ����� - 2
  LCD_cmd( 0x01, 0 );      //���� �����
  LCD_cmd( 0x02, 0 );      //������������ ������ � ������� � ������� �������
  LCD_cmd( 0x06, 0 );      //��������� ������, �������� �������� ������
  LCD_cmd( 0x0c, 0 );      //������� �������
  
  LCD_cmd( 0x40, 1 );
  
  while(1);
}
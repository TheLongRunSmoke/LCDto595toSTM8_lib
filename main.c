
#include "iostm8l152c6.h"

#define STRINGVECTOR(x) #x
#define ISR( a, b ) \
  _Pragma( STRINGVECTOR( vector = (b) ) ) \
  __interrupt void (a)( void )

    
void delay( void )      //задержка 15 тактов.
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

void delay_some( void )      //задержка.
{
  for (unsigned long delay_count=0; delay_count<500; delay_count++);
}

void Init( void )
{
  CLK_CKDIVR = 0x01;    //Устанавливаю пределитель 2, рабочая 8МГц
  
#define Data 0
#define Clock 2
#define Enable 4
  
  //PB_DDR = (1<<Data)|(1<<Clock)|(1<<Enable);
  PB_DDR_bit.DDR0 = 1;  //Data
  PB_DDR_bit.DDR2 = 1;  //Clock
  PB_DDR_bit.DDR4 = 1;  //Enable
  PB_CR1_bit.C10 = 1;
  PB_CR1_bit.C12 = 1;   //Переключаю в режим push-pull, чтобы получать лог. 1
  PB_CR1_bit.C14 = 1; 
  PB_ODR_bit.ODR0 = 0;
  PB_ODR_bit.ODR2 = 0;   //Сбрасываю выходы в 0
  PB_ODR_bit.ODR4 = 0; 
}

void shiftreg( int Cmd, int RS, int HorL )
{
  if( HorL == 1 )       //Помещаю в регистр верхний полубайт
    {
      int Mask = 128;          //Маска для проверки битов
      PB_ODR_bit.ODR0 = RS;     //Устанавливаю бит команды/данные
      PB_ODR_bit.ODR2 = 1;      //Задвигаю в регистр
      PB_ODR_bit.ODR2 = 0;      //Опускаю строб и защёлку
      for ( int i = 0; i < 4; i++)      //Пишу 4 байта, начиная со старшего
        {
          if ( Cmd & Mask )     //Если результат отличен от нуля
            {
              PB_ODR_bit.ODR0 = 1;      //Устанавливаю единицу на линии данных
            }
            else
            {
              PB_ODR_bit.ODR0 = 0;      //Иначе ноль
            }
        PB_ODR_bit.ODR2 = 1;      //Задвигаю в регистр
        PB_ODR_bit.ODR2 = 0;      //Опускаю строб и защёлку
        Mask = Mask / 2;          //Делю маску на 2 переходя к младшему биту
        }
      PB_ODR_bit.ODR0 = 0;    //Этот ноль не попадет на выход
      PB_ODR_bit.ODR2 = 1;    //Задвигаю в регистр ещё раз
      PB_ODR_bit.ODR2 = 0;    //Это надо потому, что строб и защелка объединены
    }
 if( HorL == 0 )       //Помещаю в регистр нижний полубайт
    {
      int Mask = 8;          //Маска для проверки битов
      PB_ODR_bit.ODR0 = RS;     //Устанавливаю бит команды/данные
      PB_ODR_bit.ODR2 = 1;      //Задвигаю в регистр
      PB_ODR_bit.ODR2 = 0;      //Опускаю строб и защёлку
      for ( int i = 0; i < 4; i++)      //Пишу 4 байта, начиная со старшего
        {
          if ( Cmd & Mask )     //Если результат отличен от нуля
            {
              PB_ODR_bit.ODR0 = 1;      //Устанавливаю единицу на линии данных
            }
            else
            {
              PB_ODR_bit.ODR0 = 0;      //Иначе ноль
            }
        PB_ODR_bit.ODR2 = 1;      //Задвигаю в регистр
        PB_ODR_bit.ODR2 = 0;      //Опускаю строб и защёлку
        Mask = Mask / 2;          //Делю маску на 2 переходя к младшему биту
        }
      PB_ODR_bit.ODR0 = 0;    //Этот ноль не попадет на выход
      PB_ODR_bit.ODR2 = 1;    //Задвигаю в регистр ещё раз
      PB_ODR_bit.ODR2 = 0;    //Это надо потому, что строб и защелка объединены
    }
}

void LCD_cmd( int Cmd, int RS )
{
  PB_ODR_bit.ODR4 = 1;          //Взвожу Enable дисплея
  shiftreg ( Cmd, RS, 1 );      //Пишу в регистр старший полубайт
  delay();                      //Ожидаю готовности
  PB_ODR_bit.ODR4 = 0;          //Скармливаю дисплею команду
  delay_some();                 //Ожидаю готовности
  PB_ODR_bit.ODR4 = 1;          //Взвожу Enable дисплея
  shiftreg ( Cmd, RS, 0 );      //Пишу в регистр младший полубайт
  delay();                      //Ожидаю готовности
  PB_ODR_bit.ODR4 = 0;          //Скармливаю дисплею команду
  delay_some();                 //Ожидаю готовности
}

int main( void )
{    
  Init();
  
  LCD_cmd( 0x20, 0 );      //Переключаю экран в 4-бит режим
  LCD_cmd( 0x28, 0 );      //Настраиваю колличество строк - 2
  LCD_cmd( 0x01, 0 );      //Чишу экран
  LCD_cmd( 0x02, 0 );      //Устанавливаю курсор и дисплей в нулевую позицию
  LCD_cmd( 0x06, 0 );      //Инкремент адреса, отключаю движение экрана
  LCD_cmd( 0x0c, 0 );      //Включаю дисплей
  
  LCD_cmd( 0x40, 1 );
  
  while(1);
}
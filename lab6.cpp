#include <dos.h>
#include <conio.h>
#include <stdio.h>

void interrupt NewInterrupt(...);
void interrupt(*oldInterrupt)(...);
void UpdateHighlightFlag(unsigned char);
void SetMask(unsigned char mask);
int commandIsExecuted;
int quitFlag;
int needHighlight;

void main()
{
	clrscr();
	commandIsExecuted = 0;
	quitFlag = 0;
	needHighlight = 0;
	oldInterrupt = getvect(9);
	setvect(9, NewInterrupt);
	while (!quitFlag)
	{
			if (needHighlight)
			{
				SetMask(0x04);                                      //Включение светодиода capslock
				delay(100);
     			SetMask(0x00);	
				delay(100);
     			SetMask(0x04);	
				delay(1000);
     			SetMask(0x00);		
				delay(5000);
				SetMask(0x04);
				delay(10);
				SetMask(0x00);	
				needHighlight = 0;
			}
	}
	setvect(9, oldInterrupt);
	return;
}
void interrupt NewInterrupt(...)
{
		unsigned char value = 0;
		oldInterrupt();
		value = inp(0x60);
		if (value == 0x01)                                    // Выход из программы по escape
			quitFlag = 1; 
		UpdateHighlightFlag(value);
		commandIsExecuted = commandIsExecuted || (needHighlight == 0) || (value == 0xFA);
		printf("%x\t", value);
		outp(0x20, 0x20);
}
void UpdateHighlightFlag(unsigned char value)
{
		if (value != 0x1c)                                   // Ожидание клавиши enter 
			return; 
		if (needHighlight)                        
				needHighlight = 0;
		else
				needHighlight = 1;                                // Разрешаем blinking
}
void SetMask(unsigned char mask)
{
	commandIsExecuted = 0;
		while (!commandIsExecuted)                            // Пока нет подтверждения успешного выполнения команды
		{
				while ((inp(0x64) & 0x02) != 0x00);               // Ожидаем освобождения входного буфера клавиатуры
				outp(0x60, 0xED);                                 // Записываем в порт команду управления индикаторами
				outp(0x60, mask);                                 // Записываем в порт битовую маску для настройки индикаторов
				delay(50);
		}
}

/*
 * constants.h
 *
 * Created: 17/08/2016 09:03:52
 *  Author: Tobias
 */ 


#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// VT100 Escape Sequences

#define USART_POS_IS_HEAT_ON						(char*) "\x1b[1;10f"
#define USART_TEXT_GETIO_HEAD						(char*) "           1   2   3   4   5   6  OVER\n\n"
#define USART_POS_GETIO_B1							(char*) "\x1b[2;12f"
#define USART_POS_GETIO_B2							(char*) "\x1b[2;16f"
#define USART_POS_GETIO_B3							(char*) "\x1b[2;20f"
#define USART_POS_GETIO_B4							(char*) "\x1b[2;24f"
#define USART_POS_GETIO_B5							(char*) "\x1b[2;28f"
#define USART_POS_GETIO_B6							(char*) "\x1b[2;32f"
#define USART_POS_GETIO_BOVER						(char*) "\x1b[2;36f"
#define USART_POS_GETIO_BGUN						(char*) "\x1b[8;15f"
#define USART_POS_GETIO_BRKT						(char*) "\x1b[8;20f"
#define USART_POS_GETIO_BBOM						(char*) "\x1b[8;25f"
#define USART_POS_GETIO_BO1							(char*) "\x1b[8;29f"
#define USART_POS_GETIO_BO2							(char*) "\x1b[8;33f"
#define USART_TEXT_GETIO_TAIL						(char*) "             GUN  RKT  BOM  O1  O2\n"
#define USART_TEXT_GETIO_JETT						(char*) "      JETT                          MAS\n"
#define USART_COLOR_BUTTON_PUSHED					UART_CMD_COLOR_GREEN_FOREGND
#define USART_COLOR_BUTTON_PULLED					UART_CMD_COLOR_RED_FOREGND
#define USART_COLOR_HEAD_TAIL						UART_CMD_COLOR_WHITE_FOREGND
#define USART_TEXT_GETIO_SW1						(char*) "  SW1                                   SW8\n"
#define USART_TEXT_GETIO_SW2						(char*) "  SW2                                   SW7\n"
#define USART_TEXT_GETIO_SW3						(char*) "  SW3                                   SW6\n"
#define USART_TEXT_GETIO_SW4						(char*) "  SW4                                   SW5\n"
#define USART_TEXT_GETIO_QUIT						(char*) "\n\npress any key to quit"
#define USART_POS_GETIO_SW1							(char*) "\x1b[3;10f"
#define USART_POS_GETIO_SW2							(char*) "\x1b[4;10f"
#define USART_POS_GETIO_SW3							(char*) "\x1b[5;10f"
#define USART_POS_GETIO_SW4							(char*) "\x1b[6;10f"
#define USART_POS_GETIO_SW5							(char*) "\x1b[6;38f"
#define USART_POS_GETIO_SW6							(char*) "\x1b[5;38f"
#define USART_POS_GETIO_SW7							(char*) "\x1b[4;38f"
#define USART_POS_GETIO_SW8							(char*) "\x1b[3;38f"
#define USART_POS_GETIO_JETT						(char*) "\x1b[9;9f"
#define USART_POS_GETIO_MAS							(char*) "\x1b[9;38f"
#define USART_TEXT_GETIO_EXTIO						(char*) "\n\n\nRETICLE+:\nRETICLE-:\nDAY/NIGHT:\nNVIS:\n"
#define USART_POS_GETIO_RETICLEP					(char*) "\x1b[12;14f"
#define USART_POS_GETIO_RETICLEM					(char*) "\x1b[13;14f"
#define USART_POS_GETIO_DAYNIGHT					(char*) "\x1b[14;14f"
#define USART_POS_GETIO_NVIS						(char*) "\x1b[15;14f"
#define USART_POS_GETADC1_CHAN0						(char*) "\x1b[1;12f"
#define USART_POS_GETADC1_CHAN1						(char*) "\x1b[2;12f"
#define USART_POS_GETADC1_CHAN2						(char*) "\x1b[3;12f"
#define USART_POS_GETADC1_CHAN3						(char*) "\x1b[4;12f"
#define USART_POS_GETADC1_CHAN4						(char*) "\x1b[5;12f"
#define USART_POS_GETADC1_CHAN5						(char*) "\x1b[6;12f"
#define USART_POS_GETADC1_CHAN6						(char*) "\x1b[7;12f"
#define USART_POS_GETADC1_CHAN7						(char*) "\x1b[8;12f"
#define USART_POS_GETADC1_CHAN8						(char*) "\x1b[9;12f"
#define USART_POS_GETADC1_CHAN9						(char*) "\x1b[10;12f"
#define USART_POS_GETADC1_CHAN10					(char*) "\x1b[11;12f"
#define USART_POS_GETADC1_CHAN11					(char*) "\x1b[12;12f"
#define USART_TEXT_GETLED_HEAD1						(char*) "           1   2   3   4   5   6  OVER\n"
#define USART_TEXT_GETLED_HEAD2						(char*) "                                            GA GB\n"
#define USART_TEXT_GETLED_HEAD3						(char*) "                                              Y\n"
#define USART_TEXT_GETLED_TAIL1						(char*) "             GUN  RKT  BOM  O1  O2\n"
#define USART_TEXT_GETLED_TAIL2						(char*) "      JETT                           MAS      Y\n"
#define USART_TEXT_GETLED_TAIL3						(char*) "                                            GA GB\n"
#define USART_POS_GETLED_GA1						(char*) "\x1b[2;11f"
#define USART_POS_GETLED_GB1						(char*) "\x1b[2;13f"
#define USART_POS_GETLED_Y1							(char*) "\x1b[3;12f"
#define USART_POS_GETLED_GA2						(char*) "\x1b[2;15f"
#define USART_POS_GETLED_GB2						(char*) "\x1b[2;17f"
#define USART_POS_GETLED_Y2							(char*) "\x1b[3;16f"
#define USART_POS_GETLED_GA3						(char*) "\x1b[2;19f"
#define USART_POS_GETLED_GB3						(char*) "\x1b[2;21f"
#define USART_POS_GETLED_Y3							(char*) "\x1b[3;20f"
#define USART_POS_GETLED_GA4						(char*) "\x1b[2;23f"
#define USART_POS_GETLED_GB4						(char*) "\x1b[2;25f"
#define USART_POS_GETLED_Y4							(char*) "\x1b[3;24f"
#define USART_POS_GETLED_GA5						(char*) "\x1b[2;27f"
#define USART_POS_GETLED_GB5						(char*) "\x1b[2;29f"
#define USART_POS_GETLED_Y5							(char*) "\x1b[3;28f"
#define USART_POS_GETLED_GA6						(char*) "\x1b[2;31f"
#define USART_POS_GETLED_GB6						(char*) "\x1b[2;33f"
#define USART_POS_GETLED_Y6							(char*) "\x1b[3;32f"
#define USART_POS_GETLED_GAOVER						(char*) "\x1b[2;35f"
#define USART_POS_GETLED_GBOVER						(char*) "\x1b[2;37f"
#define USART_POS_GETLED_YOVER						(char*) "\x1b[3;36f"
#define USART_POS_GETLED_GAGUN						(char*) "\x1b[10;14f"
#define USART_POS_GETLED_GBGUN						(char*) "\x1b[10;16f"
#define USART_POS_GETLED_YGUN						(char*) "\x1b[9;15f"
#define USART_POS_GETLED_GARKT						(char*) "\x1b[10;19f"
#define USART_POS_GETLED_GBRKT						(char*) "\x1b[10;21f"
#define USART_POS_GETLED_YRKT						(char*) "\x1b[9;20f"
#define USART_POS_GETLED_GABOM						(char*) "\x1b[10;24f"
#define USART_POS_GETLED_GBBOM						(char*) "\x1b[10;26f"
#define USART_POS_GETLED_YBOM						(char*) "\x1b[9;25f"
#define USART_POS_GETLED_GAO1						(char*) "\x1b[10;29f"
#define USART_POS_GETLED_GBO1						(char*) "\x1b[10;31f"
#define USART_POS_GETLED_YO1						(char*) "\x1b[9;30f"
#define USART_POS_GETLED_GAO2						(char*) "\x1b[10;33f"
#define USART_POS_GETLED_GBO2						(char*) "\x1b[10;35f"
#define USART_POS_GETLED_YO2						(char*) "\x1b[9;34f"
#define USART_TEXT_GET_TMP1							(char*) "Temperature 1:\n"
#define USART_TEXT_GET_TMP2							(char*) "Temperature 2:\n"
#define USART_POS_GET_TMP1							(char*) "\x1b[1;16f"
#define USART_POS_GET_TMP2							(char*) "\x1b[2;16f"
#define USART_TEXT_ADCINT_MENU						(char*) "/**** ADC values ****/\n"
#define USART_TEXT_ADCINT_LINE1						(char*) "Input Voltage:\n"
#define USART_TEXT_ADCINT_LINE2						(char*) "5V Dimming:\n"
#define USART_TEXT_ADCINT_LINE3						(char*) "28V Dimming:\n"
#define USART_TEXT_ADCINT_LINE4						(char*) "Backlight Current:\n"
#define USART_TEXT_ADCINT_LINE5						(char*) "LED Voltage:\n"
#define USART_POS_ADCINT_LINE1						(char*) "\x1b[2;17f"
#define USART_POS_ADCINT_LINE2						(char*) "\x1b[3;14f"
#define USART_POS_ADCINT_LINE3						(char*) "\x1b[4;15f"
#define USART_POS_ADCINT_LINE4						(char*) "\x1b[5;21f"
#define USART_POS_ADCINT_LINE5						(char*) "\x1b[6;15f"


#define UART_CMD_CLEAR_SCREEN						(char*) "\033[2J"
#define UART_CMD_CLEAR_LINE							(char*) "\033[K"
#define UART_CMD_POSITION_INIT						(char*) "\x1b[1;1f"
#define UART_CMD_POSITION_SAVE						(char*) "\x1b[s"
#define UART_CMD_POSITION_RETURN					(char*) "\x1b[u"
#define UART_CMD_COLOR_WHITE_FOREGND				(char*) "\x1b[37m"
#define UART_CMD_COLOR_RED_FOREGND					(char*) "\x1b[31m"
#define UART_CMD_COLOR_GREEN_FOREGND				(char*) "\x1b[32m"
#define UART_CMD_COLOR_YELLOW_FOREGND				(char*) "\x1b[33m"


    // VT100 Escape character: 0x1B
    //bcm2835_aux_muart_transfer(0x1B);
    // VT100 Escape equences explained here:
    // http://ascii-table.com/ansi-escape-sequences-vt-100.php
    // VT100 Escape Sequence: Clear screen: [2J
    //bcm2835_aux_muart_transfernb("[2J");
    //bcm2835_aux_muart_transfernb("\033[2J");

#endif /* CONSTANTS_H_ */

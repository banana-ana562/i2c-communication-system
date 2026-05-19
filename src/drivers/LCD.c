/*
 * LCD.c
 *
 *  Main Implementation of LCD functions such as initialization, sending
 *  commands and data, and basic functionalities
 *
 * Created on: July 26th, 2023
 *      Author: Jackie Huynh
 *
 * Spec Reference: CECS 447 Final Project - Module 6 (pages 7-8)
 * Hardware: 16x2 LCD with PCF8574T I2C backpack, address 0x27
 *
 * HOW PCF8574T BACKPACK WORKS:
 *   PCF8574T is an 8-bit I2C port expander wired to HD44780:
 *   P0=RS, P1=RW, P2=EN, P3=Backlight, P4-P7=DB4-DB7
 *
 *   HD44780 is in 4-bit mode so every byte is split into two nibbles.
 *   Each nibble needs an EN falling-edge pulse -> 2 bytes per nibble
 *   -> 4 bytes total per command or character.
 *
 *   4-byte sequence per nibble:
 *   [0] nibble | BACKLIGHT | EN_Pin  -> EN rising edge
 *   [1] nibble | BACKLIGHT           -> EN falling edge <- HD44780 latches
 */

#include "src/drivers/LCD.h"
#include "src/drivers/UART0.h"
#include <stdio.h>

extern char uart_buf[100]; // UART output buffer for sprintf formatting

/*
 *  -------------------LCD_Send_CMD------------------
 *  Local function: send one command byte to HD44780 (RS=0)
 *  Input: cmd - HD44780 command byte
 *  Output: None
 */
static void LCD_Send_CMD(uint8_t cmd){
    uint8_t upper = cmd & UPPER_NIBBLE_MSK;
    uint8_t lower = (cmd << NIBBLE_SHIFT) & UPPER_NIBBLE_MSK;

    // Upper nibble
    I2C3_Transmit(LCD_WRITE_ADDR, upper | BACKLIGHT | EN_Pin, upper | BACKLIGHT);
    DELAY_1MS(1);
    // Lower nibble
    I2C3_Transmit(LCD_WRITE_ADDR, lower | BACKLIGHT | EN_Pin, lower | BACKLIGHT);
    DELAY_1MS(1);
}

static void LCD_Send_Data(uint8_t data){
    uint8_t upper = data & UPPER_NIBBLE_MSK;
    uint8_t lower = (data << NIBBLE_SHIFT) & UPPER_NIBBLE_MSK;

    // Upper nibble
    I2C3_Transmit(LCD_WRITE_ADDR, upper | BACKLIGHT | EN_Pin | RS_Pin, upper | BACKLIGHT | RS_Pin);
    DELAY_1MS(1);
    // Lower nibble
    I2C3_Transmit(LCD_WRITE_ADDR, lower | BACKLIGHT | EN_Pin | RS_Pin, lower | BACKLIGHT | RS_Pin);
    DELAY_1MS(1);
}

/*
 *  -------------------LCD_Init------------------
 *  HD44780 4-bit power-on initialization sequence.
 *  Spec Module 6 page 7 startup sequence:
 *    1. Display first name on Row 1
 *    2. Wait 1 second
 *    3. Display last name on Row 2
 *    4. Wait 1 second
 *    5. Clear display
 *    6. Repeat (handled in main loop)
 *  Input: None
 *  Output: None
 */
void LCD_Init(void) {
    DELAY_1MS(50);
    UART0_OutString("Starting LCD Init\r\n");

    I2C3_Transmit(LCD_WRITE_ADDR, INIT_REG_CMD | BACKLIGHT | EN_Pin, INIT_REG_CMD | BACKLIGHT);
    DELAY_1MS(5);
    sprintf(uart_buf, "Wake1 MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    I2C3_Transmit(LCD_WRITE_ADDR, INIT_REG_CMD | BACKLIGHT | EN_Pin, INIT_REG_CMD | BACKLIGHT);
    DELAY_1MS(1);
    sprintf(uart_buf, "Wake2 MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    I2C3_Transmit(LCD_WRITE_ADDR, INIT_REG_CMD | BACKLIGHT | EN_Pin, INIT_REG_CMD | BACKLIGHT);
    DELAY_1MS(1);
    sprintf(uart_buf, "Wake3 MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    I2C3_Transmit(LCD_WRITE_ADDR, INIT_FUNC_CMD | BACKLIGHT | EN_Pin, INIT_FUNC_CMD | BACKLIGHT);
    DELAY_1MS(1);
    sprintf(uart_buf, "4bit MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    LCD_Send_CMD(FUNC_MODE | FUNC_4_BIT | FUNC_2_ROW | FUNC_5_7);
    DELAY_1MS(1);
    sprintf(uart_buf, "FuncSet MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    LCD_Send_CMD(DISP_CMD | DISP_OFF | DISP_CURSOR_OFF | DISP_BLINK_OFF);
    DELAY_1MS(1);
    sprintf(uart_buf, "DispOff MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    LCD_Send_CMD(CLEAR_DISP_CMD);
    DELAY_1MS(2);
    sprintf(uart_buf, "Clear MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    LCD_Send_CMD(ENTRY_MODE_CMD | ENTRY_INC_CURSOR);
    DELAY_1MS(1);
    sprintf(uart_buf, "Entry MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    LCD_Send_CMD(DISP_CMD | DISP_ON | DISP_CURSOR_ON | DISP_BLINK_ON);
    DELAY_1MS(1);
    sprintf(uart_buf, "DispOn MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);

    LCD_Set_Cursor(ROW1, 0);
    sprintf(uart_buf, "SetCursor MCS: 0x%02lX\r\n", I2C3_MCS_R); UART0_OutString(uart_buf);
}

/*
 *  -------------------LCD_Clear------------------
 *  Clear LCD display and return cursor to home.
 *  0x01 command takes ~1.52ms ? always delay after.
 *  Input: None
 *  Output: None
 */
void LCD_Clear(void){
    LCD_Send_CMD(CLEAR_DISP_CMD);
    DELAY_1MS(2);
}

/*
 *  ----------------LCD_Set_Cursor----------------
 *  Move cursor to desired row and column.
 *  Row 1: DDRAM 0x00-0x0F -> 0x80 + col
 *  Row 2: DDRAM 0x40-0x4F -> 0xC0 + col
 *  Input: row (ROW1=0 or ROW2=1), col (0-15)
 *  Output: None
 */
void LCD_Set_Cursor(uint8_t row, uint8_t col){
    if(row == ROW1){
        LCD_Send_CMD(FIRST_ROW_CMD + col);   // 0x80 + col
    } else {
        LCD_Send_CMD(SECOND_ROW_CMD + col);  // 0xC0 + col
    }
}

/*
 *  ---------------LCD_Reset_Cursor---------------
 *  Return cursor to Row 1, Column 0.
 *  RETURN_HOME_CMD (0x02) takes ~1.52ms ? always delay after.
 *  Input: None
 *  Output: None
 */
void LCD_Reset_Cursor(void){
    LCD_Send_CMD(RETURN_HOME_CMD);
    DELAY_1MS(2);
}

/*
 *  ----------------LCD_Print_Char----------------
 *  Print single ASCII character at current cursor position.
 *  Cursor auto-advances right after each character.
 *  Input: data - ASCII value
 *  Output: None
 */
void LCD_Print_Char(uint8_t data){
    LCD_Send_Data(data);
}

/*
 *  ----------------LCD_Print_Str-----------------
 *  Print null-terminated string at current cursor position.
 *  Spec output examples (Module 6, pages 7-8):
 *    LCD_Set_Cursor(ROW1, 0); LCD_Print_Str((uint8_t*)"Color: RED");
 *    LCD_Set_Cursor(ROW2, 0); LCD_Print_Str((uint8_t*)"Angle: 45");
 *  Input: str - pointer to null-terminated uint8_t array
 *  Output: None
 */
void LCD_Print_Str(uint8_t* str){
    while(*str != '\0'){
        LCD_Send_Data(*str);
        str++;
    }
}
/*
 * LCD.h
 *
 *  Provides LCD functions such as initialization, sending
 *  commands and data, and basic functionalities
 *
 * Created on: July 26th, 2023
 *      Author: Jackie Huynh
 *
 */

#ifndef LCD_H_
#define LCD_H_
#include "src/tm4c123gh6pm.h"
#include "src/drivers/wtimer.h"
#include "src/drivers/I2C3.h"

/*************PCF8574T Register*************/
/* Spec page 2: "16x2 LCD Display — I2C backpack (PCF8574T), address 0x27" */
#define LCD_WRITE_ADDR      (0x27U)     // PCF8574T variant — spec-mandated address
#define PCF8574A_REG        (0x00U)     // No internal sub-register; data goes straight to port

/**************LCD CMD Register*************/
/* HD44780 Power-On Init sequence magic bytes */
#define INIT_REG_CMD        (0x30U)     // 8-bit mode wake-up pulse (sent 3x on power-on)
#define INIT_FUNC_CMD       (0x20U)     // Switch to 4-bit mode (upper nibble only)

/* Function Set (0x20 base) — combine with bits below */
#define FUNC_MODE           (0x20U)     // Base: Function Set command
#define FUNC_4_BIT          (0x00U)     // DL=0 ? 4-bit bus
#define FUNC_2_ROW          (0x08U)     // N=1  ? 2 display lines
#define FUNC_5_7            (0x00U)     // F=0  ? 5×8 dot font

/* Display Control (0x08 base) — combine bits below */
// we need to fix these macros 
#define DISP_CMD            (0x08U)     // Base: Display Control command
#define DISP_ON             (0x04U)     // D=1 ? display on
#define DISP_OFF            (0x00U)     // D=0 ? display off
#define DISP_CURSOR_ON      (0x02U)     // C=1 ? cursor visible
#define DISP_CURSOR_OFF     (0x00U)     // C=0 ? cursor hidden
#define DISP_BLINK_ON       (0x01U)     // B=1 ? cursor blink on
#define DISP_BLINK_OFF      (0x00U)     // B=0 ? cursor blink off

/* Standalone Commands */
#define CLEAR_DISP_CMD      (0x01U)     // Clear display + return home (needs 2 ms)
#define ENTRY_MODE_CMD      (0x04U)     // Base: Entry Mode Set command
#define ENTRY_INC_CURSOR    (0x02U)     // I/D=1 ? cursor increments right after each char
//  #define ENTRY_DISP_SHIFT (0x01U)    // S=1  ? shift entire display (unused)

#define RETURN_HOME_CMD     (0x02U)     // Return cursor to home (needs 2 ms)

/* DDRAM Address Commands — spec page 7: Row 1 = 0x80+col, Row 2 = 0xC0+col */
#define FIRST_ROW_CMD       (0x80U)     // DDRAM address 0x00 | set-DDRAM-address bit
#define SECOND_ROW_CMD      (0xC0U)     // DDRAM address 0x40 | set-DDRAM-address bit

/* PCF8574T Pin Mapping (port expander P0–P3 wired to HD44780 control lines)
 *   P0 ? RS   (Register Select: 0=command, 1=data)
 *   P1 ? RW   (Read/Write:      0=write,   1=read  — always write so tie low)
 *   P2 ? EN   (Enable:          falling edge latches data)
 *   P3 ? Backlight transistor base
 */
#define RS_Pin              (0x01U)     // P0
#define RW_Pin              (0x02U)     // P1
#define EN_Pin              (0x04U)     // P2
#define BACKLIGHT           (0x08U)     // P3

/* Nibble Manipulation */
#define UPPER_NIBBLE_MSK    (0xF0U)     // Mask to isolate bits [7:4]
#define NIBBLE_SHIFT        (4U)        // Shift count to move lower nibble ? upper nibble

/* Row / Size Helpers */
#define ROW1                (0U)        // First  row index
#define ROW2                (1U)        // Second row index
#define LCD_ROW_SIZE        (16U)       // Characters per row

#include <stdint.h>


/*
 *  -------------------LCD_Init------------------
 *  Basic LCD Initialization Function
 *  Follows HD44780 4-bit power-on sequence then displays
 *  startup name sequence per spec (Module 6, page 7)
 *  Input: None
 *  Output: None
 */
void LCD_Init(void);

/*
 *  -------------------LCD_Clear------------------
 *  Clear the LCD Display by passing a command
 *  Input: None
 *  Output: None
 */
void LCD_Clear(void);

/*
 *  ----------------LCD_Set_Cursor----------------
 *  Set Cursor to Desire Place
 *  Input: Desired Row and Column to place Cursor
 *  Output: None
 */
void LCD_Set_Cursor(uint8_t row, uint8_t col);

/*
 *  ---------------LCD_Reset_Cursor---------------
 *  Reset Cursor back to Row 1 and Column 0
 *  Input: None
 *  Output: None
 */
void LCD_Reset_Cursor(void);

/*
 *  ----------------LCD_Print_Char----------------
 *  Prints a Character to LCD
 *  Input: Character Hex Value
 *  Output: None
 */
void LCD_Print_Char(uint8_t data);

/*
 *  ----------------LCD_Print_Str-----------------
 *  Prints a string to LCD
 *  Input: Pointer to Character Array
 *  Output: None
 */
void LCD_Print_Str(uint8_t* str);

#endif
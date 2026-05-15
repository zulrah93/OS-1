#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <io.h>
#include <stdint.h>

// -- Interfaces and defintions --

#define KEYBOARD_ENCODER_PORT 0x60
#define KEYBOARD_CONTROLLER_PORT 0x64
#define KEYBOARD_SELF_TEST_ACK 0x55

/* scan_to_ascii: index = scan code (0x00..0x7F), value = ASCII char or 0 if none.
   Uses PS/2/XT Set 1 scan codes for standard US keys (unshifted).
*/
const char SCAN_CODES[128] = {
    /* 0x00 */ 0,
    /* 0x01 */ 0,    /* Esc */
    /* 0x02 */ '1',
    /* 0x03 */ '2',
    /* 0x04 */ '3',
    /* 0x05 */ '4',
    /* 0x06 */ '5',
    /* 0x07 */ '6',
    /* 0x08 */ '7',
    /* 0x09 */ '8',
    /* 0x0A */ '9',
    /* 0x0B */ '0',
    /* 0x0C */ '-',
    /* 0x0D */ '=',
    /* 0x0E */ '\b', /* Backspace */
    /* 0x0F */ '\t', /* Tab */
    /* 0x10 */ 'q',
    /* 0x11 */ 'w',
    /* 0x12 */ 'e',
    /* 0x13 */ 'r',
    /* 0x14 */ 't',
    /* 0x15 */ 'y',
    /* 0x16 */ 'u',
    /* 0x17 */ 'i',
    /* 0x18 */ 'o',
    /* 0x19 */ 'p',
    /* 0x1A */ '[',
    /* 0x1B */ ']',
    /* 0x1C */ '\n', /* Enter */
    /* 0x1D */ 0,    /* Left Ctrl */
    /* 0x1E */ 'a',
    /* 0x1F */ 's',
    /* 0x20 */ 'd',
    /* 0x21 */ 'f',
    /* 0x22 */ 'g',
    /* 0x23 */ 'h',
    /* 0x24 */ 'j',
    /* 0x25 */ 'k',
    /* 0x26 */ 'l',
    /* 0x27 */ ';',
    /* 0x28 */ '\'',
    /* 0x29 */ '`',
    /* 0x2A */ 0,    /* Left Shift */
    /* 0x2B */ '\\',
    /* 0x2C */ 'z',
    /* 0x2D */ 'x',
    /* 0x2E */ 'c',
    /* 0x2F */ 'v',
    /* 0x30 */ 'b',
    /* 0x31 */ 'n',
    /* 0x32 */ 'm',
    /* 0x33 */ ',',
    /* 0x34 */ '.',
    /* 0x35 */ '/',
    /* 0x36 */ 0,    /* Right Shift */
    /* 0x37 */ '*',  /* Keypad * */
    /* 0x38 */ 0,    /* Left Alt */
    /* 0x39 */ ' ',  /* Space */
    /* 0x3A */ 0,    /* Caps Lock */
    /* 0x3B */ 0,    /* F1 */
    /* 0x3C */ 0,    /* F2 */
    /* 0x3D */ 0,    /* F3 */
    /* 0x3E */ 0,    /* F4 */
    /* 0x3F */ 0,    /* F5 */
    /* 0x40 */ 0,    /* F6 */
    /* 0x41 */ 0,    /* F7 */
    /* 0x42 */ 0,    /* F8 */
    /* 0x43 */ 0,    /* F9 */
    /* 0x44 */ 0,    /* F10 */
    /* 0x45 */ 0,    /* Num Lock */
    /* 0x46 */ 0,    /* Scroll Lock */
    /* 0x47 */ '7',  /* Keypad 7 / Home */
    /* 0x48 */ '8',  /* Keypad 8 / Up */
    /* 0x49 */ '9',  /* Keypad 9 / PgUp */
    /* 0x4A */ '-',
    /* 0x4B */ '4',  /* Keypad 4 / Left */
    /* 0x4C */ '5',  /* Keypad 5 */
    /* 0x4D */ '6',  /* Keypad 6 / Right */
    /* 0x4E */ '+',
    /* 0x4F */ '1',  /* Keypad 1 / End */
    /* 0x50 */ '2',  /* Keypad 2 / Down */
    /* 0x51 */ '3',  /* Keypad 3 / PgDn */
    /* 0x52 */ '0',  /* Keypad 0 / Ins */
    /* 0x53 */ '.',  /* Keypad . / Del */
    /* 0x54 */ 0,
    /* 0x55 */ 0,
    /* 0x56 */ 0,
    /* 0x57 */ 0,    /* F11 */
    /* 0x58 */ 0,    /* F12 */
    /* 0x59..0x7F */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

enum KEYBOARD_CONTROLLER_STATUS_MASK {
 
	OUT_BUFFER_STATUS	=	1,		//00000001
	IN_BUFFER_STATUS	=	2,		//00000010
	SYSTEM_STATUS	=	4,		//00000100
	DATA_STATUS	=	8,		//00001000
	LOCKED_STATUS	=	0x10,		//00010000
	AUX_BUFFER_STATUS	=	0x20,		//00100000
	TIMEOUT_STATUS	=	0x40,		//01000000
	PARITY_STATUS	=	0x80		//10000000
};

enum KEYBOARD_ENCODER_COMMANDS {
    ENCODER_SET_LEDS = 0xED,
    ENCODER_ECHO = 0xEE,
    ENCODER_ALTERNATE_SCAN_CODE_SET = 0xF0,
    ENCODER_SEND_TWO_BYTE_KID = 0xF2,
    ENCODER_SEND_AUTOREPEAT_DELAY_AND_REPEAT_RATE = 0xF3,
    ENCODER_ENABLE_KEYBOARD = 0xF4,
    ENCODER_RESET_AND_WAITS_FOR_ENABLE = 0xF5,
    ENCODER_RESET_AND_BEGIN_SCANNING = 0xF6,
    ENCODER_SET_ALL_KEYS_TO_AUTOREPEAT_AND_GENERATE_BREAK_CODES = 0xFA,
    ENCODER_SET_A_SINGLE_KEY_TO_AUTOREPEAT = 0xFB,
    ENCODER_SET_A_SINGLE_KEY_TO_GENERATE_BREAK_CODES = 0xFC,
    ENCODER_SET_A_SINGLE_KEY_ONLY_TO_GENERATE_BREAK_CODES = 0xFD,
    ENCODER_RESEND_LAST_RESULT = 0xFE,
    ENCODER_RESET = 0xFF
};

enum KEYBOARD_CONTROLLER_COMMANDS {
    READ_COMMAND = 0x20,
    WRITE_COMMAND = 0x60,
    CONTROLLER_SELF_TEST = 0xAA,
    INTERFACE_TEST = 0xAB,
    KC_DISABLE = 0xAD,
    KC_ENABLE = 0xAE,
    READ_INPUT = 0xC0,
    READ_OUTPUT = 0xD0,
    WRITE_OUTPUT = 0xD1,
    READ_TEST_INPUTS = 0xE0,
    SYSTEM_RESET = 0xFE
};

typedef struct {
    bool number_lock; // if true light it up else don't same as below
    bool caps_lock;
    bool scroll_lock;
} keyboard_led_config_t;

// -- FUNCTIONS --

uint8_t keyboard_poll_controller_status() {
    return inb(KEYBOARD_CONTROLLER_PORT);
}

uint8_t keyboard_poll_encoder_status() {
    return inb(KEYBOARD_ENCODER_PORT);
}

void block_till_controller_io_in_buf_ready() {
    while(0 != (keyboard_poll_controller_status() & IN_BUFFER_STATUS)) {
    }
}

void block_till_controller_io_out_buf_ready() {
    while(!(keyboard_poll_controller_status() & OUT_BUFFER_STATUS)) {
    }
}

void keyboard_set_led_config(const keyboard_led_config_t* config) {
    if (NULL == config) {
        return;
    }

    uint8_t led_data_encoded = (config->scroll_lock ? 1 : 0) | (config->number_lock ? (1 << 1) : 0) | (config->caps_lock ? (1 << 2) : 0);
    
    block_till_controller_io_in_buf_ready();
    outb(ENCODER_SET_LEDS, KEYBOARD_ENCODER_PORT);
    block_till_controller_io_in_buf_ready();
    outb(led_data_encoded, KEYBOARD_ENCODER_PORT);
}

void keyboard_controller_disable() {
    outb(KC_DISABLE, KEYBOARD_CONTROLLER_PORT);
}

void keyboard_controller_enable() {
    block_till_controller_io_in_buf_ready();
    outb(KC_ENABLE, KEYBOARD_CONTROLLER_PORT);
}

void keyboard_encoder_enable() {
    outb(ENCODER_ENABLE_KEYBOARD, KEYBOARD_ENCODER_PORT);
}

bool has_keyboard_controller_passed_self_test() {
    outb(CONTROLLER_SELF_TEST, KEYBOARD_CONTROLLER_PORT);
    block_till_controller_io_out_buf_ready();
    return keyboard_poll_encoder_status() & KEYBOARD_SELF_TEST_ACK;
}

uint8_t poll_scan_code() {
    //block_till_controller_io_in_buf_ready();
    outb(READ_OUTPUT, KEYBOARD_CONTROLLER_PORT);
    block_till_controller_io_out_buf_ready();
    return inb(KEYBOARD_ENCODER_PORT);

}

#endif
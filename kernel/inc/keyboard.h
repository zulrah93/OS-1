#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <io.h>
#include <stdint.h>

// -- Interfaces and defintions --

#define KEYBOARD_ENCODER_PORT 0x60
#define KEYBOARD_CONTROLLER_PORT 0x64

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

struct keyboard_led_config_t {
    bool number_lock; // if true light it up else don't same as below
    bool caps_lock;
    bool scroll_lock;
};

// -- FUNCTIONS --

uint8_t keyboard_poll_status() {
    return inb(KEYBOARD_CONTROLLER_PORT);
}

inline void block_till_io_ready() {
    while(true) {
        if (0 == (keyboard_poll_status() & IN_BUFFER_STATUS)) {
            return;
        }
    }
    return;
}

void keyboard_set_led_config(const struct keyboard_led_config_t* config) {
    if (NULL == config) {
        return;
    }

    uint8_t led_data_encoded = (config->scroll_lock ? 1 : 0) | (config->number_lock ? (1 << 1) : 0) | (config->caps_lock ? (1 << 2) : 0);
    
    block_till_io_ready();
    outb(KEYBOARD_ENCODER_PORT, ENCODER_SET_LEDS);
    block_till_io_ready();
    outb(KEYBOARD_ENCODER_PORT, led_data_encoded);
}

void keyboard_controller_disable() {
    outb(KC_DISABLE, KEYBOARD_CONTROLLER_PORT);
}

void keyboard_controller_enable() {
    outb(KC_ENABLE, KEYBOARD_CONTROLLER_PORT);
}

bool has_keyboard_controller_passed_self_test() {
    outb(CONTROLLER_SELF_TEST, KEYBOARD_CONTROLLER_PORT);
    return false;
}

#endif
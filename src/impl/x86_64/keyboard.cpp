#include "keyboard.hpp"

extern "C" void keyboad_handler_isr() {
    char keycode;

    static uint8_t escape = 0;
    static uint8_t ctrl = 0;
    static uint8_t shift = 0;
    static uint8_t alt = 0;

    keycode = inb(0x60);

    if(keycode >= 0) {
        io::my_cout << keyboard_driver::get_ascii_upper(keycode);
    }

    outb(0x20, 0x20);
}

void keyboard_driver::keyboard_init() {
    uint8_t curent_mask_master = inb(0x21);
    outb(0x21, curent_mask_master & 0xFD);
    io::my_cout << "Initialized Keyboard" << io::OSTREAM_APPEND::endl;
}

char keyboard_driver::get_ascii_lower(uint8_t keycode){

}

char keyboard_driver::get_ascii_upper(uint8_t keycode){
    //io::my_cout << "got Keycode Upper: " << keycode << "\n"; 
    switch(keycode) {
        case SCAN_CODE_KEY_A: return 'A';
        case SCAN_CODE_KEY_B: return 'B';
        case SCAN_CODE_KEY_C: return 'C';
        case SCAN_CODE_KEY_D: return 'D';
        case SCAN_CODE_KEY_E: return 'E';
        case SCAN_CODE_KEY_F: return 'F';
        case SCAN_CODE_KEY_G: return 'G';
        case SCAN_CODE_KEY_H: return 'H';
        case SCAN_CODE_KEY_I: return 'I';
        case SCAN_CODE_KEY_J: return 'J';
        case SCAN_CODE_KEY_K: return 'K';
        case SCAN_CODE_KEY_L: return 'L';
        case SCAN_CODE_KEY_M: return 'M';
        case SCAN_CODE_KEY_N: return 'N';
        case SCAN_CODE_KEY_O: return 'O';
        case SCAN_CODE_KEY_P: return 'P';
        case SCAN_CODE_KEY_Q: return 'Q';
        case SCAN_CODE_KEY_R: return 'R';
        case SCAN_CODE_KEY_S: return 'S';
        case SCAN_CODE_KEY_T: return 'T';
        case SCAN_CODE_KEY_U: return 'U';
        case SCAN_CODE_KEY_V: return 'V';
        case SCAN_CODE_KEY_W: return 'W';
        case SCAN_CODE_KEY_X: return 'X';
        case SCAN_CODE_KEY_Y: return 'Y';
        case SCAN_CODE_KEY_Z: return 'Z';
        default: return '\0';  // Return null character if scan code doesn't correspond to a letter
    }
}
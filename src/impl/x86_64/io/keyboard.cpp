#include "keyboard.hpp"

extern "C" void keyboad_handler_isr() {
    char keycode = inb(KEYBOARD_DATA_PORT);

    // The Current Modfiers
    static uint8_t shift = 0;
    static uint8_t ctrl = 0;
    static uint8_t alt = 0;
    static uint8_t escape = 0;

    uint8_t modifiers = 0;

    modifiers = modifiers | (shift << modifier_offset::MODIFIER_SHIFT) | (ctrl << modifier_offset::MODIFIER_CTRL) | (alt << modifier_offset::MODIFIER_ALT) | (escape << modifier_offset::MODIFIER_ESC);

    // Is the current Keycode from a Keyevent beeing released or from it beeing pressed
    //io::my_cout << "Released? " << bool(keycode & SCAN_CODE_RELEASE) << "\n";

    //The Key has been Released -> Good for Resetting Modifiers
    if(keycode & SCAN_CODE_RELEASE) {
        char released_keyCode = uint8_t(keycode & ~(0x00 | SCAN_CODE_RELEASE));
        switch(released_keyCode) {
            case SCAN_CODE_KEY_LEFT_SHIFT:
            case SCAN_CODE_KEY_RIGHT_SHIFT:
                shift = 0;
                break;
            
        }
    } else if(keycode == SCAN_CODE_KEY_ENTER) {
        io::my_cout << '\n';
        io::my_cin << '\n';
    } else if(keycode == SCAN_CODE_KEY_TAB) {
        io::my_cin << '\t';
    } else if(keycode == SCAN_CODE_KEY_LEFT_SHIFT || keycode == SCAN_CODE_KEY_RIGHT_SHIFT) {
        shift = 1;
    } else {
        // By this Point only the acutall Letters should have been caught
        char keycode_letter;
        //if(shift) {
        //    keycode_letter = keyboard_driver::get_ascii_upper(keycode);
        //} else {
        //    keycode_letter = keyboard_driver::get_ascii_lower(keycode);
        //}
        keycode_letter = keyboard_driver::keycode2ascii(keycode, modifiers);
        if(!my_keyboard_driver.get_silent()) io::my_cout << keycode_letter;
        io::my_cin << keycode_letter;
    }

    // Clear the Pending Interrupt
    outb(0x20, 0x20);
}

bool keyboard_driver::silent_mode = false;

void keyboard_driver::keyboard_init() {
    uint8_t curent_mask_master = inb(0x21);
    outb(0x21, curent_mask_master & 0xFD);
    io::my_cout << "Initialized Keyboard" << io::OSTREAM_APPEND::endl;
}

// Getters and Setters for the Silent Variable
bool keyboard_driver::get_silent() const {
    return this->silent_mode;
}

void keyboard_driver::set_silent(bool silent) {
    this->silent_mode = silent;
}


char keyboard_driver::keycode2ascii(uint8_t keycode, uint8_t modifiers) {
    // Decode the Modifiers
    bool shift = bool(modifiers);

    // Look up the Characters
    switch(keycode) {
        case SCAN_CODE_KEY_A: return 'a' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_B: return 'b' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_C: return 'c' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_D: return 'd' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_E: return 'e' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_F: return 'f' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_G: return 'g' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_H: return 'h' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_I: return 'i' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_J: return 'j' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_K: return 'k' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_L: return 'l' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_M: return 'm' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_N: return 'n' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_O: return 'o' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_P: return 'p' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_Q: return 'q' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_R: return 'r' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_S: return 's' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_T: return 't' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_U: return 'u' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_V: return 'v' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_W: return 'w' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_X: return 'x' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_Y: return 'y' - keyboard_driver::uppercase_dif * shift;
        case SCAN_CODE_KEY_Z: return 'z' - keyboard_driver::uppercase_dif * shift;
        default: return '\0';  // Return null character if scan code doesn't correspond to a letter
    }
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
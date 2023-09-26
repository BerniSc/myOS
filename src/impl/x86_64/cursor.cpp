#include "cursor.hpp"

void CursorController::disableCursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void CursorController::enableCursor(const uint8_t cursor_start, const uint8_t cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void CursorController::updateCursorPosition(const uint8_t x, const uint8_t y) {
    uint16_t pos = y * constants::NUM_COLS_VGA + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, uint8_t(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, uint8_t((pos >> 8) & 0xFF));
}

uint16_t CursorController::getCursorPosition() {
    uint16_t pos = 0;
    
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    
    return pos;
}

uint8_t  CursorController::getCursorX(const uint16_t cursorPos) {
    return (cursorPos % constants::NUM_COLS_VGA); 
}

uint8_t  CursorController::getCursorY(const uint16_t cursorPos) {
    return (cursorPos / constants::NUM_COLS_VGA);
}
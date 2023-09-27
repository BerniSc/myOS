#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#include <inttypes.h>

#include "assembly_inline.hpp"

// TODO Move to config?
#define INTERRUPT_DATA_TABLE_SIZE 256

// Forward Declaration of Function TODO Check
extern "C" void keyboard_handler_interrupt();
extern "C" void load_interrupt_data_table(void*);

struct interrupt_data_table_entry {
    uint16_t offset_lowerbits;
    uint16_t selector;
    // TODO Char?
    uint8_t zero;
    // TODO Char?
    uint8_t flags;
    uint16_t offset_higherbits;
} __attribute__((packed));

struct interrupt_data_table_pointer {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Global Data
static interrupt_data_table_entry interrupt_data_table[INTERRUPT_DATA_TABLE_SIZE];
static interrupt_data_table_pointer interrupt_data_table_ptr;

// TODO Comment
void load_idt_entry(uint8_t const isr_number, uint64_t /*ulong*/ const base, uint16_t const selector, uint8_t const flags);
//
static void initialize_itd_pointer();
// Initialize the Programmable Interrupt Controller to accept/process Interrupts from i.e. the Keyboard
static void initialize_pic();

void init_interrupt_data_table();

#endif
#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#include <inttypes.h>

#include "assembly_inline.hpp"
#include "utils.hpp"
#include "print.hpp"
#include "io.hpp"

// TODO Move to config?
#define INTERRUPT_DATA_TABLE_SIZE   256
#define PIC_MASTER_CMD_PORT         0x20
#define PIC_MASTER_DATA_PORT        0x21
#define PIC_SLAVE_CMD_PORT          0xA0
#define PIC_SLAVE_DATA_PORT         0xA1

// Forward Declaration of Function TODO Check
extern "C" void keyboard_handler_interrupt();
extern "C" void load_interrupt_data_table(void*);

struct interrupt_controller {
    private: 
        /*************************************************************************************/
        // Internaly used C-Type Structs for bundeling the IDT and its Entries 
        struct interrupt_data_table_entry {
            uint16_t offset_lowerbits;
            uint16_t selector;
            // TODO Char?
            uint8_t always_zero_1;
            // TODO Char?
            uint8_t flags;
            uint16_t offset_midbits;
            uint32_t offset_higherbits;
            uint32_t always_zero_2;
        } __attribute__((packed));

        struct interrupt_data_table_pointer {
            uint16_t limit;
            uint64_t base;
        } __attribute__((packed));

        /*************************************************************************************/
        // Data
        interrupt_data_table_entry interrupt_data_table[INTERRUPT_DATA_TABLE_SIZE];
        interrupt_data_table_pointer interrupt_data_table_ptr;
    public: 
        interrupt_controller();
        // TODO Comment
        void load_idt_entry(uint8_t const isr_number, uint64_t const base, uint16_t const selector, uint8_t const flags);
        //
        void initialize_itd_pointer();
        // Initialize the Programmable Interrupt Controller to accept/process Interrupts from i.e. the Keyboard
        void initialize_pic();

        void init_interrupt_data_table();

        void load_idt_cpp();

        void enable_interrupts() const;

        void disable_interrupts() const;
};

extern interrupt_controller my_interrupt_ctl;

#endif
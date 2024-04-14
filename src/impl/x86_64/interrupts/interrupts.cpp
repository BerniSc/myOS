#include "interrupts.hpp"

interrupt_controller::interrupt_controller() {

}

void interrupt_controller::load_idt_entry(uint8_t const isr_number, unsigned long /*ulong*/ const base, uint16_t const selector, uint8_t const flags) {
    //print_str("Loaded IDT Entry with the Number: ");
    //char buffer[3];
    //itoa(isr_number, buffer);
    //print_str(buffer);
    //print_str("\n");
    io::my_cout << "Loaded IDT Entry with the Number " << isr_number << "\n";
    interrupt_data_table[isr_number].offset_lowerbits = (base & 0xFFFF);
    interrupt_data_table[isr_number].offset_midbits = ((base >> 16) & 0xFFFF);
    interrupt_data_table[isr_number].offset_higherbits = ((base >> 32) & 0xFFFFFFFF);
    interrupt_data_table[isr_number].selector = selector;
    interrupt_data_table[isr_number].flags = flags;
    interrupt_data_table[isr_number].always_zero_1 = 0;
    interrupt_data_table[isr_number].always_zero_2 = 0;
}

void interrupt_controller::initialize_itd_pointer() {
    // Set to the maximum Address of the IDT 
    interrupt_data_table_ptr.limit = (sizeof(interrupt_data_table_entry) * INTERRUPT_DATA_TABLE_SIZE) - 1;
    // Set to the base of the Interrupt Data Table
    interrupt_data_table_ptr.base = (uint64_t) &interrupt_data_table;
}   

void interrupt_controller::initialize_pic() {
    /*
     *  Send Command Word ICW1 --> Beginn Initialization
     *  Send 0x11 to PIC_1_Controll on 0x20 and to PIC_2_Controll (0xA0)
     */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    /*
     *  Send Command Word ICW2 --> Remap the offset address of IDT
     *  
     *  In x86 protected Mode, we have to remap the PIC's beyond 0x20 as Intel has designated
     *  the first 32 Interruüts as "reserverd" for CPU exceptions
     */
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    /*
     *  Send Command Word ICW3 --> Setup Cascading
     *  Sets up the Connection to the Slave for IRQ2
     */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    /*
     *  Send Command Word ICW4 --> Setup Environment Information
     */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /*
     *  Mask all the Interrupts
     */
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void interrupt_controller::init_interrupt_data_table() {
    initialize_pic();
    initialize_itd_pointer();
    //load_interrupt_data_table(&interrupt_data_table_ptr);
    this->load_idt_cpp();
}

void interrupt_controller::load_idt_cpp() {
    // Load the IDT in inline assembly.
    asm volatile(
        "lidt %0"
        :
        : "m" (interrupt_data_table_ptr)
    );
}

void interrupt_controller::enable_interrupts() const {
    asm volatile(
        "sti"
    );
}

void interrupt_controller::disable_interrupts() const {
    asm volatile(
        "cli"
    );
}
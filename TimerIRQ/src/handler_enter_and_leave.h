// Macros to manage entry and exit from interrupt handlers.
// These macros stack and un-stack all registers on entry and exit
// Functions using these macros have to be declared as "naked" as follows
/*
    extern "C" {
    __attribute__((naked)) void my_handler()
    {

    enter_handler;
    // Do whatever you need to do.
    exit_handler;

    }
    }
*/

#define enter_handler \
asm volatile ("    addi    sp,sp,-64 "); \
asm volatile ("    sw      ra,60(sp) "); \
asm volatile ("    sw      t6,56(sp) "); \
asm volatile ("    sw      t5,52(sp) "); \
asm volatile ("    sw      t4,48(sp) "); \
asm volatile ("    sw      t3,44(sp) "); \
asm volatile ("    sw      t2,40(sp) "); \
asm volatile ("    sw      t1,36(sp) "); \
asm volatile ("    sw      t0,32(sp) "); \
asm volatile ("    sw      a7,28(sp) "); \
asm volatile ("    sw      a6,24(sp) "); \
asm volatile ("    sw      a5,20(sp) "); \
asm volatile ("    sw      a4,16(sp) "); \
asm volatile ("    sw      a3,12(sp) "); \
asm volatile ("    sw      a2,8(sp) "); \
asm volatile ("    sw      a1,4(sp) "); \
asm volatile ("    sw      a0,0(sp) ");
#define exit_handler \
asm volatile ("    lw      ra,60(sp) "); \
asm volatile ("    lw      t6,56(sp) "); \
asm volatile ("    lw      t5,52(sp) "); \
asm volatile ("    lw      t4,48(sp) "); \
asm volatile ("    lw      t3,44(sp) "); \
asm volatile ("    lw      t2,40(sp) "); \
asm volatile ("    lw      t1,36(sp) "); \
asm volatile ("    lw      t0,32(sp) "); \
asm volatile ("    lw      a7,28(sp) "); \
asm volatile ("    lw      a6,24(sp) "); \
asm volatile ("    lw      a5,20(sp) "); \
asm volatile ("    lw      a4,16(sp) "); \
asm volatile ("    lw      a3,12(sp) "); \
asm volatile ("    lw      a2,8(sp) "); \
asm volatile ("    lw      a1,4(sp) "); \
asm volatile ("    lw      a0,0(sp) "); \
asm volatile ("    addi    sp,sp,64 "); \
asm volatile ("    ret ");

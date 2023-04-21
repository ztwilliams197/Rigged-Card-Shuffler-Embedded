#include "sleep.h"

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void sleep_ms(int ms) {
    nano_wait(ms * 1000000);
}

void sleep_micros(int mus) {
    nano_wait(mus * 1000);
}

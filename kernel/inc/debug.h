#ifndef DEBUG_HPP
#define DEBUG_HPP

void debug_breakpoint() {
    asm("int $69");
}

#endif
#ifndef MEMORY_H
#define MEMORY_H


// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

uint32_t string_length(char* string) {
    uint32_t size = 0;
    while(*string) {
        size++;
        string++;
    }
    return size;
}

void integer_to_string(char* buffer, uint64_t size, uint64_t number) {
    if (size == 0) {
        return;
    }
    if (number == 0) {
        buffer[0] = '0';
        return;
    }
    int64_t digit_count = 0;
    for(;;) {
        buffer[digit_count++] = ((number % 10) + '0');
        number /= 10;
        if (number == 0) {
            break;
        }
    }
    int64_t index = digit_count - 1;
    digit_count = 0;
    while(digit_count < index) {
        char temp = buffer[digit_count]; // We could avoid this and swap in place but I feel this is fine after all its a 64-bit OS we got plenty of space
        buffer[digit_count] = buffer[index];
        buffer[index] = temp;
        index--;
        digit_count++;
    }
}

#endif
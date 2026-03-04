#ifndef UTILITY_H
#define UTILITY_H

uint32_t string_length(char* string) {
    uint32_t size = 0;
    while(*string) {
        size++;
        string++;
    }
    return size;
}

void integer_to_string(char* buffer, uint64_t number) {

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
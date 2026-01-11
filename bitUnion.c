
#include <stdint.h>
#include <stdio.h>


union EXAMPLE_REGISTER {
    uint8_t   hw;
    struct __attribute__((packed))
        {                                         // Bits  Description
        uint8_t  IDLE                       : 1;  // 0     Idle / Standby mode
        uint8_t  LOW_BRIGHTNESS             : 1;  // 1     Low brightness
        uint8_t  NORMAL_BRIGHTNESS          : 1;  // 2     Normal brightness
        uint8_t  HIGH_BRIGHTNESS            : 1;  // 3     High brightness
        uint8_t  PARTY_MODE                 : 1;  // 4     Party mode (LEDs flash)
        uint8_t  DEBUG_MODE                 : 1;  // 5     Debug mode
        uint8_t  RESERVED                   : 1;  // 6     Reserved
        uint8_t  FACTORY_TEST_MODE          : 1;  // 7     Factory test mode
        }s;
};

int main() {
    volatile union EXAMPLE_REGISTER reg;

    // Example usage: Set the register to PARTY_MODE
    reg.hw = 0; // Clear all bits
    reg.s.PARTY_MODE = 1;

    // Print the register value
    printf("Register value: 0x%02X\n", reg.hw);
    
    return 0;
}

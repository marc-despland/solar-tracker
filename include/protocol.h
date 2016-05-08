#pragma once

// This is a truncted version of the protocol.h file from pololu-usb-sdk
// Unused parts were removed to reduce clutter.

// These are the values to put in to bRequest when making a setup packet
// for a control transfer to the Maestro.
enum uscRequest
{
    REQUEST_GET_PARAMETER = 0x81,
    REQUEST_SET_PARAMETER = 0x82,
    REQUEST_GET_VARIABLES = 0x83,
    REQUEST_SET_SERVO_VARIABLE = 0x84, // (also clears the serial timeout timer)
    REQUEST_SET_TARGET = 0x85,   // (also clears the serial timeout timer)
    REQUEST_CLEAR_ERRORS = 0x86, // (also clears the serial timeout timer)
    REQUEST_REINITIALIZE = 0x90,
    REQUEST_ERASE_SCRIPT = 0xA0,
    REQUEST_WRITE_SCRIPT = 0xA1,
    REQUEST_SET_SCRIPT_DONE = 0xA2, // value.low.b is 0 for go, 1 for stop, 2 for single-step
    REQUEST_RESTART_SCRIPT_AT_SUBROUTINE = 0xA3,
    REQUEST_RESTART_SCRIPT_AT_SUBROUTINE_WITH_PARAMETER = 0xA4,
    REQUEST_RESTART_SCRIPT = 0xA5,
    REQUEST_START_BOOTLOADER = 0xFF
};

// These are the bytes used to refer to the different parameters
// in REQUEST_GET_PARAMETER and REQUEST_SET_PARAMETER.  After changing
// any parameter marked as an "Init parameter", you must do REQUEST_REINITIALIZE
// before the new value will be used.
enum uscParameter
{
    PARAMETER_SERVOS_AVAILABLE                  = 1, // 1 byte - 0-5.  Init parameter.
    PARAMETER_SERVO_PERIOD                      = 2, // 1 byte - instruction cycles allocated to each servo/256, (units of 21.3333 us).  Init parameter.
    PARAMETER_SERIAL_MODE                       = 3, // 1 byte unsigned value.  Valid values are SERIAL_MODE_*.  Init parameter.
    PARAMETER_SERIAL_FIXED_BAUD_RATE            = 4, // 2-byte unsigned value; 0 means autodetect.  Init parameter.
    PARAMETER_SERIAL_TIMEOUT                    = 6, // 2-byte unsigned value (units of 10ms)
    PARAMETER_SERIAL_ENABLE_CRC                 = 8, // 1 byte boolean value
    PARAMETER_SERIAL_NEVER_SUSPEND              = 9, // 1 byte boolean value
    PARAMETER_SERIAL_DEVICE_NUMBER              = 10, // 1 byte unsigned value, 0-127
    PARAMETER_SERIAL_BAUD_DETECT_TYPE           = 11, // 1 byte - reserved

    PARAMETER_IO_MASK_A                         = 12, // 1 byte - reserved, init parameter
    PARAMETER_OUTPUT_MASK_A                     = 13, // 1 byte - reserved, init parameter
    PARAMETER_IO_MASK_B                         = 14, // 1 byte - reserved, init parameter
    PARAMETER_OUTPUT_MASK_B                     = 15, // 1 byte - reserved, init parameter
    PARAMETER_IO_MASK_C                         = 16, // 1 byte - pins used for I/O instead of servo, init parameter
    PARAMETER_OUTPUT_MASK_C                     = 17, // 1 byte - outputs that are enabled, init parameter
    PARAMETER_IO_MASK_D                         = 18, // 1 byte - reserved, init parameter
    PARAMETER_OUTPUT_MASK_D                     = 19, // 1 byte - reserved, init parameter
    PARAMETER_IO_MASK_E                         = 20, // 1 byte - reserved, init parameter
    PARAMETER_OUTPUT_MASK_E                     = 21, // 1 byte - reserved, init parameter

    PARAMETER_SCRIPT_CRC                        = 22, // 2 byte CRC of script
    PARAMETER_SCRIPT_DONE                       = 24, // 1 byte - if 0, run the bytecode on restart, if 1, stop

    PARAMETER_SERIAL_MINI_SSC_OFFSET            = 25, // 1 byte (0-254)

    PARAMETER_SERVO0_HOME                       = 30, // 2 byte home position (0=off; 1=ignore)
    PARAMETER_SERVO0_MIN                        = 32, // 1 byte min allowed value (x2^6)
    PARAMETER_SERVO0_MAX                        = 33, // 1 byte max allowed value (x2^6)
    PARAMETER_SERVO0_NEUTRAL                    = 34, // 2 byte neutral position
    PARAMETER_SERVO0_RANGE                      = 36, // 1 byte range
    PARAMETER_SERVO0_SPEED                      = 37, // 1 byte (5 mantissa,3 exponent) us per 10ms.  Init parameter.
    PARAMETER_SERVO0_ACCELERATION               = 38, // 1 byte (speed changes that much every 10ms). Init parameter.

    PARAMETER_SERVO1_HOME                       = 39,
    // The pattern continues.  Each servo takes 9 bytes of configuration space.
};

struct servoSetting
{
    uint16_t position;
    uint16_t target;
    uint16_t speed;
    uint8_t acceleration;
};

/*
 * This struct stores all the variables that can be read via REQUEST_GET_VARIABLES.
 */
struct uscVariables
{
    uint8_t stackPointer;  		//1
    uint8_t callStackPointer;	//2
    uint16_t errors;			//4
    uint16_t programCounter;	//6
    int16_t buffer[3]; 			//12 protects other RAM from being corrupted by improper instructions
    int16_t stack[32];			//76
    uint16_t callStack[10];		//96
    uint8_t scriptDone; 		//97  1 = done; 2 = about to run a single step then be done - placed here to protect against accidental overwriting of servoSetting
    uint8_t buffer2; 			//98 protects other RAM from being corrupted by improper instructions
    struct servoSetting servoSetting[6];
}; //140

#pragma pack(push)
#pragma pack(1)
struct ServoStatus{
    uint16_t position;
    uint16_t target;
    uint16_t speed;
    uint8_t acceleration;
};
#pragma pack(pop)
struct MaestroVariables {
    uint8_t stackPointer;
    uint8_t callStackPointer;
    uint16_t errors;
    uint16_t programCounter;
    uint8_t scriptDone;
    uint8_t performanceFlags;
};

struct MicroMaestroVariables{
    uint8_t stackPointer;
    uint8_t callStackPointer;
    uint16_t errors;
    uint16_t programCounter;
    uint16_t buffer[3];
    uint16_t stack[32];
    uint16_t callStack[10];
    uint8_t scriptDone;
    uint8_t buffer2;
};

struct MiniMaestroVariables{
    uint8_t stackPointer;
    uint8_t callStackPointer;
    uint16_t errors;
    uint16_t programCounter;
    uint8_t scriptDone;
    uint8_t performanceFlags; 
};



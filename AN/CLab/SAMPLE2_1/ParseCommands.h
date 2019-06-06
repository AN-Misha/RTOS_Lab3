//Parsing special commands
#include <stdint.h>
#include <string.h>

#include "trex.h"

enum Direction
{
	LEFT, 
	RIGHT
};


enum Command 
{
    PUTC,
    CLEAR,
    PUTSTRING,
    SCROLLSTRING,
	OTHER
};

typedef struct PARSEresulte 
{
    uint8_t command_name;
    char chr;
    char* str;
    uint8_t str_len;
    uint8_t x;
    uint8_t y;
    uint16_t period;
    uint8_t direction; 
}PARSEresulte;



void getDataFromUART(PARSEresulte* parseStruct, char* cmd);

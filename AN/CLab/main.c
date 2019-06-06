#include <stdio.h>
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


typedef struct PUTCstruct 
{
    char Chr;
    uint8_t X;
    uint8_t Y;
} PUTCstruct;

typedef struct PUTSTRstruct 
{
    char *string;
    uint8_t X;
} PUTSTRstruct;

typedef struct SCROLLSTRstruct 
{
    char *string;
    uint8_t X;
    uint16_t period;
    
} SCROLLSTRstruct;

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

char* pattern_putc = "^PUTC=\"(\\w)\",(\\d*),(\\d*)";
char* pattern_putstr = "^PUTSTRING=\"(\\w*)\",(\\d*)";
char* pattern_scrollstr = "^SCROLLSTRING=\"(\\w*)\",(\\d*),(\\d*),((LEFT)|(RIGHT)))";

uint8_t cmd_is_not_found = 1;
int* strPointer;
int* dataStructPointer;

void clear_disp();
void putc_disp(PARSEresulte *resultStruct);
void putstr_disp(PARSEresulte *resultStruct);
void scrollstr_disp(PARSEresulte *resultStruct);

int main()
{
   char cmd[40];
   memset(&cmd[0], 0, sizeof(cmd));
   scanf("%s", &cmd);
   
   PARSEresulte parseStruct;
   /*
   int command = parse_command(&cmd);
   PUTCstruct PUTCdata;
   get_data_putc(&PUTCdata, cmd);
   printf("%d\n", command);
   printf("CHAR: %c, X: %d, Y: %d\n", PUTCdata.Chr, PUTCdata.X, PUTCdata.Y);
    */
    
 
   struct TRex *regex;
   if(!strncmp(cmd,"CLEAR", 5))
   {
       //printf("CLEAR!\n");
       cmd_is_not_found = 0;
       parseStruct.command_name = CLEAR;
       
   }
   if(cmd_is_not_found)
   {
       regex = trex_compile(pattern_putc);
       if (trex_match(regex, cmd))
       {
          //printf("PUTC!");
          cmd_is_not_found = 0;
          TRexMatch match;
          char buf[15];
          int matc_count = trex_getsubexpcount(regex);
          trex_getsubexp(regex, 1, &match);
          strncpy(buf, match.begin, match.len);
          parseStruct.chr = buf[0];
          trex_getsubexp(regex, 2, &match);
          strncpy(buf, match.begin, match.len);
          sscanf(buf,"%d", &parseStruct.x);
          trex_getsubexp(regex, 3, &match);
          strncpy(buf, match.begin, match.len);
          sscanf(buf,"%d", &parseStruct.y);
          parseStruct.command_name = PUTC;
       }
       trex_free(regex);
   }
   
   if(cmd_is_not_found)
   {
       regex = trex_compile(pattern_putstr);
       if (trex_match(regex, cmd))
       {
         // printf("PUTSTRING!");
          cmd_is_not_found = 0;
          TRexMatch match;
          char buf[15];
          int matc_count = trex_getsubexpcount(regex);
          trex_getsubexp(regex, 1, &match);
          strPointer = malloc(match.len + 1);
          strncpy(strPointer, match.begin, match.len);
		  //strPointer[match.len] = '\0';
          parseStruct.str = strPointer;
		  parseStruct.str[match.len] = '\0';
          parseStruct.str_len = match.len;
          trex_getsubexp(regex, 2, &match);
          strncpy(buf, match.begin, match.len);
          sscanf(buf,"%d", &parseStruct.x);
          parseStruct.command_name = PUTSTRING;
       }
       trex_free(regex);
   }
   
   if(cmd_is_not_found)
   {
       regex = trex_compile(pattern_scrollstr);
       if (trex_match(regex, cmd))
       {
          //printf("SCROLLSTRING!");
          cmd_is_not_found = 0;
          TRexMatch match;
          char buf[15];
          int matc_count = trex_getsubexpcount(regex);
          trex_getsubexp(regex, 1, &match);
          strPointer = malloc(match.len + 1);
          strncpy(strPointer, match.begin, match.len);
          parseStruct.str = strPointer;
          parseStruct.str_len = match.len;
		  parseStruct.str[match.len] = '\0';
          trex_getsubexp(regex, 2, &match);
          strncpy(buf, match.begin, match.len);
          sscanf(buf,"%d", &parseStruct.x);
		  trex_getsubexp(regex, 3, &match);
          strncpy(buf, match.begin, match.len);
          sscanf(buf,"%d", &parseStruct.period);
		  trex_getsubexp(regex, 4, &match);
          strncpy(buf, match.begin, match.len);
          if(!strncmp(buf, "LEFT", match.len))
		  {
			  parseStruct.direction = LEFT;
		  }
          else
		  {
			  parseStruct.direction = RIGHT;
		  }
		  parseStruct.command_name = SCROLLSTRING;
       }
       trex_free(regex);
   }
   
   if(cmd_is_not_found)
   {
       parseStruct.command_name = OTHER;
   }
   
   switch(parseStruct.command_name)
   {
	   case CLEAR:
	   clear_disp();
	   break;
	   
	   case PUTC:
	   putc_disp(&parseStruct);
	   break;
	   
	   case PUTSTRING:
	   putstr_disp(&parseStruct);
	   break;
	   
	   case SCROLLSTRING:
	   scrollstr_disp(&parseStruct);
	   break;
	   
	   case OTHER:
	   printf("INVALID COMMAND\n");
	   break;
	   
   }

   getch();
   
   return 0;
}

void clear_disp()
{
	printf("CLEAR!!!\n");
}

void putc_disp(PARSEresulte *resultStruct)
{
	printf("CHAR:%c, X:%d, Y:%d\n", resultStruct->chr, resultStruct->x, resultStruct->y);
}

void putstr_disp(PARSEresulte *resultStruct)
{
	printf("STR:%s, X:%d\n", resultStruct->str, resultStruct->x);
	free(strPointer);
}

void scrollstr_disp(PARSEresulte *resultStruct)
{
	printf("STR:%s, X:%d, T:%d, Dir: %d\n", resultStruct->str, resultStruct->x, resultStruct->period, resultStruct->direction);
	free(strPointer);
}
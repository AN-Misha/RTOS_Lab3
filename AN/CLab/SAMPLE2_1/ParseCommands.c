#include "ParseCommands.h"
#include <stdio.h>

char* pattern_putc = "^PUTC=\"(\\w)\",(\\d*),(\\d*)";
char* pattern_putstr = "^PUTSTRING=\"(\\w*)\",(\\d*)";
char* pattern_scrollstr = "^SCROLLSTRING=\"(\\w*)\",(\\d*),(\\d*),((LEFT)|(RIGHT)))";

uint8_t cmd_is_not_found = 1;

struct TRex *regex;


void getDataFromUART(PARSEresulte* parseStruct, char* cmd)
{
	
	char* strPointer;
  
	
	if(!strncmp(cmd,"CLEAR", 5))
 {
		 //printf("CLEAR!\n");
		 cmd_is_not_found = 0;
		 parseStruct->command_name = CLEAR;
		 
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
				parseStruct->chr = buf[0];
				trex_getsubexp(regex, 2, &match);
				strncpy(buf, match.begin, match.len);
				sscanf(buf,"%d", &parseStruct->x);
				trex_getsubexp(regex, 3, &match);
				strncpy(buf, match.begin, match.len);
				sscanf(buf,"%d", &parseStruct->y);
				parseStruct->command_name = PUTC;
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
				strPointer = (char*)malloc(match.len + 1);
				strncpy(strPointer, match.begin, match.len);
		//strPointer[match.len] = '\0';
				parseStruct->str = strPointer;
				parseStruct->str[match.len] = '\0';
				parseStruct->str_len = match.len;
				trex_getsubexp(regex, 2, &match);
				strncpy(buf, match.begin, match.len);
				sscanf(buf,"%d", &parseStruct->x);
				parseStruct->command_name = PUTSTRING;
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
				strPointer = (char*)malloc(match.len + 1);
				strncpy(strPointer, match.begin, match.len);
				parseStruct->str = strPointer;
				parseStruct->str_len = match.len;
				parseStruct->str[match.len] = '\0';
			 
				trex_getsubexp(regex, 2, &match);
				strncpy(buf, match.begin, match.len);
				sscanf(buf,"%d", &parseStruct->x);
			 
				trex_getsubexp(regex, 3, &match);
				strncpy(buf, match.begin, match.len);
				sscanf(buf,"%d", &parseStruct->period);
			 
				trex_getsubexp(regex, 4, &match);
				strncpy(buf, match.begin, match.len);
				if(!strncmp(buf, "LEFT", match.len))
		{
			parseStruct->direction = LEFT;
		}
				else
		{
			parseStruct->direction = RIGHT;
		}
		parseStruct->command_name = SCROLLSTRING;
		 }
		 trex_free(regex);
 }
 
 if(cmd_is_not_found)
 {
		 parseStruct->command_name = OTHER;
 }
}

/*----------------------------------------------------------*/
/*  Filename :  DEBUG_CONSOLE.C     Created    : 2014-12-23 */
/*  Released :                      Revision # :            */
/*  Author   :  Butch Tabios                                */
/*                                                          */
/*  Routine List:                                           */
/*                                                          */
/*  Update List:                                            */
/*                                                          */
/*  Files needed for linking:                               */
/*                                                          */
/*  Comments:                                               */
/*    1. Debug console for the AVR32 mini board.            */
/*                                                          */
/*----------------------------------------------------------*/

#include <asf.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "string.h"
#include "debug_console.h"
#include "Timer.h"

#ifndef EOF
#define EOF (-1)            /* End of file indicator */
#endif

#ifndef BS
#define	BS       	0x08
#endif
#ifndef LF
#define	LF       	0x0a
#endif
#ifndef CR
#define	CR       	0x0d
#endif

#define	forever		1
#ifndef	TRUE
#define	TRUE		1
#define	FALSE		0
#endif
#ifndef	true
#define	true		TRUE
#endif
#ifndef	false
#define	false		FALSE
#endif

#define	SendDebugPrompt	DebugSend("\r\n->\0")
#define	SendError		DebugSend("\r\n Error???\0")

typedef unsigned char BYTE;
typedef unsigned int  WORD;

static unsigned int dump_beg = 0;
static unsigned int old_dump_beg = 0;
static unsigned int dump_end = 0;
static unsigned char *debug_port = (unsigned char *)0x80000000;

Bool bTest = FALSE;

#define	BUFFER_SIZE	1024

char tx_buffer[BUFFER_SIZE];
unsigned short tx_head, tx_tail;

void DebugPutChar(char ch);
void DebugSend(char *message);

void DebugInit(void)
{
	tx_head = tx_tail = 0;
	DebugPrint("\r\n Welcome to Debug Console ver 1.0!");
	DebugPrint(" Compile Date: %s, Time: %s \r\n",__DATE__,__TIME__);
	SendDebugPrompt;
}

unsigned int do_dump(void)
{
	int i1;
	unsigned char *pByte_Val, byte_val;
	char ascii_result[18];

	old_dump_beg = dump_beg;
	DebugPrint("\r\n %08X - ",dump_beg & 0xfffffff0);
	strcpy(ascii_result,"................");
	for (i1=0;i1<(dump_beg & 0x000f);i1++) DebugSend("   ");
	while (dump_beg <= dump_end)
	{
		pByte_Val = (unsigned char *)dump_beg++;
		byte_val = *pByte_Val;
		DebugPrint("%02X ",byte_val);
		if (!iscntrl(byte_val&0x7f)) ascii_result[i1] = byte_val;
		i1++;
		if (!(dump_beg & 0x000f))
		{
			DebugSend("  ");
			DebugSend(ascii_result);
			i1 = 0;
			strcpy(ascii_result,"................");
			if (dump_beg==0) break;
			if (dump_beg <= dump_end)
			{
				DebugPrint("\r\n %08X - ",dump_beg & 0xfffffff0);
			}
		}
	}

	return (dump_beg);
}

void debug_parse(char *cmd_line)
{
	int params;
	unsigned int temp1,temp2,temp3,temp4;
	unsigned short tempword;
	unsigned char tempbyte;

	while (*cmd_line == ' ') cmd_line++;

	switch (toupper(*cmd_line++))
	{
	case '?':
		DebugPrint("\r\n Compile Date: %s, Time: %s",__DATE__,__TIME__);
		break;
	case 'A':  // assemble
		break;
	case 'B':
		break;
	case 'C':  // compare
		break;
	case 'D':  // dump
		params = sscanf(cmd_line,"%X %X",&temp2,&temp3);
		switch (params) {
		case 0:
			case EOF:
			dump_end = dump_beg + 127;
			do_dump();
			break;
		case 1:
			dump_beg = temp2;
			dump_end = dump_beg + 127;
			do_dump();
			break;
		case 2:
			if (temp3<temp2) temp3=temp2;
			dump_beg = temp2;
			dump_end = temp3;
			do_dump();
			break;
		}
		break;
	case 'E':  // read unsigned int
		if (sscanf(cmd_line,"%X",&temp1)==1)
			debug_port = (unsigned char*)(temp1&0xFFFFFFFC);
		temp2 = *(unsigned int*)debug_port;
		DebugPrint("\r\n %08X -> %08X",(int)debug_port,temp2);
//		debug_port += sizeof(int);
		break;
	case 'F':  // fill
		params = sscanf(cmd_line,"%X %X %X",&temp2,&temp3,&temp4);
		switch (params) {
		case 3:
			dump_end = temp3;
			for (old_dump_beg=temp2;old_dump_beg<=dump_end;old_dump_beg++)
			{
//				pokeb(dump_seg,old_dump_beg,temp4);
			}
			break;
		default:
			{
				DebugSend(" ?? \r\n");
			}
			break;
		}
		break;
	case 'G':  // go
		DebugInit();
		break;
	case 'H':  // hex
		break;
	case 'I':  // read byte
		if (sscanf(cmd_line,"%X",&temp1)==1)
		{
			debug_port = (unsigned char*)temp1;
			tempbyte = *(unsigned char*)debug_port;
			DebugPrint("\r\n %08X -> %02X",(int)debug_port,tempbyte);
			debug_port += sizeof(tempbyte);
		}
		break;
	case 'J':  // read word
		if (sscanf(cmd_line,"%X",&temp1)==1)
		{
			debug_port = (unsigned char*)(temp1&0xFFFFFFFE);
		}
		tempword = *(unsigned short*)debug_port;
		DebugPrint("\r\n %08X -> %04X",(int)debug_port,tempword);
		break;
	case 'K':
		break;
	case 'L':  // load
		break;
	case 'M':  // write unsigned int
		if (sscanf(cmd_line,"%X %X",&temp1,&temp2)==2)
		{
			debug_port = (unsigned char*)(temp1&0xFFFFFFFC);
			DebugPrint("\r\n %08X <- %08X",(int)debug_port,temp2);
			*(unsigned int*)debug_port = temp2;
			debug_port += sizeof(int);
		}
		else
		{
			DebugSend(" ?? \r\n");
		}
		break;
	case 'N':  // name
		break;
	case 'O':  // output byte
		if (sscanf(cmd_line,"%X %X",&temp1,&temp2)==2)
		{
			debug_port = (unsigned char*)temp1;
			tempbyte = temp2;
			DebugPrint("\r\n %08X <- %02X",(int)debug_port,tempbyte);
			*debug_port = tempbyte;
			debug_port += sizeof(tempbyte);
		}
		else
		{
			DebugSend(" ?? \r\n");
		}
		break;
	case 'P':  // proceed
		if (sscanf(cmd_line,"%X",&temp1)==1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		else
		{
		}
		break;
	case 'Q':  // quit
		DebugSend("\r\nYou can't quit now!");
		break;
	case 'R':  // register
		break;
	case 'S':  // search
		if (sscanf(cmd_line,"%X",&temp2)==1)
		{
		}
		break;
	case 'T':  // Test
		if (sscanf(cmd_line,"%X",&temp1)==1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		else
		{
		}
		break;
	case 'U':
		if (sscanf(cmd_line,"%X",&temp1)==1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:	//U5
				break;
			case 6:	//U6
				break;
			case 7:	//U7
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		else
		{
		}
		break;
	case 'W':  // write word
		if (sscanf(cmd_line,"%X %X",&temp1,&temp2)==2)
		{
			debug_port = (unsigned char*)(temp1&0xFFFFFFFE);
			tempword = temp2;
			DebugPrint("\r\n %08X <- %04X",(int)debug_port,tempword);
			*(unsigned short*)debug_port = tempword;
			debug_port += sizeof(tempword);
		}
		else
		{
			DebugSend(" ?? \r\n");
		}
		break;
	case 'X':
		if (sscanf(cmd_line,"%X",&temp1)==1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		else
		{
		}
		break;
	case 'Y':
		temp4 = sscanf(cmd_line,"%X %X",&temp1,&temp2);
		if (temp4>=1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		else
		{
		}
		break;
	case 'Z':
		if (sscanf(cmd_line,"%i %i",&temp1,&temp2)==2)
		{
		}
		else
		if (sscanf(cmd_line,"%X",&temp1)==1)
		{
			switch (temp1){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			}
		}
		else
		{
		}
		break;
	default:
		;
	}
}

void debug_rcv(char ch)
{
	static int  lineptr = 0;
	static char linebuff[256];

	if ((ch=='\r') || (lineptr==255))
	{
		linebuff[lineptr] = 0;
		if (lineptr)
		{
			debug_parse(linebuff);
		}
		lineptr = 0;
		SendDebugPrompt;
	}
	else if (iscntrl(ch))
	{
		switch (ch)
		{
		case BS:
			if (lineptr)
			{
				DebugPutChar(ch);
				lineptr--;
			}
			break;
		}
	}
	else
	{
		linebuff[lineptr++] = ch;
		DebugPutChar(ch);
	}
}

void debug_idle(void)
{
	if (bTest)
	{
	}
}

void DebugTask(void)
{
	if (debug_char_rdy())
		debug_rcv(get_debug_char());
	else
		debug_idle();

	if (tx_tail!=tx_head)
	{
		if (put_debug_char(tx_buffer[tx_tail]))
			tx_tail = (tx_tail+1)&(BUFFER_SIZE-1);
	}
}

void DebugPutChar(char ch)
{
	tx_buffer[tx_head] = ch;
	tx_head = (tx_head+1)&(BUFFER_SIZE-1);
	if (tx_head==tx_tail)
		tx_tail = (tx_tail+1)&(BUFFER_SIZE-1);	//discard oldest
}

void DebugSend(char *message)
{
	char ch;

	while (*message != '\0')
	{
		ch = *message;
		message++;
		DebugPutChar(ch);
	}
}

void DebugPrint(const char *format, ...)
{
	char debug_result[82];
	va_list argptr;
    va_start(argptr, format);
    vsprintf(debug_result, format, argptr);
    va_end(argptr);
	DebugSend(debug_result);
}

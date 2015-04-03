/*----------------------------------------------------------*/
/*  Filename :  DEBUG_CONSOLE.H     Created    : 2014-12-23 */
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
/*                                                          */
/*----------------------------------------------------------*/
#ifndef _DEBUG_CONSOLE_H_
#define _DEBUG_CONSOLE_H_

void DebugInit(void);

void DebugTask(void);

void DebugPutChar(char ch);

void DebugPrint(const char *format, ...);

#endif	//#ifndef _DEBUG_CONSOLE_H_

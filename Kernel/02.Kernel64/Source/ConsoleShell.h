#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "Types.h"

#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT 300
#define CONSOLESHELL_PROMPTMESSAGE         "DSH>"

typedef void ( * CommandFunction ) (const char* pcParameter );

#pragma pack( push, 1 )

typedef struct kShellCommandEntryStruct
{
    char* pcCommand;

    char* pcHelp;

    CommandFunction pfFunction;
} SHELLCOMMANDENTRY;

typedef struct kParameterListStruct
{
    const char* pcBuffer;

    int iLength;

    int iCurrentPosition;
} PARAMETERLIST;

#pragma pack ( pop )

void kStartConsoleShell( void );
void kExecuteCommand( const char* pcCommandBuffer );
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter );
int kGetNextParameter( PARAMETERLIST* pstLIst, char* pcParameter );
void kHelp( const char* pcCommandBuffer );
void kCls( const char* pcParameterBuffer );
void kShowTotalRAMSize( const char* pcParameterBuffer );
void kStringToDecimalHexTest( const char* pcParameterBuffer );
void kShutdown( const char* pcParamegerBuffer );
#endif
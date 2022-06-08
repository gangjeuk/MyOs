#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdarg.h>
#include "Types.h"

void kmemset( void* pvDestination, BYTE bData, int iSize );
int kmemcpy( void *pvDestination, const void* pvSource, int iSize);
int kmemcmp( const void* pvDestination, const void* pvSource, int iSize);

void kCheckTotalRAMSize( void );
QWORD kGetTotalRAMSize( void );
void kReverseString( char* buf );
QWORD kHexStringToQword( const char* buf );
long kDecimalStringToLong( const char* pcBuffer );
int kDecimalToString( long lvalue, char* buf);

int kstrlen( const char* pcBuffer );
int kitoa( long lvalue, char* buf, int radix);
int katoi( const char* buf, int idx);
int kHexToString( QWORD qwValue, char* buf);
int ksprintf( char* buf, const char* fmt_str, ...);
int kvsprintf( char* buf, const char* fmt_str, va_list ap);

BOOL kSetInterruptFlag( BOOL bEnableInterrupt );

#endif

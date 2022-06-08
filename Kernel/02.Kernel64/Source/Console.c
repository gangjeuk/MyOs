#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "AssemblyUtility.h"

CONSOLEMANAGER gs_stConsoleManager = { 0, };

void kInitializeConsole( int iX, int iY )
{
    kmemset( &gs_stConsoleManager, 0, sizeof(gs_stConsoleManager) );

    kSetCursor( iX, iY );
}
void kSetCursor(int iX, int iY)
{
    int iLinearValue;

    // 커서의 위치를 계산
    iLinearValue = iY * CONSOLE_WIDTH + iX;

    // CRTC 컨트롤 어드레스 레지스터(포트 0x3D4)에 0x0E를 전송하여
    // 상위 커서 위치 레지스터를 선택
    kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR );
    // CRTC 컨트롤 데이터 레지스터(포트 0x3D5)에 커서의 상위 바이트를 출력
    kOutPortByte( VGA_PORT_DATA, iLinearValue >> 8 );

    // CRTC 컨트롤 어드레스 레지스터(포트 0x3D4)에 0x0F를 전송하여
    // 하위 커서 위치 레지스터를 선택
    kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR );
    // CRTC 컨트롤 데이터 레지스터(포트 0x3D5)에 커서의 하위 바이트를 출력
    kOutPortByte( VGA_PORT_DATA, iLinearValue & 0xFF );

    // 문자를 출력할 위치 업데이트
    gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}
void kGetCursor( int *piX, int *piY )
{
    *piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
    *piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}
void kprintf( const char* pcFormatString, ...)
{
    va_list ap;
    char vcBuffer[ 1024 ];
    int next_print_offset;

    va_start( ap, pcFormatString );
    kvsprintf( vcBuffer, pcFormatString, ap );
    va_end( ap );

    next_print_offset = kConsolePrintString( vcBuffer );

    kSetCursor( next_print_offset % CONSOLE_WIDTH, next_print_offset / CONSOLE_WIDTH );
}
int kConsolePrintString( const char* pcBuffer )
{
    CHARACTER* video_ptr = ( CHARACTER* )CONSOLE_VIDEOMEMORYADDRESS;
    int i, j;
    int length;
    int print_offset;

    print_offset = gs_stConsoleManager.iCurrentPrintOffset;

    length = kstrlen( pcBuffer );
    for( i = 0; i < length; i++ )
    {
        if( pcBuffer[i] == '\n')
        {
            print_offset += ( CONSOLE_WIDTH - ( print_offset % CONSOLE_WIDTH ));
        }
        else if( pcBuffer[i] == '\t' )
        {
            print_offset += ( 8 - (print_offset % 8) );
        }
        else
        {
            video_ptr[print_offset].bCharactor = pcBuffer[i];
            video_ptr[print_offset].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            print_offset += 1;
        }

        if( print_offset >= ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) )
        {
            kmemcpy( (char *) CONSOLE_VIDEOMEMORYADDRESS,
                     (char *)CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof( CHARACTER ),
                     ( CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH + sizeof(CHARACTER));

            for( j = ( CONSOLE_HEIGHT - 1) * (CONSOLE_WIDTH); j < ( CONSOLE_HEIGHT) * (CONSOLE_WIDTH); j++);
            {
                video_ptr[j].bCharactor = ' ';
                video_ptr[j].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            }

            print_offset = ( CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;
        }
    }

    return print_offset;
}
void kClearScreen( void )
{
    CHARACTER* video_ptr = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;
    int i;

    for( i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; i++)
    {
        video_ptr[i].bCharactor = ' ';
        video_ptr[i].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }

    kSetCursor( 0, 0 );
}

BYTE kGetCh( void )
{
    KEYDATA stData;

    while( 1 )
    {
        while( kGetKeyFromKeyQueue( &stData ) == FALSE )
        {
            ;
        }

        if( stData.bFlags & KEY_FLAGS_DOWN )
        {
            return stData.bASCIICode;
        }
    }
}
void kPrintStringXY( int iX, int iY, const char* pcString )
{
    int i = 0;
    CHARACTER* video_ptr = ( CHARACTER * ) CONSOLE_VIDEOMEMORYADDRESS;

    video_ptr += ( iY * CONSOLE_WIDTH ) + iX;

    for( i = 0; pcString[ i ] != 0; i++)
    {
        video_ptr[i].bCharactor = pcString[ i ];
        video_ptr[i].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
}

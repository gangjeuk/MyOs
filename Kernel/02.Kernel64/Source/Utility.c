#include "Utility.h"
#include "AssemblyUtility.h"

void kMemSet( void* pvDestination, BYTE bData, int iSize )
{
    int i;

    for( i = 0; i < iSize; i++)
    {
        ( (char*) pvDestination )[ i ] = bData;
    }
}

int kMemCpy( void *pvDestination, const void* pvSource, int iSize)
{
    int i;

    for( i = 0; i < iSize; i++)
    {
        ( (char*) pvDestination )[ i ] = ( (char*) pvSource )[ i ];
    }

    return iSize;
}

int kMemCmp( const void* pvDestination, const void* pvSource, int iSize)
{
    int i;
    char cTemp;

    for( i = 0; i < iSize; i++)
    {
        cTemp = ( (char*) pvDestination)[ i ] - ( (char*) pvSource )[ i ];
        if( cTemp != 0)
        {
            return ( int ) cTemp;
        }
    }
    return 0;
}
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;

    // 이전의 RFLAGS 레지스터 값을 읽은 뒤에 인터럽트 가능/불가 처리
    qwRFLAGS = kReadRFLAGS();
    if( bEnableInterrupt == TRUE )
    {
        kEnableInterrupt();
    }
    else
    {
        kDisableInterrupt();
    }

    if( qwRFLAGS & 0x200 )
    {
        return TRUE;
    }
    return FALSE;
}
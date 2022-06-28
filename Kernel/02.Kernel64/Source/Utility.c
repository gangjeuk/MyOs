#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>

void kmemset( void* pvDestination, BYTE bData, int iSize )
{
    int i;

    for( i = 0; i < iSize; i++)
    {
        ( (char*) pvDestination )[ i ] = bData;
    }
}

int kmemcpy( void *pvDestination, const void* pvSource, int iSize)
{
    int i;

    for( i = 0; i < iSize; i++)
    {
        ( (char*) pvDestination )[ i ] = ( (char*) pvSource )[ i ];
    }

    return iSize;
}

int kmemcmp( const void* pvDestination, const void* pvSource, int iSize)
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


int kstrlen( const char* pcBuffer )
{
    int i;
    
    for( i = 0 ; ; i++ )
    {
        if( pcBuffer[ i ] == '\0' )
        {
            break;
        }
    }
    return i;
}

static int total_ram_size = 0;

void kCheckTotalRAMSize( void )
{
	DWORD *nowAddr, preValue;

	// 64MB(0x4000000)부터 4MB 단위로 검사 시작
	nowAddr = (DWORD*)0x4000000;
	while(1) {
		// 이전 메모리에 있던 값 저장
		preValue = *nowAddr;

		// 0x12345678을 써서 읽었을 때 문제가 없는 곳까지를 유효 메모리 영역으로 인정
		*nowAddr = 0x12345678;
		if(*nowAddr != 0x12345678) break;

		// 이전 메모리 값으로 복원
		*nowAddr = preValue;
		nowAddr += (0x400000 / 4);
	}
	// 체크가 성공한 어드레스를 1MB로 나누어 MB단위 계산
	total_ram_size = (QWORD)nowAddr / 0x100000;
}

QWORD kGetTotalRAMSize( void )
{
    return total_ram_size;
}

int katoi( const char* buf, int idx)
{
    long lReturn;
    
    switch( idx )
    {
        // 16진수
    case 16:
        lReturn = kHexStringToQword( buf );
        break;
        
        // 10진수 또는 기타
    case 10:
    default:
        lReturn = kDecimalStringToLong( buf );
        break;
    }
    return lReturn;
}

QWORD kHexStringToQword( const char* buf )
{
    QWORD ret = 0;
    int i;

    for( i = 0; buf[i] != '\0'; i++)
    {
        ret *= 16;
        if ( ('A' <= buf[i] ) && (buf[i] <= 'Z') )
        {
            ret += (buf[i] - 'A') + 10;
        }
        else if( ('a' <= buf[i] ) && (buf[i] <= 'z' ) )
        {
            ret += (buf[i] -'a') + 10;
        }
        else
        {
            ret += buf[i] - '0';
        }
    }
    return ret;
}

/**
 *  10진수 문자열을 long으로 변환
 */
long kDecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;
    
    // 음수이면 -를 제외하고 나머지를 먼저 long으로 변환
    if( pcBuffer[ 0 ] == '-' )
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    
    // 문자열을 돌면서 차례로 변환
    for( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }
    
    // 음수이면 - 추가
    if( pcBuffer[ 0 ] == '-' )
    {
        lValue = -lValue;
    }
    return lValue;
}

int kitoa( long lvalue, char* buf, int radix)
{
    int ret;

    switch( radix )
    {
        case 16:
            ret = kHexToString( lvalue, buf );
            break;

        case 10:
        default:
            ret = kDecimalToString(lvalue, buf);
            break;
    }
    return ret;
}


int kHexToString( QWORD qwValue, char* buf)
{
    QWORD i;
    QWORD qwCurrentValue;

    // 0이 들어오면 바로 처리
    if( qwValue == 0 )
    {
        buf[ 0 ] = '0';
        buf[ 1 ] = '\0';
        return 1;
    }
    
    // 버퍼에 1의 자리부터 16, 256, ...의 자리 순서로 숫자 삽입
    for( i = 0 ; qwValue > 0 ; i++ )
    {
        qwCurrentValue = qwValue % 16;
        if( qwCurrentValue >= 10 )
        {
            buf[ i ] = 'A' + ( qwCurrentValue - 10 );
        }
        else
        {
            buf[ i ] = '0' + qwCurrentValue;
        }
        
        qwValue = qwValue / 16;
    }
    buf[ i ] = '\0';
    
    // 버퍼에 들어있는 문자열을 뒤집어서 ... 256, 16, 1의 자리 순서로 변경
    kReverseString( buf );
    return i;
}




/**
 *  10진수 값을 문자열로 변환
 */
int kDecimalToString( long lValue, char* pcBuffer )
{
    long i;

    // 0이 들어오면 바로 처리
    if( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // 만약 음수이면 출력 버퍼에 '-'를 추가하고 양수로 변환
    if( lValue < 0 )
    {
        i = 1;
        pcBuffer[ 0 ] = '-';
        lValue = -lValue;
    }
    else
    {
        i = 0;
    }

    // 버퍼에 1의 자리부터 10, 100, 1000 ...의 자리 순서로 숫자 삽입
    for( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;        
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';
    
    // 버퍼에 들어있는 문자열을 뒤집어서 ... 1000, 100, 10, 1의 자리 순서로 변경
    if( pcBuffer[ 0 ] == '-' )
    {
        // 음수인 경우는 부호를 제외하고 문자열을 뒤집음
        kReverseString( &( pcBuffer[ 1 ] ) );
    }
    else
    {
        kReverseString( pcBuffer );
    }
    
    return i;
}


void kReverseString( char* buf )
{
   int iLength;
   int i;
   char cTemp;
   
   
   // 문자열의 가운데를 중심으로 좌/우를 바꿔서 순서를 뒤집음
   iLength = kstrlen( buf );
   for( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = buf[ i ];
       buf[ i ] = buf[ iLength - 1 - i ];
       buf[ iLength - 1 - i ] = cTemp;
   }

}




int ksprintf( char* buf, const char* fmt_str, ...)
{
    va_list ap;
    int iReturn;
    
    // 가변 인자를 꺼내서 vsprintf() 함수에 넘겨줌
    va_start( ap, fmt_str );
    iReturn = kvsprintf( buf, fmt_str, ap );
    va_end( ap );
    
    return iReturn;
}
int kvsprintf( char* buf, const char* fmt_str, va_list ap)
{
    QWORD i, j;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;

    // 포맷 문자열의 길이를 읽어서 문자열의 길이만큼 데이터를 출력 버퍼에 출력
    iFormatLength = kstrlen( fmt_str );
    
    for( i = 0 ; i < iFormatLength ; i++ ) 
    {

        // %로 시작하면 데이터 타입 문자로 처리
        if( fmt_str[ i ] == '%' ) 
        {
            // % 다음의 문자로 이동
            i++;
            switch( fmt_str[ i ] ) 
            {
                // 문자열 출력  
            case 's':
                // 가변 인자에 들어있는 파라미터를 문자열 타입으로 변환
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kstrlen( pcCopyString );
                // 문자열의 길이만큼을 출력 버퍼로 복사하고 출력한 길이만큼 
                // 버퍼의 인덱스를 이동
                kmemcpy( buf + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;
                
                // 문자 출력
            case 'c':
                // 가변 인자에 들어있는 파라미터를 문자 타입으로 변환하여 
                // 출력 버퍼에 복사하고 버퍼의 인덱스를 1만큼 이동
                buf[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

                // 정수 출력
            case 'd':
            case 'i':
                // 가변 인자에 들어있는 파라미터를 정수 타입으로 변환하여
                // 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += kitoa( iValue, buf + iBufferIndex, 10 );
                break;
                
                // 4바이트 Hex 출력
            case 'x':
            case 'X':
                // 가변 인자에 들어있는 파라미터를 DWORD 타입으로 변환하여
                // 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += kitoa( qwValue, buf + iBufferIndex, 16 );
                break;

                // 8바이트 Hex 출력
            case 'q':
            case 'Q':
            case 'p':
                // 가변 인자에 들어있는 파라미터를 QWORD 타입으로 변환하여
                // 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += kitoa( qwValue, buf + iBufferIndex, 16 );
                break;
            
                // 위에 해당하지 않으면 문자를 그대로 출력하고 버퍼의 인덱스를
                // 1만큼 이동
            default:
                buf[ iBufferIndex ] = fmt_str[ i ];
                iBufferIndex++;
                break;
            }
        } 
        // 일반 문자열 처리
        else
        {
            // 문자를 그대로 출력하고 버퍼의 인덱스를 1만큼 이동
            buf[ iBufferIndex ] = fmt_str[ i ];
            iBufferIndex++;
        }
    }

    // NULL을 추가하여 완전한 문자열로 만들고 출력한 문자의 길이를 반환
    buf[ iBufferIndex ] = '\0';
    return iBufferIndex;
}

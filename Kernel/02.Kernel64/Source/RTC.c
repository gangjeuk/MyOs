#include "RTC.h"
#include "AssemblyUtility.h"


void kReadRTCTime(BYTE* pbHour, BYTE* pbMinute, BYTE* pbSecond)
{
    BYTE bData;

    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_HOUR );

    bData = kInPortByte( RTC_CMOSDATA );
    *pbHour = RTC_BCDTOBINARY( bData );

    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_MINUTE);
    
    bData = kInPortByte(RTC_CMOSDATA);
    *pbMinute = RTC_BCDTOBINARY( bData );

    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_SECOND );
    bData = kInPortByte(RTC_CMOSDATA);
    *pbSecond = RTC_BCDTOBINARY( bData );
}

void kReadRTCDate(WORD* pwYear, BYTE* pbMonth, BYTE* pbDayOfMonth,
                    BYTE* pbDayofWeek )
{
    BYTE bData;

    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_YEAR );
    bData = kInPortByte( RTC_CMOSDATA );
    *pwYear = RTC_BCDTOBINARY( bData );

    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_MONTH);
    bData = kInPortByte(RTC_CMOSDATA);
    *pbMonth = RTC_BCDTOBINARY( bData );

    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFMONTH );
    bData = kInPortByte(RTC_CMOSDATA);
    *pbDayOfMonth = RTC_BCDTOBINARY( bData );

    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFWEEK );
    bData = kInPortByte(RTC_CMOSDATA);
    *pbDayofWeek = RTC_BCDTOBINARY( bData );
}
char* kConvertDayOfWeekToString( BYTE bDayOfWeek )
{
    char* vcpDayOfWeekString[ 8 ] = {"Error", "Saturday", "Monday",
                "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    if( bDayOfWeek >= 8)
    {
        return vcpDayOfWeekString[0];
    }

    return vcpDayOfWeekString[ bDayOfWeek ];
}
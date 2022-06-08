#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "AssemblyUtility.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Utility.h"

void Main( void )
{
    int iX, iY;
    char vcTemp[ 2 ] = {0, };
    BYTE bFlags;
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;

    
    kInitializeConsole(0, 10);
    kprintf("Switch To IA-32e Mode Success\n");
    kprintf("IA-32e C Language Kernel Start..............[Pass]\n");
    kprintf("Initialize Console.........................[Pass]\n");

    // kGetCursor( &iX, &iY );
    // kprintf("GDT Initialize And Switch For IA-32e Mode...[    ]");
    // kInitializeGDTTableAndTSS();
    // kLoadGDTR( GDTR_STARTADDRESS );
    // kSetCursor( 45, iY++ );
    // kprintf( "Pass\n"); 

    // kprintf("TSS Segment Load............................[    ]");
    // kLoadTR( GDT_TSSSEGMENT );
    // kSetCursor( 45, iY++ );
    // kprintf( "Pass\n"); 

    // kprintf("IDT Initilize...............................[    ");
    // kInitializeIDTTables();
    // kLoadIDTR( IDTR_STARTADDRESS );
    // kSetCursor( 45, iY++ );
    // kprintf( "Pass], Size = %d MB\n", kGetTotalRAMSize());
    
    // kprintf("Keyboard Activate And Queue Initialize......[    ]");

    // if( kInitializeKeyboard() == TRUE)
    // {
    //     kSetCursor( 45, iY++ );
    // kprintf( "Pass\n"); 
    //     kChangeKeyboardLED(FALSE, FALSE, FALSE);
    // }
    // else
    // {
    //     kSetCursor( 45, iY++ );
    //     kprintf( "Fail\n");
    //     while( 1 ) ;
    // }

    // kprintf("PIC Controller And Interrupt Initialize.....[    ]");
    // kInitializePIC();
    // kMaskPICInterrupt( 0 );
    // kEnableInterrupt();
    // kSetCursor( 45, iY++ );
    // kprintf( "Pass\n"); 

    // kStartConsoleShell();
}


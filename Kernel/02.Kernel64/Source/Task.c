
#include "Task.h"
#include "Descriptor.h"
#include "Utility.h"
#include "AssemblyUtility.h"

static SCHEDULER gs_stScheduler;
static TCBPOOLMANAGER gs_stTCBPoolManager;

void kInitializeTCBPool( void )
{
    int i;

    kmemset(&(gs_stTCBPoolManager), 0, sizeof(gs_stTCBPoolManager));

    gs_stTCBPoolManager.pstStartAddress = (TCB*) TASK_TCBPOOLADDRESS;
    kmemset(TASK_TCBPOOLADDRESS, 0, sizeof(TCB) * TASK_MAXCOUNT);

    for(i = 0; i < TASK_MAXCOUNT; i++)
    {
        gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID = i;
    }

    gs_stTCBPoolManager.iMaxCount = TASK_MAXCOUNT;
    gs_stTCBPoolManager.iAllocatedCount = 1;
}

TCB* kAllocateTCB( void )
{
    TCB* pstEmptyTCB;
    int i;

    if(gs_stTCBPoolManager.iUseCount == gs_stTCBPoolManager.iMaxCount)
    {
        return NULL;
    }

    for( i = 0; i < gs_stTCBPoolManager.iMaxCount; i++)
    {
        if ( ( gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID >> 32) == 0)
        {
            pstEmptyTCB = &(gs_stTCBPoolManager.pstStartAddress[i]);
            break;
        }
    }
    pstEmptyTCB->stLink.qwID = ((QWORD) gs_stTCBPoolManager.iAllocatedCount << 32 ) | i;
    gs_stTCBPoolManager.iUseCount += 1;
    gs_stTCBPoolManager.iAllocatedCount += 1;
    if( gs_stTCBPoolManager.iAllocatedCount == 0)
    {
        gs_stTCBPoolManager.iAllocatedCount = 1;
    }

    return pstEmptyTCB;
}
void kFreeTCB( QWORD qwID )
{
    int i;

    i = qwID & 0xFFFFFFFF;

    kmemset(&(gs_stTCBPoolManager.pstStartAddress[i].stContext), 0, sizeof(CONTEXT));
    gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID = i;

    gs_stTCBPoolManager.iUseCount -= 1;

}
TCB* kCreateTask( QWORD qwFlags, QWORD qwEntryPointAddress )
{
    TCB* pstTask;
    void* pvStackAddress;

    pstTask = kAllocateTCB();
    if(pstTask == NULL)
    {
        return NULL;
    }

    pvStackAddress = (void *) (TASK_STACKPOOLADDRESS + (TASK_STACKSIZE * (pstTask->stLink.qwID & 0xFFFFFFFF)));

    kSetUpTask(pstTask, qwFlags, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);
    kAddTaskToReadyList(pstTask);

    return pstTask;
}


void kSetUpTask( TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress,
        void* pvStackAddress, QWORD qwStackSize )
{
    kmemset(pstTCB->stContext.vqRegister, 0, sizeof( pstTCB->stContext.vqRegister ));

    pstTCB->stContext.vqRegister[ TASK_RSPOFFSET ] = (QWORD) pvStackAddress + qwStackSize;
    pstTCB->stContext.vqRegister[ TASK_RBPOFFSET ] = (QWORD) pvStackAddress + qwStackSize;

    pstTCB->stContext.vqRegister[ TASK_CSOFFSET ] = GDT_KERNELCODESEGMENT;
    pstTCB->stContext.vqRegister[ TASK_DSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_ESOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_FSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_GSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_SSOFFSET ] = GDT_KERNELDATASEGMENT;

    // RIP register interrupt flag
    pstTCB->stContext.vqRegister[ TASK_RIPOFFSET ] = qwEntryPointAddress;

    // RFLAGS register IF bit( = 1) 
    pstTCB->stContext.vqRegister[ TASK_RFLAGSOFFSET ] |= 0x0200;

    // ID, stack, and flag
    pstTCB->pvStackAddress = pvStackAddress;
    pstTCB->qwStackSize = qwStackSize;
    pstTCB->qwFlags = qwFlags;
}

void kInitializeScheduler( void ){

    kInitializeTCBPool();

    k_init_list( &(gs_stScheduler.stReadyList) );

    gs_stScheduler.pstRunningTask = kAllocateTCB();
}
void kSetRunningTask( TCB* pstTask )
{
    gs_stScheduler.pstRunningTask = pstTask;
}
TCB* kGetRunningTask( void )
{
    return gs_stScheduler.pstRunningTask;
}
TCB* kGetNextTaskToRun( void )
{
    if(k_get_list_count(&(gs_stScheduler.stReadyList)) == 0)
    {
        return NULL;
    }

    return (TCB*) k_remove_list_from_head(&(gs_stScheduler.stReadyList));
}
void kAddTaskToReadyList( TCB* pstTask )
{
    k_add_list_to_tail(&(gs_stScheduler.stReadyList), pstTask);
}
void kSchedule( void )
{
    TCB* pstRunningTask, * pstNextTask;
    BOOL bPreviousFlag;
    
    // 전환할 태스크가 있어야 함
    if( k_get_list_count( &( gs_stScheduler.stReadyList ) ) == 0 )
    {
        return ;
    }
    
    // 전환하는 도중 인터럽트가 발생하여 태스크 전환이 또 일어나면 곤란하므로 전환하는 
    // 동안 인터럽트가 발생하지 못하도록 설정
    bPreviousFlag = kSetInterruptFlag( FALSE );
    // 실행할 다음 태스크를 얻음
    pstNextTask = kGetNextTaskToRun();
    if( pstNextTask == NULL )
    {
        kSetInterruptFlag( bPreviousFlag );
        return ;
    }
    
    pstRunningTask = gs_stScheduler.pstRunningTask; 
    kAddTaskToReadyList( pstRunningTask );
    
    // 다음 태스크를 현재 수행 중인 태스크로 설정한 후 콘텍스트 전환
    gs_stScheduler.pstRunningTask = pstNextTask;
    kSwitchContext( &( pstRunningTask->stContext ), &( pstNextTask->stContext ) );

    // 프로세서 사용 시간을 업데이트
    gs_stScheduler.iProcessorTime = TASK_PROCESSTIME;

    kSetInterruptFlag( bPreviousFlag );
}

/**
 *  인터럽트가 발생했을 때, 다른 태스크를 찾아 전환
 *      반드시 인터럽트나 예외가 발생했을 때 호출해야 함
 */
BOOL kScheduleInInterrupt( void )
{
    TCB* pstRunningTask, * pstNextTask;
    char* pcContextAddress;
    
    // 전환할 태스크가 없으면 종료
    pstNextTask = kGetNextTaskToRun();
    if( pstNextTask == NULL )
    {
        return FALSE;
    }
    
    //==========================================================================
    //  태스크 전환 처리   
    //      인터럽트 핸들러에서 저장한 콘텍스트를 다른 콘텍스트로 덮어쓰는 방법으로 처리
    //==========================================================================
    pcContextAddress = ( char* ) IST_STARTADDRESS + IST_SIZE - sizeof( CONTEXT );
    
    // 현재 태스크를 얻어서 IST에 있는 콘텍스트를 복사하고, 현재 태스크를 준비 리스트로
    // 옮김
    pstRunningTask = gs_stScheduler.pstRunningTask;
    kmemcpy( &( pstRunningTask->stContext ), pcContextAddress, sizeof( CONTEXT ) );
    kAddTaskToReadyList( pstRunningTask );

    // 전환해서 실행할 태스크를 Running Task로 설정하고 콘텍스트를 IST에 복사해서
    // 자동으로 태스크 전환이 일어나도록 함
    gs_stScheduler.pstRunningTask = pstNextTask;
    kmemcpy( pcContextAddress, &( pstNextTask->stContext ), sizeof( CONTEXT ) );
    
    // 프로세서 사용 시간을 업데이트
    gs_stScheduler.iProcessorTime = TASK_PROCESSTIME;
    return TRUE;
}
void kDecreaseProcessorTime( void )
{
    if( gs_stScheduler.iProcessorTime > 0)
    {
        gs_stScheduler.iProcessorTime -= 1;
    }
}
BOOL kIsProcessorTimeExpired( void )
{
    if( gs_stScheduler.iProcessorTime <= 0)
        return TRUE;

    return FALSE;
}
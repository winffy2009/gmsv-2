#include "version.h"
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <stdio.h>

#include "buf.h"
#include "net.h"
#include "char.h"
#include "item.h"
#include "object.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "log.h"
#include "petmail.h"

/*------------------------------------------------------------
 * lsprotocol ������ƽ����    ëݩ  ��������ë���٣�
 * ¦�ѣ�߯Ի��
 *  ئ��
 ------------------------------------------------------------*/
static void endlsprotocol( void )
{
    lssproto_CleanupServer();
    saacproto_CleanupClient();
}

/*------------------------------------------------------------
 * ���ͻ���������ئ��ľ��ئ��ئ�з�����ëĸ������������
 * ��ľ��ľ��ƹ��ب���Ｐ����ë���ټ��ģ�
 * ¦�ѣ�߯Ի��
 *  ئ��
 ------------------------------------------------------------*/
static void allDataDump( void )
{
    closeAllLogFile();
#ifndef _SIMPLIFY_ITEMSTRING
	storeObjects( getStoredir() );
	storePetmail();
#endif
	storeCharaData();
}

/*------------------------------------------------------------
 * �����ة����  ��  ���׻�����̫���ľ�£�
 * ¦�ѣ�߯Ի��
 *  ئ��
 ------------------------------------------------------------*/
void shutdownProgram( void )
{
    close( acfd );
    close( bindedfd );
    endlsprotocol();
    endConnect();
    memEnd();
}

char *DebugFunctionName = NULL;
//char *DebugFunctionName_CHAR = NULL;
//char *DebugFunctionName_ITEM = NULL;
int DebugPoint = 0;

void sigshutdown( int number )
{
    print( "Received signal : %d\n" , number  );
    if( number == 0 )print( "\ngmsv normal down\n" );
    print( "\nDebugPoint (%d)\n", DebugPoint );
    print( "\nLastFunc (%s)\n", DebugFunctionName );

	remove( "gmsvlog.err2");
	rename( "gmsvlog.err1", "gmsvlog.err2" );
	rename( "gmsvlog.err", "gmsvlog.err1" );
	rename( "gmsvlog", "gmsvlog.err" );
#if USE_MTIO
    {
        void MTIO_join(void);
        MTIO_join();
    }
#endif
    allDataDump();


    signal( SIGINT , SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGKILL, SIG_IGN );
    signal( SIGSEGV, SIG_IGN );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, SIG_IGN );
		signal( SIGALRM, SIG_IGN );

    shutdownProgram();

    exit(number);
}

extern jztimeout;

void jztime_out( int number )
{
	print("��ʱ������Ч");
	jztimeout=TRUE;
}
void signalset( void )
{
    // CoolFish: Test Signal 2001/10/26
    print("\nCoolFish Get Signal..\n");

	print("SIGINT:%d\n", SIGINT);
	print("SIGQUIT:%d\n", SIGQUIT);
	print("SIGKILL:%d\n", SIGKILL);
	print("SIGSEGV:%d\n", SIGSEGV);
	print("SIGPIPE:%d\n", SIGPIPE);
	print("SIGTERM:%d\n", SIGTERM);
  print("SIGALRM:%d\n", SIGALRM);

    signal( SIGINT , sigshutdown );  //�ն��жϷ�
    signal( SIGQUIT, sigshutdown );	 //�ն��˳���
    signal( SIGKILL, sigshutdown );	 //��ֹ
    signal( SIGSEGV, sigshutdown );	 //��Ч�������
    signal( SIGPIPE, SIG_IGN );			 //д���޶����̵Ĺܵ�
    signal( SIGTERM, sigshutdown );	 //��ֹ
    signal( SIGALRM, jztime_out  );	 //��ʱ
}

// WaitServer--Client - this is the client - version 0.0

#define DOS_SPEC
#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tysockio.h"

#ifdef DOS_SPEC
#include <winsock.h>
#include <io.h>
#include <fcntl.h>
#else
#error XXX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif DOS_SPEC

int main (int argc, char *argv[])
{
#ifdef DOS_SPEC
// INIT-STUFF (WIN32 specific)
// Startup DLL
WORD wVersionRequested;
WSADATA wsaData;
int err;
wVersionRequested = MAKEWORD( 2, 0 );
err = WSAStartup( wVersionRequested, &wsaData );
if ( err != 0 ) {
	 /* Tell the user that we couldn't find a usable */
	 /* WinSock DLL.                                  */
	 return 1;
}
/* Confirm that the WinSock DLL supports 2.0.*/
/* Note that if the DLL supports versions greater    */
/* than 2.0 in addition to 2.0, it will still return */
/* 2.0 in wVersion since that is the version we      */
/* requested.                                        */
if ( LOBYTE( wsaData.wVersion ) != 2 ||
		  HIBYTE( wsaData.wVersion ) != 0 ) {
	 /* Tell the user that we couldn't find a usable */
	 /* WinSock DLL.                                  */
	 WSACleanup( );
	 return 2;
}
// END-STUFF
#endif DOS_SPEC

// START HERE !!! !!! !!! !!! !!! !!!

struct hostent *hp;
struct sockaddr_in para;
char host[80];
int serverPort = 123;
int sckfd;

bool doTerminateServer;

strcpy(host,"fuji");
if ( argc > 1 ) {
	strcpy(host,argv[1]);
}

doTerminateServer = (argc > 2 && argv[2][0]);

hp = gethostbyname( host );
if ( hp==NULL ) {
	fprintf(stderr,"Server unknown: %s\n",host);
	return -1;
}
memcpy( &para.sin_addr, hp->h_addr, hp->h_length );
para.sin_family = AF_INET;
para.sin_port = htons( serverPort );

sckfd = socket( AF_INET, SOCK_STREAM, 0 );
if ( sckfd < 0 ) {
	fprintf(stderr,"Creating socket");
	return -1;
}

if ( connect(sckfd,(struct sockaddr*)&para,sizeof(para)) < 0 ) {
	fprintf(stderr,"Cannot connect to %s\n",host);
	return -1;
}

SocketIO K(sckfd);
if ( doTerminateServer ) {
	fprintf(stderr,"doTerminateServer\n");
	K.write('-',"Shake-hand");
	shutdown( sckfd, 2 );
	return 1;
}

K.write('W',"Shake-hand");

if ( K.HasError()==false ) {
	unsigned char chrY;
	bool x;
	x = K.read(&chrY,"y-read");
	printf("%c: OK %d!!!\n",chrY,(int)x);
}

SkUserId skUserInfo(4660);	// =0x1234
K.write(skUserInfo.StreamOut(),skUserInfo.Size(),"User-info");

puts("BYE!");

// Shut down nicely
shutdown( sckfd, 2 );
return 0;
}


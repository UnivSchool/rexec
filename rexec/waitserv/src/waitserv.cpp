// WaitServer - version 0.0

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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif DOS_SPEC

#define LISTENQUEUESIZE 5

// Proxy-IP address: 192.138.158.131 = 3263864451UL
// ----------------: 010.000.000.001 = 167772161UL
#define PROXY_FTP_PORT 21	/*not used here*/

char* errorMsg="You are not allowed to use this service.";

int serverSocketId;

int checkClient (SkInIPAddress& clientIP)
{
#ifdef DEBUG
	printf("checkClient:%lu\n",clientIP.getIP());
#endif
return clientIP.getIP()!=0;
}

int manageConnection (int socketId)
{
SocketIO K(socketId);
unsigned char shakeHand;

if ( K.read(&shakeHand,"Shake-hand")==false ) return 0;
if ( shakeHand!='W' ) {
	return K.write('x',"Shake-hand-unsuccessful") ? -1 : -2;
}
if ( K.write('y',"y-sent")==false ) return 1;
// Handshake done, now let's read the user information
SkUserId skUserInfo;
if ( K.read( skUserInfo.StreamIn(), skUserInfo.Size(), "User-information" )==false ) return 2;
skUserInfo.GetStream();
skUserInfo.Show();
return 0;
}

int main (int argc, char *argv[])
{
   struct sockaddr_in serverAddressData;
   int serverPort;

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

   serverPort = 123;

   if ( (serverSocketId = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
   {
      perror("server: can't open stream socket");
      return 1;
   }

   memset((char*)&serverAddressData,0,sizeof(serverAddressData));
   serverAddressData.sin_family = AF_INET;
   serverAddressData.sin_addr.s_addr = htonl(INADDR_ANY);
   serverAddressData.sin_port = htons(serverPort);
#ifdef DEBUG
	printf("Using server port %u [%u]\n",serverPort,serverAddressData.sin_port);
#endif

   int serverSocketOptional = 1;
   setsockopt(serverSocketId, SOL_SOCKET, SO_REUSEADDR,(char*)&serverSocketOptional, sizeof(int));

   if ( bind(serverSocketId,(struct sockaddr*)&serverAddressData,sizeof(serverAddressData)) < 0 )
   {
      perror("server: can't bind local address");
      return 1;
   }

   if ( listen(serverSocketId,LISTENQUEUESIZE) )
   {
      return 1;
   }

#ifdef DEBUG
	printf("Listening socket %d\n",serverSocketId);
#endif
   struct sockaddr_in clientAddressData;
   int clientAddressDataLength = sizeof(clientAddressData);
   int clientSocketId;
   int manageError=0;

   while ( manageError>=0 )
   {
#ifdef DEBUG
	printf("Waiting for 'accept'\n");
#endif
      clientSocketId = accept(serverSocketId,(struct sockaddr*)&clientAddressData,&clientAddressDataLength);

      SkInIPAddress clientIPaddress(
	clientAddressData.sin_addr.S_un.S_un_b.s_b1,
	clientAddressData.sin_addr.S_un.S_un_b.s_b2,
	clientAddressData.sin_addr.S_un.S_un_b.s_b3,
	clientAddressData.sin_addr.S_un.S_un_b.s_b4);
#ifdef DEBUG
      printf("accept OK for client (%d.%d.%d.%d)\n",
	       clientAddressData.sin_addr.S_un.S_un_b.s_b1,
	       clientAddressData.sin_addr.S_un.S_un_b.s_b2,
	       clientAddressData.sin_addr.S_un.S_un_b.s_b3,
	       clientAddressData.sin_addr.S_un.S_un_b.s_b4);
#endif

      if ( checkClient(clientIPaddress)==0 )
      {
	 write(clientSocketId,errorMsg,strlen(errorMsg));
	 close(clientSocketId);
	 continue;
      }

      if (clientSocketId < 0) continue;

      manageError = manageConnection(clientSocketId);
      if ( manageError > 0 ) fprintf(stderr,"manageError error=%d\n",manageError);
      close(clientSocketId);
   }// WHILE infinite

   fprintf(stderr,"SERVER STOP: manageError ended with error code %d\n",manageError);
   return manageError;
}


/* lserver - Linear-Shell-Server, Version 1.0
*/

#include <errno.h>
#include "macro.h"
#include "targ.h"
#include "tysockio.h"

#include "main.h"

#define FNAME "lserver"

#define DOS_SPEC
#define DEBUG

#ifdef DOS_SPEC
#include <winsock.h>
#include <io.h>
//#include <fcntl.h>
#define LISTENQUEUESIZE 5
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif DOS_SPEC

int macroAll0;

#define LSERVER_USER_TBL_SIZE	100

static t_uchar UserPtyTbl[LSERVER_USER_TBL_SIZE];

int go (SocketServerIO& K, SocketIO& clientK)
{
static bool isOk;

static struct sockaddr_in clientAddressData;
static int clientAddressDataLength = sizeof(clientAddressData);
static int clientSocketId=0;

static t_uchar aPty;
static unsigned i, count;

#ifdef DEBUG
	printf("Waiting for accept (last client socket was %d)\n",clientSocketId);
#endif

clientSocketId =
	accept(
		K.GetSocket(),
		(struct sockaddr*)&clientAddressData,
		&clientAddressDataLength );

SkInIPAddress clientIPaddress(
	clientAddressData.sin_addr.S_un.S_un_b.s_b1,
	clientAddressData.sin_addr.S_un.S_un_b.s_b2,
	clientAddressData.sin_addr.S_un.S_un_b.s_b3,
	clientAddressData.sin_addr.S_un.S_un_b.s_b4);

clientK.SetSocket(clientSocketId);

#ifdef DEBUG
	printf("Accepted client %ld (socket %d)\n",clientIPaddress.getIP(),clientSocketId);
#endif

SkUserId skUserInfo;

isOk = clientK.read(skUserInfo.StreamIn(),skUserInfo.Size(),"User-information");
skUserInfo.GetStream();
skUserInfo.Show();

#ifdef DEBUG
	printf("User %d\t",skUserInfo.getInt());
	if ( skUserInfo.getInt()==0 ) {
		return 0; // quit server!
	}
#endif

aPty = UserPtyTbl[(unsigned)skUserInfo.getInt() % LSERVER_USER_TBL_SIZE];

SkInt16 skRequest;
isOk = clientK.read(skRequest.StreamIn(),skRequest.Size(),"Request");
skRequest.GetStream();

#ifdef DEBUG
	printf("Request %d\n",skRequest.getInt());
#endif

if  ( skRequest.getInt()!=1 ) {
	if ( aPty==0 ) {
		printf("Request of user %d refused by server\n",skUserInfo.getInt());
		SkRefuseRequest skRefuse;
		clientK.write(skRefuse.StreamOut(),skRefuse.Size(),"Refuse-Request");
	}
}

switch ( skRequest.getInt() ) {
case 1: {
	aPty = ++UserPtyTbl[(unsigned)skUserInfo.getInt() % LSERVER_USER_TBL_SIZE];
	SkInt16 skPty((tsk_int16)aPty);
	isOk = clientK.write(skPty.StreamOut(),skPty.Size(),"Pty");
	break;
}
case 2: {
	for (i=0, count=0; i<LSERVER_USER_TBL_SIZE; i++)
		count += UserPtyTbl[i];
	SkUInt16 skCountUsedPty((tsk_uint16)count);
	isOk = clientK.write(skCountUsedPty.StreamOut(),skCountUsedPty.Size(),"CountUsedPty16");
	break;
}
case 3: {
	for (i=0, count=0; i<LSERVER_USER_TBL_SIZE; i++)
		count += UserPtyTbl[i];
	SkUInt32 skCountUsedPty((tsk_uint32)count);
	isOk = clientK.write(skCountUsedPty.StreamOut(),skCountUsedPty.Size(),"CountUsedPty32");
	break;
}
default:
	printf("Unknown request %d\n",skRequest.getInt());
	break;
};

return isOk ? 1 : 0;
}

main (int argc, char *argv[], char **envp) {
 TShortBuffer hostName;
 int portNr=1234;
 char* s;
 char* msgHelp = "lserver - Linear-Shell-Server, Version 1.0\n\
\n\
Usage:\n\
	lserver\n\
\n\
Options are:\n\
	-port<PortNumber>";

 BEGIN_LOG;
 TArg arg(argc,argv,envp);

 if ( arg.Help() ) {
	puts(msgHelp);
//	return 0;
 }

 TyListStr O;
 int i;
 for (i=1; i<=arg.N(); i++) {
	s = arg.Str(i);
	if ( s[0]=='-' ) O.Add(s); else break;
 }

 TyListStr W;
 for (; i<=arg.N(); i++) {
	s = arg.Str(i);
	W.Add(s);
 }

 hostName.Set( arg.GetEnv("HOST") );
 if ( hostName.Empty() ) hostName.Set("fuji");

 i = O.Find("port",2);
 if ( i ) {
	portNr = atoi( O.Str(i)+5 );
 }

 SocketServerIO socketService;
 socketService.Connect(hostName.Str(),portNr);
 if ( socketService.IsConnected() ) {
	fprintf(stderr,"Already one server running on %s\n",socketService.GetHostName());
	return -1;
 }

   static struct sockaddr_in serverAddressData;
   memset((char*)&serverAddressData,0,sizeof(serverAddressData));
   serverAddressData.sin_family = AF_INET;
   serverAddressData.sin_addr.s_addr = htonl(INADDR_ANY);
   serverAddressData.sin_port = htons(portNr);
#ifdef DEBUG
	// unsigned serverAddressData.sin_port
	printf("Using server port %u\n",portNr);
#endif

   int serverSocketOptional = 1;
   setsockopt(socketService.GetSocket(), SOL_SOCKET, SO_REUSEADDR,(char*)&serverSocketOptional, sizeof(int));

   if ( bind(socketService.GetSocket(),(struct sockaddr*)&serverAddressData,sizeof(serverAddressData)) < 0 )
   {
      perror("server: can't bind local address");
      return 1;
   }

   if ( listen(socketService.GetSocket(),LISTENQUEUESIZE) )
   {
      return 1;
   }
#ifdef DEBUG
	printf("Listening socket %d\n",socketService.GetSocket());
#endif

 SocketIO clientK(stderr);

 while ( go(socketService,clientK)!=0 ) ;

 END_LOG;
 return 0;
}


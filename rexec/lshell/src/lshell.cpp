/* lshell - Local-Shell, Version 0.0
*/

//#include <dir.h>
#include "macro.h"
#include "targ.h"
#include "tylshell.h"
#include "tysockio.h"

#include "main.h"

#define FNAME "lshell"

int macroAll0;

int go (TyLShell& lShell, TyListStr& W, TyListStr& O)
{
static unsigned i;

// Parse argument
return -1;
}

main (int argc, char *argv[], char **envp) {
 TShortBuffer hostName;
 int portNr=1234;
 int userId = 12;
 char* s;
 char* msgHelp = "lshell - Local-Shell, Version 0.0\n\
\n\
Usage:\n\
	lshell [hostName] userName\n\
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
	s = O.Str(i)+5;
	portNr = atoi( s );
 }

 if ( W.N()>=1 ) {
	userId = atoi( W.Str(1) );
 }

 // Create directory as necessary
 TShortBuffer userCfgDir, defaultUserCfgDir;
 sprintf(defaultUserCfgDir.Str(),"%s%ccfg_%04X%c",
	arg.ProgramCallPath(),
	dirSlash,
	0,
	dirSlash);
 sprintf(userCfgDir.Str(),"%s%ccfg_%04X%c",
	arg.ProgramCallPath(),
	dirSlash,
	(unsigned)userId,
	dirSlash);
 TRACE(macroAll0!=0,userCfgDir.Str(),"=userCfgDir");
// mkdir( userCfgDir.Str() );

 SocketIO socketService;
 socketService.Connect(hostName.Str(),portNr);
 if ( socketService.IsConnected()==false ) {
	fprintf(stderr,"Cannot connect to %s\n",socketService.GetHostName());
 }

 TyLShell lShell(&socketService,userId,(t_uchar*)defaultUserCfgDir.Str());

 while ( (i = go(lShell,W,O))>=0 ) ;

 END_LOG;
 return i;
}


/* completion - (Unix-'where') TAB completion on DOS, Version 0.0
This utility only writes on the 'tab-cache' !
*/

#include "macro.h"
#include "targ.h"
#include "tdirw32.h"

#include "main.h"

#define FNAME "compl"

int processPath (TyListStr& O, char* pathStr, char* wildCard)
{
if ( pathStr==0 ) return 0;
TShortBuffer path(pathStr);
if ( path[path.Length()]!='\\' ) path.Add('\\');
path.Add(wildCard);
fprintf(stderr,"Processing path %s\n",path.Str());
TDirW32 d;
d.Dir(path.Str());
/*TyOlFilename* entriesPtr = &(d.Entries());
entriesPtr->Show();*/
return 1;
}

int process (char* str, TyListStr& O, TyPathList& pl)
{
int i, countFiles=0;
TShortBuffer s(str);
if ( s.IsEqual("*") ) {
	for (i=1; i<=pl.Dim(); i++)
		countFiles += processPath(O,pl.Str(i),"*.*");
	return countFiles;
}
return countFiles;
}

int go (TyListStr& W, TyListStr& O, TyPathList& pl)
{
int i, countFiles=0;
for (i=1; i<=W.Dim(); i++) {
	countFiles += process(W.Str(i),O,pl);
}
return countFiles;
}

main (int argc, char *argv[], char **envp) {
 char *s = NULL;
 char *msgHelp = "completion - (Unix-'where') TAB completion on DOS, Version 0.0\n\
\n\
Usage:\n\
	compl [options] tag [tag ...]\n\n\
Tag is * or a specific path. * means all paths.\n\
\n\
Options are:\n\
	none.";

 BEGIN_LOG;
 TArg arg(argc,argv,envp);

 if ( arg.Help() ) {
	puts(msgHelp);
	return 0;
 }

 TyListStr O;
 int i;
 for (i=1; i<=arg.N(); i++) {
	s = arg.Str(i);
	if ( s[0]=='-' ) O.Add(s); else break;
 }

 TyListStr W;
 int wIdx;
 for (wIdx=1; i<=arg.N(); i++, wIdx) {
	s = arg.Str(i);
	W.Add(s);
 }

 TyPathList pl( arg.GetEnv("PATH") );

 int nFiles = go(W,O,pl);
 fprintf(stderr,"Processed %d files\n",nFiles);

 END_LOG;
 return nFiles;
}

/* completion - (Unix-'where') TAB completion on Win32, Version 0.6
This utility only writes on the 'tab-cache' !
*/

#include "macro.h"
#include "targ.h"
#include "tdirw32.h"

#include "main.h"

#define FNAME "compl"

static TyListStr myExtAsDosExec;

// Usefull
int AddExtensionGivenString (TyListStr& L, char* str)
{
int countExtensionOnString = 0, len;
if ( str==0 ) return -1;
TShortBuffer s(str), sAux;
TShortBuffer thisExtStr;
while ( sscanf(s.Str(),"%s",thisExtStr.Str())==1 ) {
	len = thisExtStr.Length();
	sAux.Set( s.Str()+len+1 );
	s = sAux;
	if ( len >= 2 ) {
	L.Add( thisExtStr.Str() );
	countExtensionOnString++;
	}
}
return countExtensionOnString;
}

// Core...
bool isDOS_Executable (char* filename, TyListStr& extAsDosExec)
{
TShortBuffer name(filename);
unsigned i, n=extAsDosExec.N(), len = name.Length(), thisExtLen;
char* s;
if ( filename==0 ) return false;
if ( len < 5 ) return false;
// Eg: X.COM (len=5) -> FindIC(".COM") = 2 (+3=len);
for (i=1; i<=n; i++) {
	s = extAsDosExec.Str(i);
	thisExtLen = strlen(s);
	if ( thisExtLen < 2 ) continue;
	if ( name.FindIC(s)+thisExtLen-1==len )
		return true;
}
return false;
}

//TyOlFilename* entriesPtr = &(d.Entries());

int processFilesInPath (TShortBuffer& fullPathName, TyOlFilename& fileNames, bool isOneFilePerLine)
{
int i, n=fileNames.N(), countFiles=0;
char* filename;
bool isOk;

fprintf(stdout,"%s%c",
	fullPathName.Str(),
	isOneFilePerLine ? '\n' : '\t');

for (i=1; i<=n; i++) {
	isOk = isDOS_Executable( filename = fileNames.Str(i), myExtAsDosExec );
	countFiles += isOk;
	if ( isOk ) {
		fprintf(stdout,"%s%c",
			filename,
			isOneFilePerLine ? '\n' : '\t');
	}
}
if ( isOneFilePerLine==false ) fprintf(stdout,"\n");
return countFiles;
}

int processPath (TyListStr& O, char* pathStr, char* wildCard)
{
int countFiles;
bool isOneFilePerLine = O.Find("v",2)>0;
if ( pathStr==0 ) return 0;
TShortBuffer path(pathStr);
if ( path[path.Length()]!='\\' ) path.Add('\\');
TShortBuffer fullPathName(path);
path.Add(wildCard);
fprintf(stderr,"Processing files in %s\n",path.Str());
TDirW32 d;
d.Dir( path.Str() );
if ( O.Find("n",2)==0 ) {
	fprintf(stdout,"(%d)\t",d.Entries().N());
}
countFiles = processFilesInPath( fullPathName, d.Entries(), isOneFilePerLine );
return countFiles;
}

int process (char* str, TyListStr& O, TyPathList& pl)
{
int i, countFiles=0;
TShortBuffer s(str);
if ( s.IsEqual("*") ) {
	for (i=1; i<=pl.Dim(); i++)
		countFiles += processPath(O,pl.Str(i),"*.*");
}
else {
	countFiles += processPath(O,str,"*.*");
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
 char *msgHelp = "completion - (Unix-'where') TAB completion on DOS, Version 0.6\n\
\n\
Usage:\n\
	compl [-<options>] tag [tag ...]\n\n\
Tag is * or a specific path. * means all paths.\n\
\n\
Options are:\n\
	a<X>	Add extensions <X> to DOS executables\n\
	e<X>	Reset extensions to <X>\n\
	n	Supress number of files in each directory\n\
	v	View one file per line\n\
	x	View extensions\n\
	t<path>	test path\n\
\n\
Example 1:\n\
	compl -e\".ini *.dat\" -v c:\\windows\\\n\
Example 2:\n\
	compl -a.ocx *";

 BEGIN_LOG;
 TArg arg(argc,argv,envp);

 if ( arg.Help() ) {
	puts(msgHelp);
	return 0;
 }

 myExtAsDosExec.Add(".EXE");
 myExtAsDosExec.Add(".COM");
 myExtAsDosExec.Add(".BAT");
 myExtAsDosExec.Add(".LNK");

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

 unsigned pos = O.Find("t",2);
 if ( pos > 0 ) {
	W.Add( (char*)(O.Str(pos)+2) );
	fprintf(stderr,"Test\t%s\n",W.Str(1));
 }

 pos = O.Find("e",2);
 if ( pos > 0 ) {
	myExtAsDosExec.Clear();
	s = (O.Str(pos)+2);
	AddExtensionGivenString(myExtAsDosExec,s);
 }

 pos = O.Find("a",2);
 if ( pos > 0 ) {
	s = (O.Str(pos)+2);
	AddExtensionGivenString(myExtAsDosExec,s);
 }

 pos = O.Find("x",2);
 if ( pos > 0 ) {
	fprintf(stderr,"Extensions:\t");
	for (i=1; i<=myExtAsDosExec.N(); i++)
		fprintf(stderr,"%s;",myExtAsDosExec.Str(i));
	fprintf(stderr,"\n");
 }

 TyPathList pl( arg.GetEnv("PATH") );

 int nFiles = go(W,O,pl);
 fprintf(stderr,"Processed %d files\n",nFiles);

 END_LOG;
 return nFiles;
}


/* whatkind - kind of executable files, Version 0.0
*/

#include "macro.h"
#include "targ.h"
#include "tfile.h"

#include "main.h"

#define FNAME "whatkind"

#define IS_COM_CHR00	0x8C

#define IS_EXE_CHR00	'M'
#define IS_EXE_CHR01	'Z'

int processFile (char* filename, TyListStr& O)
{
TFileBinStream f(2048);
FILE* fReport = stderr;
bool isDumpFirst256b = O.Find("d",2);
bool isSupressDetailReport = O.Find("s",2);
bool thisIsConclusive = false;

if ( f.OpenToRead(filename)==false ) {
	fprintf(fReport,"Could not open %s\n",filename);
	return -1;
}
if ( f.Read(1024)==false ) {
	fprintf(fReport,"Could not read one line of file %s\n",filename);
	return -1;
}

int i;
static unsigned statChr[256], statChrLT32, statChrGT127, pos13=0;
unsigned char* ptr = f.Buffer();
unsigned uChr;

printf("%-80s",filename);

for (i=0; i<1024; i++) statChr[(unsigned)ptr[i]]++;

if ( isDumpFirst256b ) {
	for (i=0; i<256; i++) printf("[%02X]",(unsigned)ptr[i]);
	printf("\n");
}

for (i=0, statChrLT32=0; i<' '; i++)
	if ( i>0 && i!='\t' && i!='\r' && i!='\n' ) statChrLT32 += statChr[i];
for (i=128, statChrGT127=0; i<256; i++)
	statChrGT127 += statChr[i];

if ( isSupressDetailReport==false )
	printf("\\0=%u, Weird=%u, 8Bit=%u, \\r=%d\n",
		statChr[0],statChrLT32,statChrGT127,statChr['\r']);

if ( statChr['\r'] > 0 ) {
	for (i=0; i<1024 && ((unsigned)ptr[i])!='\r'; i++) ;
	pos13 = i;
	if ( ptr[i+1]=='\n' || ptr[i+2]=='\n' ) {
		// Find the string before \r\n
		i--;
		for ( ; i>0 && (uChr=(unsigned)ptr[i])>=' ' && uChr<='z' && uChr!='@'; i--) ;
		i++;
		TShortBuffer s((char*)(ptr+i));
		if ( pos13-i > 10 ) {
		thisIsConclusive = true;
		s.SetSize(pos13-i);
		if ( s.Find("Win")>0 ) {
			printf("@Windows\t");
		}
		else {
			if ( s.Find("not")>0 && s.Find("DOS mode")>0 )
				printf("@Windows\t");
			else
				printf("@DOS\t");
		}
		if ( isSupressDetailReport==false ) printf("<%s>",s.Str());
		}// IF pos13-i > 10
	}
}
if ( thisIsConclusive==false ) {
	uChr = ptr[0];
	if ( uChr==IS_COM_CHR00	) printf("@DOS-Com\t");
	else
	if ( uChr==IS_EXE_CHR00 && ptr[1]==IS_EXE_CHR01 ) printf("@DOS-Exe\t");
	else
	printf("@Unknown\t");
}
printf("\n");
return 0;
}

int go (TyListStr& W, TyListStr& O)
{
int i, countFiles=0;
for (i=1; i<=W.Dim(); i++) {
	countFiles += processFile(W.Str(i),O)==0;
}
return countFiles;
}

main (int argc, char *argv[], char **envp) {
 char *s = NULL;
 char *msgHelp = "whatkind - kind of executable files, Version 0.0\n\
\n\
Usage:\n\
	whatkind [options] [file [...file]]\n\n\
A wfile may NOT have wild cards.\n\
Options are:\n\
	d	Dump first 256 bytes\n\
	r	Supress detailed report";

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

 int nFiles = go(W,O);
 if ( nFiles )
	fprintf(stderr,"%u file%s processed\n",nFiles,nFiles!=1?"s":"\0");

 END_LOG;
 return 0;
}

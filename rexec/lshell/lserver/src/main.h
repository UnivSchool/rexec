#ifdef NOTRACE_VERSION

#error Sure?
#define BEGIN_LOG ;
#define END_LOG ;

#else

FILE *f_MacrOut;
char* f_MacrOut_filename;

#define LOG_PATHNAME "c:\\$s.log"

#define BEGIN_LOG f_MacrOut=fopen(f_MacrOut_filename=LOG_PATHNAME,"wt");
#define END_LOG fclose(f_MacrOut);

#define PARSER_DEBUG

#endif NOTRACE_VERSION

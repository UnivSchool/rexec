#ifndef STDTYPES_H
#define STDTYPES_H

#undef DO_DEFINE_BOOL
//#define DO_DEFINE_BOOL  // Comment if using BorlandC > 3.11

#ifdef LINT_AS_INT
#error
#else
typedef long lint;
#endif LINT_AS_INT

#define MAX_UINT16  0xFFFF       // Win32 WORD
#define MAX_UINT32  0xFFFFFFFF   // Win32 DWORD
#define MAX_UINT32L 4294967295L  // same in decimal_L
#define MAX_INT32   0x7FFFFFFF
#define MAX_INT32L  2147483647L  // same in decimal_L

#define MAX_A_UINT  MAX_UINT16
#define MAX_A_LUINT MAX_UINT32

#define TUNSIGNED16BIT_MAXVAL	65535
#define TUNSIGNED16BIT_MAXVALl	((unsigned long)65535)
#define TUNSIGNED24BIT_MAXVALl	((unsigned long)16777215)
#define TUNSIGNED32BIT_MAXVALl	((unsigned long)4294967295)

#ifndef dirSlash
#define dirSlash '\\'
#endif dirSlash
#ifndef dirSlashStr
#define dirSlashStr "\\"
#endif dirSlashStr

#define chrQuote 39
// Quotation mark is \'
#define chrDQuote '"'

#ifdef DO_DEFINE_BOOL
typedef char bool;
#define false 0
#define true 1
#endif
typedef unsigned char t_uchar;
typedef unsigned t_ioError;

typedef t_uchar t_ColorPart;

// 16 bit microprocessor specific
//typedef unsigned t_uint16;
//typedef int t_int16;
//typedef unsigned long t_uint32;
//typedef long t_int32;

// 32 bit microprocessor specific
typedef unsigned short t_uint16;
typedef short t_int16;
typedef unsigned long t_uint32;
typedef long t_int32;

typedef unsigned long t_fileSize;
typedef unsigned long t_fileLines;
typedef unsigned long t_timeStamp;
#define MAX_FILESIZE_fs		((t_fileSize)MAX_A_LUINT)
#define MAX_TIMESTAMP_ts	((t_timeStamp)MAX_A_LUINT)

#endif STDTYPES_H

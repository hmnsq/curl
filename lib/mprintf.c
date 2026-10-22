/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#include "curl_setup.h"

#include "curlx/dynbuf.h"
#include "curl_printf.h"
#include "curlx/strparse.h"
#include "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
const unsigned char Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
const unsigned char Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  do {                                    \
    如果(stream((unsigned char)(x), userp)) \
      返回 TRUE;                        \
    (*donep)++;                           \
  } 当(0)

/* Data type to read from the arglist */
typedef enum {
  MTYPE_STRING,
  MTYPE_PTR,
  MTYPE_INTPTR,
  MTYPE_INT,
  MTYPE_LONG,
  MTYPE_LONGLONG,
  MTYPE_INTU,
  MTYPE_LONGU,
  MTYPE_LONGLONGU,
  MTYPE_DOUBLE,
  MTYPE_LONGDOUBLE,
  MTYPE_WIDTH,
  MTYPE_PRECISION
} FormatType;

/* conversion and display flags */
enum {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_LEFT       = 1 << 2,
  FLAGS_ALT        = 1 << 3,
  FLAGS_SHORT      = 1 << 4,
  FLAGS_LONG       = 1 << 5,
  FLAGS_LONGLONG   = 1 << 6,
  FLAGS_LONGDOUBLE = 1 << 7,
  FLAGS_PAD_NIL    = 1 << 8,
  FLAGS_UNSIGNED   = 1 << 9,
  FLAGS_OCTAL      = 1 << 10,
  FLAGS_HEX        = 1 << 11,
  FLAGS_UPPER      = 1 << 12,
  FLAGS_WIDTH      = 1 << 13, /* '*' or '*<num>$' used */
  FLAGS_WIDTHPARAM = 1 << 14, /* width PARAMETER was specified */
  FLAGS_PREC       = 1 << 15, /* precision was specified */
  FLAGS_PRECPARAM  = 1 << 16, /* precision PARAMETER was specified */
  FLAGS_CHAR       = 1 << 17, /* %c story */
  FLAGS_FLOATE     = 1 << 18, /* %e or %E */
  FLAGS_FLOATG     = 1 << 19, /* %g or %G */
  FLAGS_SUBSTR     = 1 << 20  /* no input, only substring */
};

enum {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
struct va_input {
  FormatType type; /* FormatType */
  union {
    const char *str;
    void *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    double dnum;
  } val;
};

/*
 * Describes an output segment.
 */
struct outsegment {
  整数 width;     /* width OR width parameter number */
  整数 precision; /* precision OR precision parameter number */
  unsigned 整数 flags;
  unsigned 整数 input; /* input argument array index */
  const char *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

struct nsprintf {
  char *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

struct asprintf {
  struct dynbuf *b;
  char merr;
  size_t nstage;
  unsigned char stage[ASPRINTF_STAGE_SIZE];
};

struct ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
static 整数 dollarstring(const char *p, const char **end)
{
  curl_off_t num;
  如果(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    返回 -1;
  *end = p;
  返回 (整数)num - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned char)(1 << ((y) & 7)))

/*
 * Parse the format string.
 *
 * Create two arrays. One describes the inputs, one describes the outputs.
 *
 * Returns zero on success.
 */

#define PFMT_OK          0
#define PFMT_DOLLAR      1 /* bad dollar for main param */
#define PFMT_DOLLARWIDTH 2 /* bad dollar use for width */
#define PFMT_DOLLARPREC  3 /* bad dollar use for precision */
#define PFMT_MANYARGS    4 /* too many input arguments used */
#define PFMT_PREC        5 /* precision overflow */
#define PFMT_PRECMIX     6 /* bad mix of precision specifiers */
#define PFMT_WIDTH       7 /* width overflow */
#define PFMT_INPUTGAP    8 /* gap in arguments */
#define PFMT_WIDTHARG    9 /* attempted to use same arg twice, for width */
#define PFMT_PRECARG    10 /* attempted to use same arg twice, for prec */
#define PFMT_MANYSEGS   11 /* maxed out output segments */

static 整数 parse_flags(const char **fmtp, unsigned 整数 *flagsp, 整数 use_dollar,
                       整数 *precp, 整数 *widthp)
{
  const char *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned 整数 flags = 0;
  整数 width = 0;
  整数 precision = 0;

  /* Handle the flags */
  do {
    switch(*fmt++) {
    case ' ':
      flags |= FLAGS_SPACE;
      break;
    case '+':
      flags |= FLAGS_SHOWSIGN;
      break;
    case '-':
      flags |= FLAGS_LEFT;
      flags &= ~(unsigned 整数)FLAGS_PAD_NIL;
      break;
    case '#':
      flags |= FLAGS_ALT;
      break;
    case '.':
      如果('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        如果(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          如果(precision < 0)
            /* illegal combo */
            返回 PFMT_DOLLARPREC;
        }
        否则
          /* get it from the next argument */
          precision = -1;
      }
      否则 {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        如果(is_neg)
          fmt++;
        如果(curlx_str_number(&fmt, &num, INT_MAX))
          返回 PFMT_PREC;
        precision = (整数)num;
        如果(is_neg)
          precision = -precision;
      }
      如果((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        返回 PFMT_PRECMIX;
      break;
    case 'h':
      flags |= FLAGS_SHORT;
      break;
#ifdef _WIN32
    case 'I':
      /* Non-ANSI integer extensions I32 I64 */
      如果((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      否则 如果((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      否则 {
#如果 SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#否则
        flags |= FLAGS_LONG;
#endif
      }
      break;
#endif /* _WIN32 */
    case 'l':
      如果(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      否则
        flags |= FLAGS_LONG;
      break;
    case 'L':
      flags |= FLAGS_LONGDOUBLE;
      break;
    case 'q':
      flags |= FLAGS_LONGLONG;
      break;
    case 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#如果 SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#否则
      flags |= FLAGS_LONG;
#endif
      break;
    case 'O':
#如果 SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#否则
      flags |= FLAGS_LONG;
#endif
      break;
    case '0':
      如果(!(flags & FLAGS_LEFT))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      curl_off_t num;
      flags |= FLAGS_WIDTH;
      fmt--;
      如果(curlx_str_number(&fmt, &num, INT_MAX))
        返回 PFMT_WIDTH;
      width = (整数)num;
      break;
    }
    case '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      如果(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        如果(width < 0)
          /* illegal combo */
          返回 PFMT_DOLLARWIDTH;
      }
      否则
        /* pick from the next argument */
        width = -1;
      break;
    default:
      loopit = FALSE;
      fmt--;
      break;
    } /* switch */
  } 当(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  返回 PFMT_OK;
}

static bool parse_conversion(const char f, unsigned 整数 *flagp,
                             FormatType *typep)
{
  unsigned 整数 flags = *flagp;
  FormatType type;
  switch(f) {
  case 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    break;
  case 's':
    type = MTYPE_STRING;
    break;
  case 'n':
    type = MTYPE_INTPTR;
    break;
  case 'p':
    type = MTYPE_PTR;
    break;
  case 'd':
  case 'i':
    如果(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    否则 如果(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    否则
      type = MTYPE_INT;
    break;
  case 'u':
    如果(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    否则 如果(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    否则
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    break;
  case 'o':
    如果(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    否则 如果(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    否则
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    break;
  case 'x':
    如果(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    否则 如果(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    否则
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    break;
  case 'X':
    如果(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    否则 如果(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    否则
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    break;
  case 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    break;
  case 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    break;
  case 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    break;
  case 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    break;
  case 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    break;
  case 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    break;
  case 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    break;
  default:
    /* invalid instruction, disregard and continue */
    返回 TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  返回 FALSE;
}

static 整数 parsefmt(const char *format,
                    struct outsegment *out,
                    struct va_input *in,
                    整数 *opieces,
                    整数 *ipieces, va_list arglist)
{
  const char *fmt = format;
  整数 param_num = 0;
  整数 max_param = -1;
  整数 i;
  整数 ocount = 0;
  unsigned char usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  struct outsegment *optr;
  整数 use_dollar = DOLLAR_UNKNOWN;
  const char *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, sizeof(usedinput));

  当(*fmt) {
    如果(*fmt == '%') {
      struct va_input *iptr;
      FormatType type;
      unsigned 整数 flags = 0;
      整数 width = 0;
      整数 precision = 0;
      整数 param = -1;
      整数 rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      如果(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        如果(outlen) {
          optr = &out[ocount++];
          如果(ocount > MAX_SEGMENTS)
            返回 PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      如果(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        如果(param < 0) {
          如果(use_dollar == DOLLAR_USE)
            /* illegal combo */
            返回 PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        否则
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      如果(rc)
        返回 rc;

      如果(parse_conversion(*fmt, &flags, &type))
        continue;

      如果(flags & FLAGS_WIDTHPARAM) {
        如果(width < 0)
          width = param_num++;
        否则 {
          /* if this identifies a parameter already used, this is illegal */
          如果(is_arg_used(usedinput, width))
            返回 PFMT_WIDTHARG;
        }
        如果(width >= MAX_PARAMETERS)
          返回 PFMT_MANYARGS;
        如果(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      如果(flags & FLAGS_PRECPARAM) {
        如果(precision < 0)
          precision = param_num++;
        否则 {
          /* if this identifies a parameter already used, this is illegal */
          如果(is_arg_used(usedinput, precision))
            返回 PFMT_PRECARG;
        }
        如果(precision >= MAX_PARAMETERS)
          返回 PFMT_MANYARGS;
        如果(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      如果(param < 0)
        param = param_num++;
      如果(param >= MAX_PARAMETERS)
        返回 PFMT_MANYARGS;
      如果(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      如果(ocount > MAX_SEGMENTS)
        返回 PFMT_MANYSEGS;
      optr->input = (unsigned 整数)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    否则
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  如果(outlen) {
    optr = &out[ocount++];
    如果(ocount > MAX_SEGMENTS)
      返回 PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  为(i = 0; i < max_param + 1; i++) {
    struct va_input *iptr = &in[i];
    如果(!is_arg_used(usedinput, i))
      /* bad input */
      返回 PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    switch(iptr->type) {
    case MTYPE_STRING:
      iptr->val.str = va_arg(arglist, const char *);
      break;

    case MTYPE_INTPTR:
    case MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, void *);
      break;

    case MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      break;

    case MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      break;

    case MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      break;

    case MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      break;

    case MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned 整数);
      break;

    case MTYPE_INT:
    case MTYPE_WIDTH:
    case MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, 整数);
      break;

    case MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, double);
      break;

    case MTYPE_LONGDOUBLE:
      iptr->val.dnum = (double)va_arg(arglist, long double);
      break;

    default:
      DEBUGASSERT(NULL); /* unexpected */
      break;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  返回 PFMT_OK;
}

struct mproperty {
  整数 width;            /* Width of a field. */
  整数 prec;             /* Precision of a field. */
  unsigned 整数 flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned char *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef 整数 (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       整数 (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       const unsigned char *buf, size_t len, 整数 *donep)
{
  如果(!len)
    返回 FALSE;
  如果(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (整数)n;
    返回 n != len;
  }
  当(len--) {
    如果(stream(*buf++, userp))
      返回 TRUE;
    (*donep)++;
  }
  返回 FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const char pad_spaces[] = "                ";
static const char pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       整数 (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       unsigned char pad, 整数 num, 整数 *donep)
{
  const unsigned char *src;
  如果(num <= 0)
    返回 FALSE;
  如果(pad == '0')
    src = (const unsigned char *)pad_zeros;
  否则
    src = (const unsigned char *)pad_spaces;
  当(num > PAD_RUN) {
    如果(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      返回 TRUE;
    num -= PAD_RUN;
  }
  返回 stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        整数 (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        const char *str, size_t len, 整数 *donep)
{
  size_t n = 0;
  当(n < len && str[n])
    n++;
  返回 stream_run(userp, stream, streamn,
                    (const unsigned char *)str, n, donep);
}

static bool out_double(void *userp,
                       整数 (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       double dnum,
                       char *work, 整数 *donep)
{
  char fmt[32] = "%";
  char *fptr = &fmt[1];
  size_t left = sizeof(fmt) - strlen(fmt);
  整数 flags = p->flags;
  整数 width = p->width;
  整数 prec = p->prec;

  如果(flags & FLAGS_LEFT)
    *fptr++ = '-';
  如果(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  如果(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  如果(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  如果(width >= 0) {
    size_t dlen;
    如果(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  如果(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    整数 maxprec = BUFFSIZE - 1;
    double val = dnum;
    整数 len;
    如果(prec > maxprec)
      prec = maxprec - 1;
    如果(width > 0 && prec <= width)
      maxprec -= width;
    当(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    如果(prec > maxprec)
      prec = maxprec - 1;
    如果(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  如果(flags & FLAGS_LONG)
    *fptr++ = 'l';

  如果(flags & FLAGS_FLOATE)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'E' : 'e');
  否则 如果(flags & FLAGS_FLOATG)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'G' : 'g');
  否则
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#否则
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  返回 stream_run(userp, stream, streamn,
                    (const unsigned char *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       整数 (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       char *work, 整数 *donep)
{
  const unsigned char *digits = Curl_ldigits;
  整数 flags = p->flags;
  整数 width = p->width;
  整数 prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  整数 base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  char *workend = &work[BUFFSIZE - 2];
  char *w;

  如果(flags & FLAGS_CHAR) {
    /* Character. */
    如果(!(flags & FLAGS_LEFT)) {
      如果(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        返回 TRUE;
      width = 1;
    }
    OUTCHAR((char)num);
    如果(flags & FLAGS_LEFT) {
      如果(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        返回 TRUE;
    }
    返回 FALSE;
  }
  如果(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  否则 如果(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  否则 如果(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  否则 {
    /* Decimal integer. */
    is_neg = (nums < 0);
    如果(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  如果(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  case 10:
    当(num > 0) {
      *w-- = (char)('0' + (num % 10));
      num /= 10;
    }
    break;
  default:
    当(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    break;
  }
  width -= (整数)(workend - w);
  prec -= (整数)(workend - w);

  如果(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  如果(prec > 0) {
    width -= prec;
    当(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  如果(is_alt && base == 16)
    width -= 2;

  如果(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  如果(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    如果(stream_pad(userp, stream, streamn, ' ', width, donep))
      返回 TRUE;
    width = 0;
  }

  如果(is_neg)
    OUTCHAR('-');
  否则 如果(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  否则 如果(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  如果(is_alt && base == 16) {
    OUTCHAR('0');
    如果(flags & FLAGS_UPPER)
      OUTCHAR('X');
    否则
      OUTCHAR('x');
  }

  如果(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    如果(stream_pad(userp, stream, streamn, '0', width, donep))
      返回 TRUE;
    width = 0;
  }

  /* Write the number. */
  如果(stream_run(userp, stream, streamn, (const unsigned char *)w + 1,
                (size_t)(workend - w), donep))
    返回 TRUE;

  如果(flags & FLAGS_LEFT) {
    如果(stream_pad(userp, stream, streamn, ' ', width, donep))
      返回 TRUE;
  }

  返回 FALSE;
}

static const char nilstr[] = "(nil)";

static bool out_string(void *userp,
                       整数 (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       const char *str,
                       整数 *donep)
{
  整数 flags = p->flags;
  整数 width = p->width;
  整数 prec = p->prec;
  size_t len;

  如果(!str) {
    /* Write null string if there is space. */
    如果(prec == -1 || prec >= (整数)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned 整数)FLAGS_ALT;
    }
    否则 {
      str = "";
      len = 0;
    }
  }
  否则 如果(prec != -1)
    len = (size_t)prec;
  否则 如果(*str == '\0')
    len = 0;
  否则
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (整数)len;

  如果(flags & FLAGS_ALT)
    OUTCHAR('"');

  如果(!(flags & FLAGS_LEFT)) {
    如果(stream_pad(userp, stream, streamn, ' ', width, donep))
      返回 TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  如果(prec == -1) {
    如果(stream_run(userp, stream, streamn,
                  (const unsigned char *)str, len, donep))
      返回 TRUE;
  }
  否则 如果(stream_zrun(userp, stream, streamn, str, len, donep))
    返回 TRUE;
  如果(flags & FLAGS_LEFT) {
    如果(stream_pad(userp, stream, streamn, ' ', width, donep))
      返回 TRUE;
  }

  如果(flags & FLAGS_ALT)
    OUTCHAR('"');

  返回 FALSE;
}

static bool out_pointer(void *userp,
                        整数 (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        struct mproperty *p,
                        const char *ptr,
                        char *work,
                        整数 *donep)
{
  /* Generic pointer. */
  如果(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    如果(out_number(userp, stream, streamn, p, num, 0, work, donep))
      返回 TRUE;
  }
  否则 {
    /* Write "(nil)" for a nil pointer. */
    整数 width = p->width;
    整数 flags = p->flags;

    width -= (整数)CURL_CSTRLEN(nilstr);
    如果(flags & FLAGS_LEFT) {
      如果(stream_pad(userp, stream, streamn, ' ', width, donep))
        返回 TRUE;
      width = 0;
    }
    如果(stream_run(userp, stream, streamn, (const unsigned char *)nilstr,
                  sizeof(nilstr) - 1, donep))
      返回 TRUE;
    如果(!(flags & FLAGS_LEFT)) {
      如果(stream_pad(userp, stream, streamn, ' ', width, donep))
        返回 TRUE;
    }
  }
  返回 FALSE;
}

/*
 * formatf() - the general printf function.
 *
 * It calls parsefmt() to parse the format string. It populates two arrays;
 * one that describes the input arguments and one that describes a number of
 * output segments.
 *
 * On success, the input array describes the type of all arguments and their
 * values.
 *
 * The function then iterates over the output segments and outputs them one
 * by one until done. Using the appropriate input arguments (if any).
 *
 * All output is sent to the 'stream()' callback, one byte at a time.
 */
static 整数 formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   整数 (*stream)(unsigned char, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const char *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  整数 done = 0;   /* number of characters written */
  整数 i;
  整数 ocount = 0; /* number of output segments */
  整数 icount = 0; /* number of input arguments */

  struct outsegment output[MAX_SEGMENTS];
  struct va_input input[MAX_PARAMETERS];
  char work[BUFFSIZE + 2];

  /* Parse the format string */
  如果(parsefmt(format, output, input, &ocount, &icount, ap_save))
    返回 0;

  为(i = 0; i < ocount; i++) {
    struct outsegment *optr = &output[i];
    struct va_input *iptr = &input[optr->input];
    struct mproperty p;
    size_t outlen = optr->outlen;

    如果(outlen) {
      如果(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        返回 done;
      如果(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    如果(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (整数)input[optr->width].val.nums;
      如果(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        如果(p.width == INT_MIN)
          p.width = INT_MAX;
        否则
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned 整数)FLAGS_PAD_NIL;
      }
    }
    否则
      p.width = optr->width;

    /* pick up the specified precision */
    如果(p.flags & FLAGS_PRECPARAM) {
      p.prec = (整数)input[optr->precision].val.nums;
      如果(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    否则 如果(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    否则
      p.prec = -1;

    switch(iptr->type) {
    case MTYPE_INTU:
    case MTYPE_LONGU:
    case MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      如果(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        返回 done;
      break;

    case MTYPE_INT:
    case MTYPE_LONG:
    case MTYPE_LONGLONG:
      如果(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        返回 done;
      break;

    case MTYPE_STRING:
      如果(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        返回 done;
      break;

    case MTYPE_PTR:
      如果(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        返回 done;
      break;

    case MTYPE_DOUBLE:
    case MTYPE_LONGDOUBLE:
      如果(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        返回 done;
      break;

    case MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      如果(flush && flush(userp))
        返回 done;
      /* Answer the count of characters written. */
      如果(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      否则 如果(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      否则 如果(!(p.flags & FLAGS_SHORT))
        *(整数 *)iptr->val.ptr = done;
      否则
        *(short *)iptr->val.ptr = (short)done;
      break;

    default:
      break;
    }
  }
  返回 done;
}

/* fputc() look-alike */
static 整数 addbyter(unsigned char outc, void *f)
{
  struct nsprintf *infop = f;
  如果(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (char)outc; /* store */
    infop->length++; /* we are now one byte larger */
    返回 0;     /* fputc() returns like this on success */
  }
  返回 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned char *buf, size_t len, void *f)
{
  struct nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  如果(len > fit)
    len = fit;
  如果(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  返回 len;
}

整数 curl_mvsnprintf(char *buffer, size_t maxlength, const char *format,
                    va_list args)
{
  整数 retcode;
  struct nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  如果(info.max) {
    /* we null-terminate this */
    如果(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    否则
      info.buffer[0] = 0;
  }
  返回 retcode;
}

整数 curl_msnprintf(char *buffer, size_t maxlength, const char *format, ...)
{
  整数 retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  返回 retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static 整数 alloc_flush(void *f)
{
  struct asprintf *infop = f;
  如果(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    如果(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      返回 1; /* fail */
    }
  }
  返回 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static 整数 alloc_addn(struct asprintf *infop, const unsigned char *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  如果(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    返回 1; /* fail */
  }
  返回 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(struct asprintf *infop, size_t more)
{
  返回 (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static 整数 alloc_addbyter(unsigned char outc, void *f)
{
  struct asprintf *infop = f;
  如果(infop->nstage == sizeof(infop->stage) && alloc_flush(infop))
    返回 1; /* fail */
  如果(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    如果(alloc_flush(infop))
      返回 1;
    返回 alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  返回 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned char *buf, size_t len, void *f)
{
  struct asprintf *infop = f;
  size_t accepted = 0;
  当(accepted < len) {
    size_t room = sizeof(infop->stage) - infop->nstage;
    size_t n;
    如果(!room) {
      如果(alloc_flush(infop))
        返回 accepted; /* accepted < len aborts formatting */
      room = sizeof(infop->stage);
    }
    n = len - accepted;
    如果(n > room)
      n = room;
    如果(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      如果(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      如果(alloc_flush(infop))
        返回 accepted;
      如果(alloc_addn(infop, buf + accepted, 1))
        返回 accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  返回 accepted;
}

/* appends the formatted string, returns MERR error code */
整数 curlx_dyn_vprintf(struct dynbuf *dyn, const char *format, va_list args)
{
  struct asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  如果(!info.merr)
    (void)alloc_flush(&info);
  如果(info.merr) {
    curlx_dyn_free(info.b);
    返回 info.merr;
  }
  返回 0;
}

char *curl_mvaprintf(const char *format, va_list args)
{
  struct asprintf info;
  struct dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  如果(!info.merr)
    (void)alloc_flush(&info);
  如果(info.merr) {
    curlx_dyn_free(info.b);
    返回 NULL;
  }
  如果(curlx_dyn_len(info.b))
    返回 curlx_dyn_ptr(info.b);
  返回 curlx_strdup("");
}

char *curl_maprintf(const char *format, ...)
{
  va_list args;
  char *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  返回 s;
}

static 整数 storebuffer(unsigned char outc, void *f)
{
  char **buffer = f;
  **buffer = (char)outc;
  (*buffer)++;
  返回 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned char *buf, size_t len, void *f)
{
  char **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  返回 len;
}

整数 curl_msprintf(char *buffer, const char *format, ...)
{
  va_list args; /* argument pointer */
  整数 retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  返回 retcode;
}

static 整数 fputc_wrapper(unsigned char outc, void *f)
{
  struct ioprintf *i = f;
  整数 out = outc;
  FILE *s = i->fd;
  整数 rc = fputc(out, s);
  i->error = (rc == EOF);
  返回 (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned char *buf, size_t len, void *f)
{
  struct ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  返回 n;
}

整数 curl_mvfprintf(FILE *fd, const char *format, va_list args)
{
  struct ioprintf info;
  整数 n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  如果(info.error)
    返回 -1;
  返回 n;
}

整数 curl_mfprintf(FILE *fd, const char *format, ...)
{
  va_list args; /* argument pointer */
  整数 n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  返回 n;
}

整数 curl_mprintf(const char *format, ...)
{
  va_list args; /* argument pointer */
  整数 n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  返回 n;
}

整数 curl_mvsprintf(char *buffer, const char *format, va_list args)
{
  整数 retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  返回 retcode;
}

整数 curl_mvprintf(const char *format, va_list args)
{
  返回 curl_mvfprintf(stdout, format, args);
}

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
    если(stream((unsigned char)(x), userp)) \
      вернуть TRUE;                        \
    (*donep)++;                           \
  } пока(0)

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
  целое width;     /* width OR width parameter number */
  целое precision; /* precision OR precision parameter number */
  unsigned целое flags;
  unsigned целое input; /* input argument array index */
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
static целое dollarstring(const char *p, const char **end)
{
  curl_off_t num;
  если(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    вернуть -1;
  *end = p;
  вернуть (целое)num - 1;
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

static целое parse_flags(const char **fmtp, unsigned целое *flagsp, целое use_dollar,
                       целое *precp, целое *widthp)
{
  const char *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned целое flags = 0;
  целое width = 0;
  целое precision = 0;

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
      flags &= ~(unsigned целое)FLAGS_PAD_NIL;
      break;
    case '#':
      flags |= FLAGS_ALT;
      break;
    case '.':
      если('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        если(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          если(precision < 0)
            /* illegal combo */
            вернуть PFMT_DOLLARPREC;
        }
        иначе
          /* get it from the next argument */
          precision = -1;
      }
      иначе {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        если(is_neg)
          fmt++;
        если(curlx_str_number(&fmt, &num, INT_MAX))
          вернуть PFMT_PREC;
        precision = (целое)num;
        если(is_neg)
          precision = -precision;
      }
      если((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        вернуть PFMT_PRECMIX;
      break;
    case 'h':
      flags |= FLAGS_SHORT;
      break;
#ifdef _WIN32
    case 'I':
      /* Non-ANSI integer extensions I32 I64 */
      если((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      иначе если((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      иначе {
#если SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#иначе
        flags |= FLAGS_LONG;
#endif
      }
      break;
#endif /* _WIN32 */
    case 'l':
      если(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      иначе
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
#если SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#иначе
      flags |= FLAGS_LONG;
#endif
      break;
    case 'O':
#если SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#иначе
      flags |= FLAGS_LONG;
#endif
      break;
    case '0':
      если(!(flags & FLAGS_LEFT))
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
      если(curlx_str_number(&fmt, &num, INT_MAX))
        вернуть PFMT_WIDTH;
      width = (целое)num;
      break;
    }
    case '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      если(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        если(width < 0)
          /* illegal combo */
          вернуть PFMT_DOLLARWIDTH;
      }
      иначе
        /* pick from the next argument */
        width = -1;
      break;
    default:
      loopit = FALSE;
      fmt--;
      break;
    } /* switch */
  } пока(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  вернуть PFMT_OK;
}

static bool parse_conversion(const char f, unsigned целое *flagp,
                             FormatType *typep)
{
  unsigned целое flags = *flagp;
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
    если(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    иначе если(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    иначе
      type = MTYPE_INT;
    break;
  case 'u':
    если(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    иначе если(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    иначе
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    break;
  case 'o':
    если(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    иначе если(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    иначе
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    break;
  case 'x':
    если(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    иначе если(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    иначе
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    break;
  case 'X':
    если(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    иначе если(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    иначе
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
    вернуть TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  вернуть FALSE;
}

static целое parsefmt(const char *format,
                    struct outsegment *out,
                    struct va_input *in,
                    целое *opieces,
                    целое *ipieces, va_list arglist)
{
  const char *fmt = format;
  целое param_num = 0;
  целое max_param = -1;
  целое i;
  целое ocount = 0;
  unsigned char usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  struct outsegment *optr;
  целое use_dollar = DOLLAR_UNKNOWN;
  const char *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, sizeof(usedinput));

  пока(*fmt) {
    если(*fmt == '%') {
      struct va_input *iptr;
      FormatType type;
      unsigned целое flags = 0;
      целое width = 0;
      целое precision = 0;
      целое param = -1;
      целое rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      если(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        если(outlen) {
          optr = &out[ocount++];
          если(ocount > MAX_SEGMENTS)
            вернуть PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      если(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        если(param < 0) {
          если(use_dollar == DOLLAR_USE)
            /* illegal combo */
            вернуть PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        иначе
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      если(rc)
        вернуть rc;

      если(parse_conversion(*fmt, &flags, &type))
        continue;

      если(flags & FLAGS_WIDTHPARAM) {
        если(width < 0)
          width = param_num++;
        иначе {
          /* if this identifies a parameter already used, this is illegal */
          если(is_arg_used(usedinput, width))
            вернуть PFMT_WIDTHARG;
        }
        если(width >= MAX_PARAMETERS)
          вернуть PFMT_MANYARGS;
        если(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      если(flags & FLAGS_PRECPARAM) {
        если(precision < 0)
          precision = param_num++;
        иначе {
          /* if this identifies a parameter already used, this is illegal */
          если(is_arg_used(usedinput, precision))
            вернуть PFMT_PRECARG;
        }
        если(precision >= MAX_PARAMETERS)
          вернуть PFMT_MANYARGS;
        если(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      если(param < 0)
        param = param_num++;
      если(param >= MAX_PARAMETERS)
        вернуть PFMT_MANYARGS;
      если(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      если(ocount > MAX_SEGMENTS)
        вернуть PFMT_MANYSEGS;
      optr->input = (unsigned целое)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    иначе
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  если(outlen) {
    optr = &out[ocount++];
    если(ocount > MAX_SEGMENTS)
      вернуть PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  для(i = 0; i < max_param + 1; i++) {
    struct va_input *iptr = &in[i];
    если(!is_arg_used(usedinput, i))
      /* bad input */
      вернуть PFMT_INPUTGAP;

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
      iptr->val.numu = va_arg(arglist, unsigned целое);
      break;

    case MTYPE_INT:
    case MTYPE_WIDTH:
    case MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, целое);
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

  вернуть PFMT_OK;
}

struct mproperty {
  целое width;            /* Width of a field. */
  целое prec;             /* Precision of a field. */
  unsigned целое flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned char *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef целое (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       целое (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       const unsigned char *buf, size_t len, целое *donep)
{
  если(!len)
    вернуть FALSE;
  если(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (целое)n;
    вернуть n != len;
  }
  пока(len--) {
    если(stream(*buf++, userp))
      вернуть TRUE;
    (*donep)++;
  }
  вернуть FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const char pad_spaces[] = "                ";
static const char pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       целое (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       unsigned char pad, целое num, целое *donep)
{
  const unsigned char *src;
  если(num <= 0)
    вернуть FALSE;
  если(pad == '0')
    src = (const unsigned char *)pad_zeros;
  иначе
    src = (const unsigned char *)pad_spaces;
  пока(num > PAD_RUN) {
    если(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      вернуть TRUE;
    num -= PAD_RUN;
  }
  вернуть stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        целое (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        const char *str, size_t len, целое *donep)
{
  size_t n = 0;
  пока(n < len && str[n])
    n++;
  вернуть stream_run(userp, stream, streamn,
                    (const unsigned char *)str, n, donep);
}

static bool out_double(void *userp,
                       целое (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       double dnum,
                       char *work, целое *donep)
{
  char fmt[32] = "%";
  char *fptr = &fmt[1];
  size_t left = sizeof(fmt) - strlen(fmt);
  целое flags = p->flags;
  целое width = p->width;
  целое prec = p->prec;

  если(flags & FLAGS_LEFT)
    *fptr++ = '-';
  если(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  если(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  если(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  если(width >= 0) {
    size_t dlen;
    если(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  если(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    целое maxprec = BUFFSIZE - 1;
    double val = dnum;
    целое len;
    если(prec > maxprec)
      prec = maxprec - 1;
    если(width > 0 && prec <= width)
      maxprec -= width;
    пока(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    если(prec > maxprec)
      prec = maxprec - 1;
    если(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  если(flags & FLAGS_LONG)
    *fptr++ = 'l';

  если(flags & FLAGS_FLOATE)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'E' : 'e');
  иначе если(flags & FLAGS_FLOATG)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'G' : 'g');
  иначе
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
#иначе
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  вернуть stream_run(userp, stream, streamn,
                    (const unsigned char *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       целое (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       char *work, целое *donep)
{
  const unsigned char *digits = Curl_ldigits;
  целое flags = p->flags;
  целое width = p->width;
  целое prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  целое base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  char *workend = &work[BUFFSIZE - 2];
  char *w;

  если(flags & FLAGS_CHAR) {
    /* Character. */
    если(!(flags & FLAGS_LEFT)) {
      если(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        вернуть TRUE;
      width = 1;
    }
    OUTCHAR((char)num);
    если(flags & FLAGS_LEFT) {
      если(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        вернуть TRUE;
    }
    вернуть FALSE;
  }
  если(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  иначе если(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  иначе если(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  иначе {
    /* Decimal integer. */
    is_neg = (nums < 0);
    если(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  если(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  case 10:
    пока(num > 0) {
      *w-- = (char)('0' + (num % 10));
      num /= 10;
    }
    break;
  default:
    пока(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    break;
  }
  width -= (целое)(workend - w);
  prec -= (целое)(workend - w);

  если(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  если(prec > 0) {
    width -= prec;
    пока(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  если(is_alt && base == 16)
    width -= 2;

  если(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  если(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    если(stream_pad(userp, stream, streamn, ' ', width, donep))
      вернуть TRUE;
    width = 0;
  }

  если(is_neg)
    OUTCHAR('-');
  иначе если(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  иначе если(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  если(is_alt && base == 16) {
    OUTCHAR('0');
    если(flags & FLAGS_UPPER)
      OUTCHAR('X');
    иначе
      OUTCHAR('x');
  }

  если(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    если(stream_pad(userp, stream, streamn, '0', width, donep))
      вернуть TRUE;
    width = 0;
  }

  /* Write the number. */
  если(stream_run(userp, stream, streamn, (const unsigned char *)w + 1,
                (size_t)(workend - w), donep))
    вернуть TRUE;

  если(flags & FLAGS_LEFT) {
    если(stream_pad(userp, stream, streamn, ' ', width, donep))
      вернуть TRUE;
  }

  вернуть FALSE;
}

static const char nilstr[] = "(nil)";

static bool out_string(void *userp,
                       целое (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       const char *str,
                       целое *donep)
{
  целое flags = p->flags;
  целое width = p->width;
  целое prec = p->prec;
  size_t len;

  если(!str) {
    /* Write null string if there is space. */
    если(prec == -1 || prec >= (целое)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned целое)FLAGS_ALT;
    }
    иначе {
      str = "";
      len = 0;
    }
  }
  иначе если(prec != -1)
    len = (size_t)prec;
  иначе если(*str == '\0')
    len = 0;
  иначе
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (целое)len;

  если(flags & FLAGS_ALT)
    OUTCHAR('"');

  если(!(flags & FLAGS_LEFT)) {
    если(stream_pad(userp, stream, streamn, ' ', width, donep))
      вернуть TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  если(prec == -1) {
    если(stream_run(userp, stream, streamn,
                  (const unsigned char *)str, len, donep))
      вернуть TRUE;
  }
  иначе если(stream_zrun(userp, stream, streamn, str, len, donep))
    вернуть TRUE;
  если(flags & FLAGS_LEFT) {
    если(stream_pad(userp, stream, streamn, ' ', width, donep))
      вернуть TRUE;
  }

  если(flags & FLAGS_ALT)
    OUTCHAR('"');

  вернуть FALSE;
}

static bool out_pointer(void *userp,
                        целое (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        struct mproperty *p,
                        const char *ptr,
                        char *work,
                        целое *donep)
{
  /* Generic pointer. */
  если(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    если(out_number(userp, stream, streamn, p, num, 0, work, donep))
      вернуть TRUE;
  }
  иначе {
    /* Write "(nil)" for a nil pointer. */
    целое width = p->width;
    целое flags = p->flags;

    width -= (целое)CURL_CSTRLEN(nilstr);
    если(flags & FLAGS_LEFT) {
      если(stream_pad(userp, stream, streamn, ' ', width, donep))
        вернуть TRUE;
      width = 0;
    }
    если(stream_run(userp, stream, streamn, (const unsigned char *)nilstr,
                  sizeof(nilstr) - 1, donep))
      вернуть TRUE;
    если(!(flags & FLAGS_LEFT)) {
      если(stream_pad(userp, stream, streamn, ' ', width, donep))
        вернуть TRUE;
    }
  }
  вернуть FALSE;
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
static целое formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   целое (*stream)(unsigned char, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const char *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  целое done = 0;   /* number of characters written */
  целое i;
  целое ocount = 0; /* number of output segments */
  целое icount = 0; /* number of input arguments */

  struct outsegment output[MAX_SEGMENTS];
  struct va_input input[MAX_PARAMETERS];
  char work[BUFFSIZE + 2];

  /* Parse the format string */
  если(parsefmt(format, output, input, &ocount, &icount, ap_save))
    вернуть 0;

  для(i = 0; i < ocount; i++) {
    struct outsegment *optr = &output[i];
    struct va_input *iptr = &input[optr->input];
    struct mproperty p;
    size_t outlen = optr->outlen;

    если(outlen) {
      если(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        вернуть done;
      если(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    если(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (целое)input[optr->width].val.nums;
      если(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        если(p.width == INT_MIN)
          p.width = INT_MAX;
        иначе
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned целое)FLAGS_PAD_NIL;
      }
    }
    иначе
      p.width = optr->width;

    /* pick up the specified precision */
    если(p.flags & FLAGS_PRECPARAM) {
      p.prec = (целое)input[optr->precision].val.nums;
      если(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    иначе если(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    иначе
      p.prec = -1;

    switch(iptr->type) {
    case MTYPE_INTU:
    case MTYPE_LONGU:
    case MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      если(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        вернуть done;
      break;

    case MTYPE_INT:
    case MTYPE_LONG:
    case MTYPE_LONGLONG:
      если(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        вернуть done;
      break;

    case MTYPE_STRING:
      если(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        вернуть done;
      break;

    case MTYPE_PTR:
      если(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        вернуть done;
      break;

    case MTYPE_DOUBLE:
    case MTYPE_LONGDOUBLE:
      если(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        вернуть done;
      break;

    case MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      если(flush && flush(userp))
        вернуть done;
      /* Answer the count of characters written. */
      если(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      иначе если(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      иначе если(!(p.flags & FLAGS_SHORT))
        *(целое *)iptr->val.ptr = done;
      иначе
        *(short *)iptr->val.ptr = (short)done;
      break;

    default:
      break;
    }
  }
  вернуть done;
}

/* fputc() look-alike */
static целое addbyter(unsigned char outc, void *f)
{
  struct nsprintf *infop = f;
  если(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (char)outc; /* store */
    infop->length++; /* we are now one byte larger */
    вернуть 0;     /* fputc() returns like this on success */
  }
  вернуть 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned char *buf, size_t len, void *f)
{
  struct nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  если(len > fit)
    len = fit;
  если(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  вернуть len;
}

целое curl_mvsnprintf(char *buffer, size_t maxlength, const char *format,
                    va_list args)
{
  целое retcode;
  struct nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  если(info.max) {
    /* we null-terminate this */
    если(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    иначе
      info.buffer[0] = 0;
  }
  вернуть retcode;
}

целое curl_msnprintf(char *buffer, size_t maxlength, const char *format, ...)
{
  целое retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  вернуть retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static целое alloc_flush(void *f)
{
  struct asprintf *infop = f;
  если(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    если(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      вернуть 1; /* fail */
    }
  }
  вернуть 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static целое alloc_addn(struct asprintf *infop, const unsigned char *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  если(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    вернуть 1; /* fail */
  }
  вернуть 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(struct asprintf *infop, size_t more)
{
  вернуть (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static целое alloc_addbyter(unsigned char outc, void *f)
{
  struct asprintf *infop = f;
  если(infop->nstage == sizeof(infop->stage) && alloc_flush(infop))
    вернуть 1; /* fail */
  если(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    если(alloc_flush(infop))
      вернуть 1;
    вернуть alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  вернуть 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned char *buf, size_t len, void *f)
{
  struct asprintf *infop = f;
  size_t accepted = 0;
  пока(accepted < len) {
    size_t room = sizeof(infop->stage) - infop->nstage;
    size_t n;
    если(!room) {
      если(alloc_flush(infop))
        вернуть accepted; /* accepted < len aborts formatting */
      room = sizeof(infop->stage);
    }
    n = len - accepted;
    если(n > room)
      n = room;
    если(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      если(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      если(alloc_flush(infop))
        вернуть accepted;
      если(alloc_addn(infop, buf + accepted, 1))
        вернуть accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  вернуть accepted;
}

/* appends the formatted string, returns MERR error code */
целое curlx_dyn_vprintf(struct dynbuf *dyn, const char *format, va_list args)
{
  struct asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  если(!info.merr)
    (void)alloc_flush(&info);
  если(info.merr) {
    curlx_dyn_free(info.b);
    вернуть info.merr;
  }
  вернуть 0;
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
  если(!info.merr)
    (void)alloc_flush(&info);
  если(info.merr) {
    curlx_dyn_free(info.b);
    вернуть NULL;
  }
  если(curlx_dyn_len(info.b))
    вернуть curlx_dyn_ptr(info.b);
  вернуть curlx_strdup("");
}

char *curl_maprintf(const char *format, ...)
{
  va_list args;
  char *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  вернуть s;
}

static целое storebuffer(unsigned char outc, void *f)
{
  char **buffer = f;
  **buffer = (char)outc;
  (*buffer)++;
  вернуть 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned char *buf, size_t len, void *f)
{
  char **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  вернуть len;
}

целое curl_msprintf(char *buffer, const char *format, ...)
{
  va_list args; /* argument pointer */
  целое retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  вернуть retcode;
}

static целое fputc_wrapper(unsigned char outc, void *f)
{
  struct ioprintf *i = f;
  целое out = outc;
  FILE *s = i->fd;
  целое rc = fputc(out, s);
  i->error = (rc == EOF);
  вернуть (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned char *buf, size_t len, void *f)
{
  struct ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  вернуть n;
}

целое curl_mvfprintf(FILE *fd, const char *format, va_list args)
{
  struct ioprintf info;
  целое n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  если(info.error)
    вернуть -1;
  вернуть n;
}

целое curl_mfprintf(FILE *fd, const char *format, ...)
{
  va_list args; /* argument pointer */
  целое n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  вернуть n;
}

целое curl_mprintf(const char *format, ...)
{
  va_list args; /* argument pointer */
  целое n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  вернуть n;
}

целое curl_mvsprintf(char *buffer, const char *format, va_list args)
{
  целое retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  вернуть retcode;
}

целое curl_mvprintf(const char *format, va_list args)
{
  вернуть curl_mvfprintf(stdout, format, args);
}

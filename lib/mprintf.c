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
    якщо(stream((unsigned char)(x), userp)) \
      повернути TRUE;                        \
    (*donep)++;                           \
  } поки(0)

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
  ціле width;     /* width OR width parameter number */
  ціле precision; /* precision OR precision parameter number */
  unsigned ціле flags;
  unsigned ціле input; /* input argument array index */
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
static ціле dollarstring(const char *p, const char **end)
{
  curl_off_t num;
  якщо(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    повернути -1;
  *end = p;
  повернути (ціле)num - 1;
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

static ціле parse_flags(const char **fmtp, unsigned ціле *flagsp, ціле use_dollar,
                       ціле *precp, ціле *widthp)
{
  const char *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned ціле flags = 0;
  ціле width = 0;
  ціле precision = 0;

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
      flags &= ~(unsigned ціле)FLAGS_PAD_NIL;
      break;
    case '#':
      flags |= FLAGS_ALT;
      break;
    case '.':
      якщо('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        якщо(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          якщо(precision < 0)
            /* illegal combo */
            повернути PFMT_DOLLARPREC;
        }
        інакше
          /* get it from the next argument */
          precision = -1;
      }
      інакше {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        якщо(is_neg)
          fmt++;
        якщо(curlx_str_number(&fmt, &num, INT_MAX))
          повернути PFMT_PREC;
        precision = (ціле)num;
        якщо(is_neg)
          precision = -precision;
      }
      якщо((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        повернути PFMT_PRECMIX;
      break;
    case 'h':
      flags |= FLAGS_SHORT;
      break;
#ifdef _WIN32
    case 'I':
      /* Non-ANSI integer extensions I32 I64 */
      якщо((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      інакше якщо((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      інакше {
#якщо SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#інакше
        flags |= FLAGS_LONG;
#endif
      }
      break;
#endif /* _WIN32 */
    case 'l':
      якщо(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      інакше
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
#якщо SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#інакше
      flags |= FLAGS_LONG;
#endif
      break;
    case 'O':
#якщо SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#інакше
      flags |= FLAGS_LONG;
#endif
      break;
    case '0':
      якщо(!(flags & FLAGS_LEFT))
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
      якщо(curlx_str_number(&fmt, &num, INT_MAX))
        повернути PFMT_WIDTH;
      width = (ціле)num;
      break;
    }
    case '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      якщо(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        якщо(width < 0)
          /* illegal combo */
          повернути PFMT_DOLLARWIDTH;
      }
      інакше
        /* pick from the next argument */
        width = -1;
      break;
    default:
      loopit = FALSE;
      fmt--;
      break;
    } /* switch */
  } поки(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  повернути PFMT_OK;
}

static bool parse_conversion(const char f, unsigned ціле *flagp,
                             FormatType *typep)
{
  unsigned ціле flags = *flagp;
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
    якщо(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    інакше якщо(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    інакше
      type = MTYPE_INT;
    break;
  case 'u':
    якщо(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    інакше якщо(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    інакше
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    break;
  case 'o':
    якщо(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    інакше якщо(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    інакше
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    break;
  case 'x':
    якщо(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    інакше якщо(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    інакше
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    break;
  case 'X':
    якщо(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    інакше якщо(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    інакше
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
    повернути TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  повернути FALSE;
}

static ціле parsefmt(const char *format,
                    struct outsegment *out,
                    struct va_input *in,
                    ціле *opieces,
                    ціле *ipieces, va_list arglist)
{
  const char *fmt = format;
  ціле param_num = 0;
  ціле max_param = -1;
  ціле i;
  ціле ocount = 0;
  unsigned char usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  struct outsegment *optr;
  ціле use_dollar = DOLLAR_UNKNOWN;
  const char *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, sizeof(usedinput));

  поки(*fmt) {
    якщо(*fmt == '%') {
      struct va_input *iptr;
      FormatType type;
      unsigned ціле flags = 0;
      ціле width = 0;
      ціле precision = 0;
      ціле param = -1;
      ціле rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      якщо(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        якщо(outlen) {
          optr = &out[ocount++];
          якщо(ocount > MAX_SEGMENTS)
            повернути PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      якщо(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        якщо(param < 0) {
          якщо(use_dollar == DOLLAR_USE)
            /* illegal combo */
            повернути PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        інакше
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      якщо(rc)
        повернути rc;

      якщо(parse_conversion(*fmt, &flags, &type))
        continue;

      якщо(flags & FLAGS_WIDTHPARAM) {
        якщо(width < 0)
          width = param_num++;
        інакше {
          /* if this identifies a parameter already used, this is illegal */
          якщо(is_arg_used(usedinput, width))
            повернути PFMT_WIDTHARG;
        }
        якщо(width >= MAX_PARAMETERS)
          повернути PFMT_MANYARGS;
        якщо(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      якщо(flags & FLAGS_PRECPARAM) {
        якщо(precision < 0)
          precision = param_num++;
        інакше {
          /* if this identifies a parameter already used, this is illegal */
          якщо(is_arg_used(usedinput, precision))
            повернути PFMT_PRECARG;
        }
        якщо(precision >= MAX_PARAMETERS)
          повернути PFMT_MANYARGS;
        якщо(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      якщо(param < 0)
        param = param_num++;
      якщо(param >= MAX_PARAMETERS)
        повернути PFMT_MANYARGS;
      якщо(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      якщо(ocount > MAX_SEGMENTS)
        повернути PFMT_MANYSEGS;
      optr->input = (unsigned ціле)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    інакше
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  якщо(outlen) {
    optr = &out[ocount++];
    якщо(ocount > MAX_SEGMENTS)
      повернути PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  для(i = 0; i < max_param + 1; i++) {
    struct va_input *iptr = &in[i];
    якщо(!is_arg_used(usedinput, i))
      /* bad input */
      повернути PFMT_INPUTGAP;

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
      iptr->val.numu = va_arg(arglist, unsigned ціле);
      break;

    case MTYPE_INT:
    case MTYPE_WIDTH:
    case MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, ціле);
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

  повернути PFMT_OK;
}

struct mproperty {
  ціле width;            /* Width of a field. */
  ціле prec;             /* Precision of a field. */
  unsigned ціле flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned char *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef ціле (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       ціле (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       const unsigned char *buf, size_t len, ціле *donep)
{
  якщо(!len)
    повернути FALSE;
  якщо(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (ціле)n;
    повернути n != len;
  }
  поки(len--) {
    якщо(stream(*buf++, userp))
      повернути TRUE;
    (*donep)++;
  }
  повернути FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const char pad_spaces[] = "                ";
static const char pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       ціле (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       unsigned char pad, ціле num, ціле *donep)
{
  const unsigned char *src;
  якщо(num <= 0)
    повернути FALSE;
  якщо(pad == '0')
    src = (const unsigned char *)pad_zeros;
  інакше
    src = (const unsigned char *)pad_spaces;
  поки(num > PAD_RUN) {
    якщо(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      повернути TRUE;
    num -= PAD_RUN;
  }
  повернути stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        ціле (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        const char *str, size_t len, ціле *donep)
{
  size_t n = 0;
  поки(n < len && str[n])
    n++;
  повернути stream_run(userp, stream, streamn,
                    (const unsigned char *)str, n, donep);
}

static bool out_double(void *userp,
                       ціле (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       double dnum,
                       char *work, ціле *donep)
{
  char fmt[32] = "%";
  char *fptr = &fmt[1];
  size_t left = sizeof(fmt) - strlen(fmt);
  ціле flags = p->flags;
  ціле width = p->width;
  ціле prec = p->prec;

  якщо(flags & FLAGS_LEFT)
    *fptr++ = '-';
  якщо(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  якщо(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  якщо(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  якщо(width >= 0) {
    size_t dlen;
    якщо(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  якщо(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    ціле maxprec = BUFFSIZE - 1;
    double val = dnum;
    ціле len;
    якщо(prec > maxprec)
      prec = maxprec - 1;
    якщо(width > 0 && prec <= width)
      maxprec -= width;
    поки(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    якщо(prec > maxprec)
      prec = maxprec - 1;
    якщо(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  якщо(flags & FLAGS_LONG)
    *fptr++ = 'l';

  якщо(flags & FLAGS_FLOATE)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'E' : 'e');
  інакше якщо(flags & FLAGS_FLOATG)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'G' : 'g');
  інакше
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
#інакше
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  повернути stream_run(userp, stream, streamn,
                    (const unsigned char *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       ціле (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       char *work, ціле *donep)
{
  const unsigned char *digits = Curl_ldigits;
  ціле flags = p->flags;
  ціле width = p->width;
  ціле prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  ціле base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  char *workend = &work[BUFFSIZE - 2];
  char *w;

  якщо(flags & FLAGS_CHAR) {
    /* Character. */
    якщо(!(flags & FLAGS_LEFT)) {
      якщо(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        повернути TRUE;
      width = 1;
    }
    OUTCHAR((char)num);
    якщо(flags & FLAGS_LEFT) {
      якщо(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        повернути TRUE;
    }
    повернути FALSE;
  }
  якщо(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  інакше якщо(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  інакше якщо(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  інакше {
    /* Decimal integer. */
    is_neg = (nums < 0);
    якщо(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  якщо(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  case 10:
    поки(num > 0) {
      *w-- = (char)('0' + (num % 10));
      num /= 10;
    }
    break;
  default:
    поки(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    break;
  }
  width -= (ціле)(workend - w);
  prec -= (ціле)(workend - w);

  якщо(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  якщо(prec > 0) {
    width -= prec;
    поки(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  якщо(is_alt && base == 16)
    width -= 2;

  якщо(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  якщо(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    якщо(stream_pad(userp, stream, streamn, ' ', width, donep))
      повернути TRUE;
    width = 0;
  }

  якщо(is_neg)
    OUTCHAR('-');
  інакше якщо(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  інакше якщо(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  якщо(is_alt && base == 16) {
    OUTCHAR('0');
    якщо(flags & FLAGS_UPPER)
      OUTCHAR('X');
    інакше
      OUTCHAR('x');
  }

  якщо(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    якщо(stream_pad(userp, stream, streamn, '0', width, donep))
      повернути TRUE;
    width = 0;
  }

  /* Write the number. */
  якщо(stream_run(userp, stream, streamn, (const unsigned char *)w + 1,
                (size_t)(workend - w), donep))
    повернути TRUE;

  якщо(flags & FLAGS_LEFT) {
    якщо(stream_pad(userp, stream, streamn, ' ', width, donep))
      повернути TRUE;
  }

  повернути FALSE;
}

static const char nilstr[] = "(nil)";

static bool out_string(void *userp,
                       ціле (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       const char *str,
                       ціле *donep)
{
  ціле flags = p->flags;
  ціле width = p->width;
  ціле prec = p->prec;
  size_t len;

  якщо(!str) {
    /* Write null string if there is space. */
    якщо(prec == -1 || prec >= (ціле)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned ціле)FLAGS_ALT;
    }
    інакше {
      str = "";
      len = 0;
    }
  }
  інакше якщо(prec != -1)
    len = (size_t)prec;
  інакше якщо(*str == '\0')
    len = 0;
  інакше
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (ціле)len;

  якщо(flags & FLAGS_ALT)
    OUTCHAR('"');

  якщо(!(flags & FLAGS_LEFT)) {
    якщо(stream_pad(userp, stream, streamn, ' ', width, donep))
      повернути TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  якщо(prec == -1) {
    якщо(stream_run(userp, stream, streamn,
                  (const unsigned char *)str, len, donep))
      повернути TRUE;
  }
  інакше якщо(stream_zrun(userp, stream, streamn, str, len, donep))
    повернути TRUE;
  якщо(flags & FLAGS_LEFT) {
    якщо(stream_pad(userp, stream, streamn, ' ', width, donep))
      повернути TRUE;
  }

  якщо(flags & FLAGS_ALT)
    OUTCHAR('"');

  повернути FALSE;
}

static bool out_pointer(void *userp,
                        ціле (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        struct mproperty *p,
                        const char *ptr,
                        char *work,
                        ціле *donep)
{
  /* Generic pointer. */
  якщо(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    якщо(out_number(userp, stream, streamn, p, num, 0, work, donep))
      повернути TRUE;
  }
  інакше {
    /* Write "(nil)" for a nil pointer. */
    ціле width = p->width;
    ціле flags = p->flags;

    width -= (ціле)CURL_CSTRLEN(nilstr);
    якщо(flags & FLAGS_LEFT) {
      якщо(stream_pad(userp, stream, streamn, ' ', width, donep))
        повернути TRUE;
      width = 0;
    }
    якщо(stream_run(userp, stream, streamn, (const unsigned char *)nilstr,
                  sizeof(nilstr) - 1, donep))
      повернути TRUE;
    якщо(!(flags & FLAGS_LEFT)) {
      якщо(stream_pad(userp, stream, streamn, ' ', width, donep))
        повернути TRUE;
    }
  }
  повернути FALSE;
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
static ціле formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   ціле (*stream)(unsigned char, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const char *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  ціле done = 0;   /* number of characters written */
  ціле i;
  ціле ocount = 0; /* number of output segments */
  ціле icount = 0; /* number of input arguments */

  struct outsegment output[MAX_SEGMENTS];
  struct va_input input[MAX_PARAMETERS];
  char work[BUFFSIZE + 2];

  /* Parse the format string */
  якщо(parsefmt(format, output, input, &ocount, &icount, ap_save))
    повернути 0;

  для(i = 0; i < ocount; i++) {
    struct outsegment *optr = &output[i];
    struct va_input *iptr = &input[optr->input];
    struct mproperty p;
    size_t outlen = optr->outlen;

    якщо(outlen) {
      якщо(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        повернути done;
      якщо(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    якщо(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (ціле)input[optr->width].val.nums;
      якщо(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        якщо(p.width == INT_MIN)
          p.width = INT_MAX;
        інакше
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned ціле)FLAGS_PAD_NIL;
      }
    }
    інакше
      p.width = optr->width;

    /* pick up the specified precision */
    якщо(p.flags & FLAGS_PRECPARAM) {
      p.prec = (ціле)input[optr->precision].val.nums;
      якщо(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    інакше якщо(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    інакше
      p.prec = -1;

    switch(iptr->type) {
    case MTYPE_INTU:
    case MTYPE_LONGU:
    case MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      якщо(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        повернути done;
      break;

    case MTYPE_INT:
    case MTYPE_LONG:
    case MTYPE_LONGLONG:
      якщо(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        повернути done;
      break;

    case MTYPE_STRING:
      якщо(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        повернути done;
      break;

    case MTYPE_PTR:
      якщо(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        повернути done;
      break;

    case MTYPE_DOUBLE:
    case MTYPE_LONGDOUBLE:
      якщо(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        повернути done;
      break;

    case MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      якщо(flush && flush(userp))
        повернути done;
      /* Answer the count of characters written. */
      якщо(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      інакше якщо(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      інакше якщо(!(p.flags & FLAGS_SHORT))
        *(ціле *)iptr->val.ptr = done;
      інакше
        *(short *)iptr->val.ptr = (short)done;
      break;

    default:
      break;
    }
  }
  повернути done;
}

/* fputc() look-alike */
static ціле addbyter(unsigned char outc, void *f)
{
  struct nsprintf *infop = f;
  якщо(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (char)outc; /* store */
    infop->length++; /* we are now one byte larger */
    повернути 0;     /* fputc() returns like this on success */
  }
  повернути 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned char *buf, size_t len, void *f)
{
  struct nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  якщо(len > fit)
    len = fit;
  якщо(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  повернути len;
}

ціле curl_mvsnprintf(char *buffer, size_t maxlength, const char *format,
                    va_list args)
{
  ціле retcode;
  struct nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  якщо(info.max) {
    /* we null-terminate this */
    якщо(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    інакше
      info.buffer[0] = 0;
  }
  повернути retcode;
}

ціле curl_msnprintf(char *buffer, size_t maxlength, const char *format, ...)
{
  ціле retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  повернути retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static ціле alloc_flush(void *f)
{
  struct asprintf *infop = f;
  якщо(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    якщо(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      повернути 1; /* fail */
    }
  }
  повернути 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static ціле alloc_addn(struct asprintf *infop, const unsigned char *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  якщо(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    повернути 1; /* fail */
  }
  повернути 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(struct asprintf *infop, size_t more)
{
  повернути (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static ціле alloc_addbyter(unsigned char outc, void *f)
{
  struct asprintf *infop = f;
  якщо(infop->nstage == sizeof(infop->stage) && alloc_flush(infop))
    повернути 1; /* fail */
  якщо(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    якщо(alloc_flush(infop))
      повернути 1;
    повернути alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  повернути 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned char *buf, size_t len, void *f)
{
  struct asprintf *infop = f;
  size_t accepted = 0;
  поки(accepted < len) {
    size_t room = sizeof(infop->stage) - infop->nstage;
    size_t n;
    якщо(!room) {
      якщо(alloc_flush(infop))
        повернути accepted; /* accepted < len aborts formatting */
      room = sizeof(infop->stage);
    }
    n = len - accepted;
    якщо(n > room)
      n = room;
    якщо(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      якщо(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      якщо(alloc_flush(infop))
        повернути accepted;
      якщо(alloc_addn(infop, buf + accepted, 1))
        повернути accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  повернути accepted;
}

/* appends the formatted string, returns MERR error code */
ціле curlx_dyn_vprintf(struct dynbuf *dyn, const char *format, va_list args)
{
  struct asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  якщо(!info.merr)
    (void)alloc_flush(&info);
  якщо(info.merr) {
    curlx_dyn_free(info.b);
    повернути info.merr;
  }
  повернути 0;
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
  якщо(!info.merr)
    (void)alloc_flush(&info);
  якщо(info.merr) {
    curlx_dyn_free(info.b);
    повернути NULL;
  }
  якщо(curlx_dyn_len(info.b))
    повернути curlx_dyn_ptr(info.b);
  повернути curlx_strdup("");
}

char *curl_maprintf(const char *format, ...)
{
  va_list args;
  char *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  повернути s;
}

static ціле storebuffer(unsigned char outc, void *f)
{
  char **buffer = f;
  **buffer = (char)outc;
  (*buffer)++;
  повернути 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned char *buf, size_t len, void *f)
{
  char **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  повернути len;
}

ціле curl_msprintf(char *buffer, const char *format, ...)
{
  va_list args; /* argument pointer */
  ціле retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  повернути retcode;
}

static ціле fputc_wrapper(unsigned char outc, void *f)
{
  struct ioprintf *i = f;
  ціле out = outc;
  FILE *s = i->fd;
  ціле rc = fputc(out, s);
  i->error = (rc == EOF);
  повернути (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned char *buf, size_t len, void *f)
{
  struct ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  повернути n;
}

ціле curl_mvfprintf(FILE *fd, const char *format, va_list args)
{
  struct ioprintf info;
  ціле n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  якщо(info.error)
    повернути -1;
  повернути n;
}

ціле curl_mfprintf(FILE *fd, const char *format, ...)
{
  va_list args; /* argument pointer */
  ціле n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  повернути n;
}

ціле curl_mprintf(const char *format, ...)
{
  va_list args; /* argument pointer */
  ціле n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  повернути n;
}

ціле curl_mvsprintf(char *buffer, const char *format, va_list args)
{
  ціле retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  повернути retcode;
}

ціле curl_mvprintf(const char *format, va_list args)
{
  повернути curl_mvfprintf(stdout, format, args);
}

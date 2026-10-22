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
    ако(stream((unsigned char)(x), userp)) \
      врати TRUE;                        \
    (*donep)++;                           \
  } док(0)

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
  цео width;     /* width OR width parameter number */
  цео precision; /* precision OR precision parameter number */
  unsigned цео flags;
  unsigned цео input; /* input argument array index */
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
static цео dollarstring(const char *p, const char **end)
{
  curl_off_t num;
  ако(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    врати -1;
  *end = p;
  врати (цео)num - 1;
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

static цео parse_flags(const char **fmtp, unsigned цео *flagsp, цео use_dollar,
                       цео *precp, цео *widthp)
{
  const char *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned цео flags = 0;
  цео width = 0;
  цео precision = 0;

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
      flags &= ~(unsigned цео)FLAGS_PAD_NIL;
      break;
    case '#':
      flags |= FLAGS_ALT;
      break;
    case '.':
      ако('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ако(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ако(precision < 0)
            /* illegal combo */
            врати PFMT_DOLLARPREC;
        }
        инако
          /* get it from the next argument */
          precision = -1;
      }
      инако {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ако(is_neg)
          fmt++;
        ако(curlx_str_number(&fmt, &num, INT_MAX))
          врати PFMT_PREC;
        precision = (цео)num;
        ако(is_neg)
          precision = -precision;
      }
      ако((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        врати PFMT_PRECMIX;
      break;
    case 'h':
      flags |= FLAGS_SHORT;
      break;
#ifdef _WIN32
    case 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ако((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      инако ако((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      инако {
#ако SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#инако
        flags |= FLAGS_LONG;
#endif
      }
      break;
#endif /* _WIN32 */
    case 'l':
      ако(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      инако
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
#ако SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#инако
      flags |= FLAGS_LONG;
#endif
      break;
    case 'O':
#ако SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#инако
      flags |= FLAGS_LONG;
#endif
      break;
    case '0':
      ако(!(flags & FLAGS_LEFT))
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
      ако(curlx_str_number(&fmt, &num, INT_MAX))
        врати PFMT_WIDTH;
      width = (цео)num;
      break;
    }
    case '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ако(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        ако(width < 0)
          /* illegal combo */
          врати PFMT_DOLLARWIDTH;
      }
      инако
        /* pick from the next argument */
        width = -1;
      break;
    default:
      loopit = FALSE;
      fmt--;
      break;
    } /* switch */
  } док(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  врати PFMT_OK;
}

static bool parse_conversion(const char f, unsigned цео *flagp,
                             FormatType *typep)
{
  unsigned цео flags = *flagp;
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
    ако(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    инако ако(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    инако
      type = MTYPE_INT;
    break;
  case 'u':
    ако(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    инако ако(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    инако
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    break;
  case 'o':
    ако(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    инако ако(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    инако
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    break;
  case 'x':
    ако(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    инако ако(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    инако
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    break;
  case 'X':
    ако(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    инако ако(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    инако
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
    врати TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  врати FALSE;
}

static цео parsefmt(const char *format,
                    struct outsegment *out,
                    struct va_input *in,
                    цео *opieces,
                    цео *ipieces, va_list arglist)
{
  const char *fmt = format;
  цео param_num = 0;
  цео max_param = -1;
  цео i;
  цео ocount = 0;
  unsigned char usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  struct outsegment *optr;
  цео use_dollar = DOLLAR_UNKNOWN;
  const char *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, sizeof(usedinput));

  док(*fmt) {
    ако(*fmt == '%') {
      struct va_input *iptr;
      FormatType type;
      unsigned цео flags = 0;
      цео width = 0;
      цео precision = 0;
      цео param = -1;
      цео rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      ако(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ако(outlen) {
          optr = &out[ocount++];
          ако(ocount > MAX_SEGMENTS)
            врати PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      ако(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ако(param < 0) {
          ако(use_dollar == DOLLAR_USE)
            /* illegal combo */
            врати PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        инако
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      ако(rc)
        врати rc;

      ако(parse_conversion(*fmt, &flags, &type))
        continue;

      ако(flags & FLAGS_WIDTHPARAM) {
        ако(width < 0)
          width = param_num++;
        инако {
          /* if this identifies a parameter already used, this is illegal */
          ако(is_arg_used(usedinput, width))
            врати PFMT_WIDTHARG;
        }
        ако(width >= MAX_PARAMETERS)
          врати PFMT_MANYARGS;
        ако(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      ако(flags & FLAGS_PRECPARAM) {
        ако(precision < 0)
          precision = param_num++;
        инако {
          /* if this identifies a parameter already used, this is illegal */
          ако(is_arg_used(usedinput, precision))
            врати PFMT_PRECARG;
        }
        ако(precision >= MAX_PARAMETERS)
          врати PFMT_MANYARGS;
        ако(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      ако(param < 0)
        param = param_num++;
      ако(param >= MAX_PARAMETERS)
        врати PFMT_MANYARGS;
      ако(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      ако(ocount > MAX_SEGMENTS)
        врати PFMT_MANYSEGS;
      optr->input = (unsigned цео)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    инако
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  ако(outlen) {
    optr = &out[ocount++];
    ако(ocount > MAX_SEGMENTS)
      врати PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  за(i = 0; i < max_param + 1; i++) {
    struct va_input *iptr = &in[i];
    ако(!is_arg_used(usedinput, i))
      /* bad input */
      врати PFMT_INPUTGAP;

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
      iptr->val.numu = va_arg(arglist, unsigned цео);
      break;

    case MTYPE_INT:
    case MTYPE_WIDTH:
    case MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, цео);
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

  врати PFMT_OK;
}

struct mproperty {
  цео width;            /* Width of a field. */
  цео prec;             /* Precision of a field. */
  unsigned цео flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned char *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef цео (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       цео (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       const unsigned char *buf, size_t len, цео *donep)
{
  ако(!len)
    врати FALSE;
  ако(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (цео)n;
    врати n != len;
  }
  док(len--) {
    ако(stream(*buf++, userp))
      врати TRUE;
    (*donep)++;
  }
  врати FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const char pad_spaces[] = "                ";
static const char pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       цео (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       unsigned char pad, цео num, цео *donep)
{
  const unsigned char *src;
  ако(num <= 0)
    врати FALSE;
  ако(pad == '0')
    src = (const unsigned char *)pad_zeros;
  инако
    src = (const unsigned char *)pad_spaces;
  док(num > PAD_RUN) {
    ако(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      врати TRUE;
    num -= PAD_RUN;
  }
  врати stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        цео (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        const char *str, size_t len, цео *donep)
{
  size_t n = 0;
  док(n < len && str[n])
    n++;
  врати stream_run(userp, stream, streamn,
                    (const unsigned char *)str, n, donep);
}

static bool out_double(void *userp,
                       цео (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       double dnum,
                       char *work, цео *donep)
{
  char fmt[32] = "%";
  char *fptr = &fmt[1];
  size_t left = sizeof(fmt) - strlen(fmt);
  цео flags = p->flags;
  цео width = p->width;
  цео prec = p->prec;

  ако(flags & FLAGS_LEFT)
    *fptr++ = '-';
  ако(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ако(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ако(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ако(width >= 0) {
    size_t dlen;
    ако(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  ако(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    цео maxprec = BUFFSIZE - 1;
    double val = dnum;
    цео len;
    ако(prec > maxprec)
      prec = maxprec - 1;
    ако(width > 0 && prec <= width)
      maxprec -= width;
    док(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ако(prec > maxprec)
      prec = maxprec - 1;
    ако(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  ако(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ако(flags & FLAGS_FLOATE)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'E' : 'e');
  инако ако(flags & FLAGS_FLOATG)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'G' : 'g');
  инако
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
#инако
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  врати stream_run(userp, stream, streamn,
                    (const unsigned char *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       цео (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       char *work, цео *donep)
{
  const unsigned char *digits = Curl_ldigits;
  цео flags = p->flags;
  цео width = p->width;
  цео prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  цео base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  char *workend = &work[BUFFSIZE - 2];
  char *w;

  ако(flags & FLAGS_CHAR) {
    /* Character. */
    ако(!(flags & FLAGS_LEFT)) {
      ако(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        врати TRUE;
      width = 1;
    }
    OUTCHAR((char)num);
    ако(flags & FLAGS_LEFT) {
      ако(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        врати TRUE;
    }
    врати FALSE;
  }
  ако(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  инако ако(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  инако ако(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  инако {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ако(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ако(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  case 10:
    док(num > 0) {
      *w-- = (char)('0' + (num % 10));
      num /= 10;
    }
    break;
  default:
    док(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    break;
  }
  width -= (цео)(workend - w);
  prec -= (цео)(workend - w);

  ако(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  ако(prec > 0) {
    width -= prec;
    док(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ако(is_alt && base == 16)
    width -= 2;

  ако(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  ако(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    ако(stream_pad(userp, stream, streamn, ' ', width, donep))
      врати TRUE;
    width = 0;
  }

  ако(is_neg)
    OUTCHAR('-');
  инако ако(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  инако ако(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ако(is_alt && base == 16) {
    OUTCHAR('0');
    ако(flags & FLAGS_UPPER)
      OUTCHAR('X');
    инако
      OUTCHAR('x');
  }

  ако(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    ако(stream_pad(userp, stream, streamn, '0', width, donep))
      врати TRUE;
    width = 0;
  }

  /* Write the number. */
  ако(stream_run(userp, stream, streamn, (const unsigned char *)w + 1,
                (size_t)(workend - w), donep))
    врати TRUE;

  ако(flags & FLAGS_LEFT) {
    ако(stream_pad(userp, stream, streamn, ' ', width, donep))
      врати TRUE;
  }

  врати FALSE;
}

static const char nilstr[] = "(nil)";

static bool out_string(void *userp,
                       цео (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       const char *str,
                       цео *donep)
{
  цео flags = p->flags;
  цео width = p->width;
  цео prec = p->prec;
  size_t len;

  ако(!str) {
    /* Write null string if there is space. */
    ако(prec == -1 || prec >= (цео)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned цео)FLAGS_ALT;
    }
    инако {
      str = "";
      len = 0;
    }
  }
  инако ако(prec != -1)
    len = (size_t)prec;
  инако ако(*str == '\0')
    len = 0;
  инако
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (цео)len;

  ако(flags & FLAGS_ALT)
    OUTCHAR('"');

  ако(!(flags & FLAGS_LEFT)) {
    ако(stream_pad(userp, stream, streamn, ' ', width, donep))
      врати TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ако(prec == -1) {
    ако(stream_run(userp, stream, streamn,
                  (const unsigned char *)str, len, donep))
      врати TRUE;
  }
  инако ако(stream_zrun(userp, stream, streamn, str, len, donep))
    врати TRUE;
  ако(flags & FLAGS_LEFT) {
    ако(stream_pad(userp, stream, streamn, ' ', width, donep))
      врати TRUE;
  }

  ако(flags & FLAGS_ALT)
    OUTCHAR('"');

  врати FALSE;
}

static bool out_pointer(void *userp,
                        цео (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        struct mproperty *p,
                        const char *ptr,
                        char *work,
                        цео *donep)
{
  /* Generic pointer. */
  ако(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ако(out_number(userp, stream, streamn, p, num, 0, work, donep))
      врати TRUE;
  }
  инако {
    /* Write "(nil)" for a nil pointer. */
    цео width = p->width;
    цео flags = p->flags;

    width -= (цео)CURL_CSTRLEN(nilstr);
    ако(flags & FLAGS_LEFT) {
      ако(stream_pad(userp, stream, streamn, ' ', width, donep))
        врати TRUE;
      width = 0;
    }
    ако(stream_run(userp, stream, streamn, (const unsigned char *)nilstr,
                  sizeof(nilstr) - 1, donep))
      врати TRUE;
    ако(!(flags & FLAGS_LEFT)) {
      ако(stream_pad(userp, stream, streamn, ' ', width, donep))
        врати TRUE;
    }
  }
  врати FALSE;
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
static цео formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   цео (*stream)(unsigned char, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const char *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  цео done = 0;   /* number of characters written */
  цео i;
  цео ocount = 0; /* number of output segments */
  цео icount = 0; /* number of input arguments */

  struct outsegment output[MAX_SEGMENTS];
  struct va_input input[MAX_PARAMETERS];
  char work[BUFFSIZE + 2];

  /* Parse the format string */
  ако(parsefmt(format, output, input, &ocount, &icount, ap_save))
    врати 0;

  за(i = 0; i < ocount; i++) {
    struct outsegment *optr = &output[i];
    struct va_input *iptr = &input[optr->input];
    struct mproperty p;
    size_t outlen = optr->outlen;

    ако(outlen) {
      ако(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        врати done;
      ако(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ако(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (цео)input[optr->width].val.nums;
      ако(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ако(p.width == INT_MIN)
          p.width = INT_MAX;
        инако
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned цео)FLAGS_PAD_NIL;
      }
    }
    инако
      p.width = optr->width;

    /* pick up the specified precision */
    ако(p.flags & FLAGS_PRECPARAM) {
      p.prec = (цео)input[optr->precision].val.nums;
      ако(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    инако ако(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    инако
      p.prec = -1;

    switch(iptr->type) {
    case MTYPE_INTU:
    case MTYPE_LONGU:
    case MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ако(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        врати done;
      break;

    case MTYPE_INT:
    case MTYPE_LONG:
    case MTYPE_LONGLONG:
      ако(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        врати done;
      break;

    case MTYPE_STRING:
      ако(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        врати done;
      break;

    case MTYPE_PTR:
      ако(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        врати done;
      break;

    case MTYPE_DOUBLE:
    case MTYPE_LONGDOUBLE:
      ако(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        врати done;
      break;

    case MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ако(flush && flush(userp))
        врати done;
      /* Answer the count of characters written. */
      ако(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      инако ако(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      инако ако(!(p.flags & FLAGS_SHORT))
        *(цео *)iptr->val.ptr = done;
      инако
        *(short *)iptr->val.ptr = (short)done;
      break;

    default:
      break;
    }
  }
  врати done;
}

/* fputc() look-alike */
static цео addbyter(unsigned char outc, void *f)
{
  struct nsprintf *infop = f;
  ако(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (char)outc; /* store */
    infop->length++; /* we are now one byte larger */
    врати 0;     /* fputc() returns like this on success */
  }
  врати 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned char *buf, size_t len, void *f)
{
  struct nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  ако(len > fit)
    len = fit;
  ако(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  врати len;
}

цео curl_mvsnprintf(char *buffer, size_t maxlength, const char *format,
                    va_list args)
{
  цео retcode;
  struct nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  ако(info.max) {
    /* we null-terminate this */
    ако(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    инако
      info.buffer[0] = 0;
  }
  врати retcode;
}

цео curl_msnprintf(char *buffer, size_t maxlength, const char *format, ...)
{
  цео retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  врати retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static цео alloc_flush(void *f)
{
  struct asprintf *infop = f;
  ако(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ако(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      врати 1; /* fail */
    }
  }
  врати 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static цео alloc_addn(struct asprintf *infop, const unsigned char *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  ако(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    врати 1; /* fail */
  }
  врати 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(struct asprintf *infop, size_t more)
{
  врати (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static цео alloc_addbyter(unsigned char outc, void *f)
{
  struct asprintf *infop = f;
  ако(infop->nstage == sizeof(infop->stage) && alloc_flush(infop))
    врати 1; /* fail */
  ако(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ако(alloc_flush(infop))
      врати 1;
    врати alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  врати 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned char *buf, size_t len, void *f)
{
  struct asprintf *infop = f;
  size_t accepted = 0;
  док(accepted < len) {
    size_t room = sizeof(infop->stage) - infop->nstage;
    size_t n;
    ако(!room) {
      ако(alloc_flush(infop))
        врати accepted; /* accepted < len aborts formatting */
      room = sizeof(infop->stage);
    }
    n = len - accepted;
    ако(n > room)
      n = room;
    ако(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ако(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      ако(alloc_flush(infop))
        врати accepted;
      ако(alloc_addn(infop, buf + accepted, 1))
        врати accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  врати accepted;
}

/* appends the formatted string, returns MERR error code */
цео curlx_dyn_vprintf(struct dynbuf *dyn, const char *format, va_list args)
{
  struct asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ако(!info.merr)
    (void)alloc_flush(&info);
  ако(info.merr) {
    curlx_dyn_free(info.b);
    врати info.merr;
  }
  врати 0;
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
  ако(!info.merr)
    (void)alloc_flush(&info);
  ако(info.merr) {
    curlx_dyn_free(info.b);
    врати NULL;
  }
  ако(curlx_dyn_len(info.b))
    врати curlx_dyn_ptr(info.b);
  врати curlx_strdup("");
}

char *curl_maprintf(const char *format, ...)
{
  va_list args;
  char *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  врати s;
}

static цео storebuffer(unsigned char outc, void *f)
{
  char **buffer = f;
  **buffer = (char)outc;
  (*buffer)++;
  врати 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned char *buf, size_t len, void *f)
{
  char **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  врати len;
}

цео curl_msprintf(char *buffer, const char *format, ...)
{
  va_list args; /* argument pointer */
  цео retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  врати retcode;
}

static цео fputc_wrapper(unsigned char outc, void *f)
{
  struct ioprintf *i = f;
  цео out = outc;
  FILE *s = i->fd;
  цео rc = fputc(out, s);
  i->error = (rc == EOF);
  врати (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned char *buf, size_t len, void *f)
{
  struct ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  врати n;
}

цео curl_mvfprintf(FILE *fd, const char *format, va_list args)
{
  struct ioprintf info;
  цео n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  ако(info.error)
    врати -1;
  врати n;
}

цео curl_mfprintf(FILE *fd, const char *format, ...)
{
  va_list args; /* argument pointer */
  цео n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  врати n;
}

цео curl_mprintf(const char *format, ...)
{
  va_list args; /* argument pointer */
  цео n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  врати n;
}

цео curl_mvsprintf(char *buffer, const char *format, va_list args)
{
  цео retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  врати retcode;
}

цео curl_mvprintf(const char *format, va_list args)
{
  врати curl_mvfprintf(stdout, format, args);
}

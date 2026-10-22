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
    nếu(stream((unsigned char)(x), userp)) \
      trả_về TRUE;                        \
    (*donep)++;                           \
  } trong_khi(0)

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
  số_nguyên width;     /* width OR width parameter number */
  số_nguyên precision; /* precision OR precision parameter number */
  unsigned số_nguyên flags;
  unsigned số_nguyên input; /* input argument array index */
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
static số_nguyên dollarstring(const char *p, const char **end)
{
  curl_off_t num;
  nếu(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    trả_về -1;
  *end = p;
  trả_về (số_nguyên)num - 1;
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

static số_nguyên parse_flags(const char **fmtp, unsigned số_nguyên *flagsp, số_nguyên use_dollar,
                       số_nguyên *precp, số_nguyên *widthp)
{
  const char *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned số_nguyên flags = 0;
  số_nguyên width = 0;
  số_nguyên precision = 0;

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
      flags &= ~(unsigned số_nguyên)FLAGS_PAD_NIL;
      break;
    case '#':
      flags |= FLAGS_ALT;
      break;
    case '.':
      nếu('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        nếu(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          nếu(precision < 0)
            /* illegal combo */
            trả_về PFMT_DOLLARPREC;
        }
        không_thì
          /* get it from the next argument */
          precision = -1;
      }
      không_thì {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        nếu(is_neg)
          fmt++;
        nếu(curlx_str_number(&fmt, &num, INT_MAX))
          trả_về PFMT_PREC;
        precision = (số_nguyên)num;
        nếu(is_neg)
          precision = -precision;
      }
      nếu((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        trả_về PFMT_PRECMIX;
      break;
    case 'h':
      flags |= FLAGS_SHORT;
      break;
#ifdef _WIN32
    case 'I':
      /* Non-ANSI integer extensions I32 I64 */
      nếu((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      không_thì nếu((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      không_thì {
#nếu SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#không_thì
        flags |= FLAGS_LONG;
#endif
      }
      break;
#endif /* _WIN32 */
    case 'l':
      nếu(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      không_thì
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
#nếu SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#không_thì
      flags |= FLAGS_LONG;
#endif
      break;
    case 'O':
#nếu SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#không_thì
      flags |= FLAGS_LONG;
#endif
      break;
    case '0':
      nếu(!(flags & FLAGS_LEFT))
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
      nếu(curlx_str_number(&fmt, &num, INT_MAX))
        trả_về PFMT_WIDTH;
      width = (số_nguyên)num;
      break;
    }
    case '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      nếu(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        nếu(width < 0)
          /* illegal combo */
          trả_về PFMT_DOLLARWIDTH;
      }
      không_thì
        /* pick from the next argument */
        width = -1;
      break;
    default:
      loopit = FALSE;
      fmt--;
      break;
    } /* switch */
  } trong_khi(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  trả_về PFMT_OK;
}

static bool parse_conversion(const char f, unsigned số_nguyên *flagp,
                             FormatType *typep)
{
  unsigned số_nguyên flags = *flagp;
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
    nếu(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    không_thì nếu(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    không_thì
      type = MTYPE_INT;
    break;
  case 'u':
    nếu(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    không_thì nếu(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    không_thì
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    break;
  case 'o':
    nếu(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    không_thì nếu(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    không_thì
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    break;
  case 'x':
    nếu(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    không_thì nếu(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    không_thì
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    break;
  case 'X':
    nếu(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    không_thì nếu(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    không_thì
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
    trả_về TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  trả_về FALSE;
}

static số_nguyên parsefmt(const char *format,
                    struct outsegment *out,
                    struct va_input *in,
                    số_nguyên *opieces,
                    số_nguyên *ipieces, va_list arglist)
{
  const char *fmt = format;
  số_nguyên param_num = 0;
  số_nguyên max_param = -1;
  số_nguyên i;
  số_nguyên ocount = 0;
  unsigned char usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  struct outsegment *optr;
  số_nguyên use_dollar = DOLLAR_UNKNOWN;
  const char *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, sizeof(usedinput));

  trong_khi(*fmt) {
    nếu(*fmt == '%') {
      struct va_input *iptr;
      FormatType type;
      unsigned số_nguyên flags = 0;
      số_nguyên width = 0;
      số_nguyên precision = 0;
      số_nguyên param = -1;
      số_nguyên rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      nếu(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        nếu(outlen) {
          optr = &out[ocount++];
          nếu(ocount > MAX_SEGMENTS)
            trả_về PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      nếu(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        nếu(param < 0) {
          nếu(use_dollar == DOLLAR_USE)
            /* illegal combo */
            trả_về PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        không_thì
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      nếu(rc)
        trả_về rc;

      nếu(parse_conversion(*fmt, &flags, &type))
        continue;

      nếu(flags & FLAGS_WIDTHPARAM) {
        nếu(width < 0)
          width = param_num++;
        không_thì {
          /* if this identifies a parameter already used, this is illegal */
          nếu(is_arg_used(usedinput, width))
            trả_về PFMT_WIDTHARG;
        }
        nếu(width >= MAX_PARAMETERS)
          trả_về PFMT_MANYARGS;
        nếu(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      nếu(flags & FLAGS_PRECPARAM) {
        nếu(precision < 0)
          precision = param_num++;
        không_thì {
          /* if this identifies a parameter already used, this is illegal */
          nếu(is_arg_used(usedinput, precision))
            trả_về PFMT_PRECARG;
        }
        nếu(precision >= MAX_PARAMETERS)
          trả_về PFMT_MANYARGS;
        nếu(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      nếu(param < 0)
        param = param_num++;
      nếu(param >= MAX_PARAMETERS)
        trả_về PFMT_MANYARGS;
      nếu(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      nếu(ocount > MAX_SEGMENTS)
        trả_về PFMT_MANYSEGS;
      optr->input = (unsigned số_nguyên)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    không_thì
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  nếu(outlen) {
    optr = &out[ocount++];
    nếu(ocount > MAX_SEGMENTS)
      trả_về PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  cho(i = 0; i < max_param + 1; i++) {
    struct va_input *iptr = &in[i];
    nếu(!is_arg_used(usedinput, i))
      /* bad input */
      trả_về PFMT_INPUTGAP;

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
      iptr->val.numu = va_arg(arglist, unsigned số_nguyên);
      break;

    case MTYPE_INT:
    case MTYPE_WIDTH:
    case MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, số_nguyên);
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

  trả_về PFMT_OK;
}

struct mproperty {
  số_nguyên width;            /* Width of a field. */
  số_nguyên prec;             /* Precision of a field. */
  unsigned số_nguyên flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned char *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef số_nguyên (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       số_nguyên (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       const unsigned char *buf, size_t len, số_nguyên *donep)
{
  nếu(!len)
    trả_về FALSE;
  nếu(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (số_nguyên)n;
    trả_về n != len;
  }
  trong_khi(len--) {
    nếu(stream(*buf++, userp))
      trả_về TRUE;
    (*donep)++;
  }
  trả_về FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const char pad_spaces[] = "                ";
static const char pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       số_nguyên (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       unsigned char pad, số_nguyên num, số_nguyên *donep)
{
  const unsigned char *src;
  nếu(num <= 0)
    trả_về FALSE;
  nếu(pad == '0')
    src = (const unsigned char *)pad_zeros;
  không_thì
    src = (const unsigned char *)pad_spaces;
  trong_khi(num > PAD_RUN) {
    nếu(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      trả_về TRUE;
    num -= PAD_RUN;
  }
  trả_về stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        số_nguyên (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        const char *str, size_t len, số_nguyên *donep)
{
  size_t n = 0;
  trong_khi(n < len && str[n])
    n++;
  trả_về stream_run(userp, stream, streamn,
                    (const unsigned char *)str, n, donep);
}

static bool out_double(void *userp,
                       số_nguyên (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       double dnum,
                       char *work, số_nguyên *donep)
{
  char fmt[32] = "%";
  char *fptr = &fmt[1];
  size_t left = sizeof(fmt) - strlen(fmt);
  số_nguyên flags = p->flags;
  số_nguyên width = p->width;
  số_nguyên prec = p->prec;

  nếu(flags & FLAGS_LEFT)
    *fptr++ = '-';
  nếu(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  nếu(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  nếu(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  nếu(width >= 0) {
    size_t dlen;
    nếu(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  nếu(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    số_nguyên maxprec = BUFFSIZE - 1;
    double val = dnum;
    số_nguyên len;
    nếu(prec > maxprec)
      prec = maxprec - 1;
    nếu(width > 0 && prec <= width)
      maxprec -= width;
    trong_khi(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    nếu(prec > maxprec)
      prec = maxprec - 1;
    nếu(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  nếu(flags & FLAGS_LONG)
    *fptr++ = 'l';

  nếu(flags & FLAGS_FLOATE)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'E' : 'e');
  không_thì nếu(flags & FLAGS_FLOATG)
    *fptr++ = (char)((flags & FLAGS_UPPER) ? 'G' : 'g');
  không_thì
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
#không_thì
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  trả_về stream_run(userp, stream, streamn,
                    (const unsigned char *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       số_nguyên (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       char *work, số_nguyên *donep)
{
  const unsigned char *digits = Curl_ldigits;
  số_nguyên flags = p->flags;
  số_nguyên width = p->width;
  số_nguyên prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  số_nguyên base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  char *workend = &work[BUFFSIZE - 2];
  char *w;

  nếu(flags & FLAGS_CHAR) {
    /* Character. */
    nếu(!(flags & FLAGS_LEFT)) {
      nếu(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        trả_về TRUE;
      width = 1;
    }
    OUTCHAR((char)num);
    nếu(flags & FLAGS_LEFT) {
      nếu(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        trả_về TRUE;
    }
    trả_về FALSE;
  }
  nếu(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  không_thì nếu(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  không_thì nếu(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  không_thì {
    /* Decimal integer. */
    is_neg = (nums < 0);
    nếu(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  nếu(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  case 10:
    trong_khi(num > 0) {
      *w-- = (char)('0' + (num % 10));
      num /= 10;
    }
    break;
  default:
    trong_khi(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    break;
  }
  width -= (số_nguyên)(workend - w);
  prec -= (số_nguyên)(workend - w);

  nếu(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  nếu(prec > 0) {
    width -= prec;
    trong_khi(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  nếu(is_alt && base == 16)
    width -= 2;

  nếu(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  nếu(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    nếu(stream_pad(userp, stream, streamn, ' ', width, donep))
      trả_về TRUE;
    width = 0;
  }

  nếu(is_neg)
    OUTCHAR('-');
  không_thì nếu(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  không_thì nếu(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  nếu(is_alt && base == 16) {
    OUTCHAR('0');
    nếu(flags & FLAGS_UPPER)
      OUTCHAR('X');
    không_thì
      OUTCHAR('x');
  }

  nếu(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    nếu(stream_pad(userp, stream, streamn, '0', width, donep))
      trả_về TRUE;
    width = 0;
  }

  /* Write the number. */
  nếu(stream_run(userp, stream, streamn, (const unsigned char *)w + 1,
                (size_t)(workend - w), donep))
    trả_về TRUE;

  nếu(flags & FLAGS_LEFT) {
    nếu(stream_pad(userp, stream, streamn, ' ', width, donep))
      trả_về TRUE;
  }

  trả_về FALSE;
}

static const char nilstr[] = "(nil)";

static bool out_string(void *userp,
                       số_nguyên (*stream)(unsigned char, void *),
                       mp_streamn streamn,
                       struct mproperty *p,
                       const char *str,
                       số_nguyên *donep)
{
  số_nguyên flags = p->flags;
  số_nguyên width = p->width;
  số_nguyên prec = p->prec;
  size_t len;

  nếu(!str) {
    /* Write null string if there is space. */
    nếu(prec == -1 || prec >= (số_nguyên)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned số_nguyên)FLAGS_ALT;
    }
    không_thì {
      str = "";
      len = 0;
    }
  }
  không_thì nếu(prec != -1)
    len = (size_t)prec;
  không_thì nếu(*str == '\0')
    len = 0;
  không_thì
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (số_nguyên)len;

  nếu(flags & FLAGS_ALT)
    OUTCHAR('"');

  nếu(!(flags & FLAGS_LEFT)) {
    nếu(stream_pad(userp, stream, streamn, ' ', width, donep))
      trả_về TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  nếu(prec == -1) {
    nếu(stream_run(userp, stream, streamn,
                  (const unsigned char *)str, len, donep))
      trả_về TRUE;
  }
  không_thì nếu(stream_zrun(userp, stream, streamn, str, len, donep))
    trả_về TRUE;
  nếu(flags & FLAGS_LEFT) {
    nếu(stream_pad(userp, stream, streamn, ' ', width, donep))
      trả_về TRUE;
  }

  nếu(flags & FLAGS_ALT)
    OUTCHAR('"');

  trả_về FALSE;
}

static bool out_pointer(void *userp,
                        số_nguyên (*stream)(unsigned char, void *),
                        mp_streamn streamn,
                        struct mproperty *p,
                        const char *ptr,
                        char *work,
                        số_nguyên *donep)
{
  /* Generic pointer. */
  nếu(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    nếu(out_number(userp, stream, streamn, p, num, 0, work, donep))
      trả_về TRUE;
  }
  không_thì {
    /* Write "(nil)" for a nil pointer. */
    số_nguyên width = p->width;
    số_nguyên flags = p->flags;

    width -= (số_nguyên)CURL_CSTRLEN(nilstr);
    nếu(flags & FLAGS_LEFT) {
      nếu(stream_pad(userp, stream, streamn, ' ', width, donep))
        trả_về TRUE;
      width = 0;
    }
    nếu(stream_run(userp, stream, streamn, (const unsigned char *)nilstr,
                  sizeof(nilstr) - 1, donep))
      trả_về TRUE;
    nếu(!(flags & FLAGS_LEFT)) {
      nếu(stream_pad(userp, stream, streamn, ' ', width, donep))
        trả_về TRUE;
    }
  }
  trả_về FALSE;
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
static số_nguyên formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   số_nguyên (*stream)(unsigned char, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const char *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  số_nguyên done = 0;   /* number of characters written */
  số_nguyên i;
  số_nguyên ocount = 0; /* number of output segments */
  số_nguyên icount = 0; /* number of input arguments */

  struct outsegment output[MAX_SEGMENTS];
  struct va_input input[MAX_PARAMETERS];
  char work[BUFFSIZE + 2];

  /* Parse the format string */
  nếu(parsefmt(format, output, input, &ocount, &icount, ap_save))
    trả_về 0;

  cho(i = 0; i < ocount; i++) {
    struct outsegment *optr = &output[i];
    struct va_input *iptr = &input[optr->input];
    struct mproperty p;
    size_t outlen = optr->outlen;

    nếu(outlen) {
      nếu(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        trả_về done;
      nếu(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    nếu(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (số_nguyên)input[optr->width].val.nums;
      nếu(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        nếu(p.width == INT_MIN)
          p.width = INT_MAX;
        không_thì
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned số_nguyên)FLAGS_PAD_NIL;
      }
    }
    không_thì
      p.width = optr->width;

    /* pick up the specified precision */
    nếu(p.flags & FLAGS_PRECPARAM) {
      p.prec = (số_nguyên)input[optr->precision].val.nums;
      nếu(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    không_thì nếu(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    không_thì
      p.prec = -1;

    switch(iptr->type) {
    case MTYPE_INTU:
    case MTYPE_LONGU:
    case MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      nếu(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        trả_về done;
      break;

    case MTYPE_INT:
    case MTYPE_LONG:
    case MTYPE_LONGLONG:
      nếu(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        trả_về done;
      break;

    case MTYPE_STRING:
      nếu(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        trả_về done;
      break;

    case MTYPE_PTR:
      nếu(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        trả_về done;
      break;

    case MTYPE_DOUBLE:
    case MTYPE_LONGDOUBLE:
      nếu(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        trả_về done;
      break;

    case MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      nếu(flush && flush(userp))
        trả_về done;
      /* Answer the count of characters written. */
      nếu(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      không_thì nếu(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      không_thì nếu(!(p.flags & FLAGS_SHORT))
        *(số_nguyên *)iptr->val.ptr = done;
      không_thì
        *(short *)iptr->val.ptr = (short)done;
      break;

    default:
      break;
    }
  }
  trả_về done;
}

/* fputc() look-alike */
static số_nguyên addbyter(unsigned char outc, void *f)
{
  struct nsprintf *infop = f;
  nếu(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (char)outc; /* store */
    infop->length++; /* we are now one byte larger */
    trả_về 0;     /* fputc() returns like this on success */
  }
  trả_về 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned char *buf, size_t len, void *f)
{
  struct nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  nếu(len > fit)
    len = fit;
  nếu(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  trả_về len;
}

số_nguyên curl_mvsnprintf(char *buffer, size_t maxlength, const char *format,
                    va_list args)
{
  số_nguyên retcode;
  struct nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  nếu(info.max) {
    /* we null-terminate this */
    nếu(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    không_thì
      info.buffer[0] = 0;
  }
  trả_về retcode;
}

số_nguyên curl_msnprintf(char *buffer, size_t maxlength, const char *format, ...)
{
  số_nguyên retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  trả_về retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static số_nguyên alloc_flush(void *f)
{
  struct asprintf *infop = f;
  nếu(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    nếu(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      trả_về 1; /* fail */
    }
  }
  trả_về 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static số_nguyên alloc_addn(struct asprintf *infop, const unsigned char *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  nếu(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    trả_về 1; /* fail */
  }
  trả_về 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(struct asprintf *infop, size_t more)
{
  trả_về (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static số_nguyên alloc_addbyter(unsigned char outc, void *f)
{
  struct asprintf *infop = f;
  nếu(infop->nstage == sizeof(infop->stage) && alloc_flush(infop))
    trả_về 1; /* fail */
  nếu(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    nếu(alloc_flush(infop))
      trả_về 1;
    trả_về alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  trả_về 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned char *buf, size_t len, void *f)
{
  struct asprintf *infop = f;
  size_t accepted = 0;
  trong_khi(accepted < len) {
    size_t room = sizeof(infop->stage) - infop->nstage;
    size_t n;
    nếu(!room) {
      nếu(alloc_flush(infop))
        trả_về accepted; /* accepted < len aborts formatting */
      room = sizeof(infop->stage);
    }
    n = len - accepted;
    nếu(n > room)
      n = room;
    nếu(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      nếu(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      nếu(alloc_flush(infop))
        trả_về accepted;
      nếu(alloc_addn(infop, buf + accepted, 1))
        trả_về accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  trả_về accepted;
}

/* appends the formatted string, returns MERR error code */
số_nguyên curlx_dyn_vprintf(struct dynbuf *dyn, const char *format, va_list args)
{
  struct asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  nếu(!info.merr)
    (void)alloc_flush(&info);
  nếu(info.merr) {
    curlx_dyn_free(info.b);
    trả_về info.merr;
  }
  trả_về 0;
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
  nếu(!info.merr)
    (void)alloc_flush(&info);
  nếu(info.merr) {
    curlx_dyn_free(info.b);
    trả_về NULL;
  }
  nếu(curlx_dyn_len(info.b))
    trả_về curlx_dyn_ptr(info.b);
  trả_về curlx_strdup("");
}

char *curl_maprintf(const char *format, ...)
{
  va_list args;
  char *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  trả_về s;
}

static số_nguyên storebuffer(unsigned char outc, void *f)
{
  char **buffer = f;
  **buffer = (char)outc;
  (*buffer)++;
  trả_về 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned char *buf, size_t len, void *f)
{
  char **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  trả_về len;
}

số_nguyên curl_msprintf(char *buffer, const char *format, ...)
{
  va_list args; /* argument pointer */
  số_nguyên retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  trả_về retcode;
}

static số_nguyên fputc_wrapper(unsigned char outc, void *f)
{
  struct ioprintf *i = f;
  số_nguyên out = outc;
  FILE *s = i->fd;
  số_nguyên rc = fputc(out, s);
  i->error = (rc == EOF);
  trả_về (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned char *buf, size_t len, void *f)
{
  struct ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  trả_về n;
}

số_nguyên curl_mvfprintf(FILE *fd, const char *format, va_list args)
{
  struct ioprintf info;
  số_nguyên n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  nếu(info.error)
    trả_về -1;
  trả_về n;
}

số_nguyên curl_mfprintf(FILE *fd, const char *format, ...)
{
  va_list args; /* argument pointer */
  số_nguyên n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  trả_về n;
}

số_nguyên curl_mprintf(const char *format, ...)
{
  va_list args; /* argument pointer */
  số_nguyên n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  trả_về n;
}

số_nguyên curl_mvsprintf(char *buffer, const char *format, va_list args)
{
  số_nguyên retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  trả_về retcode;
}

số_nguyên curl_mvprintf(const char *format, va_list args)
{
  trả_về curl_mvfprintf(stdout, format, args);
}

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
const unsigned var_na Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
const unsigned var_na Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  do {                                    \
    ប្រសិនបើ(stream((unsigned var_na)(x), userp)) \
      ត្រឡប់ TRUE;                        \
    (*donep)++;                           \
  } ខណៈពេល(0)

/* Data type to read from the arglist */
typedef ga_nanaa {
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
ga_nanaa {
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

ga_nanaa {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
sa_nracanaa va_input {
  FormatType type; /* FormatType */
  sa_ngha {
    const var_na *str;
    void *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    dviguu_na dnum;
  } val;
};

/*
 * Describes an output segment.
 */
sa_nracanaa outsegment {
  ចំនួនគត់ width;     /* width OR width parameter number */
  ចំនួនគត់ precision; /* precision OR precision parameter number */
  unsigned ចំនួនគត់ flags;
  unsigned ចំនួនគត់ input; /* input argument array index */
  const var_na *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

sa_nracanaa nsprintf {
  var_na *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

sa_nracanaa asprintf {
  sa_nracanaa dynbuf *b;
  var_na merr;
  size_t nstage;
  unsigned var_na stage[ASPRINTF_STAGE_SIZE];
};

sa_nracanaa ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
static ចំនួនគត់ dollarstring(const var_na *p, const var_na **end)
{
  curl_off_t num;
  ប្រសិនបើ(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    ត្រឡប់ -1;
  *end = p;
  ត្រឡប់ (ចំនួនគត់)num - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned var_na)(1 << ((y) & 7)))

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

static ចំនួនគត់ parse_flags(const var_na **fmtp, unsigned ចំនួនគត់ *flagsp, ចំនួនគត់ use_dollar,
                       ចំនួនគត់ *precp, ចំនួនគត់ *widthp)
{
  const var_na *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned ចំនួនគត់ flags = 0;
  ចំនួនគត់ width = 0;
  ចំនួនគត់ precision = 0;

  /* Handle the flags */
  do {
    switch(*fmt++) {
    pak_sa ' ':
      flags |= FLAGS_SPACE;
      bha_nga;
    pak_sa '+':
      flags |= FLAGS_SHOWSIGN;
      bha_nga;
    pak_sa '-':
      flags |= FLAGS_LEFT;
      flags &= ~(unsigned ចំនួនគត់)FLAGS_PAD_NIL;
      bha_nga;
    pak_sa '#':
      flags |= FLAGS_ALT;
      bha_nga;
    pak_sa '.':
      ប្រសិនបើ('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ប្រសិនបើ(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ប្រសិនបើ(precision < 0)
            /* illegal combo */
            ត្រឡប់ PFMT_DOLLARPREC;
        }
        បើមិនដូច្នេះ
          /* get it from the next argument */
          precision = -1;
      }
      បើមិនដូច្នេះ {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ប្រសិនបើ(is_neg)
          fmt++;
        ប្រសិនបើ(curlx_str_number(&fmt, &num, INT_MAX))
          ត្រឡប់ PFMT_PREC;
        precision = (ចំនួនគត់)num;
        ប្រសិនបើ(is_neg)
          precision = -precision;
      }
      ប្រសិនបើ((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        ត្រឡប់ PFMT_PRECMIX;
      bha_nga;
    pak_sa 'h':
      flags |= FLAGS_SHORT;
      bha_nga;
#ifdef _WIN32
    pak_sa 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ប្រសិនបើ((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      បើមិនដូច្នេះ ប្រសិនបើ((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      បើមិនដូច្នេះ {
#ប្រសិនបើ SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#បើមិនដូច្នេះ
        flags |= FLAGS_LONG;
#endif
      }
      bha_nga;
#endif /* _WIN32 */
    pak_sa 'l':
      ប្រសិនបើ(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      បើមិនដូច្នេះ
        flags |= FLAGS_LONG;
      bha_nga;
    pak_sa 'L':
      flags |= FLAGS_LONGDOUBLE;
      bha_nga;
    pak_sa 'q':
      flags |= FLAGS_LONGLONG;
      bha_nga;
    pak_sa 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#ប្រសិនបើ SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#បើមិនដូច្នេះ
      flags |= FLAGS_LONG;
#endif
      bha_nga;
    pak_sa 'O':
#ប្រសិនបើ SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#បើមិនដូច្នេះ
      flags |= FLAGS_LONG;
#endif
      bha_nga;
    pak_sa '0':
      ប្រសិនបើ(!(flags & FLAGS_LEFT))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    pak_sa '1':
    pak_sa '2':
    pak_sa '3':
    pak_sa '4':
    pak_sa '5':
    pak_sa '6':
    pak_sa '7':
    pak_sa '8':
    pak_sa '9': {
      curl_off_t num;
      flags |= FLAGS_WIDTH;
      fmt--;
      ប្រសិនបើ(curlx_str_number(&fmt, &num, INT_MAX))
        ត្រឡប់ PFMT_WIDTH;
      width = (ចំនួនគត់)num;
      bha_nga;
    }
    pak_sa '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ប្រសិនបើ(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        ប្រសិនបើ(width < 0)
          /* illegal combo */
          ត្រឡប់ PFMT_DOLLARWIDTH;
      }
      បើមិនដូច្នេះ
        /* pick from the next argument */
        width = -1;
      bha_nga;
    default:
      loopit = FALSE;
      fmt--;
      bha_nga;
    } /* switch */
  } ខណៈពេល(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  ត្រឡប់ PFMT_OK;
}

static bool parse_conversion(const var_na f, unsigned ចំនួនគត់ *flagp,
                             FormatType *typep)
{
  unsigned ចំនួនគត់ flags = *flagp;
  FormatType type;
  switch(f) {
  pak_sa 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    bha_nga;
  pak_sa 's':
    type = MTYPE_STRING;
    bha_nga;
  pak_sa 'n':
    type = MTYPE_INTPTR;
    bha_nga;
  pak_sa 'p':
    type = MTYPE_PTR;
    bha_nga;
  pak_sa 'd':
  pak_sa 'i':
    ប្រសិនបើ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    បើមិនដូច្នេះ
      type = MTYPE_INT;
    bha_nga;
  pak_sa 'u':
    ប្រសិនបើ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    បើមិនដូច្នេះ
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'o':
    ប្រសិនបើ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    បើមិនដូច្នេះ
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'x':
    ប្រសិនបើ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    បើមិនដូច្នេះ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'X':
    ប្រសិនបើ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    បើមិនដូច្នេះ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    bha_nga;
  pak_sa 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    bha_nga;
  pak_sa 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    bha_nga;
  pak_sa 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    bha_nga;
  pak_sa 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    bha_nga;
  pak_sa 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    bha_nga;
  pak_sa 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    bha_nga;
  default:
    /* invalid instruction, disregard and continue */
    ត្រឡប់ TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  ត្រឡប់ FALSE;
}

static ចំនួនគត់ parsefmt(const var_na *format,
                    sa_nracanaa outsegment *out,
                    sa_nracanaa va_input *in,
                    ចំនួនគត់ *opieces,
                    ចំនួនគត់ *ipieces, va_list arglist)
{
  const var_na *fmt = format;
  ចំនួនគត់ param_num = 0;
  ចំនួនគត់ max_param = -1;
  ចំនួនគត់ i;
  ចំនួនគត់ ocount = 0;
  unsigned var_na usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  sa_nracanaa outsegment *optr;
  ចំនួនគត់ use_dollar = DOLLAR_UNKNOWN;
  const var_na *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, parimaa_na(usedinput));

  ខណៈពេល(*fmt) {
    ប្រសិនបើ(*fmt == '%') {
      sa_nracanaa va_input *iptr;
      FormatType type;
      unsigned ចំនួនគត់ flags = 0;
      ចំនួនគត់ width = 0;
      ចំនួនគត់ precision = 0;
      ចំនួនគត់ param = -1;
      ចំនួនគត់ rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      ប្រសិនបើ(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ប្រសិនបើ(outlen) {
          optr = &out[ocount++];
          ប្រសិនបើ(ocount > MAX_SEGMENTS)
            ត្រឡប់ PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      ប្រសិនបើ(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ប្រសិនបើ(param < 0) {
          ប្រសិនបើ(use_dollar == DOLLAR_USE)
            /* illegal combo */
            ត្រឡប់ PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        បើមិនដូច្នេះ
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      ប្រសិនបើ(rc)
        ត្រឡប់ rc;

      ប្រសិនបើ(parse_conversion(*fmt, &flags, &type))
        continue;

      ប្រសិនបើ(flags & FLAGS_WIDTHPARAM) {
        ប្រសិនបើ(width < 0)
          width = param_num++;
        បើមិនដូច្នេះ {
          /* if this identifies a parameter already used, this is illegal */
          ប្រសិនបើ(is_arg_used(usedinput, width))
            ត្រឡប់ PFMT_WIDTHARG;
        }
        ប្រសិនបើ(width >= MAX_PARAMETERS)
          ត្រឡប់ PFMT_MANYARGS;
        ប្រសិនបើ(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      ប្រសិនបើ(flags & FLAGS_PRECPARAM) {
        ប្រសិនបើ(precision < 0)
          precision = param_num++;
        បើមិនដូច្នេះ {
          /* if this identifies a parameter already used, this is illegal */
          ប្រសិនបើ(is_arg_used(usedinput, precision))
            ត្រឡប់ PFMT_PRECARG;
        }
        ប្រសិនបើ(precision >= MAX_PARAMETERS)
          ត្រឡប់ PFMT_MANYARGS;
        ប្រសិនបើ(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      ប្រសិនបើ(param < 0)
        param = param_num++;
      ប្រសិនបើ(param >= MAX_PARAMETERS)
        ត្រឡប់ PFMT_MANYARGS;
      ប្រសិនបើ(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      ប្រសិនបើ(ocount > MAX_SEGMENTS)
        ត្រឡប់ PFMT_MANYSEGS;
      optr->input = (unsigned ចំនួនគត់)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    បើមិនដូច្នេះ
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  ប្រសិនបើ(outlen) {
    optr = &out[ocount++];
    ប្រសិនបើ(ocount > MAX_SEGMENTS)
      ត្រឡប់ PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  សម្រាប់(i = 0; i < max_param + 1; i++) {
    sa_nracanaa va_input *iptr = &in[i];
    ប្រសិនបើ(!is_arg_used(usedinput, i))
      /* bad input */
      ត្រឡប់ PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    switch(iptr->type) {
    pak_sa MTYPE_STRING:
      iptr->val.str = va_arg(arglist, const var_na *);
      bha_nga;

    pak_sa MTYPE_INTPTR:
    pak_sa MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, void *);
      bha_nga;

    pak_sa MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      bha_nga;

    pak_sa MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      bha_nga;

    pak_sa MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      bha_nga;

    pak_sa MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      bha_nga;

    pak_sa MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned ចំនួនគត់);
      bha_nga;

    pak_sa MTYPE_INT:
    pak_sa MTYPE_WIDTH:
    pak_sa MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, ចំនួនគត់);
      bha_nga;

    pak_sa MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, dviguu_na);
      bha_nga;

    pak_sa MTYPE_LONGDOUBLE:
      iptr->val.dnum = (dviguu_na)va_arg(arglist, long dviguu_na);
      bha_nga;

    default:
      DEBUGASSERT(NULL); /* unexpected */
      bha_nga;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  ត្រឡប់ PFMT_OK;
}

sa_nracanaa mproperty {
  ចំនួនគត់ width;            /* Width of a field. */
  ចំនួនគត់ prec;             /* Precision of a field. */
  unsigned ចំនួនគត់ flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned var_na *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef ចំនួនគត់ (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       ចំនួនគត់ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       const unsigned var_na *buf, size_t len, ចំនួនគត់ *donep)
{
  ប្រសិនបើ(!len)
    ត្រឡប់ FALSE;
  ប្រសិនបើ(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (ចំនួនគត់)n;
    ត្រឡប់ n != len;
  }
  ខណៈពេល(len--) {
    ប្រសិនបើ(stream(*buf++, userp))
      ត្រឡប់ TRUE;
    (*donep)++;
  }
  ត្រឡប់ FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const var_na pad_spaces[] = "                ";
static const var_na pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       ចំនួនគត់ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       unsigned var_na pad, ចំនួនគត់ num, ចំនួនគត់ *donep)
{
  const unsigned var_na *src;
  ប្រសិនបើ(num <= 0)
    ត្រឡប់ FALSE;
  ប្រសិនបើ(pad == '0')
    src = (const unsigned var_na *)pad_zeros;
  បើមិនដូច្នេះ
    src = (const unsigned var_na *)pad_spaces;
  ខណៈពេល(num > PAD_RUN) {
    ប្រសិនបើ(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      ត្រឡប់ TRUE;
    num -= PAD_RUN;
  }
  ត្រឡប់ stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        ចំនួនគត់ (*stream)(unsigned var_na, void *),
                        mp_streamn streamn,
                        const var_na *str, size_t len, ចំនួនគត់ *donep)
{
  size_t n = 0;
  ខណៈពេល(n < len && str[n])
    n++;
  ត្រឡប់ stream_run(userp, stream, streamn,
                    (const unsigned var_na *)str, n, donep);
}

static bool out_double(void *userp,
                       ចំនួនគត់ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       dviguu_na dnum,
                       var_na *work, ចំនួនគត់ *donep)
{
  var_na fmt[32] = "%";
  var_na *fptr = &fmt[1];
  size_t left = parimaa_na(fmt) - strlen(fmt);
  ចំនួនគត់ flags = p->flags;
  ចំនួនគត់ width = p->width;
  ចំនួនគត់ prec = p->prec;

  ប្រសិនបើ(flags & FLAGS_LEFT)
    *fptr++ = '-';
  ប្រសិនបើ(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ប្រសិនបើ(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ប្រសិនបើ(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ប្រសិនបើ(width >= 0) {
    size_t dlen;
    ប្រសិនបើ(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  ប្រសិនបើ(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    ចំនួនគត់ maxprec = BUFFSIZE - 1;
    dviguu_na val = dnum;
    ចំនួនគត់ len;
    ប្រសិនបើ(prec > maxprec)
      prec = maxprec - 1;
    ប្រសិនបើ(width > 0 && prec <= width)
      maxprec -= width;
    ខណៈពេល(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ប្រសិនបើ(prec > maxprec)
      prec = maxprec - 1;
    ប្រសិនបើ(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  ប្រសិនបើ(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ប្រសិនបើ(flags & FLAGS_FLOATE)
    *fptr++ = (var_na)((flags & FLAGS_UPPER) ? 'E' : 'e');
  បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_FLOATG)
    *fptr++ = (var_na)((flags & FLAGS_UPPER) ? 'G' : 'g');
  បើមិនដូច្នេះ
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
#បើមិនដូច្នេះ
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  ត្រឡប់ stream_run(userp, stream, streamn,
                    (const unsigned var_na *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       ចំនួនគត់ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       var_na *work, ចំនួនគត់ *donep)
{
  const unsigned var_na *digits = Curl_ldigits;
  ចំនួនគត់ flags = p->flags;
  ចំនួនគត់ width = p->width;
  ចំនួនគត់ prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  ចំនួនគត់ base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  var_na *workend = &work[BUFFSIZE - 2];
  var_na *w;

  ប្រសិនបើ(flags & FLAGS_CHAR) {
    /* Character. */
    ប្រសិនបើ(!(flags & FLAGS_LEFT)) {
      ប្រសិនបើ(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ត្រឡប់ TRUE;
      width = 1;
    }
    OUTCHAR((var_na)num);
    ប្រសិនបើ(flags & FLAGS_LEFT) {
      ប្រសិនបើ(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ត្រឡប់ TRUE;
    }
    ត្រឡប់ FALSE;
  }
  ប្រសិនបើ(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  បើមិនដូច្នេះ {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ប្រសិនបើ(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ប្រសិនបើ(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  pak_sa 10:
    ខណៈពេល(num > 0) {
      *w-- = (var_na)('0' + (num % 10));
      num /= 10;
    }
    bha_nga;
  default:
    ខណៈពេល(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    bha_nga;
  }
  width -= (ចំនួនគត់)(workend - w);
  prec -= (ចំនួនគត់)(workend - w);

  ប្រសិនបើ(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  ប្រសិនបើ(prec > 0) {
    width -= prec;
    ខណៈពេល(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ប្រសិនបើ(is_alt && base == 16)
    width -= 2;

  ប្រសិនបើ(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  ប្រសិនបើ(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    ប្រសិនបើ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ត្រឡប់ TRUE;
    width = 0;
  }

  ប្រសិនបើ(is_neg)
    OUTCHAR('-');
  បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  បើមិនដូច្នេះ ប្រសិនបើ(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ប្រសិនបើ(is_alt && base == 16) {
    OUTCHAR('0');
    ប្រសិនបើ(flags & FLAGS_UPPER)
      OUTCHAR('X');
    បើមិនដូច្នេះ
      OUTCHAR('x');
  }

  ប្រសិនបើ(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    ប្រសិនបើ(stream_pad(userp, stream, streamn, '0', width, donep))
      ត្រឡប់ TRUE;
    width = 0;
  }

  /* Write the number. */
  ប្រសិនបើ(stream_run(userp, stream, streamn, (const unsigned var_na *)w + 1,
                (size_t)(workend - w), donep))
    ត្រឡប់ TRUE;

  ប្រសិនបើ(flags & FLAGS_LEFT) {
    ប្រសិនបើ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ត្រឡប់ TRUE;
  }

  ត្រឡប់ FALSE;
}

static const var_na nilstr[] = "(nil)";

static bool out_string(void *userp,
                       ចំនួនគត់ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       const var_na *str,
                       ចំនួនគត់ *donep)
{
  ចំនួនគត់ flags = p->flags;
  ចំនួនគត់ width = p->width;
  ចំនួនគត់ prec = p->prec;
  size_t len;

  ប្រសិនបើ(!str) {
    /* Write null string if there is space. */
    ប្រសិនបើ(prec == -1 || prec >= (ចំនួនគត់)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned ចំនួនគត់)FLAGS_ALT;
    }
    បើមិនដូច្នេះ {
      str = "";
      len = 0;
    }
  }
  បើមិនដូច្នេះ ប្រសិនបើ(prec != -1)
    len = (size_t)prec;
  បើមិនដូច្នេះ ប្រសិនបើ(*str == '\0')
    len = 0;
  បើមិនដូច្នេះ
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (ចំនួនគត់)len;

  ប្រសិនបើ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ប្រសិនបើ(!(flags & FLAGS_LEFT)) {
    ប្រសិនបើ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ត្រឡប់ TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ប្រសិនបើ(prec == -1) {
    ប្រសិនបើ(stream_run(userp, stream, streamn,
                  (const unsigned var_na *)str, len, donep))
      ត្រឡប់ TRUE;
  }
  បើមិនដូច្នេះ ប្រសិនបើ(stream_zrun(userp, stream, streamn, str, len, donep))
    ត្រឡប់ TRUE;
  ប្រសិនបើ(flags & FLAGS_LEFT) {
    ប្រសិនបើ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ត្រឡប់ TRUE;
  }

  ប្រសិនបើ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ត្រឡប់ FALSE;
}

static bool out_pointer(void *userp,
                        ចំនួនគត់ (*stream)(unsigned var_na, void *),
                        mp_streamn streamn,
                        sa_nracanaa mproperty *p,
                        const var_na *ptr,
                        var_na *work,
                        ចំនួនគត់ *donep)
{
  /* Generic pointer. */
  ប្រសិនបើ(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ប្រសិនបើ(out_number(userp, stream, streamn, p, num, 0, work, donep))
      ត្រឡប់ TRUE;
  }
  បើមិនដូច្នេះ {
    /* Write "(nil)" for a nil pointer. */
    ចំនួនគត់ width = p->width;
    ចំនួនគត់ flags = p->flags;

    width -= (ចំនួនគត់)CURL_CSTRLEN(nilstr);
    ប្រសិនបើ(flags & FLAGS_LEFT) {
      ប្រសិនបើ(stream_pad(userp, stream, streamn, ' ', width, donep))
        ត្រឡប់ TRUE;
      width = 0;
    }
    ប្រសិនបើ(stream_run(userp, stream, streamn, (const unsigned var_na *)nilstr,
                  parimaa_na(nilstr) - 1, donep))
      ត្រឡប់ TRUE;
    ប្រសិនបើ(!(flags & FLAGS_LEFT)) {
      ប្រសិនបើ(stream_pad(userp, stream, streamn, ' ', width, donep))
        ត្រឡប់ TRUE;
    }
  }
  ត្រឡប់ FALSE;
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
static ចំនួនគត់ formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   ចំនួនគត់ (*stream)(unsigned var_na, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const var_na *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  ចំនួនគត់ done = 0;   /* number of characters written */
  ចំនួនគត់ i;
  ចំនួនគត់ ocount = 0; /* number of output segments */
  ចំនួនគត់ icount = 0; /* number of input arguments */

  sa_nracanaa outsegment output[MAX_SEGMENTS];
  sa_nracanaa va_input input[MAX_PARAMETERS];
  var_na work[BUFFSIZE + 2];

  /* Parse the format string */
  ប្រសិនបើ(parsefmt(format, output, input, &ocount, &icount, ap_save))
    ត្រឡប់ 0;

  សម្រាប់(i = 0; i < ocount; i++) {
    sa_nracanaa outsegment *optr = &output[i];
    sa_nracanaa va_input *iptr = &input[optr->input];
    sa_nracanaa mproperty p;
    size_t outlen = optr->outlen;

    ប្រសិនបើ(outlen) {
      ប្រសិនបើ(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        ត្រឡប់ done;
      ប្រសិនបើ(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ប្រសិនបើ(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (ចំនួនគត់)input[optr->width].val.nums;
      ប្រសិនបើ(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ប្រសិនបើ(p.width == INT_MIN)
          p.width = INT_MAX;
        បើមិនដូច្នេះ
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned ចំនួនគត់)FLAGS_PAD_NIL;
      }
    }
    បើមិនដូច្នេះ
      p.width = optr->width;

    /* pick up the specified precision */
    ប្រសិនបើ(p.flags & FLAGS_PRECPARAM) {
      p.prec = (ចំនួនគត់)input[optr->precision].val.nums;
      ប្រសិនបើ(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    បើមិនដូច្នេះ ប្រសិនបើ(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    បើមិនដូច្នេះ
      p.prec = -1;

    switch(iptr->type) {
    pak_sa MTYPE_INTU:
    pak_sa MTYPE_LONGU:
    pak_sa MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ប្រសិនបើ(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        ត្រឡប់ done;
      bha_nga;

    pak_sa MTYPE_INT:
    pak_sa MTYPE_LONG:
    pak_sa MTYPE_LONGLONG:
      ប្រសិនបើ(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        ត្រឡប់ done;
      bha_nga;

    pak_sa MTYPE_STRING:
      ប្រសិនបើ(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        ត្រឡប់ done;
      bha_nga;

    pak_sa MTYPE_PTR:
      ប្រសិនបើ(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        ត្រឡប់ done;
      bha_nga;

    pak_sa MTYPE_DOUBLE:
    pak_sa MTYPE_LONGDOUBLE:
      ប្រសិនបើ(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        ត្រឡប់ done;
      bha_nga;

    pak_sa MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ប្រសិនបើ(flush && flush(userp))
        ត្រឡប់ done;
      /* Answer the count of characters written. */
      ប្រសិនបើ(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      បើមិនដូច្នេះ ប្រសិនបើ(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      បើមិនដូច្នេះ ប្រសិនបើ(!(p.flags & FLAGS_SHORT))
        *(ចំនួនគត់ *)iptr->val.ptr = done;
      បើមិនដូច្នេះ
        *(short *)iptr->val.ptr = (short)done;
      bha_nga;

    default:
      bha_nga;
    }
  }
  ត្រឡប់ done;
}

/* fputc() look-alike */
static ចំនួនគត់ addbyter(unsigned var_na outc, void *f)
{
  sa_nracanaa nsprintf *infop = f;
  ប្រសិនបើ(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (var_na)outc; /* store */
    infop->length++; /* we are now one byte larger */
    ត្រឡប់ 0;     /* fputc() returns like this on success */
  }
  ត្រឡប់ 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  ប្រសិនបើ(len > fit)
    len = fit;
  ប្រសិនបើ(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  ត្រឡប់ len;
}

ចំនួនគត់ curl_mvsnprintf(var_na *buffer, size_t maxlength, const var_na *format,
                    va_list args)
{
  ចំនួនគត់ retcode;
  sa_nracanaa nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  ប្រសិនបើ(info.max) {
    /* we null-terminate this */
    ប្រសិនបើ(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    បើមិនដូច្នេះ
      info.buffer[0] = 0;
  }
  ត្រឡប់ retcode;
}

ចំនួនគត់ curl_msnprintf(var_na *buffer, size_t maxlength, const var_na *format, ...)
{
  ចំនួនគត់ retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  ត្រឡប់ retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static ចំនួនគត់ alloc_flush(void *f)
{
  sa_nracanaa asprintf *infop = f;
  ប្រសិនបើ(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ប្រសិនបើ(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      ត្រឡប់ 1; /* fail */
    }
  }
  ត្រឡប់ 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static ចំនួនគត់ alloc_addn(sa_nracanaa asprintf *infop, const unsigned var_na *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  ប្រសិនបើ(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    ត្រឡប់ 1; /* fail */
  }
  ត្រឡប់ 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(sa_nracanaa asprintf *infop, size_t more)
{
  ត្រឡប់ (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static ចំនួនគត់ alloc_addbyter(unsigned var_na outc, void *f)
{
  sa_nracanaa asprintf *infop = f;
  ប្រសិនបើ(infop->nstage == parimaa_na(infop->stage) && alloc_flush(infop))
    ត្រឡប់ 1; /* fail */
  ប្រសិនបើ(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ប្រសិនបើ(alloc_flush(infop))
      ត្រឡប់ 1;
    ត្រឡប់ alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  ត្រឡប់ 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa asprintf *infop = f;
  size_t accepted = 0;
  ខណៈពេល(accepted < len) {
    size_t room = parimaa_na(infop->stage) - infop->nstage;
    size_t n;
    ប្រសិនបើ(!room) {
      ប្រសិនបើ(alloc_flush(infop))
        ត្រឡប់ accepted; /* accepted < len aborts formatting */
      room = parimaa_na(infop->stage);
    }
    n = len - accepted;
    ប្រសិនបើ(n > room)
      n = room;
    ប្រសិនបើ(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ប្រសិនបើ(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      ប្រសិនបើ(alloc_flush(infop))
        ត្រឡប់ accepted;
      ប្រសិនបើ(alloc_addn(infop, buf + accepted, 1))
        ត្រឡប់ accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  ត្រឡប់ accepted;
}

/* appends the formatted string, returns MERR error code */
ចំនួនគត់ curlx_dyn_vprintf(sa_nracanaa dynbuf *dyn, const var_na *format, va_list args)
{
  sa_nracanaa asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ប្រសិនបើ(!info.merr)
    (void)alloc_flush(&info);
  ប្រសិនបើ(info.merr) {
    curlx_dyn_free(info.b);
    ត្រឡប់ info.merr;
  }
  ត្រឡប់ 0;
}

var_na *curl_mvaprintf(const var_na *format, va_list args)
{
  sa_nracanaa asprintf info;
  sa_nracanaa dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ប្រសិនបើ(!info.merr)
    (void)alloc_flush(&info);
  ប្រសិនបើ(info.merr) {
    curlx_dyn_free(info.b);
    ត្រឡប់ NULL;
  }
  ប្រសិនបើ(curlx_dyn_len(info.b))
    ត្រឡប់ curlx_dyn_ptr(info.b);
  ត្រឡប់ curlx_strdup("");
}

var_na *curl_maprintf(const var_na *format, ...)
{
  va_list args;
  var_na *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  ត្រឡប់ s;
}

static ចំនួនគត់ storebuffer(unsigned var_na outc, void *f)
{
  var_na **buffer = f;
  **buffer = (var_na)outc;
  (*buffer)++;
  ត្រឡប់ 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned var_na *buf, size_t len, void *f)
{
  var_na **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  ត្រឡប់ len;
}

ចំនួនគត់ curl_msprintf(var_na *buffer, const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ចំនួនគត់ retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  ត្រឡប់ retcode;
}

static ចំនួនគត់ fputc_wrapper(unsigned var_na outc, void *f)
{
  sa_nracanaa ioprintf *i = f;
  ចំនួនគត់ out = outc;
  FILE *s = i->fd;
  ចំនួនគត់ rc = fputc(out, s);
  i->error = (rc == EOF);
  ត្រឡប់ (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  ត្រឡប់ n;
}

ចំនួនគត់ curl_mvfprintf(FILE *fd, const var_na *format, va_list args)
{
  sa_nracanaa ioprintf info;
  ចំនួនគត់ n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  ប្រសិនបើ(info.error)
    ត្រឡប់ -1;
  ត្រឡប់ n;
}

ចំនួនគត់ curl_mfprintf(FILE *fd, const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ចំនួនគត់ n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  ត្រឡប់ n;
}

ចំនួនគត់ curl_mprintf(const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ចំនួនគត់ n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  ត្រឡប់ n;
}

ចំនួនគត់ curl_mvsprintf(var_na *buffer, const var_na *format, va_list args)
{
  ចំនួនគត់ retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  ត្រឡប់ retcode;
}

ចំនួនគត់ curl_mvprintf(const var_na *format, va_list args)
{
  ត្រឡប់ curl_mvfprintf(stdout, format, args);
}

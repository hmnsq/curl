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
    အကယ်၍(stream((unsigned var_na)(x), userp)) \
      ပြန်ပေး TRUE;                        \
    (*donep)++;                           \
  } နေစဉ်(0)

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
  ကိန်းပြည့် width;     /* width OR width parameter number */
  ကိန်းပြည့် precision; /* precision OR precision parameter number */
  unsigned ကိန်းပြည့် flags;
  unsigned ကိန်းပြည့် input; /* input argument array index */
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
static ကိန်းပြည့် dollarstring(const var_na *p, const var_na **end)
{
  curl_off_t num;
  အကယ်၍(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    ပြန်ပေး -1;
  *end = p;
  ပြန်ပေး (ကိန်းပြည့်)num - 1;
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

static ကိန်းပြည့် parse_flags(const var_na **fmtp, unsigned ကိန်းပြည့် *flagsp, ကိန်းပြည့် use_dollar,
                       ကိန်းပြည့် *precp, ကိန်းပြည့် *widthp)
{
  const var_na *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned ကိန်းပြည့် flags = 0;
  ကိန်းပြည့် width = 0;
  ကိန်းပြည့် precision = 0;

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
      flags &= ~(unsigned ကိန်းပြည့်)FLAGS_PAD_NIL;
      bha_nga;
    pak_sa '#':
      flags |= FLAGS_ALT;
      bha_nga;
    pak_sa '.':
      အကယ်၍('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        အကယ်၍(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          အကယ်၍(precision < 0)
            /* illegal combo */
            ပြန်ပေး PFMT_DOLLARPREC;
        }
        မဟုတ်ပါက
          /* get it from the next argument */
          precision = -1;
      }
      မဟုတ်ပါက {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        အကယ်၍(is_neg)
          fmt++;
        အကယ်၍(curlx_str_number(&fmt, &num, INT_MAX))
          ပြန်ပေး PFMT_PREC;
        precision = (ကိန်းပြည့်)num;
        အကယ်၍(is_neg)
          precision = -precision;
      }
      အကယ်၍((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        ပြန်ပေး PFMT_PRECMIX;
      bha_nga;
    pak_sa 'h':
      flags |= FLAGS_SHORT;
      bha_nga;
#ifdef _WIN32
    pak_sa 'I':
      /* Non-ANSI integer extensions I32 I64 */
      အကယ်၍((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      မဟုတ်ပါက အကယ်၍((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      မဟုတ်ပါက {
#အကယ်၍ SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#မဟုတ်ပါက
        flags |= FLAGS_LONG;
#endif
      }
      bha_nga;
#endif /* _WIN32 */
    pak_sa 'l':
      အကယ်၍(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      မဟုတ်ပါက
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
#အကယ်၍ SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#မဟုတ်ပါက
      flags |= FLAGS_LONG;
#endif
      bha_nga;
    pak_sa 'O':
#အကယ်၍ SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#မဟုတ်ပါက
      flags |= FLAGS_LONG;
#endif
      bha_nga;
    pak_sa '0':
      အကယ်၍(!(flags & FLAGS_LEFT))
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
      အကယ်၍(curlx_str_number(&fmt, &num, INT_MAX))
        ပြန်ပေး PFMT_WIDTH;
      width = (ကိန်းပြည့်)num;
      bha_nga;
    }
    pak_sa '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      အကယ်၍(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        အကယ်၍(width < 0)
          /* illegal combo */
          ပြန်ပေး PFMT_DOLLARWIDTH;
      }
      မဟုတ်ပါက
        /* pick from the next argument */
        width = -1;
      bha_nga;
    default:
      loopit = FALSE;
      fmt--;
      bha_nga;
    } /* switch */
  } နေစဉ်(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  ပြန်ပေး PFMT_OK;
}

static bool parse_conversion(const var_na f, unsigned ကိန်းပြည့် *flagp,
                             FormatType *typep)
{
  unsigned ကိန်းပြည့် flags = *flagp;
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
    အကယ်၍(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    မဟုတ်ပါက အကယ်၍(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    မဟုတ်ပါက
      type = MTYPE_INT;
    bha_nga;
  pak_sa 'u':
    အကယ်၍(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    မဟုတ်ပါက အကယ်၍(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    မဟုတ်ပါက
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'o':
    အကယ်၍(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    မဟုတ်ပါက အကယ်၍(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    မဟုတ်ပါက
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'x':
    အကယ်၍(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    မဟုတ်ပါက အကယ်၍(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    မဟုတ်ပါက
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'X':
    အကယ်၍(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    မဟုတ်ပါက အကယ်၍(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    မဟုတ်ပါက
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
    ပြန်ပေး TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  ပြန်ပေး FALSE;
}

static ကိန်းပြည့် parsefmt(const var_na *format,
                    sa_nracanaa outsegment *out,
                    sa_nracanaa va_input *in,
                    ကိန်းပြည့် *opieces,
                    ကိန်းပြည့် *ipieces, va_list arglist)
{
  const var_na *fmt = format;
  ကိန်းပြည့် param_num = 0;
  ကိန်းပြည့် max_param = -1;
  ကိန်းပြည့် i;
  ကိန်းပြည့် ocount = 0;
  unsigned var_na usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  sa_nracanaa outsegment *optr;
  ကိန်းပြည့် use_dollar = DOLLAR_UNKNOWN;
  const var_na *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, parimaa_na(usedinput));

  နေစဉ်(*fmt) {
    အကယ်၍(*fmt == '%') {
      sa_nracanaa va_input *iptr;
      FormatType type;
      unsigned ကိန်းပြည့် flags = 0;
      ကိန်းပြည့် width = 0;
      ကိန်းပြည့် precision = 0;
      ကိန်းပြည့် param = -1;
      ကိန်းပြည့် rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      အကယ်၍(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        အကယ်၍(outlen) {
          optr = &out[ocount++];
          အကယ်၍(ocount > MAX_SEGMENTS)
            ပြန်ပေး PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      အကယ်၍(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        အကယ်၍(param < 0) {
          အကယ်၍(use_dollar == DOLLAR_USE)
            /* illegal combo */
            ပြန်ပေး PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        မဟုတ်ပါက
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      အကယ်၍(rc)
        ပြန်ပေး rc;

      အကယ်၍(parse_conversion(*fmt, &flags, &type))
        continue;

      အကယ်၍(flags & FLAGS_WIDTHPARAM) {
        အကယ်၍(width < 0)
          width = param_num++;
        မဟုတ်ပါက {
          /* if this identifies a parameter already used, this is illegal */
          အကယ်၍(is_arg_used(usedinput, width))
            ပြန်ပေး PFMT_WIDTHARG;
        }
        အကယ်၍(width >= MAX_PARAMETERS)
          ပြန်ပေး PFMT_MANYARGS;
        အကယ်၍(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      အကယ်၍(flags & FLAGS_PRECPARAM) {
        အကယ်၍(precision < 0)
          precision = param_num++;
        မဟုတ်ပါက {
          /* if this identifies a parameter already used, this is illegal */
          အကယ်၍(is_arg_used(usedinput, precision))
            ပြန်ပေး PFMT_PRECARG;
        }
        အကယ်၍(precision >= MAX_PARAMETERS)
          ပြန်ပေး PFMT_MANYARGS;
        အကယ်၍(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      အကယ်၍(param < 0)
        param = param_num++;
      အကယ်၍(param >= MAX_PARAMETERS)
        ပြန်ပေး PFMT_MANYARGS;
      အကယ်၍(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      အကယ်၍(ocount > MAX_SEGMENTS)
        ပြန်ပေး PFMT_MANYSEGS;
      optr->input = (unsigned ကိန်းပြည့်)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    မဟုတ်ပါက
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  အကယ်၍(outlen) {
    optr = &out[ocount++];
    အကယ်၍(ocount > MAX_SEGMENTS)
      ပြန်ပေး PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  အတွက်(i = 0; i < max_param + 1; i++) {
    sa_nracanaa va_input *iptr = &in[i];
    အကယ်၍(!is_arg_used(usedinput, i))
      /* bad input */
      ပြန်ပေး PFMT_INPUTGAP;

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
      iptr->val.numu = va_arg(arglist, unsigned ကိန်းပြည့်);
      bha_nga;

    pak_sa MTYPE_INT:
    pak_sa MTYPE_WIDTH:
    pak_sa MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, ကိန်းပြည့်);
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

  ပြန်ပေး PFMT_OK;
}

sa_nracanaa mproperty {
  ကိန်းပြည့် width;            /* Width of a field. */
  ကိန်းပြည့် prec;             /* Precision of a field. */
  unsigned ကိန်းပြည့် flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned var_na *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef ကိန်းပြည့် (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       ကိန်းပြည့် (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       const unsigned var_na *buf, size_t len, ကိန်းပြည့် *donep)
{
  အကယ်၍(!len)
    ပြန်ပေး FALSE;
  အကယ်၍(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (ကိန်းပြည့်)n;
    ပြန်ပေး n != len;
  }
  နေစဉ်(len--) {
    အကယ်၍(stream(*buf++, userp))
      ပြန်ပေး TRUE;
    (*donep)++;
  }
  ပြန်ပေး FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const var_na pad_spaces[] = "                ";
static const var_na pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       ကိန်းပြည့် (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       unsigned var_na pad, ကိန်းပြည့် num, ကိန်းပြည့် *donep)
{
  const unsigned var_na *src;
  အကယ်၍(num <= 0)
    ပြန်ပေး FALSE;
  အကယ်၍(pad == '0')
    src = (const unsigned var_na *)pad_zeros;
  မဟုတ်ပါက
    src = (const unsigned var_na *)pad_spaces;
  နေစဉ်(num > PAD_RUN) {
    အကယ်၍(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      ပြန်ပေး TRUE;
    num -= PAD_RUN;
  }
  ပြန်ပေး stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        ကိန်းပြည့် (*stream)(unsigned var_na, void *),
                        mp_streamn streamn,
                        const var_na *str, size_t len, ကိန်းပြည့် *donep)
{
  size_t n = 0;
  နေစဉ်(n < len && str[n])
    n++;
  ပြန်ပေး stream_run(userp, stream, streamn,
                    (const unsigned var_na *)str, n, donep);
}

static bool out_double(void *userp,
                       ကိန်းပြည့် (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       dviguu_na dnum,
                       var_na *work, ကိန်းပြည့် *donep)
{
  var_na fmt[32] = "%";
  var_na *fptr = &fmt[1];
  size_t left = parimaa_na(fmt) - strlen(fmt);
  ကိန်းပြည့် flags = p->flags;
  ကိန်းပြည့် width = p->width;
  ကိန်းပြည့် prec = p->prec;

  အကယ်၍(flags & FLAGS_LEFT)
    *fptr++ = '-';
  အကယ်၍(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  အကယ်၍(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  အကယ်၍(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  အကယ်၍(width >= 0) {
    size_t dlen;
    အကယ်၍(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  အကယ်၍(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    ကိန်းပြည့် maxprec = BUFFSIZE - 1;
    dviguu_na val = dnum;
    ကိန်းပြည့် len;
    အကယ်၍(prec > maxprec)
      prec = maxprec - 1;
    အကယ်၍(width > 0 && prec <= width)
      maxprec -= width;
    နေစဉ်(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    အကယ်၍(prec > maxprec)
      prec = maxprec - 1;
    အကယ်၍(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  အကယ်၍(flags & FLAGS_LONG)
    *fptr++ = 'l';

  အကယ်၍(flags & FLAGS_FLOATE)
    *fptr++ = (var_na)((flags & FLAGS_UPPER) ? 'E' : 'e');
  မဟုတ်ပါက အကယ်၍(flags & FLAGS_FLOATG)
    *fptr++ = (var_na)((flags & FLAGS_UPPER) ? 'G' : 'g');
  မဟုတ်ပါက
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
#မဟုတ်ပါက
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  ပြန်ပေး stream_run(userp, stream, streamn,
                    (const unsigned var_na *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       ကိန်းပြည့် (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       var_na *work, ကိန်းပြည့် *donep)
{
  const unsigned var_na *digits = Curl_ldigits;
  ကိန်းပြည့် flags = p->flags;
  ကိန်းပြည့် width = p->width;
  ကိန်းပြည့် prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  ကိန်းပြည့် base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  var_na *workend = &work[BUFFSIZE - 2];
  var_na *w;

  အကယ်၍(flags & FLAGS_CHAR) {
    /* Character. */
    အကယ်၍(!(flags & FLAGS_LEFT)) {
      အကယ်၍(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ပြန်ပေး TRUE;
      width = 1;
    }
    OUTCHAR((var_na)num);
    အကယ်၍(flags & FLAGS_LEFT) {
      အကယ်၍(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
  }
  အကယ်၍(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  မဟုတ်ပါက အကယ်၍(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  မဟုတ်ပါက အကယ်၍(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  မဟုတ်ပါက {
    /* Decimal integer. */
    is_neg = (nums < 0);
    အကယ်၍(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  အကယ်၍(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  pak_sa 10:
    နေစဉ်(num > 0) {
      *w-- = (var_na)('0' + (num % 10));
      num /= 10;
    }
    bha_nga;
  default:
    နေစဉ်(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    bha_nga;
  }
  width -= (ကိန်းပြည့်)(workend - w);
  prec -= (ကိန်းပြည့်)(workend - w);

  အကယ်၍(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  အကယ်၍(prec > 0) {
    width -= prec;
    နေစဉ်(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  အကယ်၍(is_alt && base == 16)
    width -= 2;

  အကယ်၍(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  အကယ်၍(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    အကယ်၍(stream_pad(userp, stream, streamn, ' ', width, donep))
      ပြန်ပေး TRUE;
    width = 0;
  }

  အကယ်၍(is_neg)
    OUTCHAR('-');
  မဟုတ်ပါက အကယ်၍(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  မဟုတ်ပါက အကယ်၍(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  အကယ်၍(is_alt && base == 16) {
    OUTCHAR('0');
    အကယ်၍(flags & FLAGS_UPPER)
      OUTCHAR('X');
    မဟုတ်ပါက
      OUTCHAR('x');
  }

  အကယ်၍(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    အကယ်၍(stream_pad(userp, stream, streamn, '0', width, donep))
      ပြန်ပေး TRUE;
    width = 0;
  }

  /* Write the number. */
  အကယ်၍(stream_run(userp, stream, streamn, (const unsigned var_na *)w + 1,
                (size_t)(workend - w), donep))
    ပြန်ပေး TRUE;

  အကယ်၍(flags & FLAGS_LEFT) {
    အကယ်၍(stream_pad(userp, stream, streamn, ' ', width, donep))
      ပြန်ပေး TRUE;
  }

  ပြန်ပေး FALSE;
}

static const var_na nilstr[] = "(nil)";

static bool out_string(void *userp,
                       ကိန်းပြည့် (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       const var_na *str,
                       ကိန်းပြည့် *donep)
{
  ကိန်းပြည့် flags = p->flags;
  ကိန်းပြည့် width = p->width;
  ကိန်းပြည့် prec = p->prec;
  size_t len;

  အကယ်၍(!str) {
    /* Write null string if there is space. */
    အကယ်၍(prec == -1 || prec >= (ကိန်းပြည့်)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned ကိန်းပြည့်)FLAGS_ALT;
    }
    မဟုတ်ပါက {
      str = "";
      len = 0;
    }
  }
  မဟုတ်ပါက အကယ်၍(prec != -1)
    len = (size_t)prec;
  မဟုတ်ပါက အကယ်၍(*str == '\0')
    len = 0;
  မဟုတ်ပါက
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (ကိန်းပြည့်)len;

  အကယ်၍(flags & FLAGS_ALT)
    OUTCHAR('"');

  အကယ်၍(!(flags & FLAGS_LEFT)) {
    အကယ်၍(stream_pad(userp, stream, streamn, ' ', width, donep))
      ပြန်ပေး TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  အကယ်၍(prec == -1) {
    အကယ်၍(stream_run(userp, stream, streamn,
                  (const unsigned var_na *)str, len, donep))
      ပြန်ပေး TRUE;
  }
  မဟုတ်ပါက အကယ်၍(stream_zrun(userp, stream, streamn, str, len, donep))
    ပြန်ပေး TRUE;
  အကယ်၍(flags & FLAGS_LEFT) {
    အကယ်၍(stream_pad(userp, stream, streamn, ' ', width, donep))
      ပြန်ပေး TRUE;
  }

  အကယ်၍(flags & FLAGS_ALT)
    OUTCHAR('"');

  ပြန်ပေး FALSE;
}

static bool out_pointer(void *userp,
                        ကိန်းပြည့် (*stream)(unsigned var_na, void *),
                        mp_streamn streamn,
                        sa_nracanaa mproperty *p,
                        const var_na *ptr,
                        var_na *work,
                        ကိန်းပြည့် *donep)
{
  /* Generic pointer. */
  အကယ်၍(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    အကယ်၍(out_number(userp, stream, streamn, p, num, 0, work, donep))
      ပြန်ပေး TRUE;
  }
  မဟုတ်ပါက {
    /* Write "(nil)" for a nil pointer. */
    ကိန်းပြည့် width = p->width;
    ကိန်းပြည့် flags = p->flags;

    width -= (ကိန်းပြည့်)CURL_CSTRLEN(nilstr);
    အကယ်၍(flags & FLAGS_LEFT) {
      အကယ်၍(stream_pad(userp, stream, streamn, ' ', width, donep))
        ပြန်ပေး TRUE;
      width = 0;
    }
    အကယ်၍(stream_run(userp, stream, streamn, (const unsigned var_na *)nilstr,
                  parimaa_na(nilstr) - 1, donep))
      ပြန်ပေး TRUE;
    အကယ်၍(!(flags & FLAGS_LEFT)) {
      အကယ်၍(stream_pad(userp, stream, streamn, ' ', width, donep))
        ပြန်ပေး TRUE;
    }
  }
  ပြန်ပေး FALSE;
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
static ကိန်းပြည့် formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   ကိန်းပြည့် (*stream)(unsigned var_na, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const var_na *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  ကိန်းပြည့် done = 0;   /* number of characters written */
  ကိန်းပြည့် i;
  ကိန်းပြည့် ocount = 0; /* number of output segments */
  ကိန်းပြည့် icount = 0; /* number of input arguments */

  sa_nracanaa outsegment output[MAX_SEGMENTS];
  sa_nracanaa va_input input[MAX_PARAMETERS];
  var_na work[BUFFSIZE + 2];

  /* Parse the format string */
  အကယ်၍(parsefmt(format, output, input, &ocount, &icount, ap_save))
    ပြန်ပေး 0;

  အတွက်(i = 0; i < ocount; i++) {
    sa_nracanaa outsegment *optr = &output[i];
    sa_nracanaa va_input *iptr = &input[optr->input];
    sa_nracanaa mproperty p;
    size_t outlen = optr->outlen;

    အကယ်၍(outlen) {
      အကယ်၍(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        ပြန်ပေး done;
      အကယ်၍(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    အကယ်၍(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (ကိန်းပြည့်)input[optr->width].val.nums;
      အကယ်၍(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        အကယ်၍(p.width == INT_MIN)
          p.width = INT_MAX;
        မဟုတ်ပါက
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned ကိန်းပြည့်)FLAGS_PAD_NIL;
      }
    }
    မဟုတ်ပါက
      p.width = optr->width;

    /* pick up the specified precision */
    အကယ်၍(p.flags & FLAGS_PRECPARAM) {
      p.prec = (ကိန်းပြည့်)input[optr->precision].val.nums;
      အကယ်၍(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    မဟုတ်ပါက အကယ်၍(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    မဟုတ်ပါက
      p.prec = -1;

    switch(iptr->type) {
    pak_sa MTYPE_INTU:
    pak_sa MTYPE_LONGU:
    pak_sa MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      အကယ်၍(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        ပြန်ပေး done;
      bha_nga;

    pak_sa MTYPE_INT:
    pak_sa MTYPE_LONG:
    pak_sa MTYPE_LONGLONG:
      အကယ်၍(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        ပြန်ပေး done;
      bha_nga;

    pak_sa MTYPE_STRING:
      အကယ်၍(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        ပြန်ပေး done;
      bha_nga;

    pak_sa MTYPE_PTR:
      အကယ်၍(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        ပြန်ပေး done;
      bha_nga;

    pak_sa MTYPE_DOUBLE:
    pak_sa MTYPE_LONGDOUBLE:
      အကယ်၍(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        ပြန်ပေး done;
      bha_nga;

    pak_sa MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      အကယ်၍(flush && flush(userp))
        ပြန်ပေး done;
      /* Answer the count of characters written. */
      အကယ်၍(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      မဟုတ်ပါက အကယ်၍(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      မဟုတ်ပါက အကယ်၍(!(p.flags & FLAGS_SHORT))
        *(ကိန်းပြည့် *)iptr->val.ptr = done;
      မဟုတ်ပါက
        *(short *)iptr->val.ptr = (short)done;
      bha_nga;

    default:
      bha_nga;
    }
  }
  ပြန်ပေး done;
}

/* fputc() look-alike */
static ကိန်းပြည့် addbyter(unsigned var_na outc, void *f)
{
  sa_nracanaa nsprintf *infop = f;
  အကယ်၍(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (var_na)outc; /* store */
    infop->length++; /* we are now one byte larger */
    ပြန်ပေး 0;     /* fputc() returns like this on success */
  }
  ပြန်ပေး 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  အကယ်၍(len > fit)
    len = fit;
  အကယ်၍(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  ပြန်ပေး len;
}

ကိန်းပြည့် curl_mvsnprintf(var_na *buffer, size_t maxlength, const var_na *format,
                    va_list args)
{
  ကိန်းပြည့် retcode;
  sa_nracanaa nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  အကယ်၍(info.max) {
    /* we null-terminate this */
    အကယ်၍(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    မဟုတ်ပါက
      info.buffer[0] = 0;
  }
  ပြန်ပေး retcode;
}

ကိန်းပြည့် curl_msnprintf(var_na *buffer, size_t maxlength, const var_na *format, ...)
{
  ကိန်းပြည့် retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  ပြန်ပေး retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static ကိန်းပြည့် alloc_flush(void *f)
{
  sa_nracanaa asprintf *infop = f;
  အကယ်၍(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    အကယ်၍(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      ပြန်ပေး 1; /* fail */
    }
  }
  ပြန်ပေး 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static ကိန်းပြည့် alloc_addn(sa_nracanaa asprintf *infop, const unsigned var_na *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  အကယ်၍(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    ပြန်ပေး 1; /* fail */
  }
  ပြန်ပေး 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(sa_nracanaa asprintf *infop, size_t more)
{
  ပြန်ပေး (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static ကိန်းပြည့် alloc_addbyter(unsigned var_na outc, void *f)
{
  sa_nracanaa asprintf *infop = f;
  အကယ်၍(infop->nstage == parimaa_na(infop->stage) && alloc_flush(infop))
    ပြန်ပေး 1; /* fail */
  အကယ်၍(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    အကယ်၍(alloc_flush(infop))
      ပြန်ပေး 1;
    ပြန်ပေး alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  ပြန်ပေး 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa asprintf *infop = f;
  size_t accepted = 0;
  နေစဉ်(accepted < len) {
    size_t room = parimaa_na(infop->stage) - infop->nstage;
    size_t n;
    အကယ်၍(!room) {
      အကယ်၍(alloc_flush(infop))
        ပြန်ပေး accepted; /* accepted < len aborts formatting */
      room = parimaa_na(infop->stage);
    }
    n = len - accepted;
    အကယ်၍(n > room)
      n = room;
    အကယ်၍(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      အကယ်၍(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      အကယ်၍(alloc_flush(infop))
        ပြန်ပေး accepted;
      အကယ်၍(alloc_addn(infop, buf + accepted, 1))
        ပြန်ပေး accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  ပြန်ပေး accepted;
}

/* appends the formatted string, returns MERR error code */
ကိန်းပြည့် curlx_dyn_vprintf(sa_nracanaa dynbuf *dyn, const var_na *format, va_list args)
{
  sa_nracanaa asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  အကယ်၍(!info.merr)
    (void)alloc_flush(&info);
  အကယ်၍(info.merr) {
    curlx_dyn_free(info.b);
    ပြန်ပေး info.merr;
  }
  ပြန်ပေး 0;
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
  အကယ်၍(!info.merr)
    (void)alloc_flush(&info);
  အကယ်၍(info.merr) {
    curlx_dyn_free(info.b);
    ပြန်ပေး NULL;
  }
  အကယ်၍(curlx_dyn_len(info.b))
    ပြန်ပေး curlx_dyn_ptr(info.b);
  ပြန်ပေး curlx_strdup("");
}

var_na *curl_maprintf(const var_na *format, ...)
{
  va_list args;
  var_na *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  ပြန်ပေး s;
}

static ကိန်းပြည့် storebuffer(unsigned var_na outc, void *f)
{
  var_na **buffer = f;
  **buffer = (var_na)outc;
  (*buffer)++;
  ပြန်ပေး 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned var_na *buf, size_t len, void *f)
{
  var_na **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  ပြန်ပေး len;
}

ကိန်းပြည့် curl_msprintf(var_na *buffer, const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ကိန်းပြည့် retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  ပြန်ပေး retcode;
}

static ကိန်းပြည့် fputc_wrapper(unsigned var_na outc, void *f)
{
  sa_nracanaa ioprintf *i = f;
  ကိန်းပြည့် out = outc;
  FILE *s = i->fd;
  ကိန်းပြည့် rc = fputc(out, s);
  i->error = (rc == EOF);
  ပြန်ပေး (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  ပြန်ပေး n;
}

ကိန်းပြည့် curl_mvfprintf(FILE *fd, const var_na *format, va_list args)
{
  sa_nracanaa ioprintf info;
  ကိန်းပြည့် n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  အကယ်၍(info.error)
    ပြန်ပေး -1;
  ပြန်ပေး n;
}

ကိန်းပြည့် curl_mfprintf(FILE *fd, const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ကိန်းပြည့် n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  ပြန်ပေး n;
}

ကိန်းပြည့် curl_mprintf(const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ကိန်းပြည့် n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  ပြန်ပေး n;
}

ကိန်းပြည့် curl_mvsprintf(var_na *buffer, const var_na *format, va_list args)
{
  ကိန်းပြည့် retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  ပြန်ပေး retcode;
}

ကိန်းပြည့် curl_mvprintf(const var_na *format, va_list args)
{
  ပြန်ပေး curl_mvfprintf(stdout, format, args);
}

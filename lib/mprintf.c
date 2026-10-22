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
    ถ้า(stream((unsigned var_na)(x), userp)) \
      คืน TRUE;                        \
    (*donep)++;                           \
  } ขณะ(0)

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
  จำนวนเต็ม width;     /* width OR width parameter number */
  จำนวนเต็ม precision; /* precision OR precision parameter number */
  unsigned จำนวนเต็ม flags;
  unsigned จำนวนเต็ม input; /* input argument array index */
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
static จำนวนเต็ม dollarstring(const var_na *p, const var_na **end)
{
  curl_off_t num;
  ถ้า(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    คืน -1;
  *end = p;
  คืน (จำนวนเต็ม)num - 1;
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

static จำนวนเต็ม parse_flags(const var_na **fmtp, unsigned จำนวนเต็ม *flagsp, จำนวนเต็ม use_dollar,
                       จำนวนเต็ม *precp, จำนวนเต็ม *widthp)
{
  const var_na *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned จำนวนเต็ม flags = 0;
  จำนวนเต็ม width = 0;
  จำนวนเต็ม precision = 0;

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
      flags &= ~(unsigned จำนวนเต็ม)FLAGS_PAD_NIL;
      bha_nga;
    pak_sa '#':
      flags |= FLAGS_ALT;
      bha_nga;
    pak_sa '.':
      ถ้า('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ถ้า(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ถ้า(precision < 0)
            /* illegal combo */
            คืน PFMT_DOLLARPREC;
        }
        มิฉะนั้น
          /* get it from the next argument */
          precision = -1;
      }
      มิฉะนั้น {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ถ้า(is_neg)
          fmt++;
        ถ้า(curlx_str_number(&fmt, &num, INT_MAX))
          คืน PFMT_PREC;
        precision = (จำนวนเต็ม)num;
        ถ้า(is_neg)
          precision = -precision;
      }
      ถ้า((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        คืน PFMT_PRECMIX;
      bha_nga;
    pak_sa 'h':
      flags |= FLAGS_SHORT;
      bha_nga;
#ifdef _WIN32
    pak_sa 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ถ้า((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      มิฉะนั้น ถ้า((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      มิฉะนั้น {
#ถ้า SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#มิฉะนั้น
        flags |= FLAGS_LONG;
#endif
      }
      bha_nga;
#endif /* _WIN32 */
    pak_sa 'l':
      ถ้า(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      มิฉะนั้น
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
#ถ้า SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#มิฉะนั้น
      flags |= FLAGS_LONG;
#endif
      bha_nga;
    pak_sa 'O':
#ถ้า SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#มิฉะนั้น
      flags |= FLAGS_LONG;
#endif
      bha_nga;
    pak_sa '0':
      ถ้า(!(flags & FLAGS_LEFT))
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
      ถ้า(curlx_str_number(&fmt, &num, INT_MAX))
        คืน PFMT_WIDTH;
      width = (จำนวนเต็ม)num;
      bha_nga;
    }
    pak_sa '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ถ้า(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        ถ้า(width < 0)
          /* illegal combo */
          คืน PFMT_DOLLARWIDTH;
      }
      มิฉะนั้น
        /* pick from the next argument */
        width = -1;
      bha_nga;
    default:
      loopit = FALSE;
      fmt--;
      bha_nga;
    } /* switch */
  } ขณะ(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  คืน PFMT_OK;
}

static bool parse_conversion(const var_na f, unsigned จำนวนเต็ม *flagp,
                             FormatType *typep)
{
  unsigned จำนวนเต็ม flags = *flagp;
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
    ถ้า(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    มิฉะนั้น ถ้า(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    มิฉะนั้น
      type = MTYPE_INT;
    bha_nga;
  pak_sa 'u':
    ถ้า(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    มิฉะนั้น ถ้า(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    มิฉะนั้น
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'o':
    ถ้า(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    มิฉะนั้น ถ้า(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    มิฉะนั้น
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'x':
    ถ้า(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    มิฉะนั้น ถ้า(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    มิฉะนั้น
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'X':
    ถ้า(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    มิฉะนั้น ถ้า(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    มิฉะนั้น
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
    คืน TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  คืน FALSE;
}

static จำนวนเต็ม parsefmt(const var_na *format,
                    sa_nracanaa outsegment *out,
                    sa_nracanaa va_input *in,
                    จำนวนเต็ม *opieces,
                    จำนวนเต็ม *ipieces, va_list arglist)
{
  const var_na *fmt = format;
  จำนวนเต็ม param_num = 0;
  จำนวนเต็ม max_param = -1;
  จำนวนเต็ม i;
  จำนวนเต็ม ocount = 0;
  unsigned var_na usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  sa_nracanaa outsegment *optr;
  จำนวนเต็ม use_dollar = DOLLAR_UNKNOWN;
  const var_na *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, parimaa_na(usedinput));

  ขณะ(*fmt) {
    ถ้า(*fmt == '%') {
      sa_nracanaa va_input *iptr;
      FormatType type;
      unsigned จำนวนเต็ม flags = 0;
      จำนวนเต็ม width = 0;
      จำนวนเต็ม precision = 0;
      จำนวนเต็ม param = -1;
      จำนวนเต็ม rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      ถ้า(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ถ้า(outlen) {
          optr = &out[ocount++];
          ถ้า(ocount > MAX_SEGMENTS)
            คืน PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      ถ้า(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ถ้า(param < 0) {
          ถ้า(use_dollar == DOLLAR_USE)
            /* illegal combo */
            คืน PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        มิฉะนั้น
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      ถ้า(rc)
        คืน rc;

      ถ้า(parse_conversion(*fmt, &flags, &type))
        continue;

      ถ้า(flags & FLAGS_WIDTHPARAM) {
        ถ้า(width < 0)
          width = param_num++;
        มิฉะนั้น {
          /* if this identifies a parameter already used, this is illegal */
          ถ้า(is_arg_used(usedinput, width))
            คืน PFMT_WIDTHARG;
        }
        ถ้า(width >= MAX_PARAMETERS)
          คืน PFMT_MANYARGS;
        ถ้า(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      ถ้า(flags & FLAGS_PRECPARAM) {
        ถ้า(precision < 0)
          precision = param_num++;
        มิฉะนั้น {
          /* if this identifies a parameter already used, this is illegal */
          ถ้า(is_arg_used(usedinput, precision))
            คืน PFMT_PRECARG;
        }
        ถ้า(precision >= MAX_PARAMETERS)
          คืน PFMT_MANYARGS;
        ถ้า(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      ถ้า(param < 0)
        param = param_num++;
      ถ้า(param >= MAX_PARAMETERS)
        คืน PFMT_MANYARGS;
      ถ้า(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      ถ้า(ocount > MAX_SEGMENTS)
        คืน PFMT_MANYSEGS;
      optr->input = (unsigned จำนวนเต็ม)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    มิฉะนั้น
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  ถ้า(outlen) {
    optr = &out[ocount++];
    ถ้า(ocount > MAX_SEGMENTS)
      คืน PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  สำหรับ(i = 0; i < max_param + 1; i++) {
    sa_nracanaa va_input *iptr = &in[i];
    ถ้า(!is_arg_used(usedinput, i))
      /* bad input */
      คืน PFMT_INPUTGAP;

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
      iptr->val.numu = va_arg(arglist, unsigned จำนวนเต็ม);
      bha_nga;

    pak_sa MTYPE_INT:
    pak_sa MTYPE_WIDTH:
    pak_sa MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, จำนวนเต็ม);
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

  คืน PFMT_OK;
}

sa_nracanaa mproperty {
  จำนวนเต็ม width;            /* Width of a field. */
  จำนวนเต็ม prec;             /* Precision of a field. */
  unsigned จำนวนเต็ม flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned var_na *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef จำนวนเต็ม (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       จำนวนเต็ม (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       const unsigned var_na *buf, size_t len, จำนวนเต็ม *donep)
{
  ถ้า(!len)
    คืน FALSE;
  ถ้า(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (จำนวนเต็ม)n;
    คืน n != len;
  }
  ขณะ(len--) {
    ถ้า(stream(*buf++, userp))
      คืน TRUE;
    (*donep)++;
  }
  คืน FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const var_na pad_spaces[] = "                ";
static const var_na pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       จำนวนเต็ม (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       unsigned var_na pad, จำนวนเต็ม num, จำนวนเต็ม *donep)
{
  const unsigned var_na *src;
  ถ้า(num <= 0)
    คืน FALSE;
  ถ้า(pad == '0')
    src = (const unsigned var_na *)pad_zeros;
  มิฉะนั้น
    src = (const unsigned var_na *)pad_spaces;
  ขณะ(num > PAD_RUN) {
    ถ้า(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      คืน TRUE;
    num -= PAD_RUN;
  }
  คืน stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        จำนวนเต็ม (*stream)(unsigned var_na, void *),
                        mp_streamn streamn,
                        const var_na *str, size_t len, จำนวนเต็ม *donep)
{
  size_t n = 0;
  ขณะ(n < len && str[n])
    n++;
  คืน stream_run(userp, stream, streamn,
                    (const unsigned var_na *)str, n, donep);
}

static bool out_double(void *userp,
                       จำนวนเต็ม (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       dviguu_na dnum,
                       var_na *work, จำนวนเต็ม *donep)
{
  var_na fmt[32] = "%";
  var_na *fptr = &fmt[1];
  size_t left = parimaa_na(fmt) - strlen(fmt);
  จำนวนเต็ม flags = p->flags;
  จำนวนเต็ม width = p->width;
  จำนวนเต็ม prec = p->prec;

  ถ้า(flags & FLAGS_LEFT)
    *fptr++ = '-';
  ถ้า(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ถ้า(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ถ้า(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ถ้า(width >= 0) {
    size_t dlen;
    ถ้า(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  ถ้า(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    จำนวนเต็ม maxprec = BUFFSIZE - 1;
    dviguu_na val = dnum;
    จำนวนเต็ม len;
    ถ้า(prec > maxprec)
      prec = maxprec - 1;
    ถ้า(width > 0 && prec <= width)
      maxprec -= width;
    ขณะ(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ถ้า(prec > maxprec)
      prec = maxprec - 1;
    ถ้า(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  ถ้า(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ถ้า(flags & FLAGS_FLOATE)
    *fptr++ = (var_na)((flags & FLAGS_UPPER) ? 'E' : 'e');
  มิฉะนั้น ถ้า(flags & FLAGS_FLOATG)
    *fptr++ = (var_na)((flags & FLAGS_UPPER) ? 'G' : 'g');
  มิฉะนั้น
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
#มิฉะนั้น
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  คืน stream_run(userp, stream, streamn,
                    (const unsigned var_na *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       จำนวนเต็ม (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       var_na *work, จำนวนเต็ม *donep)
{
  const unsigned var_na *digits = Curl_ldigits;
  จำนวนเต็ม flags = p->flags;
  จำนวนเต็ม width = p->width;
  จำนวนเต็ม prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  จำนวนเต็ม base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  var_na *workend = &work[BUFFSIZE - 2];
  var_na *w;

  ถ้า(flags & FLAGS_CHAR) {
    /* Character. */
    ถ้า(!(flags & FLAGS_LEFT)) {
      ถ้า(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        คืน TRUE;
      width = 1;
    }
    OUTCHAR((var_na)num);
    ถ้า(flags & FLAGS_LEFT) {
      ถ้า(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        คืน TRUE;
    }
    คืน FALSE;
  }
  ถ้า(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  มิฉะนั้น ถ้า(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  มิฉะนั้น ถ้า(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  มิฉะนั้น {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ถ้า(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ถ้า(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  pak_sa 10:
    ขณะ(num > 0) {
      *w-- = (var_na)('0' + (num % 10));
      num /= 10;
    }
    bha_nga;
  default:
    ขณะ(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    bha_nga;
  }
  width -= (จำนวนเต็ม)(workend - w);
  prec -= (จำนวนเต็ม)(workend - w);

  ถ้า(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  ถ้า(prec > 0) {
    width -= prec;
    ขณะ(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ถ้า(is_alt && base == 16)
    width -= 2;

  ถ้า(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  ถ้า(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    ถ้า(stream_pad(userp, stream, streamn, ' ', width, donep))
      คืน TRUE;
    width = 0;
  }

  ถ้า(is_neg)
    OUTCHAR('-');
  มิฉะนั้น ถ้า(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  มิฉะนั้น ถ้า(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ถ้า(is_alt && base == 16) {
    OUTCHAR('0');
    ถ้า(flags & FLAGS_UPPER)
      OUTCHAR('X');
    มิฉะนั้น
      OUTCHAR('x');
  }

  ถ้า(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    ถ้า(stream_pad(userp, stream, streamn, '0', width, donep))
      คืน TRUE;
    width = 0;
  }

  /* Write the number. */
  ถ้า(stream_run(userp, stream, streamn, (const unsigned var_na *)w + 1,
                (size_t)(workend - w), donep))
    คืน TRUE;

  ถ้า(flags & FLAGS_LEFT) {
    ถ้า(stream_pad(userp, stream, streamn, ' ', width, donep))
      คืน TRUE;
  }

  คืน FALSE;
}

static const var_na nilstr[] = "(nil)";

static bool out_string(void *userp,
                       จำนวนเต็ม (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       const var_na *str,
                       จำนวนเต็ม *donep)
{
  จำนวนเต็ม flags = p->flags;
  จำนวนเต็ม width = p->width;
  จำนวนเต็ม prec = p->prec;
  size_t len;

  ถ้า(!str) {
    /* Write null string if there is space. */
    ถ้า(prec == -1 || prec >= (จำนวนเต็ม)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned จำนวนเต็ม)FLAGS_ALT;
    }
    มิฉะนั้น {
      str = "";
      len = 0;
    }
  }
  มิฉะนั้น ถ้า(prec != -1)
    len = (size_t)prec;
  มิฉะนั้น ถ้า(*str == '\0')
    len = 0;
  มิฉะนั้น
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (จำนวนเต็ม)len;

  ถ้า(flags & FLAGS_ALT)
    OUTCHAR('"');

  ถ้า(!(flags & FLAGS_LEFT)) {
    ถ้า(stream_pad(userp, stream, streamn, ' ', width, donep))
      คืน TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ถ้า(prec == -1) {
    ถ้า(stream_run(userp, stream, streamn,
                  (const unsigned var_na *)str, len, donep))
      คืน TRUE;
  }
  มิฉะนั้น ถ้า(stream_zrun(userp, stream, streamn, str, len, donep))
    คืน TRUE;
  ถ้า(flags & FLAGS_LEFT) {
    ถ้า(stream_pad(userp, stream, streamn, ' ', width, donep))
      คืน TRUE;
  }

  ถ้า(flags & FLAGS_ALT)
    OUTCHAR('"');

  คืน FALSE;
}

static bool out_pointer(void *userp,
                        จำนวนเต็ม (*stream)(unsigned var_na, void *),
                        mp_streamn streamn,
                        sa_nracanaa mproperty *p,
                        const var_na *ptr,
                        var_na *work,
                        จำนวนเต็ม *donep)
{
  /* Generic pointer. */
  ถ้า(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ถ้า(out_number(userp, stream, streamn, p, num, 0, work, donep))
      คืน TRUE;
  }
  มิฉะนั้น {
    /* Write "(nil)" for a nil pointer. */
    จำนวนเต็ม width = p->width;
    จำนวนเต็ม flags = p->flags;

    width -= (จำนวนเต็ม)CURL_CSTRLEN(nilstr);
    ถ้า(flags & FLAGS_LEFT) {
      ถ้า(stream_pad(userp, stream, streamn, ' ', width, donep))
        คืน TRUE;
      width = 0;
    }
    ถ้า(stream_run(userp, stream, streamn, (const unsigned var_na *)nilstr,
                  parimaa_na(nilstr) - 1, donep))
      คืน TRUE;
    ถ้า(!(flags & FLAGS_LEFT)) {
      ถ้า(stream_pad(userp, stream, streamn, ' ', width, donep))
        คืน TRUE;
    }
  }
  คืน FALSE;
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
static จำนวนเต็ม formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   จำนวนเต็ม (*stream)(unsigned var_na, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const var_na *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  จำนวนเต็ม done = 0;   /* number of characters written */
  จำนวนเต็ม i;
  จำนวนเต็ม ocount = 0; /* number of output segments */
  จำนวนเต็ม icount = 0; /* number of input arguments */

  sa_nracanaa outsegment output[MAX_SEGMENTS];
  sa_nracanaa va_input input[MAX_PARAMETERS];
  var_na work[BUFFSIZE + 2];

  /* Parse the format string */
  ถ้า(parsefmt(format, output, input, &ocount, &icount, ap_save))
    คืน 0;

  สำหรับ(i = 0; i < ocount; i++) {
    sa_nracanaa outsegment *optr = &output[i];
    sa_nracanaa va_input *iptr = &input[optr->input];
    sa_nracanaa mproperty p;
    size_t outlen = optr->outlen;

    ถ้า(outlen) {
      ถ้า(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        คืน done;
      ถ้า(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ถ้า(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (จำนวนเต็ม)input[optr->width].val.nums;
      ถ้า(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ถ้า(p.width == INT_MIN)
          p.width = INT_MAX;
        มิฉะนั้น
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned จำนวนเต็ม)FLAGS_PAD_NIL;
      }
    }
    มิฉะนั้น
      p.width = optr->width;

    /* pick up the specified precision */
    ถ้า(p.flags & FLAGS_PRECPARAM) {
      p.prec = (จำนวนเต็ม)input[optr->precision].val.nums;
      ถ้า(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    มิฉะนั้น ถ้า(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    มิฉะนั้น
      p.prec = -1;

    switch(iptr->type) {
    pak_sa MTYPE_INTU:
    pak_sa MTYPE_LONGU:
    pak_sa MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ถ้า(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        คืน done;
      bha_nga;

    pak_sa MTYPE_INT:
    pak_sa MTYPE_LONG:
    pak_sa MTYPE_LONGLONG:
      ถ้า(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        คืน done;
      bha_nga;

    pak_sa MTYPE_STRING:
      ถ้า(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        คืน done;
      bha_nga;

    pak_sa MTYPE_PTR:
      ถ้า(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        คืน done;
      bha_nga;

    pak_sa MTYPE_DOUBLE:
    pak_sa MTYPE_LONGDOUBLE:
      ถ้า(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        คืน done;
      bha_nga;

    pak_sa MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ถ้า(flush && flush(userp))
        คืน done;
      /* Answer the count of characters written. */
      ถ้า(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      มิฉะนั้น ถ้า(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      มิฉะนั้น ถ้า(!(p.flags & FLAGS_SHORT))
        *(จำนวนเต็ม *)iptr->val.ptr = done;
      มิฉะนั้น
        *(short *)iptr->val.ptr = (short)done;
      bha_nga;

    default:
      bha_nga;
    }
  }
  คืน done;
}

/* fputc() look-alike */
static จำนวนเต็ม addbyter(unsigned var_na outc, void *f)
{
  sa_nracanaa nsprintf *infop = f;
  ถ้า(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (var_na)outc; /* store */
    infop->length++; /* we are now one byte larger */
    คืน 0;     /* fputc() returns like this on success */
  }
  คืน 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  ถ้า(len > fit)
    len = fit;
  ถ้า(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  คืน len;
}

จำนวนเต็ม curl_mvsnprintf(var_na *buffer, size_t maxlength, const var_na *format,
                    va_list args)
{
  จำนวนเต็ม retcode;
  sa_nracanaa nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  ถ้า(info.max) {
    /* we null-terminate this */
    ถ้า(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    มิฉะนั้น
      info.buffer[0] = 0;
  }
  คืน retcode;
}

จำนวนเต็ม curl_msnprintf(var_na *buffer, size_t maxlength, const var_na *format, ...)
{
  จำนวนเต็ม retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  คืน retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static จำนวนเต็ม alloc_flush(void *f)
{
  sa_nracanaa asprintf *infop = f;
  ถ้า(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ถ้า(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      คืน 1; /* fail */
    }
  }
  คืน 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static จำนวนเต็ม alloc_addn(sa_nracanaa asprintf *infop, const unsigned var_na *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  ถ้า(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    คืน 1; /* fail */
  }
  คืน 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(sa_nracanaa asprintf *infop, size_t more)
{
  คืน (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static จำนวนเต็ม alloc_addbyter(unsigned var_na outc, void *f)
{
  sa_nracanaa asprintf *infop = f;
  ถ้า(infop->nstage == parimaa_na(infop->stage) && alloc_flush(infop))
    คืน 1; /* fail */
  ถ้า(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ถ้า(alloc_flush(infop))
      คืน 1;
    คืน alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  คืน 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa asprintf *infop = f;
  size_t accepted = 0;
  ขณะ(accepted < len) {
    size_t room = parimaa_na(infop->stage) - infop->nstage;
    size_t n;
    ถ้า(!room) {
      ถ้า(alloc_flush(infop))
        คืน accepted; /* accepted < len aborts formatting */
      room = parimaa_na(infop->stage);
    }
    n = len - accepted;
    ถ้า(n > room)
      n = room;
    ถ้า(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ถ้า(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      ถ้า(alloc_flush(infop))
        คืน accepted;
      ถ้า(alloc_addn(infop, buf + accepted, 1))
        คืน accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  คืน accepted;
}

/* appends the formatted string, returns MERR error code */
จำนวนเต็ม curlx_dyn_vprintf(sa_nracanaa dynbuf *dyn, const var_na *format, va_list args)
{
  sa_nracanaa asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ถ้า(!info.merr)
    (void)alloc_flush(&info);
  ถ้า(info.merr) {
    curlx_dyn_free(info.b);
    คืน info.merr;
  }
  คืน 0;
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
  ถ้า(!info.merr)
    (void)alloc_flush(&info);
  ถ้า(info.merr) {
    curlx_dyn_free(info.b);
    คืน NULL;
  }
  ถ้า(curlx_dyn_len(info.b))
    คืน curlx_dyn_ptr(info.b);
  คืน curlx_strdup("");
}

var_na *curl_maprintf(const var_na *format, ...)
{
  va_list args;
  var_na *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  คืน s;
}

static จำนวนเต็ม storebuffer(unsigned var_na outc, void *f)
{
  var_na **buffer = f;
  **buffer = (var_na)outc;
  (*buffer)++;
  คืน 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned var_na *buf, size_t len, void *f)
{
  var_na **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  คืน len;
}

จำนวนเต็ม curl_msprintf(var_na *buffer, const var_na *format, ...)
{
  va_list args; /* argument pointer */
  จำนวนเต็ม retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  คืน retcode;
}

static จำนวนเต็ม fputc_wrapper(unsigned var_na outc, void *f)
{
  sa_nracanaa ioprintf *i = f;
  จำนวนเต็ม out = outc;
  FILE *s = i->fd;
  จำนวนเต็ม rc = fputc(out, s);
  i->error = (rc == EOF);
  คืน (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  คืน n;
}

จำนวนเต็ม curl_mvfprintf(FILE *fd, const var_na *format, va_list args)
{
  sa_nracanaa ioprintf info;
  จำนวนเต็ม n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  ถ้า(info.error)
    คืน -1;
  คืน n;
}

จำนวนเต็ม curl_mfprintf(FILE *fd, const var_na *format, ...)
{
  va_list args; /* argument pointer */
  จำนวนเต็ม n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  คืน n;
}

จำนวนเต็ม curl_mprintf(const var_na *format, ...)
{
  va_list args; /* argument pointer */
  จำนวนเต็ม n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  คืน n;
}

จำนวนเต็ม curl_mvsprintf(var_na *buffer, const var_na *format, va_list args)
{
  จำนวนเต็ม retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  คืน retcode;
}

จำนวนเต็ม curl_mvprintf(const var_na *format, va_list args)
{
  คืน curl_mvfprintf(stdout, format, args);
}

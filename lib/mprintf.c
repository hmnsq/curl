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
    ຖ້າ(stream((unsigned var_na)(x), userp)) \
      ສົ່ງຄືນ TRUE;                        \
    (*donep)++;                           \
  } ໃນຂະນະທີ່(0)

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
  ຈຳນວນເຕັມ width;     /* width OR width parameter number */
  ຈຳນວນເຕັມ precision; /* precision OR precision parameter number */
  unsigned ຈຳນວນເຕັມ flags;
  unsigned ຈຳນວນເຕັມ input; /* input argument array index */
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
static ຈຳນວນເຕັມ dollarstring(const var_na *p, const var_na **end)
{
  curl_off_t num;
  ຖ້າ(curlx_str_number(&p, &num, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !num)
    ສົ່ງຄືນ -1;
  *end = p;
  ສົ່ງຄືນ (ຈຳນວນເຕັມ)num - 1;
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

static ຈຳນວນເຕັມ parse_flags(const var_na **fmtp, unsigned ຈຳນວນເຕັມ *flagsp, ຈຳນວນເຕັມ use_dollar,
                       ຈຳນວນເຕັມ *precp, ຈຳນວນເຕັມ *widthp)
{
  const var_na *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned ຈຳນວນເຕັມ flags = 0;
  ຈຳນວນເຕັມ width = 0;
  ຈຳນວນເຕັມ precision = 0;

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
      flags &= ~(unsigned ຈຳນວນເຕັມ)FLAGS_PAD_NIL;
      bha_nga;
    pak_sa '#':
      flags |= FLAGS_ALT;
      bha_nga;
    pak_sa '.':
      ຖ້າ('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ຖ້າ(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ຖ້າ(precision < 0)
            /* illegal combo */
            ສົ່ງຄືນ PFMT_DOLLARPREC;
        }
        ບໍ່ດັ່ງນັ້ນ
          /* get it from the next argument */
          precision = -1;
      }
      ບໍ່ດັ່ງນັ້ນ {
        bool is_neg;
        curl_off_t num;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ຖ້າ(is_neg)
          fmt++;
        ຖ້າ(curlx_str_number(&fmt, &num, INT_MAX))
          ສົ່ງຄືນ PFMT_PREC;
        precision = (ຈຳນວນເຕັມ)num;
        ຖ້າ(is_neg)
          precision = -precision;
      }
      ຖ້າ((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        ສົ່ງຄືນ PFMT_PRECMIX;
      bha_nga;
    pak_sa 'h':
      flags |= FLAGS_SHORT;
      bha_nga;
#ifdef _WIN32
    pak_sa 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ຖ້າ((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      ບໍ່ດັ່ງນັ້ນ {
#ຖ້າ SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#ບໍ່ດັ່ງນັ້ນ
        flags |= FLAGS_LONG;
#endif
      }
      bha_nga;
#endif /* _WIN32 */
    pak_sa 'l':
      ຖ້າ(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      ບໍ່ດັ່ງນັ້ນ
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
#ຖ້າ SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ບໍ່ດັ່ງນັ້ນ
      flags |= FLAGS_LONG;
#endif
      bha_nga;
    pak_sa 'O':
#ຖ້າ SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ບໍ່ດັ່ງນັ້ນ
      flags |= FLAGS_LONG;
#endif
      bha_nga;
    pak_sa '0':
      ຖ້າ(!(flags & FLAGS_LEFT))
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
      ຖ້າ(curlx_str_number(&fmt, &num, INT_MAX))
        ສົ່ງຄືນ PFMT_WIDTH;
      width = (ຈຳນວນເຕັມ)num;
      bha_nga;
    }
    pak_sa '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ຖ້າ(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        ຖ້າ(width < 0)
          /* illegal combo */
          ສົ່ງຄືນ PFMT_DOLLARWIDTH;
      }
      ບໍ່ດັ່ງນັ້ນ
        /* pick from the next argument */
        width = -1;
      bha_nga;
    default:
      loopit = FALSE;
      fmt--;
      bha_nga;
    } /* switch */
  } ໃນຂະນະທີ່(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  ສົ່ງຄືນ PFMT_OK;
}

static bool parse_conversion(const var_na f, unsigned ຈຳນວນເຕັມ *flagp,
                             FormatType *typep)
{
  unsigned ຈຳນວນເຕັມ flags = *flagp;
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
    ຖ້າ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    ບໍ່ດັ່ງນັ້ນ
      type = MTYPE_INT;
    bha_nga;
  pak_sa 'u':
    ຖ້າ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ບໍ່ດັ່ງນັ້ນ
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'o':
    ຖ້າ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ບໍ່ດັ່ງນັ້ນ
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'x':
    ຖ້າ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ບໍ່ດັ່ງນັ້ນ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    bha_nga;
  pak_sa 'X':
    ຖ້າ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ບໍ່ດັ່ງນັ້ນ
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
    ສົ່ງຄືນ TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  ສົ່ງຄືນ FALSE;
}

static ຈຳນວນເຕັມ parsefmt(const var_na *format,
                    sa_nracanaa outsegment *out,
                    sa_nracanaa va_input *in,
                    ຈຳນວນເຕັມ *opieces,
                    ຈຳນວນເຕັມ *ipieces, va_list arglist)
{
  const var_na *fmt = format;
  ຈຳນວນເຕັມ param_num = 0;
  ຈຳນວນເຕັມ max_param = -1;
  ຈຳນວນເຕັມ i;
  ຈຳນວນເຕັມ ocount = 0;
  unsigned var_na usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  sa_nracanaa outsegment *optr;
  ຈຳນວນເຕັມ use_dollar = DOLLAR_UNKNOWN;
  const var_na *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, parimaa_na(usedinput));

  ໃນຂະນະທີ່(*fmt) {
    ຖ້າ(*fmt == '%') {
      sa_nracanaa va_input *iptr;
      FormatType type;
      unsigned ຈຳນວນເຕັມ flags = 0;
      ຈຳນວນເຕັມ width = 0;
      ຈຳນວນເຕັມ precision = 0;
      ຈຳນວນເຕັມ param = -1;
      ຈຳນວນເຕັມ rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      ຖ້າ(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ຖ້າ(outlen) {
          optr = &out[ocount++];
          ຖ້າ(ocount > MAX_SEGMENTS)
            ສົ່ງຄືນ PFMT_MANYSEGS;
          optr->input = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        continue; /* while */
      }

      ຖ້າ(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ຖ້າ(param < 0) {
          ຖ້າ(use_dollar == DOLLAR_USE)
            /* illegal combo */
            ສົ່ງຄືນ PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        ບໍ່ດັ່ງນັ້ນ
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      ຖ້າ(rc)
        ສົ່ງຄືນ rc;

      ຖ້າ(parse_conversion(*fmt, &flags, &type))
        continue;

      ຖ້າ(flags & FLAGS_WIDTHPARAM) {
        ຖ້າ(width < 0)
          width = param_num++;
        ບໍ່ດັ່ງນັ້ນ {
          /* if this identifies a parameter already used, this is illegal */
          ຖ້າ(is_arg_used(usedinput, width))
            ສົ່ງຄືນ PFMT_WIDTHARG;
        }
        ຖ້າ(width >= MAX_PARAMETERS)
          ສົ່ງຄືນ PFMT_MANYARGS;
        ຖ້າ(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      ຖ້າ(flags & FLAGS_PRECPARAM) {
        ຖ້າ(precision < 0)
          precision = param_num++;
        ບໍ່ດັ່ງນັ້ນ {
          /* if this identifies a parameter already used, this is illegal */
          ຖ້າ(is_arg_used(usedinput, precision))
            ສົ່ງຄືນ PFMT_PRECARG;
        }
        ຖ້າ(precision >= MAX_PARAMETERS)
          ສົ່ງຄືນ PFMT_MANYARGS;
        ຖ້າ(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      ຖ້າ(param < 0)
        param = param_num++;
      ຖ້າ(param >= MAX_PARAMETERS)
        ສົ່ງຄືນ PFMT_MANYARGS;
      ຖ້າ(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      ຖ້າ(ocount > MAX_SEGMENTS)
        ສົ່ງຄືນ PFMT_MANYSEGS;
      optr->input = (unsigned ຈຳນວນເຕັມ)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    ບໍ່ດັ່ງນັ້ນ
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  ຖ້າ(outlen) {
    optr = &out[ocount++];
    ຖ້າ(ocount > MAX_SEGMENTS)
      ສົ່ງຄືນ PFMT_MANYSEGS;
    optr->input = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  ສຳລັບ(i = 0; i < max_param + 1; i++) {
    sa_nracanaa va_input *iptr = &in[i];
    ຖ້າ(!is_arg_used(usedinput, i))
      /* bad input */
      ສົ່ງຄືນ PFMT_INPUTGAP;

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
      iptr->val.numu = va_arg(arglist, unsigned ຈຳນວນເຕັມ);
      bha_nga;

    pak_sa MTYPE_INT:
    pak_sa MTYPE_WIDTH:
    pak_sa MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, ຈຳນວນເຕັມ);
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

  ສົ່ງຄືນ PFMT_OK;
}

sa_nracanaa mproperty {
  ຈຳນວນເຕັມ width;            /* Width of a field. */
  ຈຳນວນເຕັມ prec;             /* Precision of a field. */
  unsigned ຈຳນວນເຕັມ flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
typedef size_t (*mp_streamn)(const unsigned var_na *buf, size_t len, void *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
typedef ຈຳນວນເຕັມ (*mp_flush)(void *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
static bool stream_run(void *userp,
                       ຈຳນວນເຕັມ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       const unsigned var_na *buf, size_t len, ຈຳນວນເຕັມ *donep)
{
  ຖ້າ(!len)
    ສົ່ງຄືນ FALSE;
  ຖ້າ(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (ຈຳນວນເຕັມ)n;
    ສົ່ງຄືນ n != len;
  }
  ໃນຂະນະທີ່(len--) {
    ຖ້າ(stream(*buf++, userp))
      ສົ່ງຄືນ TRUE;
    (*donep)++;
  }
  ສົ່ງຄືນ FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
static const var_na pad_spaces[] = "                ";
static const var_na pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
static bool stream_pad(void *userp,
                       ຈຳນວນເຕັມ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       unsigned var_na pad, ຈຳນວນເຕັມ num, ຈຳນວນເຕັມ *donep)
{
  const unsigned var_na *src;
  ຖ້າ(num <= 0)
    ສົ່ງຄືນ FALSE;
  ຖ້າ(pad == '0')
    src = (const unsigned var_na *)pad_zeros;
  ບໍ່ດັ່ງນັ້ນ
    src = (const unsigned var_na *)pad_spaces;
  ໃນຂະນະທີ່(num > PAD_RUN) {
    ຖ້າ(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      ສົ່ງຄືນ TRUE;
    num -= PAD_RUN;
  }
  ສົ່ງຄືນ stream_run(userp, stream, streamn, src, (size_t)num, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
static bool stream_zrun(void *userp,
                        ຈຳນວນເຕັມ (*stream)(unsigned var_na, void *),
                        mp_streamn streamn,
                        const var_na *str, size_t len, ຈຳນວນເຕັມ *donep)
{
  size_t n = 0;
  ໃນຂະນະທີ່(n < len && str[n])
    n++;
  ສົ່ງຄືນ stream_run(userp, stream, streamn,
                    (const unsigned var_na *)str, n, donep);
}

static bool out_double(void *userp,
                       ຈຳນວນເຕັມ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       dviguu_na dnum,
                       var_na *work, ຈຳນວນເຕັມ *donep)
{
  var_na fmt[32] = "%";
  var_na *fptr = &fmt[1];
  size_t left = parimaa_na(fmt) - strlen(fmt);
  ຈຳນວນເຕັມ flags = p->flags;
  ຈຳນວນເຕັມ width = p->width;
  ຈຳນວນເຕັມ prec = p->prec;

  ຖ້າ(flags & FLAGS_LEFT)
    *fptr++ = '-';
  ຖ້າ(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ຖ້າ(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ຖ້າ(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ຖ້າ(width >= 0) {
    size_t dlen;
    ຖ້າ(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, left, "%d", width);
    fptr += dlen;
    left -= dlen;
  }
  ຖ້າ(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    ຈຳນວນເຕັມ maxprec = BUFFSIZE - 1;
    dviguu_na val = dnum;
    ຈຳນວນເຕັມ len;
    ຖ້າ(prec > maxprec)
      prec = maxprec - 1;
    ຖ້າ(width > 0 && prec <= width)
      maxprec -= width;
    ໃນຂະນະທີ່(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ຖ້າ(prec > maxprec)
      prec = maxprec - 1;
    ຖ້າ(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, left, ".%d", prec);
    fptr += len;
  }
  ຖ້າ(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ຖ້າ(flags & FLAGS_FLOATE)
    *fptr++ = (var_na)((flags & FLAGS_UPPER) ? 'E' : 'e');
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_FLOATG)
    *fptr++ = (var_na)((flags & FLAGS_UPPER) ? 'G' : 'g');
  ບໍ່ດັ່ງນັ້ນ
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
#ບໍ່ດັ່ງນັ້ນ
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  ສົ່ງຄືນ stream_run(userp, stream, streamn,
                    (const unsigned var_na *)work, strlen(work), donep);
}

static bool out_number(void *userp,
                       ຈຳນວນເຕັມ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       uint64_t num,
                       int64_t nums,
                       var_na *work, ຈຳນວນເຕັມ *donep)
{
  const unsigned var_na *digits = Curl_ldigits;
  ຈຳນວນເຕັມ flags = p->flags;
  ຈຳນວນເຕັມ width = p->width;
  ຈຳນວນເຕັມ prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  ຈຳນວນເຕັມ base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  var_na *workend = &work[BUFFSIZE - 2];
  var_na *w;

  ຖ້າ(flags & FLAGS_CHAR) {
    /* Character. */
    ຖ້າ(!(flags & FLAGS_LEFT)) {
      ຖ້າ(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ສົ່ງຄືນ TRUE;
      width = 1;
    }
    OUTCHAR((var_na)num);
    ຖ້າ(flags & FLAGS_LEFT) {
      ຖ້າ(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ສົ່ງຄືນ TRUE;
    }
    ສົ່ງຄືນ FALSE;
  }
  ຖ້າ(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  ບໍ່ດັ່ງນັ້ນ {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ຖ້າ(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_num; /* Used to convert negative in positive. */
      signed_num = nums + (int64_t)1;
      signed_num = -signed_num;
      num = (uint64_t)signed_num;
      num += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ຖ້າ(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  switch(base) {
  pak_sa 10:
    ໃນຂະນະທີ່(num > 0) {
      *w-- = (var_na)('0' + (num % 10));
      num /= 10;
    }
    bha_nga;
  default:
    ໃນຂະນະທີ່(num > 0) {
      *w-- = digits[num % base];
      num /= base;
    }
    bha_nga;
  }
  width -= (ຈຳນວນເຕັມ)(workend - w);
  prec -= (ຈຳນວນເຕັມ)(workend - w);

  ຖ້າ(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  ຖ້າ(prec > 0) {
    width -= prec;
    ໃນຂະນະທີ່(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ຖ້າ(is_alt && base == 16)
    width -= 2;

  ຖ້າ(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  ຖ້າ(!(flags & FLAGS_LEFT) && !(flags & FLAGS_PAD_NIL)) {
    ຖ້າ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ສົ່ງຄືນ TRUE;
    width = 0;
  }

  ຖ້າ(is_neg)
    OUTCHAR('-');
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ຖ້າ(is_alt && base == 16) {
    OUTCHAR('0');
    ຖ້າ(flags & FLAGS_UPPER)
      OUTCHAR('X');
    ບໍ່ດັ່ງນັ້ນ
      OUTCHAR('x');
  }

  ຖ້າ(!(flags & FLAGS_LEFT) && (flags & FLAGS_PAD_NIL)) {
    ຖ້າ(stream_pad(userp, stream, streamn, '0', width, donep))
      ສົ່ງຄືນ TRUE;
    width = 0;
  }

  /* Write the number. */
  ຖ້າ(stream_run(userp, stream, streamn, (const unsigned var_na *)w + 1,
                (size_t)(workend - w), donep))
    ສົ່ງຄືນ TRUE;

  ຖ້າ(flags & FLAGS_LEFT) {
    ຖ້າ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ສົ່ງຄືນ TRUE;
  }

  ສົ່ງຄືນ FALSE;
}

static const var_na nilstr[] = "(nil)";

static bool out_string(void *userp,
                       ຈຳນວນເຕັມ (*stream)(unsigned var_na, void *),
                       mp_streamn streamn,
                       sa_nracanaa mproperty *p,
                       const var_na *str,
                       ຈຳນວນເຕັມ *donep)
{
  ຈຳນວນເຕັມ flags = p->flags;
  ຈຳນວນເຕັມ width = p->width;
  ຈຳນວນເຕັມ prec = p->prec;
  size_t len;

  ຖ້າ(!str) {
    /* Write null string if there is space. */
    ຖ້າ(prec == -1 || prec >= (ຈຳນວນເຕັມ)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned ຈຳນວນເຕັມ)FLAGS_ALT;
    }
    ບໍ່ດັ່ງນັ້ນ {
      str = "";
      len = 0;
    }
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(prec != -1)
    len = (size_t)prec;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(*str == '\0')
    len = 0;
  ບໍ່ດັ່ງນັ້ນ
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (ຈຳນວນເຕັມ)len;

  ຖ້າ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ຖ້າ(!(flags & FLAGS_LEFT)) {
    ຖ້າ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ສົ່ງຄືນ TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ຖ້າ(prec == -1) {
    ຖ້າ(stream_run(userp, stream, streamn,
                  (const unsigned var_na *)str, len, donep))
      ສົ່ງຄືນ TRUE;
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(stream_zrun(userp, stream, streamn, str, len, donep))
    ສົ່ງຄືນ TRUE;
  ຖ້າ(flags & FLAGS_LEFT) {
    ຖ້າ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ສົ່ງຄືນ TRUE;
  }

  ຖ້າ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ສົ່ງຄືນ FALSE;
}

static bool out_pointer(void *userp,
                        ຈຳນວນເຕັມ (*stream)(unsigned var_na, void *),
                        mp_streamn streamn,
                        sa_nracanaa mproperty *p,
                        const var_na *ptr,
                        var_na *work,
                        ຈຳນວນເຕັມ *donep)
{
  /* Generic pointer. */
  ຖ້າ(ptr) {
    size_t num = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ຖ້າ(out_number(userp, stream, streamn, p, num, 0, work, donep))
      ສົ່ງຄືນ TRUE;
  }
  ບໍ່ດັ່ງນັ້ນ {
    /* Write "(nil)" for a nil pointer. */
    ຈຳນວນເຕັມ width = p->width;
    ຈຳນວນເຕັມ flags = p->flags;

    width -= (ຈຳນວນເຕັມ)CURL_CSTRLEN(nilstr);
    ຖ້າ(flags & FLAGS_LEFT) {
      ຖ້າ(stream_pad(userp, stream, streamn, ' ', width, donep))
        ສົ່ງຄືນ TRUE;
      width = 0;
    }
    ຖ້າ(stream_run(userp, stream, streamn, (const unsigned var_na *)nilstr,
                  parimaa_na(nilstr) - 1, donep))
      ສົ່ງຄືນ TRUE;
    ຖ້າ(!(flags & FLAGS_LEFT)) {
      ຖ້າ(stream_pad(userp, stream, streamn, ' ', width, donep))
        ສົ່ງຄືນ TRUE;
    }
  }
  ສົ່ງຄືນ FALSE;
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
static ຈຳນວນເຕັມ formatf(void *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   ຈຳນວນເຕັມ (*stream)(unsigned var_na, void *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   const var_na *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  ຈຳນວນເຕັມ done = 0;   /* number of characters written */
  ຈຳນວນເຕັມ i;
  ຈຳນວນເຕັມ ocount = 0; /* number of output segments */
  ຈຳນວນເຕັມ icount = 0; /* number of input arguments */

  sa_nracanaa outsegment output[MAX_SEGMENTS];
  sa_nracanaa va_input input[MAX_PARAMETERS];
  var_na work[BUFFSIZE + 2];

  /* Parse the format string */
  ຖ້າ(parsefmt(format, output, input, &ocount, &icount, ap_save))
    ສົ່ງຄືນ 0;

  ສຳລັບ(i = 0; i < ocount; i++) {
    sa_nracanaa outsegment *optr = &output[i];
    sa_nracanaa va_input *iptr = &input[optr->input];
    sa_nracanaa mproperty p;
    size_t outlen = optr->outlen;

    ຖ້າ(outlen) {
      ຖ້າ(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        ສົ່ງຄືນ done;
      ຖ້າ(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        continue;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ຖ້າ(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (ຈຳນວນເຕັມ)input[optr->width].val.nums;
      ຖ້າ(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ຖ້າ(p.width == INT_MIN)
          p.width = INT_MAX;
        ບໍ່ດັ່ງນັ້ນ
          p.width = -p.width;
        p.flags |= FLAGS_LEFT;
        p.flags &= ~(unsigned ຈຳນວນເຕັມ)FLAGS_PAD_NIL;
      }
    }
    ບໍ່ດັ່ງນັ້ນ
      p.width = optr->width;

    /* pick up the specified precision */
    ຖ້າ(p.flags & FLAGS_PRECPARAM) {
      p.prec = (ຈຳນວນເຕັມ)input[optr->precision].val.nums;
      ຖ້າ(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    ບໍ່ດັ່ງນັ້ນ
      p.prec = -1;

    switch(iptr->type) {
    pak_sa MTYPE_INTU:
    pak_sa MTYPE_LONGU:
    pak_sa MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ຖ້າ(out_number(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        ສົ່ງຄືນ done;
      bha_nga;

    pak_sa MTYPE_INT:
    pak_sa MTYPE_LONG:
    pak_sa MTYPE_LONGLONG:
      ຖ້າ(out_number(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        ສົ່ງຄືນ done;
      bha_nga;

    pak_sa MTYPE_STRING:
      ຖ້າ(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        ສົ່ງຄືນ done;
      bha_nga;

    pak_sa MTYPE_PTR:
      ຖ້າ(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        ສົ່ງຄືນ done;
      bha_nga;

    pak_sa MTYPE_DOUBLE:
    pak_sa MTYPE_LONGDOUBLE:
      ຖ້າ(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        ສົ່ງຄືນ done;
      bha_nga;

    pak_sa MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ຖ້າ(flush && flush(userp))
        ສົ່ງຄືນ done;
      /* Answer the count of characters written. */
      ຖ້າ(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(!(p.flags & FLAGS_SHORT))
        *(ຈຳນວນເຕັມ *)iptr->val.ptr = done;
      ບໍ່ດັ່ງນັ້ນ
        *(short *)iptr->val.ptr = (short)done;
      bha_nga;

    default:
      bha_nga;
    }
  }
  ສົ່ງຄືນ done;
}

/* fputc() look-alike */
static ຈຳນວນເຕັມ addbyter(unsigned var_na outc, void *f)
{
  sa_nracanaa nsprintf *infop = f;
  ຖ້າ(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (var_na)outc; /* store */
    infop->length++; /* we are now one byte larger */
    ສົ່ງຄືນ 0;     /* fputc() returns like this on success */
  }
  ສົ່ງຄືນ 1;
}

/* block variant of addbyter */
static size_t addrun(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  ຖ້າ(len > fit)
    len = fit;
  ຖ້າ(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  ສົ່ງຄືນ len;
}

ຈຳນວນເຕັມ curl_mvsnprintf(var_na *buffer, size_t maxlength, const var_na *format,
                    va_list args)
{
  ຈຳນວນເຕັມ retcode;
  sa_nracanaa nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, NULL, format, args);
  ຖ້າ(info.max) {
    /* we null-terminate this */
    ຖ້າ(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    ບໍ່ດັ່ງນັ້ນ
      info.buffer[0] = 0;
  }
  ສົ່ງຄືນ retcode;
}

ຈຳນວນເຕັມ curl_msnprintf(var_na *buffer, size_t maxlength, const var_na *format, ...)
{
  ຈຳນວນເຕັມ retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  ສົ່ງຄືນ retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
static ຈຳນວນເຕັມ alloc_flush(void *f)
{
  sa_nracanaa asprintf *infop = f;
  ຖ້າ(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ຖ້າ(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      ສົ່ງຄືນ 1; /* fail */
    }
  }
  ສົ່ງຄືນ 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
static ຈຳນວນເຕັມ alloc_addn(sa_nracanaa asprintf *infop, const unsigned var_na *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  ຖ້າ(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    ສົ່ງຄືນ 1; /* fail */
  }
  ສົ່ງຄືນ 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
static bool alloc_fits(sa_nracanaa asprintf *infop, size_t more)
{
  ສົ່ງຄືນ (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
static ຈຳນວນເຕັມ alloc_addbyter(unsigned var_na outc, void *f)
{
  sa_nracanaa asprintf *infop = f;
  ຖ້າ(infop->nstage == parimaa_na(infop->stage) && alloc_flush(infop))
    ສົ່ງຄືນ 1; /* fail */
  ຖ້າ(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ຖ້າ(alloc_flush(infop))
      ສົ່ງຄືນ 1;
    ສົ່ງຄືນ alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  ສົ່ງຄືນ 0;
}

/* block variant of alloc_addbyter */
static size_t alloc_addrun(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa asprintf *infop = f;
  size_t accepted = 0;
  ໃນຂະນະທີ່(accepted < len) {
    size_t room = parimaa_na(infop->stage) - infop->nstage;
    size_t n;
    ຖ້າ(!room) {
      ຖ້າ(alloc_flush(infop))
        ສົ່ງຄືນ accepted; /* accepted < len aborts formatting */
      room = parimaa_na(infop->stage);
    }
    n = len - accepted;
    ຖ້າ(n > room)
      n = room;
    ຖ້າ(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ຖ້າ(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      ຖ້າ(alloc_flush(infop))
        ສົ່ງຄືນ accepted;
      ຖ້າ(alloc_addn(infop, buf + accepted, 1))
        ສົ່ງຄືນ accepted; /* accepted < len aborts formatting */
      accepted++;
      continue;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  ສົ່ງຄືນ accepted;
}

/* appends the formatted string, returns MERR error code */
ຈຳນວນເຕັມ curlx_dyn_vprintf(sa_nracanaa dynbuf *dyn, const var_na *format, va_list args)
{
  sa_nracanaa asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (void)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ຖ້າ(!info.merr)
    (void)alloc_flush(&info);
  ຖ້າ(info.merr) {
    curlx_dyn_free(info.b);
    ສົ່ງຄືນ info.merr;
  }
  ສົ່ງຄືນ 0;
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
  ຖ້າ(!info.merr)
    (void)alloc_flush(&info);
  ຖ້າ(info.merr) {
    curlx_dyn_free(info.b);
    ສົ່ງຄືນ NULL;
  }
  ຖ້າ(curlx_dyn_len(info.b))
    ສົ່ງຄືນ curlx_dyn_ptr(info.b);
  ສົ່ງຄືນ curlx_strdup("");
}

var_na *curl_maprintf(const var_na *format, ...)
{
  va_list args;
  var_na *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  ສົ່ງຄືນ s;
}

static ຈຳນວນເຕັມ storebuffer(unsigned var_na outc, void *f)
{
  var_na **buffer = f;
  **buffer = (var_na)outc;
  (*buffer)++;
  ສົ່ງຄືນ 0;
}

/* block variant of storebuffer */
static size_t storerun(const unsigned var_na *buf, size_t len, void *f)
{
  var_na **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  ສົ່ງຄືນ len;
}

ຈຳນວນເຕັມ curl_msprintf(var_na *buffer, const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ຈຳນວນເຕັມ retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  ສົ່ງຄືນ retcode;
}

static ຈຳນວນເຕັມ fputc_wrapper(unsigned var_na outc, void *f)
{
  sa_nracanaa ioprintf *i = f;
  ຈຳນວນເຕັມ out = outc;
  FILE *s = i->fd;
  ຈຳນວນເຕັມ rc = fputc(out, s);
  i->error = (rc == EOF);
  ສົ່ງຄືນ (rc == EOF);
}

/* block variant of fputc_wrapper */
static size_t fwrite_wrapper(const unsigned var_na *buf, size_t len, void *f)
{
  sa_nracanaa ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  ສົ່ງຄືນ n;
}

ຈຳນວນເຕັມ curl_mvfprintf(FILE *fd, const var_na *format, va_list args)
{
  sa_nracanaa ioprintf info;
  ຈຳນວນເຕັມ n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, NULL, format, args);
  ຖ້າ(info.error)
    ສົ່ງຄືນ -1;
  ສົ່ງຄືນ n;
}

ຈຳນວນເຕັມ curl_mfprintf(FILE *fd, const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ຈຳນວນເຕັມ n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  ສົ່ງຄືນ n;
}

ຈຳນວນເຕັມ curl_mprintf(const var_na *format, ...)
{
  va_list args; /* argument pointer */
  ຈຳນວນເຕັມ n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  ສົ່ງຄືນ n;
}

ຈຳນວນເຕັມ curl_mvsprintf(var_na *buffer, const var_na *format, va_list args)
{
  ຈຳນວນເຕັມ retcode = formatf(&buffer, storebuffer, storerun, NULL, format, args);
  *buffer = 0; /* we null-terminate this */
  ສົ່ງຄືນ retcode;
}

ຈຳນວນເຕັມ curl_mvprintf(const var_na *format, va_list args)
{
  ສົ່ງຄືນ curl_mvfprintf(stdout, format, args);
}

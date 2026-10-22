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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "curl_printf.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ಧ್ರುವ unsigned ವರ್ಣ Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ಧ್ರುವ unsigned ವರ್ಣ Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  ಕುರು {                                    \
    ಯದಿ(stream((unsigned ವರ್ಣ)(x), userp)) \
      ಪ್ರತ್ಯಾಗಮ TRUE;                        \
    (*donep)++;                           \
  } ಯಾವತ್(0)

/* Data type to read from the arglist */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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
ಗಣನಾ {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_ಎಡ       = 1 << 2,
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

ಗಣನಾ {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
ಸಂರಚನಾ va_ಒಳಹರಿವು {
  FormatType type; /* FormatType */
  ಸಙ್ಘ {
    ಧ್ರುವ ವರ್ಣ *str;
    ಶೂನ್ಯ *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    ದ್ವಿಗುಣ dnum;
  } val;
};

/*
 * Describes an output segment.
 */
ಸಂರಚನಾ outsegment {
  ಪೂರ್ಣಾಙ್ಕ width;     /* width OR width parameter number */
  ಪೂರ್ಣಾಙ್ಕ precision; /* precision OR precision parameter number */
  unsigned ಪೂರ್ಣಾಙ್ಕ flags;
  unsigned ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು; /* input argument array index */
  ಧ್ರುವ ವರ್ಣ *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

ಸಂರಚನಾ nsprintf {
  ವರ್ಣ *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

ಸಂರಚನಾ asprintf {
  ಸಂರಚನಾ dynbuf *b;
  ವರ್ಣ merr;
  size_t nstage;
  unsigned ವರ್ಣ stage[ASPRINTF_STAGE_SIZE];
};

ಸಂರಚನಾ ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dollarstring(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ **end)
{
  curl_off_t ಅಂಕೆ;
  ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !ಅಂಕೆ)
    ಪ್ರತ್ಯಾಗಮ -1;
  *end = p;
  ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned ವರ್ಣ)(1 << ((y) & 7)))

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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_flags(ಧ್ರುವ ವರ್ಣ **fmtp, unsigned ಪೂರ್ಣಾಙ್ಕ *flagsp, ಪೂರ್ಣಾಙ್ಕ use_dollar,
                       ಪೂರ್ಣಾಙ್ಕ *precp, ಪೂರ್ಣಾಙ್ಕ *widthp)
{
  ಧ್ರುವ ವರ್ಣ *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned ಪೂರ್ಣಾಙ್ಕ flags = 0;
  ಪೂರ್ಣಾಙ್ಕ width = 0;
  ಪೂರ್ಣಾಙ್ಕ precision = 0;

  /* Handle the flags */
  ಕುರು {
    ವಿಕಲ್ಪ(*fmt++) {
    ಪಕ್ಷ ' ':
      flags |= FLAGS_SPACE;
      ಭಙ್ಗ;
    ಪಕ್ಷ '+':
      flags |= FLAGS_SHOWSIGN;
      ಭಙ್ಗ;
    ಪಕ್ಷ '-':
      flags |= FLAGS_ಎಡ;
      flags &= ~(unsigned ಪೂರ್ಣಾಙ್ಕ)FLAGS_PAD_NIL;
      ಭಙ್ಗ;
    ಪಕ್ಷ '#':
      flags |= FLAGS_ALT;
      ಭಙ್ಗ;
    ಪಕ್ಷ '.':
      ಯದಿ('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ಯದಿ(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ಯದಿ(precision < 0)
            /* illegal combo */
            ಪ್ರತ್ಯಾಗಮ PFMT_DOLLARPREC;
        }
        ಅನ್ಯಥಾ
          /* get it from the next argument */
          precision = -1;
      }
      ಅನ್ಯಥಾ {
        bool is_neg;
        curl_off_t ಅಂಕೆ;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ಯದಿ(is_neg)
          fmt++;
        ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&fmt, &ಅಂಕೆ, INT_MAX))
          ಪ್ರತ್ಯಾಗಮ PFMT_PREC;
        precision = (ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ;
        ಯದಿ(is_neg)
          precision = -precision;
      }
      ಯದಿ((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        ಪ್ರತ್ಯಾಗಮ PFMT_PRECMIX;
      ಭಙ್ಗ;
    ಪಕ್ಷ 'h':
      flags |= FLAGS_SHORT;
      ಭಙ್ಗ;
#ifdef _WIN32
    ಪಕ್ಷ 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ಯದಿ((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      ಅನ್ಯಥಾ ಯದಿ((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      ಅನ್ಯಥಾ {
#ಯದಿ SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#ಅನ್ಯಥಾ
        flags |= FLAGS_LONG;
#endif
      }
      ಭಙ್ಗ;
#endif /* _WIN32 */
    ಪಕ್ಷ 'l':
      ಯದಿ(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      ಅನ್ಯಥಾ
        flags |= FLAGS_LONG;
      ಭಙ್ಗ;
    ಪಕ್ಷ 'L':
      flags |= FLAGS_LONGDOUBLE;
      ಭಙ್ಗ;
    ಪಕ್ಷ 'q':
      flags |= FLAGS_LONGLONG;
      ಭಙ್ಗ;
    ಪಕ್ಷ 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#ಯದಿ SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ಅನ್ಯಥಾ
      flags |= FLAGS_LONG;
#endif
      ಭಙ್ಗ;
    ಪಕ್ಷ 'O':
#ಯದಿ SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ಅನ್ಯಥಾ
      flags |= FLAGS_LONG;
#endif
      ಭಙ್ಗ;
    ಪಕ್ಷ '0':
      ಯದಿ(!(flags & FLAGS_ಎಡ))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    ಪಕ್ಷ '1':
    ಪಕ್ಷ '2':
    ಪಕ್ಷ '3':
    ಪಕ್ಷ '4':
    ಪಕ್ಷ '5':
    ಪಕ್ಷ '6':
    ಪಕ್ಷ '7':
    ಪಕ್ಷ '8':
    ಪಕ್ಷ '9': {
      curl_off_t ಅಂಕೆ;
      flags |= FLAGS_WIDTH;
      fmt--;
      ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&fmt, &ಅಂಕೆ, INT_MAX))
        ಪ್ರತ್ಯಾಗಮ PFMT_WIDTH;
      width = (ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ;
      ಭಙ್ಗ;
    }
    ಪಕ್ಷ '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ಯದಿ(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        ಯದಿ(width < 0)
          /* illegal combo */
          ಪ್ರತ್ಯಾಗಮ PFMT_DOLLARWIDTH;
      }
      ಅನ್ಯಥಾ
        /* pick from the next argument */
        width = -1;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      loopit = FALSE;
      fmt--;
      ಭಙ್ಗ;
    } /* switch */
  } ಯಾವತ್(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  ಪ್ರತ್ಯಾಗಮ PFMT_OK;
}

ಸ್ಥಿರ bool parse_conversion(ಧ್ರುವ ವರ್ಣ f, unsigned ಪೂರ್ಣಾಙ್ಕ *flagp,
                             FormatType *typep)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ flags = *flagp;
  FormatType type;
  ವಿಕಲ್ಪ(f) {
  ಪಕ್ಷ 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    ಭಙ್ಗ;
  ಪಕ್ಷ 's':
    type = MTYPE_STRING;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'n':
    type = MTYPE_INTPTR;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'p':
    type = MTYPE_PTR;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'd':
  ಪಕ್ಷ 'i':
    ಯದಿ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    ಅನ್ಯಥಾ
      type = MTYPE_INT;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'u':
    ಯದಿ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ಅನ್ಯಥಾ
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'o':
    ಯದಿ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ಅನ್ಯಥಾ
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'x':
    ಯದಿ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ಅನ್ಯಥಾ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'X':
    ಯದಿ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ಅನ್ಯಥಾ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    /* invalid instruction, disregard and continue */
    ಪ್ರತ್ಯಾಗಮ TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parsefmt(ಧ್ರುವ ವರ್ಣ *format,
                    ಸಂರಚನಾ outsegment *out,
                    ಸಂರಚನಾ va_ಒಳಹರಿವು *in,
                    ಪೂರ್ಣಾಙ್ಕ *opieces,
                    ಪೂರ್ಣಾಙ್ಕ *ipieces, va_list arglist)
{
  ಧ್ರುವ ವರ್ಣ *fmt = format;
  ಪೂರ್ಣಾಙ್ಕ param_ಅಂಕೆ = 0;
  ಪೂರ್ಣಾಙ್ಕ max_param = -1;
  ಪೂರ್ಣಾಙ್ಕ i;
  ಪೂರ್ಣಾಙ್ಕ ocount = 0;
  unsigned ವರ್ಣ usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  ಸಂರಚನಾ outsegment *optr;
  ಪೂರ್ಣಾಙ್ಕ use_dollar = DOLLAR_UNKNOWN;
  ಧ್ರುವ ವರ್ಣ *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, ಪರಿಮಾಣ(usedinput));

  ಯಾವತ್(*fmt) {
    ಯದಿ(*fmt == '%') {
      ಸಂರಚನಾ va_ಒಳಹರಿವು *iptr;
      FormatType type;
      unsigned ಪೂರ್ಣಾಙ್ಕ flags = 0;
      ಪೂರ್ಣಾಙ್ಕ width = 0;
      ಪೂರ್ಣಾಙ್ಕ precision = 0;
      ಪೂರ್ಣಾಙ್ಕ param = -1;
      ಪೂರ್ಣಾಙ್ಕ rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      ಯದಿ(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ಯದಿ(outlen) {
          optr = &out[ocount++];
          ಯದಿ(ocount > MAX_SEGMENTS)
            ಪ್ರತ್ಯಾಗಮ PFMT_MANYSEGS;
          optr->ಒಳಹರಿವು = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        ಅನುವರ್ತ; /* while */
      }

      ಯದಿ(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ಯದಿ(param < 0) {
          ಯದಿ(use_dollar == DOLLAR_USE)
            /* illegal combo */
            ಪ್ರತ್ಯಾಗಮ PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        ಅನ್ಯಥಾ
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      ಯದಿ(rc)
        ಪ್ರತ್ಯಾಗಮ rc;

      ಯದಿ(parse_conversion(*fmt, &flags, &type))
        ಅನುವರ್ತ;

      ಯದಿ(flags & FLAGS_WIDTHPARAM) {
        ಯದಿ(width < 0)
          width = param_ಅಂಕೆ++;
        ಅನ್ಯಥಾ {
          /* if this identifies a parameter already used, this is illegal */
          ಯದಿ(is_arg_used(usedinput, width))
            ಪ್ರತ್ಯಾಗಮ PFMT_WIDTHARG;
        }
        ಯದಿ(width >= MAX_PARAMETERS)
          ಪ್ರತ್ಯಾಗಮ PFMT_MANYARGS;
        ಯದಿ(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      ಯದಿ(flags & FLAGS_PRECPARAM) {
        ಯದಿ(precision < 0)
          precision = param_ಅಂಕೆ++;
        ಅನ್ಯಥಾ {
          /* if this identifies a parameter already used, this is illegal */
          ಯದಿ(is_arg_used(usedinput, precision))
            ಪ್ರತ್ಯಾಗಮ PFMT_PRECARG;
        }
        ಯದಿ(precision >= MAX_PARAMETERS)
          ಪ್ರತ್ಯಾಗಮ PFMT_MANYARGS;
        ಯದಿ(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      ಯದಿ(param < 0)
        param = param_ಅಂಕೆ++;
      ಯದಿ(param >= MAX_PARAMETERS)
        ಪ್ರತ್ಯಾಗಮ PFMT_MANYARGS;
      ಯದಿ(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      ಯದಿ(ocount > MAX_SEGMENTS)
        ಪ್ರತ್ಯಾಗಮ PFMT_MANYSEGS;
      optr->ಒಳಹರಿವು = (unsigned ಪೂರ್ಣಾಙ್ಕ)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    ಅನ್ಯಥಾ
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  ಯದಿ(outlen) {
    optr = &out[ocount++];
    ಯದಿ(ocount > MAX_SEGMENTS)
      ಪ್ರತ್ಯಾಗಮ PFMT_MANYSEGS;
    optr->ಒಳಹರಿವು = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  ಪ್ರತಿ(i = 0; i < max_param + 1; i++) {
    ಸಂರಚನಾ va_ಒಳಹರಿವು *iptr = &in[i];
    ಯದಿ(!is_arg_used(usedinput, i))
      /* bad input */
      ಪ್ರತ್ಯಾಗಮ PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    ವಿಕಲ್ಪ(iptr->type) {
    ಪಕ್ಷ MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ಧ್ರುವ ವರ್ಣ *);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_INTPTR:
    ಪಕ್ಷ MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, ಶೂನ್ಯ *);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned ಪೂರ್ಣಾಙ್ಕ);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_INT:
    ಪಕ್ಷ MTYPE_WIDTH:
    ಪಕ್ಷ MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, ಪೂರ್ಣಾಙ್ಕ);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, ದ್ವಿಗುಣ);
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_LONGDOUBLE:
      iptr->val.dnum = (ದ್ವಿಗುಣ)va_arg(arglist, long ದ್ವಿಗುಣ);
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      DEBUGASSERT(ರಿಕ್ತ); /* unexpected */
      ಭಙ್ಗ;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  ಪ್ರತ್ಯಾಗಮ PFMT_OK;
}

ಸಂರಚನಾ mproperty {
  ಪೂರ್ಣಾಙ್ಕ width;            /* Width of a field. */
  ಪೂರ್ಣಾಙ್ಕ prec;             /* Precision of a field. */
  unsigned ಪೂರ್ಣಾಙ್ಕ flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
ಪ್ರಕಾರನಿರ್ದೇಶ size_t (*mp_streamn)(ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t len, ಶೂನ್ಯ *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*mp_flush)(ಶೂನ್ಯ *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
ಸ್ಥಿರ bool stream_run(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಙ್ಕ (*stream)(unsigned ವರ್ಣ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t len, ಪೂರ್ಣಾಙ್ಕ *donep)
{
  ಯದಿ(!len)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (ಪೂರ್ಣಾಙ್ಕ)n;
    ಪ್ರತ್ಯಾಗಮ n != len;
  }
  ಯಾವತ್(len--) {
    ಯದಿ(stream(*buf++, userp))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    (*donep)++;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ pad_spaces[] = "                ";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
ಸ್ಥಿರ bool stream_pad(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಙ್ಕ (*stream)(unsigned ವರ್ಣ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       unsigned ವರ್ಣ pad, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ, ಪೂರ್ಣಾಙ್ಕ *donep)
{
  ಧ್ರುವ unsigned ವರ್ಣ *src;
  ಯದಿ(ಅಂಕೆ <= 0)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(pad == '0')
    src = (ಧ್ರುವ unsigned ವರ್ಣ *)pad_zeros;
  ಅನ್ಯಥಾ
    src = (ಧ್ರುವ unsigned ವರ್ಣ *)pad_spaces;
  ಯಾವತ್(ಅಂಕೆ > PAD_RUN) {
    ಯದಿ(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    ಅಂಕೆ -= PAD_RUN;
  }
  ಪ್ರತ್ಯಾಗಮ stream_run(userp, stream, streamn, src, (size_t)ಅಂಕೆ, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
ಸ್ಥಿರ bool stream_zrun(ಶೂನ್ಯ *userp,
                        ಪೂರ್ಣಾಙ್ಕ (*stream)(unsigned ವರ್ಣ, ಶೂನ್ಯ *),
                        mp_streamn streamn,
                        ಧ್ರುವ ವರ್ಣ *str, size_t len, ಪೂರ್ಣಾಙ್ಕ *donep)
{
  size_t n = 0;
  ಯಾವತ್(n < len && str[n])
    n++;
  ಪ್ರತ್ಯಾಗಮ stream_run(userp, stream, streamn,
                    (ಧ್ರುವ unsigned ವರ್ಣ *)str, n, donep);
}

ಸ್ಥಿರ bool out_double(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಙ್ಕ (*stream)(unsigned ವರ್ಣ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ಸಂರಚನಾ mproperty *p,
                       ದ್ವಿಗುಣ dnum,
                       ವರ್ಣ *work, ಪೂರ್ಣಾಙ್ಕ *donep)
{
  ವರ್ಣ fmt[32] = "%";
  ವರ್ಣ *fptr = &fmt[1];
  size_t ಎಡ = ಪರಿಮಾಣ(fmt) - strlen(fmt);
  ಪೂರ್ಣಾಙ್ಕ flags = p->flags;
  ಪೂರ್ಣಾಙ್ಕ width = p->width;
  ಪೂರ್ಣಾಙ್ಕ prec = p->prec;

  ಯದಿ(flags & FLAGS_ಎಡ)
    *fptr++ = '-';
  ಯದಿ(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ಯದಿ(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ಯದಿ(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ಯದಿ(width >= 0) {
    size_t dlen;
    ಯದಿ(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, ಎಡ, "%d", width);
    fptr += dlen;
    ಎಡ -= dlen;
  }
  ಯದಿ(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    ಪೂರ್ಣಾಙ್ಕ maxprec = BUFFSIZE - 1;
    ದ್ವಿಗುಣ val = dnum;
    ಪೂರ್ಣಾಙ್ಕ len;
    ಯದಿ(prec > maxprec)
      prec = maxprec - 1;
    ಯದಿ(width > 0 && prec <= width)
      maxprec -= width;
    ಯಾವತ್(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ಯದಿ(prec > maxprec)
      prec = maxprec - 1;
    ಯದಿ(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, ಎಡ, ".%d", prec);
    fptr += len;
  }
  ಯದಿ(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ಯದಿ(flags & FLAGS_FLOATE)
    *fptr++ = (ವರ್ಣ)((flags & FLAGS_UPPER) ? 'E' : 'e');
  ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_FLOATG)
    *fptr++ = (ವರ್ಣ)((flags & FLAGS_UPPER) ? 'G' : 'g');
  ಅನ್ಯಥಾ
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#ಅನ್ಯಥಾ
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತೆಗೆ
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  ಪ್ರತ್ಯಾಗಮ stream_run(userp, stream, streamn,
                    (ಧ್ರುವ unsigned ವರ್ಣ *)work, strlen(work), donep);
}

ಸ್ಥಿರ bool out_ಸಂಖ್ಯೆ(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಙ್ಕ (*stream)(unsigned ವರ್ಣ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ಸಂರಚನಾ mproperty *p,
                       uint64_t ಅಂಕೆ,
                       int64_t nums,
                       ವರ್ಣ *work, ಪೂರ್ಣಾಙ್ಕ *donep)
{
  ಧ್ರುವ unsigned ವರ್ಣ *digits = Curl_ldigits;
  ಪೂರ್ಣಾಙ್ಕ flags = p->flags;
  ಪೂರ್ಣಾಙ್ಕ width = p->width;
  ಪೂರ್ಣಾಙ್ಕ prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  ಪೂರ್ಣಾಙ್ಕ base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  ವರ್ಣ *workend = &work[BUFFSIZE - 2];
  ವರ್ಣ *w;

  ಯದಿ(flags & FLAGS_CHAR) {
    /* Character. */
    ಯದಿ(!(flags & FLAGS_ಎಡ)) {
      ಯದಿ(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ಪ್ರತ್ಯಾಗಮ TRUE;
      width = 1;
    }
    OUTCHAR((ವರ್ಣ)ಅಂಕೆ);
    ಯದಿ(flags & FLAGS_ಎಡ) {
      ಯದಿ(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಯದಿ(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  ಅನ್ಯಥಾ {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ಯದಿ(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_ಅಂಕೆ; /* Used to convert negative in positive. */
      signed_ಅಂಕೆ = nums + (int64_t)1;
      signed_ಅಂಕೆ = -signed_ಅಂಕೆ;
      ಅಂಕೆ = (uint64_t)signed_ಅಂಕೆ;
      ಅಂಕೆ += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ಯದಿ(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  ವಿಕಲ್ಪ(base) {
  ಪಕ್ಷ 10:
    ಯಾವತ್(ಅಂಕೆ > 0) {
      *w-- = (ವರ್ಣ)('0' + (ಅಂಕೆ % 10));
      ಅಂಕೆ /= 10;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಯಾವತ್(ಅಂಕೆ > 0) {
      *w-- = digits[ಅಂಕೆ % base];
      ಅಂಕೆ /= base;
    }
    ಭಙ್ಗ;
  }
  width -= (ಪೂರ್ಣಾಙ್ಕ)(workend - w);
  prec -= (ಪೂರ್ಣಾಙ್ಕ)(workend - w);

  ಯದಿ(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  ಯದಿ(prec > 0) {
    width -= prec;
    ಯಾವತ್(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ಯದಿ(is_alt && base == 16)
    width -= 2;

  ಯದಿ(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  ಯದಿ(!(flags & FLAGS_ಎಡ) && !(flags & FLAGS_PAD_NIL)) {
    ಯದಿ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    width = 0;
  }

  ಯದಿ(is_neg)
    OUTCHAR('-');
  ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  ಅನ್ಯಥಾ ಯದಿ(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ಯದಿ(is_alt && base == 16) {
    OUTCHAR('0');
    ಯದಿ(flags & FLAGS_UPPER)
      OUTCHAR('X');
    ಅನ್ಯಥಾ
      OUTCHAR('x');
  }

  ಯದಿ(!(flags & FLAGS_ಎಡ) && (flags & FLAGS_PAD_NIL)) {
    ಯದಿ(stream_pad(userp, stream, streamn, '0', width, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    width = 0;
  }

  /* Write the number. */
  ಯದಿ(stream_run(userp, stream, streamn, (ಧ್ರುವ unsigned ವರ್ಣ *)w + 1,
                (size_t)(workend - w), donep))
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಯದಿ(flags & FLAGS_ಎಡ) {
    ಯದಿ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ nilstr[] = "(nil)";

ಸ್ಥಿರ bool out_string(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಙ್ಕ (*stream)(unsigned ವರ್ಣ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ಸಂರಚನಾ mproperty *p,
                       ಧ್ರುವ ವರ್ಣ *str,
                       ಪೂರ್ಣಾಙ್ಕ *donep)
{
  ಪೂರ್ಣಾಙ್ಕ flags = p->flags;
  ಪೂರ್ಣಾಙ್ಕ width = p->width;
  ಪೂರ್ಣಾಙ್ಕ prec = p->prec;
  size_t len;

  ಯದಿ(!str) {
    /* Write null string if there is space. */
    ಯದಿ(prec == -1 || prec >= (ಪೂರ್ಣಾಙ್ಕ)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned ಪೂರ್ಣಾಙ್ಕ)FLAGS_ALT;
    }
    ಅನ್ಯಥಾ {
      str = "";
      len = 0;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(prec != -1)
    len = (size_t)prec;
  ಅನ್ಯಥಾ ಯದಿ(*str == '\0')
    len = 0;
  ಅನ್ಯಥಾ
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (ಪೂರ್ಣಾಙ್ಕ)len;

  ಯದಿ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ಯದಿ(!(flags & FLAGS_ಎಡ)) {
    ಯದಿ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ಯದಿ(prec == -1) {
    ಯದಿ(stream_run(userp, stream, streamn,
                  (ಧ್ರುವ unsigned ವರ್ಣ *)str, len, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(stream_zrun(userp, stream, streamn, str, len, donep))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಯದಿ(flags & FLAGS_ಎಡ) {
    ಯದಿ(stream_pad(userp, stream, streamn, ' ', width, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  ಯದಿ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ bool out_pointer(ಶೂನ್ಯ *userp,
                        ಪೂರ್ಣಾಙ್ಕ (*stream)(unsigned ವರ್ಣ, ಶೂನ್ಯ *),
                        mp_streamn streamn,
                        ಸಂರಚನಾ mproperty *p,
                        ಧ್ರುವ ವರ್ಣ *ptr,
                        ವರ್ಣ *work,
                        ಪೂರ್ಣಾಙ್ಕ *donep)
{
  /* Generic pointer. */
  ಯದಿ(ptr) {
    size_t ಅಂಕೆ = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ಯದಿ(out_ಸಂಖ್ಯೆ(userp, stream, streamn, p, ಅಂಕೆ, 0, work, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ {
    /* Write "(nil)" for a nil pointer. */
    ಪೂರ್ಣಾಙ್ಕ width = p->width;
    ಪೂರ್ಣಾಙ್ಕ flags = p->flags;

    width -= (ಪೂರ್ಣಾಙ್ಕ)CURL_CSTRLEN(nilstr);
    ಯದಿ(flags & FLAGS_ಎಡ) {
      ಯದಿ(stream_pad(userp, stream, streamn, ' ', width, donep))
        ಪ್ರತ್ಯಾಗಮ TRUE;
      width = 0;
    }
    ಯದಿ(stream_run(userp, stream, streamn, (ಧ್ರುವ unsigned ವರ್ಣ *)nilstr,
                  ಪರಿಮಾಣ(nilstr) - 1, donep))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ(!(flags & FLAGS_ಎಡ)) {
      ಯದಿ(stream_pad(userp, stream, streamn, ' ', width, donep))
        ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ formatf(ಶೂನ್ಯ *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   ಪೂರ್ಣಾಙ್ಕ (*stream)(unsigned ವರ್ಣ, ಶೂನ್ಯ *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ಧ್ರುವ ವರ್ಣ *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  ಪೂರ್ಣಾಙ್ಕ done = 0;   /* number of characters written */
  ಪೂರ್ಣಾಙ್ಕ i;
  ಪೂರ್ಣಾಙ್ಕ ocount = 0; /* number of output segments */
  ಪೂರ್ಣಾಙ್ಕ icount = 0; /* number of input arguments */

  ಸಂರಚನಾ outsegment output[MAX_SEGMENTS];
  ಸಂರಚನಾ va_ಒಳಹರಿವು ಒಳಹರಿವು[MAX_PARAMETERS];
  ವರ್ಣ work[BUFFSIZE + 2];

  /* Parse the format string */
  ಯದಿ(parsefmt(format, output, ಒಳಹರಿವು, &ocount, &icount, ap_save))
    ಪ್ರತ್ಯಾಗಮ 0;

  ಪ್ರತಿ(i = 0; i < ocount; i++) {
    ಸಂರಚನಾ outsegment *optr = &output[i];
    ಸಂರಚನಾ va_ಒಳಹರಿವು *iptr = &ಒಳಹರಿವು[optr->ಒಳಹರಿವು];
    ಸಂರಚನಾ mproperty p;
    size_t outlen = optr->outlen;

    ಯದಿ(outlen) {
      ಯದಿ(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        ಪ್ರತ್ಯಾಗಮ done;
      ಯದಿ(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        ಅನುವರ್ತ;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ಯದಿ(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (ಪೂರ್ಣಾಙ್ಕ)ಒಳಹರಿವು[optr->width].val.nums;
      ಯದಿ(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ಯದಿ(p.width == INT_MIN)
          p.width = INT_MAX;
        ಅನ್ಯಥಾ
          p.width = -p.width;
        p.flags |= FLAGS_ಎಡ;
        p.flags &= ~(unsigned ಪೂರ್ಣಾಙ್ಕ)FLAGS_PAD_NIL;
      }
    }
    ಅನ್ಯಥಾ
      p.width = optr->width;

    /* pick up the specified precision */
    ಯದಿ(p.flags & FLAGS_PRECPARAM) {
      p.prec = (ಪೂರ್ಣಾಙ್ಕ)ಒಳಹರಿವು[optr->precision].val.nums;
      ಯದಿ(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    ಅನ್ಯಥಾ ಯದಿ(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    ಅನ್ಯಥಾ
      p.prec = -1;

    ವಿಕಲ್ಪ(iptr->type) {
    ಪಕ್ಷ MTYPE_INTU:
    ಪಕ್ಷ MTYPE_LONGU:
    ಪಕ್ಷ MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ಯದಿ(out_ಸಂಖ್ಯೆ(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        ಪ್ರತ್ಯಾಗಮ done;
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_INT:
    ಪಕ್ಷ MTYPE_LONG:
    ಪಕ್ಷ MTYPE_LONGLONG:
      ಯದಿ(out_ಸಂಖ್ಯೆ(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        ಪ್ರತ್ಯಾಗಮ done;
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_STRING:
      ಯದಿ(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        ಪ್ರತ್ಯಾಗಮ done;
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_PTR:
      ಯದಿ(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        ಪ್ರತ್ಯಾಗಮ done;
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_DOUBLE:
    ಪಕ್ಷ MTYPE_LONGDOUBLE:
      ಯದಿ(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        ಪ್ರತ್ಯಾಗಮ done;
      ಭಙ್ಗ;

    ಪಕ್ಷ MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ಯದಿ(flush && flush(userp))
        ಪ್ರತ್ಯಾಗಮ done;
      /* Answer the count of characters written. */
      ಯದಿ(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      ಅನ್ಯಥಾ ಯದಿ(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      ಅನ್ಯಥಾ ಯದಿ(!(p.flags & FLAGS_SHORT))
        *(ಪೂರ್ಣಾಙ್ಕ *)iptr->val.ptr = done;
      ಅನ್ಯಥಾ
        *(short *)iptr->val.ptr = (short)done;
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ done;
}

/* fputc() look-alike */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ addbyter(unsigned ವರ್ಣ outc, ಶೂನ್ಯ *f)
{
  ಸಂರಚನಾ nsprintf *infop = f;
  ಯದಿ(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (ವರ್ಣ)outc; /* store */
    infop->length++; /* we are now one byte larger */
    ಪ್ರತ್ಯಾಗಮ 0;     /* fputc() returns like this on success */
  }
  ಪ್ರತ್ಯಾಗಮ 1;
}

/* block variant of addbyter */
ಸ್ಥಿರ size_t addrun(ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t len, ಶೂನ್ಯ *f)
{
  ಸಂರಚನಾ nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  ಯದಿ(len > fit)
    len = fit;
  ಯದಿ(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  ಪ್ರತ್ಯಾಗಮ len;
}

ಪೂರ್ಣಾಙ್ಕ curl_mvsnprintf(ವರ್ಣ *buffer, size_t maxlength, ಧ್ರುವ ವರ್ಣ *format,
                    va_list args)
{
  ಪೂರ್ಣಾಙ್ಕ retcode;
  ಸಂರಚನಾ nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, ರಿಕ್ತ, format, args);
  ಯದಿ(info.max) {
    /* we null-terminate this */
    ಯದಿ(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    ಅನ್ಯಥಾ
      info.buffer[0] = 0;
  }
  ಪ್ರತ್ಯಾಗಮ retcode;
}

ಪೂರ್ಣಾಙ್ಕ curl_msnprintf(ವರ್ಣ *buffer, size_t maxlength, ಧ್ರುವ ವರ್ಣ *format, ...)
{
  ಪೂರ್ಣಾಙ್ಕ retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  ಪ್ರತ್ಯಾಗಮ retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ alloc_flush(ಶೂನ್ಯ *f)
{
  ಸಂರಚನಾ asprintf *infop = f;
  ಯದಿ(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ಯದಿ(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      ಪ್ರತ್ಯಾಗಮ 1; /* fail */
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ alloc_addn(ಸಂರಚನಾ asprintf *infop, ಧ್ರುವ unsigned ವರ್ಣ *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  ಯದಿ(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    ಪ್ರತ್ಯಾಗಮ 1; /* fail */
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
ಸ್ಥಿರ bool alloc_fits(ಸಂರಚನಾ asprintf *infop, size_t more)
{
  ಪ್ರತ್ಯಾಗಮ (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ alloc_addbyter(unsigned ವರ್ಣ outc, ಶೂನ್ಯ *f)
{
  ಸಂರಚನಾ asprintf *infop = f;
  ಯದಿ(infop->nstage == ಪರಿಮಾಣ(infop->stage) && alloc_flush(infop))
    ಪ್ರತ್ಯಾಗಮ 1; /* fail */
  ಯದಿ(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ಯದಿ(alloc_flush(infop))
      ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* block variant of alloc_addbyter */
ಸ್ಥಿರ size_t alloc_addrun(ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t len, ಶೂನ್ಯ *f)
{
  ಸಂರಚನಾ asprintf *infop = f;
  size_t accepted = 0;
  ಯಾವತ್(accepted < len) {
    size_t room = ಪರಿಮಾಣ(infop->stage) - infop->nstage;
    size_t n;
    ಯದಿ(!room) {
      ಯದಿ(alloc_flush(infop))
        ಪ್ರತ್ಯಾಗಮ accepted; /* accepted < len aborts formatting */
      room = ಪರಿಮಾಣ(infop->stage);
    }
    n = len - accepted;
    ಯದಿ(n > room)
      n = room;
    ಯದಿ(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ಯದಿ(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      ಯದಿ(alloc_flush(infop))
        ಪ್ರತ್ಯಾಗಮ accepted;
      ಯದಿ(alloc_addn(infop, buf + accepted, 1))
        ಪ್ರತ್ಯಾಗಮ accepted; /* accepted < len aborts formatting */
      accepted++;
      ಅನುವರ್ತ;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  ಪ್ರತ್ಯಾಗಮ accepted;
}

/* appends the formatted string, returns MERR error code */
ಪೂರ್ಣಾಙ್ಕ curlx_dyn_vprintf(ಸಂರಚನಾ dynbuf *dyn, ಧ್ರುವ ವರ್ಣ *format, va_list args)
{
  ಸಂರಚನಾ asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (ಶೂನ್ಯ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ಯದಿ(!info.merr)
    (ಶೂನ್ಯ)alloc_flush(&info);
  ಯದಿ(info.merr) {
    curlx_dyn_free(info.b);
    ಪ್ರತ್ಯಾಗಮ info.merr;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ *curl_mvaprintf(ಧ್ರುವ ವರ್ಣ *format, va_list args)
{
  ಸಂರಚನಾ asprintf info;
  ಸಂರಚನಾ dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (ಶೂನ್ಯ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ಯದಿ(!info.merr)
    (ಶೂನ್ಯ)alloc_flush(&info);
  ಯದಿ(info.merr) {
    curlx_dyn_free(info.b);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  ಯದಿ(curlx_dyn_len(info.b))
    ಪ್ರತ್ಯಾಗಮ curlx_dyn_ptr(info.b);
  ಪ್ರತ್ಯಾಗಮ curlx_strdup("");
}

ವರ್ಣ *curl_maprintf(ಧ್ರುವ ವರ್ಣ *format, ...)
{
  va_list args;
  ವರ್ಣ *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  ಪ್ರತ್ಯಾಗಮ s;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ storebuffer(unsigned ವರ್ಣ outc, ಶೂನ್ಯ *f)
{
  ವರ್ಣ **buffer = f;
  **buffer = (ವರ್ಣ)outc;
  (*buffer)++;
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* block variant of storebuffer */
ಸ್ಥಿರ size_t storerun(ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t len, ಶೂನ್ಯ *f)
{
  ವರ್ಣ **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  ಪ್ರತ್ಯಾಗಮ len;
}

ಪೂರ್ಣಾಙ್ಕ curl_msprintf(ವರ್ಣ *buffer, ಧ್ರುವ ವರ್ಣ *format, ...)
{
  va_list args; /* argument pointer */
  ಪೂರ್ಣಾಙ್ಕ retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, ರಿಕ್ತ, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  ಪ್ರತ್ಯಾಗಮ retcode;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fputc_wrapper(unsigned ವರ್ಣ outc, ಶೂನ್ಯ *f)
{
  ಸಂರಚನಾ ioprintf *i = f;
  ಪೂರ್ಣಾಙ್ಕ out = outc;
  FILE *s = i->fd;
  ಪೂರ್ಣಾಙ್ಕ rc = fputc(out, s);
  i->error = (rc == EOF);
  ಪ್ರತ್ಯಾಗಮ (rc == EOF);
}

/* block variant of fputc_wrapper */
ಸ್ಥಿರ size_t fwrite_wrapper(ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t len, ಶೂನ್ಯ *f)
{
  ಸಂರಚನಾ ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ curl_mvfprintf(FILE *fd, ಧ್ರುವ ವರ್ಣ *format, va_list args)
{
  ಸಂರಚನಾ ioprintf info;
  ಪೂರ್ಣಾಙ್ಕ n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, ರಿಕ್ತ, format, args);
  ಯದಿ(info.error)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ curl_mfprintf(FILE *fd, ಧ್ರುವ ವರ್ಣ *format, ...)
{
  va_list args; /* argument pointer */
  ಪೂರ್ಣಾಙ್ಕ n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ curl_mprintf(ಧ್ರುವ ವರ್ಣ *format, ...)
{
  va_list args; /* argument pointer */
  ಪೂರ್ಣಾಙ್ಕ n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ curl_mvsprintf(ವರ್ಣ *buffer, ಧ್ರುವ ವರ್ಣ *format, va_list args)
{
  ಪೂರ್ಣಾಙ್ಕ retcode = formatf(&buffer, storebuffer, storerun, ರಿಕ್ತ, format, args);
  *buffer = 0; /* we null-terminate this */
  ಪ್ರತ್ಯಾಗಮ retcode;
}

ಪೂರ್ಣಾಙ್ಕ curl_mvprintf(ಧ್ರುವ ವರ್ಣ *format, va_list args)
{
  ಪ್ರತ್ಯಾಗಮ curl_mvfprintf(stdout, format, args);
}

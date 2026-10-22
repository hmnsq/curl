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

#ವ್ಯಾಖ್ಯಾನಿಸು BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_PARAMETERS 128 /* number of input arguments */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_udigits[] = "0123456789ABCDEF";

#ವ್ಯಾಖ್ಯಾನಿಸು OUTCHAR(x)                        \
  ಮಾಡು {                                    \
    ಆದರೆ(ಹರಿವು((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(x), userp)) \
      ಹಿಂತಿರುಗಿ TRUE;                        \
    (*donep)++;                           \
  } ಆಗ(0)

/* Data type to read from the arglist */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
  MTYPE_ಅಗಲ,
  MTYPE_PRECISION
} FormatType;

/* conversion and display flags */
ಎಣಿಕೆ {
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
  FLAGS_ಊರ್ಧ್ವ      = 1 << 12,
  FLAGS_ಅಗಲ      = 1 << 13, /* '*' or '*<num>$' used */
  FLAGS_WIDTHPARAM = 1 << 14, /* width PARAMETER was specified */
  FLAGS_PREC       = 1 << 15, /* precision was specified */
  FLAGS_PRECPARAM  = 1 << 16, /* precision PARAMETER was specified */
  FLAGS_CHAR       = 1 << 17, /* %c story */
  FLAGS_FLOATE     = 1 << 18, /* %e or %E */
  FLAGS_FLOATG     = 1 << 19, /* %g or %G */
  FLAGS_SUBSTR     = 1 << 20  /* no input, only substring */
};

ಎಣಿಕೆ {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
ರಚನೆ va_ಒಳಹರಿವು {
  FormatType type; /* FormatType */
  ಒಕ್ಕೂಟ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
    ಶೂನ್ಯ *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    ದ್ವಿಗುಣ dnum;
  } val;
};

/*
 * Describes an output segment.
 */
ರಚನೆ outsegment {
  ಪೂರ್ಣಾಂಕ ಅಗಲ;     /* width OR width parameter number */
  ಪೂರ್ಣಾಂಕ precision; /* precision OR precision parameter number */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು; /* input argument array index */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

ರಚನೆ nsprintf {
  ಅಕ್ಷರ *ಬಫರ್;
  size_t ಉದ್ದಳತೆ;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#ವ್ಯಾಖ್ಯಾನಿಸು ASPRINTF_STAGE_ಗಾತ್ರ 512

ರಚನೆ asprintf {
  ರಚನೆ dynbuf *b;
  ಅಕ್ಷರ merr;
  size_t nstage;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ stage[ASPRINTF_STAGE_ಗಾತ್ರ];
};

ರಚನೆ ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dollarstring(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಕೊನೆ)
{
  curl_off_t ಅಂಕೆ;
  ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !ಅಂಕೆ)
    ಹಿಂತಿರುಗಿ -1;
  *ಕೊನೆ = p;
  ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಅಂಕೆ - 1;
}

#ವ್ಯಾಖ್ಯಾನಿಸು is_arg_ಬಳಸಿದ(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#ವ್ಯಾಖ್ಯಾನಿಸು mark_arg_ಬಳಸಿದ(x, y) ((x)[(y) / 8] |= (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(1 << ((y) & 7)))

/*
 * Parse the format string.
 *
 * Create two arrays. One describes the inputs, one describes the outputs.
 *
 * Returns zero on success.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_OK          0
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_DOLLAR      1 /* bad dollar for main param */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_DOLLARWIDTH 2 /* bad dollar use for width */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_DOLLARPREC  3 /* bad dollar use for precision */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_MANYARGS    4 /* too many input arguments used */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_PREC        5 /* precision overflow */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_PRECMIX     6 /* bad mix of precision specifiers */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_ಅಗಲ       7 /* width overflow */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_INPUTGAP    8 /* gap in arguments */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_WIDTHARG    9 /* attempted to use same arg twice, for width */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_PRECARG    10 /* attempted to use same arg twice, for prec */
#ವ್ಯಾಖ್ಯಾನಿಸು PFMT_MANYSEGS   11 /* maxed out output segments */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_flags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **fmtp, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *flagsp, ಪೂರ್ಣಾಂಕ use_dollar,
                       ಪೂರ್ಣಾಂಕ *precp, ಪೂರ್ಣಾಂಕ *widthp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt = *fmtp;
  bool loopit = TRUE;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags = 0;
  ಪೂರ್ಣಾಂಕ ಅಗಲ = 0;
  ಪೂರ್ಣಾಂಕ precision = 0;

  /* Handle the flags */
  ಮಾಡು {
    ಆಯ್ಕೆ(*fmt++) {
    ಸ್ಥಿತಿ ' ':
      flags |= FLAGS_SPACE;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '+':
      flags |= FLAGS_SHOWSIGN;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '-':
      flags |= FLAGS_ಎಡ;
      flags &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)FLAGS_PAD_NIL;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '#':
      flags |= FLAGS_ALT;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '.':
      ಆದರೆ('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ಆದರೆ(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ಆದರೆ(precision < 0)
            /* illegal combo */
            ಹಿಂತಿರುಗಿ PFMT_DOLLARPREC;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          /* get it from the next argument */
          precision = -1;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        bool is_neg;
        curl_off_t ಅಂಕೆ;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ಆದರೆ(is_neg)
          fmt++;
        ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&fmt, &ಅಂಕೆ, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ))
          ಹಿಂತಿರುಗಿ PFMT_PREC;
        precision = (ಪೂರ್ಣಾಂಕ)ಅಂಕೆ;
        ಆದರೆ(is_neg)
          precision = -precision;
      }
      ಆದರೆ((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
ವಾದ*/
        ಹಿಂತಿರುಗಿ PFMT_PRECMIX;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 'h':
      flags |= FLAGS_SHORT;
      ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
    ಸ್ಥಿತಿ 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ಆದರೆ((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ SIZEOF_ಗಾತ್ರ_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#ಇಲ್ಲದಿದ್ದರೆ
        flags |= FLAGS_LONG;
#ಆದರೆ_ಅಂತ್ಯ
      }
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */
    ಸ್ಥಿತಿ 'l':
      ಆದರೆ(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      ಇಲ್ಲದಿದ್ದರೆ
        flags |= FLAGS_LONG;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 'L':
      flags |= FLAGS_LONGDOUBLE;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 'q':
      flags |= FLAGS_LONGLONG;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 'z':
      /* the code below generates a warning if -Wunreachable-code is
ಬಳಸಿದ*/
#ಆದರೆ SIZEOF_ಗಾತ್ರ_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ಇಲ್ಲದಿದ್ದರೆ
      flags |= FLAGS_LONG;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 'O':
#ಆದರೆ SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ಇಲ್ಲದಿದ್ದರೆ
      flags |= FLAGS_LONG;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '0':
      ಆದರೆ(!(flags & FLAGS_ಎಡ))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    ಸ್ಥಿತಿ '1':
    ಸ್ಥಿತಿ '2':
    ಸ್ಥಿತಿ '3':
    ಸ್ಥಿತಿ '4':
    ಸ್ಥಿತಿ '5':
    ಸ್ಥಿತಿ '6':
    ಸ್ಥಿತಿ '7':
    ಸ್ಥಿತಿ '8':
    ಸ್ಥಿತಿ '9': {
      curl_off_t ಅಂಕೆ;
      flags |= FLAGS_ಅಗಲ;
      fmt--;
      ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&fmt, &ಅಂಕೆ, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ))
        ಹಿಂತಿರುಗಿ PFMT_ಅಗಲ;
      ಅಗಲ = (ಪೂರ್ಣಾಂಕ)ಅಂಕೆ;
      ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ಆದರೆ(use_dollar == DOLLAR_USE) {
        ಅಗಲ = dollarstring(fmt, &fmt);
        ಆದರೆ(ಅಗಲ < 0)
          /* illegal combo */
          ಹಿಂತಿರುಗಿ PFMT_DOLLARWIDTH;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* pick from the next argument */
        ಅಗಲ = -1;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      loopit = FALSE;
      fmt--;
      ನಿಲ್ಲಿಸು;
    } /* switch */
  } ಆಗ(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = ಅಗಲ;
  *fmtp = fmt;
  ಹಿಂತಿರುಗಿ PFMT_OK;
}

ಸ್ಥಿರ bool parse_conversion(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ f, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *flagp,
                             FormatType *typep)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags = *flagp;
  FormatType type;
  ಆಯ್ಕೆ(f) {
  ಸ್ಥಿತಿ 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 's':
    type = MTYPE_STRING;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'n':
    type = MTYPE_INTPTR;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'p':
    type = MTYPE_PTR;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'd':
  ಸ್ಥಿತಿ 'i':
    ಆದರೆ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    ಇಲ್ಲದಿದ್ದರೆ
      type = MTYPE_INT;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'u':
    ಆದರೆ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ಇಲ್ಲದಿದ್ದರೆ
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'o':
    ಆದರೆ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ಇಲ್ಲದಿದ್ದರೆ
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'x':
    ಆದರೆ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ಇಲ್ಲದಿದ್ದರೆ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'X':
    ಆದರೆ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ಇಲ್ಲದಿದ್ದರೆ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_ಊರ್ಧ್ವ | FLAGS_UNSIGNED;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_ಊರ್ಧ್ವ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_ಊರ್ಧ್ವ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_ಊರ್ಧ್ವ;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* invalid instruction, disregard and continue */
    ಹಿಂತಿರುಗಿ TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parsefmt(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                    ರಚನೆ outsegment *ಹೊರಹೋಗು,
                    ರಚನೆ va_ಒಳಹರಿವು *ಒಳ,
                    ಪೂರ್ಣಾಂಕ *opieces,
                    ಪೂರ್ಣಾಂಕ *ipieces, va_list arglist)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt = format;
  ಪೂರ್ಣಾಂಕ param_ಅಂಕೆ = 0;
  ಪೂರ್ಣಾಂಕ max_param = -1;
  ಪೂರ್ಣಾಂಕ i;
  ಪೂರ್ಣಾಂಕ ocount = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  ರಚನೆ outsegment *optr;
  ಪೂರ್ಣಾಂಕ use_dollar = DOLLAR_UNKNOWN;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, ಗಾತ್ರ(usedinput));

  ಆಗ(*fmt) {
    ಆದರೆ(*fmt == '%') {
      ರಚನೆ va_ಒಳಹರಿವು *iptr;
      FormatType type;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags = 0;
      ಪೂರ್ಣಾಂಕ ಅಗಲ = 0;
      ಪೂರ್ಣಾಂಕ precision = 0;
      ಪೂರ್ಣಾಂಕ param = -1;
      ಪೂರ್ಣಾಂಕ rc;
      fmt++;
      outlen = (size_t)(fmt - ಆರಂಭ - 1);
      ಆದರೆ(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ಆದರೆ(outlen) {
          optr = &ಹೊರಹೋಗು[ocount++];
          ಆದರೆ(ocount > MAX_SEGMENTS)
            ಹಿಂತಿರುಗಿ PFMT_MANYSEGS;
          optr->ಒಳಹರಿವು = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->ಆರಂಭ = ಆರಂಭ;
          optr->outlen = outlen;
        }
        ಆರಂಭ = fmt;
        fmt++;
        ಮುಂದುವರಿ; /* while */
      }

      ಆದರೆ(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ಆದರೆ(param < 0) {
          ಆದರೆ(use_dollar == DOLLAR_USE)
            /* illegal combo */
            ಹಿಂತಿರುಗಿ PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &ಅಗಲ);
      ಆದರೆ(rc)
        ಹಿಂತಿರುಗಿ rc;

      ಆದರೆ(parse_conversion(*fmt, &flags, &type))
        ಮುಂದುವರಿ;

      ಆದರೆ(flags & FLAGS_WIDTHPARAM) {
        ಆದರೆ(ಅಗಲ < 0)
          ಅಗಲ = param_ಅಂಕೆ++;
        ಇಲ್ಲದಿದ್ದರೆ {
          /* if this identifies a parameter already used, this is illegal */
          ಆದರೆ(is_arg_ಬಳಸಿದ(usedinput, ಅಗಲ))
            ಹಿಂತಿರುಗಿ PFMT_WIDTHARG;
        }
        ಆದರೆ(ಅಗಲ >= MAX_PARAMETERS)
          ಹಿಂತಿರುಗಿ PFMT_MANYARGS;
        ಆದರೆ(ಅಗಲ >= max_param)
          max_param = ಅಗಲ;

        ಒಳ[ಅಗಲ].type = MTYPE_ಅಗಲ;
        /* mark as used */
        mark_arg_ಬಳಸಿದ(usedinput, ಅಗಲ);
      }

      ಆದರೆ(flags & FLAGS_PRECPARAM) {
        ಆದರೆ(precision < 0)
          precision = param_ಅಂಕೆ++;
        ಇಲ್ಲದಿದ್ದರೆ {
          /* if this identifies a parameter already used, this is illegal */
          ಆದರೆ(is_arg_ಬಳಸಿದ(usedinput, precision))
            ಹಿಂತಿರುಗಿ PFMT_PRECARG;
        }
        ಆದರೆ(precision >= MAX_PARAMETERS)
          ಹಿಂತಿರುಗಿ PFMT_MANYARGS;
        ಆದರೆ(precision >= max_param)
          max_param = precision;

        ಒಳ[precision].type = MTYPE_PRECISION;
        mark_arg_ಬಳಸಿದ(usedinput, precision);
      }

      /* Handle the specifier */
      ಆದರೆ(param < 0)
        param = param_ಅಂಕೆ++;
      ಆದರೆ(param >= MAX_PARAMETERS)
        ಹಿಂತಿರುಗಿ PFMT_MANYARGS;
      ಆದರೆ(param >= max_param)
        max_param = param;

      iptr = &ಒಳ[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_ಬಳಸಿದ(usedinput, param);

      fmt++;
      optr = &ಹೊರಹೋಗು[ocount++];
      ಆದರೆ(ocount > MAX_SEGMENTS)
        ಹಿಂತಿರುಗಿ PFMT_MANYSEGS;
      optr->ಒಳಹರಿವು = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)param;
      optr->flags = flags;
      optr->ಅಗಲ = ಅಗಲ;
      optr->precision = precision;
      optr->ಆರಂಭ = ಆರಂಭ;
      optr->outlen = outlen;
      ಆರಂಭ = fmt;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - ಆರಂಭ);
  ಆದರೆ(outlen) {
    optr = &ಹೊರಹೋಗು[ocount++];
    ಆದರೆ(ocount > MAX_SEGMENTS)
      ಹಿಂತಿರುಗಿ PFMT_MANYSEGS;
    optr->ಒಳಹರಿವು = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->ಆರಂಭ = ಆರಂಭ;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  ಗಾಗಿ(i = 0; i < max_param + 1; i++) {
    ರಚನೆ va_ಒಳಹರಿವು *iptr = &ಒಳ[i];
    ಆದರೆ(!is_arg_ಬಳಸಿದ(usedinput, i))
      /* bad input */
      ಹಿಂತಿರುಗಿ PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    ಆಯ್ಕೆ(iptr->type) {
    ಸ್ಥಿತಿ MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_INTPTR:
    ಸ್ಥಿತಿ MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, ಶೂನ್ಯ *);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, ದೀರ್ಘ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_INT:
    ಸ್ಥಿತಿ MTYPE_ಅಗಲ:
    ಸ್ಥಿತಿ MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, ಪೂರ್ಣಾಂಕ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, ದ್ವಿಗುಣ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_LONGDOUBLE:
      iptr->val.dnum = (ದ್ವಿಗುಣ)va_arg(arglist, ದೀರ್ಘ ದ್ವಿಗುಣ);
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      DEBUGASSERT(ಖಾಲಿ); /* unexpected */
      ನಿಲ್ಲಿಸು;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  ಹಿಂತಿರುಗಿ PFMT_OK;
}

ರಚನೆ mproperty {
  ಪೂರ್ಣಾಂಕ ಅಗಲ;            /* Width of a field. */
  ಪೂರ್ಣಾಂಕ prec;             /* Precision of a field. */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ size_t (*mp_streamn)(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len, ಶೂನ್ಯ *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*mp_flush)(ಶೂನ್ಯ *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
ಸ್ಥಿರ bool ಹರಿವು_ಸತತ(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಂಕ (*ಹರಿವು)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len, ಪೂರ್ಣಾಂಕ *donep)
{
  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (ಪೂರ್ಣಾಂಕ)n;
    ಹಿಂತಿರುಗಿ n != len;
  }
  ಆಗ(len--) {
    ಆದರೆ(ಹರಿವು(*buf++, userp))
      ಹಿಂತಿರುಗಿ TRUE;
    (*donep)++;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ pad_spaces[] = "                ";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ pad_zeros[] = "0000000000000000";
#ವ್ಯಾಖ್ಯಾನಿಸು PAD_ಸತತ 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
ಸ್ಥಿರ bool ಹರಿವು_pad(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಂಕ (*ಹರಿವು)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ pad, ಪೂರ್ಣಾಂಕ ಅಂಕೆ, ಪೂರ್ಣಾಂಕ *donep)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *src;
  ಆದರೆ(ಅಂಕೆ <= 0)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(pad == '0')
    src = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)pad_zeros;
  ಇಲ್ಲದಿದ್ದರೆ
    src = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)pad_spaces;
  ಆಗ(ಅಂಕೆ > PAD_ಸತತ) {
    ಆದರೆ(ಹರಿವು_ಸತತ(userp, ಹರಿವು, streamn, src, PAD_ಸತತ, donep))
      ಹಿಂತಿರುಗಿ TRUE;
    ಅಂಕೆ -= PAD_ಸತತ;
  }
  ಹಿಂತಿರುಗಿ ಹರಿವು_ಸತತ(userp, ಹರಿವು, streamn, src, (size_t)ಅಂಕೆ, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
ಸ್ಥಿರ bool ಹರಿವು_zrun(ಶೂನ್ಯ *userp,
                        ಪೂರ್ಣಾಂಕ (*ಹರಿವು)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, ಶೂನ್ಯ *),
                        mp_streamn streamn,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t len, ಪೂರ್ಣಾಂಕ *donep)
{
  size_t n = 0;
  ಆಗ(n < len && str[n])
    n++;
  ಹಿಂತಿರುಗಿ ಹರಿವು_ಸತತ(userp, ಹರಿವು, streamn,
                    (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)str, n, donep);
}

ಸ್ಥಿರ bool ಹೊರಹೋಗು_double(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಂಕ (*ಹರಿವು)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ರಚನೆ mproperty *p,
                       ದ್ವಿಗುಣ dnum,
                       ಅಕ್ಷರ *work, ಪೂರ್ಣಾಂಕ *donep)
{
  ಅಕ್ಷರ fmt[32] = "%";
  ಅಕ್ಷರ *fptr = &fmt[1];
  size_t ಎಡ = ಗಾತ್ರ(fmt) - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fmt);
  ಪೂರ್ಣಾಂಕ flags = p->flags;
  ಪೂರ್ಣಾಂಕ ಅಗಲ = p->ಅಗಲ;
  ಪೂರ್ಣಾಂಕ prec = p->prec;

  ಆದರೆ(flags & FLAGS_ಎಡ)
    *fptr++ = '-';
  ಆದರೆ(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ಆದರೆ(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ಆದರೆ(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ಆದರೆ(ಅಗಲ >= 0) {
    size_t dlen;
    ಆದರೆ(ಅಗಲ >= BUFFSIZE)
      ಅಗಲ = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, ಎಡ, "%d", ಅಗಲ);
    fptr += dlen;
    ಎಡ -= dlen;
  }
  ಆದರೆ(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    ಪೂರ್ಣಾಂಕ maxprec = BUFFSIZE - 1;
    ದ್ವಿಗುಣ val = dnum;
    ಪೂರ್ಣಾಂಕ len;
    ಆದರೆ(prec > maxprec)
      prec = maxprec - 1;
    ಆದರೆ(ಅಗಲ > 0 && prec <= ಅಗಲ)
      maxprec -= ಅಗಲ;
    ಆಗ(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ಆದರೆ(prec > maxprec)
      prec = maxprec - 1;
    ಆದರೆ(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, ಎಡ, ".%d", prec);
    fptr += len;
  }
  ಆದರೆ(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ಆದರೆ(flags & FLAGS_FLOATE)
    *fptr++ = (ಅಕ್ಷರ)((flags & FLAGS_ಊರ್ಧ್ವ) ? 'E' : 'e');
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_FLOATG)
    *fptr++ = (ಅಕ್ಷರ)((flags & FLAGS_ಊರ್ಧ್ವ) ? 'G' : 'g');
  ಇಲ್ಲದಿದ್ದರೆ
    *fptr++ = (flags & FLAGS_ಊರ್ಧ್ವ) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wformat-nonliteral"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#ಇಲ್ಲದಿದ್ದರೆ
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  DEBUGASSERT(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(work) < BUFFSIZE);
  ಹಿಂತಿರುಗಿ ಹರಿವು_ಸತತ(userp, ಹರಿವು, streamn,
                    (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)work, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(work), donep);
}

ಸ್ಥಿರ bool ಹೊರಹೋಗು_ಸಂಖ್ಯೆ(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಂಕ (*ಹರಿವು)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ರಚನೆ mproperty *p,
                       uint64_t ಅಂಕೆ,
                       int64_t nums,
                       ಅಕ್ಷರ *work, ಪೂರ್ಣಾಂಕ *donep)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digits = Curl_ldigits;
  ಪೂರ್ಣಾಂಕ flags = p->flags;
  ಪೂರ್ಣಾಂಕ ಅಗಲ = p->ಅಗಲ;
  ಪೂರ್ಣಾಂಕ prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  ಪೂರ್ಣಾಂಕ ತಳ = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  ಅಕ್ಷರ *workend = &work[BUFFSIZE - 2];
  ಅಕ್ಷರ *w;

  ಆದರೆ(flags & FLAGS_CHAR) {
    /* Character. */
    ಆದರೆ(!(flags & FLAGS_ಎಡ)) {
      ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, ' ', ಅಗಲ - 1, donep))
        ಹಿಂತಿರುಗಿ TRUE;
      ಅಗಲ = 1;
    }
    OUTCHAR((ಅಕ್ಷರ)ಅಂಕೆ);
    ಆದರೆ(flags & FLAGS_ಎಡ) {
      ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, ' ', ಅಗಲ - 1, donep))
        ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಆದರೆ(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    ತಳ = 8;

  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_ಊರ್ಧ್ವ) ? Curl_udigits : Curl_ldigits;
    ತಳ = 16;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  ಇಲ್ಲದಿದ್ದರೆ {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ಆದರೆ(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_ಅಂಕೆ; /* Used to convert negative in positive. */
      signed_ಅಂಕೆ = nums + (int64_t)1;
      signed_ಅಂಕೆ = -signed_ಅಂಕೆ;
      ಅಂಕೆ = (uint64_t)signed_ಅಂಕೆ;
      ಅಂಕೆ += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ಆದರೆ(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(ತಳ <= 16);
  ಆಯ್ಕೆ(ತಳ) {
  ಸ್ಥಿತಿ 10:
    ಆಗ(ಅಂಕೆ > 0) {
      *w-- = (ಅಕ್ಷರ)('0' + (ಅಂಕೆ % 10));
      ಅಂಕೆ /= 10;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆಗ(ಅಂಕೆ > 0) {
      *w-- = digits[ಅಂಕೆ % ತಳ];
      ಅಂಕೆ /= ತಳ;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಅಗಲ -= (ಪೂರ್ಣಾಂಕ)(workend - w);
  prec -= (ಪೂರ್ಣಾಂಕ)(workend - w);

  ಆದರೆ(is_alt && ತಳ == 8 && prec <= 0) {
    *w-- = '0';
    --ಅಗಲ;
  }

  ಆದರೆ(prec > 0) {
    ಅಗಲ -= prec;
    ಆಗ(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ಆದರೆ(is_alt && ತಳ == 16)
    ಅಗಲ -= 2;

  ಆದರೆ(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --ಅಗಲ;

  ಆದರೆ(!(flags & FLAGS_ಎಡ) && !(flags & FLAGS_PAD_NIL)) {
    ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, ' ', ಅಗಲ, donep))
      ಹಿಂತಿರುಗಿ TRUE;
    ಅಗಲ = 0;
  }

  ಆದರೆ(is_neg)
    OUTCHAR('-');
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ಆದರೆ(is_alt && ತಳ == 16) {
    OUTCHAR('0');
    ಆದರೆ(flags & FLAGS_ಊರ್ಧ್ವ)
      OUTCHAR('X');
    ಇಲ್ಲದಿದ್ದರೆ
      OUTCHAR('x');
  }

  ಆದರೆ(!(flags & FLAGS_ಎಡ) && (flags & FLAGS_PAD_NIL)) {
    ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, '0', ಅಗಲ, donep))
      ಹಿಂತಿರುಗಿ TRUE;
    ಅಗಲ = 0;
  }

  /* Write the number. */
  ಆದರೆ(ಹರಿವು_ಸತತ(userp, ಹರಿವು, streamn, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)w + 1,
                (size_t)(workend - w), donep))
    ಹಿಂತಿರುಗಿ TRUE;

  ಆದರೆ(flags & FLAGS_ಎಡ) {
    ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, ' ', ಅಗಲ, donep))
      ಹಿಂತಿರುಗಿ TRUE;
  }

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ nilstr[] = "(nil)";

ಸ್ಥಿರ bool ಹೊರಹೋಗು_string(ಶೂನ್ಯ *userp,
                       ಪೂರ್ಣಾಂಕ (*ಹರಿವು)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, ಶೂನ್ಯ *),
                       mp_streamn streamn,
                       ರಚನೆ mproperty *p,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str,
                       ಪೂರ್ಣಾಂಕ *donep)
{
  ಪೂರ್ಣಾಂಕ flags = p->flags;
  ಪೂರ್ಣಾಂಕ ಅಗಲ = p->ಅಗಲ;
  ಪೂರ್ಣಾಂಕ prec = p->prec;
  size_t len;

  ಆದರೆ(!str) {
    /* Write null string if there is space. */
    ಆದರೆ(prec == -1 || prec >= (ಪೂರ್ಣಾಂಕ)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)FLAGS_ALT;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      str = "";
      len = 0;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(prec != -1)
    len = (size_t)prec;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*str == '\0')
    len = 0;
  ಇಲ್ಲದಿದ್ದರೆ
    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);

  ಅಗಲ -= (len > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : (ಪೂರ್ಣಾಂಕ)len;

  ಆದರೆ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ಆದರೆ(!(flags & FLAGS_ಎಡ)) {
    ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, ' ', ಅಗಲ, donep))
      ಹಿಂತಿರುಗಿ TRUE;
    ಅಗಲ = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ಆದರೆ(prec == -1) {
    ಆದರೆ(ಹರಿವು_ಸತತ(userp, ಹರಿವು, streamn,
                  (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)str, len, donep))
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು_zrun(userp, ಹರಿವು, streamn, str, len, donep))
    ಹಿಂತಿರುಗಿ TRUE;
  ಆದರೆ(flags & FLAGS_ಎಡ) {
    ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, ' ', ಅಗಲ, donep))
      ಹಿಂತಿರುಗಿ TRUE;
  }

  ಆದರೆ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool ಹೊರಹೋಗು_ಸೂಚಕ(ಶೂನ್ಯ *userp,
                        ಪೂರ್ಣಾಂಕ (*ಹರಿವು)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, ಶೂನ್ಯ *),
                        mp_streamn streamn,
                        ರಚನೆ mproperty *p,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr,
                        ಅಕ್ಷರ *work,
                        ಪೂರ್ಣಾಂಕ *donep)
{
  /* Generic pointer. */
  ಆದರೆ(ptr) {
    size_t ಅಂಕೆ = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ಆದರೆ(ಹೊರಹೋಗು_ಸಂಖ್ಯೆ(userp, ಹರಿವು, streamn, p, ಅಂಕೆ, 0, work, donep))
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Write "(nil)" for a nil pointer. */
    ಪೂರ್ಣಾಂಕ ಅಗಲ = p->ಅಗಲ;
    ಪೂರ್ಣಾಂಕ flags = p->flags;

    ಅಗಲ -= (ಪೂರ್ಣಾಂಕ)CURL_CSTRLEN(nilstr);
    ಆದರೆ(flags & FLAGS_ಎಡ) {
      ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, ' ', ಅಗಲ, donep))
        ಹಿಂತಿರುಗಿ TRUE;
      ಅಗಲ = 0;
    }
    ಆದರೆ(ಹರಿವು_ಸತತ(userp, ಹರಿವು, streamn, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)nilstr,
                  ಗಾತ್ರ(nilstr) - 1, donep))
      ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ(!(flags & FLAGS_ಎಡ)) {
      ಆದರೆ(ಹರಿವು_pad(userp, ಹರಿವು, streamn, ' ', ಅಗಲ, donep))
        ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ formatf(ಶೂನ್ಯ *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   ಪೂರ್ಣಾಂಕ (*ಹರಿವು)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, ಶೂನ್ಯ *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  ಪೂರ್ಣಾಂಕ ಮುಗಿದ = 0;   /* number of characters written */
  ಪೂರ್ಣಾಂಕ i;
  ಪೂರ್ಣಾಂಕ ocount = 0; /* number of output segments */
  ಪೂರ್ಣಾಂಕ icount = 0; /* number of input arguments */

  ರಚನೆ outsegment output[MAX_SEGMENTS];
  ರಚನೆ va_ಒಳಹರಿವು ಒಳಹರಿವು[MAX_PARAMETERS];
  ಅಕ್ಷರ work[BUFFSIZE + 2];

  /* Parse the format string */
  ಆದರೆ(parsefmt(format, output, ಒಳಹರಿವು, &ocount, &icount, ap_save))
    ಹಿಂತಿರುಗಿ 0;

  ಗಾಗಿ(i = 0; i < ocount; i++) {
    ರಚನೆ outsegment *optr = &output[i];
    ರಚನೆ va_ಒಳಹರಿವು *iptr = &ಒಳಹರಿವು[optr->ಒಳಹರಿವು];
    ರಚನೆ mproperty p;
    size_t outlen = optr->outlen;

    ಆದರೆ(outlen) {
      ಆದರೆ(ಹರಿವು_zrun(userp, ಹರಿವು, streamn, optr->ಆರಂಭ, outlen, &ಮುಗಿದ))
        ಹಿಂತಿರುಗಿ ಮುಗಿದ;
      ಆದರೆ(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        ಮುಂದುವರಿ;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ಆದರೆ(p.flags & FLAGS_WIDTHPARAM) {
      p.ಅಗಲ = (ಪೂರ್ಣಾಂಕ)ಒಳಹರಿವು[optr->ಅಗಲ].val.nums;
      ಆದರೆ(p.ಅಗಲ < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ಆದರೆ(p.ಅಗಲ == INT_MIN)
          p.ಅಗಲ = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
        ಇಲ್ಲದಿದ್ದರೆ
          p.ಅಗಲ = -p.ಅಗಲ;
        p.flags |= FLAGS_ಎಡ;
        p.flags &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)FLAGS_PAD_NIL;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      p.ಅಗಲ = optr->ಅಗಲ;

    /* pick up the specified precision */
    ಆದರೆ(p.flags & FLAGS_PRECPARAM) {
      p.prec = (ಪೂರ್ಣಾಂಕ)ಒಳಹರಿವು[optr->precision].val.nums;
      ಆದರೆ(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    ಇಲ್ಲದಿದ್ದರೆ
      p.prec = -1;

    ಆಯ್ಕೆ(iptr->type) {
    ಸ್ಥಿತಿ MTYPE_INTU:
    ಸ್ಥಿತಿ MTYPE_LONGU:
    ಸ್ಥಿತಿ MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ಆದರೆ(ಹೊರಹೋಗು_ಸಂಖ್ಯೆ(userp, ಹರಿವು, streamn, &p, iptr->val.numu, 0, work,
                    &ಮುಗಿದ))
        ಹಿಂತಿರುಗಿ ಮುಗಿದ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_INT:
    ಸ್ಥಿತಿ MTYPE_LONG:
    ಸ್ಥಿತಿ MTYPE_LONGLONG:
      ಆದರೆ(ಹೊರಹೋಗು_ಸಂಖ್ಯೆ(userp, ಹರಿವು, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &ಮುಗಿದ))
        ಹಿಂತಿರುಗಿ ಮುಗಿದ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_STRING:
      ಆದರೆ(ಹೊರಹೋಗು_string(userp, ಹರಿವು, streamn, &p, iptr->val.str, &ಮುಗಿದ))
        ಹಿಂತಿರುಗಿ ಮುಗಿದ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_PTR:
      ಆದರೆ(ಹೊರಹೋಗು_ಸೂಚಕ(userp, ಹರಿವು, streamn, &p, iptr->val.ptr, work,
                     &ಮುಗಿದ))
        ಹಿಂತಿರುಗಿ ಮುಗಿದ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_DOUBLE:
    ಸ್ಥಿತಿ MTYPE_LONGDOUBLE:
      ಆದರೆ(ಹೊರಹೋಗು_double(userp, ಹರಿವು, streamn, &p, iptr->val.dnum, work,
                    &ಮುಗಿದ))
        ಹಿಂತಿರುಗಿ ಮುಗಿದ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ಆದರೆ(flush && flush(userp))
        ಹಿಂತಿರುಗಿ ಮುಗಿದ;
      /* Answer the count of characters written. */
      ಆದರೆ(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)ಮುಗಿದ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(p.flags & FLAGS_LONG)
        *(ದೀರ್ಘ *)iptr->val.ptr = (ದೀರ್ಘ)ಮುಗಿದ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!(p.flags & FLAGS_SHORT))
        *(ಪೂರ್ಣಾಂಕ *)iptr->val.ptr = ಮುಗಿದ;
      ಇಲ್ಲದಿದ್ದರೆ
        *(ಗಿಡ್ಡ *)iptr->val.ptr = (ಗಿಡ್ಡ)ಮುಗಿದ;
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ ಮುಗಿದ;
}

/* fputc() look-alike */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ addbyter(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ outc, ಶೂನ್ಯ *f)
{
  ರಚನೆ nsprintf *infop = f;
  ಆದರೆ(infop->ಉದ್ದಳತೆ < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->ಬಫರ್++ = (ಅಕ್ಷರ)outc; /* store */
    infop->ಉದ್ದಳತೆ++; /* we are now one byte larger */
    ಹಿಂತಿರುಗಿ 0;     /* fputc() returns like this on success */
  }
  ಹಿಂತಿರುಗಿ 1;
}

/* block variant of addbyter */
ಸ್ಥಿರ size_t addrun(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len, ಶೂನ್ಯ *f)
{
  ರಚನೆ nsprintf *infop = f;
  size_t fit = infop->max - infop->ಉದ್ದಳತೆ;
  ಆದರೆ(len > fit)
    len = fit;
  ಆದರೆ(len) {
    ಸ್ಮರಣೆ_ನಕಲು(infop->ಬಫರ್, buf, len);
    infop->ಬಫರ್ += len;
    infop->ಉದ್ದಳತೆ += len;
  }
  ಹಿಂತಿರುಗಿ len;
}

ಪೂರ್ಣಾಂಕ curl_mvsnprintf(ಅಕ್ಷರ *ಬಫರ್, size_t maxlength, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                    va_list ವಾದಗಳು)
{
  ಪೂರ್ಣಾಂಕ retcode;
  ರಚನೆ nsprintf info;

  info.ಬಫರ್ = ಬಫರ್;
  info.ಉದ್ದಳತೆ = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, ಖಾಲಿ, format, ವಾದಗಳು);
  ಆದರೆ(info.max) {
    /* we null-terminate this */
    ಆದರೆ(info.max == info.ಉದ್ದಳತೆ) {
      /* we are at maximum, scrap the last letter */
      info.ಬಫರ್[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    ಇಲ್ಲದಿದ್ದರೆ
      info.ಬಫರ್[0] = 0;
  }
  ಹಿಂತಿರುಗಿ retcode;
}

ಪೂರ್ಣಾಂಕ curl_msnprintf(ಅಕ್ಷರ *ಬಫರ್, size_t maxlength, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
  ಪೂರ್ಣಾಂಕ retcode;
  va_list ವಾದಗಳು; /* argument pointer */
  va_start(ವಾದಗಳು, format);
  retcode = curl_mvsnprintf(ಬಫರ್, maxlength, format, ವಾದಗಳು);
  va_end(ವಾದಗಳು);
  ಹಿಂತಿರುಗಿ retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ alloc_flush(ಶೂನ್ಯ *f)
{
  ರಚನೆ asprintf *infop = f;
  ಆದರೆ(infop->nstage) {
    CURLcode ಫಲಿತಾಂಶ = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      infop->merr = ಫಲಿತಾಂಶ == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      ಹಿಂತಿರುಗಿ 1; /* fail */
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ alloc_addn(ರಚನೆ asprintf *infop, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf,
                      size_t len)
{
  CURLcode ಫಲಿತಾಂಶ = curlx_dyn_addn(infop->b, buf, len);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    infop->merr = ಫಲಿತಾಂಶ == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    ಹಿಂತಿರುಗಿ 1; /* fail */
  }
  ಹಿಂತಿರುಗಿ 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
ಸ್ಥಿರ bool alloc_ಹೊಂದುತ್ತದೆ(ರಚನೆ asprintf *infop, size_t ಇನ್ನಷ್ಟು)
{
  ಹಿಂತಿರುಗಿ (infop->b->leng + infop->nstage + ಇನ್ನಷ್ಟು + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ alloc_addbyter(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ outc, ಶೂನ್ಯ *f)
{
  ರಚನೆ asprintf *infop = f;
  ಆದರೆ(infop->nstage == ಗಾತ್ರ(infop->stage) && alloc_flush(infop))
    ಹಿಂತಿರುಗಿ 1; /* fail */
  ಆದರೆ(!alloc_ಹೊಂದುತ್ತದೆ(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ಆದರೆ(alloc_flush(infop))
      ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  ಹಿಂತಿರುಗಿ 0;
}

/* block variant of alloc_addbyter */
ಸ್ಥಿರ size_t alloc_addrun(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len, ಶೂನ್ಯ *f)
{
  ರಚನೆ asprintf *infop = f;
  size_t accepted = 0;
  ಆಗ(accepted < len) {
    size_t ಸ್ಥಳ = ಗಾತ್ರ(infop->stage) - infop->nstage;
    size_t n;
    ಆದರೆ(!ಸ್ಥಳ) {
      ಆದರೆ(alloc_flush(infop))
        ಹಿಂತಿರುಗಿ accepted; /* accepted < len aborts formatting */
      ಸ್ಥಳ = ಗಾತ್ರ(infop->stage);
    }
    n = len - accepted;
    ಆದರೆ(n > ಸ್ಥಳ)
      n = ಸ್ಥಳ;
    ಆದರೆ(!alloc_ಹೊಂದುತ್ತದೆ(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t ಹೊಂದುತ್ತದೆ = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ಆದರೆ(ಹೊಂದುತ್ತದೆ) {
        ಸ್ಮರಣೆ_ನಕಲು(&infop->stage[infop->nstage], buf + accepted, ಹೊಂದುತ್ತದೆ);
        infop->nstage += ಹೊಂದುತ್ತದೆ;
        accepted += ಹೊಂದುತ್ತದೆ;
      }
      ಆದರೆ(alloc_flush(infop))
        ಹಿಂತಿರುಗಿ accepted;
      ಆದರೆ(alloc_addn(infop, buf + accepted, 1))
        ಹಿಂತಿರುಗಿ accepted; /* accepted < len aborts formatting */
      accepted++;
      ಮುಂದುವರಿ;
    }
    ಸ್ಮರಣೆ_ನಕಲು(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  ಹಿಂತಿರುಗಿ accepted;
}

/* appends the formatted string, returns MERR error code */
ಪೂರ್ಣಾಂಕ curlx_dyn_vprintf(ರಚನೆ dynbuf *dyn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು)
{
  ರಚನೆ asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (ಶೂನ್ಯ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, ವಾದಗಳು);
  ಆದರೆ(!info.merr)
    (ಶೂನ್ಯ)alloc_flush(&info);
  ಆದರೆ(info.merr) {
    curlx_dyn_free(info.b);
    ಹಿಂತಿರುಗಿ info.merr;
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ *curl_mvaprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು)
{
  ರಚನೆ asprintf info;
  ರಚನೆ dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (ಶೂನ್ಯ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, ವಾದಗಳು);
  ಆದರೆ(!info.merr)
    (ಶೂನ್ಯ)alloc_flush(&info);
  ಆದರೆ(info.merr) {
    curlx_dyn_free(info.b);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  ಆದರೆ(curlx_dyn_len(info.b))
    ಹಿಂತಿರುಗಿ curlx_dyn_ptr(info.b);
  ಹಿಂತಿರುಗಿ curlx_strdup("");
}

ಅಕ್ಷರ *curl_maprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
  va_list ವಾದಗಳು;
  ಅಕ್ಷರ *s;
  va_start(ವಾದಗಳು, format);
  s = curl_mvaprintf(format, ವಾದಗಳು);
  va_end(ವಾದಗಳು);
  ಹಿಂತಿರುಗಿ s;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ storebuffer(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ outc, ಶೂನ್ಯ *f)
{
  ಅಕ್ಷರ **ಬಫರ್ = f;
  **ಬಫರ್ = (ಅಕ್ಷರ)outc;
  (*ಬಫರ್)++;
  ಹಿಂತಿರುಗಿ 0;
}

/* block variant of storebuffer */
ಸ್ಥಿರ size_t storerun(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len, ಶೂನ್ಯ *f)
{
  ಅಕ್ಷರ **ಬಫರ್ = f;
  ಸ್ಮರಣೆ_ನಕಲು(*ಬಫರ್, buf, len);
  *ಬಫರ್ += len;
  ಹಿಂತಿರುಗಿ len;
}

ಪೂರ್ಣಾಂಕ curl_msprintf(ಅಕ್ಷರ *ಬಫರ್, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
  va_list ವಾದಗಳು; /* argument pointer */
  ಪೂರ್ಣಾಂಕ retcode;
  va_start(ವಾದಗಳು, format);
  retcode = formatf(&ಬಫರ್, storebuffer, storerun, ಖಾಲಿ, format, ವಾದಗಳು);
  va_end(ವಾದಗಳು);
  *ಬಫರ್ = 0; /* we null-terminate this */
  ಹಿಂತಿರುಗಿ retcode;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fputc_wrapper(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ outc, ಶೂನ್ಯ *f)
{
  ರಚನೆ ioprintf *i = f;
  ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು = outc;
  FILE *s = i->fd;
  ಪೂರ್ಣಾಂಕ rc = fputc(ಹೊರಹೋಗು, s);
  i->error = (rc == EOF);
  ಹಿಂತಿರುಗಿ (rc == EOF);
}

/* block variant of fputc_wrapper */
ಸ್ಥಿರ size_t fwrite_wrapper(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len, ಶೂನ್ಯ *f)
{
  ರಚನೆ ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ curl_mvfprintf(FILE *fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು)
{
  ರಚನೆ ioprintf info;
  ಪೂರ್ಣಾಂಕ n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, ಖಾಲಿ, format, ವಾದಗಳು);
  ಆದರೆ(info.error)
    ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ curl_mfprintf(FILE *fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
  va_list ವಾದಗಳು; /* argument pointer */
  ಪೂರ್ಣಾಂಕ n;
  va_start(ವಾದಗಳು, format);
  n = curl_mvfprintf(fd, format, ವಾದಗಳು);
  va_end(ವಾದಗಳು);
  ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ curl_mprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
  va_list ವಾದಗಳು; /* argument pointer */
  ಪೂರ್ಣಾಂಕ n;
  va_start(ವಾದಗಳು, format);
  n = curl_mvfprintf(stdout, format, ವಾದಗಳು);
  va_end(ವಾದಗಳು);
  ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ curl_mvsprintf(ಅಕ್ಷರ *ಬಫರ್, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು)
{
  ಪೂರ್ಣಾಂಕ retcode = formatf(&ಬಫರ್, storebuffer, storerun, ಖಾಲಿ, format, ವಾದಗಳು);
  *ಬಫರ್ = 0; /* we null-terminate this */
  ಹಿಂತಿರುಗಿ retcode;
}

ಪೂರ್ಣಾಂಕ curl_mvprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು)
{
  ಹಿಂತಿರುಗಿ curl_mvfprintf(stdout, format, ವಾದಗಳು);
}

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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_printf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_PARAMETERS 128 /* number of input arguments */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ Curl_udigits[] = "0123456789ABCDEF";

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OUTCHAR(x)                        \
  ਕਰੋ {                                    \
    ਜੇ(ਧਾਰਾ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(x), userp)) \
      ਵਾਪਸ TRUE;                        \
    (*donep)++;                           \
  } ਜਦੋਂ(0)

/* Data type to read from the arglist */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
  MTYPE_ਚੌੜਾਈ,
  MTYPE_PRECISION
} FormatType;

/* conversion and display flags */
ਗਿਣਤੀ {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_ਖੱਬਾ       = 1 << 2,
  FLAGS_ALT        = 1 << 3,
  FLAGS_SHORT      = 1 << 4,
  FLAGS_LONG       = 1 << 5,
  FLAGS_LONGLONG   = 1 << 6,
  FLAGS_LONGDOUBLE = 1 << 7,
  FLAGS_PAD_NIL    = 1 << 8,
  FLAGS_UNSIGNED   = 1 << 9,
  FLAGS_OCTAL      = 1 << 10,
  FLAGS_HEX        = 1 << 11,
  FLAGS_ਉੱਤਲਾ      = 1 << 12,
  FLAGS_ਚੌੜਾਈ      = 1 << 13, /* '*' or '*<num>$' used */
  FLAGS_WIDTHPARAM = 1 << 14, /* width PARAMETER was specified */
  FLAGS_PREC       = 1 << 15, /* precision was specified */
  FLAGS_PRECPARAM  = 1 << 16, /* precision PARAMETER was specified */
  FLAGS_CHAR       = 1 << 17, /* %c story */
  FLAGS_FLOATE     = 1 << 18, /* %e or %E */
  FLAGS_FLOATG     = 1 << 19, /* %g or %G */
  FLAGS_SUBSTR     = 1 << 20  /* no input, only substring */
};

ਗਿਣਤੀ {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
ਬਣਤਰ va_ਨਿਵੇਸ਼ {
  FormatType type; /* FormatType */
  ਸੰਘ {
    ਅਚਲ ਅੱਖਰ *str;
    ਖਾਲੀ *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    ਦੁੱਗਣਾ dnum;
  } val;
};

/*
 * Describes an output segment.
 */
ਬਣਤਰ outsegment {
  ਪੂਰਨ ਚੌੜਾਈ;     /* width OR width parameter number */
  ਪੂਰਨ precision; /* precision OR precision parameter number */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਿਵੇਸ਼; /* input argument array index */
  ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

ਬਣਤਰ nsprintf {
  ਅੱਖਰ *ਬਫਰ;
  size_t ਲੰਮਾਈ;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASPRINTF_STAGE_ਮਾਪ 512

ਬਣਤਰ asprintf {
  ਬਣਤਰ dynbuf *b;
  ਅੱਖਰ merr;
  size_t nstage;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ stage[ASPRINTF_STAGE_ਮਾਪ];
};

ਬਣਤਰ ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
ਸਥਿਰ ਪੂਰਨ dollarstring(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ **ਸਿਰਾ)
{
  curl_off_t ਅੰਕ;
  ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &ਅੰਕ, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !ਅੰਕ)
    ਵਾਪਸ -1;
  *ਸਿਰਾ = p;
  ਵਾਪਸ (ਪੂਰਨ)ਅੰਕ - 1;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_arg_ਵਰਤਿਆ(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mark_arg_ਵਰਤਿਆ(x, y) ((x)[(y) / 8] |= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(1 << ((y) & 7)))

/*
 * Parse the format string.
 *
 * Create two arrays. One describes the inputs, one describes the outputs.
 *
 * Returns zero on success.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_OK          0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_DOLLAR      1 /* bad dollar for main param */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_DOLLARWIDTH 2 /* bad dollar use for width */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_DOLLARPREC  3 /* bad dollar use for precision */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_MANYARGS    4 /* too many input arguments used */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_PREC        5 /* precision overflow */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_PRECMIX     6 /* bad mix of precision specifiers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_ਚੌੜਾਈ       7 /* width overflow */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_INPUTGAP    8 /* gap in arguments */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_WIDTHARG    9 /* attempted to use same arg twice, for width */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_PRECARG    10 /* attempted to use same arg twice, for prec */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFMT_MANYSEGS   11 /* maxed out output segments */

ਸਥਿਰ ਪੂਰਨ parse_flags(ਅਚਲ ਅੱਖਰ **fmtp, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *flagsp, ਪੂਰਨ use_dollar,
                       ਪੂਰਨ *precp, ਪੂਰਨ *widthp)
{
  ਅਚਲ ਅੱਖਰ *fmt = *fmtp;
  bool loopit = TRUE;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags = 0;
  ਪੂਰਨ ਚੌੜਾਈ = 0;
  ਪੂਰਨ precision = 0;

  /* Handle the flags */
  ਕਰੋ {
    ਚੋਣ(*fmt++) {
    ਹਾਲਤ ' ':
      flags |= FLAGS_SPACE;
      ਰੋਕੋ;
    ਹਾਲਤ '+':
      flags |= FLAGS_SHOWSIGN;
      ਰੋਕੋ;
    ਹਾਲਤ '-':
      flags |= FLAGS_ਖੱਬਾ;
      flags &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)FLAGS_PAD_NIL;
      ਰੋਕੋ;
    ਹਾਲਤ '#':
      flags |= FLAGS_ALT;
      ਰੋਕੋ;
    ਹਾਲਤ '.':
      ਜੇ('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ਜੇ(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ਜੇ(precision < 0)
            /* illegal combo */
            ਵਾਪਸ PFMT_DOLLARPREC;
        }
        ਨਹੀਂਤਾਂ
          /* get it from the next argument */
          precision = -1;
      }
      ਨਹੀਂਤਾਂ {
        bool is_neg;
        curl_off_t ਅੰਕ;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ਜੇ(is_neg)
          fmt++;
        ਜੇ(curlx_str_ਸੰਖਿਆ(&fmt, &ਅੰਕ, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ))
          ਵਾਪਸ PFMT_PREC;
        precision = (ਪੂਰਨ)ਅੰਕ;
        ਜੇ(is_neg)
          precision = -precision;
      }
      ਜੇ((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
ਦਲੀਲ*/
        ਵਾਪਸ PFMT_PRECMIX;
      ਰੋਕੋ;
    ਹਾਲਤ 'h':
      flags |= FLAGS_SHORT;
      ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
    ਹਾਲਤ 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ਜੇ((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      ਨਹੀਂਤਾਂ ਜੇ((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      ਨਹੀਂਤਾਂ {
#ਜੇ SIZEOF_ਮਾਪ_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#ਨਹੀਂਤਾਂ
        flags |= FLAGS_LONG;
#ਜੇ_ਅੰਤ
      }
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* _WIN32 */
    ਹਾਲਤ 'l':
      ਜੇ(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      ਨਹੀਂਤਾਂ
        flags |= FLAGS_LONG;
      ਰੋਕੋ;
    ਹਾਲਤ 'L':
      flags |= FLAGS_LONGDOUBLE;
      ਰੋਕੋ;
    ਹਾਲਤ 'q':
      flags |= FLAGS_LONGLONG;
      ਰੋਕੋ;
    ਹਾਲਤ 'z':
      /* the code below generates a warning if -Wunreachable-code is
ਵਰਤਿਆ*/
#ਜੇ SIZEOF_ਮਾਪ_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ਨਹੀਂਤਾਂ
      flags |= FLAGS_LONG;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;
    ਹਾਲਤ 'O':
#ਜੇ SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ਨਹੀਂਤਾਂ
      flags |= FLAGS_LONG;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;
    ਹਾਲਤ '0':
      ਜੇ(!(flags & FLAGS_ਖੱਬਾ))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    ਹਾਲਤ '1':
    ਹਾਲਤ '2':
    ਹਾਲਤ '3':
    ਹਾਲਤ '4':
    ਹਾਲਤ '5':
    ਹਾਲਤ '6':
    ਹਾਲਤ '7':
    ਹਾਲਤ '8':
    ਹਾਲਤ '9': {
      curl_off_t ਅੰਕ;
      flags |= FLAGS_ਚੌੜਾਈ;
      fmt--;
      ਜੇ(curlx_str_ਸੰਖਿਆ(&fmt, &ਅੰਕ, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ))
        ਵਾਪਸ PFMT_ਚੌੜਾਈ;
      ਚੌੜਾਈ = (ਪੂਰਨ)ਅੰਕ;
      ਰੋਕੋ;
    }
    ਹਾਲਤ '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ਜੇ(use_dollar == DOLLAR_USE) {
        ਚੌੜਾਈ = dollarstring(fmt, &fmt);
        ਜੇ(ਚੌੜਾਈ < 0)
          /* illegal combo */
          ਵਾਪਸ PFMT_DOLLARWIDTH;
      }
      ਨਹੀਂਤਾਂ
        /* pick from the next argument */
        ਚੌੜਾਈ = -1;
      ਰੋਕੋ;
    ਮੂਲ:
      loopit = FALSE;
      fmt--;
      ਰੋਕੋ;
    } /* switch */
  } ਜਦੋਂ(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = ਚੌੜਾਈ;
  *fmtp = fmt;
  ਵਾਪਸ PFMT_OK;
}

ਸਥਿਰ bool parse_conversion(ਅਚਲ ਅੱਖਰ f, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *flagp,
                             FormatType *typep)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags = *flagp;
  FormatType type;
  ਚੋਣ(f) {
  ਹਾਲਤ 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    ਰੋਕੋ;
  ਹਾਲਤ 's':
    type = MTYPE_STRING;
    ਰੋਕੋ;
  ਹਾਲਤ 'n':
    type = MTYPE_INTPTR;
    ਰੋਕੋ;
  ਹਾਲਤ 'p':
    type = MTYPE_PTR;
    ਰੋਕੋ;
  ਹਾਲਤ 'd':
  ਹਾਲਤ 'i':
    ਜੇ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    ਨਹੀਂਤਾਂ
      type = MTYPE_INT;
    ਰੋਕੋ;
  ਹਾਲਤ 'u':
    ਜੇ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ਨਹੀਂਤਾਂ
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    ਰੋਕੋ;
  ਹਾਲਤ 'o':
    ਜੇ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ਨਹੀਂਤਾਂ
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    ਰੋਕੋ;
  ਹਾਲਤ 'x':
    ਜੇ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ਨਹੀਂਤਾਂ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    ਰੋਕੋ;
  ਹਾਲਤ 'X':
    ਜੇ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ਨਹੀਂਤਾਂ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_ਉੱਤਲਾ | FLAGS_UNSIGNED;
    ਰੋਕੋ;
  ਹਾਲਤ 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    ਰੋਕੋ;
  ਹਾਲਤ 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    ਰੋਕੋ;
  ਹਾਲਤ 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_ਉੱਤਲਾ;
    ਰੋਕੋ;
  ਹਾਲਤ 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    ਰੋਕੋ;
  ਹਾਲਤ 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_ਉੱਤਲਾ;
    ਰੋਕੋ;
  ਹਾਲਤ 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    ਰੋਕੋ;
  ਹਾਲਤ 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_ਉੱਤਲਾ;
    ਰੋਕੋ;
  ਮੂਲ:
    /* invalid instruction, disregard and continue */
    ਵਾਪਸ TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਪੂਰਨ parsefmt(ਅਚਲ ਅੱਖਰ *format,
                    ਬਣਤਰ outsegment *ਬਾਹਰਲਾ,
                    ਬਣਤਰ va_ਨਿਵੇਸ਼ *ਵਿੱਚ,
                    ਪੂਰਨ *opieces,
                    ਪੂਰਨ *ipieces, va_list arglist)
{
  ਅਚਲ ਅੱਖਰ *fmt = format;
  ਪੂਰਨ param_ਅੰਕ = 0;
  ਪੂਰਨ max_param = -1;
  ਪੂਰਨ i;
  ਪੂਰਨ ocount = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  ਬਣਤਰ outsegment *optr;
  ਪੂਰਨ use_dollar = DOLLAR_UNKNOWN;
  ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, ਆਕਾਰ(usedinput));

  ਜਦੋਂ(*fmt) {
    ਜੇ(*fmt == '%') {
      ਬਣਤਰ va_ਨਿਵੇਸ਼ *iptr;
      FormatType type;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags = 0;
      ਪੂਰਨ ਚੌੜਾਈ = 0;
      ਪੂਰਨ precision = 0;
      ਪੂਰਨ param = -1;
      ਪੂਰਨ rc;
      fmt++;
      outlen = (size_t)(fmt - ਸ਼ੁਰੂ - 1);
      ਜੇ(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ਜੇ(outlen) {
          optr = &ਬਾਹਰਲਾ[ocount++];
          ਜੇ(ocount > MAX_SEGMENTS)
            ਵਾਪਸ PFMT_MANYSEGS;
          optr->ਨਿਵੇਸ਼ = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
          optr->outlen = outlen;
        }
        ਸ਼ੁਰੂ = fmt;
        fmt++;
        ਜਾਰੀ; /* while */
      }

      ਜੇ(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ਜੇ(param < 0) {
          ਜੇ(use_dollar == DOLLAR_USE)
            /* illegal combo */
            ਵਾਪਸ PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        ਨਹੀਂਤਾਂ
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &ਚੌੜਾਈ);
      ਜੇ(rc)
        ਵਾਪਸ rc;

      ਜੇ(parse_conversion(*fmt, &flags, &type))
        ਜਾਰੀ;

      ਜੇ(flags & FLAGS_WIDTHPARAM) {
        ਜੇ(ਚੌੜਾਈ < 0)
          ਚੌੜਾਈ = param_ਅੰਕ++;
        ਨਹੀਂਤਾਂ {
          /* if this identifies a parameter already used, this is illegal */
          ਜੇ(is_arg_ਵਰਤਿਆ(usedinput, ਚੌੜਾਈ))
            ਵਾਪਸ PFMT_WIDTHARG;
        }
        ਜੇ(ਚੌੜਾਈ >= MAX_PARAMETERS)
          ਵਾਪਸ PFMT_MANYARGS;
        ਜੇ(ਚੌੜਾਈ >= max_param)
          max_param = ਚੌੜਾਈ;

        ਵਿੱਚ[ਚੌੜਾਈ].type = MTYPE_ਚੌੜਾਈ;
        /* mark as used */
        mark_arg_ਵਰਤਿਆ(usedinput, ਚੌੜਾਈ);
      }

      ਜੇ(flags & FLAGS_PRECPARAM) {
        ਜੇ(precision < 0)
          precision = param_ਅੰਕ++;
        ਨਹੀਂਤਾਂ {
          /* if this identifies a parameter already used, this is illegal */
          ਜੇ(is_arg_ਵਰਤਿਆ(usedinput, precision))
            ਵਾਪਸ PFMT_PRECARG;
        }
        ਜੇ(precision >= MAX_PARAMETERS)
          ਵਾਪਸ PFMT_MANYARGS;
        ਜੇ(precision >= max_param)
          max_param = precision;

        ਵਿੱਚ[precision].type = MTYPE_PRECISION;
        mark_arg_ਵਰਤਿਆ(usedinput, precision);
      }

      /* Handle the specifier */
      ਜੇ(param < 0)
        param = param_ਅੰਕ++;
      ਜੇ(param >= MAX_PARAMETERS)
        ਵਾਪਸ PFMT_MANYARGS;
      ਜੇ(param >= max_param)
        max_param = param;

      iptr = &ਵਿੱਚ[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_ਵਰਤਿਆ(usedinput, param);

      fmt++;
      optr = &ਬਾਹਰਲਾ[ocount++];
      ਜੇ(ocount > MAX_SEGMENTS)
        ਵਾਪਸ PFMT_MANYSEGS;
      optr->ਨਿਵੇਸ਼ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)param;
      optr->flags = flags;
      optr->ਚੌੜਾਈ = ਚੌੜਾਈ;
      optr->precision = precision;
      optr->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
      optr->outlen = outlen;
      ਸ਼ੁਰੂ = fmt;
    }
    ਨਹੀਂਤਾਂ
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - ਸ਼ੁਰੂ);
  ਜੇ(outlen) {
    optr = &ਬਾਹਰਲਾ[ocount++];
    ਜੇ(ocount > MAX_SEGMENTS)
      ਵਾਪਸ PFMT_MANYSEGS;
    optr->ਨਿਵੇਸ਼ = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  ਲਈ(i = 0; i < max_param + 1; i++) {
    ਬਣਤਰ va_ਨਿਵੇਸ਼ *iptr = &ਵਿੱਚ[i];
    ਜੇ(!is_arg_ਵਰਤਿਆ(usedinput, i))
      /* bad input */
      ਵਾਪਸ PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    ਚੋਣ(iptr->type) {
    ਹਾਲਤ MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ਅਚਲ ਅੱਖਰ *);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_INTPTR:
    ਹਾਲਤ MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, ਖਾਲੀ *);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, ਲੰਮਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_INT:
    ਹਾਲਤ MTYPE_ਚੌੜਾਈ:
    ਹਾਲਤ MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, ਪੂਰਨ);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, ਦੁੱਗਣਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_LONGDOUBLE:
      iptr->val.dnum = (ਦੁੱਗਣਾ)va_arg(arglist, ਲੰਮਾ ਦੁੱਗਣਾ);
      ਰੋਕੋ;

    ਮੂਲ:
      DEBUGASSERT(ਨਿਰਾ); /* unexpected */
      ਰੋਕੋ;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  ਵਾਪਸ PFMT_OK;
}

ਬਣਤਰ mproperty {
  ਪੂਰਨ ਚੌੜਾਈ;            /* Width of a field. */
  ਪੂਰਨ prec;             /* Precision of a field. */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ size_t (*mp_streamn)(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t len, ਖਾਲੀ *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*mp_flush)(ਖਾਲੀ *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
ਸਥਿਰ bool ਧਾਰਾ_ਲੜੀਵਾਰ(ਖਾਲੀ *userp,
                       ਪੂਰਨ (*ਧਾਰਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, ਖਾਲੀ *),
                       mp_streamn streamn,
                       ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t len, ਪੂਰਨ *donep)
{
  ਜੇ(!len)
    ਵਾਪਸ FALSE;
  ਜੇ(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (ਪੂਰਨ)n;
    ਵਾਪਸ n != len;
  }
  ਜਦੋਂ(len--) {
    ਜੇ(ਧਾਰਾ(*buf++, userp))
      ਵਾਪਸ TRUE;
    (*donep)++;
  }
  ਵਾਪਸ FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ pad_spaces[] = "                ";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ pad_zeros[] = "0000000000000000";
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PAD_ਲੜੀਵਾਰ 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
ਸਥਿਰ bool ਧਾਰਾ_pad(ਖਾਲੀ *userp,
                       ਪੂਰਨ (*ਧਾਰਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, ਖਾਲੀ *),
                       mp_streamn streamn,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ pad, ਪੂਰਨ ਅੰਕ, ਪੂਰਨ *donep)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *src;
  ਜੇ(ਅੰਕ <= 0)
    ਵਾਪਸ FALSE;
  ਜੇ(pad == '0')
    src = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)pad_zeros;
  ਨਹੀਂਤਾਂ
    src = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)pad_spaces;
  ਜਦੋਂ(ਅੰਕ > PAD_ਲੜੀਵਾਰ) {
    ਜੇ(ਧਾਰਾ_ਲੜੀਵਾਰ(userp, ਧਾਰਾ, streamn, src, PAD_ਲੜੀਵਾਰ, donep))
      ਵਾਪਸ TRUE;
    ਅੰਕ -= PAD_ਲੜੀਵਾਰ;
  }
  ਵਾਪਸ ਧਾਰਾ_ਲੜੀਵਾਰ(userp, ਧਾਰਾ, streamn, src, (size_t)ਅੰਕ, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
ਸਥਿਰ bool ਧਾਰਾ_zrun(ਖਾਲੀ *userp,
                        ਪੂਰਨ (*ਧਾਰਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, ਖਾਲੀ *),
                        mp_streamn streamn,
                        ਅਚਲ ਅੱਖਰ *str, size_t len, ਪੂਰਨ *donep)
{
  size_t n = 0;
  ਜਦੋਂ(n < len && str[n])
    n++;
  ਵਾਪਸ ਧਾਰਾ_ਲੜੀਵਾਰ(userp, ਧਾਰਾ, streamn,
                    (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)str, n, donep);
}

ਸਥਿਰ bool ਬਾਹਰਲਾ_double(ਖਾਲੀ *userp,
                       ਪੂਰਨ (*ਧਾਰਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, ਖਾਲੀ *),
                       mp_streamn streamn,
                       ਬਣਤਰ mproperty *p,
                       ਦੁੱਗਣਾ dnum,
                       ਅੱਖਰ *work, ਪੂਰਨ *donep)
{
  ਅੱਖਰ fmt[32] = "%";
  ਅੱਖਰ *fptr = &fmt[1];
  size_t ਖੱਬਾ = ਆਕਾਰ(fmt) - ਲੜੀ_ਲੰਬਾਈ(fmt);
  ਪੂਰਨ flags = p->flags;
  ਪੂਰਨ ਚੌੜਾਈ = p->ਚੌੜਾਈ;
  ਪੂਰਨ prec = p->prec;

  ਜੇ(flags & FLAGS_ਖੱਬਾ)
    *fptr++ = '-';
  ਜੇ(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ਜੇ(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ਜੇ(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ਜੇ(ਚੌੜਾਈ >= 0) {
    size_t dlen;
    ਜੇ(ਚੌੜਾਈ >= BUFFSIZE)
      ਚੌੜਾਈ = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, ਖੱਬਾ, "%d", ਚੌੜਾਈ);
    fptr += dlen;
    ਖੱਬਾ -= dlen;
  }
  ਜੇ(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    ਪੂਰਨ maxprec = BUFFSIZE - 1;
    ਦੁੱਗਣਾ val = dnum;
    ਪੂਰਨ len;
    ਜੇ(prec > maxprec)
      prec = maxprec - 1;
    ਜੇ(ਚੌੜਾਈ > 0 && prec <= ਚੌੜਾਈ)
      maxprec -= ਚੌੜਾਈ;
    ਜਦੋਂ(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ਜੇ(prec > maxprec)
      prec = maxprec - 1;
    ਜੇ(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, ਖੱਬਾ, ".%d", prec);
    fptr += len;
  }
  ਜੇ(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ਜੇ(flags & FLAGS_FLOATE)
    *fptr++ = (ਅੱਖਰ)((flags & FLAGS_ਉੱਤਲਾ) ? 'E' : 'e');
  ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_FLOATG)
    *fptr++ = (ਅੱਖਰ)((flags & FLAGS_ਉੱਤਲਾ) ? 'G' : 'g');
  ਨਹੀਂਤਾਂ
    *fptr++ = (flags & FLAGS_ਉੱਤਲਾ) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wformat-nonliteral"
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#ਨਹੀਂਤਾਂ
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  DEBUGASSERT(ਲੜੀ_ਲੰਬਾਈ(work) < BUFFSIZE);
  ਵਾਪਸ ਧਾਰਾ_ਲੜੀਵਾਰ(userp, ਧਾਰਾ, streamn,
                    (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)work, ਲੜੀ_ਲੰਬਾਈ(work), donep);
}

ਸਥਿਰ bool ਬਾਹਰਲਾ_ਸੰਖਿਆ(ਖਾਲੀ *userp,
                       ਪੂਰਨ (*ਧਾਰਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, ਖਾਲੀ *),
                       mp_streamn streamn,
                       ਬਣਤਰ mproperty *p,
                       uint64_t ਅੰਕ,
                       int64_t nums,
                       ਅੱਖਰ *work, ਪੂਰਨ *donep)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digits = Curl_ldigits;
  ਪੂਰਨ flags = p->flags;
  ਪੂਰਨ ਚੌੜਾਈ = p->ਚੌੜਾਈ;
  ਪੂਰਨ prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  ਪੂਰਨ ਅਧਾਰ = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  ਅੱਖਰ *workend = &work[BUFFSIZE - 2];
  ਅੱਖਰ *w;

  ਜੇ(flags & FLAGS_CHAR) {
    /* Character. */
    ਜੇ(!(flags & FLAGS_ਖੱਬਾ)) {
      ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, ' ', ਚੌੜਾਈ - 1, donep))
        ਵਾਪਸ TRUE;
      ਚੌੜਾਈ = 1;
    }
    OUTCHAR((ਅੱਖਰ)ਅੰਕ);
    ਜੇ(flags & FLAGS_ਖੱਬਾ) {
      ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, ' ', ਚੌੜਾਈ - 1, donep))
        ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
  }
  ਜੇ(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    ਅਧਾਰ = 8;

  ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_ਉੱਤਲਾ) ? Curl_udigits : Curl_ldigits;
    ਅਧਾਰ = 16;
  }
  ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  ਨਹੀਂਤਾਂ {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ਜੇ(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_ਅੰਕ; /* Used to convert negative in positive. */
      signed_ਅੰਕ = nums + (int64_t)1;
      signed_ਅੰਕ = -signed_ਅੰਕ;
      ਅੰਕ = (uint64_t)signed_ਅੰਕ;
      ਅੰਕ += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ਜੇ(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(ਅਧਾਰ <= 16);
  ਚੋਣ(ਅਧਾਰ) {
  ਹਾਲਤ 10:
    ਜਦੋਂ(ਅੰਕ > 0) {
      *w-- = (ਅੱਖਰ)('0' + (ਅੰਕ % 10));
      ਅੰਕ /= 10;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਜਦੋਂ(ਅੰਕ > 0) {
      *w-- = digits[ਅੰਕ % ਅਧਾਰ];
      ਅੰਕ /= ਅਧਾਰ;
    }
    ਰੋਕੋ;
  }
  ਚੌੜਾਈ -= (ਪੂਰਨ)(workend - w);
  prec -= (ਪੂਰਨ)(workend - w);

  ਜੇ(is_alt && ਅਧਾਰ == 8 && prec <= 0) {
    *w-- = '0';
    --ਚੌੜਾਈ;
  }

  ਜੇ(prec > 0) {
    ਚੌੜਾਈ -= prec;
    ਜਦੋਂ(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ਜੇ(is_alt && ਅਧਾਰ == 16)
    ਚੌੜਾਈ -= 2;

  ਜੇ(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --ਚੌੜਾਈ;

  ਜੇ(!(flags & FLAGS_ਖੱਬਾ) && !(flags & FLAGS_PAD_NIL)) {
    ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, ' ', ਚੌੜਾਈ, donep))
      ਵਾਪਸ TRUE;
    ਚੌੜਾਈ = 0;
  }

  ਜੇ(is_neg)
    OUTCHAR('-');
  ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  ਨਹੀਂਤਾਂ ਜੇ(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ਜੇ(is_alt && ਅਧਾਰ == 16) {
    OUTCHAR('0');
    ਜੇ(flags & FLAGS_ਉੱਤਲਾ)
      OUTCHAR('X');
    ਨਹੀਂਤਾਂ
      OUTCHAR('x');
  }

  ਜੇ(!(flags & FLAGS_ਖੱਬਾ) && (flags & FLAGS_PAD_NIL)) {
    ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, '0', ਚੌੜਾਈ, donep))
      ਵਾਪਸ TRUE;
    ਚੌੜਾਈ = 0;
  }

  /* Write the number. */
  ਜੇ(ਧਾਰਾ_ਲੜੀਵਾਰ(userp, ਧਾਰਾ, streamn, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)w + 1,
                (size_t)(workend - w), donep))
    ਵਾਪਸ TRUE;

  ਜੇ(flags & FLAGS_ਖੱਬਾ) {
    ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, ' ', ਚੌੜਾਈ, donep))
      ਵਾਪਸ TRUE;
  }

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ nilstr[] = "(nil)";

ਸਥਿਰ bool ਬਾਹਰਲਾ_string(ਖਾਲੀ *userp,
                       ਪੂਰਨ (*ਧਾਰਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, ਖਾਲੀ *),
                       mp_streamn streamn,
                       ਬਣਤਰ mproperty *p,
                       ਅਚਲ ਅੱਖਰ *str,
                       ਪੂਰਨ *donep)
{
  ਪੂਰਨ flags = p->flags;
  ਪੂਰਨ ਚੌੜਾਈ = p->ਚੌੜਾਈ;
  ਪੂਰਨ prec = p->prec;
  size_t len;

  ਜੇ(!str) {
    /* Write null string if there is space. */
    ਜੇ(prec == -1 || prec >= (ਪੂਰਨ)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)FLAGS_ALT;
    }
    ਨਹੀਂਤਾਂ {
      str = "";
      len = 0;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(prec != -1)
    len = (size_t)prec;
  ਨਹੀਂਤਾਂ ਜੇ(*str == '\0')
    len = 0;
  ਨਹੀਂਤਾਂ
    len = ਲੜੀ_ਲੰਬਾਈ(str);

  ਚੌੜਾਈ -= (len > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : (ਪੂਰਨ)len;

  ਜੇ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ਜੇ(!(flags & FLAGS_ਖੱਬਾ)) {
    ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, ' ', ਚੌੜਾਈ, donep))
      ਵਾਪਸ TRUE;
    ਚੌੜਾਈ = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ਜੇ(prec == -1) {
    ਜੇ(ਧਾਰਾ_ਲੜੀਵਾਰ(userp, ਧਾਰਾ, streamn,
                  (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)str, len, donep))
      ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ_zrun(userp, ਧਾਰਾ, streamn, str, len, donep))
    ਵਾਪਸ TRUE;
  ਜੇ(flags & FLAGS_ਖੱਬਾ) {
    ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, ' ', ਚੌੜਾਈ, donep))
      ਵਾਪਸ TRUE;
  }

  ਜੇ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool ਬਾਹਰਲਾ_ਸੂਚਕ(ਖਾਲੀ *userp,
                        ਪੂਰਨ (*ਧਾਰਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, ਖਾਲੀ *),
                        mp_streamn streamn,
                        ਬਣਤਰ mproperty *p,
                        ਅਚਲ ਅੱਖਰ *ptr,
                        ਅੱਖਰ *work,
                        ਪੂਰਨ *donep)
{
  /* Generic pointer. */
  ਜੇ(ptr) {
    size_t ਅੰਕ = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ਜੇ(ਬਾਹਰਲਾ_ਸੰਖਿਆ(userp, ਧਾਰਾ, streamn, p, ਅੰਕ, 0, work, donep))
      ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ {
    /* Write "(nil)" for a nil pointer. */
    ਪੂਰਨ ਚੌੜਾਈ = p->ਚੌੜਾਈ;
    ਪੂਰਨ flags = p->flags;

    ਚੌੜਾਈ -= (ਪੂਰਨ)CURL_CSTRLEN(nilstr);
    ਜੇ(flags & FLAGS_ਖੱਬਾ) {
      ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, ' ', ਚੌੜਾਈ, donep))
        ਵਾਪਸ TRUE;
      ਚੌੜਾਈ = 0;
    }
    ਜੇ(ਧਾਰਾ_ਲੜੀਵਾਰ(userp, ਧਾਰਾ, streamn, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)nilstr,
                  ਆਕਾਰ(nilstr) - 1, donep))
      ਵਾਪਸ TRUE;
    ਜੇ(!(flags & FLAGS_ਖੱਬਾ)) {
      ਜੇ(ਧਾਰਾ_pad(userp, ਧਾਰਾ, streamn, ' ', ਚੌੜਾਈ, donep))
        ਵਾਪਸ TRUE;
    }
  }
  ਵਾਪਸ FALSE;
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
ਸਥਿਰ ਪੂਰਨ formatf(ਖਾਲੀ *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   ਪੂਰਨ (*ਧਾਰਾ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, ਖਾਲੀ *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ਅਚਲ ਅੱਖਰ *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  ਪੂਰਨ ਪੂਰਾ = 0;   /* number of characters written */
  ਪੂਰਨ i;
  ਪੂਰਨ ocount = 0; /* number of output segments */
  ਪੂਰਨ icount = 0; /* number of input arguments */

  ਬਣਤਰ outsegment output[MAX_SEGMENTS];
  ਬਣਤਰ va_ਨਿਵੇਸ਼ ਨਿਵੇਸ਼[MAX_PARAMETERS];
  ਅੱਖਰ work[BUFFSIZE + 2];

  /* Parse the format string */
  ਜੇ(parsefmt(format, output, ਨਿਵੇਸ਼, &ocount, &icount, ap_save))
    ਵਾਪਸ 0;

  ਲਈ(i = 0; i < ocount; i++) {
    ਬਣਤਰ outsegment *optr = &output[i];
    ਬਣਤਰ va_ਨਿਵੇਸ਼ *iptr = &ਨਿਵੇਸ਼[optr->ਨਿਵੇਸ਼];
    ਬਣਤਰ mproperty p;
    size_t outlen = optr->outlen;

    ਜੇ(outlen) {
      ਜੇ(ਧਾਰਾ_zrun(userp, ਧਾਰਾ, streamn, optr->ਸ਼ੁਰੂ, outlen, &ਪੂਰਾ))
        ਵਾਪਸ ਪੂਰਾ;
      ਜੇ(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        ਜਾਰੀ;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ਜੇ(p.flags & FLAGS_WIDTHPARAM) {
      p.ਚੌੜਾਈ = (ਪੂਰਨ)ਨਿਵੇਸ਼[optr->ਚੌੜਾਈ].val.nums;
      ਜੇ(p.ਚੌੜਾਈ < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ਜੇ(p.ਚੌੜਾਈ == INT_MIN)
          p.ਚੌੜਾਈ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
        ਨਹੀਂਤਾਂ
          p.ਚੌੜਾਈ = -p.ਚੌੜਾਈ;
        p.flags |= FLAGS_ਖੱਬਾ;
        p.flags &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)FLAGS_PAD_NIL;
      }
    }
    ਨਹੀਂਤਾਂ
      p.ਚੌੜਾਈ = optr->ਚੌੜਾਈ;

    /* pick up the specified precision */
    ਜੇ(p.flags & FLAGS_PRECPARAM) {
      p.prec = (ਪੂਰਨ)ਨਿਵੇਸ਼[optr->precision].val.nums;
      ਜੇ(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    ਨਹੀਂਤਾਂ ਜੇ(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    ਨਹੀਂਤਾਂ
      p.prec = -1;

    ਚੋਣ(iptr->type) {
    ਹਾਲਤ MTYPE_INTU:
    ਹਾਲਤ MTYPE_LONGU:
    ਹਾਲਤ MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ਜੇ(ਬਾਹਰਲਾ_ਸੰਖਿਆ(userp, ਧਾਰਾ, streamn, &p, iptr->val.numu, 0, work,
                    &ਪੂਰਾ))
        ਵਾਪਸ ਪੂਰਾ;
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_INT:
    ਹਾਲਤ MTYPE_LONG:
    ਹਾਲਤ MTYPE_LONGLONG:
      ਜੇ(ਬਾਹਰਲਾ_ਸੰਖਿਆ(userp, ਧਾਰਾ, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &ਪੂਰਾ))
        ਵਾਪਸ ਪੂਰਾ;
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_STRING:
      ਜੇ(ਬਾਹਰਲਾ_string(userp, ਧਾਰਾ, streamn, &p, iptr->val.str, &ਪੂਰਾ))
        ਵਾਪਸ ਪੂਰਾ;
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_PTR:
      ਜੇ(ਬਾਹਰਲਾ_ਸੂਚਕ(userp, ਧਾਰਾ, streamn, &p, iptr->val.ptr, work,
                     &ਪੂਰਾ))
        ਵਾਪਸ ਪੂਰਾ;
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_DOUBLE:
    ਹਾਲਤ MTYPE_LONGDOUBLE:
      ਜੇ(ਬਾਹਰਲਾ_double(userp, ਧਾਰਾ, streamn, &p, iptr->val.dnum, work,
                    &ਪੂਰਾ))
        ਵਾਪਸ ਪੂਰਾ;
      ਰੋਕੋ;

    ਹਾਲਤ MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ਜੇ(flush && flush(userp))
        ਵਾਪਸ ਪੂਰਾ;
      /* Answer the count of characters written. */
      ਜੇ(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)ਪੂਰਾ;
      ਨਹੀਂਤਾਂ ਜੇ(p.flags & FLAGS_LONG)
        *(ਲੰਮਾ *)iptr->val.ptr = (ਲੰਮਾ)ਪੂਰਾ;
      ਨਹੀਂਤਾਂ ਜੇ(!(p.flags & FLAGS_SHORT))
        *(ਪੂਰਨ *)iptr->val.ptr = ਪੂਰਾ;
      ਨਹੀਂਤਾਂ
        *(ਛੋਟਾ *)iptr->val.ptr = (ਛੋਟਾ)ਪੂਰਾ;
      ਰੋਕੋ;

    ਮੂਲ:
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ ਪੂਰਾ;
}

/* fputc() look-alike */
ਸਥਿਰ ਪੂਰਨ addbyter(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ outc, ਖਾਲੀ *f)
{
  ਬਣਤਰ nsprintf *infop = f;
  ਜੇ(infop->ਲੰਮਾਈ < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->ਬਫਰ++ = (ਅੱਖਰ)outc; /* store */
    infop->ਲੰਮਾਈ++; /* we are now one byte larger */
    ਵਾਪਸ 0;     /* fputc() returns like this on success */
  }
  ਵਾਪਸ 1;
}

/* block variant of addbyter */
ਸਥਿਰ size_t addrun(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t len, ਖਾਲੀ *f)
{
  ਬਣਤਰ nsprintf *infop = f;
  size_t fit = infop->max - infop->ਲੰਮਾਈ;
  ਜੇ(len > fit)
    len = fit;
  ਜੇ(len) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(infop->ਬਫਰ, buf, len);
    infop->ਬਫਰ += len;
    infop->ਲੰਮਾਈ += len;
  }
  ਵਾਪਸ len;
}

ਪੂਰਨ curl_mvsnprintf(ਅੱਖਰ *ਬਫਰ, size_t maxlength, ਅਚਲ ਅੱਖਰ *format,
                    va_list ਦਲੀਲਾਂ)
{
  ਪੂਰਨ retcode;
  ਬਣਤਰ nsprintf info;

  info.ਬਫਰ = ਬਫਰ;
  info.ਲੰਮਾਈ = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, ਨਿਰਾ, format, ਦਲੀਲਾਂ);
  ਜੇ(info.max) {
    /* we null-terminate this */
    ਜੇ(info.max == info.ਲੰਮਾਈ) {
      /* we are at maximum, scrap the last letter */
      info.ਬਫਰ[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    ਨਹੀਂਤਾਂ
      info.ਬਫਰ[0] = 0;
  }
  ਵਾਪਸ retcode;
}

ਪੂਰਨ curl_msnprintf(ਅੱਖਰ *ਬਫਰ, size_t maxlength, ਅਚਲ ਅੱਖਰ *format, ...)
{
  ਪੂਰਨ retcode;
  va_list ਦਲੀਲਾਂ; /* argument pointer */
  va_start(ਦਲੀਲਾਂ, format);
  retcode = curl_mvsnprintf(ਬਫਰ, maxlength, format, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);
  ਵਾਪਸ retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
ਸਥਿਰ ਪੂਰਨ alloc_flush(ਖਾਲੀ *f)
{
  ਬਣਤਰ asprintf *infop = f;
  ਜੇ(infop->nstage) {
    CURLcode ਨਤੀਜਾ = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ਜੇ(ਨਤੀਜਾ) {
      infop->merr = ਨਤੀਜਾ == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      ਵਾਪਸ 1; /* fail */
    }
  }
  ਵਾਪਸ 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
ਸਥਿਰ ਪੂਰਨ alloc_addn(ਬਣਤਰ asprintf *infop, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf,
                      size_t len)
{
  CURLcode ਨਤੀਜਾ = curlx_dyn_addn(infop->b, buf, len);
  ਜੇ(ਨਤੀਜਾ) {
    infop->merr = ਨਤੀਜਾ == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    ਵਾਪਸ 1; /* fail */
  }
  ਵਾਪਸ 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
ਸਥਿਰ bool alloc_ਸਮਾਉਂਦਾ(ਬਣਤਰ asprintf *infop, size_t ਹੋਰ)
{
  ਵਾਪਸ (infop->b->leng + infop->nstage + ਹੋਰ + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
ਸਥਿਰ ਪੂਰਨ alloc_addbyter(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ outc, ਖਾਲੀ *f)
{
  ਬਣਤਰ asprintf *infop = f;
  ਜੇ(infop->nstage == ਆਕਾਰ(infop->stage) && alloc_flush(infop))
    ਵਾਪਸ 1; /* fail */
  ਜੇ(!alloc_ਸਮਾਉਂਦਾ(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ਜੇ(alloc_flush(infop))
      ਵਾਪਸ 1;
    ਵਾਪਸ alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  ਵਾਪਸ 0;
}

/* block variant of alloc_addbyter */
ਸਥਿਰ size_t alloc_addrun(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t len, ਖਾਲੀ *f)
{
  ਬਣਤਰ asprintf *infop = f;
  size_t accepted = 0;
  ਜਦੋਂ(accepted < len) {
    size_t ਗੁੰਜਾਇਸ਼ = ਆਕਾਰ(infop->stage) - infop->nstage;
    size_t n;
    ਜੇ(!ਗੁੰਜਾਇਸ਼) {
      ਜੇ(alloc_flush(infop))
        ਵਾਪਸ accepted; /* accepted < len aborts formatting */
      ਗੁੰਜਾਇਸ਼ = ਆਕਾਰ(infop->stage);
    }
    n = len - accepted;
    ਜੇ(n > ਗੁੰਜਾਇਸ਼)
      n = ਗੁੰਜਾਇਸ਼;
    ਜੇ(!alloc_ਸਮਾਉਂਦਾ(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t ਸਮਾਉਂਦਾ = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ਜੇ(ਸਮਾਉਂਦਾ) {
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(&infop->stage[infop->nstage], buf + accepted, ਸਮਾਉਂਦਾ);
        infop->nstage += ਸਮਾਉਂਦਾ;
        accepted += ਸਮਾਉਂਦਾ;
      }
      ਜੇ(alloc_flush(infop))
        ਵਾਪਸ accepted;
      ਜੇ(alloc_addn(infop, buf + accepted, 1))
        ਵਾਪਸ accepted; /* accepted < len aborts formatting */
      accepted++;
      ਜਾਰੀ;
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  ਵਾਪਸ accepted;
}

/* appends the formatted string, returns MERR error code */
ਪੂਰਨ curlx_dyn_vprintf(ਬਣਤਰ dynbuf *dyn, ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ)
{
  ਬਣਤਰ asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (ਖਾਲੀ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, ਦਲੀਲਾਂ);
  ਜੇ(!info.merr)
    (ਖਾਲੀ)alloc_flush(&info);
  ਜੇ(info.merr) {
    curlx_dyn_free(info.b);
    ਵਾਪਸ info.merr;
  }
  ਵਾਪਸ 0;
}

ਅੱਖਰ *curl_mvaprintf(ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ)
{
  ਬਣਤਰ asprintf info;
  ਬਣਤਰ dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (ਖਾਲੀ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, ਦਲੀਲਾਂ);
  ਜੇ(!info.merr)
    (ਖਾਲੀ)alloc_flush(&info);
  ਜੇ(info.merr) {
    curlx_dyn_free(info.b);
    ਵਾਪਸ ਨਿਰਾ;
  }
  ਜੇ(curlx_dyn_len(info.b))
    ਵਾਪਸ curlx_dyn_ptr(info.b);
  ਵਾਪਸ curlx_strdup("");
}

ਅੱਖਰ *curl_maprintf(ਅਚਲ ਅੱਖਰ *format, ...)
{
  va_list ਦਲੀਲਾਂ;
  ਅੱਖਰ *s;
  va_start(ਦਲੀਲਾਂ, format);
  s = curl_mvaprintf(format, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);
  ਵਾਪਸ s;
}

ਸਥਿਰ ਪੂਰਨ storebuffer(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ outc, ਖਾਲੀ *f)
{
  ਅੱਖਰ **ਬਫਰ = f;
  **ਬਫਰ = (ਅੱਖਰ)outc;
  (*ਬਫਰ)++;
  ਵਾਪਸ 0;
}

/* block variant of storebuffer */
ਸਥਿਰ size_t storerun(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t len, ਖਾਲੀ *f)
{
  ਅੱਖਰ **ਬਫਰ = f;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(*ਬਫਰ, buf, len);
  *ਬਫਰ += len;
  ਵਾਪਸ len;
}

ਪੂਰਨ curl_msprintf(ਅੱਖਰ *ਬਫਰ, ਅਚਲ ਅੱਖਰ *format, ...)
{
  va_list ਦਲੀਲਾਂ; /* argument pointer */
  ਪੂਰਨ retcode;
  va_start(ਦਲੀਲਾਂ, format);
  retcode = formatf(&ਬਫਰ, storebuffer, storerun, ਨਿਰਾ, format, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);
  *ਬਫਰ = 0; /* we null-terminate this */
  ਵਾਪਸ retcode;
}

ਸਥਿਰ ਪੂਰਨ fputc_wrapper(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ outc, ਖਾਲੀ *f)
{
  ਬਣਤਰ ioprintf *i = f;
  ਪੂਰਨ ਬਾਹਰਲਾ = outc;
  FILE *s = i->fd;
  ਪੂਰਨ rc = fputc(ਬਾਹਰਲਾ, s);
  i->error = (rc == EOF);
  ਵਾਪਸ (rc == EOF);
}

/* block variant of fputc_wrapper */
ਸਥਿਰ size_t fwrite_wrapper(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t len, ਖਾਲੀ *f)
{
  ਬਣਤਰ ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  ਵਾਪਸ n;
}

ਪੂਰਨ curl_mvfprintf(FILE *fd, ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ)
{
  ਬਣਤਰ ioprintf info;
  ਪੂਰਨ n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, ਨਿਰਾ, format, ਦਲੀਲਾਂ);
  ਜੇ(info.error)
    ਵਾਪਸ -1;
  ਵਾਪਸ n;
}

ਪੂਰਨ curl_mfprintf(FILE *fd, ਅਚਲ ਅੱਖਰ *format, ...)
{
  va_list ਦਲੀਲਾਂ; /* argument pointer */
  ਪੂਰਨ n;
  va_start(ਦਲੀਲਾਂ, format);
  n = curl_mvfprintf(fd, format, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);
  ਵਾਪਸ n;
}

ਪੂਰਨ curl_mprintf(ਅਚਲ ਅੱਖਰ *format, ...)
{
  va_list ਦਲੀਲਾਂ; /* argument pointer */
  ਪੂਰਨ n;
  va_start(ਦਲੀਲਾਂ, format);
  n = curl_mvfprintf(stdout, format, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);
  ਵਾਪਸ n;
}

ਪੂਰਨ curl_mvsprintf(ਅੱਖਰ *ਬਫਰ, ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ)
{
  ਪੂਰਨ retcode = formatf(&ਬਫਰ, storebuffer, storerun, ਨਿਰਾ, format, ਦਲੀਲਾਂ);
  *ਬਫਰ = 0; /* we null-terminate this */
  ਵਾਪਸ retcode;
}

ਪੂਰਨ curl_mvprintf(ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ)
{
  ਵਾਪਸ curl_mvfprintf(stdout, format, ਦਲੀਲਾਂ);
}

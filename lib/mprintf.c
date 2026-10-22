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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "curl_printf.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ধ্রুব unsigned বর্ণ Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ধ্রুব unsigned বর্ণ Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  কুরু {                                    \
    যদি(stream((unsigned বর্ণ)(x), userp)) \
      প্রত্যাগম TRUE;                        \
    (*donep)++;                           \
  } যাবত্(0)

/* Data type to read from the arglist */
প্রকারনির্দেশ গণনা {
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
গণনা {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_বাম       = 1 << 2,
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

গণনা {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
সংরচনা va_প্রবেশ {
  FormatType type; /* FormatType */
  সঙ্ঘ {
    ধ্রুব বর্ণ *str;
    শূন্য *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    দ্বিগুণ dnum;
  } val;
};

/*
 * Describes an output segment.
 */
সংরচনা outsegment {
  পূর্ণাঙ্ক width;     /* width OR width parameter number */
  পূর্ণাঙ্ক precision; /* precision OR precision parameter number */
  unsigned পূর্ণাঙ্ক flags;
  unsigned পূর্ণাঙ্ক প্রবেশ; /* input argument array index */
  ধ্রুব বর্ণ *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

সংরচনা nsprintf {
  বর্ণ *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

সংরচনা asprintf {
  সংরচনা dynbuf *b;
  বর্ণ merr;
  size_t nstage;
  unsigned বর্ণ stage[ASPRINTF_STAGE_SIZE];
};

সংরচনা ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
স্থির পূর্ণাঙ্ক dollarstring(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ **end)
{
  curl_off_t অঙ্ক;
  যদি(curlx_str_সংখ্যা(&p, &অঙ্ক, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !অঙ্ক)
    প্রত্যাগম -1;
  *end = p;
  প্রত্যাগম (পূর্ণাঙ্ক)অঙ্ক - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned বর্ণ)(1 << ((y) & 7)))

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

স্থির পূর্ণাঙ্ক parse_flags(ধ্রুব বর্ণ **fmtp, unsigned পূর্ণাঙ্ক *flagsp, পূর্ণাঙ্ক use_dollar,
                       পূর্ণাঙ্ক *precp, পূর্ণাঙ্ক *widthp)
{
  ধ্রুব বর্ণ *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned পূর্ণাঙ্ক flags = 0;
  পূর্ণাঙ্ক width = 0;
  পূর্ণাঙ্ক precision = 0;

  /* Handle the flags */
  কুরু {
    বিকল্প(*fmt++) {
    পক্ষ ' ':
      flags |= FLAGS_SPACE;
      ভঙ্গ;
    পক্ষ '+':
      flags |= FLAGS_SHOWSIGN;
      ভঙ্গ;
    পক্ষ '-':
      flags |= FLAGS_বাম;
      flags &= ~(unsigned পূর্ণাঙ্ক)FLAGS_PAD_NIL;
      ভঙ্গ;
    পক্ষ '#':
      flags |= FLAGS_ALT;
      ভঙ্গ;
    পক্ষ '.':
      যদি('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        যদি(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          যদি(precision < 0)
            /* illegal combo */
            প্রত্যাগম PFMT_DOLLARPREC;
        }
        অন্যথা
          /* get it from the next argument */
          precision = -1;
      }
      অন্যথা {
        bool is_neg;
        curl_off_t অঙ্ক;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        যদি(is_neg)
          fmt++;
        যদি(curlx_str_সংখ্যা(&fmt, &অঙ্ক, INT_MAX))
          প্রত্যাগম PFMT_PREC;
        precision = (পূর্ণাঙ্ক)অঙ্ক;
        যদি(is_neg)
          precision = -precision;
      }
      যদি((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        প্রত্যাগম PFMT_PRECMIX;
      ভঙ্গ;
    পক্ষ 'h':
      flags |= FLAGS_SHORT;
      ভঙ্গ;
#ifdef _WIN32
    পক্ষ 'I':
      /* Non-ANSI integer extensions I32 I64 */
      যদি((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      অন্যথা যদি((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      অন্যথা {
#যদি SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#অন্যথা
        flags |= FLAGS_LONG;
#endif
      }
      ভঙ্গ;
#endif /* _WIN32 */
    পক্ষ 'l':
      যদি(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      অন্যথা
        flags |= FLAGS_LONG;
      ভঙ্গ;
    পক্ষ 'L':
      flags |= FLAGS_LONGDOUBLE;
      ভঙ্গ;
    পক্ষ 'q':
      flags |= FLAGS_LONGLONG;
      ভঙ্গ;
    পক্ষ 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#যদি SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#অন্যথা
      flags |= FLAGS_LONG;
#endif
      ভঙ্গ;
    পক্ষ 'O':
#যদি SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#অন্যথা
      flags |= FLAGS_LONG;
#endif
      ভঙ্গ;
    পক্ষ '0':
      যদি(!(flags & FLAGS_বাম))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    পক্ষ '1':
    পক্ষ '2':
    পক্ষ '3':
    পক্ষ '4':
    পক্ষ '5':
    পক্ষ '6':
    পক্ষ '7':
    পক্ষ '8':
    পক্ষ '9': {
      curl_off_t অঙ্ক;
      flags |= FLAGS_WIDTH;
      fmt--;
      যদি(curlx_str_সংখ্যা(&fmt, &অঙ্ক, INT_MAX))
        প্রত্যাগম PFMT_WIDTH;
      width = (পূর্ণাঙ্ক)অঙ্ক;
      ভঙ্গ;
    }
    পক্ষ '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      যদি(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        যদি(width < 0)
          /* illegal combo */
          প্রত্যাগম PFMT_DOLLARWIDTH;
      }
      অন্যথা
        /* pick from the next argument */
        width = -1;
      ভঙ্গ;
    স্বভাব:
      loopit = FALSE;
      fmt--;
      ভঙ্গ;
    } /* switch */
  } যাবত্(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  প্রত্যাগম PFMT_OK;
}

স্থির bool parse_conversion(ধ্রুব বর্ণ f, unsigned পূর্ণাঙ্ক *flagp,
                             FormatType *typep)
{
  unsigned পূর্ণাঙ্ক flags = *flagp;
  FormatType type;
  বিকল্প(f) {
  পক্ষ 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    ভঙ্গ;
  পক্ষ 's':
    type = MTYPE_STRING;
    ভঙ্গ;
  পক্ষ 'n':
    type = MTYPE_INTPTR;
    ভঙ্গ;
  পক্ষ 'p':
    type = MTYPE_PTR;
    ভঙ্গ;
  পক্ষ 'd':
  পক্ষ 'i':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    অন্যথা
      type = MTYPE_INT;
    ভঙ্গ;
  পক্ষ 'u':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    অন্যথা
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    ভঙ্গ;
  পক্ষ 'o':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    অন্যথা
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    ভঙ্গ;
  পক্ষ 'x':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    অন্যথা
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    ভঙ্গ;
  পক্ষ 'X':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    অন্যথা
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    ভঙ্গ;
  পক্ষ 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    ভঙ্গ;
  পক্ষ 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    ভঙ্গ;
  পক্ষ 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    ভঙ্গ;
  পক্ষ 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    ভঙ্গ;
  পক্ষ 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    ভঙ্গ;
  পক্ষ 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    ভঙ্গ;
  পক্ষ 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    ভঙ্গ;
  স্বভাব:
    /* invalid instruction, disregard and continue */
    প্রত্যাগম TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  প্রত্যাগম FALSE;
}

স্থির পূর্ণাঙ্ক parsefmt(ধ্রুব বর্ণ *format,
                    সংরচনা outsegment *out,
                    সংরচনা va_প্রবেশ *in,
                    পূর্ণাঙ্ক *opieces,
                    পূর্ণাঙ্ক *ipieces, va_list arglist)
{
  ধ্রুব বর্ণ *fmt = format;
  পূর্ণাঙ্ক param_অঙ্ক = 0;
  পূর্ণাঙ্ক max_param = -1;
  পূর্ণাঙ্ক i;
  পূর্ণাঙ্ক ocount = 0;
  unsigned বর্ণ usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  সংরচনা outsegment *optr;
  পূর্ণাঙ্ক use_dollar = DOLLAR_UNKNOWN;
  ধ্রুব বর্ণ *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, পরিমাণ(usedinput));

  যাবত্(*fmt) {
    যদি(*fmt == '%') {
      সংরচনা va_প্রবেশ *iptr;
      FormatType type;
      unsigned পূর্ণাঙ্ক flags = 0;
      পূর্ণাঙ্ক width = 0;
      পূর্ণাঙ্ক precision = 0;
      পূর্ণাঙ্ক param = -1;
      পূর্ণাঙ্ক rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      যদি(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        যদি(outlen) {
          optr = &out[ocount++];
          যদি(ocount > MAX_SEGMENTS)
            প্রত্যাগম PFMT_MANYSEGS;
          optr->প্রবেশ = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        অনুবর্ত; /* while */
      }

      যদি(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        যদি(param < 0) {
          যদি(use_dollar == DOLLAR_USE)
            /* illegal combo */
            প্রত্যাগম PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        অন্যথা
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      যদি(rc)
        প্রত্যাগম rc;

      যদি(parse_conversion(*fmt, &flags, &type))
        অনুবর্ত;

      যদি(flags & FLAGS_WIDTHPARAM) {
        যদি(width < 0)
          width = param_অঙ্ক++;
        অন্যথা {
          /* if this identifies a parameter already used, this is illegal */
          যদি(is_arg_used(usedinput, width))
            প্রত্যাগম PFMT_WIDTHARG;
        }
        যদি(width >= MAX_PARAMETERS)
          প্রত্যাগম PFMT_MANYARGS;
        যদি(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      যদি(flags & FLAGS_PRECPARAM) {
        যদি(precision < 0)
          precision = param_অঙ্ক++;
        অন্যথা {
          /* if this identifies a parameter already used, this is illegal */
          যদি(is_arg_used(usedinput, precision))
            প্রত্যাগম PFMT_PRECARG;
        }
        যদি(precision >= MAX_PARAMETERS)
          প্রত্যাগম PFMT_MANYARGS;
        যদি(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      যদি(param < 0)
        param = param_অঙ্ক++;
      যদি(param >= MAX_PARAMETERS)
        প্রত্যাগম PFMT_MANYARGS;
      যদি(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      যদি(ocount > MAX_SEGMENTS)
        প্রত্যাগম PFMT_MANYSEGS;
      optr->প্রবেশ = (unsigned পূর্ণাঙ্ক)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    অন্যথা
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  যদি(outlen) {
    optr = &out[ocount++];
    যদি(ocount > MAX_SEGMENTS)
      প্রত্যাগম PFMT_MANYSEGS;
    optr->প্রবেশ = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  প্রতি(i = 0; i < max_param + 1; i++) {
    সংরচনা va_প্রবেশ *iptr = &in[i];
    যদি(!is_arg_used(usedinput, i))
      /* bad input */
      প্রত্যাগম PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    বিকল্প(iptr->type) {
    পক্ষ MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ধ্রুব বর্ণ *);
      ভঙ্গ;

    পক্ষ MTYPE_INTPTR:
    পক্ষ MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, শূন্য *);
      ভঙ্গ;

    পক্ষ MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      ভঙ্গ;

    পক্ষ MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      ভঙ্গ;

    পক্ষ MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      ভঙ্গ;

    পক্ষ MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      ভঙ্গ;

    পক্ষ MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned পূর্ণাঙ্ক);
      ভঙ্গ;

    পক্ষ MTYPE_INT:
    পক্ষ MTYPE_WIDTH:
    পক্ষ MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, পূর্ণাঙ্ক);
      ভঙ্গ;

    পক্ষ MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, দ্বিগুণ);
      ভঙ্গ;

    পক্ষ MTYPE_LONGDOUBLE:
      iptr->val.dnum = (দ্বিগুণ)va_arg(arglist, long দ্বিগুণ);
      ভঙ্গ;

    স্বভাব:
      DEBUGASSERT(রিক্ত); /* unexpected */
      ভঙ্গ;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  প্রত্যাগম PFMT_OK;
}

সংরচনা mproperty {
  পূর্ণাঙ্ক width;            /* Width of a field. */
  পূর্ণাঙ্ক prec;             /* Precision of a field. */
  unsigned পূর্ণাঙ্ক flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
প্রকারনির্দেশ size_t (*mp_streamn)(ধ্রুব unsigned বর্ণ *buf, size_t len, শূন্য *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
প্রকারনির্দেশ পূর্ণাঙ্ক (*mp_flush)(শূন্য *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
স্থির bool stream_run(শূন্য *userp,
                       পূর্ণাঙ্ক (*stream)(unsigned বর্ণ, শূন্য *),
                       mp_streamn streamn,
                       ধ্রুব unsigned বর্ণ *buf, size_t len, পূর্ণাঙ্ক *donep)
{
  যদি(!len)
    প্রত্যাগম FALSE;
  যদি(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (পূর্ণাঙ্ক)n;
    প্রত্যাগম n != len;
  }
  যাবত্(len--) {
    যদি(stream(*buf++, userp))
      প্রত্যাগম TRUE;
    (*donep)++;
  }
  প্রত্যাগম FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
স্থির ধ্রুব বর্ণ pad_spaces[] = "                ";
স্থির ধ্রুব বর্ণ pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
স্থির bool stream_pad(শূন্য *userp,
                       পূর্ণাঙ্ক (*stream)(unsigned বর্ণ, শূন্য *),
                       mp_streamn streamn,
                       unsigned বর্ণ pad, পূর্ণাঙ্ক অঙ্ক, পূর্ণাঙ্ক *donep)
{
  ধ্রুব unsigned বর্ণ *src;
  যদি(অঙ্ক <= 0)
    প্রত্যাগম FALSE;
  যদি(pad == '0')
    src = (ধ্রুব unsigned বর্ণ *)pad_zeros;
  অন্যথা
    src = (ধ্রুব unsigned বর্ণ *)pad_spaces;
  যাবত্(অঙ্ক > PAD_RUN) {
    যদি(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      প্রত্যাগম TRUE;
    অঙ্ক -= PAD_RUN;
  }
  প্রত্যাগম stream_run(userp, stream, streamn, src, (size_t)অঙ্ক, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
স্থির bool stream_zrun(শূন্য *userp,
                        পূর্ণাঙ্ক (*stream)(unsigned বর্ণ, শূন্য *),
                        mp_streamn streamn,
                        ধ্রুব বর্ণ *str, size_t len, পূর্ণাঙ্ক *donep)
{
  size_t n = 0;
  যাবত্(n < len && str[n])
    n++;
  প্রত্যাগম stream_run(userp, stream, streamn,
                    (ধ্রুব unsigned বর্ণ *)str, n, donep);
}

স্থির bool out_double(শূন্য *userp,
                       পূর্ণাঙ্ক (*stream)(unsigned বর্ণ, শূন্য *),
                       mp_streamn streamn,
                       সংরচনা mproperty *p,
                       দ্বিগুণ dnum,
                       বর্ণ *work, পূর্ণাঙ্ক *donep)
{
  বর্ণ fmt[32] = "%";
  বর্ণ *fptr = &fmt[1];
  size_t বাম = পরিমাণ(fmt) - strlen(fmt);
  পূর্ণাঙ্ক flags = p->flags;
  পূর্ণাঙ্ক width = p->width;
  পূর্ণাঙ্ক prec = p->prec;

  যদি(flags & FLAGS_বাম)
    *fptr++ = '-';
  যদি(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  যদি(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  যদি(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  যদি(width >= 0) {
    size_t dlen;
    যদি(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, বাম, "%d", width);
    fptr += dlen;
    বাম -= dlen;
  }
  যদি(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    পূর্ণাঙ্ক maxprec = BUFFSIZE - 1;
    দ্বিগুণ val = dnum;
    পূর্ণাঙ্ক len;
    যদি(prec > maxprec)
      prec = maxprec - 1;
    যদি(width > 0 && prec <= width)
      maxprec -= width;
    যাবত্(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    যদি(prec > maxprec)
      prec = maxprec - 1;
    যদি(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, বাম, ".%d", prec);
    fptr += len;
  }
  যদি(flags & FLAGS_LONG)
    *fptr++ = 'l';

  যদি(flags & FLAGS_FLOATE)
    *fptr++ = (বর্ণ)((flags & FLAGS_UPPER) ? 'E' : 'e');
  অন্যথা যদি(flags & FLAGS_FLOATG)
    *fptr++ = (বর্ণ)((flags & FLAGS_UPPER) ? 'G' : 'g');
  অন্যথা
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#অন্যথা
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বের_করো
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  প্রত্যাগম stream_run(userp, stream, streamn,
                    (ধ্রুব unsigned বর্ণ *)work, strlen(work), donep);
}

স্থির bool out_সংখ্যা(শূন্য *userp,
                       পূর্ণাঙ্ক (*stream)(unsigned বর্ণ, শূন্য *),
                       mp_streamn streamn,
                       সংরচনা mproperty *p,
                       uint64_t অঙ্ক,
                       int64_t nums,
                       বর্ণ *work, পূর্ণাঙ্ক *donep)
{
  ধ্রুব unsigned বর্ণ *digits = Curl_ldigits;
  পূর্ণাঙ্ক flags = p->flags;
  পূর্ণাঙ্ক width = p->width;
  পূর্ণাঙ্ক prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  পূর্ণাঙ্ক base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  বর্ণ *workend = &work[BUFFSIZE - 2];
  বর্ণ *w;

  যদি(flags & FLAGS_CHAR) {
    /* Character. */
    যদি(!(flags & FLAGS_বাম)) {
      যদি(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        প্রত্যাগম TRUE;
      width = 1;
    }
    OUTCHAR((বর্ণ)অঙ্ক);
    যদি(flags & FLAGS_বাম) {
      যদি(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
  }
  যদি(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  অন্যথা যদি(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  অন্যথা যদি(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  অন্যথা {
    /* Decimal integer. */
    is_neg = (nums < 0);
    যদি(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_অঙ্ক; /* Used to convert negative in positive. */
      signed_অঙ্ক = nums + (int64_t)1;
      signed_অঙ্ক = -signed_অঙ্ক;
      অঙ্ক = (uint64_t)signed_অঙ্ক;
      অঙ্ক += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  যদি(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  বিকল্প(base) {
  পক্ষ 10:
    যাবত্(অঙ্ক > 0) {
      *w-- = (বর্ণ)('0' + (অঙ্ক % 10));
      অঙ্ক /= 10;
    }
    ভঙ্গ;
  স্বভাব:
    যাবত্(অঙ্ক > 0) {
      *w-- = digits[অঙ্ক % base];
      অঙ্ক /= base;
    }
    ভঙ্গ;
  }
  width -= (পূর্ণাঙ্ক)(workend - w);
  prec -= (পূর্ণাঙ্ক)(workend - w);

  যদি(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  যদি(prec > 0) {
    width -= prec;
    যাবত্(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  যদি(is_alt && base == 16)
    width -= 2;

  যদি(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  যদি(!(flags & FLAGS_বাম) && !(flags & FLAGS_PAD_NIL)) {
    যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
      প্রত্যাগম TRUE;
    width = 0;
  }

  যদি(is_neg)
    OUTCHAR('-');
  অন্যথা যদি(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  অন্যথা যদি(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  যদি(is_alt && base == 16) {
    OUTCHAR('0');
    যদি(flags & FLAGS_UPPER)
      OUTCHAR('X');
    অন্যথা
      OUTCHAR('x');
  }

  যদি(!(flags & FLAGS_বাম) && (flags & FLAGS_PAD_NIL)) {
    যদি(stream_pad(userp, stream, streamn, '0', width, donep))
      প্রত্যাগম TRUE;
    width = 0;
  }

  /* Write the number. */
  যদি(stream_run(userp, stream, streamn, (ধ্রুব unsigned বর্ণ *)w + 1,
                (size_t)(workend - w), donep))
    প্রত্যাগম TRUE;

  যদি(flags & FLAGS_বাম) {
    যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
      প্রত্যাগম TRUE;
  }

  প্রত্যাগম FALSE;
}

স্থির ধ্রুব বর্ণ nilstr[] = "(nil)";

স্থির bool out_string(শূন্য *userp,
                       পূর্ণাঙ্ক (*stream)(unsigned বর্ণ, শূন্য *),
                       mp_streamn streamn,
                       সংরচনা mproperty *p,
                       ধ্রুব বর্ণ *str,
                       পূর্ণাঙ্ক *donep)
{
  পূর্ণাঙ্ক flags = p->flags;
  পূর্ণাঙ্ক width = p->width;
  পূর্ণাঙ্ক prec = p->prec;
  size_t len;

  যদি(!str) {
    /* Write null string if there is space. */
    যদি(prec == -1 || prec >= (পূর্ণাঙ্ক)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned পূর্ণাঙ্ক)FLAGS_ALT;
    }
    অন্যথা {
      str = "";
      len = 0;
    }
  }
  অন্যথা যদি(prec != -1)
    len = (size_t)prec;
  অন্যথা যদি(*str == '\0')
    len = 0;
  অন্যথা
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (পূর্ণাঙ্ক)len;

  যদি(flags & FLAGS_ALT)
    OUTCHAR('"');

  যদি(!(flags & FLAGS_বাম)) {
    যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
      প্রত্যাগম TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  যদি(prec == -1) {
    যদি(stream_run(userp, stream, streamn,
                  (ধ্রুব unsigned বর্ণ *)str, len, donep))
      প্রত্যাগম TRUE;
  }
  অন্যথা যদি(stream_zrun(userp, stream, streamn, str, len, donep))
    প্রত্যাগম TRUE;
  যদি(flags & FLAGS_বাম) {
    যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
      প্রত্যাগম TRUE;
  }

  যদি(flags & FLAGS_ALT)
    OUTCHAR('"');

  প্রত্যাগম FALSE;
}

স্থির bool out_pointer(শূন্য *userp,
                        পূর্ণাঙ্ক (*stream)(unsigned বর্ণ, শূন্য *),
                        mp_streamn streamn,
                        সংরচনা mproperty *p,
                        ধ্রুব বর্ণ *ptr,
                        বর্ণ *work,
                        পূর্ণাঙ্ক *donep)
{
  /* Generic pointer. */
  যদি(ptr) {
    size_t অঙ্ক = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    যদি(out_সংখ্যা(userp, stream, streamn, p, অঙ্ক, 0, work, donep))
      প্রত্যাগম TRUE;
  }
  অন্যথা {
    /* Write "(nil)" for a nil pointer. */
    পূর্ণাঙ্ক width = p->width;
    পূর্ণাঙ্ক flags = p->flags;

    width -= (পূর্ণাঙ্ক)CURL_CSTRLEN(nilstr);
    যদি(flags & FLAGS_বাম) {
      যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
        প্রত্যাগম TRUE;
      width = 0;
    }
    যদি(stream_run(userp, stream, streamn, (ধ্রুব unsigned বর্ণ *)nilstr,
                  পরিমাণ(nilstr) - 1, donep))
      প্রত্যাগম TRUE;
    যদি(!(flags & FLAGS_বাম)) {
      যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
        প্রত্যাগম TRUE;
    }
  }
  প্রত্যাগম FALSE;
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
স্থির পূর্ণাঙ্ক formatf(শূন্য *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   পূর্ণাঙ্ক (*stream)(unsigned বর্ণ, শূন্য *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ধ্রুব বর্ণ *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  পূর্ণাঙ্ক done = 0;   /* number of characters written */
  পূর্ণাঙ্ক i;
  পূর্ণাঙ্ক ocount = 0; /* number of output segments */
  পূর্ণাঙ্ক icount = 0; /* number of input arguments */

  সংরচনা outsegment output[MAX_SEGMENTS];
  সংরচনা va_প্রবেশ প্রবেশ[MAX_PARAMETERS];
  বর্ণ work[BUFFSIZE + 2];

  /* Parse the format string */
  যদি(parsefmt(format, output, প্রবেশ, &ocount, &icount, ap_save))
    প্রত্যাগম 0;

  প্রতি(i = 0; i < ocount; i++) {
    সংরচনা outsegment *optr = &output[i];
    সংরচনা va_প্রবেশ *iptr = &প্রবেশ[optr->প্রবেশ];
    সংরচনা mproperty p;
    size_t outlen = optr->outlen;

    যদি(outlen) {
      যদি(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        প্রত্যাগম done;
      যদি(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        অনুবর্ত;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    যদি(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (পূর্ণাঙ্ক)প্রবেশ[optr->width].val.nums;
      যদি(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        যদি(p.width == INT_MIN)
          p.width = INT_MAX;
        অন্যথা
          p.width = -p.width;
        p.flags |= FLAGS_বাম;
        p.flags &= ~(unsigned পূর্ণাঙ্ক)FLAGS_PAD_NIL;
      }
    }
    অন্যথা
      p.width = optr->width;

    /* pick up the specified precision */
    যদি(p.flags & FLAGS_PRECPARAM) {
      p.prec = (পূর্ণাঙ্ক)প্রবেশ[optr->precision].val.nums;
      যদি(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    অন্যথা যদি(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    অন্যথা
      p.prec = -1;

    বিকল্প(iptr->type) {
    পক্ষ MTYPE_INTU:
    পক্ষ MTYPE_LONGU:
    পক্ষ MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      যদি(out_সংখ্যা(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        প্রত্যাগম done;
      ভঙ্গ;

    পক্ষ MTYPE_INT:
    পক্ষ MTYPE_LONG:
    পক্ষ MTYPE_LONGLONG:
      যদি(out_সংখ্যা(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        প্রত্যাগম done;
      ভঙ্গ;

    পক্ষ MTYPE_STRING:
      যদি(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        প্রত্যাগম done;
      ভঙ্গ;

    পক্ষ MTYPE_PTR:
      যদি(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        প্রত্যাগম done;
      ভঙ্গ;

    পক্ষ MTYPE_DOUBLE:
    পক্ষ MTYPE_LONGDOUBLE:
      যদি(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        প্রত্যাগম done;
      ভঙ্গ;

    পক্ষ MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      যদি(flush && flush(userp))
        প্রত্যাগম done;
      /* Answer the count of characters written. */
      যদি(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      অন্যথা যদি(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      অন্যথা যদি(!(p.flags & FLAGS_SHORT))
        *(পূর্ণাঙ্ক *)iptr->val.ptr = done;
      অন্যথা
        *(short *)iptr->val.ptr = (short)done;
      ভঙ্গ;

    স্বভাব:
      ভঙ্গ;
    }
  }
  প্রত্যাগম done;
}

/* fputc() look-alike */
স্থির পূর্ণাঙ্ক addbyter(unsigned বর্ণ outc, শূন্য *f)
{
  সংরচনা nsprintf *infop = f;
  যদি(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (বর্ণ)outc; /* store */
    infop->length++; /* we are now one byte larger */
    প্রত্যাগম 0;     /* fputc() returns like this on success */
  }
  প্রত্যাগম 1;
}

/* block variant of addbyter */
স্থির size_t addrun(ধ্রুব unsigned বর্ণ *buf, size_t len, শূন্য *f)
{
  সংরচনা nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  যদি(len > fit)
    len = fit;
  যদি(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  প্রত্যাগম len;
}

পূর্ণাঙ্ক curl_mvsnprintf(বর্ণ *buffer, size_t maxlength, ধ্রুব বর্ণ *format,
                    va_list args)
{
  পূর্ণাঙ্ক retcode;
  সংরচনা nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, রিক্ত, format, args);
  যদি(info.max) {
    /* we null-terminate this */
    যদি(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    অন্যথা
      info.buffer[0] = 0;
  }
  প্রত্যাগম retcode;
}

পূর্ণাঙ্ক curl_msnprintf(বর্ণ *buffer, size_t maxlength, ধ্রুব বর্ণ *format, ...)
{
  পূর্ণাঙ্ক retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  প্রত্যাগম retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
স্থির পূর্ণাঙ্ক alloc_flush(শূন্য *f)
{
  সংরচনা asprintf *infop = f;
  যদি(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    যদি(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      প্রত্যাগম 1; /* fail */
    }
  }
  প্রত্যাগম 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
স্থির পূর্ণাঙ্ক alloc_addn(সংরচনা asprintf *infop, ধ্রুব unsigned বর্ণ *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  যদি(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    প্রত্যাগম 1; /* fail */
  }
  প্রত্যাগম 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
স্থির bool alloc_fits(সংরচনা asprintf *infop, size_t more)
{
  প্রত্যাগম (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
স্থির পূর্ণাঙ্ক alloc_addbyter(unsigned বর্ণ outc, শূন্য *f)
{
  সংরচনা asprintf *infop = f;
  যদি(infop->nstage == পরিমাণ(infop->stage) && alloc_flush(infop))
    প্রত্যাগম 1; /* fail */
  যদি(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    যদি(alloc_flush(infop))
      প্রত্যাগম 1;
    প্রত্যাগম alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  প্রত্যাগম 0;
}

/* block variant of alloc_addbyter */
স্থির size_t alloc_addrun(ধ্রুব unsigned বর্ণ *buf, size_t len, শূন্য *f)
{
  সংরচনা asprintf *infop = f;
  size_t accepted = 0;
  যাবত্(accepted < len) {
    size_t room = পরিমাণ(infop->stage) - infop->nstage;
    size_t n;
    যদি(!room) {
      যদি(alloc_flush(infop))
        প্রত্যাগম accepted; /* accepted < len aborts formatting */
      room = পরিমাণ(infop->stage);
    }
    n = len - accepted;
    যদি(n > room)
      n = room;
    যদি(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      যদি(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      যদি(alloc_flush(infop))
        প্রত্যাগম accepted;
      যদি(alloc_addn(infop, buf + accepted, 1))
        প্রত্যাগম accepted; /* accepted < len aborts formatting */
      accepted++;
      অনুবর্ত;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  প্রত্যাগম accepted;
}

/* appends the formatted string, returns MERR error code */
পূর্ণাঙ্ক curlx_dyn_vprintf(সংরচনা dynbuf *dyn, ধ্রুব বর্ণ *format, va_list args)
{
  সংরচনা asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (শূন্য)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  যদি(!info.merr)
    (শূন্য)alloc_flush(&info);
  যদি(info.merr) {
    curlx_dyn_free(info.b);
    প্রত্যাগম info.merr;
  }
  প্রত্যাগম 0;
}

বর্ণ *curl_mvaprintf(ধ্রুব বর্ণ *format, va_list args)
{
  সংরচনা asprintf info;
  সংরচনা dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (শূন্য)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  যদি(!info.merr)
    (শূন্য)alloc_flush(&info);
  যদি(info.merr) {
    curlx_dyn_free(info.b);
    প্রত্যাগম রিক্ত;
  }
  যদি(curlx_dyn_len(info.b))
    প্রত্যাগম curlx_dyn_ptr(info.b);
  প্রত্যাগম curlx_strdup("");
}

বর্ণ *curl_maprintf(ধ্রুব বর্ণ *format, ...)
{
  va_list args;
  বর্ণ *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  প্রত্যাগম s;
}

স্থির পূর্ণাঙ্ক storebuffer(unsigned বর্ণ outc, শূন্য *f)
{
  বর্ণ **buffer = f;
  **buffer = (বর্ণ)outc;
  (*buffer)++;
  প্রত্যাগম 0;
}

/* block variant of storebuffer */
স্থির size_t storerun(ধ্রুব unsigned বর্ণ *buf, size_t len, শূন্য *f)
{
  বর্ণ **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  প্রত্যাগম len;
}

পূর্ণাঙ্ক curl_msprintf(বর্ণ *buffer, ধ্রুব বর্ণ *format, ...)
{
  va_list args; /* argument pointer */
  পূর্ণাঙ্ক retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, রিক্ত, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  প্রত্যাগম retcode;
}

স্থির পূর্ণাঙ্ক fputc_wrapper(unsigned বর্ণ outc, শূন্য *f)
{
  সংরচনা ioprintf *i = f;
  পূর্ণাঙ্ক out = outc;
  FILE *s = i->fd;
  পূর্ণাঙ্ক rc = fputc(out, s);
  i->error = (rc == EOF);
  প্রত্যাগম (rc == EOF);
}

/* block variant of fputc_wrapper */
স্থির size_t fwrite_wrapper(ধ্রুব unsigned বর্ণ *buf, size_t len, শূন্য *f)
{
  সংরচনা ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  প্রত্যাগম n;
}

পূর্ণাঙ্ক curl_mvfprintf(FILE *fd, ধ্রুব বর্ণ *format, va_list args)
{
  সংরচনা ioprintf info;
  পূর্ণাঙ্ক n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, রিক্ত, format, args);
  যদি(info.error)
    প্রত্যাগম -1;
  প্রত্যাগম n;
}

পূর্ণাঙ্ক curl_mfprintf(FILE *fd, ধ্রুব বর্ণ *format, ...)
{
  va_list args; /* argument pointer */
  পূর্ণাঙ্ক n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  প্রত্যাগম n;
}

পূর্ণাঙ্ক curl_mprintf(ধ্রুব বর্ণ *format, ...)
{
  va_list args; /* argument pointer */
  পূর্ণাঙ্ক n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  প্রত্যাগম n;
}

পূর্ণাঙ্ক curl_mvsprintf(বর্ণ *buffer, ধ্রুব বর্ণ *format, va_list args)
{
  পূর্ণাঙ্ক retcode = formatf(&buffer, storebuffer, storerun, রিক্ত, format, args);
  *buffer = 0; /* we null-terminate this */
  প্রত্যাগম retcode;
}

পূর্ণাঙ্ক curl_mvprintf(ধ্রুব বর্ণ *format, va_list args)
{
  প্রত্যাগম curl_mvfprintf(stdout, format, args);
}

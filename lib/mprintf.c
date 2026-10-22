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
ধ্রুবক unsigned অক্ষর Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ধ্রুবক unsigned অক্ষর Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  করো {                                    \
    যদি(stream((unsigned অক্ষর)(x), userp)) \
      ফেরত TRUE;                        \
    (*donep)++;                           \
  } যখন(0)

/* Data type to read from the arglist */
প্রকারলেখ গণনা {
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
কাঠামো va_প্রবেশ {
  FormatType type; /* FormatType */
  সংঘ {
    ধ্রুবক অক্ষর *str;
    শূন্য *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    দ্বিগুণ dnum;
  } val;
};

/*
 * Describes an output segment.
 */
কাঠামো outsegment {
  পূর্ণসংখ্যা width;     /* width OR width parameter number */
  পূর্ণসংখ্যা precision; /* precision OR precision parameter number */
  unsigned পূর্ণসংখ্যা flags;
  unsigned পূর্ণসংখ্যা প্রবেশ; /* input argument array index */
  ধ্রুবক অক্ষর *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

কাঠামো nsprintf {
  অক্ষর *বাফার;
  size_t দৈর্ঘ্য;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_আকার 512

কাঠামো asprintf {
  কাঠামো dynbuf *b;
  অক্ষর merr;
  size_t nstage;
  unsigned অক্ষর stage[ASPRINTF_STAGE_আকার];
};

কাঠামো ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
স্থির পূর্ণসংখ্যা dollarstring(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর **end)
{
  curl_off_t অঙ্ক;
  যদি(curlx_str_সংখ্যা(&p, &অঙ্ক, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !অঙ্ক)
    ফেরত -1;
  *end = p;
  ফেরত (পূর্ণসংখ্যা)অঙ্ক - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned অক্ষর)(1 << ((y) & 7)))

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

স্থির পূর্ণসংখ্যা parse_flags(ধ্রুবক অক্ষর **fmtp, unsigned পূর্ণসংখ্যা *flagsp, পূর্ণসংখ্যা use_dollar,
                       পূর্ণসংখ্যা *precp, পূর্ণসংখ্যা *widthp)
{
  ধ্রুবক অক্ষর *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned পূর্ণসংখ্যা flags = 0;
  পূর্ণসংখ্যা width = 0;
  পূর্ণসংখ্যা precision = 0;

  /* Handle the flags */
  করো {
    নির্বাচন(*fmt++) {
    ক্ষেত্র ' ':
      flags |= FLAGS_SPACE;
      বিরতি;
    ক্ষেত্র '+':
      flags |= FLAGS_SHOWSIGN;
      বিরতি;
    ক্ষেত্র '-':
      flags |= FLAGS_বাম;
      flags &= ~(unsigned পূর্ণসংখ্যা)FLAGS_PAD_NIL;
      বিরতি;
    ক্ষেত্র '#':
      flags |= FLAGS_ALT;
      বিরতি;
    ক্ষেত্র '.':
      যদি('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        যদি(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          যদি(precision < 0)
            /* illegal combo */
            ফেরত PFMT_DOLLARPREC;
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
          ফেরত PFMT_PREC;
        precision = (পূর্ণসংখ্যা)অঙ্ক;
        যদি(is_neg)
          precision = -precision;
      }
      যদি((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        ফেরত PFMT_PRECMIX;
      বিরতি;
    ক্ষেত্র 'h':
      flags |= FLAGS_SHORT;
      বিরতি;
#ifdef _WIN32
    ক্ষেত্র 'I':
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
#যদি SIZEOF_আকার_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#অন্যথা
        flags |= FLAGS_LONG;
#endif
      }
      বিরতি;
#endif /* _WIN32 */
    ক্ষেত্র 'l':
      যদি(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      অন্যথা
        flags |= FLAGS_LONG;
      বিরতি;
    ক্ষেত্র 'L':
      flags |= FLAGS_LONGDOUBLE;
      বিরতি;
    ক্ষেত্র 'q':
      flags |= FLAGS_LONGLONG;
      বিরতি;
    ক্ষেত্র 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#যদি SIZEOF_আকার_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#অন্যথা
      flags |= FLAGS_LONG;
#endif
      বিরতি;
    ক্ষেত্র 'O':
#যদি SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#অন্যথা
      flags |= FLAGS_LONG;
#endif
      বিরতি;
    ক্ষেত্র '0':
      যদি(!(flags & FLAGS_বাম))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    ক্ষেত্র '1':
    ক্ষেত্র '2':
    ক্ষেত্র '3':
    ক্ষেত্র '4':
    ক্ষেত্র '5':
    ক্ষেত্র '6':
    ক্ষেত্র '7':
    ক্ষেত্র '8':
    ক্ষেত্র '9': {
      curl_off_t অঙ্ক;
      flags |= FLAGS_WIDTH;
      fmt--;
      যদি(curlx_str_সংখ্যা(&fmt, &অঙ্ক, INT_MAX))
        ফেরত PFMT_WIDTH;
      width = (পূর্ণসংখ্যা)অঙ্ক;
      বিরতি;
    }
    ক্ষেত্র '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      যদি(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        যদি(width < 0)
          /* illegal combo */
          ফেরত PFMT_DOLLARWIDTH;
      }
      অন্যথা
        /* pick from the next argument */
        width = -1;
      বিরতি;
    পূর্বনির্ধারিত:
      loopit = FALSE;
      fmt--;
      বিরতি;
    } /* switch */
  } যখন(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  ফেরত PFMT_OK;
}

স্থির bool parse_conversion(ধ্রুবক অক্ষর f, unsigned পূর্ণসংখ্যা *flagp,
                             FormatType *typep)
{
  unsigned পূর্ণসংখ্যা flags = *flagp;
  FormatType type;
  নির্বাচন(f) {
  ক্ষেত্র 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    বিরতি;
  ক্ষেত্র 's':
    type = MTYPE_STRING;
    বিরতি;
  ক্ষেত্র 'n':
    type = MTYPE_INTPTR;
    বিরতি;
  ক্ষেত্র 'p':
    type = MTYPE_PTR;
    বিরতি;
  ক্ষেত্র 'd':
  ক্ষেত্র 'i':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    অন্যথা
      type = MTYPE_INT;
    বিরতি;
  ক্ষেত্র 'u':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    অন্যথা
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    বিরতি;
  ক্ষেত্র 'o':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    অন্যথা
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    বিরতি;
  ক্ষেত্র 'x':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    অন্যথা
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    বিরতি;
  ক্ষেত্র 'X':
    যদি(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    অন্যথা যদি(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    অন্যথা
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    বিরতি;
  ক্ষেত্র 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    বিরতি;
  ক্ষেত্র 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    বিরতি;
  ক্ষেত্র 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    বিরতি;
  ক্ষেত্র 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    বিরতি;
  ক্ষেত্র 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    বিরতি;
  ক্ষেত্র 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    বিরতি;
  ক্ষেত্র 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    বিরতি;
  পূর্বনির্ধারিত:
    /* invalid instruction, disregard and continue */
    ফেরত TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  ফেরত FALSE;
}

স্থির পূর্ণসংখ্যা parsefmt(ধ্রুবক অক্ষর *format,
                    কাঠামো outsegment *out,
                    কাঠামো va_প্রবেশ *in,
                    পূর্ণসংখ্যা *opieces,
                    পূর্ণসংখ্যা *ipieces, va_list arglist)
{
  ধ্রুবক অক্ষর *fmt = format;
  পূর্ণসংখ্যা param_অঙ্ক = 0;
  পূর্ণসংখ্যা max_param = -1;
  পূর্ণসংখ্যা i;
  পূর্ণসংখ্যা ocount = 0;
  unsigned অক্ষর usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  কাঠামো outsegment *optr;
  পূর্ণসংখ্যা use_dollar = DOLLAR_UNKNOWN;
  ধ্রুবক অক্ষর *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, আকার(usedinput));

  যখন(*fmt) {
    যদি(*fmt == '%') {
      কাঠামো va_প্রবেশ *iptr;
      FormatType type;
      unsigned পূর্ণসংখ্যা flags = 0;
      পূর্ণসংখ্যা width = 0;
      পূর্ণসংখ্যা precision = 0;
      পূর্ণসংখ্যা param = -1;
      পূর্ণসংখ্যা rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      যদি(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        যদি(outlen) {
          optr = &out[ocount++];
          যদি(ocount > MAX_SEGMENTS)
            ফেরত PFMT_MANYSEGS;
          optr->প্রবেশ = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        চালু; /* while */
      }

      যদি(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        যদি(param < 0) {
          যদি(use_dollar == DOLLAR_USE)
            /* illegal combo */
            ফেরত PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        অন্যথা
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      যদি(rc)
        ফেরত rc;

      যদি(parse_conversion(*fmt, &flags, &type))
        চালু;

      যদি(flags & FLAGS_WIDTHPARAM) {
        যদি(width < 0)
          width = param_অঙ্ক++;
        অন্যথা {
          /* if this identifies a parameter already used, this is illegal */
          যদি(is_arg_used(usedinput, width))
            ফেরত PFMT_WIDTHARG;
        }
        যদি(width >= MAX_PARAMETERS)
          ফেরত PFMT_MANYARGS;
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
            ফেরত PFMT_PRECARG;
        }
        যদি(precision >= MAX_PARAMETERS)
          ফেরত PFMT_MANYARGS;
        যদি(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      যদি(param < 0)
        param = param_অঙ্ক++;
      যদি(param >= MAX_PARAMETERS)
        ফেরত PFMT_MANYARGS;
      যদি(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      যদি(ocount > MAX_SEGMENTS)
        ফেরত PFMT_MANYSEGS;
      optr->প্রবেশ = (unsigned পূর্ণসংখ্যা)param;
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
      ফেরত PFMT_MANYSEGS;
    optr->প্রবেশ = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  জন্য(i = 0; i < max_param + 1; i++) {
    কাঠামো va_প্রবেশ *iptr = &in[i];
    যদি(!is_arg_used(usedinput, i))
      /* bad input */
      ফেরত PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    নির্বাচন(iptr->type) {
    ক্ষেত্র MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ধ্রুবক অক্ষর *);
      বিরতি;

    ক্ষেত্র MTYPE_INTPTR:
    ক্ষেত্র MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, শূন্য *);
      বিরতি;

    ক্ষেত্র MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      বিরতি;

    ক্ষেত্র MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      বিরতি;

    ক্ষেত্র MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      বিরতি;

    ক্ষেত্র MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      বিরতি;

    ক্ষেত্র MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned পূর্ণসংখ্যা);
      বিরতি;

    ক্ষেত্র MTYPE_INT:
    ক্ষেত্র MTYPE_WIDTH:
    ক্ষেত্র MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, পূর্ণসংখ্যা);
      বিরতি;

    ক্ষেত্র MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, দ্বিগুণ);
      বিরতি;

    ক্ষেত্র MTYPE_LONGDOUBLE:
      iptr->val.dnum = (দ্বিগুণ)va_arg(arglist, long দ্বিগুণ);
      বিরতি;

    পূর্বনির্ধারিত:
      DEBUGASSERT(ফাঁকা); /* unexpected */
      বিরতি;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  ফেরত PFMT_OK;
}

কাঠামো mproperty {
  পূর্ণসংখ্যা width;            /* Width of a field. */
  পূর্ণসংখ্যা prec;             /* Precision of a field. */
  unsigned পূর্ণসংখ্যা flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
প্রকারলেখ size_t (*mp_streamn)(ধ্রুবক unsigned অক্ষর *buf, size_t len, শূন্য *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
প্রকারলেখ পূর্ণসংখ্যা (*mp_flush)(শূন্য *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
স্থির bool stream_run(শূন্য *userp,
                       পূর্ণসংখ্যা (*stream)(unsigned অক্ষর, শূন্য *),
                       mp_streamn streamn,
                       ধ্রুবক unsigned অক্ষর *buf, size_t len, পূর্ণসংখ্যা *donep)
{
  যদি(!len)
    ফেরত FALSE;
  যদি(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (পূর্ণসংখ্যা)n;
    ফেরত n != len;
  }
  যখন(len--) {
    যদি(stream(*buf++, userp))
      ফেরত TRUE;
    (*donep)++;
  }
  ফেরত FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
স্থির ধ্রুবক অক্ষর pad_spaces[] = "                ";
স্থির ধ্রুবক অক্ষর pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
স্থির bool stream_pad(শূন্য *userp,
                       পূর্ণসংখ্যা (*stream)(unsigned অক্ষর, শূন্য *),
                       mp_streamn streamn,
                       unsigned অক্ষর pad, পূর্ণসংখ্যা অঙ্ক, পূর্ণসংখ্যা *donep)
{
  ধ্রুবক unsigned অক্ষর *src;
  যদি(অঙ্ক <= 0)
    ফেরত FALSE;
  যদি(pad == '0')
    src = (ধ্রুবক unsigned অক্ষর *)pad_zeros;
  অন্যথা
    src = (ধ্রুবক unsigned অক্ষর *)pad_spaces;
  যখন(অঙ্ক > PAD_RUN) {
    যদি(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      ফেরত TRUE;
    অঙ্ক -= PAD_RUN;
  }
  ফেরত stream_run(userp, stream, streamn, src, (size_t)অঙ্ক, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
স্থির bool stream_zrun(শূন্য *userp,
                        পূর্ণসংখ্যা (*stream)(unsigned অক্ষর, শূন্য *),
                        mp_streamn streamn,
                        ধ্রুবক অক্ষর *str, size_t len, পূর্ণসংখ্যা *donep)
{
  size_t n = 0;
  যখন(n < len && str[n])
    n++;
  ফেরত stream_run(userp, stream, streamn,
                    (ধ্রুবক unsigned অক্ষর *)str, n, donep);
}

স্থির bool out_double(শূন্য *userp,
                       পূর্ণসংখ্যা (*stream)(unsigned অক্ষর, শূন্য *),
                       mp_streamn streamn,
                       কাঠামো mproperty *p,
                       দ্বিগুণ dnum,
                       অক্ষর *work, পূর্ণসংখ্যা *donep)
{
  অক্ষর fmt[32] = "%";
  অক্ষর *fptr = &fmt[1];
  size_t বাম = আকার(fmt) - strlen(fmt);
  পূর্ণসংখ্যা flags = p->flags;
  পূর্ণসংখ্যা width = p->width;
  পূর্ণসংখ্যা prec = p->prec;

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
    পূর্ণসংখ্যা maxprec = BUFFSIZE - 1;
    দ্বিগুণ val = dnum;
    পূর্ণসংখ্যা len;
    যদি(prec > maxprec)
      prec = maxprec - 1;
    যদি(width > 0 && prec <= width)
      maxprec -= width;
    যখন(maxprec && (val >= 10.0)) {
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
    *fptr++ = (অক্ষর)((flags & FLAGS_UPPER) ? 'E' : 'e');
  অন্যথা যদি(flags & FLAGS_FLOATG)
    *fptr++ = (অক্ষর)((flags & FLAGS_UPPER) ? 'G' : 'g');
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
  ফেরত stream_run(userp, stream, streamn,
                    (ধ্রুবক unsigned অক্ষর *)work, strlen(work), donep);
}

স্থির bool out_সংখ্যা(শূন্য *userp,
                       পূর্ণসংখ্যা (*stream)(unsigned অক্ষর, শূন্য *),
                       mp_streamn streamn,
                       কাঠামো mproperty *p,
                       uint64_t অঙ্ক,
                       int64_t nums,
                       অক্ষর *work, পূর্ণসংখ্যা *donep)
{
  ধ্রুবক unsigned অক্ষর *digits = Curl_ldigits;
  পূর্ণসংখ্যা flags = p->flags;
  পূর্ণসংখ্যা width = p->width;
  পূর্ণসংখ্যা prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  পূর্ণসংখ্যা base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  অক্ষর *workend = &work[BUFFSIZE - 2];
  অক্ষর *w;

  যদি(flags & FLAGS_CHAR) {
    /* Character. */
    যদি(!(flags & FLAGS_বাম)) {
      যদি(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ফেরত TRUE;
      width = 1;
    }
    OUTCHAR((অক্ষর)অঙ্ক);
    যদি(flags & FLAGS_বাম) {
      যদি(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ফেরত TRUE;
    }
    ফেরত FALSE;
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
  নির্বাচন(base) {
  ক্ষেত্র 10:
    যখন(অঙ্ক > 0) {
      *w-- = (অক্ষর)('0' + (অঙ্ক % 10));
      অঙ্ক /= 10;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    যখন(অঙ্ক > 0) {
      *w-- = digits[অঙ্ক % base];
      অঙ্ক /= base;
    }
    বিরতি;
  }
  width -= (পূর্ণসংখ্যা)(workend - w);
  prec -= (পূর্ণসংখ্যা)(workend - w);

  যদি(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  যদি(prec > 0) {
    width -= prec;
    যখন(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  যদি(is_alt && base == 16)
    width -= 2;

  যদি(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  যদি(!(flags & FLAGS_বাম) && !(flags & FLAGS_PAD_NIL)) {
    যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
      ফেরত TRUE;
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
      ফেরত TRUE;
    width = 0;
  }

  /* Write the number. */
  যদি(stream_run(userp, stream, streamn, (ধ্রুবক unsigned অক্ষর *)w + 1,
                (size_t)(workend - w), donep))
    ফেরত TRUE;

  যদি(flags & FLAGS_বাম) {
    যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
      ফেরত TRUE;
  }

  ফেরত FALSE;
}

স্থির ধ্রুবক অক্ষর nilstr[] = "(nil)";

স্থির bool out_string(শূন্য *userp,
                       পূর্ণসংখ্যা (*stream)(unsigned অক্ষর, শূন্য *),
                       mp_streamn streamn,
                       কাঠামো mproperty *p,
                       ধ্রুবক অক্ষর *str,
                       পূর্ণসংখ্যা *donep)
{
  পূর্ণসংখ্যা flags = p->flags;
  পূর্ণসংখ্যা width = p->width;
  পূর্ণসংখ্যা prec = p->prec;
  size_t len;

  যদি(!str) {
    /* Write null string if there is space. */
    যদি(prec == -1 || prec >= (পূর্ণসংখ্যা)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned পূর্ণসংখ্যা)FLAGS_ALT;
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

  width -= (len > INT_MAX) ? INT_MAX : (পূর্ণসংখ্যা)len;

  যদি(flags & FLAGS_ALT)
    OUTCHAR('"');

  যদি(!(flags & FLAGS_বাম)) {
    যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
      ফেরত TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  যদি(prec == -1) {
    যদি(stream_run(userp, stream, streamn,
                  (ধ্রুবক unsigned অক্ষর *)str, len, donep))
      ফেরত TRUE;
  }
  অন্যথা যদি(stream_zrun(userp, stream, streamn, str, len, donep))
    ফেরত TRUE;
  যদি(flags & FLAGS_বাম) {
    যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
      ফেরত TRUE;
  }

  যদি(flags & FLAGS_ALT)
    OUTCHAR('"');

  ফেরত FALSE;
}

স্থির bool out_pointer(শূন্য *userp,
                        পূর্ণসংখ্যা (*stream)(unsigned অক্ষর, শূন্য *),
                        mp_streamn streamn,
                        কাঠামো mproperty *p,
                        ধ্রুবক অক্ষর *ptr,
                        অক্ষর *work,
                        পূর্ণসংখ্যা *donep)
{
  /* Generic pointer. */
  যদি(ptr) {
    size_t অঙ্ক = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    যদি(out_সংখ্যা(userp, stream, streamn, p, অঙ্ক, 0, work, donep))
      ফেরত TRUE;
  }
  অন্যথা {
    /* Write "(nil)" for a nil pointer. */
    পূর্ণসংখ্যা width = p->width;
    পূর্ণসংখ্যা flags = p->flags;

    width -= (পূর্ণসংখ্যা)CURL_CSTRLEN(nilstr);
    যদি(flags & FLAGS_বাম) {
      যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
        ফেরত TRUE;
      width = 0;
    }
    যদি(stream_run(userp, stream, streamn, (ধ্রুবক unsigned অক্ষর *)nilstr,
                  আকার(nilstr) - 1, donep))
      ফেরত TRUE;
    যদি(!(flags & FLAGS_বাম)) {
      যদি(stream_pad(userp, stream, streamn, ' ', width, donep))
        ফেরত TRUE;
    }
  }
  ফেরত FALSE;
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
স্থির পূর্ণসংখ্যা formatf(শূন্য *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   পূর্ণসংখ্যা (*stream)(unsigned অক্ষর, শূন্য *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ধ্রুবক অক্ষর *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  পূর্ণসংখ্যা done = 0;   /* number of characters written */
  পূর্ণসংখ্যা i;
  পূর্ণসংখ্যা ocount = 0; /* number of output segments */
  পূর্ণসংখ্যা icount = 0; /* number of input arguments */

  কাঠামো outsegment output[MAX_SEGMENTS];
  কাঠামো va_প্রবেশ প্রবেশ[MAX_PARAMETERS];
  অক্ষর work[BUFFSIZE + 2];

  /* Parse the format string */
  যদি(parsefmt(format, output, প্রবেশ, &ocount, &icount, ap_save))
    ফেরত 0;

  জন্য(i = 0; i < ocount; i++) {
    কাঠামো outsegment *optr = &output[i];
    কাঠামো va_প্রবেশ *iptr = &প্রবেশ[optr->প্রবেশ];
    কাঠামো mproperty p;
    size_t outlen = optr->outlen;

    যদি(outlen) {
      যদি(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        ফেরত done;
      যদি(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        চালু;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    যদি(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (পূর্ণসংখ্যা)প্রবেশ[optr->width].val.nums;
      যদি(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        যদি(p.width == INT_MIN)
          p.width = INT_MAX;
        অন্যথা
          p.width = -p.width;
        p.flags |= FLAGS_বাম;
        p.flags &= ~(unsigned পূর্ণসংখ্যা)FLAGS_PAD_NIL;
      }
    }
    অন্যথা
      p.width = optr->width;

    /* pick up the specified precision */
    যদি(p.flags & FLAGS_PRECPARAM) {
      p.prec = (পূর্ণসংখ্যা)প্রবেশ[optr->precision].val.nums;
      যদি(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    অন্যথা যদি(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    অন্যথা
      p.prec = -1;

    নির্বাচন(iptr->type) {
    ক্ষেত্র MTYPE_INTU:
    ক্ষেত্র MTYPE_LONGU:
    ক্ষেত্র MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      যদি(out_সংখ্যা(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        ফেরত done;
      বিরতি;

    ক্ষেত্র MTYPE_INT:
    ক্ষেত্র MTYPE_LONG:
    ক্ষেত্র MTYPE_LONGLONG:
      যদি(out_সংখ্যা(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        ফেরত done;
      বিরতি;

    ক্ষেত্র MTYPE_STRING:
      যদি(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        ফেরত done;
      বিরতি;

    ক্ষেত্র MTYPE_PTR:
      যদি(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        ফেরত done;
      বিরতি;

    ক্ষেত্র MTYPE_DOUBLE:
    ক্ষেত্র MTYPE_LONGDOUBLE:
      যদি(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        ফেরত done;
      বিরতি;

    ক্ষেত্র MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      যদি(flush && flush(userp))
        ফেরত done;
      /* Answer the count of characters written. */
      যদি(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      অন্যথা যদি(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      অন্যথা যদি(!(p.flags & FLAGS_SHORT))
        *(পূর্ণসংখ্যা *)iptr->val.ptr = done;
      অন্যথা
        *(short *)iptr->val.ptr = (short)done;
      বিরতি;

    পূর্বনির্ধারিত:
      বিরতি;
    }
  }
  ফেরত done;
}

/* fputc() look-alike */
স্থির পূর্ণসংখ্যা addbyter(unsigned অক্ষর outc, শূন্য *f)
{
  কাঠামো nsprintf *infop = f;
  যদি(infop->দৈর্ঘ্য < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->বাফার++ = (অক্ষর)outc; /* store */
    infop->দৈর্ঘ্য++; /* we are now one byte larger */
    ফেরত 0;     /* fputc() returns like this on success */
  }
  ফেরত 1;
}

/* block variant of addbyter */
স্থির size_t addrun(ধ্রুবক unsigned অক্ষর *buf, size_t len, শূন্য *f)
{
  কাঠামো nsprintf *infop = f;
  size_t fit = infop->max - infop->দৈর্ঘ্য;
  যদি(len > fit)
    len = fit;
  যদি(len) {
    memcpy(infop->বাফার, buf, len);
    infop->বাফার += len;
    infop->দৈর্ঘ্য += len;
  }
  ফেরত len;
}

পূর্ণসংখ্যা curl_mvsnprintf(অক্ষর *বাফার, size_t maxlength, ধ্রুবক অক্ষর *format,
                    va_list args)
{
  পূর্ণসংখ্যা retcode;
  কাঠামো nsprintf info;

  info.বাফার = বাফার;
  info.দৈর্ঘ্য = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, ফাঁকা, format, args);
  যদি(info.max) {
    /* we null-terminate this */
    যদি(info.max == info.দৈর্ঘ্য) {
      /* we are at maximum, scrap the last letter */
      info.বাফার[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    অন্যথা
      info.বাফার[0] = 0;
  }
  ফেরত retcode;
}

পূর্ণসংখ্যা curl_msnprintf(অক্ষর *বাফার, size_t maxlength, ধ্রুবক অক্ষর *format, ...)
{
  পূর্ণসংখ্যা retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(বাফার, maxlength, format, args);
  va_end(args);
  ফেরত retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
স্থির পূর্ণসংখ্যা alloc_flush(শূন্য *f)
{
  কাঠামো asprintf *infop = f;
  যদি(infop->nstage) {
    CURLcode ফলাফল = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    যদি(ফলাফল) {
      infop->merr = ফলাফল == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      ফেরত 1; /* fail */
    }
  }
  ফেরত 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
স্থির পূর্ণসংখ্যা alloc_addn(কাঠামো asprintf *infop, ধ্রুবক unsigned অক্ষর *buf,
                      size_t len)
{
  CURLcode ফলাফল = curlx_dyn_addn(infop->b, buf, len);
  যদি(ফলাফল) {
    infop->merr = ফলাফল == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    ফেরত 1; /* fail */
  }
  ফেরত 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
স্থির bool alloc_fits(কাঠামো asprintf *infop, size_t more)
{
  ফেরত (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
স্থির পূর্ণসংখ্যা alloc_addbyter(unsigned অক্ষর outc, শূন্য *f)
{
  কাঠামো asprintf *infop = f;
  যদি(infop->nstage == আকার(infop->stage) && alloc_flush(infop))
    ফেরত 1; /* fail */
  যদি(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    যদি(alloc_flush(infop))
      ফেরত 1;
    ফেরত alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  ফেরত 0;
}

/* block variant of alloc_addbyter */
স্থির size_t alloc_addrun(ধ্রুবক unsigned অক্ষর *buf, size_t len, শূন্য *f)
{
  কাঠামো asprintf *infop = f;
  size_t accepted = 0;
  যখন(accepted < len) {
    size_t room = আকার(infop->stage) - infop->nstage;
    size_t n;
    যদি(!room) {
      যদি(alloc_flush(infop))
        ফেরত accepted; /* accepted < len aborts formatting */
      room = আকার(infop->stage);
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
        ফেরত accepted;
      যদি(alloc_addn(infop, buf + accepted, 1))
        ফেরত accepted; /* accepted < len aborts formatting */
      accepted++;
      চালু;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  ফেরত accepted;
}

/* appends the formatted string, returns MERR error code */
পূর্ণসংখ্যা curlx_dyn_vprintf(কাঠামো dynbuf *dyn, ধ্রুবক অক্ষর *format, va_list args)
{
  কাঠামো asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (শূন্য)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  যদি(!info.merr)
    (শূন্য)alloc_flush(&info);
  যদি(info.merr) {
    curlx_dyn_free(info.b);
    ফেরত info.merr;
  }
  ফেরত 0;
}

অক্ষর *curl_mvaprintf(ধ্রুবক অক্ষর *format, va_list args)
{
  কাঠামো asprintf info;
  কাঠামো dynbuf dyn;
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
    ফেরত ফাঁকা;
  }
  যদি(curlx_dyn_len(info.b))
    ফেরত curlx_dyn_ptr(info.b);
  ফেরত curlx_strdup("");
}

অক্ষর *curl_maprintf(ধ্রুবক অক্ষর *format, ...)
{
  va_list args;
  অক্ষর *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  ফেরত s;
}

স্থির পূর্ণসংখ্যা storebuffer(unsigned অক্ষর outc, শূন্য *f)
{
  অক্ষর **বাফার = f;
  **বাফার = (অক্ষর)outc;
  (*বাফার)++;
  ফেরত 0;
}

/* block variant of storebuffer */
স্থির size_t storerun(ধ্রুবক unsigned অক্ষর *buf, size_t len, শূন্য *f)
{
  অক্ষর **বাফার = f;
  memcpy(*বাফার, buf, len);
  *বাফার += len;
  ফেরত len;
}

পূর্ণসংখ্যা curl_msprintf(অক্ষর *বাফার, ধ্রুবক অক্ষর *format, ...)
{
  va_list args; /* argument pointer */
  পূর্ণসংখ্যা retcode;
  va_start(args, format);
  retcode = formatf(&বাফার, storebuffer, storerun, ফাঁকা, format, args);
  va_end(args);
  *বাফার = 0; /* we null-terminate this */
  ফেরত retcode;
}

স্থির পূর্ণসংখ্যা fputc_wrapper(unsigned অক্ষর outc, শূন্য *f)
{
  কাঠামো ioprintf *i = f;
  পূর্ণসংখ্যা out = outc;
  FILE *s = i->fd;
  পূর্ণসংখ্যা rc = fputc(out, s);
  i->error = (rc == EOF);
  ফেরত (rc == EOF);
}

/* block variant of fputc_wrapper */
স্থির size_t fwrite_wrapper(ধ্রুবক unsigned অক্ষর *buf, size_t len, শূন্য *f)
{
  কাঠামো ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  ফেরত n;
}

পূর্ণসংখ্যা curl_mvfprintf(FILE *fd, ধ্রুবক অক্ষর *format, va_list args)
{
  কাঠামো ioprintf info;
  পূর্ণসংখ্যা n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, ফাঁকা, format, args);
  যদি(info.error)
    ফেরত -1;
  ফেরত n;
}

পূর্ণসংখ্যা curl_mfprintf(FILE *fd, ধ্রুবক অক্ষর *format, ...)
{
  va_list args; /* argument pointer */
  পূর্ণসংখ্যা n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  ফেরত n;
}

পূর্ণসংখ্যা curl_mprintf(ধ্রুবক অক্ষর *format, ...)
{
  va_list args; /* argument pointer */
  পূর্ণসংখ্যা n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  ফেরত n;
}

পূর্ণসংখ্যা curl_mvsprintf(অক্ষর *বাফার, ধ্রুবক অক্ষর *format, va_list args)
{
  পূর্ণসংখ্যা retcode = formatf(&বাফার, storebuffer, storerun, ফাঁকা, format, args);
  *বাফার = 0; /* we null-terminate this */
  ফেরত retcode;
}

পূর্ণসংখ্যা curl_mvprintf(ধ্রুবক অক্ষর *format, va_list args)
{
  ফেরত curl_mvfprintf(stdout, format, args);
}

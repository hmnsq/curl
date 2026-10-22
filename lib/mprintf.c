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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "curl_printf.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#परिभाषय BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#परिभाषय MAX_PARAMETERS 128 /* number of input arguments */
#परिभाषय MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ध्रुव अचिह्नित वर्ण Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ध्रुव अचिह्नित वर्ण Curl_udigits[] = "0123456789ABCDEF";

#परिभाषय OUTCHAR(x)                        \
  कुरु {                                    \
    यदि(प्रवाहः((अचिह्नित वर्ण)(x), userp)) \
      प्रत्यागम TRUE;                        \
    (*donep)++;                           \
  } यावत्(0)

/* Data type to read from the arglist */
प्रकारनिर्देश गणना {
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
  MTYPE_विस्तारः,
  MTYPE_PRECISION
} FormatType;

/* conversion and display flags */
गणना {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_वामः       = 1 << 2,
  FLAGS_ALT        = 1 << 3,
  FLAGS_SHORT      = 1 << 4,
  FLAGS_LONG       = 1 << 5,
  FLAGS_LONGLONG   = 1 << 6,
  FLAGS_LONGDOUBLE = 1 << 7,
  FLAGS_PAD_NIL    = 1 << 8,
  FLAGS_UNSIGNED   = 1 << 9,
  FLAGS_OCTAL      = 1 << 10,
  FLAGS_HEX        = 1 << 11,
  FLAGS_ऊर्ध्वस्थम्      = 1 << 12,
  FLAGS_विस्तारः      = 1 << 13, /* '*' or '*<num>$' used */
  FLAGS_WIDTHPARAM = 1 << 14, /* width PARAMETER was specified */
  FLAGS_PREC       = 1 << 15, /* precision was specified */
  FLAGS_PRECPARAM  = 1 << 16, /* precision PARAMETER was specified */
  FLAGS_CHAR       = 1 << 17, /* %c story */
  FLAGS_FLOATE     = 1 << 18, /* %e or %E */
  FLAGS_FLOATG     = 1 << 19, /* %g or %G */
  FLAGS_SUBSTR     = 1 << 20  /* no input, only substring */
};

गणना {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
संरचना va_निवेशः {
  FormatType type; /* FormatType */
  सङ्घ {
    ध्रुव वर्ण *str;
    शून्य *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    द्विगुण dnum;
  } val;
};

/*
 * Describes an output segment.
 */
संरचना outsegment {
  पूर्णाङ्क विस्तारः;     /* width OR width parameter number */
  पूर्णाङ्क precision; /* precision OR precision parameter number */
  अचिह्नित पूर्णाङ्क flags;
  अचिह्नित पूर्णाङ्क निवेशः; /* input argument array index */
  ध्रुव वर्ण *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

संरचना nsprintf {
  वर्ण *बफर;
  size_t दीर्घता;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#परिभाषय ASPRINTF_STAGE_आकार 512

संरचना asprintf {
  संरचना dynbuf *b;
  वर्ण merr;
  size_t nstage;
  अचिह्नित वर्ण stage[ASPRINTF_STAGE_आकार];
};

संरचना ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
स्थिर पूर्णाङ्क dollarstring(ध्रुव वर्ण *p, ध्रुव वर्ण **अन्तः)
{
  curl_off_t अङ्कः;
  यदि(curlx_str_सङ्ख्या(&p, &अङ्कः, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !अङ्कः)
    प्रत्यागम -1;
  *अन्तः = p;
  प्रत्यागम (पूर्णाङ्क)अङ्कः - 1;
}

#परिभाषय is_arg_प्रयुक्तम्(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#परिभाषय mark_arg_प्रयुक्तम्(x, y) ((x)[(y) / 8] |= (अचिह्नित वर्ण)(1 << ((y) & 7)))

/*
 * Parse the format string.
 *
 * Create two arrays. One describes the inputs, one describes the outputs.
 *
 * Returns zero on success.
 */

#परिभाषय PFMT_OK          0
#परिभाषय PFMT_DOLLAR      1 /* bad dollar for main param */
#परिभाषय PFMT_DOLLARWIDTH 2 /* bad dollar use for width */
#परिभाषय PFMT_DOLLARPREC  3 /* bad dollar use for precision */
#परिभाषय PFMT_MANYARGS    4 /* too many input arguments used */
#परिभाषय PFMT_PREC        5 /* precision overflow */
#परिभाषय PFMT_PRECMIX     6 /* bad mix of precision specifiers */
#परिभाषय PFMT_विस्तारः       7 /* width overflow */
#परिभाषय PFMT_INPUTGAP    8 /* gap in arguments */
#परिभाषय PFMT_WIDTHARG    9 /* attempted to use same arg twice, for width */
#परिभाषय PFMT_PRECARG    10 /* attempted to use same arg twice, for prec */
#परिभाषय PFMT_MANYSEGS   11 /* maxed out output segments */

स्थिर पूर्णाङ्क parse_flags(ध्रुव वर्ण **fmtp, अचिह्नित पूर्णाङ्क *flagsp, पूर्णाङ्क use_dollar,
                       पूर्णाङ्क *precp, पूर्णाङ्क *widthp)
{
  ध्रुव वर्ण *fmt = *fmtp;
  bool loopit = TRUE;
  अचिह्नित पूर्णाङ्क flags = 0;
  पूर्णाङ्क विस्तारः = 0;
  पूर्णाङ्क precision = 0;

  /* Handle the flags */
  कुरु {
    विकल्प(*fmt++) {
    पक्ष ' ':
      flags |= FLAGS_SPACE;
      भङ्ग;
    पक्ष '+':
      flags |= FLAGS_SHOWSIGN;
      भङ्ग;
    पक्ष '-':
      flags |= FLAGS_वामः;
      flags &= ~(अचिह्नित पूर्णाङ्क)FLAGS_PAD_NIL;
      भङ्ग;
    पक्ष '#':
      flags |= FLAGS_ALT;
      भङ्ग;
    पक्ष '.':
      यदि('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        यदि(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          यदि(precision < 0)
            /* illegal combo */
            प्रत्यागम PFMT_DOLLARPREC;
        }
        अन्यथा
          /* get it from the next argument */
          precision = -1;
      }
      अन्यथा {
        bool is_neg;
        curl_off_t अङ्कः;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        यदि(is_neg)
          fmt++;
        यदि(curlx_str_सङ्ख्या(&fmt, &अङ्कः, अधिकतम_पूर्णाङ्कः))
          प्रत्यागम PFMT_PREC;
        precision = (पूर्णाङ्क)अङ्कः;
        यदि(is_neg)
          precision = -precision;
      }
      यदि((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
तर्कः*/
        प्रत्यागम PFMT_PRECMIX;
      भङ्ग;
    पक्ष 'h':
      flags |= FLAGS_SHORT;
      भङ्ग;
#यदि_परिभाषितम् _WIN32
    पक्ष 'I':
      /* Non-ANSI integer extensions I32 I64 */
      यदि((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      अन्यथा यदि((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      अन्यथा {
#यदि SIZEOF_आकार_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#अन्यथा
        flags |= FLAGS_LONG;
#यदि_अन्तः
      }
      भङ्ग;
#यदि_अन्तः /* _WIN32 */
    पक्ष 'l':
      यदि(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      अन्यथा
        flags |= FLAGS_LONG;
      भङ्ग;
    पक्ष 'L':
      flags |= FLAGS_LONGDOUBLE;
      भङ्ग;
    पक्ष 'q':
      flags |= FLAGS_LONGLONG;
      भङ्ग;
    पक्ष 'z':
      /* the code below generates a warning if -Wunreachable-code is
प्रयुक्तम्*/
#यदि SIZEOF_आकार_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अन्यथा
      flags |= FLAGS_LONG;
#यदि_अन्तः
      भङ्ग;
    पक्ष 'O':
#यदि SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अन्यथा
      flags |= FLAGS_LONG;
#यदि_अन्तः
      भङ्ग;
    पक्ष '0':
      यदि(!(flags & FLAGS_वामः))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    पक्ष '1':
    पक्ष '2':
    पक्ष '3':
    पक्ष '4':
    पक्ष '5':
    पक्ष '6':
    पक्ष '7':
    पक्ष '8':
    पक्ष '9': {
      curl_off_t अङ्कः;
      flags |= FLAGS_विस्तारः;
      fmt--;
      यदि(curlx_str_सङ्ख्या(&fmt, &अङ्कः, अधिकतम_पूर्णाङ्कः))
        प्रत्यागम PFMT_विस्तारः;
      विस्तारः = (पूर्णाङ्क)अङ्कः;
      भङ्ग;
    }
    पक्ष '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      यदि(use_dollar == DOLLAR_USE) {
        विस्तारः = dollarstring(fmt, &fmt);
        यदि(विस्तारः < 0)
          /* illegal combo */
          प्रत्यागम PFMT_DOLLARWIDTH;
      }
      अन्यथा
        /* pick from the next argument */
        विस्तारः = -1;
      भङ्ग;
    स्वभाव:
      loopit = FALSE;
      fmt--;
      भङ्ग;
    } /* switch */
  } यावत्(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = विस्तारः;
  *fmtp = fmt;
  प्रत्यागम PFMT_OK;
}

स्थिर bool parse_conversion(ध्रुव वर्ण f, अचिह्नित पूर्णाङ्क *flagp,
                             FormatType *typep)
{
  अचिह्नित पूर्णाङ्क flags = *flagp;
  FormatType type;
  विकल्प(f) {
  पक्ष 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    भङ्ग;
  पक्ष 's':
    type = MTYPE_STRING;
    भङ्ग;
  पक्ष 'n':
    type = MTYPE_INTPTR;
    भङ्ग;
  पक्ष 'p':
    type = MTYPE_PTR;
    भङ्ग;
  पक्ष 'd':
  पक्ष 'i':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    अन्यथा
      type = MTYPE_INT;
    भङ्ग;
  पक्ष 'u':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    भङ्ग;
  पक्ष 'o':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    भङ्ग;
  पक्ष 'x':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    भङ्ग;
  पक्ष 'X':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_ऊर्ध्वस्थम् | FLAGS_UNSIGNED;
    भङ्ग;
  पक्ष 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    भङ्ग;
  पक्ष 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    भङ्ग;
  पक्ष 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_ऊर्ध्वस्थम्;
    भङ्ग;
  पक्ष 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    भङ्ग;
  पक्ष 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_ऊर्ध्वस्थम्;
    भङ्ग;
  पक्ष 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    भङ्ग;
  पक्ष 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_ऊर्ध्वस्थम्;
    भङ्ग;
  स्वभाव:
    /* invalid instruction, disregard and continue */
    प्रत्यागम TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  प्रत्यागम FALSE;
}

स्थिर पूर्णाङ्क parsefmt(ध्रुव वर्ण *format,
                    संरचना outsegment *बहिर्गमनम्,
                    संरचना va_निवेशः *अन्तर्,
                    पूर्णाङ्क *opieces,
                    पूर्णाङ्क *ipieces, va_list arglist)
{
  ध्रुव वर्ण *fmt = format;
  पूर्णाङ्क param_अङ्कः = 0;
  पूर्णाङ्क max_param = -1;
  पूर्णाङ्क i;
  पूर्णाङ्क ocount = 0;
  अचिह्नित वर्ण usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  संरचना outsegment *optr;
  पूर्णाङ्क use_dollar = DOLLAR_UNKNOWN;
  ध्रुव वर्ण *start = fmt;

  /* clear, set a bit for each used input */
  स्मृतिं_पूरय(usedinput, 0, परिमाण(usedinput));

  यावत्(*fmt) {
    यदि(*fmt == '%') {
      संरचना va_निवेशः *iptr;
      FormatType type;
      अचिह्नित पूर्णाङ्क flags = 0;
      पूर्णाङ्क विस्तारः = 0;
      पूर्णाङ्क precision = 0;
      पूर्णाङ्क param = -1;
      पूर्णाङ्क rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      यदि(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        यदि(outlen) {
          optr = &बहिर्गमनम्[ocount++];
          यदि(ocount > MAX_SEGMENTS)
            प्रत्यागम PFMT_MANYSEGS;
          optr->निवेशः = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        अनुवर्त; /* while */
      }

      यदि(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        यदि(param < 0) {
          यदि(use_dollar == DOLLAR_USE)
            /* illegal combo */
            प्रत्यागम PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        अन्यथा
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &विस्तारः);
      यदि(rc)
        प्रत्यागम rc;

      यदि(parse_conversion(*fmt, &flags, &type))
        अनुवर्त;

      यदि(flags & FLAGS_WIDTHPARAM) {
        यदि(विस्तारः < 0)
          विस्तारः = param_अङ्कः++;
        अन्यथा {
          /* if this identifies a parameter already used, this is illegal */
          यदि(is_arg_प्रयुक्तम्(usedinput, विस्तारः))
            प्रत्यागम PFMT_WIDTHARG;
        }
        यदि(विस्तारः >= MAX_PARAMETERS)
          प्रत्यागम PFMT_MANYARGS;
        यदि(विस्तारः >= max_param)
          max_param = विस्तारः;

        अन्तर्[विस्तारः].type = MTYPE_विस्तारः;
        /* mark as used */
        mark_arg_प्रयुक्तम्(usedinput, विस्तारः);
      }

      यदि(flags & FLAGS_PRECPARAM) {
        यदि(precision < 0)
          precision = param_अङ्कः++;
        अन्यथा {
          /* if this identifies a parameter already used, this is illegal */
          यदि(is_arg_प्रयुक्तम्(usedinput, precision))
            प्रत्यागम PFMT_PRECARG;
        }
        यदि(precision >= MAX_PARAMETERS)
          प्रत्यागम PFMT_MANYARGS;
        यदि(precision >= max_param)
          max_param = precision;

        अन्तर्[precision].type = MTYPE_PRECISION;
        mark_arg_प्रयुक्तम्(usedinput, precision);
      }

      /* Handle the specifier */
      यदि(param < 0)
        param = param_अङ्कः++;
      यदि(param >= MAX_PARAMETERS)
        प्रत्यागम PFMT_MANYARGS;
      यदि(param >= max_param)
        max_param = param;

      iptr = &अन्तर्[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_प्रयुक्तम्(usedinput, param);

      fmt++;
      optr = &बहिर्गमनम्[ocount++];
      यदि(ocount > MAX_SEGMENTS)
        प्रत्यागम PFMT_MANYSEGS;
      optr->निवेशः = (अचिह्नित पूर्णाङ्क)param;
      optr->flags = flags;
      optr->विस्तारः = विस्तारः;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    अन्यथा
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  यदि(outlen) {
    optr = &बहिर्गमनम्[ocount++];
    यदि(ocount > MAX_SEGMENTS)
      प्रत्यागम PFMT_MANYSEGS;
    optr->निवेशः = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  प्रति(i = 0; i < max_param + 1; i++) {
    संरचना va_निवेशः *iptr = &अन्तर्[i];
    यदि(!is_arg_प्रयुक्तम्(usedinput, i))
      /* bad input */
      प्रत्यागम PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    विकल्प(iptr->type) {
    पक्ष MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ध्रुव वर्ण *);
      भङ्ग;

    पक्ष MTYPE_INTPTR:
    पक्ष MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, शून्य *);
      भङ्ग;

    पक्ष MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      भङ्ग;

    पक्ष MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      भङ्ग;

    पक्ष MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, अचिह्नित दीर्घ);
      भङ्ग;

    पक्ष MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, दीर्घ);
      भङ्ग;

    पक्ष MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, अचिह्नित पूर्णाङ्क);
      भङ्ग;

    पक्ष MTYPE_INT:
    पक्ष MTYPE_विस्तारः:
    पक्ष MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, पूर्णाङ्क);
      भङ्ग;

    पक्ष MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, द्विगुण);
      भङ्ग;

    पक्ष MTYPE_LONGDOUBLE:
      iptr->val.dnum = (द्विगुण)va_arg(arglist, दीर्घ द्विगुण);
      भङ्ग;

    स्वभाव:
      DEBUGASSERT(रिक्त); /* unexpected */
      भङ्ग;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  प्रत्यागम PFMT_OK;
}

संरचना mproperty {
  पूर्णाङ्क विस्तारः;            /* Width of a field. */
  पूर्णाङ्क prec;             /* Precision of a field. */
  अचिह्नित पूर्णाङ्क flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
प्रकारनिर्देश size_t (*mp_streamn)(ध्रुव अचिह्नित वर्ण *buf, size_t len, शून्य *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
प्रकारनिर्देश पूर्णाङ्क (*mp_flush)(शून्य *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
स्थिर bool प्रवाहः_सततम्(शून्य *userp,
                       पूर्णाङ्क (*प्रवाहः)(अचिह्नित वर्ण, शून्य *),
                       mp_streamn streamn,
                       ध्रुव अचिह्नित वर्ण *buf, size_t len, पूर्णाङ्क *donep)
{
  यदि(!len)
    प्रत्यागम FALSE;
  यदि(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (पूर्णाङ्क)n;
    प्रत्यागम n != len;
  }
  यावत्(len--) {
    यदि(प्रवाहः(*buf++, userp))
      प्रत्यागम TRUE;
    (*donep)++;
  }
  प्रत्यागम FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
स्थिर ध्रुव वर्ण pad_spaces[] = "                ";
स्थिर ध्रुव वर्ण pad_zeros[] = "0000000000000000";
#परिभाषय PAD_सततम् 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
स्थिर bool प्रवाहः_pad(शून्य *userp,
                       पूर्णाङ्क (*प्रवाहः)(अचिह्नित वर्ण, शून्य *),
                       mp_streamn streamn,
                       अचिह्नित वर्ण pad, पूर्णाङ्क अङ्कः, पूर्णाङ्क *donep)
{
  ध्रुव अचिह्नित वर्ण *src;
  यदि(अङ्कः <= 0)
    प्रत्यागम FALSE;
  यदि(pad == '0')
    src = (ध्रुव अचिह्नित वर्ण *)pad_zeros;
  अन्यथा
    src = (ध्रुव अचिह्नित वर्ण *)pad_spaces;
  यावत्(अङ्कः > PAD_सततम्) {
    यदि(प्रवाहः_सततम्(userp, प्रवाहः, streamn, src, PAD_सततम्, donep))
      प्रत्यागम TRUE;
    अङ्कः -= PAD_सततम्;
  }
  प्रत्यागम प्रवाहः_सततम्(userp, प्रवाहः, streamn, src, (size_t)अङ्कः, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
स्थिर bool प्रवाहः_zrun(शून्य *userp,
                        पूर्णाङ्क (*प्रवाहः)(अचिह्नित वर्ण, शून्य *),
                        mp_streamn streamn,
                        ध्रुव वर्ण *str, size_t len, पूर्णाङ्क *donep)
{
  size_t n = 0;
  यावत्(n < len && str[n])
    n++;
  प्रत्यागम प्रवाहः_सततम्(userp, प्रवाहः, streamn,
                    (ध्रुव अचिह्नित वर्ण *)str, n, donep);
}

स्थिर bool बहिर्गमनम्_double(शून्य *userp,
                       पूर्णाङ्क (*प्रवाहः)(अचिह्नित वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       द्विगुण dnum,
                       वर्ण *work, पूर्णाङ्क *donep)
{
  वर्ण fmt[32] = "%";
  वर्ण *fptr = &fmt[1];
  size_t वामः = परिमाण(fmt) - वाक्यदैर्घ्यम्(fmt);
  पूर्णाङ्क flags = p->flags;
  पूर्णाङ्क विस्तारः = p->विस्तारः;
  पूर्णाङ्क prec = p->prec;

  यदि(flags & FLAGS_वामः)
    *fptr++ = '-';
  यदि(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  यदि(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  यदि(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  यदि(विस्तारः >= 0) {
    size_t dlen;
    यदि(विस्तारः >= BUFFSIZE)
      विस्तारः = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, वामः, "%d", विस्तारः);
    fptr += dlen;
    वामः -= dlen;
  }
  यदि(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    पूर्णाङ्क maxprec = BUFFSIZE - 1;
    द्विगुण val = dnum;
    पूर्णाङ्क len;
    यदि(prec > maxprec)
      prec = maxprec - 1;
    यदि(विस्तारः > 0 && prec <= विस्तारः)
      maxprec -= विस्तारः;
    यावत्(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    यदि(prec > maxprec)
      prec = maxprec - 1;
    यदि(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, वामः, ".%d", prec);
    fptr += len;
  }
  यदि(flags & FLAGS_LONG)
    *fptr++ = 'l';

  यदि(flags & FLAGS_FLOATE)
    *fptr++ = (वर्ण)((flags & FLAGS_ऊर्ध्वस्थम्) ? 'E' : 'e');
  अन्यथा यदि(flags & FLAGS_FLOATG)
    *fptr++ = (वर्ण)((flags & FLAGS_ऊर्ध्वस्थम्) ? 'G' : 'g');
  अन्यथा
    *fptr++ = (flags & FLAGS_ऊर्ध्वस्थम्) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wformat-nonliteral"
#यदि_अन्तः
#यदि_परिभाषितम् _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#यदि_अन्तः
#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
  DEBUGASSERT(वाक्यदैर्घ्यम्(work) < BUFFSIZE);
  प्रत्यागम प्रवाहः_सततम्(userp, प्रवाहः, streamn,
                    (ध्रुव अचिह्नित वर्ण *)work, वाक्यदैर्घ्यम्(work), donep);
}

स्थिर bool बहिर्गमनम्_सङ्ख्या(शून्य *userp,
                       पूर्णाङ्क (*प्रवाहः)(अचिह्नित वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       uint64_t अङ्कः,
                       int64_t nums,
                       वर्ण *work, पूर्णाङ्क *donep)
{
  ध्रुव अचिह्नित वर्ण *digits = Curl_ldigits;
  पूर्णाङ्क flags = p->flags;
  पूर्णाङ्क विस्तारः = p->विस्तारः;
  पूर्णाङ्क prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  पूर्णाङ्क आधारः = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  वर्ण *workend = &work[BUFFSIZE - 2];
  वर्ण *w;

  यदि(flags & FLAGS_CHAR) {
    /* Character. */
    यदि(!(flags & FLAGS_वामः)) {
      यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, ' ', विस्तारः - 1, donep))
        प्रत्यागम TRUE;
      विस्तारः = 1;
    }
    OUTCHAR((वर्ण)अङ्कः);
    यदि(flags & FLAGS_वामः) {
      यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, ' ', विस्तारः - 1, donep))
        प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
  }
  यदि(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    आधारः = 8;

  अन्यथा यदि(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_ऊर्ध्वस्थम्) ? Curl_udigits : Curl_ldigits;
    आधारः = 16;
  }
  अन्यथा यदि(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  अन्यथा {
    /* Decimal integer. */
    is_neg = (nums < 0);
    यदि(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_अङ्कः; /* Used to convert negative in positive. */
      signed_अङ्कः = nums + (int64_t)1;
      signed_अङ्कः = -signed_अङ्कः;
      अङ्कः = (uint64_t)signed_अङ्कः;
      अङ्कः += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  यदि(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(आधारः <= 16);
  विकल्प(आधारः) {
  पक्ष 10:
    यावत्(अङ्कः > 0) {
      *w-- = (वर्ण)('0' + (अङ्कः % 10));
      अङ्कः /= 10;
    }
    भङ्ग;
  स्वभाव:
    यावत्(अङ्कः > 0) {
      *w-- = digits[अङ्कः % आधारः];
      अङ्कः /= आधारः;
    }
    भङ्ग;
  }
  विस्तारः -= (पूर्णाङ्क)(workend - w);
  prec -= (पूर्णाङ्क)(workend - w);

  यदि(is_alt && आधारः == 8 && prec <= 0) {
    *w-- = '0';
    --विस्तारः;
  }

  यदि(prec > 0) {
    विस्तारः -= prec;
    यावत्(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  यदि(is_alt && आधारः == 16)
    विस्तारः -= 2;

  यदि(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --विस्तारः;

  यदि(!(flags & FLAGS_वामः) && !(flags & FLAGS_PAD_NIL)) {
    यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, ' ', विस्तारः, donep))
      प्रत्यागम TRUE;
    विस्तारः = 0;
  }

  यदि(is_neg)
    OUTCHAR('-');
  अन्यथा यदि(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  अन्यथा यदि(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  यदि(is_alt && आधारः == 16) {
    OUTCHAR('0');
    यदि(flags & FLAGS_ऊर्ध्वस्थम्)
      OUTCHAR('X');
    अन्यथा
      OUTCHAR('x');
  }

  यदि(!(flags & FLAGS_वामः) && (flags & FLAGS_PAD_NIL)) {
    यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, '0', विस्तारः, donep))
      प्रत्यागम TRUE;
    विस्तारः = 0;
  }

  /* Write the number. */
  यदि(प्रवाहः_सततम्(userp, प्रवाहः, streamn, (ध्रुव अचिह्नित वर्ण *)w + 1,
                (size_t)(workend - w), donep))
    प्रत्यागम TRUE;

  यदि(flags & FLAGS_वामः) {
    यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, ' ', विस्तारः, donep))
      प्रत्यागम TRUE;
  }

  प्रत्यागम FALSE;
}

स्थिर ध्रुव वर्ण nilstr[] = "(nil)";

स्थिर bool बहिर्गमनम्_string(शून्य *userp,
                       पूर्णाङ्क (*प्रवाहः)(अचिह्नित वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       ध्रुव वर्ण *str,
                       पूर्णाङ्क *donep)
{
  पूर्णाङ्क flags = p->flags;
  पूर्णाङ्क विस्तारः = p->विस्तारः;
  पूर्णाङ्क prec = p->prec;
  size_t len;

  यदि(!str) {
    /* Write null string if there is space. */
    यदि(prec == -1 || prec >= (पूर्णाङ्क)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(अचिह्नित पूर्णाङ्क)FLAGS_ALT;
    }
    अन्यथा {
      str = "";
      len = 0;
    }
  }
  अन्यथा यदि(prec != -1)
    len = (size_t)prec;
  अन्यथा यदि(*str == '\0')
    len = 0;
  अन्यथा
    len = वाक्यदैर्घ्यम्(str);

  विस्तारः -= (len > अधिकतम_पूर्णाङ्कः) ? अधिकतम_पूर्णाङ्कः : (पूर्णाङ्क)len;

  यदि(flags & FLAGS_ALT)
    OUTCHAR('"');

  यदि(!(flags & FLAGS_वामः)) {
    यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, ' ', विस्तारः, donep))
      प्रत्यागम TRUE;
    विस्तारः = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  यदि(prec == -1) {
    यदि(प्रवाहः_सततम्(userp, प्रवाहः, streamn,
                  (ध्रुव अचिह्नित वर्ण *)str, len, donep))
      प्रत्यागम TRUE;
  }
  अन्यथा यदि(प्रवाहः_zrun(userp, प्रवाहः, streamn, str, len, donep))
    प्रत्यागम TRUE;
  यदि(flags & FLAGS_वामः) {
    यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, ' ', विस्तारः, donep))
      प्रत्यागम TRUE;
  }

  यदि(flags & FLAGS_ALT)
    OUTCHAR('"');

  प्रत्यागम FALSE;
}

स्थिर bool बहिर्गमनम्_pointer(शून्य *userp,
                        पूर्णाङ्क (*प्रवाहः)(अचिह्नित वर्ण, शून्य *),
                        mp_streamn streamn,
                        संरचना mproperty *p,
                        ध्रुव वर्ण *ptr,
                        वर्ण *work,
                        पूर्णाङ्क *donep)
{
  /* Generic pointer. */
  यदि(ptr) {
    size_t अङ्कः = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    यदि(बहिर्गमनम्_सङ्ख्या(userp, प्रवाहः, streamn, p, अङ्कः, 0, work, donep))
      प्रत्यागम TRUE;
  }
  अन्यथा {
    /* Write "(nil)" for a nil pointer. */
    पूर्णाङ्क विस्तारः = p->विस्तारः;
    पूर्णाङ्क flags = p->flags;

    विस्तारः -= (पूर्णाङ्क)CURL_CSTRLEN(nilstr);
    यदि(flags & FLAGS_वामः) {
      यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, ' ', विस्तारः, donep))
        प्रत्यागम TRUE;
      विस्तारः = 0;
    }
    यदि(प्रवाहः_सततम्(userp, प्रवाहः, streamn, (ध्रुव अचिह्नित वर्ण *)nilstr,
                  परिमाण(nilstr) - 1, donep))
      प्रत्यागम TRUE;
    यदि(!(flags & FLAGS_वामः)) {
      यदि(प्रवाहः_pad(userp, प्रवाहः, streamn, ' ', विस्तारः, donep))
        प्रत्यागम TRUE;
    }
  }
  प्रत्यागम FALSE;
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
स्थिर पूर्णाङ्क formatf(शून्य *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   पूर्णाङ्क (*प्रवाहः)(अचिह्नित वर्ण, शून्य *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ध्रुव वर्ण *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  पूर्णाङ्क समाप्तम् = 0;   /* number of characters written */
  पूर्णाङ्क i;
  पूर्णाङ्क ocount = 0; /* number of output segments */
  पूर्णाङ्क icount = 0; /* number of input arguments */

  संरचना outsegment output[MAX_SEGMENTS];
  संरचना va_निवेशः निवेशः[MAX_PARAMETERS];
  वर्ण work[BUFFSIZE + 2];

  /* Parse the format string */
  यदि(parsefmt(format, output, निवेशः, &ocount, &icount, ap_save))
    प्रत्यागम 0;

  प्रति(i = 0; i < ocount; i++) {
    संरचना outsegment *optr = &output[i];
    संरचना va_निवेशः *iptr = &निवेशः[optr->निवेशः];
    संरचना mproperty p;
    size_t outlen = optr->outlen;

    यदि(outlen) {
      यदि(प्रवाहः_zrun(userp, प्रवाहः, streamn, optr->start, outlen, &समाप्तम्))
        प्रत्यागम समाप्तम्;
      यदि(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        अनुवर्त;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    यदि(p.flags & FLAGS_WIDTHPARAM) {
      p.विस्तारः = (पूर्णाङ्क)निवेशः[optr->विस्तारः].val.nums;
      यदि(p.विस्तारः < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        यदि(p.विस्तारः == INT_MIN)
          p.विस्तारः = अधिकतम_पूर्णाङ्कः;
        अन्यथा
          p.विस्तारः = -p.विस्तारः;
        p.flags |= FLAGS_वामः;
        p.flags &= ~(अचिह्नित पूर्णाङ्क)FLAGS_PAD_NIL;
      }
    }
    अन्यथा
      p.विस्तारः = optr->विस्तारः;

    /* pick up the specified precision */
    यदि(p.flags & FLAGS_PRECPARAM) {
      p.prec = (पूर्णाङ्क)निवेशः[optr->precision].val.nums;
      यदि(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    अन्यथा यदि(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    अन्यथा
      p.prec = -1;

    विकल्प(iptr->type) {
    पक्ष MTYPE_INTU:
    पक्ष MTYPE_LONGU:
    पक्ष MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      यदि(बहिर्गमनम्_सङ्ख्या(userp, प्रवाहः, streamn, &p, iptr->val.numu, 0, work,
                    &समाप्तम्))
        प्रत्यागम समाप्तम्;
      भङ्ग;

    पक्ष MTYPE_INT:
    पक्ष MTYPE_LONG:
    पक्ष MTYPE_LONGLONG:
      यदि(बहिर्गमनम्_सङ्ख्या(userp, प्रवाहः, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &समाप्तम्))
        प्रत्यागम समाप्तम्;
      भङ्ग;

    पक्ष MTYPE_STRING:
      यदि(बहिर्गमनम्_string(userp, प्रवाहः, streamn, &p, iptr->val.str, &समाप्तम्))
        प्रत्यागम समाप्तम्;
      भङ्ग;

    पक्ष MTYPE_PTR:
      यदि(बहिर्गमनम्_pointer(userp, प्रवाहः, streamn, &p, iptr->val.ptr, work,
                     &समाप्तम्))
        प्रत्यागम समाप्तम्;
      भङ्ग;

    पक्ष MTYPE_DOUBLE:
    पक्ष MTYPE_LONGDOUBLE:
      यदि(बहिर्गमनम्_double(userp, प्रवाहः, streamn, &p, iptr->val.dnum, work,
                    &समाप्तम्))
        प्रत्यागम समाप्तम्;
      भङ्ग;

    पक्ष MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      यदि(flush && flush(userp))
        प्रत्यागम समाप्तम्;
      /* Answer the count of characters written. */
      यदि(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)समाप्तम्;
      अन्यथा यदि(p.flags & FLAGS_LONG)
        *(दीर्घ *)iptr->val.ptr = (दीर्घ)समाप्तम्;
      अन्यथा यदि(!(p.flags & FLAGS_SHORT))
        *(पूर्णाङ्क *)iptr->val.ptr = समाप्तम्;
      अन्यथा
        *(ह्रस्व *)iptr->val.ptr = (ह्रस्व)समाप्तम्;
      भङ्ग;

    स्वभाव:
      भङ्ग;
    }
  }
  प्रत्यागम समाप्तम्;
}

/* fputc() look-alike */
स्थिर पूर्णाङ्क addbyter(अचिह्नित वर्ण outc, शून्य *f)
{
  संरचना nsprintf *infop = f;
  यदि(infop->दीर्घता < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->बफर++ = (वर्ण)outc; /* store */
    infop->दीर्घता++; /* we are now one byte larger */
    प्रत्यागम 0;     /* fputc() returns like this on success */
  }
  प्रत्यागम 1;
}

/* block variant of addbyter */
स्थिर size_t addrun(ध्रुव अचिह्नित वर्ण *buf, size_t len, शून्य *f)
{
  संरचना nsprintf *infop = f;
  size_t fit = infop->max - infop->दीर्घता;
  यदि(len > fit)
    len = fit;
  यदि(len) {
    स्मृतिं_प्रतिलिख(infop->बफर, buf, len);
    infop->बफर += len;
    infop->दीर्घता += len;
  }
  प्रत्यागम len;
}

पूर्णाङ्क curl_mvsnprintf(वर्ण *बफर, size_t maxlength, ध्रुव वर्ण *format,
                    va_list तर्काः)
{
  पूर्णाङ्क retcode;
  संरचना nsprintf info;

  info.बफर = बफर;
  info.दीर्घता = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, रिक्त, format, तर्काः);
  यदि(info.max) {
    /* we null-terminate this */
    यदि(info.max == info.दीर्घता) {
      /* we are at maximum, scrap the last letter */
      info.बफर[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    अन्यथा
      info.बफर[0] = 0;
  }
  प्रत्यागम retcode;
}

पूर्णाङ्क curl_msnprintf(वर्ण *बफर, size_t maxlength, ध्रुव वर्ण *format, ...)
{
  पूर्णाङ्क retcode;
  va_list तर्काः; /* argument pointer */
  va_start(तर्काः, format);
  retcode = curl_mvsnprintf(बफर, maxlength, format, तर्काः);
  va_end(तर्काः);
  प्रत्यागम retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
स्थिर पूर्णाङ्क alloc_flush(शून्य *f)
{
  संरचना asprintf *infop = f;
  यदि(infop->nstage) {
    CURLcode परिणाम = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    यदि(परिणाम) {
      infop->merr = परिणाम == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      प्रत्यागम 1; /* fail */
    }
  }
  प्रत्यागम 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
स्थिर पूर्णाङ्क alloc_addn(संरचना asprintf *infop, ध्रुव अचिह्नित वर्ण *buf,
                      size_t len)
{
  CURLcode परिणाम = curlx_dyn_addn(infop->b, buf, len);
  यदि(परिणाम) {
    infop->merr = परिणाम == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    प्रत्यागम 1; /* fail */
  }
  प्रत्यागम 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
स्थिर bool alloc_माति(संरचना asprintf *infop, size_t अधिकम्)
{
  प्रत्यागम (infop->b->leng + infop->nstage + अधिकम् + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
स्थिर पूर्णाङ्क alloc_addbyter(अचिह्नित वर्ण outc, शून्य *f)
{
  संरचना asprintf *infop = f;
  यदि(infop->nstage == परिमाण(infop->stage) && alloc_flush(infop))
    प्रत्यागम 1; /* fail */
  यदि(!alloc_माति(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    यदि(alloc_flush(infop))
      प्रत्यागम 1;
    प्रत्यागम alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  प्रत्यागम 0;
}

/* block variant of alloc_addbyter */
स्थिर size_t alloc_addrun(ध्रुव अचिह्नित वर्ण *buf, size_t len, शून्य *f)
{
  संरचना asprintf *infop = f;
  size_t accepted = 0;
  यावत्(accepted < len) {
    size_t अवकाशः = परिमाण(infop->stage) - infop->nstage;
    size_t n;
    यदि(!अवकाशः) {
      यदि(alloc_flush(infop))
        प्रत्यागम accepted; /* accepted < len aborts formatting */
      अवकाशः = परिमाण(infop->stage);
    }
    n = len - accepted;
    यदि(n > अवकाशः)
      n = अवकाशः;
    यदि(!alloc_माति(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t माति = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      यदि(माति) {
        स्मृतिं_प्रतिलिख(&infop->stage[infop->nstage], buf + accepted, माति);
        infop->nstage += माति;
        accepted += माति;
      }
      यदि(alloc_flush(infop))
        प्रत्यागम accepted;
      यदि(alloc_addn(infop, buf + accepted, 1))
        प्रत्यागम accepted; /* accepted < len aborts formatting */
      accepted++;
      अनुवर्त;
    }
    स्मृतिं_प्रतिलिख(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  प्रत्यागम accepted;
}

/* appends the formatted string, returns MERR error code */
पूर्णाङ्क curlx_dyn_vprintf(संरचना dynbuf *dyn, ध्रुव वर्ण *format, va_list तर्काः)
{
  संरचना asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (शून्य)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, तर्काः);
  यदि(!info.merr)
    (शून्य)alloc_flush(&info);
  यदि(info.merr) {
    curlx_dyn_free(info.b);
    प्रत्यागम info.merr;
  }
  प्रत्यागम 0;
}

वर्ण *curl_mvaprintf(ध्रुव वर्ण *format, va_list तर्काः)
{
  संरचना asprintf info;
  संरचना dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (शून्य)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, तर्काः);
  यदि(!info.merr)
    (शून्य)alloc_flush(&info);
  यदि(info.merr) {
    curlx_dyn_free(info.b);
    प्रत्यागम रिक्त;
  }
  यदि(curlx_dyn_len(info.b))
    प्रत्यागम curlx_dyn_ptr(info.b);
  प्रत्यागम curlx_strdup("");
}

वर्ण *curl_maprintf(ध्रुव वर्ण *format, ...)
{
  va_list तर्काः;
  वर्ण *s;
  va_start(तर्काः, format);
  s = curl_mvaprintf(format, तर्काः);
  va_end(तर्काः);
  प्रत्यागम s;
}

स्थिर पूर्णाङ्क storebuffer(अचिह्नित वर्ण outc, शून्य *f)
{
  वर्ण **बफर = f;
  **बफर = (वर्ण)outc;
  (*बफर)++;
  प्रत्यागम 0;
}

/* block variant of storebuffer */
स्थिर size_t storerun(ध्रुव अचिह्नित वर्ण *buf, size_t len, शून्य *f)
{
  वर्ण **बफर = f;
  स्मृतिं_प्रतिलिख(*बफर, buf, len);
  *बफर += len;
  प्रत्यागम len;
}

पूर्णाङ्क curl_msprintf(वर्ण *बफर, ध्रुव वर्ण *format, ...)
{
  va_list तर्काः; /* argument pointer */
  पूर्णाङ्क retcode;
  va_start(तर्काः, format);
  retcode = formatf(&बफर, storebuffer, storerun, रिक्त, format, तर्काः);
  va_end(तर्काः);
  *बफर = 0; /* we null-terminate this */
  प्रत्यागम retcode;
}

स्थिर पूर्णाङ्क fputc_wrapper(अचिह्नित वर्ण outc, शून्य *f)
{
  संरचना ioprintf *i = f;
  पूर्णाङ्क बहिर्गमनम् = outc;
  FILE *s = i->fd;
  पूर्णाङ्क rc = fputc(बहिर्गमनम्, s);
  i->error = (rc == EOF);
  प्रत्यागम (rc == EOF);
}

/* block variant of fputc_wrapper */
स्थिर size_t fwrite_wrapper(ध्रुव अचिह्नित वर्ण *buf, size_t len, शून्य *f)
{
  संरचना ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  प्रत्यागम n;
}

पूर्णाङ्क curl_mvfprintf(FILE *fd, ध्रुव वर्ण *format, va_list तर्काः)
{
  संरचना ioprintf info;
  पूर्णाङ्क n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, रिक्त, format, तर्काः);
  यदि(info.error)
    प्रत्यागम -1;
  प्रत्यागम n;
}

पूर्णाङ्क curl_mfprintf(FILE *fd, ध्रुव वर्ण *format, ...)
{
  va_list तर्काः; /* argument pointer */
  पूर्णाङ्क n;
  va_start(तर्काः, format);
  n = curl_mvfprintf(fd, format, तर्काः);
  va_end(तर्काः);
  प्रत्यागम n;
}

पूर्णाङ्क curl_mprintf(ध्रुव वर्ण *format, ...)
{
  va_list तर्काः; /* argument pointer */
  पूर्णाङ्क n;
  va_start(तर्काः, format);
  n = curl_mvfprintf(stdout, format, तर्काः);
  va_end(तर्काः);
  प्रत्यागम n;
}

पूर्णाङ्क curl_mvsprintf(वर्ण *बफर, ध्रुव वर्ण *format, va_list तर्काः)
{
  पूर्णाङ्क retcode = formatf(&बफर, storebuffer, storerun, रिक्त, format, तर्काः);
  *बफर = 0; /* we null-terminate this */
  प्रत्यागम retcode;
}

पूर्णाङ्क curl_mvprintf(ध्रुव वर्ण *format, va_list तर्काः)
{
  प्रत्यागम curl_mvfprintf(stdout, format, तर्काः);
}

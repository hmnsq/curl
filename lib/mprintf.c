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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "curl_printf.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#నిర్వచించు BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#నిర్వచించు MAX_PARAMETERS 128 /* number of input arguments */
#నిర్వచించు MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
స్థిరరాశి గుర్తులేని అక్షరం Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
స్థిరరాశి గుర్తులేని అక్షరం Curl_udigits[] = "0123456789ABCDEF";

#నిర్వచించు OUTCHAR(x)                        \
  చేయి {                                    \
    ఒకవేళ(ప్రవాహం((గుర్తులేని అక్షరం)(x), userp)) \
      తిరిగి TRUE;                        \
    (*donep)++;                           \
  } అయితే(0)

/* Data type to read from the arglist */
రకనిర్వచనం గణన {
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
  MTYPE_వెడల్పు,
  MTYPE_PRECISION
} FormatType;

/* conversion and display flags */
గణన {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_ఎడమ       = 1 << 2,
  FLAGS_ALT        = 1 << 3,
  FLAGS_SHORT      = 1 << 4,
  FLAGS_LONG       = 1 << 5,
  FLAGS_LONGLONG   = 1 << 6,
  FLAGS_LONGDOUBLE = 1 << 7,
  FLAGS_PAD_NIL    = 1 << 8,
  FLAGS_UNSIGNED   = 1 << 9,
  FLAGS_OCTAL      = 1 << 10,
  FLAGS_HEX        = 1 << 11,
  FLAGS_పై      = 1 << 12,
  FLAGS_వెడల్పు      = 1 << 13, /* '*' or '*<num>$' used */
  FLAGS_WIDTHPARAM = 1 << 14, /* width PARAMETER was specified */
  FLAGS_PREC       = 1 << 15, /* precision was specified */
  FLAGS_PRECPARAM  = 1 << 16, /* precision PARAMETER was specified */
  FLAGS_CHAR       = 1 << 17, /* %c story */
  FLAGS_FLOATE     = 1 << 18, /* %e or %E */
  FLAGS_FLOATG     = 1 << 19, /* %g or %G */
  FLAGS_SUBSTR     = 1 << 20  /* no input, only substring */
};

గణన {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
నిర్మాణం va_ఆదానం {
  FormatType type; /* FormatType */
  సంఘం {
    స్థిరరాశి అక్షరం *str;
    శూన్య *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    ద్విగుణ dnum;
  } val;
};

/*
 * Describes an output segment.
 */
నిర్మాణం outsegment {
  పూర్ణాంకం వెడల్పు;     /* width OR width parameter number */
  పూర్ణాంకం precision; /* precision OR precision parameter number */
  గుర్తులేని పూర్ణాంకం flags;
  గుర్తులేని పూర్ణాంకం ఆదానం; /* input argument array index */
  స్థిరరాశి అక్షరం *ప్రారంభం; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

నిర్మాణం nsprintf {
  అక్షరం *బఫరు;
  size_t నిడివి;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#నిర్వచించు ASPRINTF_STAGE_కొలత 512

నిర్మాణం asprintf {
  నిర్మాణం dynbuf *b;
  అక్షరం merr;
  size_t nstage;
  గుర్తులేని అక్షరం stage[ASPRINTF_STAGE_కొలత];
};

నిర్మాణం ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
స్థిర పూర్ణాంకం dollarstring(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం **చివర)
{
  curl_off_t అంకె;
  ఒకవేళ(curlx_str_సంఖ్య(&p, &అంకె, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !అంకె)
    తిరిగి -1;
  *చివర = p;
  తిరిగి (పూర్ణాంకం)అంకె - 1;
}

#నిర్వచించు is_arg_వాడిన(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#నిర్వచించు mark_arg_వాడిన(x, y) ((x)[(y) / 8] |= (గుర్తులేని అక్షరం)(1 << ((y) & 7)))

/*
 * Parse the format string.
 *
 * Create two arrays. One describes the inputs, one describes the outputs.
 *
 * Returns zero on success.
 */

#నిర్వచించు PFMT_OK          0
#నిర్వచించు PFMT_DOLLAR      1 /* bad dollar for main param */
#నిర్వచించు PFMT_DOLLARWIDTH 2 /* bad dollar use for width */
#నిర్వచించు PFMT_DOLLARPREC  3 /* bad dollar use for precision */
#నిర్వచించు PFMT_MANYARGS    4 /* too many input arguments used */
#నిర్వచించు PFMT_PREC        5 /* precision overflow */
#నిర్వచించు PFMT_PRECMIX     6 /* bad mix of precision specifiers */
#నిర్వచించు PFMT_వెడల్పు       7 /* width overflow */
#నిర్వచించు PFMT_INPUTGAP    8 /* gap in arguments */
#నిర్వచించు PFMT_WIDTHARG    9 /* attempted to use same arg twice, for width */
#నిర్వచించు PFMT_PRECARG    10 /* attempted to use same arg twice, for prec */
#నిర్వచించు PFMT_MANYSEGS   11 /* maxed out output segments */

స్థిర పూర్ణాంకం parse_flags(స్థిరరాశి అక్షరం **fmtp, గుర్తులేని పూర్ణాంకం *flagsp, పూర్ణాంకం use_dollar,
                       పూర్ణాంకం *precp, పూర్ణాంకం *widthp)
{
  స్థిరరాశి అక్షరం *fmt = *fmtp;
  bool loopit = TRUE;
  గుర్తులేని పూర్ణాంకం flags = 0;
  పూర్ణాంకం వెడల్పు = 0;
  పూర్ణాంకం precision = 0;

  /* Handle the flags */
  చేయి {
    ఎంపిక(*fmt++) {
    స్థితి ' ':
      flags |= FLAGS_SPACE;
      ఆపు;
    స్థితి '+':
      flags |= FLAGS_SHOWSIGN;
      ఆపు;
    స్థితి '-':
      flags |= FLAGS_ఎడమ;
      flags &= ~(గుర్తులేని పూర్ణాంకం)FLAGS_PAD_NIL;
      ఆపు;
    స్థితి '#':
      flags |= FLAGS_ALT;
      ఆపు;
    స్థితి '.':
      ఒకవేళ('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ఒకవేళ(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ఒకవేళ(precision < 0)
            /* illegal combo */
            తిరిగి PFMT_DOLLARPREC;
        }
        లేకపోతే
          /* get it from the next argument */
          precision = -1;
      }
      లేకపోతే {
        bool is_neg;
        curl_off_t అంకె;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ఒకవేళ(is_neg)
          fmt++;
        ఒకవేళ(curlx_str_సంఖ్య(&fmt, &అంకె, గరిష్ఠ_పూర్ణాంకం))
          తిరిగి PFMT_PREC;
        precision = (పూర్ణాంకం)అంకె;
        ఒకవేళ(is_neg)
          precision = -precision;
      }
      ఒకవేళ((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
వాదం*/
        తిరిగి PFMT_PRECMIX;
      ఆపు;
    స్థితి 'h':
      flags |= FLAGS_SHORT;
      ఆపు;
#నిర్వచితమైతే _WIN32
    స్థితి 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ఒకవేళ((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      లేకపోతే ఒకవేళ((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      లేకపోతే {
#ఒకవేళ SIZEOF_కొలత_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#లేకపోతే
        flags |= FLAGS_LONG;
#అయితే_ముగింపు
      }
      ఆపు;
#అయితే_ముగింపు /* _WIN32 */
    స్థితి 'l':
      ఒకవేళ(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      లేకపోతే
        flags |= FLAGS_LONG;
      ఆపు;
    స్థితి 'L':
      flags |= FLAGS_LONGDOUBLE;
      ఆపు;
    స్థితి 'q':
      flags |= FLAGS_LONGLONG;
      ఆపు;
    స్థితి 'z':
      /* the code below generates a warning if -Wunreachable-code is
వాడిన*/
#ఒకవేళ SIZEOF_కొలత_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#లేకపోతే
      flags |= FLAGS_LONG;
#అయితే_ముగింపు
      ఆపు;
    స్థితి 'O':
#ఒకవేళ SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#లేకపోతే
      flags |= FLAGS_LONG;
#అయితే_ముగింపు
      ఆపు;
    స్థితి '0':
      ఒకవేళ(!(flags & FLAGS_ఎడమ))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    స్థితి '1':
    స్థితి '2':
    స్థితి '3':
    స్థితి '4':
    స్థితి '5':
    స్థితి '6':
    స్థితి '7':
    స్థితి '8':
    స్థితి '9': {
      curl_off_t అంకె;
      flags |= FLAGS_వెడల్పు;
      fmt--;
      ఒకవేళ(curlx_str_సంఖ్య(&fmt, &అంకె, గరిష్ఠ_పూర్ణాంకం))
        తిరిగి PFMT_వెడల్పు;
      వెడల్పు = (పూర్ణాంకం)అంకె;
      ఆపు;
    }
    స్థితి '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ఒకవేళ(use_dollar == DOLLAR_USE) {
        వెడల్పు = dollarstring(fmt, &fmt);
        ఒకవేళ(వెడల్పు < 0)
          /* illegal combo */
          తిరిగి PFMT_DOLLARWIDTH;
      }
      లేకపోతే
        /* pick from the next argument */
        వెడల్పు = -1;
      ఆపు;
    అప్రమేయ:
      loopit = FALSE;
      fmt--;
      ఆపు;
    } /* switch */
  } అయితే(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = వెడల్పు;
  *fmtp = fmt;
  తిరిగి PFMT_OK;
}

స్థిర bool parse_conversion(స్థిరరాశి అక్షరం f, గుర్తులేని పూర్ణాంకం *flagp,
                             FormatType *typep)
{
  గుర్తులేని పూర్ణాంకం flags = *flagp;
  FormatType type;
  ఎంపిక(f) {
  స్థితి 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    ఆపు;
  స్థితి 's':
    type = MTYPE_STRING;
    ఆపు;
  స్థితి 'n':
    type = MTYPE_INTPTR;
    ఆపు;
  స్థితి 'p':
    type = MTYPE_PTR;
    ఆపు;
  స్థితి 'd':
  స్థితి 'i':
    ఒకవేళ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    లేకపోతే ఒకవేళ(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    లేకపోతే
      type = MTYPE_INT;
    ఆపు;
  స్థితి 'u':
    ఒకవేళ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    లేకపోతే ఒకవేళ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    లేకపోతే
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    ఆపు;
  స్థితి 'o':
    ఒకవేళ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    లేకపోతే ఒకవేళ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    లేకపోతే
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    ఆపు;
  స్థితి 'x':
    ఒకవేళ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    లేకపోతే ఒకవేళ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    లేకపోతే
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    ఆపు;
  స్థితి 'X':
    ఒకవేళ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    లేకపోతే ఒకవేళ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    లేకపోతే
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_పై | FLAGS_UNSIGNED;
    ఆపు;
  స్థితి 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    ఆపు;
  స్థితి 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    ఆపు;
  స్థితి 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_పై;
    ఆపు;
  స్థితి 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    ఆపు;
  స్థితి 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_పై;
    ఆపు;
  స్థితి 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    ఆపు;
  స్థితి 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_పై;
    ఆపు;
  అప్రమేయ:
    /* invalid instruction, disregard and continue */
    తిరిగి TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  తిరిగి FALSE;
}

స్థిర పూర్ణాంకం parsefmt(స్థిరరాశి అక్షరం *format,
                    నిర్మాణం outsegment *బహిర్గతం,
                    నిర్మాణం va_ఆదానం *లోపల,
                    పూర్ణాంకం *opieces,
                    పూర్ణాంకం *ipieces, va_list arglist)
{
  స్థిరరాశి అక్షరం *fmt = format;
  పూర్ణాంకం param_అంకె = 0;
  పూర్ణాంకం max_param = -1;
  పూర్ణాంకం i;
  పూర్ణాంకం ocount = 0;
  గుర్తులేని అక్షరం usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  నిర్మాణం outsegment *optr;
  పూర్ణాంకం use_dollar = DOLLAR_UNKNOWN;
  స్థిరరాశి అక్షరం *ప్రారంభం = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, పరిమాణం(usedinput));

  అయితే(*fmt) {
    ఒకవేళ(*fmt == '%') {
      నిర్మాణం va_ఆదానం *iptr;
      FormatType type;
      గుర్తులేని పూర్ణాంకం flags = 0;
      పూర్ణాంకం వెడల్పు = 0;
      పూర్ణాంకం precision = 0;
      పూర్ణాంకం param = -1;
      పూర్ణాంకం rc;
      fmt++;
      outlen = (size_t)(fmt - ప్రారంభం - 1);
      ఒకవేళ(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ఒకవేళ(outlen) {
          optr = &బహిర్గతం[ocount++];
          ఒకవేళ(ocount > MAX_SEGMENTS)
            తిరిగి PFMT_MANYSEGS;
          optr->ఆదానం = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->ప్రారంభం = ప్రారంభం;
          optr->outlen = outlen;
        }
        ప్రారంభం = fmt;
        fmt++;
        కొనసాగు; /* while */
      }

      ఒకవేళ(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ఒకవేళ(param < 0) {
          ఒకవేళ(use_dollar == DOLLAR_USE)
            /* illegal combo */
            తిరిగి PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        లేకపోతే
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &వెడల్పు);
      ఒకవేళ(rc)
        తిరిగి rc;

      ఒకవేళ(parse_conversion(*fmt, &flags, &type))
        కొనసాగు;

      ఒకవేళ(flags & FLAGS_WIDTHPARAM) {
        ఒకవేళ(వెడల్పు < 0)
          వెడల్పు = param_అంకె++;
        లేకపోతే {
          /* if this identifies a parameter already used, this is illegal */
          ఒకవేళ(is_arg_వాడిన(usedinput, వెడల్పు))
            తిరిగి PFMT_WIDTHARG;
        }
        ఒకవేళ(వెడల్పు >= MAX_PARAMETERS)
          తిరిగి PFMT_MANYARGS;
        ఒకవేళ(వెడల్పు >= max_param)
          max_param = వెడల్పు;

        లోపల[వెడల్పు].type = MTYPE_వెడల్పు;
        /* mark as used */
        mark_arg_వాడిన(usedinput, వెడల్పు);
      }

      ఒకవేళ(flags & FLAGS_PRECPARAM) {
        ఒకవేళ(precision < 0)
          precision = param_అంకె++;
        లేకపోతే {
          /* if this identifies a parameter already used, this is illegal */
          ఒకవేళ(is_arg_వాడిన(usedinput, precision))
            తిరిగి PFMT_PRECARG;
        }
        ఒకవేళ(precision >= MAX_PARAMETERS)
          తిరిగి PFMT_MANYARGS;
        ఒకవేళ(precision >= max_param)
          max_param = precision;

        లోపల[precision].type = MTYPE_PRECISION;
        mark_arg_వాడిన(usedinput, precision);
      }

      /* Handle the specifier */
      ఒకవేళ(param < 0)
        param = param_అంకె++;
      ఒకవేళ(param >= MAX_PARAMETERS)
        తిరిగి PFMT_MANYARGS;
      ఒకవేళ(param >= max_param)
        max_param = param;

      iptr = &లోపల[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_వాడిన(usedinput, param);

      fmt++;
      optr = &బహిర్గతం[ocount++];
      ఒకవేళ(ocount > MAX_SEGMENTS)
        తిరిగి PFMT_MANYSEGS;
      optr->ఆదానం = (గుర్తులేని పూర్ణాంకం)param;
      optr->flags = flags;
      optr->వెడల్పు = వెడల్పు;
      optr->precision = precision;
      optr->ప్రారంభం = ప్రారంభం;
      optr->outlen = outlen;
      ప్రారంభం = fmt;
    }
    లేకపోతే
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - ప్రారంభం);
  ఒకవేళ(outlen) {
    optr = &బహిర్గతం[ocount++];
    ఒకవేళ(ocount > MAX_SEGMENTS)
      తిరిగి PFMT_MANYSEGS;
    optr->ఆదానం = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->ప్రారంభం = ప్రారంభం;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  కొరకు(i = 0; i < max_param + 1; i++) {
    నిర్మాణం va_ఆదానం *iptr = &లోపల[i];
    ఒకవేళ(!is_arg_వాడిన(usedinput, i))
      /* bad input */
      తిరిగి PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    ఎంపిక(iptr->type) {
    స్థితి MTYPE_STRING:
      iptr->val.str = va_arg(arglist, స్థిరరాశి అక్షరం *);
      ఆపు;

    స్థితి MTYPE_INTPTR:
    స్థితి MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, శూన్య *);
      ఆపు;

    స్థితి MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      ఆపు;

    స్థితి MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      ఆపు;

    స్థితి MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, గుర్తులేని దీర్ఘ);
      ఆపు;

    స్థితి MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, దీర్ఘ);
      ఆపు;

    స్థితి MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, గుర్తులేని పూర్ణాంకం);
      ఆపు;

    స్థితి MTYPE_INT:
    స్థితి MTYPE_వెడల్పు:
    స్థితి MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, పూర్ణాంకం);
      ఆపు;

    స్థితి MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, ద్విగుణ);
      ఆపు;

    స్థితి MTYPE_LONGDOUBLE:
      iptr->val.dnum = (ద్విగుణ)va_arg(arglist, దీర్ఘ ద్విగుణ);
      ఆపు;

    అప్రమేయ:
      DEBUGASSERT(ఖాళీ); /* unexpected */
      ఆపు;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  తిరిగి PFMT_OK;
}

నిర్మాణం mproperty {
  పూర్ణాంకం వెడల్పు;            /* Width of a field. */
  పూర్ణాంకం prec;             /* Precision of a field. */
  గుర్తులేని పూర్ణాంకం flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
రకనిర్వచనం size_t (*mp_streamn)(స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t len, శూన్య *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
రకనిర్వచనం పూర్ణాంకం (*mp_flush)(శూన్య *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
స్థిర bool ప్రవాహం_నిరంతరం(శూన్య *userp,
                       పూర్ణాంకం (*ప్రవాహం)(గుర్తులేని అక్షరం, శూన్య *),
                       mp_streamn streamn,
                       స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t len, పూర్ణాంకం *donep)
{
  ఒకవేళ(!len)
    తిరిగి FALSE;
  ఒకవేళ(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (పూర్ణాంకం)n;
    తిరిగి n != len;
  }
  అయితే(len--) {
    ఒకవేళ(ప్రవాహం(*buf++, userp))
      తిరిగి TRUE;
    (*donep)++;
  }
  తిరిగి FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
స్థిర స్థిరరాశి అక్షరం pad_spaces[] = "                ";
స్థిర స్థిరరాశి అక్షరం pad_zeros[] = "0000000000000000";
#నిర్వచించు PAD_నిరంతరం 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
స్థిర bool ప్రవాహం_pad(శూన్య *userp,
                       పూర్ణాంకం (*ప్రవాహం)(గుర్తులేని అక్షరం, శూన్య *),
                       mp_streamn streamn,
                       గుర్తులేని అక్షరం pad, పూర్ణాంకం అంకె, పూర్ణాంకం *donep)
{
  స్థిరరాశి గుర్తులేని అక్షరం *src;
  ఒకవేళ(అంకె <= 0)
    తిరిగి FALSE;
  ఒకవేళ(pad == '0')
    src = (స్థిరరాశి గుర్తులేని అక్షరం *)pad_zeros;
  లేకపోతే
    src = (స్థిరరాశి గుర్తులేని అక్షరం *)pad_spaces;
  అయితే(అంకె > PAD_నిరంతరం) {
    ఒకవేళ(ప్రవాహం_నిరంతరం(userp, ప్రవాహం, streamn, src, PAD_నిరంతరం, donep))
      తిరిగి TRUE;
    అంకె -= PAD_నిరంతరం;
  }
  తిరిగి ప్రవాహం_నిరంతరం(userp, ప్రవాహం, streamn, src, (size_t)అంకె, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
స్థిర bool ప్రవాహం_zrun(శూన్య *userp,
                        పూర్ణాంకం (*ప్రవాహం)(గుర్తులేని అక్షరం, శూన్య *),
                        mp_streamn streamn,
                        స్థిరరాశి అక్షరం *str, size_t len, పూర్ణాంకం *donep)
{
  size_t n = 0;
  అయితే(n < len && str[n])
    n++;
  తిరిగి ప్రవాహం_నిరంతరం(userp, ప్రవాహం, streamn,
                    (స్థిరరాశి గుర్తులేని అక్షరం *)str, n, donep);
}

స్థిర bool బహిర్గతం_double(శూన్య *userp,
                       పూర్ణాంకం (*ప్రవాహం)(గుర్తులేని అక్షరం, శూన్య *),
                       mp_streamn streamn,
                       నిర్మాణం mproperty *p,
                       ద్విగుణ dnum,
                       అక్షరం *work, పూర్ణాంకం *donep)
{
  అక్షరం fmt[32] = "%";
  అక్షరం *fptr = &fmt[1];
  size_t ఎడమ = పరిమాణం(fmt) - అక్షరశ్రేణి_పొడవు(fmt);
  పూర్ణాంకం flags = p->flags;
  పూర్ణాంకం వెడల్పు = p->వెడల్పు;
  పూర్ణాంకం prec = p->prec;

  ఒకవేళ(flags & FLAGS_ఎడమ)
    *fptr++ = '-';
  ఒకవేళ(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ఒకవేళ(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ఒకవేళ(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ఒకవేళ(వెడల్పు >= 0) {
    size_t dlen;
    ఒకవేళ(వెడల్పు >= BUFFSIZE)
      వెడల్పు = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, ఎడమ, "%d", వెడల్పు);
    fptr += dlen;
    ఎడమ -= dlen;
  }
  ఒకవేళ(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    పూర్ణాంకం maxprec = BUFFSIZE - 1;
    ద్విగుణ val = dnum;
    పూర్ణాంకం len;
    ఒకవేళ(prec > maxprec)
      prec = maxprec - 1;
    ఒకవేళ(వెడల్పు > 0 && prec <= వెడల్పు)
      maxprec -= వెడల్పు;
    అయితే(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ఒకవేళ(prec > maxprec)
      prec = maxprec - 1;
    ఒకవేళ(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, ఎడమ, ".%d", prec);
    fptr += len;
  }
  ఒకవేళ(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ఒకవేళ(flags & FLAGS_FLOATE)
    *fptr++ = (అక్షరం)((flags & FLAGS_పై) ? 'E' : 'e');
  లేకపోతే ఒకవేళ(flags & FLAGS_FLOATG)
    *fptr++ = (అక్షరం)((flags & FLAGS_పై) ? 'G' : 'g');
  లేకపోతే
    *fptr++ = (flags & FLAGS_పై) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wformat-nonliteral"
#అయితే_ముగింపు
#నిర్వచితమైతే _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#లేకపోతే
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
  DEBUGASSERT(అక్షరశ్రేణి_పొడవు(work) < BUFFSIZE);
  తిరిగి ప్రవాహం_నిరంతరం(userp, ప్రవాహం, streamn,
                    (స్థిరరాశి గుర్తులేని అక్షరం *)work, అక్షరశ్రేణి_పొడవు(work), donep);
}

స్థిర bool బహిర్గతం_సంఖ్య(శూన్య *userp,
                       పూర్ణాంకం (*ప్రవాహం)(గుర్తులేని అక్షరం, శూన్య *),
                       mp_streamn streamn,
                       నిర్మాణం mproperty *p,
                       uint64_t అంకె,
                       int64_t nums,
                       అక్షరం *work, పూర్ణాంకం *donep)
{
  స్థిరరాశి గుర్తులేని అక్షరం *digits = Curl_ldigits;
  పూర్ణాంకం flags = p->flags;
  పూర్ణాంకం వెడల్పు = p->వెడల్పు;
  పూర్ణాంకం prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  పూర్ణాంకం పునాది = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  అక్షరం *workend = &work[BUFFSIZE - 2];
  అక్షరం *w;

  ఒకవేళ(flags & FLAGS_CHAR) {
    /* Character. */
    ఒకవేళ(!(flags & FLAGS_ఎడమ)) {
      ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, ' ', వెడల్పు - 1, donep))
        తిరిగి TRUE;
      వెడల్పు = 1;
    }
    OUTCHAR((అక్షరం)అంకె);
    ఒకవేళ(flags & FLAGS_ఎడమ) {
      ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, ' ', వెడల్పు - 1, donep))
        తిరిగి TRUE;
    }
    తిరిగి FALSE;
  }
  ఒకవేళ(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    పునాది = 8;

  లేకపోతే ఒకవేళ(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_పై) ? Curl_udigits : Curl_ldigits;
    పునాది = 16;
  }
  లేకపోతే ఒకవేళ(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  లేకపోతే {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ఒకవేళ(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_అంకె; /* Used to convert negative in positive. */
      signed_అంకె = nums + (int64_t)1;
      signed_అంకె = -signed_అంకె;
      అంకె = (uint64_t)signed_అంకె;
      అంకె += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ఒకవేళ(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(పునాది <= 16);
  ఎంపిక(పునాది) {
  స్థితి 10:
    అయితే(అంకె > 0) {
      *w-- = (అక్షరం)('0' + (అంకె % 10));
      అంకె /= 10;
    }
    ఆపు;
  అప్రమేయ:
    అయితే(అంకె > 0) {
      *w-- = digits[అంకె % పునాది];
      అంకె /= పునాది;
    }
    ఆపు;
  }
  వెడల్పు -= (పూర్ణాంకం)(workend - w);
  prec -= (పూర్ణాంకం)(workend - w);

  ఒకవేళ(is_alt && పునాది == 8 && prec <= 0) {
    *w-- = '0';
    --వెడల్పు;
  }

  ఒకవేళ(prec > 0) {
    వెడల్పు -= prec;
    అయితే(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ఒకవేళ(is_alt && పునాది == 16)
    వెడల్పు -= 2;

  ఒకవేళ(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --వెడల్పు;

  ఒకవేళ(!(flags & FLAGS_ఎడమ) && !(flags & FLAGS_PAD_NIL)) {
    ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, ' ', వెడల్పు, donep))
      తిరిగి TRUE;
    వెడల్పు = 0;
  }

  ఒకవేళ(is_neg)
    OUTCHAR('-');
  లేకపోతే ఒకవేళ(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  లేకపోతే ఒకవేళ(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ఒకవేళ(is_alt && పునాది == 16) {
    OUTCHAR('0');
    ఒకవేళ(flags & FLAGS_పై)
      OUTCHAR('X');
    లేకపోతే
      OUTCHAR('x');
  }

  ఒకవేళ(!(flags & FLAGS_ఎడమ) && (flags & FLAGS_PAD_NIL)) {
    ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, '0', వెడల్పు, donep))
      తిరిగి TRUE;
    వెడల్పు = 0;
  }

  /* Write the number. */
  ఒకవేళ(ప్రవాహం_నిరంతరం(userp, ప్రవాహం, streamn, (స్థిరరాశి గుర్తులేని అక్షరం *)w + 1,
                (size_t)(workend - w), donep))
    తిరిగి TRUE;

  ఒకవేళ(flags & FLAGS_ఎడమ) {
    ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, ' ', వెడల్పు, donep))
      తిరిగి TRUE;
  }

  తిరిగి FALSE;
}

స్థిర స్థిరరాశి అక్షరం nilstr[] = "(nil)";

స్థిర bool బహిర్గతం_string(శూన్య *userp,
                       పూర్ణాంకం (*ప్రవాహం)(గుర్తులేని అక్షరం, శూన్య *),
                       mp_streamn streamn,
                       నిర్మాణం mproperty *p,
                       స్థిరరాశి అక్షరం *str,
                       పూర్ణాంకం *donep)
{
  పూర్ణాంకం flags = p->flags;
  పూర్ణాంకం వెడల్పు = p->వెడల్పు;
  పూర్ణాంకం prec = p->prec;
  size_t len;

  ఒకవేళ(!str) {
    /* Write null string if there is space. */
    ఒకవేళ(prec == -1 || prec >= (పూర్ణాంకం)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(గుర్తులేని పూర్ణాంకం)FLAGS_ALT;
    }
    లేకపోతే {
      str = "";
      len = 0;
    }
  }
  లేకపోతే ఒకవేళ(prec != -1)
    len = (size_t)prec;
  లేకపోతే ఒకవేళ(*str == '\0')
    len = 0;
  లేకపోతే
    len = అక్షరశ్రేణి_పొడవు(str);

  వెడల్పు -= (len > గరిష్ఠ_పూర్ణాంకం) ? గరిష్ఠ_పూర్ణాంకం : (పూర్ణాంకం)len;

  ఒకవేళ(flags & FLAGS_ALT)
    OUTCHAR('"');

  ఒకవేళ(!(flags & FLAGS_ఎడమ)) {
    ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, ' ', వెడల్పు, donep))
      తిరిగి TRUE;
    వెడల్పు = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ఒకవేళ(prec == -1) {
    ఒకవేళ(ప్రవాహం_నిరంతరం(userp, ప్రవాహం, streamn,
                  (స్థిరరాశి గుర్తులేని అక్షరం *)str, len, donep))
      తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(ప్రవాహం_zrun(userp, ప్రవాహం, streamn, str, len, donep))
    తిరిగి TRUE;
  ఒకవేళ(flags & FLAGS_ఎడమ) {
    ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, ' ', వెడల్పు, donep))
      తిరిగి TRUE;
  }

  ఒకవేళ(flags & FLAGS_ALT)
    OUTCHAR('"');

  తిరిగి FALSE;
}

స్థిర bool బహిర్గతం_సూచిక(శూన్య *userp,
                        పూర్ణాంకం (*ప్రవాహం)(గుర్తులేని అక్షరం, శూన్య *),
                        mp_streamn streamn,
                        నిర్మాణం mproperty *p,
                        స్థిరరాశి అక్షరం *ptr,
                        అక్షరం *work,
                        పూర్ణాంకం *donep)
{
  /* Generic pointer. */
  ఒకవేళ(ptr) {
    size_t అంకె = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ఒకవేళ(బహిర్గతం_సంఖ్య(userp, ప్రవాహం, streamn, p, అంకె, 0, work, donep))
      తిరిగి TRUE;
  }
  లేకపోతే {
    /* Write "(nil)" for a nil pointer. */
    పూర్ణాంకం వెడల్పు = p->వెడల్పు;
    పూర్ణాంకం flags = p->flags;

    వెడల్పు -= (పూర్ణాంకం)CURL_CSTRLEN(nilstr);
    ఒకవేళ(flags & FLAGS_ఎడమ) {
      ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, ' ', వెడల్పు, donep))
        తిరిగి TRUE;
      వెడల్పు = 0;
    }
    ఒకవేళ(ప్రవాహం_నిరంతరం(userp, ప్రవాహం, streamn, (స్థిరరాశి గుర్తులేని అక్షరం *)nilstr,
                  పరిమాణం(nilstr) - 1, donep))
      తిరిగి TRUE;
    ఒకవేళ(!(flags & FLAGS_ఎడమ)) {
      ఒకవేళ(ప్రవాహం_pad(userp, ప్రవాహం, streamn, ' ', వెడల్పు, donep))
        తిరిగి TRUE;
    }
  }
  తిరిగి FALSE;
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
స్థిర పూర్ణాంకం formatf(శూన్య *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   పూర్ణాంకం (*ప్రవాహం)(గుర్తులేని అక్షరం, శూన్య *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   స్థిరరాశి అక్షరం *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  పూర్ణాంకం పూర్తి = 0;   /* number of characters written */
  పూర్ణాంకం i;
  పూర్ణాంకం ocount = 0; /* number of output segments */
  పూర్ణాంకం icount = 0; /* number of input arguments */

  నిర్మాణం outsegment output[MAX_SEGMENTS];
  నిర్మాణం va_ఆదానం ఆదానం[MAX_PARAMETERS];
  అక్షరం work[BUFFSIZE + 2];

  /* Parse the format string */
  ఒకవేళ(parsefmt(format, output, ఆదానం, &ocount, &icount, ap_save))
    తిరిగి 0;

  కొరకు(i = 0; i < ocount; i++) {
    నిర్మాణం outsegment *optr = &output[i];
    నిర్మాణం va_ఆదానం *iptr = &ఆదానం[optr->ఆదానం];
    నిర్మాణం mproperty p;
    size_t outlen = optr->outlen;

    ఒకవేళ(outlen) {
      ఒకవేళ(ప్రవాహం_zrun(userp, ప్రవాహం, streamn, optr->ప్రారంభం, outlen, &పూర్తి))
        తిరిగి పూర్తి;
      ఒకవేళ(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        కొనసాగు;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ఒకవేళ(p.flags & FLAGS_WIDTHPARAM) {
      p.వెడల్పు = (పూర్ణాంకం)ఆదానం[optr->వెడల్పు].val.nums;
      ఒకవేళ(p.వెడల్పు < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ఒకవేళ(p.వెడల్పు == INT_MIN)
          p.వెడల్పు = గరిష్ఠ_పూర్ణాంకం;
        లేకపోతే
          p.వెడల్పు = -p.వెడల్పు;
        p.flags |= FLAGS_ఎడమ;
        p.flags &= ~(గుర్తులేని పూర్ణాంకం)FLAGS_PAD_NIL;
      }
    }
    లేకపోతే
      p.వెడల్పు = optr->వెడల్పు;

    /* pick up the specified precision */
    ఒకవేళ(p.flags & FLAGS_PRECPARAM) {
      p.prec = (పూర్ణాంకం)ఆదానం[optr->precision].val.nums;
      ఒకవేళ(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    లేకపోతే ఒకవేళ(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    లేకపోతే
      p.prec = -1;

    ఎంపిక(iptr->type) {
    స్థితి MTYPE_INTU:
    స్థితి MTYPE_LONGU:
    స్థితి MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ఒకవేళ(బహిర్గతం_సంఖ్య(userp, ప్రవాహం, streamn, &p, iptr->val.numu, 0, work,
                    &పూర్తి))
        తిరిగి పూర్తి;
      ఆపు;

    స్థితి MTYPE_INT:
    స్థితి MTYPE_LONG:
    స్థితి MTYPE_LONGLONG:
      ఒకవేళ(బహిర్గతం_సంఖ్య(userp, ప్రవాహం, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &పూర్తి))
        తిరిగి పూర్తి;
      ఆపు;

    స్థితి MTYPE_STRING:
      ఒకవేళ(బహిర్గతం_string(userp, ప్రవాహం, streamn, &p, iptr->val.str, &పూర్తి))
        తిరిగి పూర్తి;
      ఆపు;

    స్థితి MTYPE_PTR:
      ఒకవేళ(బహిర్గతం_సూచిక(userp, ప్రవాహం, streamn, &p, iptr->val.ptr, work,
                     &పూర్తి))
        తిరిగి పూర్తి;
      ఆపు;

    స్థితి MTYPE_DOUBLE:
    స్థితి MTYPE_LONGDOUBLE:
      ఒకవేళ(బహిర్గతం_double(userp, ప్రవాహం, streamn, &p, iptr->val.dnum, work,
                    &పూర్తి))
        తిరిగి పూర్తి;
      ఆపు;

    స్థితి MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ఒకవేళ(flush && flush(userp))
        తిరిగి పూర్తి;
      /* Answer the count of characters written. */
      ఒకవేళ(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)పూర్తి;
      లేకపోతే ఒకవేళ(p.flags & FLAGS_LONG)
        *(దీర్ఘ *)iptr->val.ptr = (దీర్ఘ)పూర్తి;
      లేకపోతే ఒకవేళ(!(p.flags & FLAGS_SHORT))
        *(పూర్ణాంకం *)iptr->val.ptr = పూర్తి;
      లేకపోతే
        *(పొట్టి *)iptr->val.ptr = (పొట్టి)పూర్తి;
      ఆపు;

    అప్రమేయ:
      ఆపు;
    }
  }
  తిరిగి పూర్తి;
}

/* fputc() look-alike */
స్థిర పూర్ణాంకం addbyter(గుర్తులేని అక్షరం outc, శూన్య *f)
{
  నిర్మాణం nsprintf *infop = f;
  ఒకవేళ(infop->నిడివి < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->బఫరు++ = (అక్షరం)outc; /* store */
    infop->నిడివి++; /* we are now one byte larger */
    తిరిగి 0;     /* fputc() returns like this on success */
  }
  తిరిగి 1;
}

/* block variant of addbyter */
స్థిర size_t addrun(స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t len, శూన్య *f)
{
  నిర్మాణం nsprintf *infop = f;
  size_t fit = infop->max - infop->నిడివి;
  ఒకవేళ(len > fit)
    len = fit;
  ఒకవేళ(len) {
    జ్ఞాపకం_నకలు(infop->బఫరు, buf, len);
    infop->బఫరు += len;
    infop->నిడివి += len;
  }
  తిరిగి len;
}

పూర్ణాంకం curl_mvsnprintf(అక్షరం *బఫరు, size_t maxlength, స్థిరరాశి అక్షరం *format,
                    va_list వాదనలు)
{
  పూర్ణాంకం retcode;
  నిర్మాణం nsprintf info;

  info.బఫరు = బఫరు;
  info.నిడివి = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, ఖాళీ, format, వాదనలు);
  ఒకవేళ(info.max) {
    /* we null-terminate this */
    ఒకవేళ(info.max == info.నిడివి) {
      /* we are at maximum, scrap the last letter */
      info.బఫరు[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    లేకపోతే
      info.బఫరు[0] = 0;
  }
  తిరిగి retcode;
}

పూర్ణాంకం curl_msnprintf(అక్షరం *బఫరు, size_t maxlength, స్థిరరాశి అక్షరం *format, ...)
{
  పూర్ణాంకం retcode;
  va_list వాదనలు; /* argument pointer */
  va_start(వాదనలు, format);
  retcode = curl_mvsnprintf(బఫరు, maxlength, format, వాదనలు);
  va_end(వాదనలు);
  తిరిగి retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
స్థిర పూర్ణాంకం alloc_flush(శూన్య *f)
{
  నిర్మాణం asprintf *infop = f;
  ఒకవేళ(infop->nstage) {
    CURLcode ఫలితం = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ఒకవేళ(ఫలితం) {
      infop->merr = ఫలితం == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      తిరిగి 1; /* fail */
    }
  }
  తిరిగి 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
స్థిర పూర్ణాంకం alloc_addn(నిర్మాణం asprintf *infop, స్థిరరాశి గుర్తులేని అక్షరం *buf,
                      size_t len)
{
  CURLcode ఫలితం = curlx_dyn_addn(infop->b, buf, len);
  ఒకవేళ(ఫలితం) {
    infop->merr = ఫలితం == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    తిరిగి 1; /* fail */
  }
  తిరిగి 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
స్థిర bool alloc_సరిపోతుంది(నిర్మాణం asprintf *infop, size_t మరిన్ని)
{
  తిరిగి (infop->b->leng + infop->nstage + మరిన్ని + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
స్థిర పూర్ణాంకం alloc_addbyter(గుర్తులేని అక్షరం outc, శూన్య *f)
{
  నిర్మాణం asprintf *infop = f;
  ఒకవేళ(infop->nstage == పరిమాణం(infop->stage) && alloc_flush(infop))
    తిరిగి 1; /* fail */
  ఒకవేళ(!alloc_సరిపోతుంది(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ఒకవేళ(alloc_flush(infop))
      తిరిగి 1;
    తిరిగి alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  తిరిగి 0;
}

/* block variant of alloc_addbyter */
స్థిర size_t alloc_addrun(స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t len, శూన్య *f)
{
  నిర్మాణం asprintf *infop = f;
  size_t accepted = 0;
  అయితే(accepted < len) {
    size_t స్థలం = పరిమాణం(infop->stage) - infop->nstage;
    size_t n;
    ఒకవేళ(!స్థలం) {
      ఒకవేళ(alloc_flush(infop))
        తిరిగి accepted; /* accepted < len aborts formatting */
      స్థలం = పరిమాణం(infop->stage);
    }
    n = len - accepted;
    ఒకవేళ(n > స్థలం)
      n = స్థలం;
    ఒకవేళ(!alloc_సరిపోతుంది(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t సరిపోతుంది = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ఒకవేళ(సరిపోతుంది) {
        జ్ఞాపకం_నకలు(&infop->stage[infop->nstage], buf + accepted, సరిపోతుంది);
        infop->nstage += సరిపోతుంది;
        accepted += సరిపోతుంది;
      }
      ఒకవేళ(alloc_flush(infop))
        తిరిగి accepted;
      ఒకవేళ(alloc_addn(infop, buf + accepted, 1))
        తిరిగి accepted; /* accepted < len aborts formatting */
      accepted++;
      కొనసాగు;
    }
    జ్ఞాపకం_నకలు(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  తిరిగి accepted;
}

/* appends the formatted string, returns MERR error code */
పూర్ణాంకం curlx_dyn_vprintf(నిర్మాణం dynbuf *dyn, స్థిరరాశి అక్షరం *format, va_list వాదనలు)
{
  నిర్మాణం asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (శూన్య)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, వాదనలు);
  ఒకవేళ(!info.merr)
    (శూన్య)alloc_flush(&info);
  ఒకవేళ(info.merr) {
    curlx_dyn_free(info.b);
    తిరిగి info.merr;
  }
  తిరిగి 0;
}

అక్షరం *curl_mvaprintf(స్థిరరాశి అక్షరం *format, va_list వాదనలు)
{
  నిర్మాణం asprintf info;
  నిర్మాణం dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (శూన్య)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, వాదనలు);
  ఒకవేళ(!info.merr)
    (శూన్య)alloc_flush(&info);
  ఒకవేళ(info.merr) {
    curlx_dyn_free(info.b);
    తిరిగి ఖాళీ;
  }
  ఒకవేళ(curlx_dyn_len(info.b))
    తిరిగి curlx_dyn_ptr(info.b);
  తిరిగి curlx_strdup("");
}

అక్షరం *curl_maprintf(స్థిరరాశి అక్షరం *format, ...)
{
  va_list వాదనలు;
  అక్షరం *s;
  va_start(వాదనలు, format);
  s = curl_mvaprintf(format, వాదనలు);
  va_end(వాదనలు);
  తిరిగి s;
}

స్థిర పూర్ణాంకం storebuffer(గుర్తులేని అక్షరం outc, శూన్య *f)
{
  అక్షరం **బఫరు = f;
  **బఫరు = (అక్షరం)outc;
  (*బఫరు)++;
  తిరిగి 0;
}

/* block variant of storebuffer */
స్థిర size_t storerun(స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t len, శూన్య *f)
{
  అక్షరం **బఫరు = f;
  జ్ఞాపకం_నకలు(*బఫరు, buf, len);
  *బఫరు += len;
  తిరిగి len;
}

పూర్ణాంకం curl_msprintf(అక్షరం *బఫరు, స్థిరరాశి అక్షరం *format, ...)
{
  va_list వాదనలు; /* argument pointer */
  పూర్ణాంకం retcode;
  va_start(వాదనలు, format);
  retcode = formatf(&బఫరు, storebuffer, storerun, ఖాళీ, format, వాదనలు);
  va_end(వాదనలు);
  *బఫరు = 0; /* we null-terminate this */
  తిరిగి retcode;
}

స్థిర పూర్ణాంకం fputc_wrapper(గుర్తులేని అక్షరం outc, శూన్య *f)
{
  నిర్మాణం ioprintf *i = f;
  పూర్ణాంకం బహిర్గతం = outc;
  FILE *s = i->fd;
  పూర్ణాంకం rc = fputc(బహిర్గతం, s);
  i->error = (rc == EOF);
  తిరిగి (rc == EOF);
}

/* block variant of fputc_wrapper */
స్థిర size_t fwrite_wrapper(స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t len, శూన్య *f)
{
  నిర్మాణం ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  తిరిగి n;
}

పూర్ణాంకం curl_mvfprintf(FILE *fd, స్థిరరాశి అక్షరం *format, va_list వాదనలు)
{
  నిర్మాణం ioprintf info;
  పూర్ణాంకం n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, ఖాళీ, format, వాదనలు);
  ఒకవేళ(info.error)
    తిరిగి -1;
  తిరిగి n;
}

పూర్ణాంకం curl_mfprintf(FILE *fd, స్థిరరాశి అక్షరం *format, ...)
{
  va_list వాదనలు; /* argument pointer */
  పూర్ణాంకం n;
  va_start(వాదనలు, format);
  n = curl_mvfprintf(fd, format, వాదనలు);
  va_end(వాదనలు);
  తిరిగి n;
}

పూర్ణాంకం curl_mprintf(స్థిరరాశి అక్షరం *format, ...)
{
  va_list వాదనలు; /* argument pointer */
  పూర్ణాంకం n;
  va_start(వాదనలు, format);
  n = curl_mvfprintf(stdout, format, వాదనలు);
  va_end(వాదనలు);
  తిరిగి n;
}

పూర్ణాంకం curl_mvsprintf(అక్షరం *బఫరు, స్థిరరాశి అక్షరం *format, va_list వాదనలు)
{
  పూర్ణాంకం retcode = formatf(&బఫరు, storebuffer, storerun, ఖాళీ, format, వాదనలు);
  *బఫరు = 0; /* we null-terminate this */
  తిరిగి retcode;
}

పూర్ణాంకం curl_mvprintf(స్థిరరాశి అక్షరం *format, va_list వాదనలు)
{
  తిరిగి curl_mvfprintf(stdout, format, వాదనలు);
}

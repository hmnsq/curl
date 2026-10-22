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

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
अचर अचिन्हित वर्ण Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
अचर अचिन्हित वर्ण Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  गर {                                    \
    यदि(stream((अचिन्हित वर्ण)(x), userp)) \
      फर्काऊ TRUE;                        \
    (*donep)++;                           \
  } जबसम्म(0)

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
  MTYPE_WIDTH,
  MTYPE_PRECISION
} FormatType;

/* conversion and display flags */
गणना {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_बायाँ       = 1 << 2,
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

गणना {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
संरचना va_आगत {
  FormatType type; /* FormatType */
  सङ्घ {
    अचर वर्ण *str;
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
  पूर्णाङ्क width;     /* width OR width parameter number */
  पूर्णाङ्क precision; /* precision OR precision parameter number */
  अचिन्हित पूर्णाङ्क flags;
  अचिन्हित पूर्णाङ्क आगत; /* input argument array index */
  अचर वर्ण *start; /* format string start to output */
  माप_प्रकार outlen;     /* number of bytes from the format string to output */
};

संरचना nsprintf {
  वर्ण *buffer;
  माप_प्रकार length;
  माप_प्रकार max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

संरचना asprintf {
  संरचना dynbuf *b;
  वर्ण merr;
  माप_प्रकार nstage;
  अचिन्हित वर्ण stage[ASPRINTF_STAGE_SIZE];
};

संरचना ioprintf {
  खाता *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
स्थिर पूर्णाङ्क dollarstring(अचर वर्ण *p, अचर वर्ण **end)
{
  curl_off_t अङ्क;
  यदि(curlx_str_सङ्ख्या(&p, &अङ्क, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !अङ्क)
    फर्काऊ -1;
  *end = p;
  फर्काऊ (पूर्णाङ्क)अङ्क - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (अचिन्हित वर्ण)(1 << ((y) & 7)))

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

स्थिर पूर्णाङ्क parse_flags(अचर वर्ण **fmtp, अचिन्हित पूर्णाङ्क *flagsp, पूर्णाङ्क use_dollar,
                       पूर्णाङ्क *precp, पूर्णाङ्क *widthp)
{
  अचर वर्ण *fmt = *fmtp;
  bool loopit = TRUE;
  अचिन्हित पूर्णाङ्क flags = 0;
  पूर्णाङ्क width = 0;
  पूर्णाङ्क precision = 0;

  /* Handle the flags */
  गर {
    विकल्प(*fmt++) {
    पक्ष ' ':
      flags |= FLAGS_SPACE;
      रोक;
    पक्ष '+':
      flags |= FLAGS_SHOWSIGN;
      रोक;
    पक्ष '-':
      flags |= FLAGS_बायाँ;
      flags &= ~(अचिन्हित पूर्णाङ्क)FLAGS_PAD_NIL;
      रोक;
    पक्ष '#':
      flags |= FLAGS_ALT;
      रोक;
    पक्ष '.':
      यदि('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        यदि(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          यदि(precision < 0)
            /* illegal combo */
            फर्काऊ PFMT_DOLLARPREC;
        }
        अन्यथा
          /* get it from the next argument */
          precision = -1;
      }
      अन्यथा {
        bool is_neg;
        curl_off_t अङ्क;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        यदि(is_neg)
          fmt++;
        यदि(curlx_str_सङ्ख्या(&fmt, &अङ्क, पूर्णांक_उच्च))
          फर्काऊ PFMT_PREC;
        precision = (पूर्णाङ्क)अङ्क;
        यदि(is_neg)
          precision = -precision;
      }
      यदि((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        फर्काऊ PFMT_PRECMIX;
      रोक;
    पक्ष 'h':
      flags |= FLAGS_SHORT;
      रोक;
#ifdef _WIN32
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
#यदि SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#अन्यथा
        flags |= FLAGS_LONG;
#endif
      }
      रोक;
#endif /* _WIN32 */
    पक्ष 'l':
      यदि(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      अन्यथा
        flags |= FLAGS_LONG;
      रोक;
    पक्ष 'L':
      flags |= FLAGS_LONGDOUBLE;
      रोक;
    पक्ष 'q':
      flags |= FLAGS_LONGLONG;
      रोक;
    पक्ष 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#यदि SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अन्यथा
      flags |= FLAGS_LONG;
#endif
      रोक;
    पक्ष 'O':
#यदि SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अन्यथा
      flags |= FLAGS_LONG;
#endif
      रोक;
    पक्ष '0':
      यदि(!(flags & FLAGS_बायाँ))
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
      curl_off_t अङ्क;
      flags |= FLAGS_WIDTH;
      fmt--;
      यदि(curlx_str_सङ्ख्या(&fmt, &अङ्क, पूर्णांक_उच्च))
        फर्काऊ PFMT_WIDTH;
      width = (पूर्णाङ्क)अङ्क;
      रोक;
    }
    पक्ष '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      यदि(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        यदि(width < 0)
          /* illegal combo */
          फर्काऊ PFMT_DOLLARWIDTH;
      }
      अन्यथा
        /* pick from the next argument */
        width = -1;
      रोक;
    स्वभाव:
      loopit = FALSE;
      fmt--;
      रोक;
    } /* switch */
  } जबसम्म(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  फर्काऊ PFMT_OK;
}

स्थिर bool parse_conversion(अचर वर्ण f, अचिन्हित पूर्णाङ्क *flagp,
                             FormatType *typep)
{
  अचिन्हित पूर्णाङ्क flags = *flagp;
  FormatType type;
  विकल्प(f) {
  पक्ष 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    रोक;
  पक्ष 's':
    type = MTYPE_STRING;
    रोक;
  पक्ष 'n':
    type = MTYPE_INTPTR;
    रोक;
  पक्ष 'p':
    type = MTYPE_PTR;
    रोक;
  पक्ष 'd':
  पक्ष 'i':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    अन्यथा
      type = MTYPE_INT;
    रोक;
  पक्ष 'u':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    रोक;
  पक्ष 'o':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    रोक;
  पक्ष 'x':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    रोक;
  पक्ष 'X':
    यदि(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा यदि(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    रोक;
  पक्ष 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    रोक;
  पक्ष 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    रोक;
  पक्ष 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    रोक;
  पक्ष 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    रोक;
  पक्ष 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    रोक;
  पक्ष 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    रोक;
  पक्ष 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    रोक;
  स्वभाव:
    /* invalid instruction, disregard and continue */
    फर्काऊ TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  फर्काऊ FALSE;
}

स्थिर पूर्णाङ्क parsefmt(अचर वर्ण *format,
                    संरचना outsegment *out,
                    संरचना va_आगत *in,
                    पूर्णाङ्क *opieces,
                    पूर्णाङ्क *ipieces, बहु_सूची arglist)
{
  अचर वर्ण *fmt = format;
  पूर्णाङ्क param_अङ्क = 0;
  पूर्णाङ्क max_param = -1;
  पूर्णाङ्क i;
  पूर्णाङ्क ocount = 0;
  अचिन्हित वर्ण usedinput[MAX_PARAMETERS / 8];
  माप_प्रकार outlen = 0;
  संरचना outsegment *optr;
  पूर्णाङ्क use_dollar = DOLLAR_UNKNOWN;
  अचर वर्ण *start = fmt;

  /* clear, set a bit for each used input */
  स_रखो(usedinput, 0, परिमाण(usedinput));

  जबसम्म(*fmt) {
    यदि(*fmt == '%') {
      संरचना va_आगत *iptr;
      FormatType type;
      अचिन्हित पूर्णाङ्क flags = 0;
      पूर्णाङ्क width = 0;
      पूर्णाङ्क precision = 0;
      पूर्णाङ्क param = -1;
      पूर्णाङ्क rc;
      fmt++;
      outlen = (माप_प्रकार)(fmt - start - 1);
      यदि(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        यदि(outlen) {
          optr = &out[ocount++];
          यदि(ocount > MAX_SEGMENTS)
            फर्काऊ PFMT_MANYSEGS;
          optr->आगत = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        जारी; /* while */
      }

      यदि(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        यदि(param < 0) {
          यदि(use_dollar == DOLLAR_USE)
            /* illegal combo */
            फर्काऊ PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        अन्यथा
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      यदि(rc)
        फर्काऊ rc;

      यदि(parse_conversion(*fmt, &flags, &type))
        जारी;

      यदि(flags & FLAGS_WIDTHPARAM) {
        यदि(width < 0)
          width = param_अङ्क++;
        अन्यथा {
          /* if this identifies a parameter already used, this is illegal */
          यदि(is_arg_used(usedinput, width))
            फर्काऊ PFMT_WIDTHARG;
        }
        यदि(width >= MAX_PARAMETERS)
          फर्काऊ PFMT_MANYARGS;
        यदि(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      यदि(flags & FLAGS_PRECPARAM) {
        यदि(precision < 0)
          precision = param_अङ्क++;
        अन्यथा {
          /* if this identifies a parameter already used, this is illegal */
          यदि(is_arg_used(usedinput, precision))
            फर्काऊ PFMT_PRECARG;
        }
        यदि(precision >= MAX_PARAMETERS)
          फर्काऊ PFMT_MANYARGS;
        यदि(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      यदि(param < 0)
        param = param_अङ्क++;
      यदि(param >= MAX_PARAMETERS)
        फर्काऊ PFMT_MANYARGS;
      यदि(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      यदि(ocount > MAX_SEGMENTS)
        फर्काऊ PFMT_MANYSEGS;
      optr->आगत = (अचिन्हित पूर्णाङ्क)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    अन्यथा
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (माप_प्रकार)(fmt - start);
  यदि(outlen) {
    optr = &out[ocount++];
    यदि(ocount > MAX_SEGMENTS)
      फर्काऊ PFMT_MANYSEGS;
    optr->आगत = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  लागि(i = 0; i < max_param + 1; i++) {
    संरचना va_आगत *iptr = &in[i];
    यदि(!is_arg_used(usedinput, i))
      /* bad input */
      फर्काऊ PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    विकल्प(iptr->type) {
    पक्ष MTYPE_STRING:
      iptr->val.str = बहु_तर्क(arglist, अचर वर्ण *);
      रोक;

    पक्ष MTYPE_INTPTR:
    पक्ष MTYPE_PTR:
      iptr->val.ptr = बहु_तर्क(arglist, शून्य *);
      रोक;

    पक्ष MTYPE_LONGLONGU:
      iptr->val.numu = बहु_तर्क(arglist, uint64_t);
      रोक;

    पक्ष MTYPE_LONGLONG:
      iptr->val.nums = बहु_तर्क(arglist, int64_t);
      रोक;

    पक्ष MTYPE_LONGU:
      iptr->val.numu = बहु_तर्क(arglist, अचिन्हित दीर्घ);
      रोक;

    पक्ष MTYPE_LONG:
      iptr->val.nums = बहु_तर्क(arglist, दीर्घ);
      रोक;

    पक्ष MTYPE_INTU:
      iptr->val.numu = बहु_तर्क(arglist, अचिन्हित पूर्णाङ्क);
      रोक;

    पक्ष MTYPE_INT:
    पक्ष MTYPE_WIDTH:
    पक्ष MTYPE_PRECISION:
      iptr->val.nums = बहु_तर्क(arglist, पूर्णाङ्क);
      रोक;

    पक्ष MTYPE_DOUBLE:
      iptr->val.dnum = बहु_तर्क(arglist, द्विगुण);
      रोक;

    पक्ष MTYPE_LONGDOUBLE:
      iptr->val.dnum = (द्विगुण)बहु_तर्क(arglist, दीर्घ द्विगुण);
      रोक;

    स्वभाव:
      DEBUGASSERT(रिक्त); /* unexpected */
      रोक;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  फर्काऊ PFMT_OK;
}

संरचना mproperty {
  पूर्णाङ्क width;            /* Width of a field. */
  पूर्णाङ्क prec;             /* Precision of a field. */
  अचिन्हित पूर्णाङ्क flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
प्रकारनिर्देश माप_प्रकार (*mp_streamn)(अचर अचिन्हित वर्ण *buf, माप_प्रकार len, शून्य *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
प्रकारनिर्देश पूर्णाङ्क (*mp_flush)(शून्य *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
स्थिर bool stream_run(शून्य *userp,
                       पूर्णाङ्क (*stream)(अचिन्हित वर्ण, शून्य *),
                       mp_streamn streamn,
                       अचर अचिन्हित वर्ण *buf, माप_प्रकार len, पूर्णाङ्क *donep)
{
  यदि(!len)
    फर्काऊ FALSE;
  यदि(streamn) {
    माप_प्रकार n = streamn(buf, len, userp);
    *donep += (पूर्णाङ्क)n;
    फर्काऊ n != len;
  }
  जबसम्म(len--) {
    यदि(stream(*buf++, userp))
      फर्काऊ TRUE;
    (*donep)++;
  }
  फर्काऊ FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
स्थिर अचर वर्ण pad_spaces[] = "                ";
स्थिर अचर वर्ण pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
स्थिर bool stream_pad(शून्य *userp,
                       पूर्णाङ्क (*stream)(अचिन्हित वर्ण, शून्य *),
                       mp_streamn streamn,
                       अचिन्हित वर्ण pad, पूर्णाङ्क अङ्क, पूर्णाङ्क *donep)
{
  अचर अचिन्हित वर्ण *src;
  यदि(अङ्क <= 0)
    फर्काऊ FALSE;
  यदि(pad == '0')
    src = (अचर अचिन्हित वर्ण *)pad_zeros;
  अन्यथा
    src = (अचर अचिन्हित वर्ण *)pad_spaces;
  जबसम्म(अङ्क > PAD_RUN) {
    यदि(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      फर्काऊ TRUE;
    अङ्क -= PAD_RUN;
  }
  फर्काऊ stream_run(userp, stream, streamn, src, (माप_प्रकार)अङ्क, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
स्थिर bool stream_zrun(शून्य *userp,
                        पूर्णाङ्क (*stream)(अचिन्हित वर्ण, शून्य *),
                        mp_streamn streamn,
                        अचर वर्ण *str, माप_प्रकार len, पूर्णाङ्क *donep)
{
  माप_प्रकार n = 0;
  जबसम्म(n < len && str[n])
    n++;
  फर्काऊ stream_run(userp, stream, streamn,
                    (अचर अचिन्हित वर्ण *)str, n, donep);
}

स्थिर bool out_double(शून्य *userp,
                       पूर्णाङ्क (*stream)(अचिन्हित वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       द्विगुण dnum,
                       वर्ण *work, पूर्णाङ्क *donep)
{
  वर्ण fmt[32] = "%";
  वर्ण *fptr = &fmt[1];
  माप_प्रकार बायाँ = परिमाण(fmt) - म_माप(fmt);
  पूर्णाङ्क flags = p->flags;
  पूर्णाङ्क width = p->width;
  पूर्णाङ्क prec = p->prec;

  यदि(flags & FLAGS_बायाँ)
    *fptr++ = '-';
  यदि(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  यदि(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  यदि(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  यदि(width >= 0) {
    माप_प्रकार dlen;
    यदि(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (माप_प्रकार)curl_msnprintf(fptr, बायाँ, "%d", width);
    fptr += dlen;
    बायाँ -= dlen;
  }
  यदि(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    पूर्णाङ्क maxprec = BUFFSIZE - 1;
    द्विगुण val = dnum;
    पूर्णाङ्क len;
    यदि(prec > maxprec)
      prec = maxprec - 1;
    यदि(width > 0 && prec <= width)
      maxprec -= width;
    जबसम्म(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    यदि(prec > maxprec)
      prec = maxprec - 1;
    यदि(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, बायाँ, ".%d", prec);
    fptr += len;
  }
  यदि(flags & FLAGS_LONG)
    *fptr++ = 'l';

  यदि(flags & FLAGS_FLOATE)
    *fptr++ = (वर्ण)((flags & FLAGS_UPPER) ? 'E' : 'e');
  अन्यथा यदि(flags & FLAGS_FLOATG)
    *fptr++ = (वर्ण)((flags & FLAGS_UPPER) ? 'G' : 'g');
  अन्यथा
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic थप
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic झिक
#endif
  DEBUGASSERT(म_माप(work) < BUFFSIZE);
  फर्काऊ stream_run(userp, stream, streamn,
                    (अचर अचिन्हित वर्ण *)work, म_माप(work), donep);
}

स्थिर bool out_सङ्ख्या(शून्य *userp,
                       पूर्णाङ्क (*stream)(अचिन्हित वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       uint64_t अङ्क,
                       int64_t nums,
                       वर्ण *work, पूर्णाङ्क *donep)
{
  अचर अचिन्हित वर्ण *digits = Curl_ldigits;
  पूर्णाङ्क flags = p->flags;
  पूर्णाङ्क width = p->width;
  पूर्णाङ्क prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  पूर्णाङ्क base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  वर्ण *workend = &work[BUFFSIZE - 2];
  वर्ण *w;

  यदि(flags & FLAGS_CHAR) {
    /* Character. */
    यदि(!(flags & FLAGS_बायाँ)) {
      यदि(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        फर्काऊ TRUE;
      width = 1;
    }
    OUTCHAR((वर्ण)अङ्क);
    यदि(flags & FLAGS_बायाँ) {
      यदि(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        फर्काऊ TRUE;
    }
    फर्काऊ FALSE;
  }
  यदि(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  अन्यथा यदि(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  अन्यथा यदि(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  अन्यथा {
    /* Decimal integer. */
    is_neg = (nums < 0);
    यदि(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_अङ्क; /* Used to convert negative in positive. */
      signed_अङ्क = nums + (int64_t)1;
      signed_अङ्क = -signed_अङ्क;
      अङ्क = (uint64_t)signed_अङ्क;
      अङ्क += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  यदि(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  विकल्प(base) {
  पक्ष 10:
    जबसम्म(अङ्क > 0) {
      *w-- = (वर्ण)('0' + (अङ्क % 10));
      अङ्क /= 10;
    }
    रोक;
  स्वभाव:
    जबसम्म(अङ्क > 0) {
      *w-- = digits[अङ्क % base];
      अङ्क /= base;
    }
    रोक;
  }
  width -= (पूर्णाङ्क)(workend - w);
  prec -= (पूर्णाङ्क)(workend - w);

  यदि(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  यदि(prec > 0) {
    width -= prec;
    जबसम्म(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  यदि(is_alt && base == 16)
    width -= 2;

  यदि(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  यदि(!(flags & FLAGS_बायाँ) && !(flags & FLAGS_PAD_NIL)) {
    यदि(stream_pad(userp, stream, streamn, ' ', width, donep))
      फर्काऊ TRUE;
    width = 0;
  }

  यदि(is_neg)
    OUTCHAR('-');
  अन्यथा यदि(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  अन्यथा यदि(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  यदि(is_alt && base == 16) {
    OUTCHAR('0');
    यदि(flags & FLAGS_UPPER)
      OUTCHAR('X');
    अन्यथा
      OUTCHAR('x');
  }

  यदि(!(flags & FLAGS_बायाँ) && (flags & FLAGS_PAD_NIL)) {
    यदि(stream_pad(userp, stream, streamn, '0', width, donep))
      फर्काऊ TRUE;
    width = 0;
  }

  /* Write the number. */
  यदि(stream_run(userp, stream, streamn, (अचर अचिन्हित वर्ण *)w + 1,
                (माप_प्रकार)(workend - w), donep))
    फर्काऊ TRUE;

  यदि(flags & FLAGS_बायाँ) {
    यदि(stream_pad(userp, stream, streamn, ' ', width, donep))
      फर्काऊ TRUE;
  }

  फर्काऊ FALSE;
}

स्थिर अचर वर्ण nilstr[] = "(nil)";

स्थिर bool out_string(शून्य *userp,
                       पूर्णाङ्क (*stream)(अचिन्हित वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       अचर वर्ण *str,
                       पूर्णाङ्क *donep)
{
  पूर्णाङ्क flags = p->flags;
  पूर्णाङ्क width = p->width;
  पूर्णाङ्क prec = p->prec;
  माप_प्रकार len;

  यदि(!str) {
    /* Write null string if there is space. */
    यदि(prec == -1 || prec >= (पूर्णाङ्क)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(अचिन्हित पूर्णाङ्क)FLAGS_ALT;
    }
    अन्यथा {
      str = "";
      len = 0;
    }
  }
  अन्यथा यदि(prec != -1)
    len = (माप_प्रकार)prec;
  अन्यथा यदि(*str == '\0')
    len = 0;
  अन्यथा
    len = म_माप(str);

  width -= (len > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णाङ्क)len;

  यदि(flags & FLAGS_ALT)
    OUTCHAR('"');

  यदि(!(flags & FLAGS_बायाँ)) {
    यदि(stream_pad(userp, stream, streamn, ' ', width, donep))
      फर्काऊ TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  यदि(prec == -1) {
    यदि(stream_run(userp, stream, streamn,
                  (अचर अचिन्हित वर्ण *)str, len, donep))
      फर्काऊ TRUE;
  }
  अन्यथा यदि(stream_zrun(userp, stream, streamn, str, len, donep))
    फर्काऊ TRUE;
  यदि(flags & FLAGS_बायाँ) {
    यदि(stream_pad(userp, stream, streamn, ' ', width, donep))
      फर्काऊ TRUE;
  }

  यदि(flags & FLAGS_ALT)
    OUTCHAR('"');

  फर्काऊ FALSE;
}

स्थिर bool out_pointer(शून्य *userp,
                        पूर्णाङ्क (*stream)(अचिन्हित वर्ण, शून्य *),
                        mp_streamn streamn,
                        संरचना mproperty *p,
                        अचर वर्ण *ptr,
                        वर्ण *work,
                        पूर्णाङ्क *donep)
{
  /* Generic pointer. */
  यदि(ptr) {
    माप_प्रकार अङ्क = (माप_प्रकार)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    यदि(out_सङ्ख्या(userp, stream, streamn, p, अङ्क, 0, work, donep))
      फर्काऊ TRUE;
  }
  अन्यथा {
    /* Write "(nil)" for a nil pointer. */
    पूर्णाङ्क width = p->width;
    पूर्णाङ्क flags = p->flags;

    width -= (पूर्णाङ्क)CURL_CSTRLEN(nilstr);
    यदि(flags & FLAGS_बायाँ) {
      यदि(stream_pad(userp, stream, streamn, ' ', width, donep))
        फर्काऊ TRUE;
      width = 0;
    }
    यदि(stream_run(userp, stream, streamn, (अचर अचिन्हित वर्ण *)nilstr,
                  परिमाण(nilstr) - 1, donep))
      फर्काऊ TRUE;
    यदि(!(flags & FLAGS_बायाँ)) {
      यदि(stream_pad(userp, stream, streamn, ' ', width, donep))
        फर्काऊ TRUE;
    }
  }
  फर्काऊ FALSE;
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
                   पूर्णाङ्क (*stream)(अचिन्हित वर्ण, शून्य *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   अचर वर्ण *format, /* %-formatted string */
                   बहु_सूची ap_save) /* list of parameters */
{
  पूर्णाङ्क done = 0;   /* number of characters written */
  पूर्णाङ्क i;
  पूर्णाङ्क ocount = 0; /* number of output segments */
  पूर्णाङ्क icount = 0; /* number of input arguments */

  संरचना outsegment output[MAX_SEGMENTS];
  संरचना va_आगत आगत[MAX_PARAMETERS];
  वर्ण work[BUFFSIZE + 2];

  /* Parse the format string */
  यदि(parsefmt(format, output, आगत, &ocount, &icount, ap_save))
    फर्काऊ 0;

  लागि(i = 0; i < ocount; i++) {
    संरचना outsegment *optr = &output[i];
    संरचना va_आगत *iptr = &आगत[optr->आगत];
    संरचना mproperty p;
    माप_प्रकार outlen = optr->outlen;

    यदि(outlen) {
      यदि(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        फर्काऊ done;
      यदि(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        जारी;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    यदि(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (पूर्णाङ्क)आगत[optr->width].val.nums;
      यदि(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        यदि(p.width == पूर्णांक_न्यून)
          p.width = पूर्णांक_उच्च;
        अन्यथा
          p.width = -p.width;
        p.flags |= FLAGS_बायाँ;
        p.flags &= ~(अचिन्हित पूर्णाङ्क)FLAGS_PAD_NIL;
      }
    }
    अन्यथा
      p.width = optr->width;

    /* pick up the specified precision */
    यदि(p.flags & FLAGS_PRECPARAM) {
      p.prec = (पूर्णाङ्क)आगत[optr->precision].val.nums;
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
      यदि(out_सङ्ख्या(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        फर्काऊ done;
      रोक;

    पक्ष MTYPE_INT:
    पक्ष MTYPE_LONG:
    पक्ष MTYPE_LONGLONG:
      यदि(out_सङ्ख्या(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        फर्काऊ done;
      रोक;

    पक्ष MTYPE_STRING:
      यदि(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        फर्काऊ done;
      रोक;

    पक्ष MTYPE_PTR:
      यदि(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        फर्काऊ done;
      रोक;

    पक्ष MTYPE_DOUBLE:
    पक्ष MTYPE_LONGDOUBLE:
      यदि(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        फर्काऊ done;
      रोक;

    पक्ष MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      यदि(flush && flush(userp))
        फर्काऊ done;
      /* Answer the count of characters written. */
      यदि(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      अन्यथा यदि(p.flags & FLAGS_LONG)
        *(दीर्घ *)iptr->val.ptr = (दीर्घ)done;
      अन्यथा यदि(!(p.flags & FLAGS_SHORT))
        *(पूर्णाङ्क *)iptr->val.ptr = done;
      अन्यथा
        *(लघु *)iptr->val.ptr = (लघु)done;
      रोक;

    स्वभाव:
      रोक;
    }
  }
  फर्काऊ done;
}

/* fputc() look-alike */
स्थिर पूर्णाङ्क addbyter(अचिन्हित वर्ण outc, शून्य *f)
{
  संरचना nsprintf *infop = f;
  यदि(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (वर्ण)outc; /* store */
    infop->length++; /* we are now one byte larger */
    फर्काऊ 0;     /* fputc() returns like this on success */
  }
  फर्काऊ 1;
}

/* block variant of addbyter */
स्थिर माप_प्रकार addrun(अचर अचिन्हित वर्ण *buf, माप_प्रकार len, शून्य *f)
{
  संरचना nsprintf *infop = f;
  माप_प्रकार fit = infop->max - infop->length;
  यदि(len > fit)
    len = fit;
  यदि(len) {
    स_नकल(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  फर्काऊ len;
}

पूर्णाङ्क curl_mvsnprintf(वर्ण *buffer, माप_प्रकार maxlength, अचर वर्ण *format,
                    बहु_सूची args)
{
  पूर्णाङ्क retcode;
  संरचना nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, रिक्त, format, args);
  यदि(info.max) {
    /* we null-terminate this */
    यदि(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    अन्यथा
      info.buffer[0] = 0;
  }
  फर्काऊ retcode;
}

पूर्णाङ्क curl_msnprintf(वर्ण *buffer, माप_प्रकार maxlength, अचर वर्ण *format, ...)
{
  पूर्णाङ्क retcode;
  बहु_सूची args; /* argument pointer */
  बहु_शुरू(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  बहु_पूर्ण(args);
  फर्काऊ retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
स्थिर पूर्णाङ्क alloc_flush(शून्य *f)
{
  संरचना asprintf *infop = f;
  यदि(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    यदि(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      फर्काऊ 1; /* fail */
    }
  }
  फर्काऊ 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
स्थिर पूर्णाङ्क alloc_addn(संरचना asprintf *infop, अचर अचिन्हित वर्ण *buf,
                      माप_प्रकार len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  यदि(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    फर्काऊ 1; /* fail */
  }
  फर्काऊ 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
स्थिर bool alloc_fits(संरचना asprintf *infop, माप_प्रकार more)
{
  फर्काऊ (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
स्थिर पूर्णाङ्क alloc_addbyter(अचिन्हित वर्ण outc, शून्य *f)
{
  संरचना asprintf *infop = f;
  यदि(infop->nstage == परिमाण(infop->stage) && alloc_flush(infop))
    फर्काऊ 1; /* fail */
  यदि(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    यदि(alloc_flush(infop))
      फर्काऊ 1;
    फर्काऊ alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  फर्काऊ 0;
}

/* block variant of alloc_addbyter */
स्थिर माप_प्रकार alloc_addrun(अचर अचिन्हित वर्ण *buf, माप_प्रकार len, शून्य *f)
{
  संरचना asprintf *infop = f;
  माप_प्रकार accepted = 0;
  जबसम्म(accepted < len) {
    माप_प्रकार room = परिमाण(infop->stage) - infop->nstage;
    माप_प्रकार n;
    यदि(!room) {
      यदि(alloc_flush(infop))
        फर्काऊ accepted; /* accepted < len aborts formatting */
      room = परिमाण(infop->stage);
    }
    n = len - accepted;
    यदि(n > room)
      n = room;
    यदि(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      माप_प्रकार fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      यदि(fits) {
        स_नकल(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      यदि(alloc_flush(infop))
        फर्काऊ accepted;
      यदि(alloc_addn(infop, buf + accepted, 1))
        फर्काऊ accepted; /* accepted < len aborts formatting */
      accepted++;
      जारी;
    }
    स_नकल(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  फर्काऊ accepted;
}

/* appends the formatted string, returns MERR error code */
पूर्णाङ्क curlx_dyn_vprintf(संरचना dynbuf *dyn, अचर वर्ण *format, बहु_सूची args)
{
  संरचना asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (शून्य)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  यदि(!info.merr)
    (शून्य)alloc_flush(&info);
  यदि(info.merr) {
    curlx_dyn_free(info.b);
    फर्काऊ info.merr;
  }
  फर्काऊ 0;
}

वर्ण *curl_mvaprintf(अचर वर्ण *format, बहु_सूची args)
{
  संरचना asprintf info;
  संरचना dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (शून्य)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  यदि(!info.merr)
    (शून्य)alloc_flush(&info);
  यदि(info.merr) {
    curlx_dyn_free(info.b);
    फर्काऊ रिक्त;
  }
  यदि(curlx_dyn_len(info.b))
    फर्काऊ curlx_dyn_ptr(info.b);
  फर्काऊ curlx_strdup("");
}

वर्ण *curl_maprintf(अचर वर्ण *format, ...)
{
  बहु_सूची args;
  वर्ण *s;
  बहु_शुरू(args, format);
  s = curl_mvaprintf(format, args);
  बहु_पूर्ण(args);
  फर्काऊ s;
}

स्थिर पूर्णाङ्क storebuffer(अचिन्हित वर्ण outc, शून्य *f)
{
  वर्ण **buffer = f;
  **buffer = (वर्ण)outc;
  (*buffer)++;
  फर्काऊ 0;
}

/* block variant of storebuffer */
स्थिर माप_प्रकार storerun(अचर अचिन्हित वर्ण *buf, माप_प्रकार len, शून्य *f)
{
  वर्ण **buffer = f;
  स_नकल(*buffer, buf, len);
  *buffer += len;
  फर्काऊ len;
}

पूर्णाङ्क curl_msprintf(वर्ण *buffer, अचर वर्ण *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णाङ्क retcode;
  बहु_शुरू(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, रिक्त, format, args);
  बहु_पूर्ण(args);
  *buffer = 0; /* we null-terminate this */
  फर्काऊ retcode;
}

स्थिर पूर्णाङ्क fputc_wrapper(अचिन्हित वर्ण outc, शून्य *f)
{
  संरचना ioprintf *i = f;
  पूर्णाङ्क out = outc;
  खाता *s = i->fd;
  पूर्णाङ्क rc = ख_अक्षर_दो(out, s);
  i->error = (rc == खातापूर्ण);
  फर्काऊ (rc == खातापूर्ण);
}

/* block variant of fputc_wrapper */
स्थिर माप_प्रकार fwrite_wrapper(अचर अचिन्हित वर्ण *buf, माप_प्रकार len, शून्य *f)
{
  संरचना ioprintf *i = f;
  माप_प्रकार n;
  खाता *s = i->fd;
  n = ख_डालो(buf, 1, len, s);
  i->error = (n != len);
  फर्काऊ n;
}

पूर्णाङ्क curl_mvfprintf(खाता *fd, अचर वर्ण *format, बहु_सूची args)
{
  संरचना ioprintf info;
  पूर्णाङ्क n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, रिक्त, format, args);
  यदि(info.error)
    फर्काऊ -1;
  फर्काऊ n;
}

पूर्णाङ्क curl_mfprintf(खाता *fd, अचर वर्ण *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णाङ्क n;
  बहु_शुरू(args, format);
  n = curl_mvfprintf(fd, format, args);
  बहु_पूर्ण(args);
  फर्काऊ n;
}

पूर्णाङ्क curl_mprintf(अचर वर्ण *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णाङ्क n;
  बहु_शुरू(args, format);
  n = curl_mvfprintf(मानक_निकास, format, args);
  बहु_पूर्ण(args);
  फर्काऊ n;
}

पूर्णाङ्क curl_mvsprintf(वर्ण *buffer, अचर वर्ण *format, बहु_सूची args)
{
  पूर्णाङ्क retcode = formatf(&buffer, storebuffer, storerun, रिक्त, format, args);
  *buffer = 0; /* we null-terminate this */
  फर्काऊ retcode;
}

पूर्णाङ्क curl_mvprintf(अचर वर्ण *format, बहु_सूची args)
{
  फर्काऊ curl_mvfprintf(मानक_निकास, format, args);
}

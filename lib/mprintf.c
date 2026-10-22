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
ध्रुव unsigned वर्ण Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ध्रुव unsigned वर्ण Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  करोहि {                                    \
    सचे(stream((unsigned वर्ण)(x), userp)) \
      पटिनिवत्त TRUE;                        \
    (*donep)++;                           \
  } याव(0)

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
  पूर्णाङ्क width;     /* width OR width parameter number */
  पूर्णाङ्क precision; /* precision OR precision parameter number */
  unsigned पूर्णाङ्क flags;
  unsigned पूर्णाङ्क निवेशः; /* input argument array index */
  ध्रुव वर्ण *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

संरचना nsprintf {
  वर्ण *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

संरचना asprintf {
  संरचना dynbuf *b;
  वर्ण merr;
  size_t nstage;
  unsigned वर्ण stage[ASPRINTF_STAGE_SIZE];
};

संरचना ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
स्थिर पूर्णाङ्क dollarstring(ध्रुव वर्ण *p, ध्रुव वर्ण **end)
{
  curl_off_t अङ्कः;
  सचे(curlx_str_सङ्ख्या(&p, &अङ्कः, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !अङ्कः)
    पटिनिवत्त -1;
  *end = p;
  पटिनिवत्त (पूर्णाङ्क)अङ्कः - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned वर्ण)(1 << ((y) & 7)))

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

स्थिर पूर्णाङ्क parse_flags(ध्रुव वर्ण **fmtp, unsigned पूर्णाङ्क *flagsp, पूर्णाङ्क use_dollar,
                       पूर्णाङ्क *precp, पूर्णाङ्क *widthp)
{
  ध्रुव वर्ण *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned पूर्णाङ्क flags = 0;
  पूर्णाङ्क width = 0;
  पूर्णाङ्क precision = 0;

  /* Handle the flags */
  करोहि {
    विकल्प(*fmt++) {
    पक्ष ' ':
      flags |= FLAGS_SPACE;
      भङ्ग;
    पक्ष '+':
      flags |= FLAGS_SHOWSIGN;
      भङ्ग;
    पक्ष '-':
      flags |= FLAGS_वामः;
      flags &= ~(unsigned पूर्णाङ्क)FLAGS_PAD_NIL;
      भङ्ग;
    पक्ष '#':
      flags |= FLAGS_ALT;
      भङ्ग;
    पक्ष '.':
      सचे('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        सचे(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          सचे(precision < 0)
            /* illegal combo */
            पटिनिवत्त PFMT_DOLLARPREC;
        }
        अञ्ञथा
          /* get it from the next argument */
          precision = -1;
      }
      अञ्ञथा {
        bool is_neg;
        curl_off_t अङ्कः;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        सचे(is_neg)
          fmt++;
        सचे(curlx_str_सङ्ख्या(&fmt, &अङ्कः, INT_MAX))
          पटिनिवत्त PFMT_PREC;
        precision = (पूर्णाङ्क)अङ्कः;
        सचे(is_neg)
          precision = -precision;
      }
      सचे((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        पटिनिवत्त PFMT_PRECMIX;
      भङ्ग;
    पक्ष 'h':
      flags |= FLAGS_SHORT;
      भङ्ग;
#ifdef _WIN32
    पक्ष 'I':
      /* Non-ANSI integer extensions I32 I64 */
      सचे((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      अञ्ञथा सचे((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      अञ्ञथा {
#सचे SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#अञ्ञथा
        flags |= FLAGS_LONG;
#endif
      }
      भङ्ग;
#endif /* _WIN32 */
    पक्ष 'l':
      सचे(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      अञ्ञथा
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
         used */
#सचे SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अञ्ञथा
      flags |= FLAGS_LONG;
#endif
      भङ्ग;
    पक्ष 'O':
#सचे SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अञ्ञथा
      flags |= FLAGS_LONG;
#endif
      भङ्ग;
    पक्ष '0':
      सचे(!(flags & FLAGS_वामः))
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
      flags |= FLAGS_WIDTH;
      fmt--;
      सचे(curlx_str_सङ्ख्या(&fmt, &अङ्कः, INT_MAX))
        पटिनिवत्त PFMT_WIDTH;
      width = (पूर्णाङ्क)अङ्कः;
      भङ्ग;
    }
    पक्ष '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      सचे(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        सचे(width < 0)
          /* illegal combo */
          पटिनिवत्त PFMT_DOLLARWIDTH;
      }
      अञ्ञथा
        /* pick from the next argument */
        width = -1;
      भङ्ग;
    स्वभाव:
      loopit = FALSE;
      fmt--;
      भङ्ग;
    } /* switch */
  } याव(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  पटिनिवत्त PFMT_OK;
}

स्थिर bool parse_conversion(ध्रुव वर्ण f, unsigned पूर्णाङ्क *flagp,
                             FormatType *typep)
{
  unsigned पूर्णाङ्क flags = *flagp;
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
    सचे(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    अञ्ञथा सचे(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    अञ्ञथा
      type = MTYPE_INT;
    भङ्ग;
  पक्ष 'u':
    सचे(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अञ्ञथा सचे(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अञ्ञथा
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    भङ्ग;
  पक्ष 'o':
    सचे(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अञ्ञथा सचे(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अञ्ञथा
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    भङ्ग;
  पक्ष 'x':
    सचे(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अञ्ञथा सचे(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अञ्ञथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    भङ्ग;
  पक्ष 'X':
    सचे(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अञ्ञथा सचे(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अञ्ञथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
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
    flags |= FLAGS_UPPER;
    भङ्ग;
  पक्ष 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    भङ्ग;
  पक्ष 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    भङ्ग;
  पक्ष 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    भङ्ग;
  पक्ष 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    भङ्ग;
  स्वभाव:
    /* invalid instruction, disregard and continue */
    पटिनिवत्त TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  पटिनिवत्त FALSE;
}

स्थिर पूर्णाङ्क parsefmt(ध्रुव वर्ण *format,
                    संरचना outsegment *out,
                    संरचना va_निवेशः *in,
                    पूर्णाङ्क *opieces,
                    पूर्णाङ्क *ipieces, va_list arglist)
{
  ध्रुव वर्ण *fmt = format;
  पूर्णाङ्क param_अङ्कः = 0;
  पूर्णाङ्क max_param = -1;
  पूर्णाङ्क i;
  पूर्णाङ्क ocount = 0;
  unsigned वर्ण usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  संरचना outsegment *optr;
  पूर्णाङ्क use_dollar = DOLLAR_UNKNOWN;
  ध्रुव वर्ण *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, परिमाण(usedinput));

  याव(*fmt) {
    सचे(*fmt == '%') {
      संरचना va_निवेशः *iptr;
      FormatType type;
      unsigned पूर्णाङ्क flags = 0;
      पूर्णाङ्क width = 0;
      पूर्णाङ्क precision = 0;
      पूर्णाङ्क param = -1;
      पूर्णाङ्क rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      सचे(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        सचे(outlen) {
          optr = &out[ocount++];
          सचे(ocount > MAX_SEGMENTS)
            पटिनिवत्त PFMT_MANYSEGS;
          optr->निवेशः = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        अनुवर्त; /* while */
      }

      सचे(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        सचे(param < 0) {
          सचे(use_dollar == DOLLAR_USE)
            /* illegal combo */
            पटिनिवत्त PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        अञ्ञथा
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      सचे(rc)
        पटिनिवत्त rc;

      सचे(parse_conversion(*fmt, &flags, &type))
        अनुवर्त;

      सचे(flags & FLAGS_WIDTHPARAM) {
        सचे(width < 0)
          width = param_अङ्कः++;
        अञ्ञथा {
          /* if this identifies a parameter already used, this is illegal */
          सचे(is_arg_used(usedinput, width))
            पटिनिवत्त PFMT_WIDTHARG;
        }
        सचे(width >= MAX_PARAMETERS)
          पटिनिवत्त PFMT_MANYARGS;
        सचे(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      सचे(flags & FLAGS_PRECPARAM) {
        सचे(precision < 0)
          precision = param_अङ्कः++;
        अञ्ञथा {
          /* if this identifies a parameter already used, this is illegal */
          सचे(is_arg_used(usedinput, precision))
            पटिनिवत्त PFMT_PRECARG;
        }
        सचे(precision >= MAX_PARAMETERS)
          पटिनिवत्त PFMT_MANYARGS;
        सचे(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      सचे(param < 0)
        param = param_अङ्कः++;
      सचे(param >= MAX_PARAMETERS)
        पटिनिवत्त PFMT_MANYARGS;
      सचे(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      सचे(ocount > MAX_SEGMENTS)
        पटिनिवत्त PFMT_MANYSEGS;
      optr->निवेशः = (unsigned पूर्णाङ्क)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    अञ्ञथा
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  सचे(outlen) {
    optr = &out[ocount++];
    सचे(ocount > MAX_SEGMENTS)
      पटिनिवत्त PFMT_MANYSEGS;
    optr->निवेशः = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  अत्थाय(i = 0; i < max_param + 1; i++) {
    संरचना va_निवेशः *iptr = &in[i];
    सचे(!is_arg_used(usedinput, i))
      /* bad input */
      पटिनिवत्त PFMT_INPUTGAP;

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
      iptr->val.numu = va_arg(arglist, unsigned long);
      भङ्ग;

    पक्ष MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      भङ्ग;

    पक्ष MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned पूर्णाङ्क);
      भङ्ग;

    पक्ष MTYPE_INT:
    पक्ष MTYPE_WIDTH:
    पक्ष MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, पूर्णाङ्क);
      भङ्ग;

    पक्ष MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, द्विगुण);
      भङ्ग;

    पक्ष MTYPE_LONGDOUBLE:
      iptr->val.dnum = (द्विगुण)va_arg(arglist, long द्विगुण);
      भङ्ग;

    स्वभाव:
      DEBUGASSERT(रिक्त); /* unexpected */
      भङ्ग;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  पटिनिवत्त PFMT_OK;
}

संरचना mproperty {
  पूर्णाङ्क width;            /* Width of a field. */
  पूर्णाङ्क prec;             /* Precision of a field. */
  unsigned पूर्णाङ्क flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
प्रकारनिर्देश size_t (*mp_streamn)(ध्रुव unsigned वर्ण *buf, size_t len, शून्य *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
प्रकारनिर्देश पूर्णाङ्क (*mp_flush)(शून्य *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
स्थिर bool stream_run(शून्य *userp,
                       पूर्णाङ्क (*stream)(unsigned वर्ण, शून्य *),
                       mp_streamn streamn,
                       ध्रुव unsigned वर्ण *buf, size_t len, पूर्णाङ्क *donep)
{
  सचे(!len)
    पटिनिवत्त FALSE;
  सचे(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (पूर्णाङ्क)n;
    पटिनिवत्त n != len;
  }
  याव(len--) {
    सचे(stream(*buf++, userp))
      पटिनिवत्त TRUE;
    (*donep)++;
  }
  पटिनिवत्त FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
स्थिर ध्रुव वर्ण pad_spaces[] = "                ";
स्थिर ध्रुव वर्ण pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
स्थिर bool stream_pad(शून्य *userp,
                       पूर्णाङ्क (*stream)(unsigned वर्ण, शून्य *),
                       mp_streamn streamn,
                       unsigned वर्ण pad, पूर्णाङ्क अङ्कः, पूर्णाङ्क *donep)
{
  ध्रुव unsigned वर्ण *src;
  सचे(अङ्कः <= 0)
    पटिनिवत्त FALSE;
  सचे(pad == '0')
    src = (ध्रुव unsigned वर्ण *)pad_zeros;
  अञ्ञथा
    src = (ध्रुव unsigned वर्ण *)pad_spaces;
  याव(अङ्कः > PAD_RUN) {
    सचे(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      पटिनिवत्त TRUE;
    अङ्कः -= PAD_RUN;
  }
  पटिनिवत्त stream_run(userp, stream, streamn, src, (size_t)अङ्कः, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
स्थिर bool stream_zrun(शून्य *userp,
                        पूर्णाङ्क (*stream)(unsigned वर्ण, शून्य *),
                        mp_streamn streamn,
                        ध्रुव वर्ण *str, size_t len, पूर्णाङ्क *donep)
{
  size_t n = 0;
  याव(n < len && str[n])
    n++;
  पटिनिवत्त stream_run(userp, stream, streamn,
                    (ध्रुव unsigned वर्ण *)str, n, donep);
}

स्थिर bool out_double(शून्य *userp,
                       पूर्णाङ्क (*stream)(unsigned वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       द्विगुण dnum,
                       वर्ण *work, पूर्णाङ्क *donep)
{
  वर्ण fmt[32] = "%";
  वर्ण *fptr = &fmt[1];
  size_t वामः = परिमाण(fmt) - strlen(fmt);
  पूर्णाङ्क flags = p->flags;
  पूर्णाङ्क width = p->width;
  पूर्णाङ्क prec = p->prec;

  सचे(flags & FLAGS_वामः)
    *fptr++ = '-';
  सचे(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  सचे(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  सचे(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  सचे(width >= 0) {
    size_t dlen;
    सचे(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, वामः, "%d", width);
    fptr += dlen;
    वामः -= dlen;
  }
  सचे(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    पूर्णाङ्क maxprec = BUFFSIZE - 1;
    द्विगुण val = dnum;
    पूर्णाङ्क len;
    सचे(prec > maxprec)
      prec = maxprec - 1;
    सचे(width > 0 && prec <= width)
      maxprec -= width;
    याव(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    सचे(prec > maxprec)
      prec = maxprec - 1;
    सचे(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, वामः, ".%d", prec);
    fptr += len;
  }
  सचे(flags & FLAGS_LONG)
    *fptr++ = 'l';

  सचे(flags & FLAGS_FLOATE)
    *fptr++ = (वर्ण)((flags & FLAGS_UPPER) ? 'E' : 'e');
  अञ्ञथा सचे(flags & FLAGS_FLOATG)
    *fptr++ = (वर्ण)((flags & FLAGS_UPPER) ? 'G' : 'g');
  अञ्ञथा
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic प्रक्षिप
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#अञ्ञथा
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic निष्कासय
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  पटिनिवत्त stream_run(userp, stream, streamn,
                    (ध्रुव unsigned वर्ण *)work, strlen(work), donep);
}

स्थिर bool out_सङ्ख्या(शून्य *userp,
                       पूर्णाङ्क (*stream)(unsigned वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       uint64_t अङ्कः,
                       int64_t nums,
                       वर्ण *work, पूर्णाङ्क *donep)
{
  ध्रुव unsigned वर्ण *digits = Curl_ldigits;
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

  सचे(flags & FLAGS_CHAR) {
    /* Character. */
    सचे(!(flags & FLAGS_वामः)) {
      सचे(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        पटिनिवत्त TRUE;
      width = 1;
    }
    OUTCHAR((वर्ण)अङ्कः);
    सचे(flags & FLAGS_वामः) {
      सचे(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
  }
  सचे(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  अञ्ञथा सचे(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  अञ्ञथा सचे(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  अञ्ञथा {
    /* Decimal integer. */
    is_neg = (nums < 0);
    सचे(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_अङ्कः; /* Used to convert negative in positive. */
      signed_अङ्कः = nums + (int64_t)1;
      signed_अङ्कः = -signed_अङ्कः;
      अङ्कः = (uint64_t)signed_अङ्कः;
      अङ्कः += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  सचे(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  विकल्प(base) {
  पक्ष 10:
    याव(अङ्कः > 0) {
      *w-- = (वर्ण)('0' + (अङ्कः % 10));
      अङ्कः /= 10;
    }
    भङ्ग;
  स्वभाव:
    याव(अङ्कः > 0) {
      *w-- = digits[अङ्कः % base];
      अङ्कः /= base;
    }
    भङ्ग;
  }
  width -= (पूर्णाङ्क)(workend - w);
  prec -= (पूर्णाङ्क)(workend - w);

  सचे(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  सचे(prec > 0) {
    width -= prec;
    याव(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  सचे(is_alt && base == 16)
    width -= 2;

  सचे(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  सचे(!(flags & FLAGS_वामः) && !(flags & FLAGS_PAD_NIL)) {
    सचे(stream_pad(userp, stream, streamn, ' ', width, donep))
      पटिनिवत्त TRUE;
    width = 0;
  }

  सचे(is_neg)
    OUTCHAR('-');
  अञ्ञथा सचे(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  अञ्ञथा सचे(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  सचे(is_alt && base == 16) {
    OUTCHAR('0');
    सचे(flags & FLAGS_UPPER)
      OUTCHAR('X');
    अञ्ञथा
      OUTCHAR('x');
  }

  सचे(!(flags & FLAGS_वामः) && (flags & FLAGS_PAD_NIL)) {
    सचे(stream_pad(userp, stream, streamn, '0', width, donep))
      पटिनिवत्त TRUE;
    width = 0;
  }

  /* Write the number. */
  सचे(stream_run(userp, stream, streamn, (ध्रुव unsigned वर्ण *)w + 1,
                (size_t)(workend - w), donep))
    पटिनिवत्त TRUE;

  सचे(flags & FLAGS_वामः) {
    सचे(stream_pad(userp, stream, streamn, ' ', width, donep))
      पटिनिवत्त TRUE;
  }

  पटिनिवत्त FALSE;
}

स्थिर ध्रुव वर्ण nilstr[] = "(nil)";

स्थिर bool out_string(शून्य *userp,
                       पूर्णाङ्क (*stream)(unsigned वर्ण, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       ध्रुव वर्ण *str,
                       पूर्णाङ्क *donep)
{
  पूर्णाङ्क flags = p->flags;
  पूर्णाङ्क width = p->width;
  पूर्णाङ्क prec = p->prec;
  size_t len;

  सचे(!str) {
    /* Write null string if there is space. */
    सचे(prec == -1 || prec >= (पूर्णाङ्क)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned पूर्णाङ्क)FLAGS_ALT;
    }
    अञ्ञथा {
      str = "";
      len = 0;
    }
  }
  अञ्ञथा सचे(prec != -1)
    len = (size_t)prec;
  अञ्ञथा सचे(*str == '\0')
    len = 0;
  अञ्ञथा
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (पूर्णाङ्क)len;

  सचे(flags & FLAGS_ALT)
    OUTCHAR('"');

  सचे(!(flags & FLAGS_वामः)) {
    सचे(stream_pad(userp, stream, streamn, ' ', width, donep))
      पटिनिवत्त TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  सचे(prec == -1) {
    सचे(stream_run(userp, stream, streamn,
                  (ध्रुव unsigned वर्ण *)str, len, donep))
      पटिनिवत्त TRUE;
  }
  अञ्ञथा सचे(stream_zrun(userp, stream, streamn, str, len, donep))
    पटिनिवत्त TRUE;
  सचे(flags & FLAGS_वामः) {
    सचे(stream_pad(userp, stream, streamn, ' ', width, donep))
      पटिनिवत्त TRUE;
  }

  सचे(flags & FLAGS_ALT)
    OUTCHAR('"');

  पटिनिवत्त FALSE;
}

स्थिर bool out_pointer(शून्य *userp,
                        पूर्णाङ्क (*stream)(unsigned वर्ण, शून्य *),
                        mp_streamn streamn,
                        संरचना mproperty *p,
                        ध्रुव वर्ण *ptr,
                        वर्ण *work,
                        पूर्णाङ्क *donep)
{
  /* Generic pointer. */
  सचे(ptr) {
    size_t अङ्कः = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    सचे(out_सङ्ख्या(userp, stream, streamn, p, अङ्कः, 0, work, donep))
      पटिनिवत्त TRUE;
  }
  अञ्ञथा {
    /* Write "(nil)" for a nil pointer. */
    पूर्णाङ्क width = p->width;
    पूर्णाङ्क flags = p->flags;

    width -= (पूर्णाङ्क)CURL_CSTRLEN(nilstr);
    सचे(flags & FLAGS_वामः) {
      सचे(stream_pad(userp, stream, streamn, ' ', width, donep))
        पटिनिवत्त TRUE;
      width = 0;
    }
    सचे(stream_run(userp, stream, streamn, (ध्रुव unsigned वर्ण *)nilstr,
                  परिमाण(nilstr) - 1, donep))
      पटिनिवत्त TRUE;
    सचे(!(flags & FLAGS_वामः)) {
      सचे(stream_pad(userp, stream, streamn, ' ', width, donep))
        पटिनिवत्त TRUE;
    }
  }
  पटिनिवत्त FALSE;
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
                   पूर्णाङ्क (*stream)(unsigned वर्ण, शून्य *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ध्रुव वर्ण *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  पूर्णाङ्क done = 0;   /* number of characters written */
  पूर्णाङ्क i;
  पूर्णाङ्क ocount = 0; /* number of output segments */
  पूर्णाङ्क icount = 0; /* number of input arguments */

  संरचना outsegment output[MAX_SEGMENTS];
  संरचना va_निवेशः निवेशः[MAX_PARAMETERS];
  वर्ण work[BUFFSIZE + 2];

  /* Parse the format string */
  सचे(parsefmt(format, output, निवेशः, &ocount, &icount, ap_save))
    पटिनिवत्त 0;

  अत्थाय(i = 0; i < ocount; i++) {
    संरचना outsegment *optr = &output[i];
    संरचना va_निवेशः *iptr = &निवेशः[optr->निवेशः];
    संरचना mproperty p;
    size_t outlen = optr->outlen;

    सचे(outlen) {
      सचे(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        पटिनिवत्त done;
      सचे(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        अनुवर्त;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    सचे(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (पूर्णाङ्क)निवेशः[optr->width].val.nums;
      सचे(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        सचे(p.width == INT_MIN)
          p.width = INT_MAX;
        अञ्ञथा
          p.width = -p.width;
        p.flags |= FLAGS_वामः;
        p.flags &= ~(unsigned पूर्णाङ्क)FLAGS_PAD_NIL;
      }
    }
    अञ्ञथा
      p.width = optr->width;

    /* pick up the specified precision */
    सचे(p.flags & FLAGS_PRECPARAM) {
      p.prec = (पूर्णाङ्क)निवेशः[optr->precision].val.nums;
      सचे(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    अञ्ञथा सचे(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    अञ्ञथा
      p.prec = -1;

    विकल्प(iptr->type) {
    पक्ष MTYPE_INTU:
    पक्ष MTYPE_LONGU:
    पक्ष MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      सचे(out_सङ्ख्या(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        पटिनिवत्त done;
      भङ्ग;

    पक्ष MTYPE_INT:
    पक्ष MTYPE_LONG:
    पक्ष MTYPE_LONGLONG:
      सचे(out_सङ्ख्या(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        पटिनिवत्त done;
      भङ्ग;

    पक्ष MTYPE_STRING:
      सचे(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        पटिनिवत्त done;
      भङ्ग;

    पक्ष MTYPE_PTR:
      सचे(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        पटिनिवत्त done;
      भङ्ग;

    पक्ष MTYPE_DOUBLE:
    पक्ष MTYPE_LONGDOUBLE:
      सचे(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        पटिनिवत्त done;
      भङ्ग;

    पक्ष MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      सचे(flush && flush(userp))
        पटिनिवत्त done;
      /* Answer the count of characters written. */
      सचे(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      अञ्ञथा सचे(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      अञ्ञथा सचे(!(p.flags & FLAGS_SHORT))
        *(पूर्णाङ्क *)iptr->val.ptr = done;
      अञ्ञथा
        *(short *)iptr->val.ptr = (short)done;
      भङ्ग;

    स्वभाव:
      भङ्ग;
    }
  }
  पटिनिवत्त done;
}

/* fputc() look-alike */
स्थिर पूर्णाङ्क addbyter(unsigned वर्ण outc, शून्य *f)
{
  संरचना nsprintf *infop = f;
  सचे(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (वर्ण)outc; /* store */
    infop->length++; /* we are now one byte larger */
    पटिनिवत्त 0;     /* fputc() returns like this on success */
  }
  पटिनिवत्त 1;
}

/* block variant of addbyter */
स्थिर size_t addrun(ध्रुव unsigned वर्ण *buf, size_t len, शून्य *f)
{
  संरचना nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  सचे(len > fit)
    len = fit;
  सचे(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  पटिनिवत्त len;
}

पूर्णाङ्क curl_mvsnprintf(वर्ण *buffer, size_t maxlength, ध्रुव वर्ण *format,
                    va_list args)
{
  पूर्णाङ्क retcode;
  संरचना nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, रिक्त, format, args);
  सचे(info.max) {
    /* we null-terminate this */
    सचे(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    अञ्ञथा
      info.buffer[0] = 0;
  }
  पटिनिवत्त retcode;
}

पूर्णाङ्क curl_msnprintf(वर्ण *buffer, size_t maxlength, ध्रुव वर्ण *format, ...)
{
  पूर्णाङ्क retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  पटिनिवत्त retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
स्थिर पूर्णाङ्क alloc_flush(शून्य *f)
{
  संरचना asprintf *infop = f;
  सचे(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    सचे(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      पटिनिवत्त 1; /* fail */
    }
  }
  पटिनिवत्त 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
स्थिर पूर्णाङ्क alloc_addn(संरचना asprintf *infop, ध्रुव unsigned वर्ण *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  सचे(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    पटिनिवत्त 1; /* fail */
  }
  पटिनिवत्त 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
स्थिर bool alloc_fits(संरचना asprintf *infop, size_t more)
{
  पटिनिवत्त (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
स्थिर पूर्णाङ्क alloc_addbyter(unsigned वर्ण outc, शून्य *f)
{
  संरचना asprintf *infop = f;
  सचे(infop->nstage == परिमाण(infop->stage) && alloc_flush(infop))
    पटिनिवत्त 1; /* fail */
  सचे(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    सचे(alloc_flush(infop))
      पटिनिवत्त 1;
    पटिनिवत्त alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  पटिनिवत्त 0;
}

/* block variant of alloc_addbyter */
स्थिर size_t alloc_addrun(ध्रुव unsigned वर्ण *buf, size_t len, शून्य *f)
{
  संरचना asprintf *infop = f;
  size_t accepted = 0;
  याव(accepted < len) {
    size_t room = परिमाण(infop->stage) - infop->nstage;
    size_t n;
    सचे(!room) {
      सचे(alloc_flush(infop))
        पटिनिवत्त accepted; /* accepted < len aborts formatting */
      room = परिमाण(infop->stage);
    }
    n = len - accepted;
    सचे(n > room)
      n = room;
    सचे(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      सचे(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      सचे(alloc_flush(infop))
        पटिनिवत्त accepted;
      सचे(alloc_addn(infop, buf + accepted, 1))
        पटिनिवत्त accepted; /* accepted < len aborts formatting */
      accepted++;
      अनुवर्त;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  पटिनिवत्त accepted;
}

/* appends the formatted string, returns MERR error code */
पूर्णाङ्क curlx_dyn_vprintf(संरचना dynbuf *dyn, ध्रुव वर्ण *format, va_list args)
{
  संरचना asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (शून्य)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  सचे(!info.merr)
    (शून्य)alloc_flush(&info);
  सचे(info.merr) {
    curlx_dyn_free(info.b);
    पटिनिवत्त info.merr;
  }
  पटिनिवत्त 0;
}

वर्ण *curl_mvaprintf(ध्रुव वर्ण *format, va_list args)
{
  संरचना asprintf info;
  संरचना dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (शून्य)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  सचे(!info.merr)
    (शून्य)alloc_flush(&info);
  सचे(info.merr) {
    curlx_dyn_free(info.b);
    पटिनिवत्त रिक्त;
  }
  सचे(curlx_dyn_len(info.b))
    पटिनिवत्त curlx_dyn_ptr(info.b);
  पटिनिवत्त curlx_strdup("");
}

वर्ण *curl_maprintf(ध्रुव वर्ण *format, ...)
{
  va_list args;
  वर्ण *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  पटिनिवत्त s;
}

स्थिर पूर्णाङ्क storebuffer(unsigned वर्ण outc, शून्य *f)
{
  वर्ण **buffer = f;
  **buffer = (वर्ण)outc;
  (*buffer)++;
  पटिनिवत्त 0;
}

/* block variant of storebuffer */
स्थिर size_t storerun(ध्रुव unsigned वर्ण *buf, size_t len, शून्य *f)
{
  वर्ण **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  पटिनिवत्त len;
}

पूर्णाङ्क curl_msprintf(वर्ण *buffer, ध्रुव वर्ण *format, ...)
{
  va_list args; /* argument pointer */
  पूर्णाङ्क retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, रिक्त, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  पटिनिवत्त retcode;
}

स्थिर पूर्णाङ्क fputc_wrapper(unsigned वर्ण outc, शून्य *f)
{
  संरचना ioprintf *i = f;
  पूर्णाङ्क out = outc;
  FILE *s = i->fd;
  पूर्णाङ्क rc = fputc(out, s);
  i->error = (rc == EOF);
  पटिनिवत्त (rc == EOF);
}

/* block variant of fputc_wrapper */
स्थिर size_t fwrite_wrapper(ध्रुव unsigned वर्ण *buf, size_t len, शून्य *f)
{
  संरचना ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  पटिनिवत्त n;
}

पूर्णाङ्क curl_mvfprintf(FILE *fd, ध्रुव वर्ण *format, va_list args)
{
  संरचना ioprintf info;
  पूर्णाङ्क n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, रिक्त, format, args);
  सचे(info.error)
    पटिनिवत्त -1;
  पटिनिवत्त n;
}

पूर्णाङ्क curl_mfprintf(FILE *fd, ध्रुव वर्ण *format, ...)
{
  va_list args; /* argument pointer */
  पूर्णाङ्क n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  पटिनिवत्त n;
}

पूर्णाङ्क curl_mprintf(ध्रुव वर्ण *format, ...)
{
  va_list args; /* argument pointer */
  पूर्णाङ्क n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  पटिनिवत्त n;
}

पूर्णाङ्क curl_mvsprintf(वर्ण *buffer, ध्रुव वर्ण *format, va_list args)
{
  पूर्णाङ्क retcode = formatf(&buffer, storebuffer, storerun, रिक्त, format, args);
  *buffer = 0; /* we null-terminate this */
  पटिनिवत्त retcode;
}

पूर्णाङ्क curl_mvprintf(ध्रुव वर्ण *format, va_list args)
{
  पटिनिवत्त curl_mvfprintf(stdout, format, args);
}

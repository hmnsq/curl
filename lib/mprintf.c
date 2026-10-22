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
स्थिरांक अचिन्हित अक्षर Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
स्थिरांक अचिन्हित अक्षर Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  करा {                                    \
    जर(stream((अचिन्हित अक्षर)(x), userp)) \
      परत TRUE;                        \
    (*donep)++;                           \
  } पर्यंत(0)

/* Data type to read from the arglist */
प्रकारव्याख्या गणना {
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
  FLAGS_डावा       = 1 << 2,
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
संरचना va_आदान {
  FormatType type; /* FormatType */
  संघ {
    स्थिरांक अक्षर *str;
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
  पूर्णांक width;     /* width OR width parameter number */
  पूर्णांक precision; /* precision OR precision parameter number */
  अचिन्हित पूर्णांक flags;
  अचिन्हित पूर्णांक आदान; /* input argument array index */
  स्थिरांक अक्षर *start; /* format string start to output */
  माप_प्रकार outlen;     /* number of bytes from the format string to output */
};

संरचना nsprintf {
  अक्षर *buffer;
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
  अक्षर merr;
  माप_प्रकार nstage;
  अचिन्हित अक्षर stage[ASPRINTF_STAGE_SIZE];
};

संरचना ioprintf {
  खाता *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
स्थिर पूर्णांक dollarstring(स्थिरांक अक्षर *p, स्थिरांक अक्षर **end)
{
  curl_off_t अंक;
  जर(curlx_str_संख्या(&p, &अंक, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !अंक)
    परत -1;
  *end = p;
  परत (पूर्णांक)अंक - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (अचिन्हित अक्षर)(1 << ((y) & 7)))

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

स्थिर पूर्णांक parse_flags(स्थिरांक अक्षर **fmtp, अचिन्हित पूर्णांक *flagsp, पूर्णांक use_dollar,
                       पूर्णांक *precp, पूर्णांक *widthp)
{
  स्थिरांक अक्षर *fmt = *fmtp;
  bool loopit = TRUE;
  अचिन्हित पूर्णांक flags = 0;
  पूर्णांक width = 0;
  पूर्णांक precision = 0;

  /* Handle the flags */
  करा {
    निवड(*fmt++) {
    स्थिती ' ':
      flags |= FLAGS_SPACE;
      थांबा;
    स्थिती '+':
      flags |= FLAGS_SHOWSIGN;
      थांबा;
    स्थिती '-':
      flags |= FLAGS_डावा;
      flags &= ~(अचिन्हित पूर्णांक)FLAGS_PAD_NIL;
      थांबा;
    स्थिती '#':
      flags |= FLAGS_ALT;
      थांबा;
    स्थिती '.':
      जर('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        जर(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          जर(precision < 0)
            /* illegal combo */
            परत PFMT_DOLLARPREC;
        }
        अन्यथा
          /* get it from the next argument */
          precision = -1;
      }
      अन्यथा {
        bool is_neg;
        curl_off_t अंक;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        जर(is_neg)
          fmt++;
        जर(curlx_str_संख्या(&fmt, &अंक, पूर्णांक_उच्च))
          परत PFMT_PREC;
        precision = (पूर्णांक)अंक;
        जर(is_neg)
          precision = -precision;
      }
      जर((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        परत PFMT_PRECMIX;
      थांबा;
    स्थिती 'h':
      flags |= FLAGS_SHORT;
      थांबा;
#ifdef _WIN32
    स्थिती 'I':
      /* Non-ANSI integer extensions I32 I64 */
      जर((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      अन्यथा जर((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      अन्यथा {
#जर SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#अन्यथा
        flags |= FLAGS_LONG;
#endif
      }
      थांबा;
#endif /* _WIN32 */
    स्थिती 'l':
      जर(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      अन्यथा
        flags |= FLAGS_LONG;
      थांबा;
    स्थिती 'L':
      flags |= FLAGS_LONGDOUBLE;
      थांबा;
    स्थिती 'q':
      flags |= FLAGS_LONGLONG;
      थांबा;
    स्थिती 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#जर SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अन्यथा
      flags |= FLAGS_LONG;
#endif
      थांबा;
    स्थिती 'O':
#जर SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अन्यथा
      flags |= FLAGS_LONG;
#endif
      थांबा;
    स्थिती '0':
      जर(!(flags & FLAGS_डावा))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    स्थिती '1':
    स्थिती '2':
    स्थिती '3':
    स्थिती '4':
    स्थिती '5':
    स्थिती '6':
    स्थिती '7':
    स्थिती '8':
    स्थिती '9': {
      curl_off_t अंक;
      flags |= FLAGS_WIDTH;
      fmt--;
      जर(curlx_str_संख्या(&fmt, &अंक, पूर्णांक_उच्च))
        परत PFMT_WIDTH;
      width = (पूर्णांक)अंक;
      थांबा;
    }
    स्थिती '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      जर(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        जर(width < 0)
          /* illegal combo */
          परत PFMT_DOLLARWIDTH;
      }
      अन्यथा
        /* pick from the next argument */
        width = -1;
      थांबा;
    पूर्वनिर्धारित:
      loopit = FALSE;
      fmt--;
      थांबा;
    } /* switch */
  } पर्यंत(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  परत PFMT_OK;
}

स्थिर bool parse_conversion(स्थिरांक अक्षर f, अचिन्हित पूर्णांक *flagp,
                             FormatType *typep)
{
  अचिन्हित पूर्णांक flags = *flagp;
  FormatType type;
  निवड(f) {
  स्थिती 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    थांबा;
  स्थिती 's':
    type = MTYPE_STRING;
    थांबा;
  स्थिती 'n':
    type = MTYPE_INTPTR;
    थांबा;
  स्थिती 'p':
    type = MTYPE_PTR;
    थांबा;
  स्थिती 'd':
  स्थिती 'i':
    जर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    अन्यथा जर(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    अन्यथा
      type = MTYPE_INT;
    थांबा;
  स्थिती 'u':
    जर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा जर(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    थांबा;
  स्थिती 'o':
    जर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा जर(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    थांबा;
  स्थिती 'x':
    जर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा जर(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    थांबा;
  स्थिती 'X':
    जर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा जर(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    थांबा;
  स्थिती 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    थांबा;
  स्थिती 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    थांबा;
  स्थिती 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    थांबा;
  स्थिती 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    थांबा;
  स्थिती 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    थांबा;
  स्थिती 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    थांबा;
  स्थिती 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    थांबा;
  पूर्वनिर्धारित:
    /* invalid instruction, disregard and continue */
    परत TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  परत FALSE;
}

स्थिर पूर्णांक parsefmt(स्थिरांक अक्षर *format,
                    संरचना outsegment *out,
                    संरचना va_आदान *in,
                    पूर्णांक *opieces,
                    पूर्णांक *ipieces, बहु_सूची arglist)
{
  स्थिरांक अक्षर *fmt = format;
  पूर्णांक param_अंक = 0;
  पूर्णांक max_param = -1;
  पूर्णांक i;
  पूर्णांक ocount = 0;
  अचिन्हित अक्षर usedinput[MAX_PARAMETERS / 8];
  माप_प्रकार outlen = 0;
  संरचना outsegment *optr;
  पूर्णांक use_dollar = DOLLAR_UNKNOWN;
  स्थिरांक अक्षर *start = fmt;

  /* clear, set a bit for each used input */
  स_रखो(usedinput, 0, आकार(usedinput));

  पर्यंत(*fmt) {
    जर(*fmt == '%') {
      संरचना va_आदान *iptr;
      FormatType type;
      अचिन्हित पूर्णांक flags = 0;
      पूर्णांक width = 0;
      पूर्णांक precision = 0;
      पूर्णांक param = -1;
      पूर्णांक rc;
      fmt++;
      outlen = (माप_प्रकार)(fmt - start - 1);
      जर(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        जर(outlen) {
          optr = &out[ocount++];
          जर(ocount > MAX_SEGMENTS)
            परत PFMT_MANYSEGS;
          optr->आदान = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        सुरूठेवा; /* while */
      }

      जर(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        जर(param < 0) {
          जर(use_dollar == DOLLAR_USE)
            /* illegal combo */
            परत PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        अन्यथा
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      जर(rc)
        परत rc;

      जर(parse_conversion(*fmt, &flags, &type))
        सुरूठेवा;

      जर(flags & FLAGS_WIDTHPARAM) {
        जर(width < 0)
          width = param_अंक++;
        अन्यथा {
          /* if this identifies a parameter already used, this is illegal */
          जर(is_arg_used(usedinput, width))
            परत PFMT_WIDTHARG;
        }
        जर(width >= MAX_PARAMETERS)
          परत PFMT_MANYARGS;
        जर(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      जर(flags & FLAGS_PRECPARAM) {
        जर(precision < 0)
          precision = param_अंक++;
        अन्यथा {
          /* if this identifies a parameter already used, this is illegal */
          जर(is_arg_used(usedinput, precision))
            परत PFMT_PRECARG;
        }
        जर(precision >= MAX_PARAMETERS)
          परत PFMT_MANYARGS;
        जर(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      जर(param < 0)
        param = param_अंक++;
      जर(param >= MAX_PARAMETERS)
        परत PFMT_MANYARGS;
      जर(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      जर(ocount > MAX_SEGMENTS)
        परत PFMT_MANYSEGS;
      optr->आदान = (अचिन्हित पूर्णांक)param;
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
  जर(outlen) {
    optr = &out[ocount++];
    जर(ocount > MAX_SEGMENTS)
      परत PFMT_MANYSEGS;
    optr->आदान = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  साठी(i = 0; i < max_param + 1; i++) {
    संरचना va_आदान *iptr = &in[i];
    जर(!is_arg_used(usedinput, i))
      /* bad input */
      परत PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    निवड(iptr->type) {
    स्थिती MTYPE_STRING:
      iptr->val.str = बहु_तर्क(arglist, स्थिरांक अक्षर *);
      थांबा;

    स्थिती MTYPE_INTPTR:
    स्थिती MTYPE_PTR:
      iptr->val.ptr = बहु_तर्क(arglist, शून्य *);
      थांबा;

    स्थिती MTYPE_LONGLONGU:
      iptr->val.numu = बहु_तर्क(arglist, uint64_t);
      थांबा;

    स्थिती MTYPE_LONGLONG:
      iptr->val.nums = बहु_तर्क(arglist, int64_t);
      थांबा;

    स्थिती MTYPE_LONGU:
      iptr->val.numu = बहु_तर्क(arglist, अचिन्हित दीर्घ);
      थांबा;

    स्थिती MTYPE_LONG:
      iptr->val.nums = बहु_तर्क(arglist, दीर्घ);
      थांबा;

    स्थिती MTYPE_INTU:
      iptr->val.numu = बहु_तर्क(arglist, अचिन्हित पूर्णांक);
      थांबा;

    स्थिती MTYPE_INT:
    स्थिती MTYPE_WIDTH:
    स्थिती MTYPE_PRECISION:
      iptr->val.nums = बहु_तर्क(arglist, पूर्णांक);
      थांबा;

    स्थिती MTYPE_DOUBLE:
      iptr->val.dnum = बहु_तर्क(arglist, द्विगुण);
      थांबा;

    स्थिती MTYPE_LONGDOUBLE:
      iptr->val.dnum = (द्विगुण)बहु_तर्क(arglist, दीर्घ द्विगुण);
      थांबा;

    पूर्वनिर्धारित:
      DEBUGASSERT(रिक्त); /* unexpected */
      थांबा;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  परत PFMT_OK;
}

संरचना mproperty {
  पूर्णांक width;            /* Width of a field. */
  पूर्णांक prec;             /* Precision of a field. */
  अचिन्हित पूर्णांक flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
प्रकारव्याख्या माप_प्रकार (*mp_streamn)(स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार len, शून्य *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
प्रकारव्याख्या पूर्णांक (*mp_flush)(शून्य *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
स्थिर bool stream_run(शून्य *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, शून्य *),
                       mp_streamn streamn,
                       स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार len, पूर्णांक *donep)
{
  जर(!len)
    परत FALSE;
  जर(streamn) {
    माप_प्रकार n = streamn(buf, len, userp);
    *donep += (पूर्णांक)n;
    परत n != len;
  }
  पर्यंत(len--) {
    जर(stream(*buf++, userp))
      परत TRUE;
    (*donep)++;
  }
  परत FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
स्थिर स्थिरांक अक्षर pad_spaces[] = "                ";
स्थिर स्थिरांक अक्षर pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
स्थिर bool stream_pad(शून्य *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, शून्य *),
                       mp_streamn streamn,
                       अचिन्हित अक्षर pad, पूर्णांक अंक, पूर्णांक *donep)
{
  स्थिरांक अचिन्हित अक्षर *src;
  जर(अंक <= 0)
    परत FALSE;
  जर(pad == '0')
    src = (स्थिरांक अचिन्हित अक्षर *)pad_zeros;
  अन्यथा
    src = (स्थिरांक अचिन्हित अक्षर *)pad_spaces;
  पर्यंत(अंक > PAD_RUN) {
    जर(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      परत TRUE;
    अंक -= PAD_RUN;
  }
  परत stream_run(userp, stream, streamn, src, (माप_प्रकार)अंक, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
स्थिर bool stream_zrun(शून्य *userp,
                        पूर्णांक (*stream)(अचिन्हित अक्षर, शून्य *),
                        mp_streamn streamn,
                        स्थिरांक अक्षर *str, माप_प्रकार len, पूर्णांक *donep)
{
  माप_प्रकार n = 0;
  पर्यंत(n < len && str[n])
    n++;
  परत stream_run(userp, stream, streamn,
                    (स्थिरांक अचिन्हित अक्षर *)str, n, donep);
}

स्थिर bool out_double(शून्य *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       द्विगुण dnum,
                       अक्षर *work, पूर्णांक *donep)
{
  अक्षर fmt[32] = "%";
  अक्षर *fptr = &fmt[1];
  माप_प्रकार डावा = आकार(fmt) - म_माप(fmt);
  पूर्णांक flags = p->flags;
  पूर्णांक width = p->width;
  पूर्णांक prec = p->prec;

  जर(flags & FLAGS_डावा)
    *fptr++ = '-';
  जर(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  जर(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  जर(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  जर(width >= 0) {
    माप_प्रकार dlen;
    जर(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (माप_प्रकार)curl_msnprintf(fptr, डावा, "%d", width);
    fptr += dlen;
    डावा -= dlen;
  }
  जर(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    पूर्णांक maxprec = BUFFSIZE - 1;
    द्विगुण val = dnum;
    पूर्णांक len;
    जर(prec > maxprec)
      prec = maxprec - 1;
    जर(width > 0 && prec <= width)
      maxprec -= width;
    पर्यंत(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    जर(prec > maxprec)
      prec = maxprec - 1;
    जर(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, डावा, ".%d", prec);
    fptr += len;
  }
  जर(flags & FLAGS_LONG)
    *fptr++ = 'l';

  जर(flags & FLAGS_FLOATE)
    *fptr++ = (अक्षर)((flags & FLAGS_UPPER) ? 'E' : 'e');
  अन्यथा जर(flags & FLAGS_FLOATG)
    *fptr++ = (अक्षर)((flags & FLAGS_UPPER) ? 'G' : 'g');
  अन्यथा
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic काढा
#endif
  DEBUGASSERT(म_माप(work) < BUFFSIZE);
  परत stream_run(userp, stream, streamn,
                    (स्थिरांक अचिन्हित अक्षर *)work, म_माप(work), donep);
}

स्थिर bool out_संख्या(शून्य *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       uint64_t अंक,
                       int64_t nums,
                       अक्षर *work, पूर्णांक *donep)
{
  स्थिरांक अचिन्हित अक्षर *digits = Curl_ldigits;
  पूर्णांक flags = p->flags;
  पूर्णांक width = p->width;
  पूर्णांक prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  पूर्णांक base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  अक्षर *workend = &work[BUFFSIZE - 2];
  अक्षर *w;

  जर(flags & FLAGS_CHAR) {
    /* Character. */
    जर(!(flags & FLAGS_डावा)) {
      जर(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        परत TRUE;
      width = 1;
    }
    OUTCHAR((अक्षर)अंक);
    जर(flags & FLAGS_डावा) {
      जर(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        परत TRUE;
    }
    परत FALSE;
  }
  जर(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  अन्यथा जर(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  अन्यथा जर(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  अन्यथा {
    /* Decimal integer. */
    is_neg = (nums < 0);
    जर(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_अंक; /* Used to convert negative in positive. */
      signed_अंक = nums + (int64_t)1;
      signed_अंक = -signed_अंक;
      अंक = (uint64_t)signed_अंक;
      अंक += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  जर(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  निवड(base) {
  स्थिती 10:
    पर्यंत(अंक > 0) {
      *w-- = (अक्षर)('0' + (अंक % 10));
      अंक /= 10;
    }
    थांबा;
  पूर्वनिर्धारित:
    पर्यंत(अंक > 0) {
      *w-- = digits[अंक % base];
      अंक /= base;
    }
    थांबा;
  }
  width -= (पूर्णांक)(workend - w);
  prec -= (पूर्णांक)(workend - w);

  जर(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  जर(prec > 0) {
    width -= prec;
    पर्यंत(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  जर(is_alt && base == 16)
    width -= 2;

  जर(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  जर(!(flags & FLAGS_डावा) && !(flags & FLAGS_PAD_NIL)) {
    जर(stream_pad(userp, stream, streamn, ' ', width, donep))
      परत TRUE;
    width = 0;
  }

  जर(is_neg)
    OUTCHAR('-');
  अन्यथा जर(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  अन्यथा जर(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  जर(is_alt && base == 16) {
    OUTCHAR('0');
    जर(flags & FLAGS_UPPER)
      OUTCHAR('X');
    अन्यथा
      OUTCHAR('x');
  }

  जर(!(flags & FLAGS_डावा) && (flags & FLAGS_PAD_NIL)) {
    जर(stream_pad(userp, stream, streamn, '0', width, donep))
      परत TRUE;
    width = 0;
  }

  /* Write the number. */
  जर(stream_run(userp, stream, streamn, (स्थिरांक अचिन्हित अक्षर *)w + 1,
                (माप_प्रकार)(workend - w), donep))
    परत TRUE;

  जर(flags & FLAGS_डावा) {
    जर(stream_pad(userp, stream, streamn, ' ', width, donep))
      परत TRUE;
  }

  परत FALSE;
}

स्थिर स्थिरांक अक्षर nilstr[] = "(nil)";

स्थिर bool out_string(शून्य *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, शून्य *),
                       mp_streamn streamn,
                       संरचना mproperty *p,
                       स्थिरांक अक्षर *str,
                       पूर्णांक *donep)
{
  पूर्णांक flags = p->flags;
  पूर्णांक width = p->width;
  पूर्णांक prec = p->prec;
  माप_प्रकार len;

  जर(!str) {
    /* Write null string if there is space. */
    जर(prec == -1 || prec >= (पूर्णांक)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(अचिन्हित पूर्णांक)FLAGS_ALT;
    }
    अन्यथा {
      str = "";
      len = 0;
    }
  }
  अन्यथा जर(prec != -1)
    len = (माप_प्रकार)prec;
  अन्यथा जर(*str == '\0')
    len = 0;
  अन्यथा
    len = म_माप(str);

  width -= (len > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)len;

  जर(flags & FLAGS_ALT)
    OUTCHAR('"');

  जर(!(flags & FLAGS_डावा)) {
    जर(stream_pad(userp, stream, streamn, ' ', width, donep))
      परत TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  जर(prec == -1) {
    जर(stream_run(userp, stream, streamn,
                  (स्थिरांक अचिन्हित अक्षर *)str, len, donep))
      परत TRUE;
  }
  अन्यथा जर(stream_zrun(userp, stream, streamn, str, len, donep))
    परत TRUE;
  जर(flags & FLAGS_डावा) {
    जर(stream_pad(userp, stream, streamn, ' ', width, donep))
      परत TRUE;
  }

  जर(flags & FLAGS_ALT)
    OUTCHAR('"');

  परत FALSE;
}

स्थिर bool out_pointer(शून्य *userp,
                        पूर्णांक (*stream)(अचिन्हित अक्षर, शून्य *),
                        mp_streamn streamn,
                        संरचना mproperty *p,
                        स्थिरांक अक्षर *ptr,
                        अक्षर *work,
                        पूर्णांक *donep)
{
  /* Generic pointer. */
  जर(ptr) {
    माप_प्रकार अंक = (माप_प्रकार)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    जर(out_संख्या(userp, stream, streamn, p, अंक, 0, work, donep))
      परत TRUE;
  }
  अन्यथा {
    /* Write "(nil)" for a nil pointer. */
    पूर्णांक width = p->width;
    पूर्णांक flags = p->flags;

    width -= (पूर्णांक)CURL_CSTRLEN(nilstr);
    जर(flags & FLAGS_डावा) {
      जर(stream_pad(userp, stream, streamn, ' ', width, donep))
        परत TRUE;
      width = 0;
    }
    जर(stream_run(userp, stream, streamn, (स्थिरांक अचिन्हित अक्षर *)nilstr,
                  आकार(nilstr) - 1, donep))
      परत TRUE;
    जर(!(flags & FLAGS_डावा)) {
      जर(stream_pad(userp, stream, streamn, ' ', width, donep))
        परत TRUE;
    }
  }
  परत FALSE;
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
स्थिर पूर्णांक formatf(शून्य *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   पूर्णांक (*stream)(अचिन्हित अक्षर, शून्य *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   स्थिरांक अक्षर *format, /* %-formatted string */
                   बहु_सूची ap_save) /* list of parameters */
{
  पूर्णांक done = 0;   /* number of characters written */
  पूर्णांक i;
  पूर्णांक ocount = 0; /* number of output segments */
  पूर्णांक icount = 0; /* number of input arguments */

  संरचना outsegment output[MAX_SEGMENTS];
  संरचना va_आदान आदान[MAX_PARAMETERS];
  अक्षर work[BUFFSIZE + 2];

  /* Parse the format string */
  जर(parsefmt(format, output, आदान, &ocount, &icount, ap_save))
    परत 0;

  साठी(i = 0; i < ocount; i++) {
    संरचना outsegment *optr = &output[i];
    संरचना va_आदान *iptr = &आदान[optr->आदान];
    संरचना mproperty p;
    माप_प्रकार outlen = optr->outlen;

    जर(outlen) {
      जर(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        परत done;
      जर(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        सुरूठेवा;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    जर(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (पूर्णांक)आदान[optr->width].val.nums;
      जर(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        जर(p.width == पूर्णांक_न्यून)
          p.width = पूर्णांक_उच्च;
        अन्यथा
          p.width = -p.width;
        p.flags |= FLAGS_डावा;
        p.flags &= ~(अचिन्हित पूर्णांक)FLAGS_PAD_NIL;
      }
    }
    अन्यथा
      p.width = optr->width;

    /* pick up the specified precision */
    जर(p.flags & FLAGS_PRECPARAM) {
      p.prec = (पूर्णांक)आदान[optr->precision].val.nums;
      जर(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    अन्यथा जर(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    अन्यथा
      p.prec = -1;

    निवड(iptr->type) {
    स्थिती MTYPE_INTU:
    स्थिती MTYPE_LONGU:
    स्थिती MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      जर(out_संख्या(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        परत done;
      थांबा;

    स्थिती MTYPE_INT:
    स्थिती MTYPE_LONG:
    स्थिती MTYPE_LONGLONG:
      जर(out_संख्या(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        परत done;
      थांबा;

    स्थिती MTYPE_STRING:
      जर(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        परत done;
      थांबा;

    स्थिती MTYPE_PTR:
      जर(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        परत done;
      थांबा;

    स्थिती MTYPE_DOUBLE:
    स्थिती MTYPE_LONGDOUBLE:
      जर(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        परत done;
      थांबा;

    स्थिती MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      जर(flush && flush(userp))
        परत done;
      /* Answer the count of characters written. */
      जर(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      अन्यथा जर(p.flags & FLAGS_LONG)
        *(दीर्घ *)iptr->val.ptr = (दीर्घ)done;
      अन्यथा जर(!(p.flags & FLAGS_SHORT))
        *(पूर्णांक *)iptr->val.ptr = done;
      अन्यथा
        *(लघु *)iptr->val.ptr = (लघु)done;
      थांबा;

    पूर्वनिर्धारित:
      थांबा;
    }
  }
  परत done;
}

/* fputc() look-alike */
स्थिर पूर्णांक addbyter(अचिन्हित अक्षर outc, शून्य *f)
{
  संरचना nsprintf *infop = f;
  जर(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (अक्षर)outc; /* store */
    infop->length++; /* we are now one byte larger */
    परत 0;     /* fputc() returns like this on success */
  }
  परत 1;
}

/* block variant of addbyter */
स्थिर माप_प्रकार addrun(स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार len, शून्य *f)
{
  संरचना nsprintf *infop = f;
  माप_प्रकार fit = infop->max - infop->length;
  जर(len > fit)
    len = fit;
  जर(len) {
    स_नकल(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  परत len;
}

पूर्णांक curl_mvsnprintf(अक्षर *buffer, माप_प्रकार maxlength, स्थिरांक अक्षर *format,
                    बहु_सूची args)
{
  पूर्णांक retcode;
  संरचना nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, रिक्त, format, args);
  जर(info.max) {
    /* we null-terminate this */
    जर(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    अन्यथा
      info.buffer[0] = 0;
  }
  परत retcode;
}

पूर्णांक curl_msnprintf(अक्षर *buffer, माप_प्रकार maxlength, स्थिरांक अक्षर *format, ...)
{
  पूर्णांक retcode;
  बहु_सूची args; /* argument pointer */
  बहु_शुरू(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  बहु_पूर्ण(args);
  परत retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
स्थिर पूर्णांक alloc_flush(शून्य *f)
{
  संरचना asprintf *infop = f;
  जर(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    जर(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      परत 1; /* fail */
    }
  }
  परत 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
स्थिर पूर्णांक alloc_addn(संरचना asprintf *infop, स्थिरांक अचिन्हित अक्षर *buf,
                      माप_प्रकार len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  जर(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    परत 1; /* fail */
  }
  परत 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
स्थिर bool alloc_fits(संरचना asprintf *infop, माप_प्रकार more)
{
  परत (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
स्थिर पूर्णांक alloc_addbyter(अचिन्हित अक्षर outc, शून्य *f)
{
  संरचना asprintf *infop = f;
  जर(infop->nstage == आकार(infop->stage) && alloc_flush(infop))
    परत 1; /* fail */
  जर(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    जर(alloc_flush(infop))
      परत 1;
    परत alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  परत 0;
}

/* block variant of alloc_addbyter */
स्थिर माप_प्रकार alloc_addrun(स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार len, शून्य *f)
{
  संरचना asprintf *infop = f;
  माप_प्रकार accepted = 0;
  पर्यंत(accepted < len) {
    माप_प्रकार room = आकार(infop->stage) - infop->nstage;
    माप_प्रकार n;
    जर(!room) {
      जर(alloc_flush(infop))
        परत accepted; /* accepted < len aborts formatting */
      room = आकार(infop->stage);
    }
    n = len - accepted;
    जर(n > room)
      n = room;
    जर(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      माप_प्रकार fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      जर(fits) {
        स_नकल(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      जर(alloc_flush(infop))
        परत accepted;
      जर(alloc_addn(infop, buf + accepted, 1))
        परत accepted; /* accepted < len aborts formatting */
      accepted++;
      सुरूठेवा;
    }
    स_नकल(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  परत accepted;
}

/* appends the formatted string, returns MERR error code */
पूर्णांक curlx_dyn_vprintf(संरचना dynbuf *dyn, स्थिरांक अक्षर *format, बहु_सूची args)
{
  संरचना asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (शून्य)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  जर(!info.merr)
    (शून्य)alloc_flush(&info);
  जर(info.merr) {
    curlx_dyn_free(info.b);
    परत info.merr;
  }
  परत 0;
}

अक्षर *curl_mvaprintf(स्थिरांक अक्षर *format, बहु_सूची args)
{
  संरचना asprintf info;
  संरचना dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (शून्य)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  जर(!info.merr)
    (शून्य)alloc_flush(&info);
  जर(info.merr) {
    curlx_dyn_free(info.b);
    परत रिक्त;
  }
  जर(curlx_dyn_len(info.b))
    परत curlx_dyn_ptr(info.b);
  परत curlx_strdup("");
}

अक्षर *curl_maprintf(स्थिरांक अक्षर *format, ...)
{
  बहु_सूची args;
  अक्षर *s;
  बहु_शुरू(args, format);
  s = curl_mvaprintf(format, args);
  बहु_पूर्ण(args);
  परत s;
}

स्थिर पूर्णांक storebuffer(अचिन्हित अक्षर outc, शून्य *f)
{
  अक्षर **buffer = f;
  **buffer = (अक्षर)outc;
  (*buffer)++;
  परत 0;
}

/* block variant of storebuffer */
स्थिर माप_प्रकार storerun(स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार len, शून्य *f)
{
  अक्षर **buffer = f;
  स_नकल(*buffer, buf, len);
  *buffer += len;
  परत len;
}

पूर्णांक curl_msprintf(अक्षर *buffer, स्थिरांक अक्षर *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णांक retcode;
  बहु_शुरू(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, रिक्त, format, args);
  बहु_पूर्ण(args);
  *buffer = 0; /* we null-terminate this */
  परत retcode;
}

स्थिर पूर्णांक fputc_wrapper(अचिन्हित अक्षर outc, शून्य *f)
{
  संरचना ioprintf *i = f;
  पूर्णांक out = outc;
  खाता *s = i->fd;
  पूर्णांक rc = ख_अक्षर_दो(out, s);
  i->error = (rc == खातापूर्ण);
  परत (rc == खातापूर्ण);
}

/* block variant of fputc_wrapper */
स्थिर माप_प्रकार fwrite_wrapper(स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार len, शून्य *f)
{
  संरचना ioprintf *i = f;
  माप_प्रकार n;
  खाता *s = i->fd;
  n = ख_डालो(buf, 1, len, s);
  i->error = (n != len);
  परत n;
}

पूर्णांक curl_mvfprintf(खाता *fd, स्थिरांक अक्षर *format, बहु_सूची args)
{
  संरचना ioprintf info;
  पूर्णांक n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, रिक्त, format, args);
  जर(info.error)
    परत -1;
  परत n;
}

पूर्णांक curl_mfprintf(खाता *fd, स्थिरांक अक्षर *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णांक n;
  बहु_शुरू(args, format);
  n = curl_mvfprintf(fd, format, args);
  बहु_पूर्ण(args);
  परत n;
}

पूर्णांक curl_mprintf(स्थिरांक अक्षर *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णांक n;
  बहु_शुरू(args, format);
  n = curl_mvfprintf(मानक_निकास, format, args);
  बहु_पूर्ण(args);
  परत n;
}

पूर्णांक curl_mvsprintf(अक्षर *buffer, स्थिरांक अक्षर *format, बहु_सूची args)
{
  पूर्णांक retcode = formatf(&buffer, storebuffer, storerun, रिक्त, format, args);
  *buffer = 0; /* we null-terminate this */
  परत retcode;
}

पूर्णांक curl_mvprintf(स्थिरांक अक्षर *format, बहु_सूची args)
{
  परत curl_mvfprintf(मानक_निकास, format, args);
}

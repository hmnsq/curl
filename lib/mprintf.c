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
#समावेश "curl_setup.h"

#समावेश "curlx/dynbuf.h"
#समावेश "curl_printf.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#घोषणा BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#घोषणा MAX_PARAMETERS 128 /* number of input arguments */
#घोषणा MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
स्थिर अचिन्हित अक्षर Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
स्थिर अचिन्हित अक्षर Curl_udigits[] = "0123456789ABCDEF";

#घोषणा OUTCHAR(x)                        \
  करो {                                    \
    अगर(stream((अचिन्हित अक्षर)(x), userp)) \
      वापस TRUE;                        \
    (*donep)++;                           \
  } जबतक(0)

/* Data type to read from the arglist */
प्रकार क्रमागत {
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
क्रमागत {
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

क्रमागत {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
काष्ठा va_निवेश {
  FormatType type; /* FormatType */
  जोड़ {
    स्थिर अक्षर *str;
    व्योम *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    द्विगुन dnum;
  } val;
};

/*
 * Describes an output segment.
 */
काष्ठा outsegment {
  पूर्णांक width;     /* width OR width parameter number */
  पूर्णांक precision; /* precision OR precision parameter number */
  अचिन्हित पूर्णांक flags;
  अचिन्हित पूर्णांक निवेश; /* input argument array index */
  स्थिर अक्षर *start; /* format string start to output */
  माप_प्रकार outlen;     /* number of bytes from the format string to output */
};

काष्ठा nsprintf {
  अक्षर *बफर;
  माप_प्रकार लंबाई;
  माप_प्रकार max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#घोषणा ASPRINTF_STAGE_आकार 512

काष्ठा asprintf {
  काष्ठा dynbuf *b;
  अक्षर merr;
  माप_प्रकार nstage;
  अचिन्हित अक्षर stage[ASPRINTF_STAGE_आकार];
};

काष्ठा ioprintf {
  खाता *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
अटल पूर्णांक dollarstring(स्थिर अक्षर *p, स्थिर अक्षर **end)
{
  curl_off_t अंक;
  अगर(curlx_str_संख्या(&p, &अंक, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !अंक)
    वापस -1;
  *end = p;
  वापस (पूर्णांक)अंक - 1;
}

#घोषणा is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#घोषणा mark_arg_used(x, y) ((x)[(y) / 8] |= (अचिन्हित अक्षर)(1 << ((y) & 7)))

/*
 * Parse the format string.
 *
 * Create two arrays. One describes the inputs, one describes the outputs.
 *
 * Returns zero on success.
 */

#घोषणा PFMT_OK          0
#घोषणा PFMT_DOLLAR      1 /* bad dollar for main param */
#घोषणा PFMT_DOLLARWIDTH 2 /* bad dollar use for width */
#घोषणा PFMT_DOLLARPREC  3 /* bad dollar use for precision */
#घोषणा PFMT_MANYARGS    4 /* too many input arguments used */
#घोषणा PFMT_PREC        5 /* precision overflow */
#घोषणा PFMT_PRECMIX     6 /* bad mix of precision specifiers */
#घोषणा PFMT_WIDTH       7 /* width overflow */
#घोषणा PFMT_INPUTGAP    8 /* gap in arguments */
#घोषणा PFMT_WIDTHARG    9 /* attempted to use same arg twice, for width */
#घोषणा PFMT_PRECARG    10 /* attempted to use same arg twice, for prec */
#घोषणा PFMT_MANYSEGS   11 /* maxed out output segments */

अटल पूर्णांक parse_flags(स्थिर अक्षर **fmtp, अचिन्हित पूर्णांक *flagsp, पूर्णांक use_dollar,
                       पूर्णांक *precp, पूर्णांक *widthp)
{
  स्थिर अक्षर *fmt = *fmtp;
  bool loopit = TRUE;
  अचिन्हित पूर्णांक flags = 0;
  पूर्णांक width = 0;
  पूर्णांक precision = 0;

  /* Handle the flags */
  करो {
    चयन(*fmt++) {
    हाल ' ':
      flags |= FLAGS_SPACE;
      अवरोध;
    हाल '+':
      flags |= FLAGS_SHOWSIGN;
      अवरोध;
    हाल '-':
      flags |= FLAGS_बायाँ;
      flags &= ~(अचिन्हित पूर्णांक)FLAGS_PAD_NIL;
      अवरोध;
    हाल '#':
      flags |= FLAGS_ALT;
      अवरोध;
    हाल '.':
      अगर('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        अगर(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          अगर(precision < 0)
            /* illegal combo */
            वापस PFMT_DOLLARPREC;
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
        अगर(is_neg)
          fmt++;
        अगर(curlx_str_संख्या(&fmt, &अंक, पूर्णांक_उच्च))
          वापस PFMT_PREC;
        precision = (पूर्णांक)अंक;
        अगर(is_neg)
          precision = -precision;
      }
      अगर((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        वापस PFMT_PRECMIX;
      अवरोध;
    हाल 'h':
      flags |= FLAGS_SHORT;
      अवरोध;
#अगर_घोषित _WIN32
    हाल 'I':
      /* Non-ANSI integer extensions I32 I64 */
      अगर((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      अन्यथा अगर((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      अन्यथा {
#अगर SIZEOF_आकार_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#अन्यथा
        flags |= FLAGS_LONG;
#पूर्ण_अगर
      }
      अवरोध;
#पूर्ण_अगर /* _WIN32 */
    हाल 'l':
      अगर(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      अन्यथा
        flags |= FLAGS_LONG;
      अवरोध;
    हाल 'L':
      flags |= FLAGS_LONGDOUBLE;
      अवरोध;
    हाल 'q':
      flags |= FLAGS_LONGLONG;
      अवरोध;
    हाल 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#अगर SIZEOF_आकार_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अन्यथा
      flags |= FLAGS_LONG;
#पूर्ण_अगर
      अवरोध;
    हाल 'O':
#अगर SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#अन्यथा
      flags |= FLAGS_LONG;
#पूर्ण_अगर
      अवरोध;
    हाल '0':
      अगर(!(flags & FLAGS_बायाँ))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    हाल '1':
    हाल '2':
    हाल '3':
    हाल '4':
    हाल '5':
    हाल '6':
    हाल '7':
    हाल '8':
    हाल '9': {
      curl_off_t अंक;
      flags |= FLAGS_WIDTH;
      fmt--;
      अगर(curlx_str_संख्या(&fmt, &अंक, पूर्णांक_उच्च))
        वापस PFMT_WIDTH;
      width = (पूर्णांक)अंक;
      अवरोध;
    }
    हाल '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      अगर(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        अगर(width < 0)
          /* illegal combo */
          वापस PFMT_DOLLARWIDTH;
      }
      अन्यथा
        /* pick from the next argument */
        width = -1;
      अवरोध;
    शेष:
      loopit = FALSE;
      fmt--;
      अवरोध;
    } /* switch */
  } जबतक(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  वापस PFMT_OK;
}

अटल bool parse_conversion(स्थिर अक्षर f, अचिन्हित पूर्णांक *flagp,
                             FormatType *typep)
{
  अचिन्हित पूर्णांक flags = *flagp;
  FormatType type;
  चयन(f) {
  हाल 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    अवरोध;
  हाल 's':
    type = MTYPE_STRING;
    अवरोध;
  हाल 'n':
    type = MTYPE_INTPTR;
    अवरोध;
  हाल 'p':
    type = MTYPE_PTR;
    अवरोध;
  हाल 'd':
  हाल 'i':
    अगर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    अन्यथा अगर(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    अन्यथा
      type = MTYPE_INT;
    अवरोध;
  हाल 'u':
    अगर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा अगर(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    अवरोध;
  हाल 'o':
    अगर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा अगर(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    अवरोध;
  हाल 'x':
    अगर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा अगर(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    अवरोध;
  हाल 'X':
    अगर(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    अन्यथा अगर(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    अन्यथा
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    अवरोध;
  हाल 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    अवरोध;
  हाल 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    अवरोध;
  हाल 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    अवरोध;
  हाल 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    अवरोध;
  हाल 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    अवरोध;
  हाल 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    अवरोध;
  हाल 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    अवरोध;
  शेष:
    /* invalid instruction, disregard and continue */
    वापस TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  वापस FALSE;
}

अटल पूर्णांक parsefmt(स्थिर अक्षर *format,
                    काष्ठा outsegment *out,
                    काष्ठा va_निवेश *in,
                    पूर्णांक *opieces,
                    पूर्णांक *ipieces, बहु_सूची arglist)
{
  स्थिर अक्षर *fmt = format;
  पूर्णांक param_अंक = 0;
  पूर्णांक max_param = -1;
  पूर्णांक i;
  पूर्णांक ocount = 0;
  अचिन्हित अक्षर usedinput[MAX_PARAMETERS / 8];
  माप_प्रकार outlen = 0;
  काष्ठा outsegment *optr;
  पूर्णांक use_dollar = DOLLAR_UNKNOWN;
  स्थिर अक्षर *start = fmt;

  /* clear, set a bit for each used input */
  स_रखो(usedinput, 0, माप(usedinput));

  जबतक(*fmt) {
    अगर(*fmt == '%') {
      काष्ठा va_निवेश *iptr;
      FormatType type;
      अचिन्हित पूर्णांक flags = 0;
      पूर्णांक width = 0;
      पूर्णांक precision = 0;
      पूर्णांक param = -1;
      पूर्णांक rc;
      fmt++;
      outlen = (माप_प्रकार)(fmt - start - 1);
      अगर(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        अगर(outlen) {
          optr = &out[ocount++];
          अगर(ocount > MAX_SEGMENTS)
            वापस PFMT_MANYSEGS;
          optr->निवेश = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        जारी; /* while */
      }

      अगर(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        अगर(param < 0) {
          अगर(use_dollar == DOLLAR_USE)
            /* illegal combo */
            वापस PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        अन्यथा
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      अगर(rc)
        वापस rc;

      अगर(parse_conversion(*fmt, &flags, &type))
        जारी;

      अगर(flags & FLAGS_WIDTHPARAM) {
        अगर(width < 0)
          width = param_अंक++;
        अन्यथा {
          /* if this identifies a parameter already used, this is illegal */
          अगर(is_arg_used(usedinput, width))
            वापस PFMT_WIDTHARG;
        }
        अगर(width >= MAX_PARAMETERS)
          वापस PFMT_MANYARGS;
        अगर(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      अगर(flags & FLAGS_PRECPARAM) {
        अगर(precision < 0)
          precision = param_अंक++;
        अन्यथा {
          /* if this identifies a parameter already used, this is illegal */
          अगर(is_arg_used(usedinput, precision))
            वापस PFMT_PRECARG;
        }
        अगर(precision >= MAX_PARAMETERS)
          वापस PFMT_MANYARGS;
        अगर(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      अगर(param < 0)
        param = param_अंक++;
      अगर(param >= MAX_PARAMETERS)
        वापस PFMT_MANYARGS;
      अगर(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      अगर(ocount > MAX_SEGMENTS)
        वापस PFMT_MANYSEGS;
      optr->निवेश = (अचिन्हित पूर्णांक)param;
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
  अगर(outlen) {
    optr = &out[ocount++];
    अगर(ocount > MAX_SEGMENTS)
      वापस PFMT_MANYSEGS;
    optr->निवेश = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  क्रम(i = 0; i < max_param + 1; i++) {
    काष्ठा va_निवेश *iptr = &in[i];
    अगर(!is_arg_used(usedinput, i))
      /* bad input */
      वापस PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    चयन(iptr->type) {
    हाल MTYPE_STRING:
      iptr->val.str = बहु_तर्क(arglist, स्थिर अक्षर *);
      अवरोध;

    हाल MTYPE_INTPTR:
    हाल MTYPE_PTR:
      iptr->val.ptr = बहु_तर्क(arglist, व्योम *);
      अवरोध;

    हाल MTYPE_LONGLONGU:
      iptr->val.numu = बहु_तर्क(arglist, uint64_t);
      अवरोध;

    हाल MTYPE_LONGLONG:
      iptr->val.nums = बहु_तर्क(arglist, int64_t);
      अवरोध;

    हाल MTYPE_LONGU:
      iptr->val.numu = बहु_तर्क(arglist, अचिन्हित दीर्घ);
      अवरोध;

    हाल MTYPE_LONG:
      iptr->val.nums = बहु_तर्क(arglist, दीर्घ);
      अवरोध;

    हाल MTYPE_INTU:
      iptr->val.numu = बहु_तर्क(arglist, अचिन्हित पूर्णांक);
      अवरोध;

    हाल MTYPE_INT:
    हाल MTYPE_WIDTH:
    हाल MTYPE_PRECISION:
      iptr->val.nums = बहु_तर्क(arglist, पूर्णांक);
      अवरोध;

    हाल MTYPE_DOUBLE:
      iptr->val.dnum = बहु_तर्क(arglist, द्विगुन);
      अवरोध;

    हाल MTYPE_LONGDOUBLE:
      iptr->val.dnum = (द्विगुन)बहु_तर्क(arglist, दीर्घ द्विगुन);
      अवरोध;

    शेष:
      DEBUGASSERT(शून्य); /* unexpected */
      अवरोध;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  वापस PFMT_OK;
}

काष्ठा mproperty {
  पूर्णांक width;            /* Width of a field. */
  पूर्णांक prec;             /* Precision of a field. */
  अचिन्हित पूर्णांक flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
प्रकार माप_प्रकार (*mp_streamn)(स्थिर अचिन्हित अक्षर *buf, माप_प्रकार len, व्योम *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
प्रकार पूर्णांक (*mp_flush)(व्योम *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
अटल bool stream_run(व्योम *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, व्योम *),
                       mp_streamn streamn,
                       स्थिर अचिन्हित अक्षर *buf, माप_प्रकार len, पूर्णांक *donep)
{
  अगर(!len)
    वापस FALSE;
  अगर(streamn) {
    माप_प्रकार n = streamn(buf, len, userp);
    *donep += (पूर्णांक)n;
    वापस n != len;
  }
  जबतक(len--) {
    अगर(stream(*buf++, userp))
      वापस TRUE;
    (*donep)++;
  }
  वापस FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
अटल स्थिर अक्षर pad_spaces[] = "                ";
अटल स्थिर अक्षर pad_zeros[] = "0000000000000000";
#घोषणा PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
अटल bool stream_pad(व्योम *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, व्योम *),
                       mp_streamn streamn,
                       अचिन्हित अक्षर pad, पूर्णांक अंक, पूर्णांक *donep)
{
  स्थिर अचिन्हित अक्षर *src;
  अगर(अंक <= 0)
    वापस FALSE;
  अगर(pad == '0')
    src = (स्थिर अचिन्हित अक्षर *)pad_zeros;
  अन्यथा
    src = (स्थिर अचिन्हित अक्षर *)pad_spaces;
  जबतक(अंक > PAD_RUN) {
    अगर(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      वापस TRUE;
    अंक -= PAD_RUN;
  }
  वापस stream_run(userp, stream, streamn, src, (माप_प्रकार)अंक, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
अटल bool stream_zrun(व्योम *userp,
                        पूर्णांक (*stream)(अचिन्हित अक्षर, व्योम *),
                        mp_streamn streamn,
                        स्थिर अक्षर *str, माप_प्रकार len, पूर्णांक *donep)
{
  माप_प्रकार n = 0;
  जबतक(n < len && str[n])
    n++;
  वापस stream_run(userp, stream, streamn,
                    (स्थिर अचिन्हित अक्षर *)str, n, donep);
}

अटल bool out_double(व्योम *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, व्योम *),
                       mp_streamn streamn,
                       काष्ठा mproperty *p,
                       द्विगुन dnum,
                       अक्षर *work, पूर्णांक *donep)
{
  अक्षर fmt[32] = "%";
  अक्षर *fptr = &fmt[1];
  माप_प्रकार बायाँ = माप(fmt) - म_माप(fmt);
  पूर्णांक flags = p->flags;
  पूर्णांक width = p->width;
  पूर्णांक prec = p->prec;

  अगर(flags & FLAGS_बायाँ)
    *fptr++ = '-';
  अगर(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  अगर(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  अगर(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  अगर(width >= 0) {
    माप_प्रकार dlen;
    अगर(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (माप_प्रकार)curl_msnprintf(fptr, बायाँ, "%d", width);
    fptr += dlen;
    बायाँ -= dlen;
  }
  अगर(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    पूर्णांक maxprec = BUFFSIZE - 1;
    द्विगुन val = dnum;
    पूर्णांक len;
    अगर(prec > maxprec)
      prec = maxprec - 1;
    अगर(width > 0 && prec <= width)
      maxprec -= width;
    जबतक(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    अगर(prec > maxprec)
      prec = maxprec - 1;
    अगर(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, बायाँ, ".%d", prec);
    fptr += len;
  }
  अगर(flags & FLAGS_LONG)
    *fptr++ = 'l';

  अगर(flags & FLAGS_FLOATE)
    *fptr++ = (अक्षर)((flags & FLAGS_UPPER) ? 'E' : 'e');
  अन्यथा अगर(flags & FLAGS_FLOATG)
    *fptr++ = (अक्षर)((flags & FLAGS_UPPER) ? 'G' : 'g');
  अन्यथा
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wformat-nonliteral"
#पूर्ण_अगर
#अगर_घोषित _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#पूर्ण_अगर
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
  DEBUGASSERT(म_माप(work) < BUFFSIZE);
  वापस stream_run(userp, stream, streamn,
                    (स्थिर अचिन्हित अक्षर *)work, म_माप(work), donep);
}

अटल bool out_संख्या(व्योम *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, व्योम *),
                       mp_streamn streamn,
                       काष्ठा mproperty *p,
                       uint64_t अंक,
                       int64_t nums,
                       अक्षर *work, पूर्णांक *donep)
{
  स्थिर अचिन्हित अक्षर *digits = Curl_ldigits;
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

  अगर(flags & FLAGS_CHAR) {
    /* Character. */
    अगर(!(flags & FLAGS_बायाँ)) {
      अगर(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        वापस TRUE;
      width = 1;
    }
    OUTCHAR((अक्षर)अंक);
    अगर(flags & FLAGS_बायाँ) {
      अगर(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        वापस TRUE;
    }
    वापस FALSE;
  }
  अगर(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  अन्यथा अगर(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  अन्यथा अगर(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  अन्यथा {
    /* Decimal integer. */
    is_neg = (nums < 0);
    अगर(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_अंक; /* Used to convert negative in positive. */
      signed_अंक = nums + (int64_t)1;
      signed_अंक = -signed_अंक;
      अंक = (uint64_t)signed_अंक;
      अंक += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  अगर(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  चयन(base) {
  हाल 10:
    जबतक(अंक > 0) {
      *w-- = (अक्षर)('0' + (अंक % 10));
      अंक /= 10;
    }
    अवरोध;
  शेष:
    जबतक(अंक > 0) {
      *w-- = digits[अंक % base];
      अंक /= base;
    }
    अवरोध;
  }
  width -= (पूर्णांक)(workend - w);
  prec -= (पूर्णांक)(workend - w);

  अगर(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  अगर(prec > 0) {
    width -= prec;
    जबतक(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  अगर(is_alt && base == 16)
    width -= 2;

  अगर(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  अगर(!(flags & FLAGS_बायाँ) && !(flags & FLAGS_PAD_NIL)) {
    अगर(stream_pad(userp, stream, streamn, ' ', width, donep))
      वापस TRUE;
    width = 0;
  }

  अगर(is_neg)
    OUTCHAR('-');
  अन्यथा अगर(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  अन्यथा अगर(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  अगर(is_alt && base == 16) {
    OUTCHAR('0');
    अगर(flags & FLAGS_UPPER)
      OUTCHAR('X');
    अन्यथा
      OUTCHAR('x');
  }

  अगर(!(flags & FLAGS_बायाँ) && (flags & FLAGS_PAD_NIL)) {
    अगर(stream_pad(userp, stream, streamn, '0', width, donep))
      वापस TRUE;
    width = 0;
  }

  /* Write the number. */
  अगर(stream_run(userp, stream, streamn, (स्थिर अचिन्हित अक्षर *)w + 1,
                (माप_प्रकार)(workend - w), donep))
    वापस TRUE;

  अगर(flags & FLAGS_बायाँ) {
    अगर(stream_pad(userp, stream, streamn, ' ', width, donep))
      वापस TRUE;
  }

  वापस FALSE;
}

अटल स्थिर अक्षर nilstr[] = "(nil)";

अटल bool out_string(व्योम *userp,
                       पूर्णांक (*stream)(अचिन्हित अक्षर, व्योम *),
                       mp_streamn streamn,
                       काष्ठा mproperty *p,
                       स्थिर अक्षर *str,
                       पूर्णांक *donep)
{
  पूर्णांक flags = p->flags;
  पूर्णांक width = p->width;
  पूर्णांक prec = p->prec;
  माप_प्रकार len;

  अगर(!str) {
    /* Write null string if there is space. */
    अगर(prec == -1 || prec >= (पूर्णांक)CURL_CSTRLEN(nilstr)) {
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
  अन्यथा अगर(prec != -1)
    len = (माप_प्रकार)prec;
  अन्यथा अगर(*str == '\0')
    len = 0;
  अन्यथा
    len = म_माप(str);

  width -= (len > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)len;

  अगर(flags & FLAGS_ALT)
    OUTCHAR('"');

  अगर(!(flags & FLAGS_बायाँ)) {
    अगर(stream_pad(userp, stream, streamn, ' ', width, donep))
      वापस TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  अगर(prec == -1) {
    अगर(stream_run(userp, stream, streamn,
                  (स्थिर अचिन्हित अक्षर *)str, len, donep))
      वापस TRUE;
  }
  अन्यथा अगर(stream_zrun(userp, stream, streamn, str, len, donep))
    वापस TRUE;
  अगर(flags & FLAGS_बायाँ) {
    अगर(stream_pad(userp, stream, streamn, ' ', width, donep))
      वापस TRUE;
  }

  अगर(flags & FLAGS_ALT)
    OUTCHAR('"');

  वापस FALSE;
}

अटल bool out_pointer(व्योम *userp,
                        पूर्णांक (*stream)(अचिन्हित अक्षर, व्योम *),
                        mp_streamn streamn,
                        काष्ठा mproperty *p,
                        स्थिर अक्षर *ptr,
                        अक्षर *work,
                        पूर्णांक *donep)
{
  /* Generic pointer. */
  अगर(ptr) {
    माप_प्रकार अंक = (माप_प्रकार)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    अगर(out_संख्या(userp, stream, streamn, p, अंक, 0, work, donep))
      वापस TRUE;
  }
  अन्यथा {
    /* Write "(nil)" for a nil pointer. */
    पूर्णांक width = p->width;
    पूर्णांक flags = p->flags;

    width -= (पूर्णांक)CURL_CSTRLEN(nilstr);
    अगर(flags & FLAGS_बायाँ) {
      अगर(stream_pad(userp, stream, streamn, ' ', width, donep))
        वापस TRUE;
      width = 0;
    }
    अगर(stream_run(userp, stream, streamn, (स्थिर अचिन्हित अक्षर *)nilstr,
                  माप(nilstr) - 1, donep))
      वापस TRUE;
    अगर(!(flags & FLAGS_बायाँ)) {
      अगर(stream_pad(userp, stream, streamn, ' ', width, donep))
        वापस TRUE;
    }
  }
  वापस FALSE;
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
अटल पूर्णांक formatf(व्योम *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   पूर्णांक (*stream)(अचिन्हित अक्षर, व्योम *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   स्थिर अक्षर *format, /* %-formatted string */
                   बहु_सूची ap_save) /* list of parameters */
{
  पूर्णांक done = 0;   /* number of characters written */
  पूर्णांक i;
  पूर्णांक ocount = 0; /* number of output segments */
  पूर्णांक icount = 0; /* number of input arguments */

  काष्ठा outsegment output[MAX_SEGMENTS];
  काष्ठा va_निवेश निवेश[MAX_PARAMETERS];
  अक्षर work[BUFFSIZE + 2];

  /* Parse the format string */
  अगर(parsefmt(format, output, निवेश, &ocount, &icount, ap_save))
    वापस 0;

  क्रम(i = 0; i < ocount; i++) {
    काष्ठा outsegment *optr = &output[i];
    काष्ठा va_निवेश *iptr = &निवेश[optr->निवेश];
    काष्ठा mproperty p;
    माप_प्रकार outlen = optr->outlen;

    अगर(outlen) {
      अगर(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        वापस done;
      अगर(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        जारी;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    अगर(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (पूर्णांक)निवेश[optr->width].val.nums;
      अगर(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        अगर(p.width == पूर्णांक_न्यून)
          p.width = पूर्णांक_उच्च;
        अन्यथा
          p.width = -p.width;
        p.flags |= FLAGS_बायाँ;
        p.flags &= ~(अचिन्हित पूर्णांक)FLAGS_PAD_NIL;
      }
    }
    अन्यथा
      p.width = optr->width;

    /* pick up the specified precision */
    अगर(p.flags & FLAGS_PRECPARAM) {
      p.prec = (पूर्णांक)निवेश[optr->precision].val.nums;
      अगर(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    अन्यथा अगर(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    अन्यथा
      p.prec = -1;

    चयन(iptr->type) {
    हाल MTYPE_INTU:
    हाल MTYPE_LONGU:
    हाल MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      अगर(out_संख्या(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        वापस done;
      अवरोध;

    हाल MTYPE_INT:
    हाल MTYPE_LONG:
    हाल MTYPE_LONGLONG:
      अगर(out_संख्या(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        वापस done;
      अवरोध;

    हाल MTYPE_STRING:
      अगर(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        वापस done;
      अवरोध;

    हाल MTYPE_PTR:
      अगर(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        वापस done;
      अवरोध;

    हाल MTYPE_DOUBLE:
    हाल MTYPE_LONGDOUBLE:
      अगर(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        वापस done;
      अवरोध;

    हाल MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      अगर(flush && flush(userp))
        वापस done;
      /* Answer the count of characters written. */
      अगर(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      अन्यथा अगर(p.flags & FLAGS_LONG)
        *(दीर्घ *)iptr->val.ptr = (दीर्घ)done;
      अन्यथा अगर(!(p.flags & FLAGS_SHORT))
        *(पूर्णांक *)iptr->val.ptr = done;
      अन्यथा
        *(लघु *)iptr->val.ptr = (लघु)done;
      अवरोध;

    शेष:
      अवरोध;
    }
  }
  वापस done;
}

/* fputc() look-alike */
अटल पूर्णांक addbyter(अचिन्हित अक्षर outc, व्योम *f)
{
  काष्ठा nsprintf *infop = f;
  अगर(infop->लंबाई < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->बफर++ = (अक्षर)outc; /* store */
    infop->लंबाई++; /* we are now one byte larger */
    वापस 0;     /* fputc() returns like this on success */
  }
  वापस 1;
}

/* block variant of addbyter */
अटल माप_प्रकार addrun(स्थिर अचिन्हित अक्षर *buf, माप_प्रकार len, व्योम *f)
{
  काष्ठा nsprintf *infop = f;
  माप_प्रकार fit = infop->max - infop->लंबाई;
  अगर(len > fit)
    len = fit;
  अगर(len) {
    स_नकल(infop->बफर, buf, len);
    infop->बफर += len;
    infop->लंबाई += len;
  }
  वापस len;
}

पूर्णांक curl_mvsnprintf(अक्षर *बफर, माप_प्रकार maxlength, स्थिर अक्षर *format,
                    बहु_सूची args)
{
  पूर्णांक retcode;
  काष्ठा nsprintf info;

  info.बफर = बफर;
  info.लंबाई = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, शून्य, format, args);
  अगर(info.max) {
    /* we null-terminate this */
    अगर(info.max == info.लंबाई) {
      /* we are at maximum, scrap the last letter */
      info.बफर[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    अन्यथा
      info.बफर[0] = 0;
  }
  वापस retcode;
}

पूर्णांक curl_msnprintf(अक्षर *बफर, माप_प्रकार maxlength, स्थिर अक्षर *format, ...)
{
  पूर्णांक retcode;
  बहु_सूची args; /* argument pointer */
  बहु_शुरू(args, format);
  retcode = curl_mvsnprintf(बफर, maxlength, format, args);
  बहु_पूर्ण(args);
  वापस retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
अटल पूर्णांक alloc_flush(व्योम *f)
{
  काष्ठा asprintf *infop = f;
  अगर(infop->nstage) {
    CURLcode परिणाम = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    अगर(परिणाम) {
      infop->merr = परिणाम == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      वापस 1; /* fail */
    }
  }
  वापस 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
अटल पूर्णांक alloc_addn(काष्ठा asprintf *infop, स्थिर अचिन्हित अक्षर *buf,
                      माप_प्रकार len)
{
  CURLcode परिणाम = curlx_dyn_addn(infop->b, buf, len);
  अगर(परिणाम) {
    infop->merr = परिणाम == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    वापस 1; /* fail */
  }
  वापस 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
अटल bool alloc_fits(काष्ठा asprintf *infop, माप_प्रकार more)
{
  वापस (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
अटल पूर्णांक alloc_addbyter(अचिन्हित अक्षर outc, व्योम *f)
{
  काष्ठा asprintf *infop = f;
  अगर(infop->nstage == माप(infop->stage) && alloc_flush(infop))
    वापस 1; /* fail */
  अगर(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    अगर(alloc_flush(infop))
      वापस 1;
    वापस alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  वापस 0;
}

/* block variant of alloc_addbyter */
अटल माप_प्रकार alloc_addrun(स्थिर अचिन्हित अक्षर *buf, माप_प्रकार len, व्योम *f)
{
  काष्ठा asprintf *infop = f;
  माप_प्रकार accepted = 0;
  जबतक(accepted < len) {
    माप_प्रकार room = माप(infop->stage) - infop->nstage;
    माप_प्रकार n;
    अगर(!room) {
      अगर(alloc_flush(infop))
        वापस accepted; /* accepted < len aborts formatting */
      room = माप(infop->stage);
    }
    n = len - accepted;
    अगर(n > room)
      n = room;
    अगर(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      माप_प्रकार fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      अगर(fits) {
        स_नकल(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      अगर(alloc_flush(infop))
        वापस accepted;
      अगर(alloc_addn(infop, buf + accepted, 1))
        वापस accepted; /* accepted < len aborts formatting */
      accepted++;
      जारी;
    }
    स_नकल(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  वापस accepted;
}

/* appends the formatted string, returns MERR error code */
पूर्णांक curlx_dyn_vprintf(काष्ठा dynbuf *dyn, स्थिर अक्षर *format, बहु_सूची args)
{
  काष्ठा asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (व्योम)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  अगर(!info.merr)
    (व्योम)alloc_flush(&info);
  अगर(info.merr) {
    curlx_dyn_free(info.b);
    वापस info.merr;
  }
  वापस 0;
}

अक्षर *curl_mvaprintf(स्थिर अक्षर *format, बहु_सूची args)
{
  काष्ठा asprintf info;
  काष्ठा dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (व्योम)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  अगर(!info.merr)
    (व्योम)alloc_flush(&info);
  अगर(info.merr) {
    curlx_dyn_free(info.b);
    वापस शून्य;
  }
  अगर(curlx_dyn_len(info.b))
    वापस curlx_dyn_ptr(info.b);
  वापस curlx_strdup("");
}

अक्षर *curl_maprintf(स्थिर अक्षर *format, ...)
{
  बहु_सूची args;
  अक्षर *s;
  बहु_शुरू(args, format);
  s = curl_mvaprintf(format, args);
  बहु_पूर्ण(args);
  वापस s;
}

अटल पूर्णांक storebuffer(अचिन्हित अक्षर outc, व्योम *f)
{
  अक्षर **बफर = f;
  **बफर = (अक्षर)outc;
  (*बफर)++;
  वापस 0;
}

/* block variant of storebuffer */
अटल माप_प्रकार storerun(स्थिर अचिन्हित अक्षर *buf, माप_प्रकार len, व्योम *f)
{
  अक्षर **बफर = f;
  स_नकल(*बफर, buf, len);
  *बफर += len;
  वापस len;
}

पूर्णांक curl_msprintf(अक्षर *बफर, स्थिर अक्षर *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णांक retcode;
  बहु_शुरू(args, format);
  retcode = formatf(&बफर, storebuffer, storerun, शून्य, format, args);
  बहु_पूर्ण(args);
  *बफर = 0; /* we null-terminate this */
  वापस retcode;
}

अटल पूर्णांक fputc_wrapper(अचिन्हित अक्षर outc, व्योम *f)
{
  काष्ठा ioprintf *i = f;
  पूर्णांक out = outc;
  खाता *s = i->fd;
  पूर्णांक rc = ख_अक्षर_दो(out, s);
  i->error = (rc == खातापूर्ण);
  वापस (rc == खातापूर्ण);
}

/* block variant of fputc_wrapper */
अटल माप_प्रकार fwrite_wrapper(स्थिर अचिन्हित अक्षर *buf, माप_प्रकार len, व्योम *f)
{
  काष्ठा ioprintf *i = f;
  माप_प्रकार n;
  खाता *s = i->fd;
  n = ख_डालो(buf, 1, len, s);
  i->error = (n != len);
  वापस n;
}

पूर्णांक curl_mvfprintf(खाता *fd, स्थिर अक्षर *format, बहु_सूची args)
{
  काष्ठा ioprintf info;
  पूर्णांक n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, शून्य, format, args);
  अगर(info.error)
    वापस -1;
  वापस n;
}

पूर्णांक curl_mfprintf(खाता *fd, स्थिर अक्षर *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णांक n;
  बहु_शुरू(args, format);
  n = curl_mvfprintf(fd, format, args);
  बहु_पूर्ण(args);
  वापस n;
}

पूर्णांक curl_mprintf(स्थिर अक्षर *format, ...)
{
  बहु_सूची args; /* argument pointer */
  पूर्णांक n;
  बहु_शुरू(args, format);
  n = curl_mvfprintf(मानक_निकास, format, args);
  बहु_पूर्ण(args);
  वापस n;
}

पूर्णांक curl_mvsprintf(अक्षर *बफर, स्थिर अक्षर *format, बहु_सूची args)
{
  पूर्णांक retcode = formatf(&बफर, storebuffer, storerun, शून्य, format, args);
  *बफर = 0; /* we null-terminate this */
  वापस retcode;
}

पूर्णांक curl_mvprintf(स्थिर अक्षर *format, बहु_सूची args)
{
  वापस curl_mvfprintf(मानक_निकास, format, args);
}

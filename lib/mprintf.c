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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "curl_printf.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
സ്ഥിരം unsigned അക്ഷരം Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
സ്ഥിരം unsigned അക്ഷരം Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  ചെയ്യുക {                                    \
    എങ്കിൽ(stream((unsigned അക്ഷരം)(x), userp)) \
      തിരികെ TRUE;                        \
    (*donep)++;                           \
  } ആകുമ്പോൾ(0)

/* Data type to read from the arglist */
തരനിർവചനം എണ്ണൽ {
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
എണ്ണൽ {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_ഇടത്       = 1 << 2,
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

എണ്ണൽ {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
ഘടന va_നിവേശം {
  FormatType type; /* FormatType */
  സംഘം {
    സ്ഥിരം അക്ഷരം *str;
    ശൂന്യം *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    ഇരട്ട dnum;
  } val;
};

/*
 * Describes an output segment.
 */
ഘടന outsegment {
  പൂർണ്ണസംഖ്യ width;     /* width OR width parameter number */
  പൂർണ്ണസംഖ്യ precision; /* precision OR precision parameter number */
  unsigned പൂർണ്ണസംഖ്യ flags;
  unsigned പൂർണ്ണസംഖ്യ നിവേശം; /* input argument array index */
  സ്ഥിരം അക്ഷരം *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

ഘടന nsprintf {
  അക്ഷരം *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

ഘടന asprintf {
  ഘടന dynbuf *b;
  അക്ഷരം merr;
  size_t nstage;
  unsigned അക്ഷരം stage[ASPRINTF_STAGE_SIZE];
};

ഘടന ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ dollarstring(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം **end)
{
  curl_off_t അക്കം;
  എങ്കിൽ(curlx_str_സംഖ്യ(&p, &അക്കം, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !അക്കം)
    തിരികെ -1;
  *end = p;
  തിരികെ (പൂർണ്ണസംഖ്യ)അക്കം - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned അക്ഷരം)(1 << ((y) & 7)))

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

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_flags(സ്ഥിരം അക്ഷരം **fmtp, unsigned പൂർണ്ണസംഖ്യ *flagsp, പൂർണ്ണസംഖ്യ use_dollar,
                       പൂർണ്ണസംഖ്യ *precp, പൂർണ്ണസംഖ്യ *widthp)
{
  സ്ഥിരം അക്ഷരം *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned പൂർണ്ണസംഖ്യ flags = 0;
  പൂർണ്ണസംഖ്യ width = 0;
  പൂർണ്ണസംഖ്യ precision = 0;

  /* Handle the flags */
  ചെയ്യുക {
    തിരഞ്ഞെടുപ്പ്(*fmt++) {
    അവസ്ഥ ' ':
      flags |= FLAGS_SPACE;
      നിർത്തുക;
    അവസ്ഥ '+':
      flags |= FLAGS_SHOWSIGN;
      നിർത്തുക;
    അവസ്ഥ '-':
      flags |= FLAGS_ഇടത്;
      flags &= ~(unsigned പൂർണ്ണസംഖ്യ)FLAGS_PAD_NIL;
      നിർത്തുക;
    അവസ്ഥ '#':
      flags |= FLAGS_ALT;
      നിർത്തുക;
    അവസ്ഥ '.':
      എങ്കിൽ('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        എങ്കിൽ(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          എങ്കിൽ(precision < 0)
            /* illegal combo */
            തിരികെ PFMT_DOLLARPREC;
        }
        അല്ലെങ്കിൽ
          /* get it from the next argument */
          precision = -1;
      }
      അല്ലെങ്കിൽ {
        bool is_neg;
        curl_off_t അക്കം;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        എങ്കിൽ(is_neg)
          fmt++;
        എങ്കിൽ(curlx_str_സംഖ്യ(&fmt, &അക്കം, INT_MAX))
          തിരികെ PFMT_PREC;
        precision = (പൂർണ്ണസംഖ്യ)അക്കം;
        എങ്കിൽ(is_neg)
          precision = -precision;
      }
      എങ്കിൽ((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        തിരികെ PFMT_PRECMIX;
      നിർത്തുക;
    അവസ്ഥ 'h':
      flags |= FLAGS_SHORT;
      നിർത്തുക;
#ifdef _WIN32
    അവസ്ഥ 'I':
      /* Non-ANSI integer extensions I32 I64 */
      എങ്കിൽ((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      അല്ലെങ്കിൽ എങ്കിൽ((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      അല്ലെങ്കിൽ {
#എങ്കിൽ SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#അല്ലെങ്കിൽ
        flags |= FLAGS_LONG;
#endif
      }
      നിർത്തുക;
#endif /* _WIN32 */
    അവസ്ഥ 'l':
      എങ്കിൽ(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      അല്ലെങ്കിൽ
        flags |= FLAGS_LONG;
      നിർത്തുക;
    അവസ്ഥ 'L':
      flags |= FLAGS_LONGDOUBLE;
      നിർത്തുക;
    അവസ്ഥ 'q':
      flags |= FLAGS_LONGLONG;
      നിർത്തുക;
    അവസ്ഥ 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#എങ്കിൽ SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#അല്ലെങ്കിൽ
      flags |= FLAGS_LONG;
#endif
      നിർത്തുക;
    അവസ്ഥ 'O':
#എങ്കിൽ SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#അല്ലെങ്കിൽ
      flags |= FLAGS_LONG;
#endif
      നിർത്തുക;
    അവസ്ഥ '0':
      എങ്കിൽ(!(flags & FLAGS_ഇടത്))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    അവസ്ഥ '1':
    അവസ്ഥ '2':
    അവസ്ഥ '3':
    അവസ്ഥ '4':
    അവസ്ഥ '5':
    അവസ്ഥ '6':
    അവസ്ഥ '7':
    അവസ്ഥ '8':
    അവസ്ഥ '9': {
      curl_off_t അക്കം;
      flags |= FLAGS_WIDTH;
      fmt--;
      എങ്കിൽ(curlx_str_സംഖ്യ(&fmt, &അക്കം, INT_MAX))
        തിരികെ PFMT_WIDTH;
      width = (പൂർണ്ണസംഖ്യ)അക്കം;
      നിർത്തുക;
    }
    അവസ്ഥ '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      എങ്കിൽ(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        എങ്കിൽ(width < 0)
          /* illegal combo */
          തിരികെ PFMT_DOLLARWIDTH;
      }
      അല്ലെങ്കിൽ
        /* pick from the next argument */
        width = -1;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      loopit = FALSE;
      fmt--;
      നിർത്തുക;
    } /* switch */
  } ആകുമ്പോൾ(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  തിരികെ PFMT_OK;
}

സ്ഥിതം bool parse_conversion(സ്ഥിരം അക്ഷരം f, unsigned പൂർണ്ണസംഖ്യ *flagp,
                             FormatType *typep)
{
  unsigned പൂർണ്ണസംഖ്യ flags = *flagp;
  FormatType type;
  തിരഞ്ഞെടുപ്പ്(f) {
  അവസ്ഥ 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    നിർത്തുക;
  അവസ്ഥ 's':
    type = MTYPE_STRING;
    നിർത്തുക;
  അവസ്ഥ 'n':
    type = MTYPE_INTPTR;
    നിർത്തുക;
  അവസ്ഥ 'p':
    type = MTYPE_PTR;
    നിർത്തുക;
  അവസ്ഥ 'd':
  അവസ്ഥ 'i':
    എങ്കിൽ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    അല്ലെങ്കിൽ
      type = MTYPE_INT;
    നിർത്തുക;
  അവസ്ഥ 'u':
    എങ്കിൽ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    അല്ലെങ്കിൽ
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    നിർത്തുക;
  അവസ്ഥ 'o':
    എങ്കിൽ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    അല്ലെങ്കിൽ
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    നിർത്തുക;
  അവസ്ഥ 'x':
    എങ്കിൽ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    അല്ലെങ്കിൽ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    നിർത്തുക;
  അവസ്ഥ 'X':
    എങ്കിൽ(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    അല്ലെങ്കിൽ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    നിർത്തുക;
  അവസ്ഥ 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    നിർത്തുക;
  അവസ്ഥ 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    നിർത്തുക;
  അവസ്ഥ 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    നിർത്തുക;
  അവസ്ഥ 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    നിർത്തുക;
  അവസ്ഥ 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    നിർത്തുക;
  അവസ്ഥ 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    നിർത്തുക;
  അവസ്ഥ 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    /* invalid instruction, disregard and continue */
    തിരികെ TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  തിരികെ FALSE;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parsefmt(സ്ഥിരം അക്ഷരം *format,
                    ഘടന outsegment *out,
                    ഘടന va_നിവേശം *in,
                    പൂർണ്ണസംഖ്യ *opieces,
                    പൂർണ്ണസംഖ്യ *ipieces, va_list arglist)
{
  സ്ഥിരം അക്ഷരം *fmt = format;
  പൂർണ്ണസംഖ്യ param_അക്കം = 0;
  പൂർണ്ണസംഖ്യ max_param = -1;
  പൂർണ്ണസംഖ്യ i;
  പൂർണ്ണസംഖ്യ ocount = 0;
  unsigned അക്ഷരം usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  ഘടന outsegment *optr;
  പൂർണ്ണസംഖ്യ use_dollar = DOLLAR_UNKNOWN;
  സ്ഥിരം അക്ഷരം *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, വലുപ്പം(usedinput));

  ആകുമ്പോൾ(*fmt) {
    എങ്കിൽ(*fmt == '%') {
      ഘടന va_നിവേശം *iptr;
      FormatType type;
      unsigned പൂർണ്ണസംഖ്യ flags = 0;
      പൂർണ്ണസംഖ്യ width = 0;
      പൂർണ്ണസംഖ്യ precision = 0;
      പൂർണ്ണസംഖ്യ param = -1;
      പൂർണ്ണസംഖ്യ rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      എങ്കിൽ(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        എങ്കിൽ(outlen) {
          optr = &out[ocount++];
          എങ്കിൽ(ocount > MAX_SEGMENTS)
            തിരികെ PFMT_MANYSEGS;
          optr->നിവേശം = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        തുടരുക; /* while */
      }

      എങ്കിൽ(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        എങ്കിൽ(param < 0) {
          എങ്കിൽ(use_dollar == DOLLAR_USE)
            /* illegal combo */
            തിരികെ PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        അല്ലെങ്കിൽ
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      എങ്കിൽ(rc)
        തിരികെ rc;

      എങ്കിൽ(parse_conversion(*fmt, &flags, &type))
        തുടരുക;

      എങ്കിൽ(flags & FLAGS_WIDTHPARAM) {
        എങ്കിൽ(width < 0)
          width = param_അക്കം++;
        അല്ലെങ്കിൽ {
          /* if this identifies a parameter already used, this is illegal */
          എങ്കിൽ(is_arg_used(usedinput, width))
            തിരികെ PFMT_WIDTHARG;
        }
        എങ്കിൽ(width >= MAX_PARAMETERS)
          തിരികെ PFMT_MANYARGS;
        എങ്കിൽ(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      എങ്കിൽ(flags & FLAGS_PRECPARAM) {
        എങ്കിൽ(precision < 0)
          precision = param_അക്കം++;
        അല്ലെങ്കിൽ {
          /* if this identifies a parameter already used, this is illegal */
          എങ്കിൽ(is_arg_used(usedinput, precision))
            തിരികെ PFMT_PRECARG;
        }
        എങ്കിൽ(precision >= MAX_PARAMETERS)
          തിരികെ PFMT_MANYARGS;
        എങ്കിൽ(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      എങ്കിൽ(param < 0)
        param = param_അക്കം++;
      എങ്കിൽ(param >= MAX_PARAMETERS)
        തിരികെ PFMT_MANYARGS;
      എങ്കിൽ(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      എങ്കിൽ(ocount > MAX_SEGMENTS)
        തിരികെ PFMT_MANYSEGS;
      optr->നിവേശം = (unsigned പൂർണ്ണസംഖ്യ)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    അല്ലെങ്കിൽ
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  എങ്കിൽ(outlen) {
    optr = &out[ocount++];
    എങ്കിൽ(ocount > MAX_SEGMENTS)
      തിരികെ PFMT_MANYSEGS;
    optr->നിവേശം = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  വേണ്ടി(i = 0; i < max_param + 1; i++) {
    ഘടന va_നിവേശം *iptr = &in[i];
    എങ്കിൽ(!is_arg_used(usedinput, i))
      /* bad input */
      തിരികെ PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    തിരഞ്ഞെടുപ്പ്(iptr->type) {
    അവസ്ഥ MTYPE_STRING:
      iptr->val.str = va_arg(arglist, സ്ഥിരം അക്ഷരം *);
      നിർത്തുക;

    അവസ്ഥ MTYPE_INTPTR:
    അവസ്ഥ MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, ശൂന്യം *);
      നിർത്തുക;

    അവസ്ഥ MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      നിർത്തുക;

    അവസ്ഥ MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      നിർത്തുക;

    അവസ്ഥ MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      നിർത്തുക;

    അവസ്ഥ MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      നിർത്തുക;

    അവസ്ഥ MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned പൂർണ്ണസംഖ്യ);
      നിർത്തുക;

    അവസ്ഥ MTYPE_INT:
    അവസ്ഥ MTYPE_WIDTH:
    അവസ്ഥ MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, പൂർണ്ണസംഖ്യ);
      നിർത്തുക;

    അവസ്ഥ MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, ഇരട്ട);
      നിർത്തുക;

    അവസ്ഥ MTYPE_LONGDOUBLE:
      iptr->val.dnum = (ഇരട്ട)va_arg(arglist, long ഇരട്ട);
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      DEBUGASSERT(ശൂന്യ); /* unexpected */
      നിർത്തുക;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  തിരികെ PFMT_OK;
}

ഘടന mproperty {
  പൂർണ്ണസംഖ്യ width;            /* Width of a field. */
  പൂർണ്ണസംഖ്യ prec;             /* Precision of a field. */
  unsigned പൂർണ്ണസംഖ്യ flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
തരനിർവചനം size_t (*mp_streamn)(സ്ഥിരം unsigned അക്ഷരം *buf, size_t len, ശൂന്യം *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
തരനിർവചനം പൂർണ്ണസംഖ്യ (*mp_flush)(ശൂന്യം *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
സ്ഥിതം bool stream_run(ശൂന്യം *userp,
                       പൂർണ്ണസംഖ്യ (*stream)(unsigned അക്ഷരം, ശൂന്യം *),
                       mp_streamn streamn,
                       സ്ഥിരം unsigned അക്ഷരം *buf, size_t len, പൂർണ്ണസംഖ്യ *donep)
{
  എങ്കിൽ(!len)
    തിരികെ FALSE;
  എങ്കിൽ(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (പൂർണ്ണസംഖ്യ)n;
    തിരികെ n != len;
  }
  ആകുമ്പോൾ(len--) {
    എങ്കിൽ(stream(*buf++, userp))
      തിരികെ TRUE;
    (*donep)++;
  }
  തിരികെ FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
സ്ഥിതം സ്ഥിരം അക്ഷരം pad_spaces[] = "                ";
സ്ഥിതം സ്ഥിരം അക്ഷരം pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
സ്ഥിതം bool stream_pad(ശൂന്യം *userp,
                       പൂർണ്ണസംഖ്യ (*stream)(unsigned അക്ഷരം, ശൂന്യം *),
                       mp_streamn streamn,
                       unsigned അക്ഷരം pad, പൂർണ്ണസംഖ്യ അക്കം, പൂർണ്ണസംഖ്യ *donep)
{
  സ്ഥിരം unsigned അക്ഷരം *src;
  എങ്കിൽ(അക്കം <= 0)
    തിരികെ FALSE;
  എങ്കിൽ(pad == '0')
    src = (സ്ഥിരം unsigned അക്ഷരം *)pad_zeros;
  അല്ലെങ്കിൽ
    src = (സ്ഥിരം unsigned അക്ഷരം *)pad_spaces;
  ആകുമ്പോൾ(അക്കം > PAD_RUN) {
    എങ്കിൽ(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      തിരികെ TRUE;
    അക്കം -= PAD_RUN;
  }
  തിരികെ stream_run(userp, stream, streamn, src, (size_t)അക്കം, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
സ്ഥിതം bool stream_zrun(ശൂന്യം *userp,
                        പൂർണ്ണസംഖ്യ (*stream)(unsigned അക്ഷരം, ശൂന്യം *),
                        mp_streamn streamn,
                        സ്ഥിരം അക്ഷരം *str, size_t len, പൂർണ്ണസംഖ്യ *donep)
{
  size_t n = 0;
  ആകുമ്പോൾ(n < len && str[n])
    n++;
  തിരികെ stream_run(userp, stream, streamn,
                    (സ്ഥിരം unsigned അക്ഷരം *)str, n, donep);
}

സ്ഥിതം bool out_double(ശൂന്യം *userp,
                       പൂർണ്ണസംഖ്യ (*stream)(unsigned അക്ഷരം, ശൂന്യം *),
                       mp_streamn streamn,
                       ഘടന mproperty *p,
                       ഇരട്ട dnum,
                       അക്ഷരം *work, പൂർണ്ണസംഖ്യ *donep)
{
  അക്ഷരം fmt[32] = "%";
  അക്ഷരം *fptr = &fmt[1];
  size_t ഇടത് = വലുപ്പം(fmt) - strlen(fmt);
  പൂർണ്ണസംഖ്യ flags = p->flags;
  പൂർണ്ണസംഖ്യ width = p->width;
  പൂർണ്ണസംഖ്യ prec = p->prec;

  എങ്കിൽ(flags & FLAGS_ഇടത്)
    *fptr++ = '-';
  എങ്കിൽ(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  എങ്കിൽ(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  എങ്കിൽ(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  എങ്കിൽ(width >= 0) {
    size_t dlen;
    എങ്കിൽ(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, ഇടത്, "%d", width);
    fptr += dlen;
    ഇടത് -= dlen;
  }
  എങ്കിൽ(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    പൂർണ്ണസംഖ്യ maxprec = BUFFSIZE - 1;
    ഇരട്ട val = dnum;
    പൂർണ്ണസംഖ്യ len;
    എങ്കിൽ(prec > maxprec)
      prec = maxprec - 1;
    എങ്കിൽ(width > 0 && prec <= width)
      maxprec -= width;
    ആകുമ്പോൾ(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    എങ്കിൽ(prec > maxprec)
      prec = maxprec - 1;
    എങ്കിൽ(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, ഇടത്, ".%d", prec);
    fptr += len;
  }
  എങ്കിൽ(flags & FLAGS_LONG)
    *fptr++ = 'l';

  എങ്കിൽ(flags & FLAGS_FLOATE)
    *fptr++ = (അക്ഷരം)((flags & FLAGS_UPPER) ? 'E' : 'e');
  അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_FLOATG)
    *fptr++ = (അക്ഷരം)((flags & FLAGS_UPPER) ? 'G' : 'g');
  അല്ലെങ്കിൽ
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#അല്ലെങ്കിൽ
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic എടുക്കുക
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  തിരികെ stream_run(userp, stream, streamn,
                    (സ്ഥിരം unsigned അക്ഷരം *)work, strlen(work), donep);
}

സ്ഥിതം bool out_സംഖ്യ(ശൂന്യം *userp,
                       പൂർണ്ണസംഖ്യ (*stream)(unsigned അക്ഷരം, ശൂന്യം *),
                       mp_streamn streamn,
                       ഘടന mproperty *p,
                       uint64_t അക്കം,
                       int64_t nums,
                       അക്ഷരം *work, പൂർണ്ണസംഖ്യ *donep)
{
  സ്ഥിരം unsigned അക്ഷരം *digits = Curl_ldigits;
  പൂർണ്ണസംഖ്യ flags = p->flags;
  പൂർണ്ണസംഖ്യ width = p->width;
  പൂർണ്ണസംഖ്യ prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  പൂർണ്ണസംഖ്യ base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  അക്ഷരം *workend = &work[BUFFSIZE - 2];
  അക്ഷരം *w;

  എങ്കിൽ(flags & FLAGS_CHAR) {
    /* Character. */
    എങ്കിൽ(!(flags & FLAGS_ഇടത്)) {
      എങ്കിൽ(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        തിരികെ TRUE;
      width = 1;
    }
    OUTCHAR((അക്ഷരം)അക്കം);
    എങ്കിൽ(flags & FLAGS_ഇടത്) {
      എങ്കിൽ(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        തിരികെ TRUE;
    }
    തിരികെ FALSE;
  }
  എങ്കിൽ(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  അല്ലെങ്കിൽ {
    /* Decimal integer. */
    is_neg = (nums < 0);
    എങ്കിൽ(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_അക്കം; /* Used to convert negative in positive. */
      signed_അക്കം = nums + (int64_t)1;
      signed_അക്കം = -signed_അക്കം;
      അക്കം = (uint64_t)signed_അക്കം;
      അക്കം += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  എങ്കിൽ(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  തിരഞ്ഞെടുപ്പ്(base) {
  അവസ്ഥ 10:
    ആകുമ്പോൾ(അക്കം > 0) {
      *w-- = (അക്ഷരം)('0' + (അക്കം % 10));
      അക്കം /= 10;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    ആകുമ്പോൾ(അക്കം > 0) {
      *w-- = digits[അക്കം % base];
      അക്കം /= base;
    }
    നിർത്തുക;
  }
  width -= (പൂർണ്ണസംഖ്യ)(workend - w);
  prec -= (പൂർണ്ണസംഖ്യ)(workend - w);

  എങ്കിൽ(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  എങ്കിൽ(prec > 0) {
    width -= prec;
    ആകുമ്പോൾ(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  എങ്കിൽ(is_alt && base == 16)
    width -= 2;

  എങ്കിൽ(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  എങ്കിൽ(!(flags & FLAGS_ഇടത്) && !(flags & FLAGS_PAD_NIL)) {
    എങ്കിൽ(stream_pad(userp, stream, streamn, ' ', width, donep))
      തിരികെ TRUE;
    width = 0;
  }

  എങ്കിൽ(is_neg)
    OUTCHAR('-');
  അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  അല്ലെങ്കിൽ എങ്കിൽ(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  എങ്കിൽ(is_alt && base == 16) {
    OUTCHAR('0');
    എങ്കിൽ(flags & FLAGS_UPPER)
      OUTCHAR('X');
    അല്ലെങ്കിൽ
      OUTCHAR('x');
  }

  എങ്കിൽ(!(flags & FLAGS_ഇടത്) && (flags & FLAGS_PAD_NIL)) {
    എങ്കിൽ(stream_pad(userp, stream, streamn, '0', width, donep))
      തിരികെ TRUE;
    width = 0;
  }

  /* Write the number. */
  എങ്കിൽ(stream_run(userp, stream, streamn, (സ്ഥിരം unsigned അക്ഷരം *)w + 1,
                (size_t)(workend - w), donep))
    തിരികെ TRUE;

  എങ്കിൽ(flags & FLAGS_ഇടത്) {
    എങ്കിൽ(stream_pad(userp, stream, streamn, ' ', width, donep))
      തിരികെ TRUE;
  }

  തിരികെ FALSE;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം nilstr[] = "(nil)";

സ്ഥിതം bool out_string(ശൂന്യം *userp,
                       പൂർണ്ണസംഖ്യ (*stream)(unsigned അക്ഷരം, ശൂന്യം *),
                       mp_streamn streamn,
                       ഘടന mproperty *p,
                       സ്ഥിരം അക്ഷരം *str,
                       പൂർണ്ണസംഖ്യ *donep)
{
  പൂർണ്ണസംഖ്യ flags = p->flags;
  പൂർണ്ണസംഖ്യ width = p->width;
  പൂർണ്ണസംഖ്യ prec = p->prec;
  size_t len;

  എങ്കിൽ(!str) {
    /* Write null string if there is space. */
    എങ്കിൽ(prec == -1 || prec >= (പൂർണ്ണസംഖ്യ)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned പൂർണ്ണസംഖ്യ)FLAGS_ALT;
    }
    അല്ലെങ്കിൽ {
      str = "";
      len = 0;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(prec != -1)
    len = (size_t)prec;
  അല്ലെങ്കിൽ എങ്കിൽ(*str == '\0')
    len = 0;
  അല്ലെങ്കിൽ
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (പൂർണ്ണസംഖ്യ)len;

  എങ്കിൽ(flags & FLAGS_ALT)
    OUTCHAR('"');

  എങ്കിൽ(!(flags & FLAGS_ഇടത്)) {
    എങ്കിൽ(stream_pad(userp, stream, streamn, ' ', width, donep))
      തിരികെ TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  എങ്കിൽ(prec == -1) {
    എങ്കിൽ(stream_run(userp, stream, streamn,
                  (സ്ഥിരം unsigned അക്ഷരം *)str, len, donep))
      തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream_zrun(userp, stream, streamn, str, len, donep))
    തിരികെ TRUE;
  എങ്കിൽ(flags & FLAGS_ഇടത്) {
    എങ്കിൽ(stream_pad(userp, stream, streamn, ' ', width, donep))
      തിരികെ TRUE;
  }

  എങ്കിൽ(flags & FLAGS_ALT)
    OUTCHAR('"');

  തിരികെ FALSE;
}

സ്ഥിതം bool out_pointer(ശൂന്യം *userp,
                        പൂർണ്ണസംഖ്യ (*stream)(unsigned അക്ഷരം, ശൂന്യം *),
                        mp_streamn streamn,
                        ഘടന mproperty *p,
                        സ്ഥിരം അക്ഷരം *ptr,
                        അക്ഷരം *work,
                        പൂർണ്ണസംഖ്യ *donep)
{
  /* Generic pointer. */
  എങ്കിൽ(ptr) {
    size_t അക്കം = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    എങ്കിൽ(out_സംഖ്യ(userp, stream, streamn, p, അക്കം, 0, work, donep))
      തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ {
    /* Write "(nil)" for a nil pointer. */
    പൂർണ്ണസംഖ്യ width = p->width;
    പൂർണ്ണസംഖ്യ flags = p->flags;

    width -= (പൂർണ്ണസംഖ്യ)CURL_CSTRLEN(nilstr);
    എങ്കിൽ(flags & FLAGS_ഇടത്) {
      എങ്കിൽ(stream_pad(userp, stream, streamn, ' ', width, donep))
        തിരികെ TRUE;
      width = 0;
    }
    എങ്കിൽ(stream_run(userp, stream, streamn, (സ്ഥിരം unsigned അക്ഷരം *)nilstr,
                  വലുപ്പം(nilstr) - 1, donep))
      തിരികെ TRUE;
    എങ്കിൽ(!(flags & FLAGS_ഇടത്)) {
      എങ്കിൽ(stream_pad(userp, stream, streamn, ' ', width, donep))
        തിരികെ TRUE;
    }
  }
  തിരികെ FALSE;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ formatf(ശൂന്യം *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   പൂർണ്ണസംഖ്യ (*stream)(unsigned അക്ഷരം, ശൂന്യം *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   സ്ഥിരം അക്ഷരം *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  പൂർണ്ണസംഖ്യ done = 0;   /* number of characters written */
  പൂർണ്ണസംഖ്യ i;
  പൂർണ്ണസംഖ്യ ocount = 0; /* number of output segments */
  പൂർണ്ണസംഖ്യ icount = 0; /* number of input arguments */

  ഘടന outsegment output[MAX_SEGMENTS];
  ഘടന va_നിവേശം നിവേശം[MAX_PARAMETERS];
  അക്ഷരം work[BUFFSIZE + 2];

  /* Parse the format string */
  എങ്കിൽ(parsefmt(format, output, നിവേശം, &ocount, &icount, ap_save))
    തിരികെ 0;

  വേണ്ടി(i = 0; i < ocount; i++) {
    ഘടന outsegment *optr = &output[i];
    ഘടന va_നിവേശം *iptr = &നിവേശം[optr->നിവേശം];
    ഘടന mproperty p;
    size_t outlen = optr->outlen;

    എങ്കിൽ(outlen) {
      എങ്കിൽ(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        തിരികെ done;
      എങ്കിൽ(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        തുടരുക;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    എങ്കിൽ(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (പൂർണ്ണസംഖ്യ)നിവേശം[optr->width].val.nums;
      എങ്കിൽ(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        എങ്കിൽ(p.width == INT_MIN)
          p.width = INT_MAX;
        അല്ലെങ്കിൽ
          p.width = -p.width;
        p.flags |= FLAGS_ഇടത്;
        p.flags &= ~(unsigned പൂർണ്ണസംഖ്യ)FLAGS_PAD_NIL;
      }
    }
    അല്ലെങ്കിൽ
      p.width = optr->width;

    /* pick up the specified precision */
    എങ്കിൽ(p.flags & FLAGS_PRECPARAM) {
      p.prec = (പൂർണ്ണസംഖ്യ)നിവേശം[optr->precision].val.nums;
      എങ്കിൽ(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    അല്ലെങ്കിൽ
      p.prec = -1;

    തിരഞ്ഞെടുപ്പ്(iptr->type) {
    അവസ്ഥ MTYPE_INTU:
    അവസ്ഥ MTYPE_LONGU:
    അവസ്ഥ MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      എങ്കിൽ(out_സംഖ്യ(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        തിരികെ done;
      നിർത്തുക;

    അവസ്ഥ MTYPE_INT:
    അവസ്ഥ MTYPE_LONG:
    അവസ്ഥ MTYPE_LONGLONG:
      എങ്കിൽ(out_സംഖ്യ(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        തിരികെ done;
      നിർത്തുക;

    അവസ്ഥ MTYPE_STRING:
      എങ്കിൽ(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        തിരികെ done;
      നിർത്തുക;

    അവസ്ഥ MTYPE_PTR:
      എങ്കിൽ(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        തിരികെ done;
      നിർത്തുക;

    അവസ്ഥ MTYPE_DOUBLE:
    അവസ്ഥ MTYPE_LONGDOUBLE:
      എങ്കിൽ(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        തിരികെ done;
      നിർത്തുക;

    അവസ്ഥ MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      എങ്കിൽ(flush && flush(userp))
        തിരികെ done;
      /* Answer the count of characters written. */
      എങ്കിൽ(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      അല്ലെങ്കിൽ എങ്കിൽ(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      അല്ലെങ്കിൽ എങ്കിൽ(!(p.flags & FLAGS_SHORT))
        *(പൂർണ്ണസംഖ്യ *)iptr->val.ptr = done;
      അല്ലെങ്കിൽ
        *(short *)iptr->val.ptr = (short)done;
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
  }
  തിരികെ done;
}

/* fputc() look-alike */
സ്ഥിതം പൂർണ്ണസംഖ്യ addbyter(unsigned അക്ഷരം outc, ശൂന്യം *f)
{
  ഘടന nsprintf *infop = f;
  എങ്കിൽ(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (അക്ഷരം)outc; /* store */
    infop->length++; /* we are now one byte larger */
    തിരികെ 0;     /* fputc() returns like this on success */
  }
  തിരികെ 1;
}

/* block variant of addbyter */
സ്ഥിതം size_t addrun(സ്ഥിരം unsigned അക്ഷരം *buf, size_t len, ശൂന്യം *f)
{
  ഘടന nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  എങ്കിൽ(len > fit)
    len = fit;
  എങ്കിൽ(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  തിരികെ len;
}

പൂർണ്ണസംഖ്യ curl_mvsnprintf(അക്ഷരം *buffer, size_t maxlength, സ്ഥിരം അക്ഷരം *format,
                    va_list args)
{
  പൂർണ്ണസംഖ്യ retcode;
  ഘടന nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, ശൂന്യ, format, args);
  എങ്കിൽ(info.max) {
    /* we null-terminate this */
    എങ്കിൽ(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    അല്ലെങ്കിൽ
      info.buffer[0] = 0;
  }
  തിരികെ retcode;
}

പൂർണ്ണസംഖ്യ curl_msnprintf(അക്ഷരം *buffer, size_t maxlength, സ്ഥിരം അക്ഷരം *format, ...)
{
  പൂർണ്ണസംഖ്യ retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  തിരികെ retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
സ്ഥിതം പൂർണ്ണസംഖ്യ alloc_flush(ശൂന്യം *f)
{
  ഘടന asprintf *infop = f;
  എങ്കിൽ(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    എങ്കിൽ(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      തിരികെ 1; /* fail */
    }
  }
  തിരികെ 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
സ്ഥിതം പൂർണ്ണസംഖ്യ alloc_addn(ഘടന asprintf *infop, സ്ഥിരം unsigned അക്ഷരം *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  എങ്കിൽ(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    തിരികെ 1; /* fail */
  }
  തിരികെ 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
സ്ഥിതം bool alloc_fits(ഘടന asprintf *infop, size_t more)
{
  തിരികെ (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
സ്ഥിതം പൂർണ്ണസംഖ്യ alloc_addbyter(unsigned അക്ഷരം outc, ശൂന്യം *f)
{
  ഘടന asprintf *infop = f;
  എങ്കിൽ(infop->nstage == വലുപ്പം(infop->stage) && alloc_flush(infop))
    തിരികെ 1; /* fail */
  എങ്കിൽ(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    എങ്കിൽ(alloc_flush(infop))
      തിരികെ 1;
    തിരികെ alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  തിരികെ 0;
}

/* block variant of alloc_addbyter */
സ്ഥിതം size_t alloc_addrun(സ്ഥിരം unsigned അക്ഷരം *buf, size_t len, ശൂന്യം *f)
{
  ഘടന asprintf *infop = f;
  size_t accepted = 0;
  ആകുമ്പോൾ(accepted < len) {
    size_t room = വലുപ്പം(infop->stage) - infop->nstage;
    size_t n;
    എങ്കിൽ(!room) {
      എങ്കിൽ(alloc_flush(infop))
        തിരികെ accepted; /* accepted < len aborts formatting */
      room = വലുപ്പം(infop->stage);
    }
    n = len - accepted;
    എങ്കിൽ(n > room)
      n = room;
    എങ്കിൽ(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      എങ്കിൽ(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      എങ്കിൽ(alloc_flush(infop))
        തിരികെ accepted;
      എങ്കിൽ(alloc_addn(infop, buf + accepted, 1))
        തിരികെ accepted; /* accepted < len aborts formatting */
      accepted++;
      തുടരുക;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  തിരികെ accepted;
}

/* appends the formatted string, returns MERR error code */
പൂർണ്ണസംഖ്യ curlx_dyn_vprintf(ഘടന dynbuf *dyn, സ്ഥിരം അക്ഷരം *format, va_list args)
{
  ഘടന asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (ശൂന്യം)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  എങ്കിൽ(!info.merr)
    (ശൂന്യം)alloc_flush(&info);
  എങ്കിൽ(info.merr) {
    curlx_dyn_free(info.b);
    തിരികെ info.merr;
  }
  തിരികെ 0;
}

അക്ഷരം *curl_mvaprintf(സ്ഥിരം അക്ഷരം *format, va_list args)
{
  ഘടന asprintf info;
  ഘടന dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (ശൂന്യം)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  എങ്കിൽ(!info.merr)
    (ശൂന്യം)alloc_flush(&info);
  എങ്കിൽ(info.merr) {
    curlx_dyn_free(info.b);
    തിരികെ ശൂന്യ;
  }
  എങ്കിൽ(curlx_dyn_len(info.b))
    തിരികെ curlx_dyn_ptr(info.b);
  തിരികെ curlx_strdup("");
}

അക്ഷരം *curl_maprintf(സ്ഥിരം അക്ഷരം *format, ...)
{
  va_list args;
  അക്ഷരം *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  തിരികെ s;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ storebuffer(unsigned അക്ഷരം outc, ശൂന്യം *f)
{
  അക്ഷരം **buffer = f;
  **buffer = (അക്ഷരം)outc;
  (*buffer)++;
  തിരികെ 0;
}

/* block variant of storebuffer */
സ്ഥിതം size_t storerun(സ്ഥിരം unsigned അക്ഷരം *buf, size_t len, ശൂന്യം *f)
{
  അക്ഷരം **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  തിരികെ len;
}

പൂർണ്ണസംഖ്യ curl_msprintf(അക്ഷരം *buffer, സ്ഥിരം അക്ഷരം *format, ...)
{
  va_list args; /* argument pointer */
  പൂർണ്ണസംഖ്യ retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, ശൂന്യ, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  തിരികെ retcode;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fputc_wrapper(unsigned അക്ഷരം outc, ശൂന്യം *f)
{
  ഘടന ioprintf *i = f;
  പൂർണ്ണസംഖ്യ out = outc;
  FILE *s = i->fd;
  പൂർണ്ണസംഖ്യ rc = fputc(out, s);
  i->error = (rc == EOF);
  തിരികെ (rc == EOF);
}

/* block variant of fputc_wrapper */
സ്ഥിതം size_t fwrite_wrapper(സ്ഥിരം unsigned അക്ഷരം *buf, size_t len, ശൂന്യം *f)
{
  ഘടന ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  തിരികെ n;
}

പൂർണ്ണസംഖ്യ curl_mvfprintf(FILE *fd, സ്ഥിരം അക്ഷരം *format, va_list args)
{
  ഘടന ioprintf info;
  പൂർണ്ണസംഖ്യ n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, ശൂന്യ, format, args);
  എങ്കിൽ(info.error)
    തിരികെ -1;
  തിരികെ n;
}

പൂർണ്ണസംഖ്യ curl_mfprintf(FILE *fd, സ്ഥിരം അക്ഷരം *format, ...)
{
  va_list args; /* argument pointer */
  പൂർണ്ണസംഖ്യ n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  തിരികെ n;
}

പൂർണ്ണസംഖ്യ curl_mprintf(സ്ഥിരം അക്ഷരം *format, ...)
{
  va_list args; /* argument pointer */
  പൂർണ്ണസംഖ്യ n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  തിരികെ n;
}

പൂർണ്ണസംഖ്യ curl_mvsprintf(അക്ഷരം *buffer, സ്ഥിരം അക്ഷരം *format, va_list args)
{
  പൂർണ്ണസംഖ്യ retcode = formatf(&buffer, storebuffer, storerun, ശൂന്യ, format, args);
  *buffer = 0; /* we null-terminate this */
  തിരികെ retcode;
}

പൂർണ്ണസംഖ്യ curl_mvprintf(സ്ഥിരം അക്ഷരം *format, va_list args)
{
  തിരികെ curl_mvfprintf(stdout, format, args);
}

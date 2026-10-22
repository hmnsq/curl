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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "curl_printf.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ଅଚଳ unsigned ଅକ୍ଷର Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ଅଚଳ unsigned ଅକ୍ଷର Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  କର {                                    \
    ଯଦି(stream((unsigned ଅକ୍ଷର)(x), userp)) \
      ଫେରନ୍ତୁ TRUE;                        \
    (*donep)++;                           \
  } ଯେତେବେଳେ(0)

/* Data type to read from the arglist */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
ଗଣନା {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_ବାମ       = 1 << 2,
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

ଗଣନା {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
ସଂରଚନା va_ନିବେଶ {
  FormatType type; /* FormatType */
  ସଂଘ {
    ଅଚଳ ଅକ୍ଷର *str;
    ଶୂନ୍ୟ *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    ଦ୍ୱିଗୁଣ dnum;
  } val;
};

/*
 * Describes an output segment.
 */
ସଂରଚନା outsegment {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;     /* width OR width parameter number */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା precision; /* precision OR precision parameter number */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ; /* input argument array index */
  ଅଚଳ ଅକ୍ଷର *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

ସଂରଚନା nsprintf {
  ଅକ୍ଷର *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

ସଂରଚନା asprintf {
  ସଂରଚନା dynbuf *b;
  ଅକ୍ଷର merr;
  size_t nstage;
  unsigned ଅକ୍ଷର stage[ASPRINTF_STAGE_SIZE];
};

ସଂରଚନା ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dollarstring(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର **end)
{
  curl_off_t ଅଙ୍କ;
  ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &ଅଙ୍କ, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !ଅଙ୍କ)
    ଫେରନ୍ତୁ -1;
  *end = p;
  ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned ଅକ୍ଷର)(1 << ((y) & 7)))

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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_flags(ଅଚଳ ଅକ୍ଷର **fmtp, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagsp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_dollar,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା *precp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *widthp)
{
  ଅଚଳ ଅକ୍ଷର *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା precision = 0;

  /* Handle the flags */
  କର {
    ବାଛ(*fmt++) {
    ସ୍ଥିତି ' ':
      flags |= FLAGS_SPACE;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି '+':
      flags |= FLAGS_SHOWSIGN;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି '-':
      flags |= FLAGS_ବାମ;
      flags &= ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)FLAGS_PAD_NIL;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି '#':
      flags |= FLAGS_ALT;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି '.':
      ଯଦି('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        ଯଦି(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          ଯଦି(precision < 0)
            /* illegal combo */
            ଫେରନ୍ତୁ PFMT_DOLLARPREC;
        }
        ଅନ୍ୟଥା
          /* get it from the next argument */
          precision = -1;
      }
      ଅନ୍ୟଥା {
        bool is_neg;
        curl_off_t ଅଙ୍କ;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        ଯଦି(is_neg)
          fmt++;
        ଯଦି(curlx_str_ସଂଖ୍ୟା(&fmt, &ଅଙ୍କ, INT_MAX))
          ଫେରନ୍ତୁ PFMT_PREC;
        precision = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ;
        ଯଦି(is_neg)
          precision = -precision;
      }
      ଯଦି((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        ଫେରନ୍ତୁ PFMT_PRECMIX;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 'h':
      flags |= FLAGS_SHORT;
      ଭାଙ୍ଗ;
#ifdef _WIN32
    ସ୍ଥିତି 'I':
      /* Non-ANSI integer extensions I32 I64 */
      ଯଦି((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      ଅନ୍ୟଥା ଯଦି((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      ଅନ୍ୟଥା {
#ଯଦି SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#ଅନ୍ୟଥା
        flags |= FLAGS_LONG;
#endif
      }
      ଭାଙ୍ଗ;
#endif /* _WIN32 */
    ସ୍ଥିତି 'l':
      ଯଦି(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      ଅନ୍ୟଥା
        flags |= FLAGS_LONG;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 'L':
      flags |= FLAGS_LONGDOUBLE;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 'q':
      flags |= FLAGS_LONGLONG;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#ଯଦି SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ଅନ୍ୟଥା
      flags |= FLAGS_LONG;
#endif
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 'O':
#ଯଦି SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ଅନ୍ୟଥା
      flags |= FLAGS_LONG;
#endif
      ଭାଙ୍ଗ;
    ସ୍ଥିତି '0':
      ଯଦି(!(flags & FLAGS_ବାମ))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    ସ୍ଥିତି '1':
    ସ୍ଥିତି '2':
    ସ୍ଥିତି '3':
    ସ୍ଥିତି '4':
    ସ୍ଥିତି '5':
    ସ୍ଥିତି '6':
    ସ୍ଥିତି '7':
    ସ୍ଥିତି '8':
    ସ୍ଥିତି '9': {
      curl_off_t ଅଙ୍କ;
      flags |= FLAGS_WIDTH;
      fmt--;
      ଯଦି(curlx_str_ସଂଖ୍ୟା(&fmt, &ଅଙ୍କ, INT_MAX))
        ଫେରନ୍ତୁ PFMT_WIDTH;
      width = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ;
      ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      ଯଦି(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        ଯଦି(width < 0)
          /* illegal combo */
          ଫେରନ୍ତୁ PFMT_DOLLARWIDTH;
      }
      ଅନ୍ୟଥା
        /* pick from the next argument */
        width = -1;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      loopit = FALSE;
      fmt--;
      ଭାଙ୍ଗ;
    } /* switch */
  } ଯେତେବେଳେ(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  ଫେରନ୍ତୁ PFMT_OK;
}

ସ୍ଥିର bool parse_conversion(ଅଚଳ ଅକ୍ଷର f, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagp,
                             FormatType *typep)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = *flagp;
  FormatType type;
  ବାଛ(f) {
  ସ୍ଥିତି 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 's':
    type = MTYPE_STRING;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'n':
    type = MTYPE_INTPTR;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'p':
    type = MTYPE_PTR;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'd':
  ସ୍ଥିତି 'i':
    ଯଦି(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    ଅନ୍ୟଥା ଯଦି(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    ଅନ୍ୟଥା
      type = MTYPE_INT;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'u':
    ଯଦି(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ଅନ୍ୟଥା ଯଦି(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ଅନ୍ୟଥା
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'o':
    ଯଦି(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ଅନ୍ୟଥା ଯଦି(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ଅନ୍ୟଥା
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'x':
    ଯଦି(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ଅନ୍ୟଥା ଯଦି(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ଅନ୍ୟଥା
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'X':
    ଯଦି(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ଅନ୍ୟଥା ଯଦି(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ଅନ୍ୟଥା
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* invalid instruction, disregard and continue */
    ଫେରନ୍ତୁ TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parsefmt(ଅଚଳ ଅକ୍ଷର *format,
                    ସଂରଚନା outsegment *out,
                    ସଂରଚନା va_ନିବେଶ *in,
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *opieces,
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ipieces, va_list arglist)
{
  ଅଚଳ ଅକ୍ଷର *fmt = format;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_ଅଙ୍କ = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_param = -1;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ocount = 0;
  unsigned ଅକ୍ଷର usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  ସଂରଚନା outsegment *optr;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_dollar = DOLLAR_UNKNOWN;
  ଅଚଳ ଅକ୍ଷର *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, ଆକାର(usedinput));

  ଯେତେବେଳେ(*fmt) {
    ଯଦି(*fmt == '%') {
      ସଂରଚନା va_ନିବେଶ *iptr;
      FormatType type;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = 0;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 0;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା precision = 0;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା param = -1;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      ଯଦି(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        ଯଦି(outlen) {
          optr = &out[ocount++];
          ଯଦି(ocount > MAX_SEGMENTS)
            ଫେରନ୍ତୁ PFMT_MANYSEGS;
          optr->ନିବେଶ = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        ଜାରି; /* while */
      }

      ଯଦି(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        ଯଦି(param < 0) {
          ଯଦି(use_dollar == DOLLAR_USE)
            /* illegal combo */
            ଫେରନ୍ତୁ PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        ଅନ୍ୟଥା
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      ଯଦି(rc)
        ଫେରନ୍ତୁ rc;

      ଯଦି(parse_conversion(*fmt, &flags, &type))
        ଜାରି;

      ଯଦି(flags & FLAGS_WIDTHPARAM) {
        ଯଦି(width < 0)
          width = param_ଅଙ୍କ++;
        ଅନ୍ୟଥା {
          /* if this identifies a parameter already used, this is illegal */
          ଯଦି(is_arg_used(usedinput, width))
            ଫେରନ୍ତୁ PFMT_WIDTHARG;
        }
        ଯଦି(width >= MAX_PARAMETERS)
          ଫେରନ୍ତୁ PFMT_MANYARGS;
        ଯଦି(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      ଯଦି(flags & FLAGS_PRECPARAM) {
        ଯଦି(precision < 0)
          precision = param_ଅଙ୍କ++;
        ଅନ୍ୟଥା {
          /* if this identifies a parameter already used, this is illegal */
          ଯଦି(is_arg_used(usedinput, precision))
            ଫେରନ୍ତୁ PFMT_PRECARG;
        }
        ଯଦି(precision >= MAX_PARAMETERS)
          ଫେରନ୍ତୁ PFMT_MANYARGS;
        ଯଦି(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      ଯଦି(param < 0)
        param = param_ଅଙ୍କ++;
      ଯଦି(param >= MAX_PARAMETERS)
        ଫେରନ୍ତୁ PFMT_MANYARGS;
      ଯଦି(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      ଯଦି(ocount > MAX_SEGMENTS)
        ଫେରନ୍ତୁ PFMT_MANYSEGS;
      optr->ନିବେଶ = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    ଅନ୍ୟଥା
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  ଯଦି(outlen) {
    optr = &out[ocount++];
    ଯଦି(ocount > MAX_SEGMENTS)
      ଫେରନ୍ତୁ PFMT_MANYSEGS;
    optr->ନିବେଶ = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  ପାଇଁ(i = 0; i < max_param + 1; i++) {
    ସଂରଚନା va_ନିବେଶ *iptr = &in[i];
    ଯଦି(!is_arg_used(usedinput, i))
      /* bad input */
      ଫେରନ୍ତୁ PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    ବାଛ(iptr->type) {
    ସ୍ଥିତି MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ଅଚଳ ଅକ୍ଷର *);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_INTPTR:
    ସ୍ଥିତି MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, ଶୂନ୍ୟ *);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_INT:
    ସ୍ଥିତି MTYPE_WIDTH:
    ସ୍ଥିତି MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, ଦ୍ୱିଗୁଣ);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_LONGDOUBLE:
      iptr->val.dnum = (ଦ୍ୱିଗୁଣ)va_arg(arglist, long ଦ୍ୱିଗୁଣ);
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      DEBUGASSERT(ଖାଲି); /* unexpected */
      ଭାଙ୍ଗ;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  ଫେରନ୍ତୁ PFMT_OK;
}

ସଂରଚନା mproperty {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;            /* Width of a field. */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା prec;             /* Precision of a field. */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା size_t (*mp_streamn)(ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t len, ଶୂନ୍ୟ *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*mp_flush)(ଶୂନ୍ୟ *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
ସ୍ଥିର bool stream_run(ଶୂନ୍ୟ *userp,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*stream)(unsigned ଅକ୍ଷର, ଶୂନ୍ୟ *),
                       mp_streamn streamn,
                       ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *donep)
{
  ଯଦି(!len)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n;
    ଫେରନ୍ତୁ n != len;
  }
  ଯେତେବେଳେ(len--) {
    ଯଦି(stream(*buf++, userp))
      ଫେରନ୍ତୁ TRUE;
    (*donep)++;
  }
  ଫେରନ୍ତୁ FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର pad_spaces[] = "                ";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
ସ୍ଥିର bool stream_pad(ଶୂନ୍ୟ *userp,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*stream)(unsigned ଅକ୍ଷର, ଶୂନ୍ୟ *),
                       mp_streamn streamn,
                       unsigned ଅକ୍ଷର pad, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *donep)
{
  ଅଚଳ unsigned ଅକ୍ଷର *src;
  ଯଦି(ଅଙ୍କ <= 0)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(pad == '0')
    src = (ଅଚଳ unsigned ଅକ୍ଷର *)pad_zeros;
  ଅନ୍ୟଥା
    src = (ଅଚଳ unsigned ଅକ୍ଷର *)pad_spaces;
  ଯେତେବେଳେ(ଅଙ୍କ > PAD_RUN) {
    ଯଦି(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      ଫେରନ୍ତୁ TRUE;
    ଅଙ୍କ -= PAD_RUN;
  }
  ଫେରନ୍ତୁ stream_run(userp, stream, streamn, src, (size_t)ଅଙ୍କ, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
ସ୍ଥିର bool stream_zrun(ଶୂନ୍ୟ *userp,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*stream)(unsigned ଅକ୍ଷର, ଶୂନ୍ୟ *),
                        mp_streamn streamn,
                        ଅଚଳ ଅକ୍ଷର *str, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *donep)
{
  size_t n = 0;
  ଯେତେବେଳେ(n < len && str[n])
    n++;
  ଫେରନ୍ତୁ stream_run(userp, stream, streamn,
                    (ଅଚଳ unsigned ଅକ୍ଷର *)str, n, donep);
}

ସ୍ଥିର bool out_double(ଶୂନ୍ୟ *userp,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*stream)(unsigned ଅକ୍ଷର, ଶୂନ୍ୟ *),
                       mp_streamn streamn,
                       ସଂରଚନା mproperty *p,
                       ଦ୍ୱିଗୁଣ dnum,
                       ଅକ୍ଷର *work, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *donep)
{
  ଅକ୍ଷର fmt[32] = "%";
  ଅକ୍ଷର *fptr = &fmt[1];
  size_t ବାମ = ଆକାର(fmt) - strlen(fmt);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = p->flags;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = p->width;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା prec = p->prec;

  ଯଦି(flags & FLAGS_ବାମ)
    *fptr++ = '-';
  ଯଦି(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  ଯଦି(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  ଯଦି(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  ଯଦି(width >= 0) {
    size_t dlen;
    ଯଦି(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, ବାମ, "%d", width);
    fptr += dlen;
    ବାମ -= dlen;
  }
  ଯଦି(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxprec = BUFFSIZE - 1;
    ଦ୍ୱିଗୁଣ val = dnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    ଯଦି(prec > maxprec)
      prec = maxprec - 1;
    ଯଦି(width > 0 && prec <= width)
      maxprec -= width;
    ଯେତେବେଳେ(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    ଯଦି(prec > maxprec)
      prec = maxprec - 1;
    ଯଦି(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, ବାମ, ".%d", prec);
    fptr += len;
  }
  ଯଦି(flags & FLAGS_LONG)
    *fptr++ = 'l';

  ଯଦି(flags & FLAGS_FLOATE)
    *fptr++ = (ଅକ୍ଷର)((flags & FLAGS_UPPER) ? 'E' : 'e');
  ଅନ୍ୟଥା ଯଦି(flags & FLAGS_FLOATG)
    *fptr++ = (ଅକ୍ଷର)((flags & FLAGS_UPPER) ? 'G' : 'g');
  ଅନ୍ୟଥା
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#ଅନ୍ୟଥା
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic କାଢ଼
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  ଫେରନ୍ତୁ stream_run(userp, stream, streamn,
                    (ଅଚଳ unsigned ଅକ୍ଷର *)work, strlen(work), donep);
}

ସ୍ଥିର bool out_ସଂଖ୍ୟା(ଶୂନ୍ୟ *userp,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*stream)(unsigned ଅକ୍ଷର, ଶୂନ୍ୟ *),
                       mp_streamn streamn,
                       ସଂରଚନା mproperty *p,
                       uint64_t ଅଙ୍କ,
                       int64_t nums,
                       ଅକ୍ଷର *work, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *donep)
{
  ଅଚଳ unsigned ଅକ୍ଷର *digits = Curl_ldigits;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = p->flags;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = p->width;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  ଅକ୍ଷର *workend = &work[BUFFSIZE - 2];
  ଅକ୍ଷର *w;

  ଯଦି(flags & FLAGS_CHAR) {
    /* Character. */
    ଯଦି(!(flags & FLAGS_ବାମ)) {
      ଯଦି(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ଫେରନ୍ତୁ TRUE;
      width = 1;
    }
    OUTCHAR((ଅକ୍ଷର)ଅଙ୍କ);
    ଯଦି(flags & FLAGS_ବାମ) {
      ଯଦି(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
  }
  ଯଦି(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  ଅନ୍ୟଥା ଯଦି(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  ଅନ୍ୟଥା ଯଦି(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  ଅନ୍ୟଥା {
    /* Decimal integer. */
    is_neg = (nums < 0);
    ଯଦି(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_ଅଙ୍କ; /* Used to convert negative in positive. */
      signed_ଅଙ୍କ = nums + (int64_t)1;
      signed_ଅଙ୍କ = -signed_ଅଙ୍କ;
      ଅଙ୍କ = (uint64_t)signed_ଅଙ୍କ;
      ଅଙ୍କ += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  ଯଦି(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  ବାଛ(base) {
  ସ୍ଥିତି 10:
    ଯେତେବେଳେ(ଅଙ୍କ > 0) {
      *w-- = (ଅକ୍ଷର)('0' + (ଅଙ୍କ % 10));
      ଅଙ୍କ /= 10;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯେତେବେଳେ(ଅଙ୍କ > 0) {
      *w-- = digits[ଅଙ୍କ % base];
      ଅଙ୍କ /= base;
    }
    ଭାଙ୍ଗ;
  }
  width -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(workend - w);
  prec -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(workend - w);

  ଯଦି(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  ଯଦି(prec > 0) {
    width -= prec;
    ଯେତେବେଳେ(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  ଯଦି(is_alt && base == 16)
    width -= 2;

  ଯଦି(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  ଯଦି(!(flags & FLAGS_ବାମ) && !(flags & FLAGS_PAD_NIL)) {
    ଯଦି(stream_pad(userp, stream, streamn, ' ', width, donep))
      ଫେରନ୍ତୁ TRUE;
    width = 0;
  }

  ଯଦି(is_neg)
    OUTCHAR('-');
  ଅନ୍ୟଥା ଯଦି(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  ଅନ୍ୟଥା ଯଦି(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  ଯଦି(is_alt && base == 16) {
    OUTCHAR('0');
    ଯଦି(flags & FLAGS_UPPER)
      OUTCHAR('X');
    ଅନ୍ୟଥା
      OUTCHAR('x');
  }

  ଯଦି(!(flags & FLAGS_ବାମ) && (flags & FLAGS_PAD_NIL)) {
    ଯଦି(stream_pad(userp, stream, streamn, '0', width, donep))
      ଫେରନ୍ତୁ TRUE;
    width = 0;
  }

  /* Write the number. */
  ଯଦି(stream_run(userp, stream, streamn, (ଅଚଳ unsigned ଅକ୍ଷର *)w + 1,
                (size_t)(workend - w), donep))
    ଫେରନ୍ତୁ TRUE;

  ଯଦି(flags & FLAGS_ବାମ) {
    ଯଦି(stream_pad(userp, stream, streamn, ' ', width, donep))
      ଫେରନ୍ତୁ TRUE;
  }

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର nilstr[] = "(nil)";

ସ୍ଥିର bool out_string(ଶୂନ୍ୟ *userp,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*stream)(unsigned ଅକ୍ଷର, ଶୂନ୍ୟ *),
                       mp_streamn streamn,
                       ସଂରଚନା mproperty *p,
                       ଅଚଳ ଅକ୍ଷର *str,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା *donep)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = p->flags;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = p->width;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା prec = p->prec;
  size_t len;

  ଯଦି(!str) {
    /* Write null string if there is space. */
    ଯଦି(prec == -1 || prec >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)FLAGS_ALT;
    }
    ଅନ୍ୟଥା {
      str = "";
      len = 0;
    }
  }
  ଅନ୍ୟଥା ଯଦି(prec != -1)
    len = (size_t)prec;
  ଅନ୍ୟଥା ଯଦି(*str == '\0')
    len = 0;
  ଅନ୍ୟଥା
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;

  ଯଦି(flags & FLAGS_ALT)
    OUTCHAR('"');

  ଯଦି(!(flags & FLAGS_ବାମ)) {
    ଯଦି(stream_pad(userp, stream, streamn, ' ', width, donep))
      ଫେରନ୍ତୁ TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  ଯଦି(prec == -1) {
    ଯଦି(stream_run(userp, stream, streamn,
                  (ଅଚଳ unsigned ଅକ୍ଷର *)str, len, donep))
      ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(stream_zrun(userp, stream, streamn, str, len, donep))
    ଫେରନ୍ତୁ TRUE;
  ଯଦି(flags & FLAGS_ବାମ) {
    ଯଦି(stream_pad(userp, stream, streamn, ' ', width, donep))
      ଫେରନ୍ତୁ TRUE;
  }

  ଯଦି(flags & FLAGS_ALT)
    OUTCHAR('"');

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool out_pointer(ଶୂନ୍ୟ *userp,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*stream)(unsigned ଅକ୍ଷର, ଶୂନ୍ୟ *),
                        mp_streamn streamn,
                        ସଂରଚନା mproperty *p,
                        ଅଚଳ ଅକ୍ଷର *ptr,
                        ଅକ୍ଷର *work,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା *donep)
{
  /* Generic pointer. */
  ଯଦି(ptr) {
    size_t ଅଙ୍କ = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    ଯଦି(out_ସଂଖ୍ୟା(userp, stream, streamn, p, ଅଙ୍କ, 0, work, donep))
      ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା {
    /* Write "(nil)" for a nil pointer. */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = p->width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = p->flags;

    width -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURL_CSTRLEN(nilstr);
    ଯଦି(flags & FLAGS_ବାମ) {
      ଯଦି(stream_pad(userp, stream, streamn, ' ', width, donep))
        ଫେରନ୍ତୁ TRUE;
      width = 0;
    }
    ଯଦି(stream_run(userp, stream, streamn, (ଅଚଳ unsigned ଅକ୍ଷର *)nilstr,
                  ଆକାର(nilstr) - 1, donep))
      ଫେରନ୍ତୁ TRUE;
    ଯଦି(!(flags & FLAGS_ବାମ)) {
      ଯଦି(stream_pad(userp, stream, streamn, ' ', width, donep))
        ଫେରନ୍ତୁ TRUE;
    }
  }
  ଫେରନ୍ତୁ FALSE;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା formatf(ଶୂନ୍ୟ *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*stream)(unsigned ଅକ୍ଷର, ଶୂନ୍ୟ *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ଅଚଳ ଅକ୍ଷର *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା done = 0;   /* number of characters written */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ocount = 0; /* number of output segments */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା icount = 0; /* number of input arguments */

  ସଂରଚନା outsegment output[MAX_SEGMENTS];
  ସଂରଚନା va_ନିବେଶ ନିବେଶ[MAX_PARAMETERS];
  ଅକ୍ଷର work[BUFFSIZE + 2];

  /* Parse the format string */
  ଯଦି(parsefmt(format, output, ନିବେଶ, &ocount, &icount, ap_save))
    ଫେରନ୍ତୁ 0;

  ପାଇଁ(i = 0; i < ocount; i++) {
    ସଂରଚନା outsegment *optr = &output[i];
    ସଂରଚନା va_ନିବେଶ *iptr = &ନିବେଶ[optr->ନିବେଶ];
    ସଂରଚନା mproperty p;
    size_t outlen = optr->outlen;

    ଯଦି(outlen) {
      ଯଦି(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        ଫେରନ୍ତୁ done;
      ଯଦି(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        ଜାରି;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    ଯଦି(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ନିବେଶ[optr->width].val.nums;
      ଯଦି(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        ଯଦି(p.width == INT_MIN)
          p.width = INT_MAX;
        ଅନ୍ୟଥା
          p.width = -p.width;
        p.flags |= FLAGS_ବାମ;
        p.flags &= ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)FLAGS_PAD_NIL;
      }
    }
    ଅନ୍ୟଥା
      p.width = optr->width;

    /* pick up the specified precision */
    ଯଦି(p.flags & FLAGS_PRECPARAM) {
      p.prec = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ନିବେଶ[optr->precision].val.nums;
      ଯଦି(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    ଅନ୍ୟଥା ଯଦି(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    ଅନ୍ୟଥା
      p.prec = -1;

    ବାଛ(iptr->type) {
    ସ୍ଥିତି MTYPE_INTU:
    ସ୍ଥିତି MTYPE_LONGU:
    ସ୍ଥିତି MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      ଯଦି(out_ସଂଖ୍ୟା(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        ଫେରନ୍ତୁ done;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_INT:
    ସ୍ଥିତି MTYPE_LONG:
    ସ୍ଥିତି MTYPE_LONGLONG:
      ଯଦି(out_ସଂଖ୍ୟା(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        ଫେରନ୍ତୁ done;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_STRING:
      ଯଦି(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        ଫେରନ୍ତୁ done;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_PTR:
      ଯଦି(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        ଫେରନ୍ତୁ done;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_DOUBLE:
    ସ୍ଥିତି MTYPE_LONGDOUBLE:
      ଯଦି(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        ଫେରନ୍ତୁ done;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      ଯଦି(flush && flush(userp))
        ଫେରନ୍ତୁ done;
      /* Answer the count of characters written. */
      ଯଦି(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      ଅନ୍ୟଥା ଯଦି(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      ଅନ୍ୟଥା ଯଦି(!(p.flags & FLAGS_SHORT))
        *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)iptr->val.ptr = done;
      ଅନ୍ୟଥା
        *(short *)iptr->val.ptr = (short)done;
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ done;
}

/* fputc() look-alike */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା addbyter(unsigned ଅକ୍ଷର outc, ଶୂନ୍ୟ *f)
{
  ସଂରଚନା nsprintf *infop = f;
  ଯଦି(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (ଅକ୍ଷର)outc; /* store */
    infop->length++; /* we are now one byte larger */
    ଫେରନ୍ତୁ 0;     /* fputc() returns like this on success */
  }
  ଫେରନ୍ତୁ 1;
}

/* block variant of addbyter */
ସ୍ଥିର size_t addrun(ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t len, ଶୂନ୍ୟ *f)
{
  ସଂରଚନା nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  ଯଦି(len > fit)
    len = fit;
  ଯଦି(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  ଫେରନ୍ତୁ len;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_mvsnprintf(ଅକ୍ଷର *buffer, size_t maxlength, ଅଚଳ ଅକ୍ଷର *format,
                    va_list args)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା retcode;
  ସଂରଚନା nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, ଖାଲି, format, args);
  ଯଦି(info.max) {
    /* we null-terminate this */
    ଯଦି(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    ଅନ୍ୟଥା
      info.buffer[0] = 0;
  }
  ଫେରନ୍ତୁ retcode;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_msnprintf(ଅକ୍ଷର *buffer, size_t maxlength, ଅଚଳ ଅକ୍ଷର *format, ...)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  ଫେରନ୍ତୁ retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା alloc_flush(ଶୂନ୍ୟ *f)
{
  ସଂରଚନା asprintf *infop = f;
  ଯଦି(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    ଯଦି(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      ଫେରନ୍ତୁ 1; /* fail */
    }
  }
  ଫେରନ୍ତୁ 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା alloc_addn(ସଂରଚନା asprintf *infop, ଅଚଳ unsigned ଅକ୍ଷର *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  ଯଦି(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    ଫେରନ୍ତୁ 1; /* fail */
  }
  ଫେରନ୍ତୁ 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
ସ୍ଥିର bool alloc_fits(ସଂରଚନା asprintf *infop, size_t more)
{
  ଫେରନ୍ତୁ (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା alloc_addbyter(unsigned ଅକ୍ଷର outc, ଶୂନ୍ୟ *f)
{
  ସଂରଚନା asprintf *infop = f;
  ଯଦି(infop->nstage == ଆକାର(infop->stage) && alloc_flush(infop))
    ଫେରନ୍ତୁ 1; /* fail */
  ଯଦି(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    ଯଦି(alloc_flush(infop))
      ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  ଫେରନ୍ତୁ 0;
}

/* block variant of alloc_addbyter */
ସ୍ଥିର size_t alloc_addrun(ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t len, ଶୂନ୍ୟ *f)
{
  ସଂରଚନା asprintf *infop = f;
  size_t accepted = 0;
  ଯେତେବେଳେ(accepted < len) {
    size_t room = ଆକାର(infop->stage) - infop->nstage;
    size_t n;
    ଯଦି(!room) {
      ଯଦି(alloc_flush(infop))
        ଫେରନ୍ତୁ accepted; /* accepted < len aborts formatting */
      room = ଆକାର(infop->stage);
    }
    n = len - accepted;
    ଯଦି(n > room)
      n = room;
    ଯଦି(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      ଯଦି(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      ଯଦି(alloc_flush(infop))
        ଫେରନ୍ତୁ accepted;
      ଯଦି(alloc_addn(infop, buf + accepted, 1))
        ଫେରନ୍ତୁ accepted; /* accepted < len aborts formatting */
      accepted++;
      ଜାରି;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  ଫେରନ୍ତୁ accepted;
}

/* appends the formatted string, returns MERR error code */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_dyn_vprintf(ସଂରଚନା dynbuf *dyn, ଅଚଳ ଅକ୍ଷର *format, va_list args)
{
  ସଂରଚନା asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (ଶୂନ୍ୟ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ଯଦି(!info.merr)
    (ଶୂନ୍ୟ)alloc_flush(&info);
  ଯଦି(info.merr) {
    curlx_dyn_free(info.b);
    ଫେରନ୍ତୁ info.merr;
  }
  ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର *curl_mvaprintf(ଅଚଳ ଅକ୍ଷର *format, va_list args)
{
  ସଂରଚନା asprintf info;
  ସଂରଚନା dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (ଶୂନ୍ୟ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  ଯଦି(!info.merr)
    (ଶୂନ୍ୟ)alloc_flush(&info);
  ଯଦି(info.merr) {
    curlx_dyn_free(info.b);
    ଫେରନ୍ତୁ ଖାଲି;
  }
  ଯଦି(curlx_dyn_len(info.b))
    ଫେରନ୍ତୁ curlx_dyn_ptr(info.b);
  ଫେରନ୍ତୁ curlx_strdup("");
}

ଅକ୍ଷର *curl_maprintf(ଅଚଳ ଅକ୍ଷର *format, ...)
{
  va_list args;
  ଅକ୍ଷର *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  ଫେରନ୍ତୁ s;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା storebuffer(unsigned ଅକ୍ଷର outc, ଶୂନ୍ୟ *f)
{
  ଅକ୍ଷର **buffer = f;
  **buffer = (ଅକ୍ଷର)outc;
  (*buffer)++;
  ଫେରନ୍ତୁ 0;
}

/* block variant of storebuffer */
ସ୍ଥିର size_t storerun(ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t len, ଶୂନ୍ୟ *f)
{
  ଅକ୍ଷର **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  ଫେରନ୍ତୁ len;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_msprintf(ଅକ୍ଷର *buffer, ଅଚଳ ଅକ୍ଷର *format, ...)
{
  va_list args; /* argument pointer */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, ଖାଲି, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  ଫେରନ୍ତୁ retcode;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fputc_wrapper(unsigned ଅକ୍ଷର outc, ଶୂନ୍ୟ *f)
{
  ସଂରଚନା ioprintf *i = f;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା out = outc;
  FILE *s = i->fd;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = fputc(out, s);
  i->error = (rc == EOF);
  ଫେରନ୍ତୁ (rc == EOF);
}

/* block variant of fputc_wrapper */
ସ୍ଥିର size_t fwrite_wrapper(ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t len, ଶୂନ୍ୟ *f)
{
  ସଂରଚନା ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_mvfprintf(FILE *fd, ଅଚଳ ଅକ୍ଷର *format, va_list args)
{
  ସଂରଚନା ioprintf info;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, ଖାଲି, format, args);
  ଯଦି(info.error)
    ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_mfprintf(FILE *fd, ଅଚଳ ଅକ୍ଷର *format, ...)
{
  va_list args; /* argument pointer */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_mprintf(ଅଚଳ ଅକ୍ଷର *format, ...)
{
  va_list args; /* argument pointer */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_mvsprintf(ଅକ୍ଷର *buffer, ଅଚଳ ଅକ୍ଷର *format, va_list args)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା retcode = formatf(&buffer, storebuffer, storerun, ଖାଲି, format, args);
  *buffer = 0; /* we null-terminate this */
  ଫେରନ୍ତୁ retcode;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_mvprintf(ଅଚଳ ଅକ୍ଷର *format, va_list args)
{
  ଫେରନ୍ତୁ curl_mvfprintf(stdout, format, args);
}

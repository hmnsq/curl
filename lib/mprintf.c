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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "curl_printf.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
மாறிலி unsigned எழுத்து Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
மாறிலி unsigned எழுத்து Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  செய் {                                    \
    எனில்(stream((unsigned எழுத்து)(x), userp)) \
      திரும்பு TRUE;                        \
    (*donep)++;                           \
  } போது(0)

/* Data type to read from the arglist */
வகைவரை எண்ணிக்கை {
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
எண்ணிக்கை {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_இடது       = 1 << 2,
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

எண்ணிக்கை {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
கட்டமைப்பு va_உள்ளீடு {
  FormatType type; /* FormatType */
  ஒன்றிப்பு {
    மாறிலி எழுத்து *str;
    வெறுமை *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    இரட்டை dnum;
  } val;
};

/*
 * Describes an output segment.
 */
கட்டமைப்பு outsegment {
  முழு width;     /* width OR width parameter number */
  முழு precision; /* precision OR precision parameter number */
  unsigned முழு flags;
  unsigned முழு உள்ளீடு; /* input argument array index */
  மாறிலி எழுத்து *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

கட்டமைப்பு nsprintf {
  எழுத்து *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

கட்டமைப்பு asprintf {
  கட்டமைப்பு dynbuf *b;
  எழுத்து merr;
  size_t nstage;
  unsigned எழுத்து stage[ASPRINTF_STAGE_SIZE];
};

கட்டமைப்பு ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
நிலையான முழு dollarstring(மாறிலி எழுத்து *p, மாறிலி எழுத்து **end)
{
  curl_off_t இலக்கம்;
  எனில்(curlx_str_எண்(&p, &இலக்கம், MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !இலக்கம்)
    திரும்பு -1;
  *end = p;
  திரும்பு (முழு)இலக்கம் - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned எழுத்து)(1 << ((y) & 7)))

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

நிலையான முழு parse_flags(மாறிலி எழுத்து **fmtp, unsigned முழு *flagsp, முழு use_dollar,
                       முழு *precp, முழு *widthp)
{
  மாறிலி எழுத்து *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned முழு flags = 0;
  முழு width = 0;
  முழு precision = 0;

  /* Handle the flags */
  செய் {
    தேர்வு(*fmt++) {
    நிலை ' ':
      flags |= FLAGS_SPACE;
      நிறுத்து;
    நிலை '+':
      flags |= FLAGS_SHOWSIGN;
      நிறுத்து;
    நிலை '-':
      flags |= FLAGS_இடது;
      flags &= ~(unsigned முழு)FLAGS_PAD_NIL;
      நிறுத்து;
    நிலை '#':
      flags |= FLAGS_ALT;
      நிறுத்து;
    நிலை '.':
      எனில்('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        எனில்(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          எனில்(precision < 0)
            /* illegal combo */
            திரும்பு PFMT_DOLLARPREC;
        }
        இல்லையெனில்
          /* get it from the next argument */
          precision = -1;
      }
      இல்லையெனில் {
        bool is_neg;
        curl_off_t இலக்கம்;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        எனில்(is_neg)
          fmt++;
        எனில்(curlx_str_எண்(&fmt, &இலக்கம், INT_MAX))
          திரும்பு PFMT_PREC;
        precision = (முழு)இலக்கம்;
        எனில்(is_neg)
          precision = -precision;
      }
      எனில்((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        திரும்பு PFMT_PRECMIX;
      நிறுத்து;
    நிலை 'h':
      flags |= FLAGS_SHORT;
      நிறுத்து;
#ifdef _WIN32
    நிலை 'I':
      /* Non-ANSI integer extensions I32 I64 */
      எனில்((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      இல்லையெனில் எனில்((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      இல்லையெனில் {
#எனில் SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#இல்லையெனில்
        flags |= FLAGS_LONG;
#endif
      }
      நிறுத்து;
#endif /* _WIN32 */
    நிலை 'l':
      எனில்(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      இல்லையெனில்
        flags |= FLAGS_LONG;
      நிறுத்து;
    நிலை 'L':
      flags |= FLAGS_LONGDOUBLE;
      நிறுத்து;
    நிலை 'q':
      flags |= FLAGS_LONGLONG;
      நிறுத்து;
    நிலை 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#எனில் SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#இல்லையெனில்
      flags |= FLAGS_LONG;
#endif
      நிறுத்து;
    நிலை 'O':
#எனில் SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#இல்லையெனில்
      flags |= FLAGS_LONG;
#endif
      நிறுத்து;
    நிலை '0':
      எனில்(!(flags & FLAGS_இடது))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    நிலை '1':
    நிலை '2':
    நிலை '3':
    நிலை '4':
    நிலை '5':
    நிலை '6':
    நிலை '7':
    நிலை '8':
    நிலை '9': {
      curl_off_t இலக்கம்;
      flags |= FLAGS_WIDTH;
      fmt--;
      எனில்(curlx_str_எண்(&fmt, &இலக்கம், INT_MAX))
        திரும்பு PFMT_WIDTH;
      width = (முழு)இலக்கம்;
      நிறுத்து;
    }
    நிலை '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      எனில்(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        எனில்(width < 0)
          /* illegal combo */
          திரும்பு PFMT_DOLLARWIDTH;
      }
      இல்லையெனில்
        /* pick from the next argument */
        width = -1;
      நிறுத்து;
    இயல்பு:
      loopit = FALSE;
      fmt--;
      நிறுத்து;
    } /* switch */
  } போது(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  திரும்பு PFMT_OK;
}

நிலையான bool parse_conversion(மாறிலி எழுத்து f, unsigned முழு *flagp,
                             FormatType *typep)
{
  unsigned முழு flags = *flagp;
  FormatType type;
  தேர்வு(f) {
  நிலை 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    நிறுத்து;
  நிலை 's':
    type = MTYPE_STRING;
    நிறுத்து;
  நிலை 'n':
    type = MTYPE_INTPTR;
    நிறுத்து;
  நிலை 'p':
    type = MTYPE_PTR;
    நிறுத்து;
  நிலை 'd':
  நிலை 'i':
    எனில்(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    இல்லையெனில் எனில்(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    இல்லையெனில்
      type = MTYPE_INT;
    நிறுத்து;
  நிலை 'u':
    எனில்(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    இல்லையெனில் எனில்(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    இல்லையெனில்
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    நிறுத்து;
  நிலை 'o':
    எனில்(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    இல்லையெனில் எனில்(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    இல்லையெனில்
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    நிறுத்து;
  நிலை 'x':
    எனில்(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    இல்லையெனில் எனில்(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    இல்லையெனில்
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    நிறுத்து;
  நிலை 'X':
    எனில்(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    இல்லையெனில் எனில்(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    இல்லையெனில்
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    நிறுத்து;
  நிலை 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    நிறுத்து;
  நிலை 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    நிறுத்து;
  நிலை 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    நிறுத்து;
  நிலை 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    நிறுத்து;
  நிலை 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    நிறுத்து;
  நிலை 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    நிறுத்து;
  நிலை 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    நிறுத்து;
  இயல்பு:
    /* invalid instruction, disregard and continue */
    திரும்பு TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  திரும்பு FALSE;
}

நிலையான முழு parsefmt(மாறிலி எழுத்து *format,
                    கட்டமைப்பு outsegment *out,
                    கட்டமைப்பு va_உள்ளீடு *in,
                    முழு *opieces,
                    முழு *ipieces, va_list arglist)
{
  மாறிலி எழுத்து *fmt = format;
  முழு param_இலக்கம் = 0;
  முழு max_param = -1;
  முழு i;
  முழு ocount = 0;
  unsigned எழுத்து usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  கட்டமைப்பு outsegment *optr;
  முழு use_dollar = DOLLAR_UNKNOWN;
  மாறிலி எழுத்து *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, அளவு(usedinput));

  போது(*fmt) {
    எனில்(*fmt == '%') {
      கட்டமைப்பு va_உள்ளீடு *iptr;
      FormatType type;
      unsigned முழு flags = 0;
      முழு width = 0;
      முழு precision = 0;
      முழு param = -1;
      முழு rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      எனில்(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        எனில்(outlen) {
          optr = &out[ocount++];
          எனில்(ocount > MAX_SEGMENTS)
            திரும்பு PFMT_MANYSEGS;
          optr->உள்ளீடு = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        தொடர்; /* while */
      }

      எனில்(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        எனில்(param < 0) {
          எனில்(use_dollar == DOLLAR_USE)
            /* illegal combo */
            திரும்பு PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        இல்லையெனில்
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      எனில்(rc)
        திரும்பு rc;

      எனில்(parse_conversion(*fmt, &flags, &type))
        தொடர்;

      எனில்(flags & FLAGS_WIDTHPARAM) {
        எனில்(width < 0)
          width = param_இலக்கம்++;
        இல்லையெனில் {
          /* if this identifies a parameter already used, this is illegal */
          எனில்(is_arg_used(usedinput, width))
            திரும்பு PFMT_WIDTHARG;
        }
        எனில்(width >= MAX_PARAMETERS)
          திரும்பு PFMT_MANYARGS;
        எனில்(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      எனில்(flags & FLAGS_PRECPARAM) {
        எனில்(precision < 0)
          precision = param_இலக்கம்++;
        இல்லையெனில் {
          /* if this identifies a parameter already used, this is illegal */
          எனில்(is_arg_used(usedinput, precision))
            திரும்பு PFMT_PRECARG;
        }
        எனில்(precision >= MAX_PARAMETERS)
          திரும்பு PFMT_MANYARGS;
        எனில்(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      எனில்(param < 0)
        param = param_இலக்கம்++;
      எனில்(param >= MAX_PARAMETERS)
        திரும்பு PFMT_MANYARGS;
      எனில்(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      எனில்(ocount > MAX_SEGMENTS)
        திரும்பு PFMT_MANYSEGS;
      optr->உள்ளீடு = (unsigned முழு)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    இல்லையெனில்
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  எனில்(outlen) {
    optr = &out[ocount++];
    எனில்(ocount > MAX_SEGMENTS)
      திரும்பு PFMT_MANYSEGS;
    optr->உள்ளீடு = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  க்கு(i = 0; i < max_param + 1; i++) {
    கட்டமைப்பு va_உள்ளீடு *iptr = &in[i];
    எனில்(!is_arg_used(usedinput, i))
      /* bad input */
      திரும்பு PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    தேர்வு(iptr->type) {
    நிலை MTYPE_STRING:
      iptr->val.str = va_arg(arglist, மாறிலி எழுத்து *);
      நிறுத்து;

    நிலை MTYPE_INTPTR:
    நிலை MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, வெறுமை *);
      நிறுத்து;

    நிலை MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      நிறுத்து;

    நிலை MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      நிறுத்து;

    நிலை MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      நிறுத்து;

    நிலை MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      நிறுத்து;

    நிலை MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned முழு);
      நிறுத்து;

    நிலை MTYPE_INT:
    நிலை MTYPE_WIDTH:
    நிலை MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, முழு);
      நிறுத்து;

    நிலை MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, இரட்டை);
      நிறுத்து;

    நிலை MTYPE_LONGDOUBLE:
      iptr->val.dnum = (இரட்டை)va_arg(arglist, long இரட்டை);
      நிறுத்து;

    இயல்பு:
      DEBUGASSERT(வெற்று); /* unexpected */
      நிறுத்து;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  திரும்பு PFMT_OK;
}

கட்டமைப்பு mproperty {
  முழு width;            /* Width of a field. */
  முழு prec;             /* Precision of a field. */
  unsigned முழு flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
வகைவரை size_t (*mp_streamn)(மாறிலி unsigned எழுத்து *buf, size_t len, வெறுமை *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
வகைவரை முழு (*mp_flush)(வெறுமை *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
நிலையான bool stream_run(வெறுமை *userp,
                       முழு (*stream)(unsigned எழுத்து, வெறுமை *),
                       mp_streamn streamn,
                       மாறிலி unsigned எழுத்து *buf, size_t len, முழு *donep)
{
  எனில்(!len)
    திரும்பு FALSE;
  எனில்(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (முழு)n;
    திரும்பு n != len;
  }
  போது(len--) {
    எனில்(stream(*buf++, userp))
      திரும்பு TRUE;
    (*donep)++;
  }
  திரும்பு FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
நிலையான மாறிலி எழுத்து pad_spaces[] = "                ";
நிலையான மாறிலி எழுத்து pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
நிலையான bool stream_pad(வெறுமை *userp,
                       முழு (*stream)(unsigned எழுத்து, வெறுமை *),
                       mp_streamn streamn,
                       unsigned எழுத்து pad, முழு இலக்கம், முழு *donep)
{
  மாறிலி unsigned எழுத்து *src;
  எனில்(இலக்கம் <= 0)
    திரும்பு FALSE;
  எனில்(pad == '0')
    src = (மாறிலி unsigned எழுத்து *)pad_zeros;
  இல்லையெனில்
    src = (மாறிலி unsigned எழுத்து *)pad_spaces;
  போது(இலக்கம் > PAD_RUN) {
    எனில்(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      திரும்பு TRUE;
    இலக்கம் -= PAD_RUN;
  }
  திரும்பு stream_run(userp, stream, streamn, src, (size_t)இலக்கம், donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
நிலையான bool stream_zrun(வெறுமை *userp,
                        முழு (*stream)(unsigned எழுத்து, வெறுமை *),
                        mp_streamn streamn,
                        மாறிலி எழுத்து *str, size_t len, முழு *donep)
{
  size_t n = 0;
  போது(n < len && str[n])
    n++;
  திரும்பு stream_run(userp, stream, streamn,
                    (மாறிலி unsigned எழுத்து *)str, n, donep);
}

நிலையான bool out_double(வெறுமை *userp,
                       முழு (*stream)(unsigned எழுத்து, வெறுமை *),
                       mp_streamn streamn,
                       கட்டமைப்பு mproperty *p,
                       இரட்டை dnum,
                       எழுத்து *work, முழு *donep)
{
  எழுத்து fmt[32] = "%";
  எழுத்து *fptr = &fmt[1];
  size_t இடது = அளவு(fmt) - strlen(fmt);
  முழு flags = p->flags;
  முழு width = p->width;
  முழு prec = p->prec;

  எனில்(flags & FLAGS_இடது)
    *fptr++ = '-';
  எனில்(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  எனில்(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  எனில்(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  எனில்(width >= 0) {
    size_t dlen;
    எனில்(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, இடது, "%d", width);
    fptr += dlen;
    இடது -= dlen;
  }
  எனில்(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    முழு maxprec = BUFFSIZE - 1;
    இரட்டை val = dnum;
    முழு len;
    எனில்(prec > maxprec)
      prec = maxprec - 1;
    எனில்(width > 0 && prec <= width)
      maxprec -= width;
    போது(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    எனில்(prec > maxprec)
      prec = maxprec - 1;
    எனில்(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, இடது, ".%d", prec);
    fptr += len;
  }
  எனில்(flags & FLAGS_LONG)
    *fptr++ = 'l';

  எனில்(flags & FLAGS_FLOATE)
    *fptr++ = (எழுத்து)((flags & FLAGS_UPPER) ? 'E' : 'e');
  இல்லையெனில் எனில்(flags & FLAGS_FLOATG)
    *fptr++ = (எழுத்து)((flags & FLAGS_UPPER) ? 'G' : 'g');
  இல்லையெனில்
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#இல்லையெனில்
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic எடு
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  திரும்பு stream_run(userp, stream, streamn,
                    (மாறிலி unsigned எழுத்து *)work, strlen(work), donep);
}

நிலையான bool out_எண்(வெறுமை *userp,
                       முழு (*stream)(unsigned எழுத்து, வெறுமை *),
                       mp_streamn streamn,
                       கட்டமைப்பு mproperty *p,
                       uint64_t இலக்கம்,
                       int64_t nums,
                       எழுத்து *work, முழு *donep)
{
  மாறிலி unsigned எழுத்து *digits = Curl_ldigits;
  முழு flags = p->flags;
  முழு width = p->width;
  முழு prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  முழு base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  எழுத்து *workend = &work[BUFFSIZE - 2];
  எழுத்து *w;

  எனில்(flags & FLAGS_CHAR) {
    /* Character. */
    எனில்(!(flags & FLAGS_இடது)) {
      எனில்(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        திரும்பு TRUE;
      width = 1;
    }
    OUTCHAR((எழுத்து)இலக்கம்);
    எனில்(flags & FLAGS_இடது) {
      எனில்(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        திரும்பு TRUE;
    }
    திரும்பு FALSE;
  }
  எனில்(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  இல்லையெனில் எனில்(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  இல்லையெனில் எனில்(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  இல்லையெனில் {
    /* Decimal integer. */
    is_neg = (nums < 0);
    எனில்(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_இலக்கம்; /* Used to convert negative in positive. */
      signed_இலக்கம் = nums + (int64_t)1;
      signed_இலக்கம் = -signed_இலக்கம்;
      இலக்கம் = (uint64_t)signed_இலக்கம்;
      இலக்கம் += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  எனில்(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  தேர்வு(base) {
  நிலை 10:
    போது(இலக்கம் > 0) {
      *w-- = (எழுத்து)('0' + (இலக்கம் % 10));
      இலக்கம் /= 10;
    }
    நிறுத்து;
  இயல்பு:
    போது(இலக்கம் > 0) {
      *w-- = digits[இலக்கம் % base];
      இலக்கம் /= base;
    }
    நிறுத்து;
  }
  width -= (முழு)(workend - w);
  prec -= (முழு)(workend - w);

  எனில்(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  எனில்(prec > 0) {
    width -= prec;
    போது(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  எனில்(is_alt && base == 16)
    width -= 2;

  எனில்(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  எனில்(!(flags & FLAGS_இடது) && !(flags & FLAGS_PAD_NIL)) {
    எனில்(stream_pad(userp, stream, streamn, ' ', width, donep))
      திரும்பு TRUE;
    width = 0;
  }

  எனில்(is_neg)
    OUTCHAR('-');
  இல்லையெனில் எனில்(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  இல்லையெனில் எனில்(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  எனில்(is_alt && base == 16) {
    OUTCHAR('0');
    எனில்(flags & FLAGS_UPPER)
      OUTCHAR('X');
    இல்லையெனில்
      OUTCHAR('x');
  }

  எனில்(!(flags & FLAGS_இடது) && (flags & FLAGS_PAD_NIL)) {
    எனில்(stream_pad(userp, stream, streamn, '0', width, donep))
      திரும்பு TRUE;
    width = 0;
  }

  /* Write the number. */
  எனில்(stream_run(userp, stream, streamn, (மாறிலி unsigned எழுத்து *)w + 1,
                (size_t)(workend - w), donep))
    திரும்பு TRUE;

  எனில்(flags & FLAGS_இடது) {
    எனில்(stream_pad(userp, stream, streamn, ' ', width, donep))
      திரும்பு TRUE;
  }

  திரும்பு FALSE;
}

நிலையான மாறிலி எழுத்து nilstr[] = "(nil)";

நிலையான bool out_string(வெறுமை *userp,
                       முழு (*stream)(unsigned எழுத்து, வெறுமை *),
                       mp_streamn streamn,
                       கட்டமைப்பு mproperty *p,
                       மாறிலி எழுத்து *str,
                       முழு *donep)
{
  முழு flags = p->flags;
  முழு width = p->width;
  முழு prec = p->prec;
  size_t len;

  எனில்(!str) {
    /* Write null string if there is space. */
    எனில்(prec == -1 || prec >= (முழு)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned முழு)FLAGS_ALT;
    }
    இல்லையெனில் {
      str = "";
      len = 0;
    }
  }
  இல்லையெனில் எனில்(prec != -1)
    len = (size_t)prec;
  இல்லையெனில் எனில்(*str == '\0')
    len = 0;
  இல்லையெனில்
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (முழு)len;

  எனில்(flags & FLAGS_ALT)
    OUTCHAR('"');

  எனில்(!(flags & FLAGS_இடது)) {
    எனில்(stream_pad(userp, stream, streamn, ' ', width, donep))
      திரும்பு TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  எனில்(prec == -1) {
    எனில்(stream_run(userp, stream, streamn,
                  (மாறிலி unsigned எழுத்து *)str, len, donep))
      திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(stream_zrun(userp, stream, streamn, str, len, donep))
    திரும்பு TRUE;
  எனில்(flags & FLAGS_இடது) {
    எனில்(stream_pad(userp, stream, streamn, ' ', width, donep))
      திரும்பு TRUE;
  }

  எனில்(flags & FLAGS_ALT)
    OUTCHAR('"');

  திரும்பு FALSE;
}

நிலையான bool out_pointer(வெறுமை *userp,
                        முழு (*stream)(unsigned எழுத்து, வெறுமை *),
                        mp_streamn streamn,
                        கட்டமைப்பு mproperty *p,
                        மாறிலி எழுத்து *ptr,
                        எழுத்து *work,
                        முழு *donep)
{
  /* Generic pointer. */
  எனில்(ptr) {
    size_t இலக்கம் = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    எனில்(out_எண்(userp, stream, streamn, p, இலக்கம், 0, work, donep))
      திரும்பு TRUE;
  }
  இல்லையெனில் {
    /* Write "(nil)" for a nil pointer. */
    முழு width = p->width;
    முழு flags = p->flags;

    width -= (முழு)CURL_CSTRLEN(nilstr);
    எனில்(flags & FLAGS_இடது) {
      எனில்(stream_pad(userp, stream, streamn, ' ', width, donep))
        திரும்பு TRUE;
      width = 0;
    }
    எனில்(stream_run(userp, stream, streamn, (மாறிலி unsigned எழுத்து *)nilstr,
                  அளவு(nilstr) - 1, donep))
      திரும்பு TRUE;
    எனில்(!(flags & FLAGS_இடது)) {
      எனில்(stream_pad(userp, stream, streamn, ' ', width, donep))
        திரும்பு TRUE;
    }
  }
  திரும்பு FALSE;
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
நிலையான முழு formatf(வெறுமை *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   முழு (*stream)(unsigned எழுத்து, வெறுமை *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   மாறிலி எழுத்து *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  முழு done = 0;   /* number of characters written */
  முழு i;
  முழு ocount = 0; /* number of output segments */
  முழு icount = 0; /* number of input arguments */

  கட்டமைப்பு outsegment output[MAX_SEGMENTS];
  கட்டமைப்பு va_உள்ளீடு உள்ளீடு[MAX_PARAMETERS];
  எழுத்து work[BUFFSIZE + 2];

  /* Parse the format string */
  எனில்(parsefmt(format, output, உள்ளீடு, &ocount, &icount, ap_save))
    திரும்பு 0;

  க்கு(i = 0; i < ocount; i++) {
    கட்டமைப்பு outsegment *optr = &output[i];
    கட்டமைப்பு va_உள்ளீடு *iptr = &உள்ளீடு[optr->உள்ளீடு];
    கட்டமைப்பு mproperty p;
    size_t outlen = optr->outlen;

    எனில்(outlen) {
      எனில்(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        திரும்பு done;
      எனில்(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        தொடர்;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    எனில்(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (முழு)உள்ளீடு[optr->width].val.nums;
      எனில்(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        எனில்(p.width == INT_MIN)
          p.width = INT_MAX;
        இல்லையெனில்
          p.width = -p.width;
        p.flags |= FLAGS_இடது;
        p.flags &= ~(unsigned முழு)FLAGS_PAD_NIL;
      }
    }
    இல்லையெனில்
      p.width = optr->width;

    /* pick up the specified precision */
    எனில்(p.flags & FLAGS_PRECPARAM) {
      p.prec = (முழு)உள்ளீடு[optr->precision].val.nums;
      எனில்(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    இல்லையெனில் எனில்(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    இல்லையெனில்
      p.prec = -1;

    தேர்வு(iptr->type) {
    நிலை MTYPE_INTU:
    நிலை MTYPE_LONGU:
    நிலை MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      எனில்(out_எண்(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        திரும்பு done;
      நிறுத்து;

    நிலை MTYPE_INT:
    நிலை MTYPE_LONG:
    நிலை MTYPE_LONGLONG:
      எனில்(out_எண்(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        திரும்பு done;
      நிறுத்து;

    நிலை MTYPE_STRING:
      எனில்(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        திரும்பு done;
      நிறுத்து;

    நிலை MTYPE_PTR:
      எனில்(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        திரும்பு done;
      நிறுத்து;

    நிலை MTYPE_DOUBLE:
    நிலை MTYPE_LONGDOUBLE:
      எனில்(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        திரும்பு done;
      நிறுத்து;

    நிலை MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      எனில்(flush && flush(userp))
        திரும்பு done;
      /* Answer the count of characters written. */
      எனில்(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      இல்லையெனில் எனில்(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      இல்லையெனில் எனில்(!(p.flags & FLAGS_SHORT))
        *(முழு *)iptr->val.ptr = done;
      இல்லையெனில்
        *(short *)iptr->val.ptr = (short)done;
      நிறுத்து;

    இயல்பு:
      நிறுத்து;
    }
  }
  திரும்பு done;
}

/* fputc() look-alike */
நிலையான முழு addbyter(unsigned எழுத்து outc, வெறுமை *f)
{
  கட்டமைப்பு nsprintf *infop = f;
  எனில்(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (எழுத்து)outc; /* store */
    infop->length++; /* we are now one byte larger */
    திரும்பு 0;     /* fputc() returns like this on success */
  }
  திரும்பு 1;
}

/* block variant of addbyter */
நிலையான size_t addrun(மாறிலி unsigned எழுத்து *buf, size_t len, வெறுமை *f)
{
  கட்டமைப்பு nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  எனில்(len > fit)
    len = fit;
  எனில்(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  திரும்பு len;
}

முழு curl_mvsnprintf(எழுத்து *buffer, size_t maxlength, மாறிலி எழுத்து *format,
                    va_list args)
{
  முழு retcode;
  கட்டமைப்பு nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, வெற்று, format, args);
  எனில்(info.max) {
    /* we null-terminate this */
    எனில்(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    இல்லையெனில்
      info.buffer[0] = 0;
  }
  திரும்பு retcode;
}

முழு curl_msnprintf(எழுத்து *buffer, size_t maxlength, மாறிலி எழுத்து *format, ...)
{
  முழு retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  திரும்பு retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
நிலையான முழு alloc_flush(வெறுமை *f)
{
  கட்டமைப்பு asprintf *infop = f;
  எனில்(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    எனில்(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      திரும்பு 1; /* fail */
    }
  }
  திரும்பு 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
நிலையான முழு alloc_addn(கட்டமைப்பு asprintf *infop, மாறிலி unsigned எழுத்து *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  எனில்(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    திரும்பு 1; /* fail */
  }
  திரும்பு 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
நிலையான bool alloc_fits(கட்டமைப்பு asprintf *infop, size_t more)
{
  திரும்பு (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
நிலையான முழு alloc_addbyter(unsigned எழுத்து outc, வெறுமை *f)
{
  கட்டமைப்பு asprintf *infop = f;
  எனில்(infop->nstage == அளவு(infop->stage) && alloc_flush(infop))
    திரும்பு 1; /* fail */
  எனில்(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    எனில்(alloc_flush(infop))
      திரும்பு 1;
    திரும்பு alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  திரும்பு 0;
}

/* block variant of alloc_addbyter */
நிலையான size_t alloc_addrun(மாறிலி unsigned எழுத்து *buf, size_t len, வெறுமை *f)
{
  கட்டமைப்பு asprintf *infop = f;
  size_t accepted = 0;
  போது(accepted < len) {
    size_t room = அளவு(infop->stage) - infop->nstage;
    size_t n;
    எனில்(!room) {
      எனில்(alloc_flush(infop))
        திரும்பு accepted; /* accepted < len aborts formatting */
      room = அளவு(infop->stage);
    }
    n = len - accepted;
    எனில்(n > room)
      n = room;
    எனில்(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      எனில்(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      எனில்(alloc_flush(infop))
        திரும்பு accepted;
      எனில்(alloc_addn(infop, buf + accepted, 1))
        திரும்பு accepted; /* accepted < len aborts formatting */
      accepted++;
      தொடர்;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  திரும்பு accepted;
}

/* appends the formatted string, returns MERR error code */
முழு curlx_dyn_vprintf(கட்டமைப்பு dynbuf *dyn, மாறிலி எழுத்து *format, va_list args)
{
  கட்டமைப்பு asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (வெறுமை)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  எனில்(!info.merr)
    (வெறுமை)alloc_flush(&info);
  எனில்(info.merr) {
    curlx_dyn_free(info.b);
    திரும்பு info.merr;
  }
  திரும்பு 0;
}

எழுத்து *curl_mvaprintf(மாறிலி எழுத்து *format, va_list args)
{
  கட்டமைப்பு asprintf info;
  கட்டமைப்பு dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (வெறுமை)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  எனில்(!info.merr)
    (வெறுமை)alloc_flush(&info);
  எனில்(info.merr) {
    curlx_dyn_free(info.b);
    திரும்பு வெற்று;
  }
  எனில்(curlx_dyn_len(info.b))
    திரும்பு curlx_dyn_ptr(info.b);
  திரும்பு curlx_strdup("");
}

எழுத்து *curl_maprintf(மாறிலி எழுத்து *format, ...)
{
  va_list args;
  எழுத்து *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  திரும்பு s;
}

நிலையான முழு storebuffer(unsigned எழுத்து outc, வெறுமை *f)
{
  எழுத்து **buffer = f;
  **buffer = (எழுத்து)outc;
  (*buffer)++;
  திரும்பு 0;
}

/* block variant of storebuffer */
நிலையான size_t storerun(மாறிலி unsigned எழுத்து *buf, size_t len, வெறுமை *f)
{
  எழுத்து **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  திரும்பு len;
}

முழு curl_msprintf(எழுத்து *buffer, மாறிலி எழுத்து *format, ...)
{
  va_list args; /* argument pointer */
  முழு retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, வெற்று, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  திரும்பு retcode;
}

நிலையான முழு fputc_wrapper(unsigned எழுத்து outc, வெறுமை *f)
{
  கட்டமைப்பு ioprintf *i = f;
  முழு out = outc;
  FILE *s = i->fd;
  முழு rc = fputc(out, s);
  i->error = (rc == EOF);
  திரும்பு (rc == EOF);
}

/* block variant of fputc_wrapper */
நிலையான size_t fwrite_wrapper(மாறிலி unsigned எழுத்து *buf, size_t len, வெறுமை *f)
{
  கட்டமைப்பு ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  திரும்பு n;
}

முழு curl_mvfprintf(FILE *fd, மாறிலி எழுத்து *format, va_list args)
{
  கட்டமைப்பு ioprintf info;
  முழு n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, வெற்று, format, args);
  எனில்(info.error)
    திரும்பு -1;
  திரும்பு n;
}

முழு curl_mfprintf(FILE *fd, மாறிலி எழுத்து *format, ...)
{
  va_list args; /* argument pointer */
  முழு n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  திரும்பு n;
}

முழு curl_mprintf(மாறிலி எழுத்து *format, ...)
{
  va_list args; /* argument pointer */
  முழு n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  திரும்பு n;
}

முழு curl_mvsprintf(எழுத்து *buffer, மாறிலி எழுத்து *format, va_list args)
{
  முழு retcode = formatf(&buffer, storebuffer, storerun, வெற்று, format, args);
  *buffer = 0; /* we null-terminate this */
  திரும்பு retcode;
}

முழு curl_mvprintf(மாறிலி எழுத்து *format, va_list args)
{
  திரும்பு curl_mvfprintf(stdout, format, args);
}

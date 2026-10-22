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
#شامل "curl_setup.h"

#شامل "curlx/dynbuf.h"
#شامل "curl_printf.h"
#شامل "curlx/strparse.h"
#شامل "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ثابت unsigned نویسه Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ثابت unsigned نویسه Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  انجامبده {                                    \
    اگر(stream((unsigned نویسه)(x), userp)) \
      بازگشت TRUE;                        \
    (*donep)++;                           \
  } وقتی(0)

/* Data type to read from the arglist */
تعریفنوع شمارش {
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
شمارش {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_چپ       = 1 << 2,
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

شمارش {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
ساختار va_درون_داد {
  FormatType type; /* FormatType */
  اجتماع {
    ثابت نویسه *str;
    تهی *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    دوگانه dnum;
  } val;
};

/*
 * Describes an output segment.
 */
ساختار outsegment {
  صحیح width;     /* width OR width parameter number */
  صحیح precision; /* precision OR precision parameter number */
  unsigned صحیح flags;
  unsigned صحیح درون_داد; /* input argument array index */
  ثابت نویسه *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

ساختار nsprintf {
  نویسه *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

ساختار asprintf {
  ساختار dynbuf *b;
  نویسه merr;
  size_t nstage;
  unsigned نویسه stage[ASPRINTF_STAGE_SIZE];
};

ساختار ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
ایستا صحیح dollarstring(ثابت نویسه *p, ثابت نویسه **end)
{
  curl_off_t رقم;
  اگر(curlx_str_عدد(&p, &رقم, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !رقم)
    بازگشت -1;
  *end = p;
  بازگشت (صحیح)رقم - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned نویسه)(1 << ((y) & 7)))

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

ایستا صحیح parse_flags(ثابت نویسه **fmtp, unsigned صحیح *flagsp, صحیح use_dollar,
                       صحیح *precp, صحیح *widthp)
{
  ثابت نویسه *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned صحیح flags = 0;
  صحیح width = 0;
  صحیح precision = 0;

  /* Handle the flags */
  انجامبده {
    انتخاب(*fmt++) {
    حالت ' ':
      flags |= FLAGS_SPACE;
      بشکن;
    حالت '+':
      flags |= FLAGS_SHOWSIGN;
      بشکن;
    حالت '-':
      flags |= FLAGS_چپ;
      flags &= ~(unsigned صحیح)FLAGS_PAD_NIL;
      بشکن;
    حالت '#':
      flags |= FLAGS_ALT;
      بشکن;
    حالت '.':
      اگر('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        اگر(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          اگر(precision < 0)
            /* illegal combo */
            بازگشت PFMT_DOLLARPREC;
        }
        وگرنه
          /* get it from the next argument */
          precision = -1;
      }
      وگرنه {
        bool is_neg;
        curl_off_t رقم;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        اگر(is_neg)
          fmt++;
        اگر(curlx_str_عدد(&fmt, &رقم, INT_MAX))
          بازگشت PFMT_PREC;
        precision = (صحیح)رقم;
        اگر(is_neg)
          precision = -precision;
      }
      اگر((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        بازگشت PFMT_PRECMIX;
      بشکن;
    حالت 'h':
      flags |= FLAGS_SHORT;
      بشکن;
#ifdef _WIN32
    حالت 'I':
      /* Non-ANSI integer extensions I32 I64 */
      اگر((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      وگرنه اگر((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      وگرنه {
#اگر SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#وگرنه
        flags |= FLAGS_LONG;
#endif
      }
      بشکن;
#endif /* _WIN32 */
    حالت 'l':
      اگر(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      وگرنه
        flags |= FLAGS_LONG;
      بشکن;
    حالت 'L':
      flags |= FLAGS_LONGDOUBLE;
      بشکن;
    حالت 'q':
      flags |= FLAGS_LONGLONG;
      بشکن;
    حالت 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#اگر SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#وگرنه
      flags |= FLAGS_LONG;
#endif
      بشکن;
    حالت 'O':
#اگر SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#وگرنه
      flags |= FLAGS_LONG;
#endif
      بشکن;
    حالت '0':
      اگر(!(flags & FLAGS_چپ))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    حالت '1':
    حالت '2':
    حالت '3':
    حالت '4':
    حالت '5':
    حالت '6':
    حالت '7':
    حالت '8':
    حالت '9': {
      curl_off_t رقم;
      flags |= FLAGS_WIDTH;
      fmt--;
      اگر(curlx_str_عدد(&fmt, &رقم, INT_MAX))
        بازگشت PFMT_WIDTH;
      width = (صحیح)رقم;
      بشکن;
    }
    حالت '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      اگر(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        اگر(width < 0)
          /* illegal combo */
          بازگشت PFMT_DOLLARWIDTH;
      }
      وگرنه
        /* pick from the next argument */
        width = -1;
      بشکن;
    پیشفرض:
      loopit = FALSE;
      fmt--;
      بشکن;
    } /* switch */
  } وقتی(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  بازگشت PFMT_OK;
}

ایستا bool parse_conversion(ثابت نویسه f, unsigned صحیح *flagp,
                             FormatType *typep)
{
  unsigned صحیح flags = *flagp;
  FormatType type;
  انتخاب(f) {
  حالت 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    بشکن;
  حالت 's':
    type = MTYPE_STRING;
    بشکن;
  حالت 'n':
    type = MTYPE_INTPTR;
    بشکن;
  حالت 'p':
    type = MTYPE_PTR;
    بشکن;
  حالت 'd':
  حالت 'i':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    وگرنه اگر(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    وگرنه
      type = MTYPE_INT;
    بشکن;
  حالت 'u':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    وگرنه اگر(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    وگرنه
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    بشکن;
  حالت 'o':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    وگرنه اگر(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    وگرنه
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    بشکن;
  حالت 'x':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    وگرنه اگر(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    وگرنه
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    بشکن;
  حالت 'X':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    وگرنه اگر(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    وگرنه
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    بشکن;
  حالت 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    بشکن;
  حالت 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    بشکن;
  حالت 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    بشکن;
  حالت 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    بشکن;
  حالت 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    بشکن;
  حالت 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    بشکن;
  حالت 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    بشکن;
  پیشفرض:
    /* invalid instruction, disregard and continue */
    بازگشت TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  بازگشت FALSE;
}

ایستا صحیح parsefmt(ثابت نویسه *format,
                    ساختار outsegment *out,
                    ساختار va_درون_داد *in,
                    صحیح *opieces,
                    صحیح *ipieces, va_list arglist)
{
  ثابت نویسه *fmt = format;
  صحیح param_رقم = 0;
  صحیح max_param = -1;
  صحیح i;
  صحیح ocount = 0;
  unsigned نویسه usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  ساختار outsegment *optr;
  صحیح use_dollar = DOLLAR_UNKNOWN;
  ثابت نویسه *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, اندازه(usedinput));

  وقتی(*fmt) {
    اگر(*fmt == '%') {
      ساختار va_درون_داد *iptr;
      FormatType type;
      unsigned صحیح flags = 0;
      صحیح width = 0;
      صحیح precision = 0;
      صحیح param = -1;
      صحیح rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      اگر(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        اگر(outlen) {
          optr = &out[ocount++];
          اگر(ocount > MAX_SEGMENTS)
            بازگشت PFMT_MANYSEGS;
          optr->درون_داد = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        ادامه; /* while */
      }

      اگر(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        اگر(param < 0) {
          اگر(use_dollar == DOLLAR_USE)
            /* illegal combo */
            بازگشت PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        وگرنه
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      اگر(rc)
        بازگشت rc;

      اگر(parse_conversion(*fmt, &flags, &type))
        ادامه;

      اگر(flags & FLAGS_WIDTHPARAM) {
        اگر(width < 0)
          width = param_رقم++;
        وگرنه {
          /* if this identifies a parameter already used, this is illegal */
          اگر(is_arg_used(usedinput, width))
            بازگشت PFMT_WIDTHARG;
        }
        اگر(width >= MAX_PARAMETERS)
          بازگشت PFMT_MANYARGS;
        اگر(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      اگر(flags & FLAGS_PRECPARAM) {
        اگر(precision < 0)
          precision = param_رقم++;
        وگرنه {
          /* if this identifies a parameter already used, this is illegal */
          اگر(is_arg_used(usedinput, precision))
            بازگشت PFMT_PRECARG;
        }
        اگر(precision >= MAX_PARAMETERS)
          بازگشت PFMT_MANYARGS;
        اگر(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      اگر(param < 0)
        param = param_رقم++;
      اگر(param >= MAX_PARAMETERS)
        بازگشت PFMT_MANYARGS;
      اگر(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      اگر(ocount > MAX_SEGMENTS)
        بازگشت PFMT_MANYSEGS;
      optr->درون_داد = (unsigned صحیح)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    وگرنه
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  اگر(outlen) {
    optr = &out[ocount++];
    اگر(ocount > MAX_SEGMENTS)
      بازگشت PFMT_MANYSEGS;
    optr->درون_داد = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  برای(i = 0; i < max_param + 1; i++) {
    ساختار va_درون_داد *iptr = &in[i];
    اگر(!is_arg_used(usedinput, i))
      /* bad input */
      بازگشت PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    انتخاب(iptr->type) {
    حالت MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ثابت نویسه *);
      بشکن;

    حالت MTYPE_INTPTR:
    حالت MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, تهی *);
      بشکن;

    حالت MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      بشکن;

    حالت MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      بشکن;

    حالت MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      بشکن;

    حالت MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      بشکن;

    حالت MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned صحیح);
      بشکن;

    حالت MTYPE_INT:
    حالت MTYPE_WIDTH:
    حالت MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, صحیح);
      بشکن;

    حالت MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, دوگانه);
      بشکن;

    حالت MTYPE_LONGDOUBLE:
      iptr->val.dnum = (دوگانه)va_arg(arglist, long دوگانه);
      بشکن;

    پیشفرض:
      DEBUGASSERT(پوچ); /* unexpected */
      بشکن;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  بازگشت PFMT_OK;
}

ساختار mproperty {
  صحیح width;            /* Width of a field. */
  صحیح prec;             /* Precision of a field. */
  unsigned صحیح flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
تعریفنوع size_t (*mp_streamn)(ثابت unsigned نویسه *buf, size_t len, تهی *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
تعریفنوع صحیح (*mp_flush)(تهی *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
ایستا bool stream_run(تهی *userp,
                       صحیح (*stream)(unsigned نویسه, تهی *),
                       mp_streamn streamn,
                       ثابت unsigned نویسه *buf, size_t len, صحیح *donep)
{
  اگر(!len)
    بازگشت FALSE;
  اگر(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (صحیح)n;
    بازگشت n != len;
  }
  وقتی(len--) {
    اگر(stream(*buf++, userp))
      بازگشت TRUE;
    (*donep)++;
  }
  بازگشت FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
ایستا ثابت نویسه pad_spaces[] = "                ";
ایستا ثابت نویسه pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
ایستا bool stream_pad(تهی *userp,
                       صحیح (*stream)(unsigned نویسه, تهی *),
                       mp_streamn streamn,
                       unsigned نویسه pad, صحیح رقم, صحیح *donep)
{
  ثابت unsigned نویسه *src;
  اگر(رقم <= 0)
    بازگشت FALSE;
  اگر(pad == '0')
    src = (ثابت unsigned نویسه *)pad_zeros;
  وگرنه
    src = (ثابت unsigned نویسه *)pad_spaces;
  وقتی(رقم > PAD_RUN) {
    اگر(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      بازگشت TRUE;
    رقم -= PAD_RUN;
  }
  بازگشت stream_run(userp, stream, streamn, src, (size_t)رقم, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
ایستا bool stream_zrun(تهی *userp,
                        صحیح (*stream)(unsigned نویسه, تهی *),
                        mp_streamn streamn,
                        ثابت نویسه *str, size_t len, صحیح *donep)
{
  size_t n = 0;
  وقتی(n < len && str[n])
    n++;
  بازگشت stream_run(userp, stream, streamn,
                    (ثابت unsigned نویسه *)str, n, donep);
}

ایستا bool out_double(تهی *userp,
                       صحیح (*stream)(unsigned نویسه, تهی *),
                       mp_streamn streamn,
                       ساختار mproperty *p,
                       دوگانه dnum,
                       نویسه *work, صحیح *donep)
{
  نویسه fmt[32] = "%";
  نویسه *fptr = &fmt[1];
  size_t چپ = اندازه(fmt) - strlen(fmt);
  صحیح flags = p->flags;
  صحیح width = p->width;
  صحیح prec = p->prec;

  اگر(flags & FLAGS_چپ)
    *fptr++ = '-';
  اگر(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  اگر(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  اگر(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  اگر(width >= 0) {
    size_t dlen;
    اگر(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, چپ, "%d", width);
    fptr += dlen;
    چپ -= dlen;
  }
  اگر(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    صحیح maxprec = BUFFSIZE - 1;
    دوگانه val = dnum;
    صحیح len;
    اگر(prec > maxprec)
      prec = maxprec - 1;
    اگر(width > 0 && prec <= width)
      maxprec -= width;
    وقتی(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    اگر(prec > maxprec)
      prec = maxprec - 1;
    اگر(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, چپ, ".%d", prec);
    fptr += len;
  }
  اگر(flags & FLAGS_LONG)
    *fptr++ = 'l';

  اگر(flags & FLAGS_FLOATE)
    *fptr++ = (نویسه)((flags & FLAGS_UPPER) ? 'E' : 'e');
  وگرنه اگر(flags & FLAGS_FLOATG)
    *fptr++ = (نویسه)((flags & FLAGS_UPPER) ? 'G' : 'g');
  وگرنه
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#وگرنه
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic برداشت
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  بازگشت stream_run(userp, stream, streamn,
                    (ثابت unsigned نویسه *)work, strlen(work), donep);
}

ایستا bool out_عدد(تهی *userp,
                       صحیح (*stream)(unsigned نویسه, تهی *),
                       mp_streamn streamn,
                       ساختار mproperty *p,
                       uint64_t رقم,
                       int64_t nums,
                       نویسه *work, صحیح *donep)
{
  ثابت unsigned نویسه *digits = Curl_ldigits;
  صحیح flags = p->flags;
  صحیح width = p->width;
  صحیح prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  صحیح base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  نویسه *workend = &work[BUFFSIZE - 2];
  نویسه *w;

  اگر(flags & FLAGS_CHAR) {
    /* Character. */
    اگر(!(flags & FLAGS_چپ)) {
      اگر(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        بازگشت TRUE;
      width = 1;
    }
    OUTCHAR((نویسه)رقم);
    اگر(flags & FLAGS_چپ) {
      اگر(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        بازگشت TRUE;
    }
    بازگشت FALSE;
  }
  اگر(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  وگرنه اگر(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  وگرنه اگر(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  وگرنه {
    /* Decimal integer. */
    is_neg = (nums < 0);
    اگر(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_رقم; /* Used to convert negative in positive. */
      signed_رقم = nums + (int64_t)1;
      signed_رقم = -signed_رقم;
      رقم = (uint64_t)signed_رقم;
      رقم += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  اگر(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  انتخاب(base) {
  حالت 10:
    وقتی(رقم > 0) {
      *w-- = (نویسه)('0' + (رقم % 10));
      رقم /= 10;
    }
    بشکن;
  پیشفرض:
    وقتی(رقم > 0) {
      *w-- = digits[رقم % base];
      رقم /= base;
    }
    بشکن;
  }
  width -= (صحیح)(workend - w);
  prec -= (صحیح)(workend - w);

  اگر(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  اگر(prec > 0) {
    width -= prec;
    وقتی(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  اگر(is_alt && base == 16)
    width -= 2;

  اگر(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  اگر(!(flags & FLAGS_چپ) && !(flags & FLAGS_PAD_NIL)) {
    اگر(stream_pad(userp, stream, streamn, ' ', width, donep))
      بازگشت TRUE;
    width = 0;
  }

  اگر(is_neg)
    OUTCHAR('-');
  وگرنه اگر(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  وگرنه اگر(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  اگر(is_alt && base == 16) {
    OUTCHAR('0');
    اگر(flags & FLAGS_UPPER)
      OUTCHAR('X');
    وگرنه
      OUTCHAR('x');
  }

  اگر(!(flags & FLAGS_چپ) && (flags & FLAGS_PAD_NIL)) {
    اگر(stream_pad(userp, stream, streamn, '0', width, donep))
      بازگشت TRUE;
    width = 0;
  }

  /* Write the number. */
  اگر(stream_run(userp, stream, streamn, (ثابت unsigned نویسه *)w + 1,
                (size_t)(workend - w), donep))
    بازگشت TRUE;

  اگر(flags & FLAGS_چپ) {
    اگر(stream_pad(userp, stream, streamn, ' ', width, donep))
      بازگشت TRUE;
  }

  بازگشت FALSE;
}

ایستا ثابت نویسه nilstr[] = "(nil)";

ایستا bool out_string(تهی *userp,
                       صحیح (*stream)(unsigned نویسه, تهی *),
                       mp_streamn streamn,
                       ساختار mproperty *p,
                       ثابت نویسه *str,
                       صحیح *donep)
{
  صحیح flags = p->flags;
  صحیح width = p->width;
  صحیح prec = p->prec;
  size_t len;

  اگر(!str) {
    /* Write null string if there is space. */
    اگر(prec == -1 || prec >= (صحیح)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned صحیح)FLAGS_ALT;
    }
    وگرنه {
      str = "";
      len = 0;
    }
  }
  وگرنه اگر(prec != -1)
    len = (size_t)prec;
  وگرنه اگر(*str == '\0')
    len = 0;
  وگرنه
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (صحیح)len;

  اگر(flags & FLAGS_ALT)
    OUTCHAR('"');

  اگر(!(flags & FLAGS_چپ)) {
    اگر(stream_pad(userp, stream, streamn, ' ', width, donep))
      بازگشت TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  اگر(prec == -1) {
    اگر(stream_run(userp, stream, streamn,
                  (ثابت unsigned نویسه *)str, len, donep))
      بازگشت TRUE;
  }
  وگرنه اگر(stream_zrun(userp, stream, streamn, str, len, donep))
    بازگشت TRUE;
  اگر(flags & FLAGS_چپ) {
    اگر(stream_pad(userp, stream, streamn, ' ', width, donep))
      بازگشت TRUE;
  }

  اگر(flags & FLAGS_ALT)
    OUTCHAR('"');

  بازگشت FALSE;
}

ایستا bool out_pointer(تهی *userp,
                        صحیح (*stream)(unsigned نویسه, تهی *),
                        mp_streamn streamn,
                        ساختار mproperty *p,
                        ثابت نویسه *ptr,
                        نویسه *work,
                        صحیح *donep)
{
  /* Generic pointer. */
  اگر(ptr) {
    size_t رقم = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    اگر(out_عدد(userp, stream, streamn, p, رقم, 0, work, donep))
      بازگشت TRUE;
  }
  وگرنه {
    /* Write "(nil)" for a nil pointer. */
    صحیح width = p->width;
    صحیح flags = p->flags;

    width -= (صحیح)CURL_CSTRLEN(nilstr);
    اگر(flags & FLAGS_چپ) {
      اگر(stream_pad(userp, stream, streamn, ' ', width, donep))
        بازگشت TRUE;
      width = 0;
    }
    اگر(stream_run(userp, stream, streamn, (ثابت unsigned نویسه *)nilstr,
                  اندازه(nilstr) - 1, donep))
      بازگشت TRUE;
    اگر(!(flags & FLAGS_چپ)) {
      اگر(stream_pad(userp, stream, streamn, ' ', width, donep))
        بازگشت TRUE;
    }
  }
  بازگشت FALSE;
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
ایستا صحیح formatf(تهی *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   صحیح (*stream)(unsigned نویسه, تهی *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ثابت نویسه *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  صحیح done = 0;   /* number of characters written */
  صحیح i;
  صحیح ocount = 0; /* number of output segments */
  صحیح icount = 0; /* number of input arguments */

  ساختار outsegment output[MAX_SEGMENTS];
  ساختار va_درون_داد درون_داد[MAX_PARAMETERS];
  نویسه work[BUFFSIZE + 2];

  /* Parse the format string */
  اگر(parsefmt(format, output, درون_داد, &ocount, &icount, ap_save))
    بازگشت 0;

  برای(i = 0; i < ocount; i++) {
    ساختار outsegment *optr = &output[i];
    ساختار va_درون_داد *iptr = &درون_داد[optr->درون_داد];
    ساختار mproperty p;
    size_t outlen = optr->outlen;

    اگر(outlen) {
      اگر(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        بازگشت done;
      اگر(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        ادامه;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    اگر(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (صحیح)درون_داد[optr->width].val.nums;
      اگر(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        اگر(p.width == INT_MIN)
          p.width = INT_MAX;
        وگرنه
          p.width = -p.width;
        p.flags |= FLAGS_چپ;
        p.flags &= ~(unsigned صحیح)FLAGS_PAD_NIL;
      }
    }
    وگرنه
      p.width = optr->width;

    /* pick up the specified precision */
    اگر(p.flags & FLAGS_PRECPARAM) {
      p.prec = (صحیح)درون_داد[optr->precision].val.nums;
      اگر(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    وگرنه اگر(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    وگرنه
      p.prec = -1;

    انتخاب(iptr->type) {
    حالت MTYPE_INTU:
    حالت MTYPE_LONGU:
    حالت MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      اگر(out_عدد(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        بازگشت done;
      بشکن;

    حالت MTYPE_INT:
    حالت MTYPE_LONG:
    حالت MTYPE_LONGLONG:
      اگر(out_عدد(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        بازگشت done;
      بشکن;

    حالت MTYPE_STRING:
      اگر(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        بازگشت done;
      بشکن;

    حالت MTYPE_PTR:
      اگر(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        بازگشت done;
      بشکن;

    حالت MTYPE_DOUBLE:
    حالت MTYPE_LONGDOUBLE:
      اگر(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        بازگشت done;
      بشکن;

    حالت MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      اگر(flush && flush(userp))
        بازگشت done;
      /* Answer the count of characters written. */
      اگر(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      وگرنه اگر(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      وگرنه اگر(!(p.flags & FLAGS_SHORT))
        *(صحیح *)iptr->val.ptr = done;
      وگرنه
        *(short *)iptr->val.ptr = (short)done;
      بشکن;

    پیشفرض:
      بشکن;
    }
  }
  بازگشت done;
}

/* fputc() look-alike */
ایستا صحیح addbyter(unsigned نویسه outc, تهی *f)
{
  ساختار nsprintf *infop = f;
  اگر(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (نویسه)outc; /* store */
    infop->length++; /* we are now one byte larger */
    بازگشت 0;     /* fputc() returns like this on success */
  }
  بازگشت 1;
}

/* block variant of addbyter */
ایستا size_t addrun(ثابت unsigned نویسه *buf, size_t len, تهی *f)
{
  ساختار nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  اگر(len > fit)
    len = fit;
  اگر(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  بازگشت len;
}

صحیح curl_mvsnprintf(نویسه *buffer, size_t maxlength, ثابت نویسه *format,
                    va_list args)
{
  صحیح retcode;
  ساختار nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, پوچ, format, args);
  اگر(info.max) {
    /* we null-terminate this */
    اگر(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    وگرنه
      info.buffer[0] = 0;
  }
  بازگشت retcode;
}

صحیح curl_msnprintf(نویسه *buffer, size_t maxlength, ثابت نویسه *format, ...)
{
  صحیح retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  بازگشت retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
ایستا صحیح alloc_flush(تهی *f)
{
  ساختار asprintf *infop = f;
  اگر(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    اگر(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      بازگشت 1; /* fail */
    }
  }
  بازگشت 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
ایستا صحیح alloc_addn(ساختار asprintf *infop, ثابت unsigned نویسه *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  اگر(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    بازگشت 1; /* fail */
  }
  بازگشت 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
ایستا bool alloc_fits(ساختار asprintf *infop, size_t more)
{
  بازگشت (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
ایستا صحیح alloc_addbyter(unsigned نویسه outc, تهی *f)
{
  ساختار asprintf *infop = f;
  اگر(infop->nstage == اندازه(infop->stage) && alloc_flush(infop))
    بازگشت 1; /* fail */
  اگر(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    اگر(alloc_flush(infop))
      بازگشت 1;
    بازگشت alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  بازگشت 0;
}

/* block variant of alloc_addbyter */
ایستا size_t alloc_addrun(ثابت unsigned نویسه *buf, size_t len, تهی *f)
{
  ساختار asprintf *infop = f;
  size_t accepted = 0;
  وقتی(accepted < len) {
    size_t room = اندازه(infop->stage) - infop->nstage;
    size_t n;
    اگر(!room) {
      اگر(alloc_flush(infop))
        بازگشت accepted; /* accepted < len aborts formatting */
      room = اندازه(infop->stage);
    }
    n = len - accepted;
    اگر(n > room)
      n = room;
    اگر(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      اگر(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      اگر(alloc_flush(infop))
        بازگشت accepted;
      اگر(alloc_addn(infop, buf + accepted, 1))
        بازگشت accepted; /* accepted < len aborts formatting */
      accepted++;
      ادامه;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  بازگشت accepted;
}

/* appends the formatted string, returns MERR error code */
صحیح curlx_dyn_vprintf(ساختار dynbuf *dyn, ثابت نویسه *format, va_list args)
{
  ساختار asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (تهی)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  اگر(!info.merr)
    (تهی)alloc_flush(&info);
  اگر(info.merr) {
    curlx_dyn_free(info.b);
    بازگشت info.merr;
  }
  بازگشت 0;
}

نویسه *curl_mvaprintf(ثابت نویسه *format, va_list args)
{
  ساختار asprintf info;
  ساختار dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (تهی)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  اگر(!info.merr)
    (تهی)alloc_flush(&info);
  اگر(info.merr) {
    curlx_dyn_free(info.b);
    بازگشت پوچ;
  }
  اگر(curlx_dyn_len(info.b))
    بازگشت curlx_dyn_ptr(info.b);
  بازگشت curlx_strdup("");
}

نویسه *curl_maprintf(ثابت نویسه *format, ...)
{
  va_list args;
  نویسه *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  بازگشت s;
}

ایستا صحیح storebuffer(unsigned نویسه outc, تهی *f)
{
  نویسه **buffer = f;
  **buffer = (نویسه)outc;
  (*buffer)++;
  بازگشت 0;
}

/* block variant of storebuffer */
ایستا size_t storerun(ثابت unsigned نویسه *buf, size_t len, تهی *f)
{
  نویسه **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  بازگشت len;
}

صحیح curl_msprintf(نویسه *buffer, ثابت نویسه *format, ...)
{
  va_list args; /* argument pointer */
  صحیح retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, پوچ, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  بازگشت retcode;
}

ایستا صحیح fputc_wrapper(unsigned نویسه outc, تهی *f)
{
  ساختار ioprintf *i = f;
  صحیح out = outc;
  FILE *s = i->fd;
  صحیح rc = fputc(out, s);
  i->error = (rc == EOF);
  بازگشت (rc == EOF);
}

/* block variant of fputc_wrapper */
ایستا size_t fwrite_wrapper(ثابت unsigned نویسه *buf, size_t len, تهی *f)
{
  ساختار ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  بازگشت n;
}

صحیح curl_mvfprintf(FILE *fd, ثابت نویسه *format, va_list args)
{
  ساختار ioprintf info;
  صحیح n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, پوچ, format, args);
  اگر(info.error)
    بازگشت -1;
  بازگشت n;
}

صحیح curl_mfprintf(FILE *fd, ثابت نویسه *format, ...)
{
  va_list args; /* argument pointer */
  صحیح n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  بازگشت n;
}

صحیح curl_mprintf(ثابت نویسه *format, ...)
{
  va_list args; /* argument pointer */
  صحیح n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  بازگشت n;
}

صحیح curl_mvsprintf(نویسه *buffer, ثابت نویسه *format, va_list args)
{
  صحیح retcode = formatf(&buffer, storebuffer, storerun, پوچ, format, args);
  *buffer = 0; /* we null-terminate this */
  بازگشت retcode;
}

صحیح curl_mvprintf(ثابت نویسه *format, va_list args)
{
  بازگشت curl_mvfprintf(stdout, format, args);
}

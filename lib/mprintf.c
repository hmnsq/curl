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
#ضمن "curl_setup.h"

#ضمن "curlx/dynbuf.h"
#ضمن "curl_printf.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ثابت unsigned حرف Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ثابت unsigned حرف Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  افعل {                                    \
    إذا(stream((unsigned حرف)(x), userp)) \
      أرجع TRUE;                        \
    (*donep)++;                           \
  } طالما(0)

/* Data type to read from the arglist */
تعريفنوع تعداد {
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
تعداد {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_يسار       = 1 << 2,
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

تعداد {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
بنية va_مدخل {
  FormatType type; /* FormatType */
  اتحاد {
    ثابت حرف *str;
    فراغ *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    مزدوج dnum;
  } val;
};

/*
 * Describes an output segment.
 */
بنية outsegment {
  صحيح width;     /* width OR width parameter number */
  صحيح precision; /* precision OR precision parameter number */
  unsigned صحيح flags;
  unsigned صحيح مدخل; /* input argument array index */
  ثابت حرف *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

بنية nsprintf {
  حرف *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

بنية asprintf {
  بنية dynbuf *b;
  حرف merr;
  size_t nstage;
  unsigned حرف stage[ASPRINTF_STAGE_SIZE];
};

بنية ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
ساكن صحيح dollarstring(ثابت حرف *p, ثابت حرف **end)
{
  curl_off_t رقم;
  إذا(curlx_str_رقم(&p, &رقم, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !رقم)
    أرجع -1;
  *end = p;
  أرجع (صحيح)رقم - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned حرف)(1 << ((y) & 7)))

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

ساكن صحيح parse_flags(ثابت حرف **fmtp, unsigned صحيح *flagsp, صحيح use_dollar,
                       صحيح *precp, صحيح *widthp)
{
  ثابت حرف *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned صحيح flags = 0;
  صحيح width = 0;
  صحيح precision = 0;

  /* Handle the flags */
  افعل {
    بدل(*fmt++) {
    حالة ' ':
      flags |= FLAGS_SPACE;
      اكسر;
    حالة '+':
      flags |= FLAGS_SHOWSIGN;
      اكسر;
    حالة '-':
      flags |= FLAGS_يسار;
      flags &= ~(unsigned صحيح)FLAGS_PAD_NIL;
      اكسر;
    حالة '#':
      flags |= FLAGS_ALT;
      اكسر;
    حالة '.':
      إذا('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        إذا(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          إذا(precision < 0)
            /* illegal combo */
            أرجع PFMT_DOLLARPREC;
        }
        وإلا
          /* get it from the next argument */
          precision = -1;
      }
      وإلا {
        bool is_neg;
        curl_off_t رقم;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        إذا(is_neg)
          fmt++;
        إذا(curlx_str_رقم(&fmt, &رقم, INT_MAX))
          أرجع PFMT_PREC;
        precision = (صحيح)رقم;
        إذا(is_neg)
          precision = -precision;
      }
      إذا((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        أرجع PFMT_PRECMIX;
      اكسر;
    حالة 'h':
      flags |= FLAGS_SHORT;
      اكسر;
#ifdef _WIN32
    حالة 'I':
      /* Non-ANSI integer extensions I32 I64 */
      إذا((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      وإلا إذا((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      وإلا {
#إذا SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#وإلا
        flags |= FLAGS_LONG;
#endif
      }
      اكسر;
#endif /* _WIN32 */
    حالة 'l':
      إذا(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      وإلا
        flags |= FLAGS_LONG;
      اكسر;
    حالة 'L':
      flags |= FLAGS_LONGDOUBLE;
      اكسر;
    حالة 'q':
      flags |= FLAGS_LONGLONG;
      اكسر;
    حالة 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#إذا SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#وإلا
      flags |= FLAGS_LONG;
#endif
      اكسر;
    حالة 'O':
#إذا SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#وإلا
      flags |= FLAGS_LONG;
#endif
      اكسر;
    حالة '0':
      إذا(!(flags & FLAGS_يسار))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    حالة '1':
    حالة '2':
    حالة '3':
    حالة '4':
    حالة '5':
    حالة '6':
    حالة '7':
    حالة '8':
    حالة '9': {
      curl_off_t رقم;
      flags |= FLAGS_WIDTH;
      fmt--;
      إذا(curlx_str_رقم(&fmt, &رقم, INT_MAX))
        أرجع PFMT_WIDTH;
      width = (صحيح)رقم;
      اكسر;
    }
    حالة '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      إذا(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        إذا(width < 0)
          /* illegal combo */
          أرجع PFMT_DOLLARWIDTH;
      }
      وإلا
        /* pick from the next argument */
        width = -1;
      اكسر;
    افتراضي:
      loopit = FALSE;
      fmt--;
      اكسر;
    } /* switch */
  } طالما(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  أرجع PFMT_OK;
}

ساكن bool parse_conversion(ثابت حرف f, unsigned صحيح *flagp,
                             FormatType *typep)
{
  unsigned صحيح flags = *flagp;
  FormatType type;
  بدل(f) {
  حالة 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    اكسر;
  حالة 's':
    type = MTYPE_STRING;
    اكسر;
  حالة 'n':
    type = MTYPE_INTPTR;
    اكسر;
  حالة 'p':
    type = MTYPE_PTR;
    اكسر;
  حالة 'd':
  حالة 'i':
    إذا(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    وإلا إذا(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    وإلا
      type = MTYPE_INT;
    اكسر;
  حالة 'u':
    إذا(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    وإلا إذا(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    وإلا
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    اكسر;
  حالة 'o':
    إذا(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    وإلا إذا(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    وإلا
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    اكسر;
  حالة 'x':
    إذا(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    وإلا إذا(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    وإلا
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    اكسر;
  حالة 'X':
    إذا(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    وإلا إذا(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    وإلا
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    اكسر;
  حالة 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    اكسر;
  حالة 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    اكسر;
  حالة 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    اكسر;
  حالة 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    اكسر;
  حالة 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    اكسر;
  حالة 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    اكسر;
  حالة 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    اكسر;
  افتراضي:
    /* invalid instruction, disregard and continue */
    أرجع TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  أرجع FALSE;
}

ساكن صحيح parsefmt(ثابت حرف *format,
                    بنية outsegment *out,
                    بنية va_مدخل *in,
                    صحيح *opieces,
                    صحيح *ipieces, va_list arglist)
{
  ثابت حرف *fmt = format;
  صحيح param_رقم = 0;
  صحيح max_param = -1;
  صحيح i;
  صحيح ocount = 0;
  unsigned حرف usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  بنية outsegment *optr;
  صحيح use_dollar = DOLLAR_UNKNOWN;
  ثابت حرف *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, حجم(usedinput));

  طالما(*fmt) {
    إذا(*fmt == '%') {
      بنية va_مدخل *iptr;
      FormatType type;
      unsigned صحيح flags = 0;
      صحيح width = 0;
      صحيح precision = 0;
      صحيح param = -1;
      صحيح rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      إذا(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        إذا(outlen) {
          optr = &out[ocount++];
          إذا(ocount > MAX_SEGMENTS)
            أرجع PFMT_MANYSEGS;
          optr->مدخل = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        استمر; /* while */
      }

      إذا(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        إذا(param < 0) {
          إذا(use_dollar == DOLLAR_USE)
            /* illegal combo */
            أرجع PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        وإلا
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      إذا(rc)
        أرجع rc;

      إذا(parse_conversion(*fmt, &flags, &type))
        استمر;

      إذا(flags & FLAGS_WIDTHPARAM) {
        إذا(width < 0)
          width = param_رقم++;
        وإلا {
          /* if this identifies a parameter already used, this is illegal */
          إذا(is_arg_used(usedinput, width))
            أرجع PFMT_WIDTHARG;
        }
        إذا(width >= MAX_PARAMETERS)
          أرجع PFMT_MANYARGS;
        إذا(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      إذا(flags & FLAGS_PRECPARAM) {
        إذا(precision < 0)
          precision = param_رقم++;
        وإلا {
          /* if this identifies a parameter already used, this is illegal */
          إذا(is_arg_used(usedinput, precision))
            أرجع PFMT_PRECARG;
        }
        إذا(precision >= MAX_PARAMETERS)
          أرجع PFMT_MANYARGS;
        إذا(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      إذا(param < 0)
        param = param_رقم++;
      إذا(param >= MAX_PARAMETERS)
        أرجع PFMT_MANYARGS;
      إذا(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      إذا(ocount > MAX_SEGMENTS)
        أرجع PFMT_MANYSEGS;
      optr->مدخل = (unsigned صحيح)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    وإلا
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  إذا(outlen) {
    optr = &out[ocount++];
    إذا(ocount > MAX_SEGMENTS)
      أرجع PFMT_MANYSEGS;
    optr->مدخل = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  لكل(i = 0; i < max_param + 1; i++) {
    بنية va_مدخل *iptr = &in[i];
    إذا(!is_arg_used(usedinput, i))
      /* bad input */
      أرجع PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    بدل(iptr->type) {
    حالة MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ثابت حرف *);
      اكسر;

    حالة MTYPE_INTPTR:
    حالة MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, فراغ *);
      اكسر;

    حالة MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      اكسر;

    حالة MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      اكسر;

    حالة MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      اكسر;

    حالة MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      اكسر;

    حالة MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned صحيح);
      اكسر;

    حالة MTYPE_INT:
    حالة MTYPE_WIDTH:
    حالة MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, صحيح);
      اكسر;

    حالة MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, مزدوج);
      اكسر;

    حالة MTYPE_LONGDOUBLE:
      iptr->val.dnum = (مزدوج)va_arg(arglist, long مزدوج);
      اكسر;

    افتراضي:
      DEBUGASSERT(عدم); /* unexpected */
      اكسر;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  أرجع PFMT_OK;
}

بنية mproperty {
  صحيح width;            /* Width of a field. */
  صحيح prec;             /* Precision of a field. */
  unsigned صحيح flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
تعريفنوع size_t (*mp_streamn)(ثابت unsigned حرف *buf, size_t len, فراغ *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
تعريفنوع صحيح (*mp_flush)(فراغ *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
ساكن bool stream_run(فراغ *userp,
                       صحيح (*stream)(unsigned حرف, فراغ *),
                       mp_streamn streamn,
                       ثابت unsigned حرف *buf, size_t len, صحيح *donep)
{
  إذا(!len)
    أرجع FALSE;
  إذا(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (صحيح)n;
    أرجع n != len;
  }
  طالما(len--) {
    إذا(stream(*buf++, userp))
      أرجع TRUE;
    (*donep)++;
  }
  أرجع FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
ساكن ثابت حرف pad_spaces[] = "                ";
ساكن ثابت حرف pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
ساكن bool stream_pad(فراغ *userp,
                       صحيح (*stream)(unsigned حرف, فراغ *),
                       mp_streamn streamn,
                       unsigned حرف pad, صحيح رقم, صحيح *donep)
{
  ثابت unsigned حرف *src;
  إذا(رقم <= 0)
    أرجع FALSE;
  إذا(pad == '0')
    src = (ثابت unsigned حرف *)pad_zeros;
  وإلا
    src = (ثابت unsigned حرف *)pad_spaces;
  طالما(رقم > PAD_RUN) {
    إذا(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      أرجع TRUE;
    رقم -= PAD_RUN;
  }
  أرجع stream_run(userp, stream, streamn, src, (size_t)رقم, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
ساكن bool stream_zrun(فراغ *userp,
                        صحيح (*stream)(unsigned حرف, فراغ *),
                        mp_streamn streamn,
                        ثابت حرف *str, size_t len, صحيح *donep)
{
  size_t n = 0;
  طالما(n < len && str[n])
    n++;
  أرجع stream_run(userp, stream, streamn,
                    (ثابت unsigned حرف *)str, n, donep);
}

ساكن bool out_double(فراغ *userp,
                       صحيح (*stream)(unsigned حرف, فراغ *),
                       mp_streamn streamn,
                       بنية mproperty *p,
                       مزدوج dnum,
                       حرف *work, صحيح *donep)
{
  حرف fmt[32] = "%";
  حرف *fptr = &fmt[1];
  size_t يسار = حجم(fmt) - strlen(fmt);
  صحيح flags = p->flags;
  صحيح width = p->width;
  صحيح prec = p->prec;

  إذا(flags & FLAGS_يسار)
    *fptr++ = '-';
  إذا(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  إذا(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  إذا(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  إذا(width >= 0) {
    size_t dlen;
    إذا(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, يسار, "%d", width);
    fptr += dlen;
    يسار -= dlen;
  }
  إذا(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    صحيح maxprec = BUFFSIZE - 1;
    مزدوج val = dnum;
    صحيح len;
    إذا(prec > maxprec)
      prec = maxprec - 1;
    إذا(width > 0 && prec <= width)
      maxprec -= width;
    طالما(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    إذا(prec > maxprec)
      prec = maxprec - 1;
    إذا(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, يسار, ".%d", prec);
    fptr += len;
  }
  إذا(flags & FLAGS_LONG)
    *fptr++ = 'l';

  إذا(flags & FLAGS_FLOATE)
    *fptr++ = (حرف)((flags & FLAGS_UPPER) ? 'E' : 'e');
  وإلا إذا(flags & FLAGS_FLOATG)
    *fptr++ = (حرف)((flags & FLAGS_UPPER) ? 'G' : 'g');
  وإلا
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ادفع
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#وإلا
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic اسحب
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  أرجع stream_run(userp, stream, streamn,
                    (ثابت unsigned حرف *)work, strlen(work), donep);
}

ساكن bool out_رقم(فراغ *userp,
                       صحيح (*stream)(unsigned حرف, فراغ *),
                       mp_streamn streamn,
                       بنية mproperty *p,
                       uint64_t رقم,
                       int64_t nums,
                       حرف *work, صحيح *donep)
{
  ثابت unsigned حرف *digits = Curl_ldigits;
  صحيح flags = p->flags;
  صحيح width = p->width;
  صحيح prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  صحيح base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  حرف *workend = &work[BUFFSIZE - 2];
  حرف *w;

  إذا(flags & FLAGS_CHAR) {
    /* Character. */
    إذا(!(flags & FLAGS_يسار)) {
      إذا(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        أرجع TRUE;
      width = 1;
    }
    OUTCHAR((حرف)رقم);
    إذا(flags & FLAGS_يسار) {
      إذا(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        أرجع TRUE;
    }
    أرجع FALSE;
  }
  إذا(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  وإلا إذا(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  وإلا إذا(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  وإلا {
    /* Decimal integer. */
    is_neg = (nums < 0);
    إذا(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_رقم; /* Used to convert negative in positive. */
      signed_رقم = nums + (int64_t)1;
      signed_رقم = -signed_رقم;
      رقم = (uint64_t)signed_رقم;
      رقم += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  إذا(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  بدل(base) {
  حالة 10:
    طالما(رقم > 0) {
      *w-- = (حرف)('0' + (رقم % 10));
      رقم /= 10;
    }
    اكسر;
  افتراضي:
    طالما(رقم > 0) {
      *w-- = digits[رقم % base];
      رقم /= base;
    }
    اكسر;
  }
  width -= (صحيح)(workend - w);
  prec -= (صحيح)(workend - w);

  إذا(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  إذا(prec > 0) {
    width -= prec;
    طالما(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  إذا(is_alt && base == 16)
    width -= 2;

  إذا(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  إذا(!(flags & FLAGS_يسار) && !(flags & FLAGS_PAD_NIL)) {
    إذا(stream_pad(userp, stream, streamn, ' ', width, donep))
      أرجع TRUE;
    width = 0;
  }

  إذا(is_neg)
    OUTCHAR('-');
  وإلا إذا(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  وإلا إذا(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  إذا(is_alt && base == 16) {
    OUTCHAR('0');
    إذا(flags & FLAGS_UPPER)
      OUTCHAR('X');
    وإلا
      OUTCHAR('x');
  }

  إذا(!(flags & FLAGS_يسار) && (flags & FLAGS_PAD_NIL)) {
    إذا(stream_pad(userp, stream, streamn, '0', width, donep))
      أرجع TRUE;
    width = 0;
  }

  /* Write the number. */
  إذا(stream_run(userp, stream, streamn, (ثابت unsigned حرف *)w + 1,
                (size_t)(workend - w), donep))
    أرجع TRUE;

  إذا(flags & FLAGS_يسار) {
    إذا(stream_pad(userp, stream, streamn, ' ', width, donep))
      أرجع TRUE;
  }

  أرجع FALSE;
}

ساكن ثابت حرف nilstr[] = "(nil)";

ساكن bool out_string(فراغ *userp,
                       صحيح (*stream)(unsigned حرف, فراغ *),
                       mp_streamn streamn,
                       بنية mproperty *p,
                       ثابت حرف *str,
                       صحيح *donep)
{
  صحيح flags = p->flags;
  صحيح width = p->width;
  صحيح prec = p->prec;
  size_t len;

  إذا(!str) {
    /* Write null string if there is space. */
    إذا(prec == -1 || prec >= (صحيح)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned صحيح)FLAGS_ALT;
    }
    وإلا {
      str = "";
      len = 0;
    }
  }
  وإلا إذا(prec != -1)
    len = (size_t)prec;
  وإلا إذا(*str == '\0')
    len = 0;
  وإلا
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (صحيح)len;

  إذا(flags & FLAGS_ALT)
    OUTCHAR('"');

  إذا(!(flags & FLAGS_يسار)) {
    إذا(stream_pad(userp, stream, streamn, ' ', width, donep))
      أرجع TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  إذا(prec == -1) {
    إذا(stream_run(userp, stream, streamn,
                  (ثابت unsigned حرف *)str, len, donep))
      أرجع TRUE;
  }
  وإلا إذا(stream_zrun(userp, stream, streamn, str, len, donep))
    أرجع TRUE;
  إذا(flags & FLAGS_يسار) {
    إذا(stream_pad(userp, stream, streamn, ' ', width, donep))
      أرجع TRUE;
  }

  إذا(flags & FLAGS_ALT)
    OUTCHAR('"');

  أرجع FALSE;
}

ساكن bool out_pointer(فراغ *userp,
                        صحيح (*stream)(unsigned حرف, فراغ *),
                        mp_streamn streamn,
                        بنية mproperty *p,
                        ثابت حرف *ptr,
                        حرف *work,
                        صحيح *donep)
{
  /* Generic pointer. */
  إذا(ptr) {
    size_t رقم = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    إذا(out_رقم(userp, stream, streamn, p, رقم, 0, work, donep))
      أرجع TRUE;
  }
  وإلا {
    /* Write "(nil)" for a nil pointer. */
    صحيح width = p->width;
    صحيح flags = p->flags;

    width -= (صحيح)CURL_CSTRLEN(nilstr);
    إذا(flags & FLAGS_يسار) {
      إذا(stream_pad(userp, stream, streamn, ' ', width, donep))
        أرجع TRUE;
      width = 0;
    }
    إذا(stream_run(userp, stream, streamn, (ثابت unsigned حرف *)nilstr,
                  حجم(nilstr) - 1, donep))
      أرجع TRUE;
    إذا(!(flags & FLAGS_يسار)) {
      إذا(stream_pad(userp, stream, streamn, ' ', width, donep))
        أرجع TRUE;
    }
  }
  أرجع FALSE;
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
ساكن صحيح formatf(فراغ *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   صحيح (*stream)(unsigned حرف, فراغ *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ثابت حرف *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  صحيح done = 0;   /* number of characters written */
  صحيح i;
  صحيح ocount = 0; /* number of output segments */
  صحيح icount = 0; /* number of input arguments */

  بنية outsegment output[MAX_SEGMENTS];
  بنية va_مدخل مدخل[MAX_PARAMETERS];
  حرف work[BUFFSIZE + 2];

  /* Parse the format string */
  إذا(parsefmt(format, output, مدخل, &ocount, &icount, ap_save))
    أرجع 0;

  لكل(i = 0; i < ocount; i++) {
    بنية outsegment *optr = &output[i];
    بنية va_مدخل *iptr = &مدخل[optr->مدخل];
    بنية mproperty p;
    size_t outlen = optr->outlen;

    إذا(outlen) {
      إذا(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        أرجع done;
      إذا(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        استمر;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    إذا(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (صحيح)مدخل[optr->width].val.nums;
      إذا(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        إذا(p.width == INT_MIN)
          p.width = INT_MAX;
        وإلا
          p.width = -p.width;
        p.flags |= FLAGS_يسار;
        p.flags &= ~(unsigned صحيح)FLAGS_PAD_NIL;
      }
    }
    وإلا
      p.width = optr->width;

    /* pick up the specified precision */
    إذا(p.flags & FLAGS_PRECPARAM) {
      p.prec = (صحيح)مدخل[optr->precision].val.nums;
      إذا(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    وإلا إذا(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    وإلا
      p.prec = -1;

    بدل(iptr->type) {
    حالة MTYPE_INTU:
    حالة MTYPE_LONGU:
    حالة MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      إذا(out_رقم(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        أرجع done;
      اكسر;

    حالة MTYPE_INT:
    حالة MTYPE_LONG:
    حالة MTYPE_LONGLONG:
      إذا(out_رقم(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        أرجع done;
      اكسر;

    حالة MTYPE_STRING:
      إذا(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        أرجع done;
      اكسر;

    حالة MTYPE_PTR:
      إذا(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        أرجع done;
      اكسر;

    حالة MTYPE_DOUBLE:
    حالة MTYPE_LONGDOUBLE:
      إذا(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        أرجع done;
      اكسر;

    حالة MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      إذا(flush && flush(userp))
        أرجع done;
      /* Answer the count of characters written. */
      إذا(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      وإلا إذا(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      وإلا إذا(!(p.flags & FLAGS_SHORT))
        *(صحيح *)iptr->val.ptr = done;
      وإلا
        *(short *)iptr->val.ptr = (short)done;
      اكسر;

    افتراضي:
      اكسر;
    }
  }
  أرجع done;
}

/* fputc() look-alike */
ساكن صحيح addbyter(unsigned حرف outc, فراغ *f)
{
  بنية nsprintf *infop = f;
  إذا(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (حرف)outc; /* store */
    infop->length++; /* we are now one byte larger */
    أرجع 0;     /* fputc() returns like this on success */
  }
  أرجع 1;
}

/* block variant of addbyter */
ساكن size_t addrun(ثابت unsigned حرف *buf, size_t len, فراغ *f)
{
  بنية nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  إذا(len > fit)
    len = fit;
  إذا(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  أرجع len;
}

صحيح curl_mvsnprintf(حرف *buffer, size_t maxlength, ثابت حرف *format,
                    va_list وسائط)
{
  صحيح retcode;
  بنية nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, عدم, format, وسائط);
  إذا(info.max) {
    /* we null-terminate this */
    إذا(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    وإلا
      info.buffer[0] = 0;
  }
  أرجع retcode;
}

صحيح curl_msnprintf(حرف *buffer, size_t maxlength, ثابت حرف *format, ...)
{
  صحيح retcode;
  va_list وسائط; /* argument pointer */
  va_start(وسائط, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, وسائط);
  va_end(وسائط);
  أرجع retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
ساكن صحيح alloc_flush(فراغ *f)
{
  بنية asprintf *infop = f;
  إذا(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    إذا(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      أرجع 1; /* fail */
    }
  }
  أرجع 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
ساكن صحيح alloc_addn(بنية asprintf *infop, ثابت unsigned حرف *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  إذا(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    أرجع 1; /* fail */
  }
  أرجع 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
ساكن bool alloc_fits(بنية asprintf *infop, size_t more)
{
  أرجع (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
ساكن صحيح alloc_addbyter(unsigned حرف outc, فراغ *f)
{
  بنية asprintf *infop = f;
  إذا(infop->nstage == حجم(infop->stage) && alloc_flush(infop))
    أرجع 1; /* fail */
  إذا(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    إذا(alloc_flush(infop))
      أرجع 1;
    أرجع alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  أرجع 0;
}

/* block variant of alloc_addbyter */
ساكن size_t alloc_addrun(ثابت unsigned حرف *buf, size_t len, فراغ *f)
{
  بنية asprintf *infop = f;
  size_t accepted = 0;
  طالما(accepted < len) {
    size_t room = حجم(infop->stage) - infop->nstage;
    size_t n;
    إذا(!room) {
      إذا(alloc_flush(infop))
        أرجع accepted; /* accepted < len aborts formatting */
      room = حجم(infop->stage);
    }
    n = len - accepted;
    إذا(n > room)
      n = room;
    إذا(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      إذا(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      إذا(alloc_flush(infop))
        أرجع accepted;
      إذا(alloc_addn(infop, buf + accepted, 1))
        أرجع accepted; /* accepted < len aborts formatting */
      accepted++;
      استمر;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  أرجع accepted;
}

/* appends the formatted string, returns MERR error code */
صحيح curlx_dyn_vprintf(بنية dynbuf *dyn, ثابت حرف *format, va_list وسائط)
{
  بنية asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (فراغ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, وسائط);
  إذا(!info.merr)
    (فراغ)alloc_flush(&info);
  إذا(info.merr) {
    curlx_dyn_free(info.b);
    أرجع info.merr;
  }
  أرجع 0;
}

حرف *curl_mvaprintf(ثابت حرف *format, va_list وسائط)
{
  بنية asprintf info;
  بنية dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (فراغ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, وسائط);
  إذا(!info.merr)
    (فراغ)alloc_flush(&info);
  إذا(info.merr) {
    curlx_dyn_free(info.b);
    أرجع عدم;
  }
  إذا(curlx_dyn_len(info.b))
    أرجع curlx_dyn_ptr(info.b);
  أرجع curlx_strdup("");
}

حرف *curl_maprintf(ثابت حرف *format, ...)
{
  va_list وسائط;
  حرف *s;
  va_start(وسائط, format);
  s = curl_mvaprintf(format, وسائط);
  va_end(وسائط);
  أرجع s;
}

ساكن صحيح storebuffer(unsigned حرف outc, فراغ *f)
{
  حرف **buffer = f;
  **buffer = (حرف)outc;
  (*buffer)++;
  أرجع 0;
}

/* block variant of storebuffer */
ساكن size_t storerun(ثابت unsigned حرف *buf, size_t len, فراغ *f)
{
  حرف **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  أرجع len;
}

صحيح curl_msprintf(حرف *buffer, ثابت حرف *format, ...)
{
  va_list وسائط; /* argument pointer */
  صحيح retcode;
  va_start(وسائط, format);
  retcode = formatf(&buffer, storebuffer, storerun, عدم, format, وسائط);
  va_end(وسائط);
  *buffer = 0; /* we null-terminate this */
  أرجع retcode;
}

ساكن صحيح fputc_wrapper(unsigned حرف outc, فراغ *f)
{
  بنية ioprintf *i = f;
  صحيح out = outc;
  FILE *s = i->fd;
  صحيح rc = fputc(out, s);
  i->error = (rc == EOF);
  أرجع (rc == EOF);
}

/* block variant of fputc_wrapper */
ساكن size_t fwrite_wrapper(ثابت unsigned حرف *buf, size_t len, فراغ *f)
{
  بنية ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  أرجع n;
}

صحيح curl_mvfprintf(FILE *fd, ثابت حرف *format, va_list وسائط)
{
  بنية ioprintf info;
  صحيح n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, عدم, format, وسائط);
  إذا(info.error)
    أرجع -1;
  أرجع n;
}

صحيح curl_mfprintf(FILE *fd, ثابت حرف *format, ...)
{
  va_list وسائط; /* argument pointer */
  صحيح n;
  va_start(وسائط, format);
  n = curl_mvfprintf(fd, format, وسائط);
  va_end(وسائط);
  أرجع n;
}

صحيح curl_mprintf(ثابت حرف *format, ...)
{
  va_list وسائط; /* argument pointer */
  صحيح n;
  va_start(وسائط, format);
  n = curl_mvfprintf(stdout, format, وسائط);
  va_end(وسائط);
  أرجع n;
}

صحيح curl_mvsprintf(حرف *buffer, ثابت حرف *format, va_list وسائط)
{
  صحيح retcode = formatf(&buffer, storebuffer, storerun, عدم, format, وسائط);
  *buffer = 0; /* we null-terminate this */
  أرجع retcode;
}

صحيح curl_mvprintf(ثابت حرف *format, va_list وسائط)
{
  أرجع curl_mvfprintf(stdout, format, وسائط);
}

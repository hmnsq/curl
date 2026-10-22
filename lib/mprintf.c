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

#وصف_ڪريو BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#وصف_ڪريو MAX_PARAMETERS 128 /* number of input arguments */
#وصف_ڪريو MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ثابت بنانشان حرف Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ثابت بنانشان حرف Curl_udigits[] = "0123456789ABCDEF";

#وصف_ڪريو OUTCHAR(x)                        \
  افعل {                                    \
    إذا(وهڪرو((بنانشان حرف)(x), userp)) \
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
  MTYPE_ويڪر,
  MTYPE_PRECISION
} FormatType;

/* conversion and display flags */
تعداد {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_کاٻو       = 1 << 2,
  FLAGS_ALT        = 1 << 3,
  FLAGS_SHORT      = 1 << 4,
  FLAGS_LONG       = 1 << 5,
  FLAGS_LONGLONG   = 1 << 6,
  FLAGS_LONGDOUBLE = 1 << 7,
  FLAGS_PAD_NIL    = 1 << 8,
  FLAGS_UNSIGNED   = 1 << 9,
  FLAGS_OCTAL      = 1 << 10,
  FLAGS_HEX        = 1 << 11,
  FLAGS_مٿانهون      = 1 << 12,
  FLAGS_ويڪر      = 1 << 13, /* '*' or '*<num>$' used */
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
بنية va_داخلا {
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
  صحيح ويڪر;     /* width OR width parameter number */
  صحيح precision; /* precision OR precision parameter number */
  بنانشان صحيح flags;
  بنانشان صحيح داخلا; /* input argument array index */
  ثابت حرف *شروعات; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

بنية nsprintf {
  حرف *بفر;
  size_t ڊيگهائي;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#وصف_ڪريو ASPRINTF_STAGE_سائيز 512

بنية asprintf {
  بنية dynbuf *b;
  حرف merr;
  size_t nstage;
  بنانشان حرف stage[ASPRINTF_STAGE_سائيز];
};

بنية ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
ساكن صحيح dollarstring(ثابت حرف *p, ثابت حرف **پڇاڙي)
{
  curl_off_t انگ;
  إذا(curlx_str_عدد(&p, &انگ, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !انگ)
    أرجع -1;
  *پڇاڙي = p;
  أرجع (صحيح)انگ - 1;
}

#وصف_ڪريو is_arg_استعمال_ٿيل(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#وصف_ڪريو mark_arg_استعمال_ٿيل(x, y) ((x)[(y) / 8] |= (بنانشان حرف)(1 << ((y) & 7)))

/*
 * Parse the format string.
 *
 * Create two arrays. One describes the inputs, one describes the outputs.
 *
 * Returns zero on success.
 */

#وصف_ڪريو PFMT_OK          0
#وصف_ڪريو PFMT_DOLLAR      1 /* bad dollar for main param */
#وصف_ڪريو PFMT_DOLLARWIDTH 2 /* bad dollar use for width */
#وصف_ڪريو PFMT_DOLLARPREC  3 /* bad dollar use for precision */
#وصف_ڪريو PFMT_MANYARGS    4 /* too many input arguments used */
#وصف_ڪريو PFMT_PREC        5 /* precision overflow */
#وصف_ڪريو PFMT_PRECMIX     6 /* bad mix of precision specifiers */
#وصف_ڪريو PFMT_ويڪر       7 /* width overflow */
#وصف_ڪريو PFMT_INPUTGAP    8 /* gap in arguments */
#وصف_ڪريو PFMT_WIDTHARG    9 /* attempted to use same arg twice, for width */
#وصف_ڪريو PFMT_PRECARG    10 /* attempted to use same arg twice, for prec */
#وصف_ڪريو PFMT_MANYSEGS   11 /* maxed out output segments */

ساكن صحيح parse_flags(ثابت حرف **fmtp, بنانشان صحيح *flagsp, صحيح use_dollar,
                       صحيح *precp, صحيح *widthp)
{
  ثابت حرف *fmt = *fmtp;
  bool loopit = TRUE;
  بنانشان صحيح flags = 0;
  صحيح ويڪر = 0;
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
      flags |= FLAGS_کاٻو;
      flags &= ~(بنانشان صحيح)FLAGS_PAD_NIL;
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
        curl_off_t انگ;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        إذا(is_neg)
          fmt++;
        إذا(curlx_str_عدد(&fmt, &انگ, وڌ_کان_وڌ_عدد))
          أرجع PFMT_PREC;
        precision = (صحيح)انگ;
        إذا(is_neg)
          precision = -precision;
      }
      إذا((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
دليل*/
        أرجع PFMT_PRECMIX;
      اكسر;
    حالة 'h':
      flags |= FLAGS_SHORT;
      اكسر;
#جيڪڏهن_وصف_ٿيل _WIN32
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
#إذا SIZEOF_سائيز_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#وإلا
        flags |= FLAGS_LONG;
#جيڪڏهن_پورو
      }
      اكسر;
#جيڪڏهن_پورو /* _WIN32 */
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
استعمال_ٿيل*/
#إذا SIZEOF_سائيز_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#وإلا
      flags |= FLAGS_LONG;
#جيڪڏهن_پورو
      اكسر;
    حالة 'O':
#إذا SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#وإلا
      flags |= FLAGS_LONG;
#جيڪڏهن_پورو
      اكسر;
    حالة '0':
      إذا(!(flags & FLAGS_کاٻو))
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
      curl_off_t انگ;
      flags |= FLAGS_ويڪر;
      fmt--;
      إذا(curlx_str_عدد(&fmt, &انگ, وڌ_کان_وڌ_عدد))
        أرجع PFMT_ويڪر;
      ويڪر = (صحيح)انگ;
      اكسر;
    }
    حالة '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      إذا(use_dollar == DOLLAR_USE) {
        ويڪر = dollarstring(fmt, &fmt);
        إذا(ويڪر < 0)
          /* illegal combo */
          أرجع PFMT_DOLLARWIDTH;
      }
      وإلا
        /* pick from the next argument */
        ويڪر = -1;
      اكسر;
    افتراضي:
      loopit = FALSE;
      fmt--;
      اكسر;
    } /* switch */
  } طالما(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = ويڪر;
  *fmtp = fmt;
  أرجع PFMT_OK;
}

ساكن bool parse_conversion(ثابت حرف f, بنانشان صحيح *flagp,
                             FormatType *typep)
{
  بنانشان صحيح flags = *flagp;
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
    flags |= FLAGS_HEX | FLAGS_مٿانهون | FLAGS_UNSIGNED;
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
    flags |= FLAGS_مٿانهون;
    اكسر;
  حالة 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    اكسر;
  حالة 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_مٿانهون;
    اكسر;
  حالة 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    اكسر;
  حالة 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_مٿانهون;
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
                    بنية outsegment *ٻاهريون,
                    بنية va_داخلا *اندريون,
                    صحيح *opieces,
                    صحيح *ipieces, va_list arglist)
{
  ثابت حرف *fmt = format;
  صحيح param_انگ = 0;
  صحيح max_param = -1;
  صحيح i;
  صحيح ocount = 0;
  بنانشان حرف usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  بنية outsegment *optr;
  صحيح use_dollar = DOLLAR_UNKNOWN;
  ثابت حرف *شروعات = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, حجم(usedinput));

  طالما(*fmt) {
    إذا(*fmt == '%') {
      بنية va_داخلا *iptr;
      FormatType type;
      بنانشان صحيح flags = 0;
      صحيح ويڪر = 0;
      صحيح precision = 0;
      صحيح param = -1;
      صحيح rc;
      fmt++;
      outlen = (size_t)(fmt - شروعات - 1);
      إذا(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        إذا(outlen) {
          optr = &ٻاهريون[ocount++];
          إذا(ocount > MAX_SEGMENTS)
            أرجع PFMT_MANYSEGS;
          optr->داخلا = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->شروعات = شروعات;
          optr->outlen = outlen;
        }
        شروعات = fmt;
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

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &ويڪر);
      إذا(rc)
        أرجع rc;

      إذا(parse_conversion(*fmt, &flags, &type))
        استمر;

      إذا(flags & FLAGS_WIDTHPARAM) {
        إذا(ويڪر < 0)
          ويڪر = param_انگ++;
        وإلا {
          /* if this identifies a parameter already used, this is illegal */
          إذا(is_arg_استعمال_ٿيل(usedinput, ويڪر))
            أرجع PFMT_WIDTHARG;
        }
        إذا(ويڪر >= MAX_PARAMETERS)
          أرجع PFMT_MANYARGS;
        إذا(ويڪر >= max_param)
          max_param = ويڪر;

        اندريون[ويڪر].type = MTYPE_ويڪر;
        /* mark as used */
        mark_arg_استعمال_ٿيل(usedinput, ويڪر);
      }

      إذا(flags & FLAGS_PRECPARAM) {
        إذا(precision < 0)
          precision = param_انگ++;
        وإلا {
          /* if this identifies a parameter already used, this is illegal */
          إذا(is_arg_استعمال_ٿيل(usedinput, precision))
            أرجع PFMT_PRECARG;
        }
        إذا(precision >= MAX_PARAMETERS)
          أرجع PFMT_MANYARGS;
        إذا(precision >= max_param)
          max_param = precision;

        اندريون[precision].type = MTYPE_PRECISION;
        mark_arg_استعمال_ٿيل(usedinput, precision);
      }

      /* Handle the specifier */
      إذا(param < 0)
        param = param_انگ++;
      إذا(param >= MAX_PARAMETERS)
        أرجع PFMT_MANYARGS;
      إذا(param >= max_param)
        max_param = param;

      iptr = &اندريون[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_استعمال_ٿيل(usedinput, param);

      fmt++;
      optr = &ٻاهريون[ocount++];
      إذا(ocount > MAX_SEGMENTS)
        أرجع PFMT_MANYSEGS;
      optr->داخلا = (بنانشان صحيح)param;
      optr->flags = flags;
      optr->ويڪر = ويڪر;
      optr->precision = precision;
      optr->شروعات = شروعات;
      optr->outlen = outlen;
      شروعات = fmt;
    }
    وإلا
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - شروعات);
  إذا(outlen) {
    optr = &ٻاهريون[ocount++];
    إذا(ocount > MAX_SEGMENTS)
      أرجع PFMT_MANYSEGS;
    optr->داخلا = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->شروعات = شروعات;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  لكل(i = 0; i < max_param + 1; i++) {
    بنية va_داخلا *iptr = &اندريون[i];
    إذا(!is_arg_استعمال_ٿيل(usedinput, i))
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
      iptr->val.numu = va_arg(arglist, بنانشان ڊگهو);
      اكسر;

    حالة MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, ڊگهو);
      اكسر;

    حالة MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, بنانشان صحيح);
      اكسر;

    حالة MTYPE_INT:
    حالة MTYPE_ويڪر:
    حالة MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, صحيح);
      اكسر;

    حالة MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, مزدوج);
      اكسر;

    حالة MTYPE_LONGDOUBLE:
      iptr->val.dnum = (مزدوج)va_arg(arglist, ڊگهو مزدوج);
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
  صحيح ويڪر;            /* Width of a field. */
  صحيح prec;             /* Precision of a field. */
  بنانشان صحيح flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
تعريفنوع size_t (*mp_streamn)(ثابت بنانشان حرف *buf, size_t len, فراغ *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
تعريفنوع صحيح (*mp_flush)(فراغ *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
ساكن bool وهڪرو_لاڳيتو(فراغ *userp,
                       صحيح (*وهڪرو)(بنانشان حرف, فراغ *),
                       mp_streamn streamn,
                       ثابت بنانشان حرف *buf, size_t len, صحيح *donep)
{
  إذا(!len)
    أرجع FALSE;
  إذا(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (صحيح)n;
    أرجع n != len;
  }
  طالما(len--) {
    إذا(وهڪرو(*buf++, userp))
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
#وصف_ڪريو PAD_لاڳيتو 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
ساكن bool وهڪرو_pad(فراغ *userp,
                       صحيح (*وهڪرو)(بنانشان حرف, فراغ *),
                       mp_streamn streamn,
                       بنانشان حرف pad, صحيح انگ, صحيح *donep)
{
  ثابت بنانشان حرف *src;
  إذا(انگ <= 0)
    أرجع FALSE;
  إذا(pad == '0')
    src = (ثابت بنانشان حرف *)pad_zeros;
  وإلا
    src = (ثابت بنانشان حرف *)pad_spaces;
  طالما(انگ > PAD_لاڳيتو) {
    إذا(وهڪرو_لاڳيتو(userp, وهڪرو, streamn, src, PAD_لاڳيتو, donep))
      أرجع TRUE;
    انگ -= PAD_لاڳيتو;
  }
  أرجع وهڪرو_لاڳيتو(userp, وهڪرو, streamn, src, (size_t)انگ, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
ساكن bool وهڪرو_zrun(فراغ *userp,
                        صحيح (*وهڪرو)(بنانشان حرف, فراغ *),
                        mp_streamn streamn,
                        ثابت حرف *str, size_t len, صحيح *donep)
{
  size_t n = 0;
  طالما(n < len && str[n])
    n++;
  أرجع وهڪرو_لاڳيتو(userp, وهڪرو, streamn,
                    (ثابت بنانشان حرف *)str, n, donep);
}

ساكن bool ٻاهريون_double(فراغ *userp,
                       صحيح (*وهڪرو)(بنانشان حرف, فراغ *),
                       mp_streamn streamn,
                       بنية mproperty *p,
                       مزدوج dnum,
                       حرف *work, صحيح *donep)
{
  حرف fmt[32] = "%";
  حرف *fptr = &fmt[1];
  size_t کاٻو = حجم(fmt) - لڙي_ڊيگهه(fmt);
  صحيح flags = p->flags;
  صحيح ويڪر = p->ويڪر;
  صحيح prec = p->prec;

  إذا(flags & FLAGS_کاٻو)
    *fptr++ = '-';
  إذا(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  إذا(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  إذا(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  إذا(ويڪر >= 0) {
    size_t dlen;
    إذا(ويڪر >= BUFFSIZE)
      ويڪر = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, کاٻو, "%d", ويڪر);
    fptr += dlen;
    کاٻو -= dlen;
  }
  إذا(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    صحيح maxprec = BUFFSIZE - 1;
    مزدوج val = dnum;
    صحيح len;
    إذا(prec > maxprec)
      prec = maxprec - 1;
    إذا(ويڪر > 0 && prec <= ويڪر)
      maxprec -= ويڪر;
    طالما(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    إذا(prec > maxprec)
      prec = maxprec - 1;
    إذا(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, کاٻو, ".%d", prec);
    fptr += len;
  }
  إذا(flags & FLAGS_LONG)
    *fptr++ = 'l';

  إذا(flags & FLAGS_FLOATE)
    *fptr++ = (حرف)((flags & FLAGS_مٿانهون) ? 'E' : 'e');
  وإلا إذا(flags & FLAGS_FLOATG)
    *fptr++ = (حرف)((flags & FLAGS_مٿانهون) ? 'G' : 'g');
  وإلا
    *fptr++ = (flags & FLAGS_مٿانهون) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#جيڪڏهن_وصف_ٿيل CURL_HAVE_DIAG
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wformat-nonliteral"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#وإلا
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURL_HAVE_DIAG
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو
  DEBUGASSERT(لڙي_ڊيگهه(work) < BUFFSIZE);
  أرجع وهڪرو_لاڳيتو(userp, وهڪرو, streamn,
                    (ثابت بنانشان حرف *)work, لڙي_ڊيگهه(work), donep);
}

ساكن bool ٻاهريون_عدد(فراغ *userp,
                       صحيح (*وهڪرو)(بنانشان حرف, فراغ *),
                       mp_streamn streamn,
                       بنية mproperty *p,
                       uint64_t انگ,
                       int64_t nums,
                       حرف *work, صحيح *donep)
{
  ثابت بنانشان حرف *digits = Curl_ldigits;
  صحيح flags = p->flags;
  صحيح ويڪر = p->ويڪر;
  صحيح prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  صحيح بنياد = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  حرف *workend = &work[BUFFSIZE - 2];
  حرف *w;

  إذا(flags & FLAGS_CHAR) {
    /* Character. */
    إذا(!(flags & FLAGS_کاٻو)) {
      إذا(وهڪرو_pad(userp, وهڪرو, streamn, ' ', ويڪر - 1, donep))
        أرجع TRUE;
      ويڪر = 1;
    }
    OUTCHAR((حرف)انگ);
    إذا(flags & FLAGS_کاٻو) {
      إذا(وهڪرو_pad(userp, وهڪرو, streamn, ' ', ويڪر - 1, donep))
        أرجع TRUE;
    }
    أرجع FALSE;
  }
  إذا(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    بنياد = 8;

  وإلا إذا(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_مٿانهون) ? Curl_udigits : Curl_ldigits;
    بنياد = 16;
  }
  وإلا إذا(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  وإلا {
    /* Decimal integer. */
    is_neg = (nums < 0);
    إذا(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_انگ; /* Used to convert negative in positive. */
      signed_انگ = nums + (int64_t)1;
      signed_انگ = -signed_انگ;
      انگ = (uint64_t)signed_انگ;
      انگ += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  إذا(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(بنياد <= 16);
  بدل(بنياد) {
  حالة 10:
    طالما(انگ > 0) {
      *w-- = (حرف)('0' + (انگ % 10));
      انگ /= 10;
    }
    اكسر;
  افتراضي:
    طالما(انگ > 0) {
      *w-- = digits[انگ % بنياد];
      انگ /= بنياد;
    }
    اكسر;
  }
  ويڪر -= (صحيح)(workend - w);
  prec -= (صحيح)(workend - w);

  إذا(is_alt && بنياد == 8 && prec <= 0) {
    *w-- = '0';
    --ويڪر;
  }

  إذا(prec > 0) {
    ويڪر -= prec;
    طالما(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  إذا(is_alt && بنياد == 16)
    ويڪر -= 2;

  إذا(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --ويڪر;

  إذا(!(flags & FLAGS_کاٻو) && !(flags & FLAGS_PAD_NIL)) {
    إذا(وهڪرو_pad(userp, وهڪرو, streamn, ' ', ويڪر, donep))
      أرجع TRUE;
    ويڪر = 0;
  }

  إذا(is_neg)
    OUTCHAR('-');
  وإلا إذا(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  وإلا إذا(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  إذا(is_alt && بنياد == 16) {
    OUTCHAR('0');
    إذا(flags & FLAGS_مٿانهون)
      OUTCHAR('X');
    وإلا
      OUTCHAR('x');
  }

  إذا(!(flags & FLAGS_کاٻو) && (flags & FLAGS_PAD_NIL)) {
    إذا(وهڪرو_pad(userp, وهڪرو, streamn, '0', ويڪر, donep))
      أرجع TRUE;
    ويڪر = 0;
  }

  /* Write the number. */
  إذا(وهڪرو_لاڳيتو(userp, وهڪرو, streamn, (ثابت بنانشان حرف *)w + 1,
                (size_t)(workend - w), donep))
    أرجع TRUE;

  إذا(flags & FLAGS_کاٻو) {
    إذا(وهڪرو_pad(userp, وهڪرو, streamn, ' ', ويڪر, donep))
      أرجع TRUE;
  }

  أرجع FALSE;
}

ساكن ثابت حرف nilstr[] = "(nil)";

ساكن bool ٻاهريون_string(فراغ *userp,
                       صحيح (*وهڪرو)(بنانشان حرف, فراغ *),
                       mp_streamn streamn,
                       بنية mproperty *p,
                       ثابت حرف *str,
                       صحيح *donep)
{
  صحيح flags = p->flags;
  صحيح ويڪر = p->ويڪر;
  صحيح prec = p->prec;
  size_t len;

  إذا(!str) {
    /* Write null string if there is space. */
    إذا(prec == -1 || prec >= (صحيح)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(بنانشان صحيح)FLAGS_ALT;
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
    len = لڙي_ڊيگهه(str);

  ويڪر -= (len > وڌ_کان_وڌ_عدد) ? وڌ_کان_وڌ_عدد : (صحيح)len;

  إذا(flags & FLAGS_ALT)
    OUTCHAR('"');

  إذا(!(flags & FLAGS_کاٻو)) {
    إذا(وهڪرو_pad(userp, وهڪرو, streamn, ' ', ويڪر, donep))
      أرجع TRUE;
    ويڪر = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  إذا(prec == -1) {
    إذا(وهڪرو_لاڳيتو(userp, وهڪرو, streamn,
                  (ثابت بنانشان حرف *)str, len, donep))
      أرجع TRUE;
  }
  وإلا إذا(وهڪرو_zrun(userp, وهڪرو, streamn, str, len, donep))
    أرجع TRUE;
  إذا(flags & FLAGS_کاٻو) {
    إذا(وهڪرو_pad(userp, وهڪرو, streamn, ' ', ويڪر, donep))
      أرجع TRUE;
  }

  إذا(flags & FLAGS_ALT)
    OUTCHAR('"');

  أرجع FALSE;
}

ساكن bool ٻاهريون_اشارو(فراغ *userp,
                        صحيح (*وهڪرو)(بنانشان حرف, فراغ *),
                        mp_streamn streamn,
                        بنية mproperty *p,
                        ثابت حرف *ptr,
                        حرف *work,
                        صحيح *donep)
{
  /* Generic pointer. */
  إذا(ptr) {
    size_t انگ = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    إذا(ٻاهريون_عدد(userp, وهڪرو, streamn, p, انگ, 0, work, donep))
      أرجع TRUE;
  }
  وإلا {
    /* Write "(nil)" for a nil pointer. */
    صحيح ويڪر = p->ويڪر;
    صحيح flags = p->flags;

    ويڪر -= (صحيح)CURL_CSTRLEN(nilstr);
    إذا(flags & FLAGS_کاٻو) {
      إذا(وهڪرو_pad(userp, وهڪرو, streamn, ' ', ويڪر, donep))
        أرجع TRUE;
      ويڪر = 0;
    }
    إذا(وهڪرو_لاڳيتو(userp, وهڪرو, streamn, (ثابت بنانشان حرف *)nilstr,
                  حجم(nilstr) - 1, donep))
      أرجع TRUE;
    إذا(!(flags & FLAGS_کاٻو)) {
      إذا(وهڪرو_pad(userp, وهڪرو, streamn, ' ', ويڪر, donep))
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
                   صحيح (*وهڪرو)(بنانشان حرف, فراغ *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ثابت حرف *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  صحيح پورو = 0;   /* number of characters written */
  صحيح i;
  صحيح ocount = 0; /* number of output segments */
  صحيح icount = 0; /* number of input arguments */

  بنية outsegment output[MAX_SEGMENTS];
  بنية va_داخلا داخلا[MAX_PARAMETERS];
  حرف work[BUFFSIZE + 2];

  /* Parse the format string */
  إذا(parsefmt(format, output, داخلا, &ocount, &icount, ap_save))
    أرجع 0;

  لكل(i = 0; i < ocount; i++) {
    بنية outsegment *optr = &output[i];
    بنية va_داخلا *iptr = &داخلا[optr->داخلا];
    بنية mproperty p;
    size_t outlen = optr->outlen;

    إذا(outlen) {
      إذا(وهڪرو_zrun(userp, وهڪرو, streamn, optr->شروعات, outlen, &پورو))
        أرجع پورو;
      إذا(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        استمر;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    إذا(p.flags & FLAGS_WIDTHPARAM) {
      p.ويڪر = (صحيح)داخلا[optr->ويڪر].val.nums;
      إذا(p.ويڪر < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        إذا(p.ويڪر == INT_MIN)
          p.ويڪر = وڌ_کان_وڌ_عدد;
        وإلا
          p.ويڪر = -p.ويڪر;
        p.flags |= FLAGS_کاٻو;
        p.flags &= ~(بنانشان صحيح)FLAGS_PAD_NIL;
      }
    }
    وإلا
      p.ويڪر = optr->ويڪر;

    /* pick up the specified precision */
    إذا(p.flags & FLAGS_PRECPARAM) {
      p.prec = (صحيح)داخلا[optr->precision].val.nums;
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
      إذا(ٻاهريون_عدد(userp, وهڪرو, streamn, &p, iptr->val.numu, 0, work,
                    &پورو))
        أرجع پورو;
      اكسر;

    حالة MTYPE_INT:
    حالة MTYPE_LONG:
    حالة MTYPE_LONGLONG:
      إذا(ٻاهريون_عدد(userp, وهڪرو, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &پورو))
        أرجع پورو;
      اكسر;

    حالة MTYPE_STRING:
      إذا(ٻاهريون_string(userp, وهڪرو, streamn, &p, iptr->val.str, &پورو))
        أرجع پورو;
      اكسر;

    حالة MTYPE_PTR:
      إذا(ٻاهريون_اشارو(userp, وهڪرو, streamn, &p, iptr->val.ptr, work,
                     &پورو))
        أرجع پورو;
      اكسر;

    حالة MTYPE_DOUBLE:
    حالة MTYPE_LONGDOUBLE:
      إذا(ٻاهريون_double(userp, وهڪرو, streamn, &p, iptr->val.dnum, work,
                    &پورو))
        أرجع پورو;
      اكسر;

    حالة MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      إذا(flush && flush(userp))
        أرجع پورو;
      /* Answer the count of characters written. */
      إذا(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)پورو;
      وإلا إذا(p.flags & FLAGS_LONG)
        *(ڊگهو *)iptr->val.ptr = (ڊگهو)پورو;
      وإلا إذا(!(p.flags & FLAGS_SHORT))
        *(صحيح *)iptr->val.ptr = پورو;
      وإلا
        *(ننڍو *)iptr->val.ptr = (ننڍو)پورو;
      اكسر;

    افتراضي:
      اكسر;
    }
  }
  أرجع پورو;
}

/* fputc() look-alike */
ساكن صحيح addbyter(بنانشان حرف outc, فراغ *f)
{
  بنية nsprintf *infop = f;
  إذا(infop->ڊيگهائي < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->بفر++ = (حرف)outc; /* store */
    infop->ڊيگهائي++; /* we are now one byte larger */
    أرجع 0;     /* fputc() returns like this on success */
  }
  أرجع 1;
}

/* block variant of addbyter */
ساكن size_t addrun(ثابت بنانشان حرف *buf, size_t len, فراغ *f)
{
  بنية nsprintf *infop = f;
  size_t fit = infop->max - infop->ڊيگهائي;
  إذا(len > fit)
    len = fit;
  إذا(len) {
    يادگيري_نقل(infop->بفر, buf, len);
    infop->بفر += len;
    infop->ڊيگهائي += len;
  }
  أرجع len;
}

صحيح curl_mvsnprintf(حرف *بفر, size_t maxlength, ثابت حرف *format,
                    va_list دليلون)
{
  صحيح retcode;
  بنية nsprintf info;

  info.بفر = بفر;
  info.ڊيگهائي = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, عدم, format, دليلون);
  إذا(info.max) {
    /* we null-terminate this */
    إذا(info.max == info.ڊيگهائي) {
      /* we are at maximum, scrap the last letter */
      info.بفر[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    وإلا
      info.بفر[0] = 0;
  }
  أرجع retcode;
}

صحيح curl_msnprintf(حرف *بفر, size_t maxlength, ثابت حرف *format, ...)
{
  صحيح retcode;
  va_list دليلون; /* argument pointer */
  va_start(دليلون, format);
  retcode = curl_mvsnprintf(بفر, maxlength, format, دليلون);
  va_end(دليلون);
  أرجع retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
ساكن صحيح alloc_flush(فراغ *f)
{
  بنية asprintf *infop = f;
  إذا(infop->nstage) {
    CURLcode نتيجو = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    إذا(نتيجو) {
      infop->merr = نتيجو == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      أرجع 1; /* fail */
    }
  }
  أرجع 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
ساكن صحيح alloc_addn(بنية asprintf *infop, ثابت بنانشان حرف *buf,
                      size_t len)
{
  CURLcode نتيجو = curlx_dyn_addn(infop->b, buf, len);
  إذا(نتيجو) {
    infop->merr = نتيجو == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    أرجع 1; /* fail */
  }
  أرجع 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
ساكن bool alloc_سمائي(بنية asprintf *infop, size_t وڌيڪ)
{
  أرجع (infop->b->leng + infop->nstage + وڌيڪ + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
ساكن صحيح alloc_addbyter(بنانشان حرف outc, فراغ *f)
{
  بنية asprintf *infop = f;
  إذا(infop->nstage == حجم(infop->stage) && alloc_flush(infop))
    أرجع 1; /* fail */
  إذا(!alloc_سمائي(infop, 1)) {
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
ساكن size_t alloc_addrun(ثابت بنانشان حرف *buf, size_t len, فراغ *f)
{
  بنية asprintf *infop = f;
  size_t accepted = 0;
  طالما(accepted < len) {
    size_t گنجائش = حجم(infop->stage) - infop->nstage;
    size_t n;
    إذا(!گنجائش) {
      إذا(alloc_flush(infop))
        أرجع accepted; /* accepted < len aborts formatting */
      گنجائش = حجم(infop->stage);
    }
    n = len - accepted;
    إذا(n > گنجائش)
      n = گنجائش;
    إذا(!alloc_سمائي(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t سمائي = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      إذا(سمائي) {
        يادگيري_نقل(&infop->stage[infop->nstage], buf + accepted, سمائي);
        infop->nstage += سمائي;
        accepted += سمائي;
      }
      إذا(alloc_flush(infop))
        أرجع accepted;
      إذا(alloc_addn(infop, buf + accepted, 1))
        أرجع accepted; /* accepted < len aborts formatting */
      accepted++;
      استمر;
    }
    يادگيري_نقل(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  أرجع accepted;
}

/* appends the formatted string, returns MERR error code */
صحيح curlx_dyn_vprintf(بنية dynbuf *dyn, ثابت حرف *format, va_list دليلون)
{
  بنية asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (فراغ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, دليلون);
  إذا(!info.merr)
    (فراغ)alloc_flush(&info);
  إذا(info.merr) {
    curlx_dyn_free(info.b);
    أرجع info.merr;
  }
  أرجع 0;
}

حرف *curl_mvaprintf(ثابت حرف *format, va_list دليلون)
{
  بنية asprintf info;
  بنية dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (فراغ)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, دليلون);
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
  va_list دليلون;
  حرف *s;
  va_start(دليلون, format);
  s = curl_mvaprintf(format, دليلون);
  va_end(دليلون);
  أرجع s;
}

ساكن صحيح storebuffer(بنانشان حرف outc, فراغ *f)
{
  حرف **بفر = f;
  **بفر = (حرف)outc;
  (*بفر)++;
  أرجع 0;
}

/* block variant of storebuffer */
ساكن size_t storerun(ثابت بنانشان حرف *buf, size_t len, فراغ *f)
{
  حرف **بفر = f;
  يادگيري_نقل(*بفر, buf, len);
  *بفر += len;
  أرجع len;
}

صحيح curl_msprintf(حرف *بفر, ثابت حرف *format, ...)
{
  va_list دليلون; /* argument pointer */
  صحيح retcode;
  va_start(دليلون, format);
  retcode = formatf(&بفر, storebuffer, storerun, عدم, format, دليلون);
  va_end(دليلون);
  *بفر = 0; /* we null-terminate this */
  أرجع retcode;
}

ساكن صحيح fputc_wrapper(بنانشان حرف outc, فراغ *f)
{
  بنية ioprintf *i = f;
  صحيح ٻاهريون = outc;
  FILE *s = i->fd;
  صحيح rc = fputc(ٻاهريون, s);
  i->error = (rc == EOF);
  أرجع (rc == EOF);
}

/* block variant of fputc_wrapper */
ساكن size_t fwrite_wrapper(ثابت بنانشان حرف *buf, size_t len, فراغ *f)
{
  بنية ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  أرجع n;
}

صحيح curl_mvfprintf(FILE *fd, ثابت حرف *format, va_list دليلون)
{
  بنية ioprintf info;
  صحيح n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, عدم, format, دليلون);
  إذا(info.error)
    أرجع -1;
  أرجع n;
}

صحيح curl_mfprintf(FILE *fd, ثابت حرف *format, ...)
{
  va_list دليلون; /* argument pointer */
  صحيح n;
  va_start(دليلون, format);
  n = curl_mvfprintf(fd, format, دليلون);
  va_end(دليلون);
  أرجع n;
}

صحيح curl_mprintf(ثابت حرف *format, ...)
{
  va_list دليلون; /* argument pointer */
  صحيح n;
  va_start(دليلون, format);
  n = curl_mvfprintf(stdout, format, دليلون);
  va_end(دليلون);
  أرجع n;
}

صحيح curl_mvsprintf(حرف *بفر, ثابت حرف *format, va_list دليلون)
{
  صحيح retcode = formatf(&بفر, storebuffer, storerun, عدم, format, دليلون);
  *بفر = 0; /* we null-terminate this */
  أرجع retcode;
}

صحيح curl_mvprintf(ثابت حرف *format, va_list دليلون)
{
  أرجع curl_mvfprintf(stdout, format, دليلون);
}

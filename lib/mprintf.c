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

#تعریف_کرو BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#تعریف_کرو MAX_PARAMETERS 128 /* number of input arguments */
#تعریف_کرو MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
ثابت غیرعلامتی حرف Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
ثابت غیرعلامتی حرف Curl_udigits[] = "0123456789ABCDEF";

#تعریف_کرو OUTCHAR(x)                        \
  کرو {                                    \
    اگر(دھارا((غیرعلامتی حرف)(x), userp)) \
      واپس TRUE;                        \
    (*donep)++;                           \
  } جب(0)

/* Data type to read from the arglist */
قسمتعریف شمار {
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
  MTYPE_چوڑائی,
  MTYPE_PRECISION
} FormatType;

/* conversion and display flags */
شمار {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_بایاں       = 1 << 2,
  FLAGS_ALT        = 1 << 3,
  FLAGS_SHORT      = 1 << 4,
  FLAGS_LONG       = 1 << 5,
  FLAGS_LONGLONG   = 1 << 6,
  FLAGS_LONGDOUBLE = 1 << 7,
  FLAGS_PAD_NIL    = 1 << 8,
  FLAGS_UNSIGNED   = 1 << 9,
  FLAGS_OCTAL      = 1 << 10,
  FLAGS_HEX        = 1 << 11,
  FLAGS_بالائی      = 1 << 12,
  FLAGS_چوڑائی      = 1 << 13, /* '*' or '*<num>$' used */
  FLAGS_WIDTHPARAM = 1 << 14, /* width PARAMETER was specified */
  FLAGS_PREC       = 1 << 15, /* precision was specified */
  FLAGS_PRECPARAM  = 1 << 16, /* precision PARAMETER was specified */
  FLAGS_CHAR       = 1 << 17, /* %c story */
  FLAGS_FLOATE     = 1 << 18, /* %e or %E */
  FLAGS_FLOATG     = 1 << 19, /* %g or %G */
  FLAGS_SUBSTR     = 1 << 20  /* no input, only substring */
};

شمار {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
ساخت va_داخلہ {
  FormatType type; /* FormatType */
  اتحاد {
    ثابت حرف *str;
    خالی *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    دوہرا dnum;
  } val;
};

/*
 * Describes an output segment.
 */
ساخت outsegment {
  صحیح چوڑائی;     /* width OR width parameter number */
  صحیح precision; /* precision OR precision parameter number */
  غیرعلامتی صحیح flags;
  غیرعلامتی صحیح داخلہ; /* input argument array index */
  ثابت حرف *آغاز; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

ساخت nsprintf {
  حرف *بفر;
  size_t لمبائی;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#تعریف_کرو ASPRINTF_STAGE_سائز 512

ساخت asprintf {
  ساخت dynbuf *b;
  حرف merr;
  size_t nstage;
  غیرعلامتی حرف stage[ASPRINTF_STAGE_سائز];
};

ساخت ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
ساکن صحیح dollarstring(ثابت حرف *p, ثابت حرف **انتہا)
{
  curl_off_t عدد;
  اگر(curlx_str_نمبر(&p, &عدد, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !عدد)
    واپس -1;
  *انتہا = p;
  واپس (صحیح)عدد - 1;
}

#تعریف_کرو is_arg_مستعمل(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#تعریف_کرو mark_arg_مستعمل(x, y) ((x)[(y) / 8] |= (غیرعلامتی حرف)(1 << ((y) & 7)))

/*
 * Parse the format string.
 *
 * Create two arrays. One describes the inputs, one describes the outputs.
 *
 * Returns zero on success.
 */

#تعریف_کرو PFMT_OK          0
#تعریف_کرو PFMT_DOLLAR      1 /* bad dollar for main param */
#تعریف_کرو PFMT_DOLLARWIDTH 2 /* bad dollar use for width */
#تعریف_کرو PFMT_DOLLARPREC  3 /* bad dollar use for precision */
#تعریف_کرو PFMT_MANYARGS    4 /* too many input arguments used */
#تعریف_کرو PFMT_PREC        5 /* precision overflow */
#تعریف_کرو PFMT_PRECMIX     6 /* bad mix of precision specifiers */
#تعریف_کرو PFMT_چوڑائی       7 /* width overflow */
#تعریف_کرو PFMT_INPUTGAP    8 /* gap in arguments */
#تعریف_کرو PFMT_WIDTHARG    9 /* attempted to use same arg twice, for width */
#تعریف_کرو PFMT_PRECARG    10 /* attempted to use same arg twice, for prec */
#تعریف_کرو PFMT_MANYSEGS   11 /* maxed out output segments */

ساکن صحیح parse_flags(ثابت حرف **fmtp, غیرعلامتی صحیح *flagsp, صحیح use_dollar,
                       صحیح *precp, صحیح *widthp)
{
  ثابت حرف *fmt = *fmtp;
  bool loopit = TRUE;
  غیرعلامتی صحیح flags = 0;
  صحیح چوڑائی = 0;
  صحیح precision = 0;

  /* Handle the flags */
  کرو {
    انتخاب(*fmt++) {
    حالت ' ':
      flags |= FLAGS_SPACE;
      توڑو;
    حالت '+':
      flags |= FLAGS_SHOWSIGN;
      توڑو;
    حالت '-':
      flags |= FLAGS_بایاں;
      flags &= ~(غیرعلامتی صحیح)FLAGS_PAD_NIL;
      توڑو;
    حالت '#':
      flags |= FLAGS_ALT;
      توڑو;
    حالت '.':
      اگر('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        اگر(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          اگر(precision < 0)
            /* illegal combo */
            واپس PFMT_DOLLARPREC;
        }
        ورنہ
          /* get it from the next argument */
          precision = -1;
      }
      ورنہ {
        bool is_neg;
        curl_off_t عدد;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        اگر(is_neg)
          fmt++;
        اگر(curlx_str_نمبر(&fmt, &عدد, زیادہ_سے_زیادہ_عدد))
          واپس PFMT_PREC;
        precision = (صحیح)عدد;
        اگر(is_neg)
          precision = -precision;
      }
      اگر((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
دلیل*/
        واپس PFMT_PRECMIX;
      توڑو;
    حالت 'h':
      flags |= FLAGS_SHORT;
      توڑو;
#اگر_تعریف_شدہ _WIN32
    حالت 'I':
      /* Non-ANSI integer extensions I32 I64 */
      اگر((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      ورنہ اگر((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      ورنہ {
#اگر SIZEOF_سائز_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#ورنہ
        flags |= FLAGS_LONG;
#اگر_ختم
      }
      توڑو;
#اگر_ختم /* _WIN32 */
    حالت 'l':
      اگر(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      ورنہ
        flags |= FLAGS_LONG;
      توڑو;
    حالت 'L':
      flags |= FLAGS_LONGDOUBLE;
      توڑو;
    حالت 'q':
      flags |= FLAGS_LONGLONG;
      توڑو;
    حالت 'z':
      /* the code below generates a warning if -Wunreachable-code is
مستعمل*/
#اگر SIZEOF_سائز_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ورنہ
      flags |= FLAGS_LONG;
#اگر_ختم
      توڑو;
    حالت 'O':
#اگر SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#ورنہ
      flags |= FLAGS_LONG;
#اگر_ختم
      توڑو;
    حالت '0':
      اگر(!(flags & FLAGS_بایاں))
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
      curl_off_t عدد;
      flags |= FLAGS_چوڑائی;
      fmt--;
      اگر(curlx_str_نمبر(&fmt, &عدد, زیادہ_سے_زیادہ_عدد))
        واپس PFMT_چوڑائی;
      چوڑائی = (صحیح)عدد;
      توڑو;
    }
    حالت '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      اگر(use_dollar == DOLLAR_USE) {
        چوڑائی = dollarstring(fmt, &fmt);
        اگر(چوڑائی < 0)
          /* illegal combo */
          واپس PFMT_DOLLARWIDTH;
      }
      ورنہ
        /* pick from the next argument */
        چوڑائی = -1;
      توڑو;
    طےشدہ:
      loopit = FALSE;
      fmt--;
      توڑو;
    } /* switch */
  } جب(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = چوڑائی;
  *fmtp = fmt;
  واپس PFMT_OK;
}

ساکن bool parse_conversion(ثابت حرف f, غیرعلامتی صحیح *flagp,
                             FormatType *typep)
{
  غیرعلامتی صحیح flags = *flagp;
  FormatType type;
  انتخاب(f) {
  حالت 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    توڑو;
  حالت 's':
    type = MTYPE_STRING;
    توڑو;
  حالت 'n':
    type = MTYPE_INTPTR;
    توڑو;
  حالت 'p':
    type = MTYPE_PTR;
    توڑو;
  حالت 'd':
  حالت 'i':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    ورنہ اگر(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    ورنہ
      type = MTYPE_INT;
    توڑو;
  حالت 'u':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ورنہ اگر(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ورنہ
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    توڑو;
  حالت 'o':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ورنہ اگر(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ورنہ
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    توڑو;
  حالت 'x':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ورنہ اگر(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ورنہ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    توڑو;
  حالت 'X':
    اگر(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    ورنہ اگر(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    ورنہ
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_بالائی | FLAGS_UNSIGNED;
    توڑو;
  حالت 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    توڑو;
  حالت 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    توڑو;
  حالت 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_بالائی;
    توڑو;
  حالت 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    توڑو;
  حالت 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_بالائی;
    توڑو;
  حالت 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    توڑو;
  حالت 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_بالائی;
    توڑو;
  طےشدہ:
    /* invalid instruction, disregard and continue */
    واپس TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  واپس FALSE;
}

ساکن صحیح parsefmt(ثابت حرف *format,
                    ساخت outsegment *برآمد,
                    ساخت va_داخلہ *اندر,
                    صحیح *opieces,
                    صحیح *ipieces, va_list arglist)
{
  ثابت حرف *fmt = format;
  صحیح param_عدد = 0;
  صحیح max_param = -1;
  صحیح i;
  صحیح ocount = 0;
  غیرعلامتی حرف usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  ساخت outsegment *optr;
  صحیح use_dollar = DOLLAR_UNKNOWN;
  ثابت حرف *آغاز = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, حجم(usedinput));

  جب(*fmt) {
    اگر(*fmt == '%') {
      ساخت va_داخلہ *iptr;
      FormatType type;
      غیرعلامتی صحیح flags = 0;
      صحیح چوڑائی = 0;
      صحیح precision = 0;
      صحیح param = -1;
      صحیح rc;
      fmt++;
      outlen = (size_t)(fmt - آغاز - 1);
      اگر(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        اگر(outlen) {
          optr = &برآمد[ocount++];
          اگر(ocount > MAX_SEGMENTS)
            واپس PFMT_MANYSEGS;
          optr->داخلہ = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->آغاز = آغاز;
          optr->outlen = outlen;
        }
        آغاز = fmt;
        fmt++;
        جاری; /* while */
      }

      اگر(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        اگر(param < 0) {
          اگر(use_dollar == DOLLAR_USE)
            /* illegal combo */
            واپس PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        ورنہ
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &چوڑائی);
      اگر(rc)
        واپس rc;

      اگر(parse_conversion(*fmt, &flags, &type))
        جاری;

      اگر(flags & FLAGS_WIDTHPARAM) {
        اگر(چوڑائی < 0)
          چوڑائی = param_عدد++;
        ورنہ {
          /* if this identifies a parameter already used, this is illegal */
          اگر(is_arg_مستعمل(usedinput, چوڑائی))
            واپس PFMT_WIDTHARG;
        }
        اگر(چوڑائی >= MAX_PARAMETERS)
          واپس PFMT_MANYARGS;
        اگر(چوڑائی >= max_param)
          max_param = چوڑائی;

        اندر[چوڑائی].type = MTYPE_چوڑائی;
        /* mark as used */
        mark_arg_مستعمل(usedinput, چوڑائی);
      }

      اگر(flags & FLAGS_PRECPARAM) {
        اگر(precision < 0)
          precision = param_عدد++;
        ورنہ {
          /* if this identifies a parameter already used, this is illegal */
          اگر(is_arg_مستعمل(usedinput, precision))
            واپس PFMT_PRECARG;
        }
        اگر(precision >= MAX_PARAMETERS)
          واپس PFMT_MANYARGS;
        اگر(precision >= max_param)
          max_param = precision;

        اندر[precision].type = MTYPE_PRECISION;
        mark_arg_مستعمل(usedinput, precision);
      }

      /* Handle the specifier */
      اگر(param < 0)
        param = param_عدد++;
      اگر(param >= MAX_PARAMETERS)
        واپس PFMT_MANYARGS;
      اگر(param >= max_param)
        max_param = param;

      iptr = &اندر[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_مستعمل(usedinput, param);

      fmt++;
      optr = &برآمد[ocount++];
      اگر(ocount > MAX_SEGMENTS)
        واپس PFMT_MANYSEGS;
      optr->داخلہ = (غیرعلامتی صحیح)param;
      optr->flags = flags;
      optr->چوڑائی = چوڑائی;
      optr->precision = precision;
      optr->آغاز = آغاز;
      optr->outlen = outlen;
      آغاز = fmt;
    }
    ورنہ
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - آغاز);
  اگر(outlen) {
    optr = &برآمد[ocount++];
    اگر(ocount > MAX_SEGMENTS)
      واپس PFMT_MANYSEGS;
    optr->داخلہ = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->آغاز = آغاز;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  برائے(i = 0; i < max_param + 1; i++) {
    ساخت va_داخلہ *iptr = &اندر[i];
    اگر(!is_arg_مستعمل(usedinput, i))
      /* bad input */
      واپس PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    انتخاب(iptr->type) {
    حالت MTYPE_STRING:
      iptr->val.str = va_arg(arglist, ثابت حرف *);
      توڑو;

    حالت MTYPE_INTPTR:
    حالت MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, خالی *);
      توڑو;

    حالت MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      توڑو;

    حالت MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      توڑو;

    حالت MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, غیرعلامتی طویل);
      توڑو;

    حالت MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, طویل);
      توڑو;

    حالت MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, غیرعلامتی صحیح);
      توڑو;

    حالت MTYPE_INT:
    حالت MTYPE_چوڑائی:
    حالت MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, صحیح);
      توڑو;

    حالت MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, دوہرا);
      توڑو;

    حالت MTYPE_LONGDOUBLE:
      iptr->val.dnum = (دوہرا)va_arg(arglist, طویل دوہرا);
      توڑو;

    طےشدہ:
      DEBUGASSERT(تہی); /* unexpected */
      توڑو;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  واپس PFMT_OK;
}

ساخت mproperty {
  صحیح چوڑائی;            /* Width of a field. */
  صحیح prec;             /* Precision of a field. */
  غیرعلامتی صحیح flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
قسمتعریف size_t (*mp_streamn)(ثابت غیرعلامتی حرف *buf, size_t len, خالی *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
قسمتعریف صحیح (*mp_flush)(خالی *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
ساکن bool دھارا_تسلسل(خالی *userp,
                       صحیح (*دھارا)(غیرعلامتی حرف, خالی *),
                       mp_streamn streamn,
                       ثابت غیرعلامتی حرف *buf, size_t len, صحیح *donep)
{
  اگر(!len)
    واپس FALSE;
  اگر(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (صحیح)n;
    واپس n != len;
  }
  جب(len--) {
    اگر(دھارا(*buf++, userp))
      واپس TRUE;
    (*donep)++;
  }
  واپس FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
ساکن ثابت حرف pad_spaces[] = "                ";
ساکن ثابت حرف pad_zeros[] = "0000000000000000";
#تعریف_کرو PAD_تسلسل 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
ساکن bool دھارا_pad(خالی *userp,
                       صحیح (*دھارا)(غیرعلامتی حرف, خالی *),
                       mp_streamn streamn,
                       غیرعلامتی حرف pad, صحیح عدد, صحیح *donep)
{
  ثابت غیرعلامتی حرف *src;
  اگر(عدد <= 0)
    واپس FALSE;
  اگر(pad == '0')
    src = (ثابت غیرعلامتی حرف *)pad_zeros;
  ورنہ
    src = (ثابت غیرعلامتی حرف *)pad_spaces;
  جب(عدد > PAD_تسلسل) {
    اگر(دھارا_تسلسل(userp, دھارا, streamn, src, PAD_تسلسل, donep))
      واپس TRUE;
    عدد -= PAD_تسلسل;
  }
  واپس دھارا_تسلسل(userp, دھارا, streamn, src, (size_t)عدد, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
ساکن bool دھارا_zrun(خالی *userp,
                        صحیح (*دھارا)(غیرعلامتی حرف, خالی *),
                        mp_streamn streamn,
                        ثابت حرف *str, size_t len, صحیح *donep)
{
  size_t n = 0;
  جب(n < len && str[n])
    n++;
  واپس دھارا_تسلسل(userp, دھارا, streamn,
                    (ثابت غیرعلامتی حرف *)str, n, donep);
}

ساکن bool برآمد_double(خالی *userp,
                       صحیح (*دھارا)(غیرعلامتی حرف, خالی *),
                       mp_streamn streamn,
                       ساخت mproperty *p,
                       دوہرا dnum,
                       حرف *work, صحیح *donep)
{
  حرف fmt[32] = "%";
  حرف *fptr = &fmt[1];
  size_t بایاں = حجم(fmt) - لڑی_لمبائی(fmt);
  صحیح flags = p->flags;
  صحیح چوڑائی = p->چوڑائی;
  صحیح prec = p->prec;

  اگر(flags & FLAGS_بایاں)
    *fptr++ = '-';
  اگر(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  اگر(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  اگر(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  اگر(چوڑائی >= 0) {
    size_t dlen;
    اگر(چوڑائی >= BUFFSIZE)
      چوڑائی = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, بایاں, "%d", چوڑائی);
    fptr += dlen;
    بایاں -= dlen;
  }
  اگر(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    صحیح maxprec = BUFFSIZE - 1;
    دوہرا val = dnum;
    صحیح len;
    اگر(prec > maxprec)
      prec = maxprec - 1;
    اگر(چوڑائی > 0 && prec <= چوڑائی)
      maxprec -= چوڑائی;
    جب(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    اگر(prec > maxprec)
      prec = maxprec - 1;
    اگر(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, بایاں, ".%d", prec);
    fptr += len;
  }
  اگر(flags & FLAGS_LONG)
    *fptr++ = 'l';

  اگر(flags & FLAGS_FLOATE)
    *fptr++ = (حرف)((flags & FLAGS_بالائی) ? 'E' : 'e');
  ورنہ اگر(flags & FLAGS_FLOATG)
    *fptr++ = (حرف)((flags & FLAGS_بالائی) ? 'G' : 'g');
  ورنہ
    *fptr++ = (flags & FLAGS_بالائی) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wformat-nonliteral"
#اگر_ختم
#اگر_تعریف_شدہ _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#ورنہ
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#اگر_ختم
#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#اگر_ختم
  DEBUGASSERT(لڑی_لمبائی(work) < BUFFSIZE);
  واپس دھارا_تسلسل(userp, دھارا, streamn,
                    (ثابت غیرعلامتی حرف *)work, لڑی_لمبائی(work), donep);
}

ساکن bool برآمد_نمبر(خالی *userp,
                       صحیح (*دھارا)(غیرعلامتی حرف, خالی *),
                       mp_streamn streamn,
                       ساخت mproperty *p,
                       uint64_t عدد,
                       int64_t nums,
                       حرف *work, صحیح *donep)
{
  ثابت غیرعلامتی حرف *digits = Curl_ldigits;
  صحیح flags = p->flags;
  صحیح چوڑائی = p->چوڑائی;
  صحیح prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  صحیح بنیاد = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  حرف *workend = &work[BUFFSIZE - 2];
  حرف *w;

  اگر(flags & FLAGS_CHAR) {
    /* Character. */
    اگر(!(flags & FLAGS_بایاں)) {
      اگر(دھارا_pad(userp, دھارا, streamn, ' ', چوڑائی - 1, donep))
        واپس TRUE;
      چوڑائی = 1;
    }
    OUTCHAR((حرف)عدد);
    اگر(flags & FLAGS_بایاں) {
      اگر(دھارا_pad(userp, دھارا, streamn, ' ', چوڑائی - 1, donep))
        واپس TRUE;
    }
    واپس FALSE;
  }
  اگر(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    بنیاد = 8;

  ورنہ اگر(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_بالائی) ? Curl_udigits : Curl_ldigits;
    بنیاد = 16;
  }
  ورنہ اگر(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  ورنہ {
    /* Decimal integer. */
    is_neg = (nums < 0);
    اگر(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_عدد; /* Used to convert negative in positive. */
      signed_عدد = nums + (int64_t)1;
      signed_عدد = -signed_عدد;
      عدد = (uint64_t)signed_عدد;
      عدد += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  اگر(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(بنیاد <= 16);
  انتخاب(بنیاد) {
  حالت 10:
    جب(عدد > 0) {
      *w-- = (حرف)('0' + (عدد % 10));
      عدد /= 10;
    }
    توڑو;
  طےشدہ:
    جب(عدد > 0) {
      *w-- = digits[عدد % بنیاد];
      عدد /= بنیاد;
    }
    توڑو;
  }
  چوڑائی -= (صحیح)(workend - w);
  prec -= (صحیح)(workend - w);

  اگر(is_alt && بنیاد == 8 && prec <= 0) {
    *w-- = '0';
    --چوڑائی;
  }

  اگر(prec > 0) {
    چوڑائی -= prec;
    جب(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  اگر(is_alt && بنیاد == 16)
    چوڑائی -= 2;

  اگر(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --چوڑائی;

  اگر(!(flags & FLAGS_بایاں) && !(flags & FLAGS_PAD_NIL)) {
    اگر(دھارا_pad(userp, دھارا, streamn, ' ', چوڑائی, donep))
      واپس TRUE;
    چوڑائی = 0;
  }

  اگر(is_neg)
    OUTCHAR('-');
  ورنہ اگر(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  ورنہ اگر(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  اگر(is_alt && بنیاد == 16) {
    OUTCHAR('0');
    اگر(flags & FLAGS_بالائی)
      OUTCHAR('X');
    ورنہ
      OUTCHAR('x');
  }

  اگر(!(flags & FLAGS_بایاں) && (flags & FLAGS_PAD_NIL)) {
    اگر(دھارا_pad(userp, دھارا, streamn, '0', چوڑائی, donep))
      واپس TRUE;
    چوڑائی = 0;
  }

  /* Write the number. */
  اگر(دھارا_تسلسل(userp, دھارا, streamn, (ثابت غیرعلامتی حرف *)w + 1,
                (size_t)(workend - w), donep))
    واپس TRUE;

  اگر(flags & FLAGS_بایاں) {
    اگر(دھارا_pad(userp, دھارا, streamn, ' ', چوڑائی, donep))
      واپس TRUE;
  }

  واپس FALSE;
}

ساکن ثابت حرف nilstr[] = "(nil)";

ساکن bool برآمد_string(خالی *userp,
                       صحیح (*دھارا)(غیرعلامتی حرف, خالی *),
                       mp_streamn streamn,
                       ساخت mproperty *p,
                       ثابت حرف *str,
                       صحیح *donep)
{
  صحیح flags = p->flags;
  صحیح چوڑائی = p->چوڑائی;
  صحیح prec = p->prec;
  size_t len;

  اگر(!str) {
    /* Write null string if there is space. */
    اگر(prec == -1 || prec >= (صحیح)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(غیرعلامتی صحیح)FLAGS_ALT;
    }
    ورنہ {
      str = "";
      len = 0;
    }
  }
  ورنہ اگر(prec != -1)
    len = (size_t)prec;
  ورنہ اگر(*str == '\0')
    len = 0;
  ورنہ
    len = لڑی_لمبائی(str);

  چوڑائی -= (len > زیادہ_سے_زیادہ_عدد) ? زیادہ_سے_زیادہ_عدد : (صحیح)len;

  اگر(flags & FLAGS_ALT)
    OUTCHAR('"');

  اگر(!(flags & FLAGS_بایاں)) {
    اگر(دھارا_pad(userp, دھارا, streamn, ' ', چوڑائی, donep))
      واپس TRUE;
    چوڑائی = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  اگر(prec == -1) {
    اگر(دھارا_تسلسل(userp, دھارا, streamn,
                  (ثابت غیرعلامتی حرف *)str, len, donep))
      واپس TRUE;
  }
  ورنہ اگر(دھارا_zrun(userp, دھارا, streamn, str, len, donep))
    واپس TRUE;
  اگر(flags & FLAGS_بایاں) {
    اگر(دھارا_pad(userp, دھارا, streamn, ' ', چوڑائی, donep))
      واپس TRUE;
  }

  اگر(flags & FLAGS_ALT)
    OUTCHAR('"');

  واپس FALSE;
}

ساکن bool برآمد_اشارہ_گر(خالی *userp,
                        صحیح (*دھارا)(غیرعلامتی حرف, خالی *),
                        mp_streamn streamn,
                        ساخت mproperty *p,
                        ثابت حرف *ptr,
                        حرف *work,
                        صحیح *donep)
{
  /* Generic pointer. */
  اگر(ptr) {
    size_t عدد = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    اگر(برآمد_نمبر(userp, دھارا, streamn, p, عدد, 0, work, donep))
      واپس TRUE;
  }
  ورنہ {
    /* Write "(nil)" for a nil pointer. */
    صحیح چوڑائی = p->چوڑائی;
    صحیح flags = p->flags;

    چوڑائی -= (صحیح)CURL_CSTRLEN(nilstr);
    اگر(flags & FLAGS_بایاں) {
      اگر(دھارا_pad(userp, دھارا, streamn, ' ', چوڑائی, donep))
        واپس TRUE;
      چوڑائی = 0;
    }
    اگر(دھارا_تسلسل(userp, دھارا, streamn, (ثابت غیرعلامتی حرف *)nilstr,
                  حجم(nilstr) - 1, donep))
      واپس TRUE;
    اگر(!(flags & FLAGS_بایاں)) {
      اگر(دھارا_pad(userp, دھارا, streamn, ' ', چوڑائی, donep))
        واپس TRUE;
    }
  }
  واپس FALSE;
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
ساکن صحیح formatf(خالی *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   صحیح (*دھارا)(غیرعلامتی حرف, خالی *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   ثابت حرف *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  صحیح مکمل = 0;   /* number of characters written */
  صحیح i;
  صحیح ocount = 0; /* number of output segments */
  صحیح icount = 0; /* number of input arguments */

  ساخت outsegment output[MAX_SEGMENTS];
  ساخت va_داخلہ داخلہ[MAX_PARAMETERS];
  حرف work[BUFFSIZE + 2];

  /* Parse the format string */
  اگر(parsefmt(format, output, داخلہ, &ocount, &icount, ap_save))
    واپس 0;

  برائے(i = 0; i < ocount; i++) {
    ساخت outsegment *optr = &output[i];
    ساخت va_داخلہ *iptr = &داخلہ[optr->داخلہ];
    ساخت mproperty p;
    size_t outlen = optr->outlen;

    اگر(outlen) {
      اگر(دھارا_zrun(userp, دھارا, streamn, optr->آغاز, outlen, &مکمل))
        واپس مکمل;
      اگر(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        جاری;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    اگر(p.flags & FLAGS_WIDTHPARAM) {
      p.چوڑائی = (صحیح)داخلہ[optr->چوڑائی].val.nums;
      اگر(p.چوڑائی < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        اگر(p.چوڑائی == INT_MIN)
          p.چوڑائی = زیادہ_سے_زیادہ_عدد;
        ورنہ
          p.چوڑائی = -p.چوڑائی;
        p.flags |= FLAGS_بایاں;
        p.flags &= ~(غیرعلامتی صحیح)FLAGS_PAD_NIL;
      }
    }
    ورنہ
      p.چوڑائی = optr->چوڑائی;

    /* pick up the specified precision */
    اگر(p.flags & FLAGS_PRECPARAM) {
      p.prec = (صحیح)داخلہ[optr->precision].val.nums;
      اگر(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    ورنہ اگر(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    ورنہ
      p.prec = -1;

    انتخاب(iptr->type) {
    حالت MTYPE_INTU:
    حالت MTYPE_LONGU:
    حالت MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      اگر(برآمد_نمبر(userp, دھارا, streamn, &p, iptr->val.numu, 0, work,
                    &مکمل))
        واپس مکمل;
      توڑو;

    حالت MTYPE_INT:
    حالت MTYPE_LONG:
    حالت MTYPE_LONGLONG:
      اگر(برآمد_نمبر(userp, دھارا, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &مکمل))
        واپس مکمل;
      توڑو;

    حالت MTYPE_STRING:
      اگر(برآمد_string(userp, دھارا, streamn, &p, iptr->val.str, &مکمل))
        واپس مکمل;
      توڑو;

    حالت MTYPE_PTR:
      اگر(برآمد_اشارہ_گر(userp, دھارا, streamn, &p, iptr->val.ptr, work,
                     &مکمل))
        واپس مکمل;
      توڑو;

    حالت MTYPE_DOUBLE:
    حالت MTYPE_LONGDOUBLE:
      اگر(برآمد_double(userp, دھارا, streamn, &p, iptr->val.dnum, work,
                    &مکمل))
        واپس مکمل;
      توڑو;

    حالت MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      اگر(flush && flush(userp))
        واپس مکمل;
      /* Answer the count of characters written. */
      اگر(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)مکمل;
      ورنہ اگر(p.flags & FLAGS_LONG)
        *(طویل *)iptr->val.ptr = (طویل)مکمل;
      ورنہ اگر(!(p.flags & FLAGS_SHORT))
        *(صحیح *)iptr->val.ptr = مکمل;
      ورنہ
        *(مختصر *)iptr->val.ptr = (مختصر)مکمل;
      توڑو;

    طےشدہ:
      توڑو;
    }
  }
  واپس مکمل;
}

/* fputc() look-alike */
ساکن صحیح addbyter(غیرعلامتی حرف outc, خالی *f)
{
  ساخت nsprintf *infop = f;
  اگر(infop->لمبائی < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->بفر++ = (حرف)outc; /* store */
    infop->لمبائی++; /* we are now one byte larger */
    واپس 0;     /* fputc() returns like this on success */
  }
  واپس 1;
}

/* block variant of addbyter */
ساکن size_t addrun(ثابت غیرعلامتی حرف *buf, size_t len, خالی *f)
{
  ساخت nsprintf *infop = f;
  size_t fit = infop->max - infop->لمبائی;
  اگر(len > fit)
    len = fit;
  اگر(len) {
    حافظہ_نقل(infop->بفر, buf, len);
    infop->بفر += len;
    infop->لمبائی += len;
  }
  واپس len;
}

صحیح curl_mvsnprintf(حرف *بفر, size_t maxlength, ثابت حرف *format,
                    va_list دلائل)
{
  صحیح retcode;
  ساخت nsprintf info;

  info.بفر = بفر;
  info.لمبائی = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, تہی, format, دلائل);
  اگر(info.max) {
    /* we null-terminate this */
    اگر(info.max == info.لمبائی) {
      /* we are at maximum, scrap the last letter */
      info.بفر[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    ورنہ
      info.بفر[0] = 0;
  }
  واپس retcode;
}

صحیح curl_msnprintf(حرف *بفر, size_t maxlength, ثابت حرف *format, ...)
{
  صحیح retcode;
  va_list دلائل; /* argument pointer */
  va_start(دلائل, format);
  retcode = curl_mvsnprintf(بفر, maxlength, format, دلائل);
  va_end(دلائل);
  واپس retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
ساکن صحیح alloc_flush(خالی *f)
{
  ساخت asprintf *infop = f;
  اگر(infop->nstage) {
    CURLcode نتیجہ = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    اگر(نتیجہ) {
      infop->merr = نتیجہ == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      واپس 1; /* fail */
    }
  }
  واپس 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
ساکن صحیح alloc_addn(ساخت asprintf *infop, ثابت غیرعلامتی حرف *buf,
                      size_t len)
{
  CURLcode نتیجہ = curlx_dyn_addn(infop->b, buf, len);
  اگر(نتیجہ) {
    infop->merr = نتیجہ == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    واپس 1; /* fail */
  }
  واپس 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
ساکن bool alloc_سماتا(ساخت asprintf *infop, size_t مزید)
{
  واپس (infop->b->leng + infop->nstage + مزید + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
ساکن صحیح alloc_addbyter(غیرعلامتی حرف outc, خالی *f)
{
  ساخت asprintf *infop = f;
  اگر(infop->nstage == حجم(infop->stage) && alloc_flush(infop))
    واپس 1; /* fail */
  اگر(!alloc_سماتا(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    اگر(alloc_flush(infop))
      واپس 1;
    واپس alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  واپس 0;
}

/* block variant of alloc_addbyter */
ساکن size_t alloc_addrun(ثابت غیرعلامتی حرف *buf, size_t len, خالی *f)
{
  ساخت asprintf *infop = f;
  size_t accepted = 0;
  جب(accepted < len) {
    size_t گنجائش = حجم(infop->stage) - infop->nstage;
    size_t n;
    اگر(!گنجائش) {
      اگر(alloc_flush(infop))
        واپس accepted; /* accepted < len aborts formatting */
      گنجائش = حجم(infop->stage);
    }
    n = len - accepted;
    اگر(n > گنجائش)
      n = گنجائش;
    اگر(!alloc_سماتا(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t سماتا = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      اگر(سماتا) {
        حافظہ_نقل(&infop->stage[infop->nstage], buf + accepted, سماتا);
        infop->nstage += سماتا;
        accepted += سماتا;
      }
      اگر(alloc_flush(infop))
        واپس accepted;
      اگر(alloc_addn(infop, buf + accepted, 1))
        واپس accepted; /* accepted < len aborts formatting */
      accepted++;
      جاری;
    }
    حافظہ_نقل(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  واپس accepted;
}

/* appends the formatted string, returns MERR error code */
صحیح curlx_dyn_vprintf(ساخت dynbuf *dyn, ثابت حرف *format, va_list دلائل)
{
  ساخت asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (خالی)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, دلائل);
  اگر(!info.merr)
    (خالی)alloc_flush(&info);
  اگر(info.merr) {
    curlx_dyn_free(info.b);
    واپس info.merr;
  }
  واپس 0;
}

حرف *curl_mvaprintf(ثابت حرف *format, va_list دلائل)
{
  ساخت asprintf info;
  ساخت dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (خالی)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, دلائل);
  اگر(!info.merr)
    (خالی)alloc_flush(&info);
  اگر(info.merr) {
    curlx_dyn_free(info.b);
    واپس تہی;
  }
  اگر(curlx_dyn_len(info.b))
    واپس curlx_dyn_ptr(info.b);
  واپس curlx_strdup("");
}

حرف *curl_maprintf(ثابت حرف *format, ...)
{
  va_list دلائل;
  حرف *s;
  va_start(دلائل, format);
  s = curl_mvaprintf(format, دلائل);
  va_end(دلائل);
  واپس s;
}

ساکن صحیح storebuffer(غیرعلامتی حرف outc, خالی *f)
{
  حرف **بفر = f;
  **بفر = (حرف)outc;
  (*بفر)++;
  واپس 0;
}

/* block variant of storebuffer */
ساکن size_t storerun(ثابت غیرعلامتی حرف *buf, size_t len, خالی *f)
{
  حرف **بفر = f;
  حافظہ_نقل(*بفر, buf, len);
  *بفر += len;
  واپس len;
}

صحیح curl_msprintf(حرف *بفر, ثابت حرف *format, ...)
{
  va_list دلائل; /* argument pointer */
  صحیح retcode;
  va_start(دلائل, format);
  retcode = formatf(&بفر, storebuffer, storerun, تہی, format, دلائل);
  va_end(دلائل);
  *بفر = 0; /* we null-terminate this */
  واپس retcode;
}

ساکن صحیح fputc_wrapper(غیرعلامتی حرف outc, خالی *f)
{
  ساخت ioprintf *i = f;
  صحیح برآمد = outc;
  FILE *s = i->fd;
  صحیح rc = fputc(برآمد, s);
  i->error = (rc == EOF);
  واپس (rc == EOF);
}

/* block variant of fputc_wrapper */
ساکن size_t fwrite_wrapper(ثابت غیرعلامتی حرف *buf, size_t len, خالی *f)
{
  ساخت ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  واپس n;
}

صحیح curl_mvfprintf(FILE *fd, ثابت حرف *format, va_list دلائل)
{
  ساخت ioprintf info;
  صحیح n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, تہی, format, دلائل);
  اگر(info.error)
    واپس -1;
  واپس n;
}

صحیح curl_mfprintf(FILE *fd, ثابت حرف *format, ...)
{
  va_list دلائل; /* argument pointer */
  صحیح n;
  va_start(دلائل, format);
  n = curl_mvfprintf(fd, format, دلائل);
  va_end(دلائل);
  واپس n;
}

صحیح curl_mprintf(ثابت حرف *format, ...)
{
  va_list دلائل; /* argument pointer */
  صحیح n;
  va_start(دلائل, format);
  n = curl_mvfprintf(stdout, format, دلائل);
  va_end(دلائل);
  واپس n;
}

صحیح curl_mvsprintf(حرف *بفر, ثابت حرف *format, va_list دلائل)
{
  صحیح retcode = formatf(&بفر, storebuffer, storerun, تہی, format, دلائل);
  *بفر = 0; /* we null-terminate this */
  واپس retcode;
}

صحیح curl_mvprintf(ثابت حرف *format, va_list دلائل)
{
  واپس curl_mvfprintf(stdout, format, دلائل);
}

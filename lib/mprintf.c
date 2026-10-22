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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "curl_printf.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curlx/snprintf.h"  /* for curlx_win32_snprintf() */

#define BUFFSIZE 326 /* buffer for long-to-str and float-to-str calcs, should
                        fit negative DBL_MAX (317 letters) */
#define MAX_PARAMETERS 128 /* number of input arguments */
#define MAX_SEGMENTS   128 /* number of output segments */

/* Lower-case digits. */
અચલ unsigned અક્ષર Curl_ldigits[] = "0123456789abcdef";

/* Upper-case digits. */
અચલ unsigned અક્ષર Curl_udigits[] = "0123456789ABCDEF";

#define OUTCHAR(x)                        \
  કરો {                                    \
    જો(stream((unsigned અક્ષર)(x), userp)) \
      પાછા TRUE;                        \
    (*donep)++;                           \
  } જ્યારે(0)

/* Data type to read from the arglist */
પ્રકારવ્યાખ્યા ગણના {
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
ગણના {
  FLAGS_SPACE      = 1 << 0,
  FLAGS_SHOWSIGN   = 1 << 1,
  FLAGS_ડાબું       = 1 << 2,
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

ગણના {
  DOLLAR_UNKNOWN,
  DOLLAR_NOPE,
  DOLLAR_USE
};

/*
 * Describes an input va_arg type and hold its value.
 */
સંરચના va_આગત {
  FormatType type; /* FormatType */
  સંઘ {
    અચલ અક્ષર *str;
    શૂન્ય *ptr;
    int64_t nums; /* signed */
    uint64_t numu; /* unsigned */
    દ્વિગુણ dnum;
  } val;
};

/*
 * Describes an output segment.
 */
સંરચના outsegment {
  પૂર્ણાંક width;     /* width OR width parameter number */
  પૂર્ણાંક precision; /* precision OR precision parameter number */
  unsigned પૂર્ણાંક flags;
  unsigned પૂર્ણાંક આગત; /* input argument array index */
  અચલ અક્ષર *start; /* format string start to output */
  size_t outlen;     /* number of bytes from the format string to output */
};

સંરચના nsprintf {
  અક્ષર *buffer;
  size_t length;
  size_t max;
};

/* Output bytes are staged here and appended to the dynbuf in chunks: the
   dynbuf append overhead (bounds checks, growth, memcpy, null-termination)
   once per byte dominates formatting. 512 bytes of stack holds a typical
   request line or header in a single flush. */
#define ASPRINTF_STAGE_SIZE 512

સંરચના asprintf {
  સંરચના dynbuf *b;
  અક્ષર merr;
  size_t nstage;
  unsigned અક્ષર stage[ASPRINTF_STAGE_SIZE];
};

સંરચના ioprintf {
  FILE *fd;
  bool error;
};

/* the provided input number is 1-based but this returns the number 0-based.
 *
 * returns -1 if no valid number was provided.
 */
સ્થિર પૂર્ણાંક dollarstring(અચલ અક્ષર *p, અચલ અક્ષર **end)
{
  curl_off_t અંક;
  જો(curlx_str_સંખ્યા(&p, &અંક, MAX_PARAMETERS) ||
     curlx_str_single(&p, '$') || !અંક)
    પાછા -1;
  *end = p;
  પાછા (પૂર્ણાંક)અંક - 1;
}

#define is_arg_used(x, y)   ((x)[(y) / 8] & (1 << ((y) & 7)))
#define mark_arg_used(x, y) ((x)[(y) / 8] |= (unsigned અક્ષર)(1 << ((y) & 7)))

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

સ્થિર પૂર્ણાંક parse_flags(અચલ અક્ષર **fmtp, unsigned પૂર્ણાંક *flagsp, પૂર્ણાંક use_dollar,
                       પૂર્ણાંક *precp, પૂર્ણાંક *widthp)
{
  અચલ અક્ષર *fmt = *fmtp;
  bool loopit = TRUE;
  unsigned પૂર્ણાંક flags = 0;
  પૂર્ણાંક width = 0;
  પૂર્ણાંક precision = 0;

  /* Handle the flags */
  કરો {
    પસંદગી(*fmt++) {
    સ્થિતિ ' ':
      flags |= FLAGS_SPACE;
      તોડો;
    સ્થિતિ '+':
      flags |= FLAGS_SHOWSIGN;
      તોડો;
    સ્થિતિ '-':
      flags |= FLAGS_ડાબું;
      flags &= ~(unsigned પૂર્ણાંક)FLAGS_PAD_NIL;
      તોડો;
    સ્થિતિ '#':
      flags |= FLAGS_ALT;
      તોડો;
    સ્થિતિ '.':
      જો('*' == *fmt) {
        /* The precision is picked from a specified parameter */
        flags |= FLAGS_PRECPARAM;
        fmt++;

        જો(use_dollar == DOLLAR_USE) {
          precision = dollarstring(fmt, &fmt);
          જો(precision < 0)
            /* illegal combo */
            પાછા PFMT_DOLLARPREC;
        }
        નહીંતર
          /* get it from the next argument */
          precision = -1;
      }
      નહીંતર {
        bool is_neg;
        curl_off_t અંક;
        flags |= FLAGS_PREC;
        is_neg = ('-' == *fmt);
        જો(is_neg)
          fmt++;
        જો(curlx_str_સંખ્યા(&fmt, &અંક, INT_MAX))
          પાછા PFMT_PREC;
        precision = (પૂર્ણાંક)અંક;
        જો(is_neg)
          precision = -precision;
      }
      જો((flags & (FLAGS_PREC | FLAGS_PRECPARAM)) ==
         (FLAGS_PREC | FLAGS_PRECPARAM))
        /* it is not permitted to use both kinds of precision for the same
           argument */
        પાછા PFMT_PRECMIX;
      તોડો;
    સ્થિતિ 'h':
      flags |= FLAGS_SHORT;
      તોડો;
#ifdef _WIN32
    સ્થિતિ 'I':
      /* Non-ANSI integer extensions I32 I64 */
      જો((fmt[0] == '3') && (fmt[1] == '2')) {
        flags |= FLAGS_LONG;
        fmt += 2;
      }
      નહીંતર જો((fmt[0] == '6') && (fmt[1] == '4')) {
        flags |= FLAGS_LONGLONG;
        fmt += 2;
      }
      નહીંતર {
#જો SIZEOF_SIZE_T > SIZEOF_LONG
        flags |= FLAGS_LONGLONG;
#નહીંતર
        flags |= FLAGS_LONG;
#endif
      }
      તોડો;
#endif /* _WIN32 */
    સ્થિતિ 'l':
      જો(flags & FLAGS_LONG)
        flags |= FLAGS_LONGLONG;
      નહીંતર
        flags |= FLAGS_LONG;
      તોડો;
    સ્થિતિ 'L':
      flags |= FLAGS_LONGDOUBLE;
      તોડો;
    સ્થિતિ 'q':
      flags |= FLAGS_LONGLONG;
      તોડો;
    સ્થિતિ 'z':
      /* the code below generates a warning if -Wunreachable-code is
         used */
#જો SIZEOF_SIZE_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#નહીંતર
      flags |= FLAGS_LONG;
#endif
      તોડો;
    સ્થિતિ 'O':
#જો SIZEOF_CURL_OFF_T > SIZEOF_LONG
      flags |= FLAGS_LONGLONG;
#નહીંતર
      flags |= FLAGS_LONG;
#endif
      તોડો;
    સ્થિતિ '0':
      જો(!(flags & FLAGS_ડાબું))
        flags |= FLAGS_PAD_NIL;
      FALLTHROUGH();
    સ્થિતિ '1':
    સ્થિતિ '2':
    સ્થિતિ '3':
    સ્થિતિ '4':
    સ્થિતિ '5':
    સ્થિતિ '6':
    સ્થિતિ '7':
    સ્થિતિ '8':
    સ્થિતિ '9': {
      curl_off_t અંક;
      flags |= FLAGS_WIDTH;
      fmt--;
      જો(curlx_str_સંખ્યા(&fmt, &અંક, INT_MAX))
        પાછા PFMT_WIDTH;
      width = (પૂર્ણાંક)અંક;
      તોડો;
    }
    સ્થિતિ '*':  /* read width from argument list */
      flags |= FLAGS_WIDTHPARAM;
      જો(use_dollar == DOLLAR_USE) {
        width = dollarstring(fmt, &fmt);
        જો(width < 0)
          /* illegal combo */
          પાછા PFMT_DOLLARWIDTH;
      }
      નહીંતર
        /* pick from the next argument */
        width = -1;
      તોડો;
    મૂળભૂત:
      loopit = FALSE;
      fmt--;
      તોડો;
    } /* switch */
  } જ્યારે(loopit); /* do */
  *flagsp = flags;
  *precp = precision;
  *widthp = width;
  *fmtp = fmt;
  પાછા PFMT_OK;
}

સ્થિર bool parse_conversion(અચલ અક્ષર f, unsigned પૂર્ણાંક *flagp,
                             FormatType *typep)
{
  unsigned પૂર્ણાંક flags = *flagp;
  FormatType type;
  પસંદગી(f) {
  સ્થિતિ 'S':
    flags |= FLAGS_ALT;
    type = MTYPE_STRING;
    તોડો;
  સ્થિતિ 's':
    type = MTYPE_STRING;
    તોડો;
  સ્થિતિ 'n':
    type = MTYPE_INTPTR;
    તોડો;
  સ્થિતિ 'p':
    type = MTYPE_PTR;
    તોડો;
  સ્થિતિ 'd':
  સ્થિતિ 'i':
    જો(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONG;
    નહીંતર જો(flags & FLAGS_LONG)
      type = MTYPE_LONG;
    નહીંતર
      type = MTYPE_INT;
    તોડો;
  સ્થિતિ 'u':
    જો(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    નહીંતર જો(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    નહીંતર
      type = MTYPE_INTU;
    flags |= FLAGS_UNSIGNED;
    તોડો;
  સ્થિતિ 'o':
    જો(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    નહીંતર જો(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    નહીંતર
      type = MTYPE_INTU;
    flags |= FLAGS_OCTAL | FLAGS_UNSIGNED;
    તોડો;
  સ્થિતિ 'x':
    જો(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    નહીંતર જો(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    નહીંતર
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UNSIGNED;
    તોડો;
  સ્થિતિ 'X':
    જો(flags & FLAGS_LONGLONG)
      type = MTYPE_LONGLONGU;
    નહીંતર જો(flags & FLAGS_LONG)
      type = MTYPE_LONGU;
    નહીંતર
      type = MTYPE_INTU;
    flags |= FLAGS_HEX | FLAGS_UPPER | FLAGS_UNSIGNED;
    તોડો;
  સ્થિતિ 'c':
    type = MTYPE_INT;
    flags |= FLAGS_CHAR;
    તોડો;
  સ્થિતિ 'f':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    તોડો;
  સ્થિતિ 'F':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_UPPER;
    તોડો;
  સ્થિતિ 'e':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE;
    તોડો;
  સ્થિતિ 'E':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATE | FLAGS_UPPER;
    તોડો;
  સ્થિતિ 'g':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG;
    તોડો;
  સ્થિતિ 'G':
    type = flags & FLAGS_LONGDOUBLE ? MTYPE_LONGDOUBLE : MTYPE_DOUBLE;
    flags |= FLAGS_FLOATG | FLAGS_UPPER;
    તોડો;
  મૂળભૂત:
    /* invalid instruction, disregard and continue */
    પાછા TRUE;
  } /* switch */

  *flagp |= flags;
  *typep = type;
  પાછા FALSE;
}

સ્થિર પૂર્ણાંક parsefmt(અચલ અક્ષર *format,
                    સંરચના outsegment *out,
                    સંરચના va_આગત *in,
                    પૂર્ણાંક *opieces,
                    પૂર્ણાંક *ipieces, va_list arglist)
{
  અચલ અક્ષર *fmt = format;
  પૂર્ણાંક param_અંક = 0;
  પૂર્ણાંક max_param = -1;
  પૂર્ણાંક i;
  પૂર્ણાંક ocount = 0;
  unsigned અક્ષર usedinput[MAX_PARAMETERS / 8];
  size_t outlen = 0;
  સંરચના outsegment *optr;
  પૂર્ણાંક use_dollar = DOLLAR_UNKNOWN;
  અચલ અક્ષર *start = fmt;

  /* clear, set a bit for each used input */
  memset(usedinput, 0, કદ(usedinput));

  જ્યારે(*fmt) {
    જો(*fmt == '%') {
      સંરચના va_આગત *iptr;
      FormatType type;
      unsigned પૂર્ણાંક flags = 0;
      પૂર્ણાંક width = 0;
      પૂર્ણાંક precision = 0;
      પૂર્ણાંક param = -1;
      પૂર્ણાંક rc;
      fmt++;
      outlen = (size_t)(fmt - start - 1);
      જો(*fmt == '%') {
        /* this means a %% that should be output only as %. Create an output
           segment. */
        જો(outlen) {
          optr = &out[ocount++];
          જો(ocount > MAX_SEGMENTS)
            પાછા PFMT_MANYSEGS;
          optr->આગત = 0;
          optr->flags = FLAGS_SUBSTR;
          optr->start = start;
          optr->outlen = outlen;
        }
        start = fmt;
        fmt++;
        ચાલુ; /* while */
      }

      જો(use_dollar != DOLLAR_NOPE) {
        param = dollarstring(fmt, &fmt);
        જો(param < 0) {
          જો(use_dollar == DOLLAR_USE)
            /* illegal combo */
            પાછા PFMT_DOLLAR;

          /* we got no positional, get the next arg */
          param = -1;
          use_dollar = DOLLAR_NOPE;
        }
        નહીંતર
          use_dollar = DOLLAR_USE;
      }

      rc = parse_flags(&fmt, &flags, use_dollar, &precision, &width);
      જો(rc)
        પાછા rc;

      જો(parse_conversion(*fmt, &flags, &type))
        ચાલુ;

      જો(flags & FLAGS_WIDTHPARAM) {
        જો(width < 0)
          width = param_અંક++;
        નહીંતર {
          /* if this identifies a parameter already used, this is illegal */
          જો(is_arg_used(usedinput, width))
            પાછા PFMT_WIDTHARG;
        }
        જો(width >= MAX_PARAMETERS)
          પાછા PFMT_MANYARGS;
        જો(width >= max_param)
          max_param = width;

        in[width].type = MTYPE_WIDTH;
        /* mark as used */
        mark_arg_used(usedinput, width);
      }

      જો(flags & FLAGS_PRECPARAM) {
        જો(precision < 0)
          precision = param_અંક++;
        નહીંતર {
          /* if this identifies a parameter already used, this is illegal */
          જો(is_arg_used(usedinput, precision))
            પાછા PFMT_PRECARG;
        }
        જો(precision >= MAX_PARAMETERS)
          પાછા PFMT_MANYARGS;
        જો(precision >= max_param)
          max_param = precision;

        in[precision].type = MTYPE_PRECISION;
        mark_arg_used(usedinput, precision);
      }

      /* Handle the specifier */
      જો(param < 0)
        param = param_અંક++;
      જો(param >= MAX_PARAMETERS)
        પાછા PFMT_MANYARGS;
      જો(param >= max_param)
        max_param = param;

      iptr = &in[param];
      iptr->type = type;

      /* mark this input as used */
      mark_arg_used(usedinput, param);

      fmt++;
      optr = &out[ocount++];
      જો(ocount > MAX_SEGMENTS)
        પાછા PFMT_MANYSEGS;
      optr->આગત = (unsigned પૂર્ણાંક)param;
      optr->flags = flags;
      optr->width = width;
      optr->precision = precision;
      optr->start = start;
      optr->outlen = outlen;
      start = fmt;
    }
    નહીંતર
      fmt++;
  }

  /* is there a trailing piece */
  outlen = (size_t)(fmt - start);
  જો(outlen) {
    optr = &out[ocount++];
    જો(ocount > MAX_SEGMENTS)
      પાછા PFMT_MANYSEGS;
    optr->આગત = 0;
    optr->flags = FLAGS_SUBSTR;
    optr->start = start;
    optr->outlen = outlen;
  }

  /* Read the arg list parameters into our data list */
  માટે(i = 0; i < max_param + 1; i++) {
    સંરચના va_આગત *iptr = &in[i];
    જો(!is_arg_used(usedinput, i))
      /* bad input */
      પાછા PFMT_INPUTGAP;

    /* based on the type, read the correct argument */
    પસંદગી(iptr->type) {
    સ્થિતિ MTYPE_STRING:
      iptr->val.str = va_arg(arglist, અચલ અક્ષર *);
      તોડો;

    સ્થિતિ MTYPE_INTPTR:
    સ્થિતિ MTYPE_PTR:
      iptr->val.ptr = va_arg(arglist, શૂન્ય *);
      તોડો;

    સ્થિતિ MTYPE_LONGLONGU:
      iptr->val.numu = va_arg(arglist, uint64_t);
      તોડો;

    સ્થિતિ MTYPE_LONGLONG:
      iptr->val.nums = va_arg(arglist, int64_t);
      તોડો;

    સ્થિતિ MTYPE_LONGU:
      iptr->val.numu = va_arg(arglist, unsigned long);
      તોડો;

    સ્થિતિ MTYPE_LONG:
      iptr->val.nums = va_arg(arglist, long);
      તોડો;

    સ્થિતિ MTYPE_INTU:
      iptr->val.numu = va_arg(arglist, unsigned પૂર્ણાંક);
      તોડો;

    સ્થિતિ MTYPE_INT:
    સ્થિતિ MTYPE_WIDTH:
    સ્થિતિ MTYPE_PRECISION:
      iptr->val.nums = va_arg(arglist, પૂર્ણાંક);
      તોડો;

    સ્થિતિ MTYPE_DOUBLE:
      iptr->val.dnum = va_arg(arglist, દ્વિગુણ);
      તોડો;

    સ્થિતિ MTYPE_LONGDOUBLE:
      iptr->val.dnum = (દ્વિગુણ)va_arg(arglist, long દ્વિગુણ);
      તોડો;

    મૂળભૂત:
      DEBUGASSERT(ખાલી); /* unexpected */
      તોડો;
    }
  }
  *ipieces = max_param + 1;
  *opieces = ocount;

  પાછા PFMT_OK;
}

સંરચના mproperty {
  પૂર્ણાંક width;            /* Width of a field. */
  પૂર્ણાંક prec;             /* Precision of a field. */
  unsigned પૂર્ણાંક flags;
};

/* block output callback: consume up to 'len' bytes, return the number of
   bytes accepted. Accepting fewer than 'len' bytes aborts formatting, like
   the byte callback returning failure. May be NULL, in which case output
   falls back to the per-byte 'stream' callback. */
પ્રકારવ્યાખ્યા size_t (*mp_streamn)(અચલ unsigned અક્ષર *buf, size_t len, શૂન્ય *f);

/* commit any output the sink is holding. Returns non-zero on failure. Sinks
   whose accepted bytes are already committed do not need one. */
પ્રકારવ્યાખ્યા પૂર્ણાંક (*mp_flush)(શૂન્ય *f);

/* emit a run of bytes through the block callback when available, else byte
   by byte. Returns TRUE to abort formatting (mirroring OUTCHAR). */
સ્થિર bool stream_run(શૂન્ય *userp,
                       પૂર્ણાંક (*stream)(unsigned અક્ષર, શૂન્ય *),
                       mp_streamn streamn,
                       અચલ unsigned અક્ષર *buf, size_t len, પૂર્ણાંક *donep)
{
  જો(!len)
    પાછા FALSE;
  જો(streamn) {
    size_t n = streamn(buf, len, userp);
    *donep += (પૂર્ણાંક)n;
    પાછા n != len;
  }
  જ્યારે(len--) {
    જો(stream(*buf++, userp))
      પાછા TRUE;
    (*donep)++;
  }
  પાછા FALSE;
}

/* Padding is always spaces or zeros, so it comes from a constant rather than a
   buffer that has to be filled first. 16 keeps every width used in the tree to
   a single run; wider padding loops. */
સ્થિર અચલ અક્ષર pad_spaces[] = "                ";
સ્થિર અચલ અક્ષર pad_zeros[] = "0000000000000000";
#define PAD_RUN 16 /* strlen() of both of the above */

/* emit 'num' copies of the pad byte. Returns TRUE to abort formatting. */
સ્થિર bool stream_pad(શૂન્ય *userp,
                       પૂર્ણાંક (*stream)(unsigned અક્ષર, શૂન્ય *),
                       mp_streamn streamn,
                       unsigned અક્ષર pad, પૂર્ણાંક અંક, પૂર્ણાંક *donep)
{
  અચલ unsigned અક્ષર *src;
  જો(અંક <= 0)
    પાછા FALSE;
  જો(pad == '0')
    src = (અચલ unsigned અક્ષર *)pad_zeros;
  નહીંતર
    src = (અચલ unsigned અક્ષર *)pad_spaces;
  જ્યારે(અંક > PAD_RUN) {
    જો(stream_run(userp, stream, streamn, src, PAD_RUN, donep))
      પાછા TRUE;
    અંક -= PAD_RUN;
  }
  પાછા stream_run(userp, stream, streamn, src, (size_t)અંક, donep);
}

/* emit the leading null-terminated part of a run of at most 'len' bytes.
   Returns TRUE to abort formatting.

   A byte loop and not memchr(): 'len' is the requested precision, which
   out_string() does not clamp to the string length, so "%.64s" on a 10-byte
   string arrives with len == 64 and memchr() would read past the object. */
સ્થિર bool stream_zrun(શૂન્ય *userp,
                        પૂર્ણાંક (*stream)(unsigned અક્ષર, શૂન્ય *),
                        mp_streamn streamn,
                        અચલ અક્ષર *str, size_t len, પૂર્ણાંક *donep)
{
  size_t n = 0;
  જ્યારે(n < len && str[n])
    n++;
  પાછા stream_run(userp, stream, streamn,
                    (અચલ unsigned અક્ષર *)str, n, donep);
}

સ્થિર bool out_double(શૂન્ય *userp,
                       પૂર્ણાંક (*stream)(unsigned અક્ષર, શૂન્ય *),
                       mp_streamn streamn,
                       સંરચના mproperty *p,
                       દ્વિગુણ dnum,
                       અક્ષર *work, પૂર્ણાંક *donep)
{
  અક્ષર fmt[32] = "%";
  અક્ષર *fptr = &fmt[1];
  size_t ડાબું = કદ(fmt) - strlen(fmt);
  પૂર્ણાંક flags = p->flags;
  પૂર્ણાંક width = p->width;
  પૂર્ણાંક prec = p->prec;

  જો(flags & FLAGS_ડાબું)
    *fptr++ = '-';
  જો(flags & FLAGS_SHOWSIGN)
    *fptr++ = '+';
  જો(flags & FLAGS_SPACE)
    *fptr++ = ' ';
  જો(flags & FLAGS_ALT)
    *fptr++ = '#';

  *fptr = 0;

  જો(width >= 0) {
    size_t dlen;
    જો(width >= BUFFSIZE)
      width = BUFFSIZE - 1;
    /* RECURSIVE USAGE */
    dlen = (size_t)curl_msnprintf(fptr, ડાબું, "%d", width);
    fptr += dlen;
    ડાબું -= dlen;
  }
  જો(prec >= 0) {
    /* for each digit in the integer part, we can have one less
       precision */
    પૂર્ણાંક maxprec = BUFFSIZE - 1;
    દ્વિગુણ val = dnum;
    પૂર્ણાંક len;
    જો(prec > maxprec)
      prec = maxprec - 1;
    જો(width > 0 && prec <= width)
      maxprec -= width;
    જ્યારે(maxprec && (val >= 10.0)) {
      val /= 10;
      maxprec--;
    }

    જો(prec > maxprec)
      prec = maxprec - 1;
    જો(prec < 0)
      prec = 0;
    /* RECURSIVE USAGE */
    len = curl_msnprintf(fptr, ડાબું, ".%d", prec);
    fptr += len;
  }
  જો(flags & FLAGS_LONG)
    *fptr++ = 'l';

  જો(flags & FLAGS_FLOATE)
    *fptr++ = (અક્ષર)((flags & FLAGS_UPPER) ? 'E' : 'e');
  નહીંતર જો(flags & FLAGS_FLOATG)
    *fptr++ = (અક્ષર)((flags & FLAGS_UPPER) ? 'G' : 'g');
  નહીંતર
    *fptr++ = (flags & FLAGS_UPPER) ? 'F' : 'f';

  *fptr = 0; /* and a final null-termination */

  /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
     output characters */
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
#ifdef _WIN32
  curlx_win32_snprintf(work, BUFFSIZE, fmt, dnum);
#નહીંતર
  /* !checksrc! disable BANNEDFUNC 1 */
  snprintf(work, BUFFSIZE, fmt, dnum);
#endif
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic કાઢો
#endif
  DEBUGASSERT(strlen(work) < BUFFSIZE);
  પાછા stream_run(userp, stream, streamn,
                    (અચલ unsigned અક્ષર *)work, strlen(work), donep);
}

સ્થિર bool out_સંખ્યા(શૂન્ય *userp,
                       પૂર્ણાંક (*stream)(unsigned અક્ષર, શૂન્ય *),
                       mp_streamn streamn,
                       સંરચના mproperty *p,
                       uint64_t અંક,
                       int64_t nums,
                       અક્ષર *work, પૂર્ણાંક *donep)
{
  અચલ unsigned અક્ષર *digits = Curl_ldigits;
  પૂર્ણાંક flags = p->flags;
  પૂર્ણાંક width = p->width;
  પૂર્ણાંક prec = p->prec;
  bool is_alt = flags & FLAGS_ALT;
  bool is_neg = FALSE;
  પૂર્ણાંક base = 10;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (FALSE?) warning Coverity gives us
     otherwise */
  અક્ષર *workend = &work[BUFFSIZE - 2];
  અક્ષર *w;

  જો(flags & FLAGS_CHAR) {
    /* Character. */
    જો(!(flags & FLAGS_ડાબું)) {
      જો(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        પાછા TRUE;
      width = 1;
    }
    OUTCHAR((અક્ષર)અંક);
    જો(flags & FLAGS_ડાબું) {
      જો(stream_pad(userp, stream, streamn, ' ', width - 1, donep))
        પાછા TRUE;
    }
    પાછા FALSE;
  }
  જો(flags & FLAGS_OCTAL)
    /* Octal unsigned integer */
    base = 8;

  નહીંતર જો(flags & FLAGS_HEX) {
    /* Hexadecimal unsigned integer */
    digits = (flags & FLAGS_UPPER) ? Curl_udigits : Curl_ldigits;
    base = 16;
  }
  નહીંતર જો(flags & FLAGS_UNSIGNED)
    /* Decimal unsigned integer */
    ;

  નહીંતર {
    /* Decimal integer. */
    is_neg = (nums < 0);
    જો(is_neg) {
      /* signed_num might fail to hold absolute negative minimum by 1 */
      int64_t signed_અંક; /* Used to convert negative in positive. */
      signed_અંક = nums + (int64_t)1;
      signed_અંક = -signed_અંક;
      અંક = (uint64_t)signed_અંક;
      અંક += (uint64_t)1;
    }
  }

  /* Supply a default precision if none was given. */
  જો(prec == -1)
    prec = 1;

  /* Put the number in WORK. */
  w = workend;
  DEBUGASSERT(base <= 16);
  પસંદગી(base) {
  સ્થિતિ 10:
    જ્યારે(અંક > 0) {
      *w-- = (અક્ષર)('0' + (અંક % 10));
      અંક /= 10;
    }
    તોડો;
  મૂળભૂત:
    જ્યારે(અંક > 0) {
      *w-- = digits[અંક % base];
      અંક /= base;
    }
    તોડો;
  }
  width -= (પૂર્ણાંક)(workend - w);
  prec -= (પૂર્ણાંક)(workend - w);

  જો(is_alt && base == 8 && prec <= 0) {
    *w-- = '0';
    --width;
  }

  જો(prec > 0) {
    width -= prec;
    જ્યારે(prec-- > 0 && w >= work)
      *w-- = '0';
  }

  જો(is_alt && base == 16)
    width -= 2;

  જો(is_neg || (flags & FLAGS_SHOWSIGN) || (flags & FLAGS_SPACE))
    --width;

  જો(!(flags & FLAGS_ડાબું) && !(flags & FLAGS_PAD_NIL)) {
    જો(stream_pad(userp, stream, streamn, ' ', width, donep))
      પાછા TRUE;
    width = 0;
  }

  જો(is_neg)
    OUTCHAR('-');
  નહીંતર જો(flags & FLAGS_SHOWSIGN)
    OUTCHAR('+');
  નહીંતર જો(flags & FLAGS_SPACE)
    OUTCHAR(' ');

  જો(is_alt && base == 16) {
    OUTCHAR('0');
    જો(flags & FLAGS_UPPER)
      OUTCHAR('X');
    નહીંતર
      OUTCHAR('x');
  }

  જો(!(flags & FLAGS_ડાબું) && (flags & FLAGS_PAD_NIL)) {
    જો(stream_pad(userp, stream, streamn, '0', width, donep))
      પાછા TRUE;
    width = 0;
  }

  /* Write the number. */
  જો(stream_run(userp, stream, streamn, (અચલ unsigned અક્ષર *)w + 1,
                (size_t)(workend - w), donep))
    પાછા TRUE;

  જો(flags & FLAGS_ડાબું) {
    જો(stream_pad(userp, stream, streamn, ' ', width, donep))
      પાછા TRUE;
  }

  પાછા FALSE;
}

સ્થિર અચલ અક્ષર nilstr[] = "(nil)";

સ્થિર bool out_string(શૂન્ય *userp,
                       પૂર્ણાંક (*stream)(unsigned અક્ષર, શૂન્ય *),
                       mp_streamn streamn,
                       સંરચના mproperty *p,
                       અચલ અક્ષર *str,
                       પૂર્ણાંક *donep)
{
  પૂર્ણાંક flags = p->flags;
  પૂર્ણાંક width = p->width;
  પૂર્ણાંક prec = p->prec;
  size_t len;

  જો(!str) {
    /* Write null string if there is space. */
    જો(prec == -1 || prec >= (પૂર્ણાંક)CURL_CSTRLEN(nilstr)) {
      str = nilstr;
      len = CURL_CSTRLEN(nilstr);
      /* Disable quotes around (nil) */
      flags &= ~(unsigned પૂર્ણાંક)FLAGS_ALT;
    }
    નહીંતર {
      str = "";
      len = 0;
    }
  }
  નહીંતર જો(prec != -1)
    len = (size_t)prec;
  નહીંતર જો(*str == '\0')
    len = 0;
  નહીંતર
    len = strlen(str);

  width -= (len > INT_MAX) ? INT_MAX : (પૂર્ણાંક)len;

  જો(flags & FLAGS_ALT)
    OUTCHAR('"');

  જો(!(flags & FLAGS_ડાબું)) {
    જો(stream_pad(userp, stream, streamn, ' ', width, donep))
      પાછા TRUE;
    width = 0;
  }

  /* With no precision, len is already the exact length; emit it without
     scanning again. With precision, scan up to len but stop at NUL, since
     the string may be shorter or the buffer may not be null-terminated. */
  જો(prec == -1) {
    જો(stream_run(userp, stream, streamn,
                  (અચલ unsigned અક્ષર *)str, len, donep))
      પાછા TRUE;
  }
  નહીંતર જો(stream_zrun(userp, stream, streamn, str, len, donep))
    પાછા TRUE;
  જો(flags & FLAGS_ડાબું) {
    જો(stream_pad(userp, stream, streamn, ' ', width, donep))
      પાછા TRUE;
  }

  જો(flags & FLAGS_ALT)
    OUTCHAR('"');

  પાછા FALSE;
}

સ્થિર bool out_pointer(શૂન્ય *userp,
                        પૂર્ણાંક (*stream)(unsigned અક્ષર, શૂન્ય *),
                        mp_streamn streamn,
                        સંરચના mproperty *p,
                        અચલ અક્ષર *ptr,
                        અક્ષર *work,
                        પૂર્ણાંક *donep)
{
  /* Generic pointer. */
  જો(ptr) {
    size_t અંક = (size_t)ptr;

    /* If the pointer is not NULL, write it as a %#x spec. */
    p->flags |= FLAGS_HEX | FLAGS_ALT;
    જો(out_સંખ્યા(userp, stream, streamn, p, અંક, 0, work, donep))
      પાછા TRUE;
  }
  નહીંતર {
    /* Write "(nil)" for a nil pointer. */
    પૂર્ણાંક width = p->width;
    પૂર્ણાંક flags = p->flags;

    width -= (પૂર્ણાંક)CURL_CSTRLEN(nilstr);
    જો(flags & FLAGS_ડાબું) {
      જો(stream_pad(userp, stream, streamn, ' ', width, donep))
        પાછા TRUE;
      width = 0;
    }
    જો(stream_run(userp, stream, streamn, (અચલ unsigned અક્ષર *)nilstr,
                  કદ(nilstr) - 1, donep))
      પાછા TRUE;
    જો(!(flags & FLAGS_ડાબું)) {
      જો(stream_pad(userp, stream, streamn, ' ', width, donep))
        પાછા TRUE;
    }
  }
  પાછા FALSE;
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
સ્થિર પૂર્ણાંક formatf(શૂન્ય *userp, /* untouched by format(), sent to the
                                   stream() function in the second argument */
                   /* function pointer called for each output character */
                   પૂર્ણાંક (*stream)(unsigned અક્ષર, શૂન્ય *),
                   /* optional block-output function pointer */
                   mp_streamn streamn,
                   /* optional, commits output the sink is holding */
                   mp_flush flush,
                   અચલ અક્ષર *format, /* %-formatted string */
                   va_list ap_save) /* list of parameters */
{
  પૂર્ણાંક done = 0;   /* number of characters written */
  પૂર્ણાંક i;
  પૂર્ણાંક ocount = 0; /* number of output segments */
  પૂર્ણાંક icount = 0; /* number of input arguments */

  સંરચના outsegment output[MAX_SEGMENTS];
  સંરચના va_આગત આગત[MAX_PARAMETERS];
  અક્ષર work[BUFFSIZE + 2];

  /* Parse the format string */
  જો(parsefmt(format, output, આગત, &ocount, &icount, ap_save))
    પાછા 0;

  માટે(i = 0; i < ocount; i++) {
    સંરચના outsegment *optr = &output[i];
    સંરચના va_આગત *iptr = &આગત[optr->આગત];
    સંરચના mproperty p;
    size_t outlen = optr->outlen;

    જો(outlen) {
      જો(stream_zrun(userp, stream, streamn, optr->start, outlen, &done))
        પાછા done;
      જો(optr->flags & FLAGS_SUBSTR)
        /* this is a substring */
        ચાલુ;
    }

    p.flags = optr->flags;

    /* pick up the specified width */
    જો(p.flags & FLAGS_WIDTHPARAM) {
      p.width = (પૂર્ણાંક)આગત[optr->width].val.nums;
      જો(p.width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        જો(p.width == INT_MIN)
          p.width = INT_MAX;
        નહીંતર
          p.width = -p.width;
        p.flags |= FLAGS_ડાબું;
        p.flags &= ~(unsigned પૂર્ણાંક)FLAGS_PAD_NIL;
      }
    }
    નહીંતર
      p.width = optr->width;

    /* pick up the specified precision */
    જો(p.flags & FLAGS_PRECPARAM) {
      p.prec = (પૂર્ણાંક)આગત[optr->precision].val.nums;
      જો(p.prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        p.prec = -1;
    }
    નહીંતર જો(p.flags & FLAGS_PREC)
      p.prec = optr->precision;
    નહીંતર
      p.prec = -1;

    પસંદગી(iptr->type) {
    સ્થિતિ MTYPE_INTU:
    સ્થિતિ MTYPE_LONGU:
    સ્થિતિ MTYPE_LONGLONGU:
      p.flags |= FLAGS_UNSIGNED;
      જો(out_સંખ્યા(userp, stream, streamn, &p, iptr->val.numu, 0, work,
                    &done))
        પાછા done;
      તોડો;

    સ્થિતિ MTYPE_INT:
    સ્થિતિ MTYPE_LONG:
    સ્થિતિ MTYPE_LONGLONG:
      જો(out_સંખ્યા(userp, stream, streamn, &p, iptr->val.numu,
                    iptr->val.nums, work, &done))
        પાછા done;
      તોડો;

    સ્થિતિ MTYPE_STRING:
      જો(out_string(userp, stream, streamn, &p, iptr->val.str, &done))
        પાછા done;
      તોડો;

    સ્થિતિ MTYPE_PTR:
      જો(out_pointer(userp, stream, streamn, &p, iptr->val.ptr, work,
                     &done))
        પાછા done;
      તોડો;

    સ્થિતિ MTYPE_DOUBLE:
    સ્થિતિ MTYPE_LONGDOUBLE:
      જો(out_double(userp, stream, streamn, &p, iptr->val.dnum, work,
                    &done))
        પાછા done;
      તોડો;

    સ્થિતિ MTYPE_INTPTR:
      /* %n is observable by the caller, so commit what is pending first and
         perform it only if that succeeded, as a sink that never holds output
         would. */
      જો(flush && flush(userp))
        પાછા done;
      /* Answer the count of characters written. */
      જો(p.flags & FLAGS_LONGLONG)
        *(int64_t *)iptr->val.ptr = (int64_t)done;
      નહીંતર જો(p.flags & FLAGS_LONG)
        *(long *)iptr->val.ptr = (long)done;
      નહીંતર જો(!(p.flags & FLAGS_SHORT))
        *(પૂર્ણાંક *)iptr->val.ptr = done;
      નહીંતર
        *(short *)iptr->val.ptr = (short)done;
      તોડો;

    મૂળભૂત:
      તોડો;
    }
  }
  પાછા done;
}

/* fputc() look-alike */
સ્થિર પૂર્ણાંક addbyter(unsigned અક્ષર outc, શૂન્ય *f)
{
  સંરચના nsprintf *infop = f;
  જો(infop->length < infop->max) {
    /* only do this if we have not reached max length yet */
    *infop->buffer++ = (અક્ષર)outc; /* store */
    infop->length++; /* we are now one byte larger */
    પાછા 0;     /* fputc() returns like this on success */
  }
  પાછા 1;
}

/* block variant of addbyter */
સ્થિર size_t addrun(અચલ unsigned અક્ષર *buf, size_t len, શૂન્ય *f)
{
  સંરચના nsprintf *infop = f;
  size_t fit = infop->max - infop->length;
  જો(len > fit)
    len = fit;
  જો(len) {
    memcpy(infop->buffer, buf, len);
    infop->buffer += len;
    infop->length += len;
  }
  પાછા len;
}

પૂર્ણાંક curl_mvsnprintf(અક્ષર *buffer, size_t maxlength, અચલ અક્ષર *format,
                    va_list args)
{
  પૂર્ણાંક retcode;
  સંરચના nsprintf info;

  info.buffer = buffer;
  info.length = 0;
  info.max = maxlength;

  retcode = formatf(&info, addbyter, addrun, ખાલી, format, args);
  જો(info.max) {
    /* we null-terminate this */
    જો(info.max == info.length) {
      /* we are at maximum, scrap the last letter */
      info.buffer[-1] = 0;
      DEBUGASSERT(retcode);
      retcode--; /* do not count the NUL byte */
    }
    નહીંતર
      info.buffer[0] = 0;
  }
  પાછા retcode;
}

પૂર્ણાંક curl_msnprintf(અક્ષર *buffer, size_t maxlength, અચલ અક્ષર *format, ...)
{
  પૂર્ણાંક retcode;
  va_list args; /* argument pointer */
  va_start(args, format);
  retcode = curl_mvsnprintf(buffer, maxlength, format, args);
  va_end(args);
  પાછા retcode;
}

/* append the staged bytes to the dynbuf. Returns 1 on failure (and sets
   merr), 0 on success. */
સ્થિર પૂર્ણાંક alloc_flush(શૂન્ય *f)
{
  સંરચના asprintf *infop = f;
  જો(infop->nstage) {
    CURLcode result = curlx_dyn_addn(infop->b, infop->stage, infop->nstage);
    infop->nstage = 0;
    જો(result) {
      infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
      પાછા 1; /* fail */
    }
  }
  પાછા 0;
}

/* append directly, bypassing the stage. Returns 1 on failure. */
સ્થિર પૂર્ણાંક alloc_addn(સંરચના asprintf *infop, અચલ unsigned અક્ષર *buf,
                      size_t len)
{
  CURLcode result = curlx_dyn_addn(infop->b, buf, len);
  જો(result) {
    infop->merr = result == CURLE_TOO_LARGE ? MERR_TOO_LARGE : MERR_MEM;
    પાછા 1; /* fail */
  }
  પાછા 0;
}

/* TRUE if 'more' additional bytes still fit the dynbuf size limit, counting
   what is already staged. curlx_dyn_addn() rejects an append when
   len + leng + 1 exceeds it. */
સ્થિર bool alloc_fits(સંરચના asprintf *infop, size_t more)
{
  પાછા (infop->b->leng + infop->nstage + more + 1) <= infop->b->toobig;
}

/* fputc() look-alike */
સ્થિર પૂર્ણાંક alloc_addbyter(unsigned અક્ષર outc, શૂન્ય *f)
{
  સંરચના asprintf *infop = f;
  જો(infop->nstage == કદ(infop->stage) && alloc_flush(infop))
    પાછા 1; /* fail */
  જો(!alloc_fits(infop, 1)) {
    /* Does not fit the dynbuf size limit. Flush and append this byte on
       its own, so the limit is reported at exactly this byte and formatting
       stops where it would without staging. %n depends on that. */
    જો(alloc_flush(infop))
      પાછા 1;
    પાછા alloc_addn(infop, &outc, 1);
  }
  infop->stage[infop->nstage++] = outc;
  પાછા 0;
}

/* block variant of alloc_addbyter */
સ્થિર size_t alloc_addrun(અચલ unsigned અક્ષર *buf, size_t len, શૂન્ય *f)
{
  સંરચના asprintf *infop = f;
  size_t accepted = 0;
  જ્યારે(accepted < len) {
    size_t room = કદ(infop->stage) - infop->nstage;
    size_t n;
    જો(!room) {
      જો(alloc_flush(infop))
        પાછા accepted; /* accepted < len aborts formatting */
      room = કદ(infop->stage);
    }
    n = len - accepted;
    જો(n > room)
      n = room;
    જો(!alloc_fits(infop, n)) {
      /* Run does not fit the dynbuf size limit. Stage what does, then
         append the first byte that does not on its own, so the limit is
         reported at exactly that byte. */
      size_t fits = infop->b->toobig - infop->b->leng - infop->nstage - 1;
      જો(fits) {
        memcpy(&infop->stage[infop->nstage], buf + accepted, fits);
        infop->nstage += fits;
        accepted += fits;
      }
      જો(alloc_flush(infop))
        પાછા accepted;
      જો(alloc_addn(infop, buf + accepted, 1))
        પાછા accepted; /* accepted < len aborts formatting */
      accepted++;
      ચાલુ;
    }
    memcpy(&infop->stage[infop->nstage], buf + accepted, n);
    infop->nstage += n;
    accepted += n;
  }
  પાછા accepted;
}

/* appends the formatted string, returns MERR error code */
પૂર્ણાંક curlx_dyn_vprintf(સંરચના dynbuf *dyn, અચલ અક્ષર *format, va_list args)
{
  સંરચના asprintf info;
  info.b = dyn;
  info.merr = MERR_OK;
  info.nstage = 0;

  (શૂન્ય)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  જો(!info.merr)
    (શૂન્ય)alloc_flush(&info);
  જો(info.merr) {
    curlx_dyn_free(info.b);
    પાછા info.merr;
  }
  પાછા 0;
}

અક્ષર *curl_mvaprintf(અચલ અક્ષર *format, va_list args)
{
  સંરચના asprintf info;
  સંરચના dynbuf dyn;
  info.b = &dyn;
  curlx_dyn_init(info.b, DYN_APRINTF);
  info.merr = MERR_OK;
  info.nstage = 0;

  (શૂન્ય)formatf(&info, alloc_addbyter, alloc_addrun, alloc_flush,
                format, args);
  જો(!info.merr)
    (શૂન્ય)alloc_flush(&info);
  જો(info.merr) {
    curlx_dyn_free(info.b);
    પાછા ખાલી;
  }
  જો(curlx_dyn_len(info.b))
    પાછા curlx_dyn_ptr(info.b);
  પાછા curlx_strdup("");
}

અક્ષર *curl_maprintf(અચલ અક્ષર *format, ...)
{
  va_list args;
  અક્ષર *s;
  va_start(args, format);
  s = curl_mvaprintf(format, args);
  va_end(args);
  પાછા s;
}

સ્થિર પૂર્ણાંક storebuffer(unsigned અક્ષર outc, શૂન્ય *f)
{
  અક્ષર **buffer = f;
  **buffer = (અક્ષર)outc;
  (*buffer)++;
  પાછા 0;
}

/* block variant of storebuffer */
સ્થિર size_t storerun(અચલ unsigned અક્ષર *buf, size_t len, શૂન્ય *f)
{
  અક્ષર **buffer = f;
  memcpy(*buffer, buf, len);
  *buffer += len;
  પાછા len;
}

પૂર્ણાંક curl_msprintf(અક્ષર *buffer, અચલ અક્ષર *format, ...)
{
  va_list args; /* argument pointer */
  પૂર્ણાંક retcode;
  va_start(args, format);
  retcode = formatf(&buffer, storebuffer, storerun, ખાલી, format, args);
  va_end(args);
  *buffer = 0; /* we null-terminate this */
  પાછા retcode;
}

સ્થિર પૂર્ણાંક fputc_wrapper(unsigned અક્ષર outc, શૂન્ય *f)
{
  સંરચના ioprintf *i = f;
  પૂર્ણાંક out = outc;
  FILE *s = i->fd;
  પૂર્ણાંક rc = fputc(out, s);
  i->error = (rc == EOF);
  પાછા (rc == EOF);
}

/* block variant of fputc_wrapper */
સ્થિર size_t fwrite_wrapper(અચલ unsigned અક્ષર *buf, size_t len, શૂન્ય *f)
{
  સંરચના ioprintf *i = f;
  size_t n;
  FILE *s = i->fd;
  n = fwrite(buf, 1, len, s);
  i->error = (n != len);
  પાછા n;
}

પૂર્ણાંક curl_mvfprintf(FILE *fd, અચલ અક્ષર *format, va_list args)
{
  સંરચના ioprintf info;
  પૂર્ણાંક n;
  info.fd = fd;
  info.error = FALSE;
  n = formatf(&info, fputc_wrapper, fwrite_wrapper, ખાલી, format, args);
  જો(info.error)
    પાછા -1;
  પાછા n;
}

પૂર્ણાંક curl_mfprintf(FILE *fd, અચલ અક્ષર *format, ...)
{
  va_list args; /* argument pointer */
  પૂર્ણાંક n;
  va_start(args, format);
  n = curl_mvfprintf(fd, format, args);
  va_end(args);
  પાછા n;
}

પૂર્ણાંક curl_mprintf(અચલ અક્ષર *format, ...)
{
  va_list args; /* argument pointer */
  પૂર્ણાંક n;
  va_start(args, format);
  n = curl_mvfprintf(stdout, format, args);
  va_end(args);
  પાછા n;
}

પૂર્ણાંક curl_mvsprintf(અક્ષર *buffer, અચલ અક્ષર *format, va_list args)
{
  પૂર્ણાંક retcode = formatf(&buffer, storebuffer, storerun, ખાલી, format, args);
  *buffer = 0; /* we null-terminate this */
  પાછા retcode;
}

પૂર્ણાંક curl_mvprintf(અચલ અક્ષર *format, va_list args)
{
  પાછા curl_mvfprintf(stdout, format, args);
}

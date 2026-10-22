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
#অন্তর্ভাব "curl_setup.h"

কাঠামো Curl_easy;

#অন্তর্ভাব "mime.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/basename.h"
#অন্তর্ভাব "curlx/strcopy.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curlx/base64.h"

#যদি !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "slist.h"
#অন্তর্ভাব "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

স্থির size_t mime_subparts_read(অক্ষর *বাফার, size_t আকার_2, size_t nitems,
                                 শূন্য *instream, bool *hasread);
স্থির curl_off_t mime_আকার(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
স্থির ধ্রুবক unsigned অক্ষর qp_class[] = {
  0,     0,     0,     0,     0,     0,     0,     0,            /* 00 - 07 */
  0,     QP_SP, QP_LF, 0,     0,     QP_CR, 0,     0,            /* 08 - 0F */
  0,     0,     0,     0,     0,     0,     0,     0,            /* 10 - 17 */
  0,     0,     0,     0,     0,     0,     0,     0,            /* 18 - 1F */
  QP_SP, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 20 - 27 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 28 - 2F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 30 - 37 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, 0    , QP_OK, QP_OK,        /* 38 - 3F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 40 - 47 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 48 - 4F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 50 - 57 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 58 - 5F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 60 - 67 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 68 - 6F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 70 - 77 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, 0,            /* 78 - 7F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* 80 - 8F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* 90 - 9F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* A0 - AF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* B0 - BF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* C0 - CF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* D0 - DF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* E0 - EF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0                 /* F0 - FF */
};

/* Binary --> hexadecimal ASCII table. */
স্থির ধ্রুবক অক্ষর aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_তথ্য) stat_তথ্য.st_আকার
#define fopen_read                curlx_fopen

#অন্যথা

#অন্তর্ভাব <fabdef.h>
/*
 * get_vms_file_size does what it takes to get the real size of the file
 *
 * For fixed files, find out the size of the EOF block and adjust.
 *
 * For all others, have to read the entire file in, discarding the contents.
 * Most posted text files will be small, and binary files like zlib archives
 * and CD/DVD images should be either a STREAM_LF format or a fixed format.
 *
 */
curl_off_t Vms_Real_File_আকার(ধ্রুবক অক্ষর *name, ধ্রুবক curlx_struct_stat *stat_buf)
{
  অক্ষর বাফার[8192];
  curl_off_t গুনতি;
  পূর্ণসংখ্যা ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  যদি(!file)
    ফেরত 0;

  গুনতি = 0;
  ret_stat = 1;
  যখন(ret_stat > 0) {
    ret_stat = fread(বাফার, 1, আকার(বাফার), file);
    যদি(ret_stat)
      গুনতি += ret_stat;
  }
  curlx_fclose(file);

  ফেরত গুনতি;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
স্থির curl_off_t Vms_Special_আকার(ধ্রুবক অক্ষর *name,
                                 ধ্রুবক curlx_struct_stat *stat_buf)
{
  নির্বাচন(stat_buf->st_fab_rfm) {
  ক্ষেত্র FAB$C_VAR:
  ক্ষেত্র FAB$C_VFC:
    ফেরত Vms_Real_File_আকার(name, stat_buf);
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত stat_buf->st_আকার;
  }
}

#define filesize(name, stat_তথ্য) Vms_Special_আকার(name, &stat_তথ্য)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
স্থির FILE *vmsfopenread(ধ্রুবক অক্ষর *file, ধ্রুবক অক্ষর *mode)
{
  curlx_struct_stat statbuf;
  পূর্ণসংখ্যা res = curlx_stat(file, &statbuf);

  যদি(res != -1) {
    নির্বাচন(statbuf.st_fab_rfm) {
    ক্ষেত্র FAB$C_VAR:
    ক্ষেত্র FAB$C_VFC:
    ক্ষেত্র FAB$C_STMCR:
      ফেরত curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      বিরতি;
    পূর্বনির্ধারিত:
      ফেরত curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  ফেরত ফাঁকা;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
স্থির শূন্য mimesetstate(কাঠামো mime_state *state,
                         গণনা mimestate tok, শূন্য *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
স্থির অক্ষর *escape_string(কাঠামো Curl_easy *তথ্য,
                           ধ্রুবক অক্ষর *src, গণনা mimestrategy strategy)
{
  CURLcode ফলাফল;
  কাঠামো dynbuf db;
  ধ্রুবক অক্ষর * ধ্রুবক *table;
  ধ্রুবক অক্ষর * ধ্রুবক *p;
  /* replace first character by rest of string. */
  স্থির ধ্রুবক অক্ষর * ধ্রুবক mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    ফাঁকা
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  স্থির ধ্রুবক অক্ষর * ধ্রুবক formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    ফাঁকা
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  যদি(strategy == MIMESTRATEGY_MAIL || (তথ্য && তথ্য->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_প্রবেশ_দৈর্ঘ্য);

  জন্য(ফলাফল = curlx_dyn_addn(&db, STRCONST("")); !ফলাফল && *src; src++) {
    জন্য(p = table; *p && **p != *src; p++)
      ;

    যদি(*p)
      ফলাফল = curlx_dyn_add(&db, *p + 1);
    অন্যথা
      ফলাফল = curlx_dyn_addn(&db, src, 1);
  }

  ফেরত curlx_dyn_ptr(&db);
}

/* Check if header matches. */
স্থির অক্ষর *match_header(কাঠামো curl_slist *hdr, ধ্রুবক অক্ষর *lbl, size_t len)
{
  অক্ষর *মান = ফাঁকা;

  যদি(curl_strnequal(hdr->তথ্য, lbl, len) && hdr->তথ্য[len] == ':')
    জন্য(মান = hdr->তথ্য + len + 1; *মান == ' '; মান++)
      ;
  ফেরত মান;
}

/* Get a header from an slist. */
স্থির অক্ষর *অনুসন্ধান_header(কাঠামো curl_slist *hdrlist,
                           ধ্রুবক অক্ষর *hdr, size_t len)
{
  অক্ষর *মান = ফাঁকা;

  জন্য(; !মান && hdrlist; hdrlist = hdrlist->পরবর্তী)
    মান = match_header(hdrlist, hdr, len);

  ফেরত মান;
}

স্থির অক্ষর *strippath(ধ্রুবক অক্ষর *fullfile)
{
  অক্ষর *filename;
  অক্ষর *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  যদি(!filename)
    ফেরত ফাঁকা;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  ফেরত base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
স্থির শূন্য cleanup_encoder_state(কাঠামো mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
স্থির size_t encoder_nop_read(অক্ষর *বাফার, size_t আকার_2, bool ateof,
                               কাঠামো curl_mimepart *part)
{
  কাঠামো mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (শূন্য)ateof;

  যদি(!আকার_2)
    ফেরত STOP_FILLING;

  যদি(আকার_2 > insize)
    আকার_2 = insize;

  যদি(আকার_2)
    memcpy(বাফার, st->buf + st->bufbeg, আকার_2);

  st->bufbeg += আকার_2;
  ফেরত আকার_2;
}

স্থির curl_off_t encoder_nop_আকার(curl_mimepart *part)
{
  ফেরত part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
স্থির size_t encoder_7bit_read(অক্ষর *বাফার, size_t আকার_2, bool ateof,
                                curl_mimepart *part)
{
  কাঠামো mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (শূন্য)ateof;

  যদি(!আকার_2)
    ফেরত STOP_FILLING;

  যদি(আকার_2 > cursize)
    আকার_2 = cursize;

  জন্য(cursize = 0; cursize < আকার_2; cursize++) {
    *বাফার = st->buf[st->bufbeg];
    যদি(*বাফার++ & 0x80)
      ফেরত cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  ফেরত cursize;
}

/* Base64 content encoder. */
স্থির size_t encoder_base64_read(অক্ষর *বাফার, size_t আকার_2, bool ateof,
                                  curl_mimepart *part)
{
  কাঠামো mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  পূর্ণসংখ্যা i;
  অক্ষর *ptr = বাফার;

  যখন(st->bufbeg < st->bufend) {
    /* Line full ? */
    যদি(st->pos > MAX_ENCODED_সারি_দৈর্ঘ্য - 4) {
      /* Yes, we need 2 characters for CRLF. */
      যদি(আকার_2 < 2) {
        যদি(!cursize)
          ফেরত STOP_FILLING;
        বিরতি;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      আকার_2 -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    যদি(আকার_2 < 4) {
      যদি(!cursize)
        ফেরত STOP_FILLING;
      বিরতি;
    }
    যদি(st->bufend - st->bufbeg < 3)
      বিরতি;

    /* Encode three bytes as four characters. */
    i = st->buf[st->bufbeg++] & 0xFF;
    i = (i << 8) | (st->buf[st->bufbeg++] & 0xFF);
    i = (i << 8) | (st->buf[st->bufbeg++] & 0xFF);
    *ptr++ = curlx_base64encdec[(i >> 18) & 0x3F];
    *ptr++ = curlx_base64encdec[(i >> 12) & 0x3F];
    *ptr++ = curlx_base64encdec[(i >> 6) & 0x3F];
    *ptr++ = curlx_base64encdec[i & 0x3F];
    cursize += 4;
    st->pos += 4;
    আকার_2 -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  যদি(ateof) {
    যদি(আকার_2 < 4) {
      যদি(!cursize)
        ফেরত STOP_FILLING;
    }
    অন্যথা {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      যদি(st->bufend != st->bufbeg) {

        যদি(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        যদি(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  ফেরত cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_প্রবেশ_আকার                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_সারি_দৈর্ঘ্য + 2)) *                  \
    MAX_ENCODED_সারি_দৈর্ঘ্য / 4) * 3 - 3)

স্থির curl_off_t encoder_base64_আকার(curl_mimepart *part)
{
  curl_off_t আকার_2 = part->datasize;

  যদি(আকার_2 <= 0)
    ফেরত আকার_2;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  যদি(আকার_2 > BASE64_MAX_প্রবেশ_আকার)
    ফেরত -1;

  /* Compute base64 character count. */
  আকার_2 = 4 * (1 + ((আকার_2 - 1) / 3));

  /* Effective character count must include CRLFs. */
  ফেরত আকার_2 + (2 * ((আকার_2 - 1) / MAX_ENCODED_সারি_দৈর্ঘ্য));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
স্থির পূর্ণসংখ্যা qp_lookahead_eol(কাঠামো mime_encoder_state *st, পূর্ণসংখ্যা ateof, size_t n)
{
  n += st->bufbeg;
  যদি(n >= st->bufend && ateof)
    ফেরত 1;
  যদি(n + 2 > st->bufend)
    ফেরত ateof ? 0 : -1;
  যদি(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    ফেরত 1;
  ফেরত 0;
}

/* Quoted-printable encoder. */
স্থির size_t encoder_qp_read(অক্ষর *বাফার, size_t আকার_2, bool ateof,
                              curl_mimepart *part)
{
  কাঠামো mime_encoder_state *st = &part->encstate;
  অক্ষর *ptr = বাফার;
  size_t cursize = 0;
  পূর্ণসংখ্যা softlinebreak;
  অক্ষর buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  যখন(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    পূর্ণসংখ্যা i = (unsigned অক্ষর)st->buf[st->bufbeg];
    buf[0] = (অক্ষর)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    নির্বাচন(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    ক্ষেত্র QP_OK:          /* Not a special character. */
      বিরতি;
    ক্ষেত্র QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      নির্বাচন(qp_lookahead_eol(st, ateof, 1)) {
      ক্ষেত্র -1:          /* More input data needed. */
        ফেরত cursize;
      ক্ষেত্র 0:           /* No encoding needed. */
        বিরতি;
      পূর্বনির্ধারিত:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        বিরতি;
      }
      বিরতি;
    ক্ষেত্র QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      নির্বাচন(qp_lookahead_eol(st, ateof, 0)) {
      ক্ষেত্র -1:          /* Need more data. */
        ফেরত cursize;
      ক্ষেত্র 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        বিরতি;
      পূর্বনির্ধারিত:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        বিরতি;
      }
      বিরতি;
    পূর্বনির্ধারিত:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      বিরতি;
    }

    /* Be sure the encoded character fits within maximum line length. */
    যদি(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_সারি_দৈর্ঘ্য;
      যদি(!softlinebreak && st->pos + len == MAX_ENCODED_সারি_দৈর্ঘ্য) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        নির্বাচন(qp_lookahead_eol(st, ateof, consumed)) {
        ক্ষেত্র -1:        /* Need more data. */
          ফেরত cursize;
        ক্ষেত্র 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          বিরতি;
        }
      }
      যদি(softlinebreak) {
        curlx_strcopy(buf, আকার(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    যদি(len > আকার_2) {
      যদি(!cursize)
        ফেরত STOP_FILLING;
      বিরতি;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    আকার_2 -= len;
    st->pos += len;
    যদি(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  ফেরত cursize;
}

স্থির curl_off_t encoder_qp_আকার(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  ফেরত part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
স্থির size_t mime_mem_read(অক্ষর *বাফার, size_t আকার_2, size_t nitems,
                            শূন্য *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (শূন্য)আকার_2;  /* Always 1 */

  যদি(!nitems)
    ফেরত STOP_FILLING;

  যদি(sz > nitems)
    sz = nitems;

  যদি(sz)
    memcpy(বাফার, part->তথ্য + curlx_sotouz(part->state.offset), sz);

  ফেরত sz;
}

স্থির পূর্ণসংখ্যা mime_mem_seek(শূন্য *instream, curl_off_t offset, পূর্ণসংখ্যা whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  নির্বাচন(whence) {
  ক্ষেত্র SEEK_CUR:
    offset += part->state.offset;
    বিরতি;
  ক্ষেত্র SEEK_END:
    offset += part->datasize;
    বিরতি;
  }

  যদি(offset < 0 || offset > part->datasize)
    ফেরত CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  ফেরত CURL_SEEKFUNC_OK;
}

স্থির শূন্য mime_mem_free(শূন্য *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->তথ্য);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
স্থির bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  যদি(part->fp)
    ফেরত FALSE;
  part->fp = fopen_read(part->তথ্য, "rb");
  ফেরত part->fp ? FALSE : TRUE;
}

স্থির size_t mime_file_read(অক্ষর *বাফার, size_t আকার_2, size_t nitems,
                             শূন্য *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  যদি(!nitems)
    ফেরত STOP_FILLING;

  যদি(mime_open_file(part))
    ফেরত READ_ERROR;

  ফেরত fread(বাফার, আকার_2, nitems, part->fp);
}

স্থির পূর্ণসংখ্যা mime_file_seek(শূন্য *instream, curl_off_t offset, পূর্ণসংখ্যা whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  যদি(whence == SEEK_SET && !offset && !part->fp)
    ফেরত CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  যদি(mime_open_file(part))
    ফেরত CURL_SEEKFUNC_FAIL;

  ফেরত curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

স্থির শূন্য mime_file_free(শূন্য *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  যদি(part->fp) {
    curlx_fclose(part->fp);
    part->fp = ফাঁকা;
  }
  curlx_safefree(part->তথ্য);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
স্থির size_t readback_bytes(কাঠামো mime_state *state,
                             অক্ষর *বাফার, size_t bufsize,
                             ধ্রুবক অক্ষর *bytes, size_t numbytes,
                             ধ্রুবক অক্ষর *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  যদি(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  অন্যথা {
    sz = offset - numbytes;
    যদি(sz >= traillen)
      ফেরত 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  যদি(sz > bufsize)
    sz = bufsize;

  memcpy(বাফার, bytes, sz);
  state->offset += sz;
  ফেরত sz;
}

/* Read a non-encoded part content. */

স্থির size_t read_part_content(curl_mimepart *part, অক্ষর *বাফার,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  নির্বাচন(part->lastreadstatus) {
  ক্ষেত্র 0:
  ক্ষেত্র CURL_READFUNC_ABORT:
  ক্ষেত্র CURL_READFUNC_PAUSE:
  ক্ষেত্র READ_ERROR:
    ফেরত part->lastreadstatus;
  পূর্বনির্ধারিত:
    বিরতি;
  }

  /* If we can determine we are at end of part data, spare a read. */
  যদি(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  অন্যথা {
    নির্বাচন(part->kind) {
    ক্ষেত্র MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(বাফার, 1, bufsize, part->arg, hasread);
      বিরতি;
    ক্ষেত্র MIMEKIND_FILE:
      যদি(part->fp && feof(part->fp))
        বিরতি;  /* At EOF. */
      FALLTHROUGH();
    পূর্বনির্ধারিত:
      যদি(part->readfunc) {
        যদি(!(part->flags & MIME_FAST_READ)) {
          যদি(*hasread)
            ফেরত STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(বাফার, 1, bufsize, part->arg);
      }
      বিরতি;
    }
  }

  নির্বাচন(sz) {
  ক্ষেত্র STOP_FILLING:
    বিরতি;
  ক্ষেত্র 0:
  ক্ষেত্র CURL_READFUNC_ABORT:
  ক্ষেত্র CURL_READFUNC_PAUSE:
  ক্ষেত্র READ_ERROR:
    part->lastreadstatus = sz;
    বিরতি;
  পূর্বনির্ধারিত:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    বিরতি;
  }

  ফেরত sz;
}

/* Read and encode part content. */
স্থির size_t read_encoded_part_content(curl_mimepart *part, অক্ষর *বাফার,
                                        size_t bufsize, bool *hasread)
{
  কাঠামো mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  জন্য(;;) {
    যদি(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(বাফার, bufsize, ateof, part);
      নির্বাচন(sz) {
      ক্ষেত্র 0:
        যদি(ateof)
          ফেরত cursize;
        বিরতি;
      ক্ষেত্র READ_ERROR:
      ক্ষেত্র STOP_FILLING:
        ফেরত cursize ? cursize : sz;
      পূর্বনির্ধারিত:
        cursize += sz;
        বাফার += sz;
        bufsize -= sz;
        চালু;
      }
    }

    /* We need more data in input buffer. */
    যদি(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      যদি(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    যদি(st->bufend >= আকার(st->buf))
      ফেরত cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           আকার(st->buf) - st->bufend, hasread);
    নির্বাচন(sz) {
    ক্ষেত্র 0:
      ateof = TRUE;
      বিরতি;
    ক্ষেত্র CURL_READFUNC_ABORT:
    ক্ষেত্র CURL_READFUNC_PAUSE:
    ক্ষেত্র READ_ERROR:
    ক্ষেত্র STOP_FILLING:
      ফেরত cursize ? cursize : sz;
    পূর্বনির্ধারিত:
      st->bufend += sz;
      বিরতি;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
স্থির size_t readback_part(curl_mimepart *part,
                            অক্ষর *বাফার, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  যখন(bufsize) {
    size_t sz = 0;
    কাঠামো curl_slist *hdr = (কাঠামো curl_slist *)part->state.ptr;
    নির্বাচন(part->state.state) {
    ক্ষেত্র MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      বিরতি;
    ক্ষেত্র MIMESTATE_USERHEADERS:
      যদি(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, ফাঁকা);
        বিরতি;
      }
      যদি(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->পরবর্তী);
        বিরতি;
      }
      FALLTHROUGH();
    ক্ষেত্র MIMESTATE_CURLHEADERS:
      যদি(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      অন্যথা {
        sz = readback_bytes(&part->state, বাফার, bufsize,
                            hdr->তথ্য, strlen(hdr->তথ্য), STRCONST("\r\n"));
        যদি(!sz)
          mimesetstate(&part->state, part->state.state, hdr->পরবর্তী);
      }
      বিরতি;
    ক্ষেত্র MIMESTATE_EOH:
      sz = readback_bytes(&part->state, বাফার, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      যদি(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, ফাঁকা);
      বিরতি;
    ক্ষেত্র MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, ফাঁকা);
      বিরতি;
    ক্ষেত্র MIMESTATE_CONTENT:
      যদি(part->encoder)
        sz = read_encoded_part_content(part, বাফার, bufsize, hasread);
      অন্যথা
        sz = read_part_content(part, বাফার, bufsize, hasread);
      নির্বাচন(sz) {
      ক্ষেত্র 0:
        mimesetstate(&part->state, MIMESTATE_END, ফাঁকা);
        /* Try sparing open file descriptors. */
        যদি(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = ফাঁকা;
        }
        FALLTHROUGH();
      ক্ষেত্র CURL_READFUNC_ABORT:
      ক্ষেত্র CURL_READFUNC_PAUSE:
      ক্ষেত্র READ_ERROR:
      ক্ষেত্র STOP_FILLING:
        ফেরত cursize ? cursize : sz;
      }
      বিরতি;
    ক্ষেত্র MIMESTATE_END:
      ফেরত cursize;
    পূর্বনির্ধারিত:
      বিরতি;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    বাফার += sz;
    bufsize -= sz;
  }

  ফেরত cursize;
}

/* Readback from mime. Warning: not a read callback function. */
স্থির size_t mime_subparts_read(অক্ষর *বাফার, size_t আকার_2, size_t nitems,
                                 শূন্য *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (শূন্য)আকার_2;  /* Always 1 */

  যখন(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    নির্বাচন(mime->state.state) {
    ক্ষেত্র MIMESTATE_BEGIN:
    ক্ষেত্র MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      বিরতি;
    ক্ষেত্র MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, বাফার, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      যদি(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      বিরতি;
    ক্ষেত্র MIMESTATE_BOUNDARY2:
      যদি(part)
        sz = readback_bytes(&mime->state, বাফার, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      অন্যথা
        sz = readback_bytes(&mime->state, বাফার, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      যদি(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      বিরতি;
    ক্ষেত্র MIMESTATE_CONTENT:
      যদি(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, ফাঁকা);
        বিরতি;
      }
      sz = readback_part(part, বাফার, nitems, hasread);
      নির্বাচন(sz) {
      ক্ষেত্র CURL_READFUNC_ABORT:
      ক্ষেত্র CURL_READFUNC_PAUSE:
      ক্ষেত্র READ_ERROR:
      ক্ষেত্র STOP_FILLING:
        ফেরত cursize ? cursize : sz;
      ক্ষেত্র 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        বিরতি;
      }
      বিরতি;
    ক্ষেত্র MIMESTATE_END:
      ফেরত cursize;
    পূর্বনির্ধারিত:
      বিরতি;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    বাফার += sz;
    nitems -= sz;
  }

  ফেরত cursize;
}

স্থির পূর্ণসংখ্যা mime_part_rewind(curl_mimepart *part)
{
  পূর্ণসংখ্যা res = CURL_SEEKFUNC_OK;
  গণনা mimestate targetstate = MIMESTATE_BEGIN;

  যদি(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  যদি(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    যদি(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      নির্বাচন(res) {
      ক্ষেত্র CURL_SEEKFUNC_OK:
      ক্ষেত্র CURL_SEEKFUNC_FAIL:
      ক্ষেত্র CURL_SEEKFUNC_CANTSEEK:
        বিরতি;
      ক্ষেত্র -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        বিরতি;
      পূর্বনির্ধারিত:
        res = CURL_SEEKFUNC_FAIL;
        বিরতি;
      }
    }
  }

  যদি(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, ফাঁকা);

  part->lastreadstatus = 1; /* Successful read status. */
  ফেরত res;
}

স্থির পূর্ণসংখ্যা mime_subparts_seek(শূন্য *instream, curl_off_t offset, পূর্ণসংখ্যা whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  পূর্ণসংখ্যা rc = CURL_SEEKFUNC_OK;

  যদি(whence != SEEK_SET || offset)
    ফেরত CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  যদি(mime->state.state == MIMESTATE_BEGIN)
    ফেরত CURL_SEEKFUNC_OK;           /* Already rewound. */

  জন্য(part = mime->firstpart; part; part = part->nextpart) {
    পূর্ণসংখ্যা res = mime_part_rewind(part);
    যদি(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  যদি(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ফাঁকা);

  ফেরত rc;
}

/* Release part content. */
স্থির শূন্য cleanup_part_content(curl_mimepart *part)
{
  যদি(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = ফাঁকা;
  part->seekfunc = ফাঁকা;
  part->freefunc = ফাঁকা;
  part->arg = (শূন্য *)part;           /* Defaults to part itself. */
  part->তথ্য = ফাঁকা;
  part->fp = ফাঁকা;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned পূর্ণসংখ্যা)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

স্থির শূন্য mime_subparts_free(শূন্য *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  যদি(mime && mime->parent) {
    mime->parent->freefunc = ফাঁকা;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
স্থির শূন্য mime_subparts_unbind(শূন্য *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  যদি(mime && mime->parent) {
    mime->parent->freefunc = ফাঁকা;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = ফাঁকা;
  }
}

শূন্য Curl_mime_cleanpart(curl_mimepart *part)
{
  যদি(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    যদি(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
শূন্য curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  যদি(!mime)
    ফেরত;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  যখন(mime) {
    part = mime->firstpart;
    যদি(part) {
      mime->firstpart = part->nextpart;
      যদি(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = ফাঁকা;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    অন্যথা {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

স্থির bool mime_is_too_deep(ধ্রুবক curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  যদি(++call_depth > MAX_MIME_LEVELS)
    ফেরত TRUE;

  যদি(part->kind == MIMEKIND_MULTIPART && part->arg) {
    ধ্রুবক curl_mime *mime = (ধ্রুবক curl_mime *)part->arg;
    ধ্রুবক curl_mimepart *s;

    জন্য(s = mime->firstpart; s; s = s->nextpart) {
      যদি(mime_is_too_deep(s, call_depth))
        ফেরত TRUE;
    }
  }

  ফেরত FALSE;
}

স্থির CURLcode mime_duppart(কাঠামো Curl_easy *তথ্য,
                             curl_mimepart *dst, ধ্রুবক curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  ধ্রুবক curl_mimepart *s;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  যদি(++call_depth > MAX_MIME_LEVELS)
    ফেরত CURLE_TOO_LARGE;

  /* Duplicate content. */
  নির্বাচন(src->kind) {
  ক্ষেত্র MIMEKIND_NONE:
    বিরতি;
  ক্ষেত্র MIMEKIND_তথ্য:
    ফলাফল = curl_mime_তথ্য(dst, src->তথ্য, (size_t)src->datasize);
    বিরতি;
  ক্ষেত্র MIMEKIND_FILE:
    ফলাফল = curl_mime_filedata(dst, src->তথ্য);
    /* Do not abort duplication if file is not readable. */
    যদি(ফলাফল == CURLE_READ_ERROR)
      ফলাফল = CURLE_OK;
    বিরতি;
  ক্ষেত্র MIMEKIND_CALLBACK:
    ফলাফল = curl_mime_তথ্য_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    বিরতি;
  ক্ষেত্র MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(তথ্য);
    ফলাফল = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    জন্য(s = ((curl_mime *)src->arg)->firstpart; !ফলাফল && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      ফলাফল = d ? mime_duppart(তথ্য, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    বিরতি;
  পূর্বনির্ধারিত:  /* Invalid kind: should not occur. */
    DEBUGF(infof(তথ্য, "invalid MIMEKIND* attempt"));
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    বিরতি;
  }

  /* Duplicate headers. */
  যদি(!ফলাফল && src->userheaders) {
    কাঠামো curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    যদি(!hdrs)
      ফলাফল = CURLE_OUT_OF_MEMORY;
    অন্যথা {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      ফলাফল = curl_mime_headers(dst, hdrs, TRUE);
      যদি(ফলাফল)
        curl_slist_free_all(hdrs);
    }
  }

  যদি(!ফলাফল) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    ফলাফল = curl_mime_type(dst, src->mimetype);
  }
  যদি(!ফলাফল)
    ফলাফল = curl_mime_name(dst, src->name);
  যদি(!ফলাফল)
    ফলাফল = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  যদি(ফলাফল)
    Curl_mime_cleanpart(dst);

  ফেরত ফলাফল;
}

CURLcode Curl_mime_duppart(কাঠামো Curl_easy *তথ্য,
                           curl_mimepart *dst, ধ্রুবক curl_mimepart *src)
{
  যদি(!dst || !src)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(mime_is_too_deep(src, 0))
    ফেরত CURLE_TOO_LARGE;

  ফেরত mime_duppart(তথ্য, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(শূন্য *easy)
{
  curl_mime *mime = curlx_malloc(আকার(*mime));

  যদি(mime) {
    mime->parent = ফাঁকা;
    mime->firstpart = ফাঁকা;
    mime->lastpart = ফাঁকা;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    যদি(Curl_rand_alnum(easy,
                       (unsigned অক্ষর *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      ফেরত ফাঁকা;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ফাঁকা);
  }

  ফেরত mime;
}

/* Initialize a mime part. */
শূন্য Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, আকার(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, ফাঁকা);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  যদি(!mime)
    ফেরত ফাঁকা;

  part = curlx_malloc(আকার(*part));

  যদি(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    যদি(mime->lastpart)
      mime->lastpart->nextpart = part;
    অন্যথা
      mime->firstpart = part;

    mime->lastpart = part;
  }

  ফেরত part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ধ্রুবক অক্ষর *name)
{
  যদি(!part)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  যদি(name) {
    part->name = curlx_strdup(name);
    যদি(!part->name)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  ফেরত CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ধ্রুবক অক্ষর *filename)
{
  যদি(!part)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  যদি(filename) {
    part->filename = curlx_strdup(filename);
    যদি(!part->filename)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  ফেরত CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_তথ্য(curl_mimepart *part, ধ্রুবক অক্ষর *তথ্য, size_t datasize)
{
  যদি(!part)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  যদি(তথ্য) {
    যদি(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(তথ্য);

    part->তথ্য = curlx_memdup0(তথ্য, datasize);
    যদি(!part->তথ্য)
      ফেরত CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_তথ্য;
  }

  ফেরত CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ধ্রুবক অক্ষর *filename)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(!part)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  যদি(filename) {
    অক্ষর *base;
    curlx_struct_stat sbuf;

    যদি(curlx_stat(filename, &sbuf))
      ফলাফল = CURLE_READ_ERROR;
    অন্যথা {
      part->তথ্য = curlx_strdup(filename);
      যদি(!part->তথ্য)
        ফলাফল = CURLE_OUT_OF_MEMORY;
      অন্যথা {
        part->datasize = -1;
        যদি(S_ISREG(sbuf.st_mode)) {
          part->datasize = filesize(filename, sbuf);
          part->seekfunc = mime_file_seek;
        }

        part->readfunc = mime_file_read;
        part->freefunc = mime_file_free;
        part->kind = MIMEKIND_FILE;

        /* As a side effect, set the filename to the current file's base name.
           It is possible to withdraw this by explicitly calling
           curl_mime_filename() with a NULL filename argument after the current
           call. */
        base = strippath(filename);
        যদি(!base)
          ফলাফল = CURLE_OUT_OF_MEMORY;
        অন্যথা {
          ফলাফল = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  ফেরত ফলাফল;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ধ্রুবক অক্ষর *mimetype)
{
  যদি(!part)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  যদি(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    যদি(!part->mimetype)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  ফেরত CURLE_OK;
}

স্থির ধ্রুবক কাঠামো mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_আকার },
  { "8bit", encoder_nop_read, encoder_nop_আকার },
  { "7bit", encoder_7bit_read, encoder_nop_আকার },
  { "base64", encoder_base64_read, encoder_base64_আকার },
  { "quoted-printable", encoder_qp_read, encoder_qp_আকার },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ধ্রুবক অক্ষর *encoding)
{
  CURLcode ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
  ধ্রুবক কাঠামো mime_encoder *mep;

  যদি(!part)
    ফেরত ফলাফল;

  part->encoder = ফাঁকা;

  যদি(!encoding)
    ফেরত CURLE_OK;    /* Removing current encoder. */

  জন্য(mep = encoders; mep->name; mep++)
    যদি(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      ফলাফল = CURLE_OK;
    }

  ফেরত ফলাফল;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           কাঠামো curl_slist *headers, পূর্ণসংখ্যা take_ownership)
{
  যদি(!part)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(part->flags & MIME_USERHEADERS_OWNER) {
    যদি(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned পূর্ণসংখ্যা)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  যদি(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  ফেরত CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_তথ্য_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, শূন্য *arg)
{
  যদি(!part)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  যদি(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  ফেরত CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, পূর্ণসংখ্যা take_ownership)
{
  curl_mime *মূল;

  যদি(!part)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  যদি(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    ফেরত CURLE_OK;

  cleanup_part_content(part);

  যদি(subparts) {
    /* Should not have been attached already. */
    যদি(subparts->parent)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    মূল = part->parent;
    যদি(মূল) {
      যখন(মূল->parent && মূল->parent->parent)
        মূল = মূল->parent->parent;
      যদি(subparts == মূল) {
        /* cannot add as a subpart of itself. */
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    যদি(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      ফেরত CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  ফেরত CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  ফেরত Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(অক্ষর *বাফার, size_t আকার_2, size_t nitems, শূন্য *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (শূন্য)আকার_2;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  করো {
    hasread = FALSE;
    ret = readback_part(part, বাফার, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } যখন(ret == STOP_FILLING);

  ফেরত ret;
}

/* Rewind mime stream. */
স্থির CURLcode mime_rewind(curl_mimepart *part)
{
  ফেরত mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
স্থির size_t slist_আকার(কাঠামো curl_slist *s,
                         size_t overhead, ধ্রুবক অক্ষর *skip, size_t skiplen)
{
  size_t আকার_2 = 0;

  জন্য(; s; s = s->পরবর্তী)
    যদি(!skip || !match_header(s, skip, skiplen))
      আকার_2 += strlen(s->তথ্য) + overhead;
  ফেরত আকার_2;
}

/* Get/compute multipart size. */
স্থির curl_off_t multipart_আকার(curl_mime *mime)
{
  curl_off_t আকার_2;
  curl_off_t boundarysize;
  curl_mimepart *part;

  যদি(!mime)
    ফেরত 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  আকার_2 = boundarysize;  /* Final boundary - CRLF after headers. */

  জন্য(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_আকার(part);

    যদি(sz < 0)
      আকার_2 = sz;

    যদি(আকার_2 >= 0)
      আকার_2 += boundarysize + sz;
  }

  ফেরত আকার_2;
}

/* Get/compute mime size. */
স্থির curl_off_t mime_আকার(curl_mimepart *part)
{
  curl_off_t আকার_2;

  যদি(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_আকার(part->arg);

  আকার_2 = part->datasize;

  যদি(part->encoder)
    আকার_2 = part->encoder->sizefunc(part);

  যদি(আকার_2 >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    আকার_2 += slist_আকার(part->curlheaders, 2, ফাঁকা, 0);
    আকার_2 += slist_আকার(part->userheaders, 2, STRCONST("Content-Type"));
    আকার_2 += 2;    /* CRLF after headers. */
  }
  ফেরত আকার_2;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(কাঠামো curl_slist **slp, ধ্রুবক অক্ষর *fmt, ...)
{
  কাঠামো curl_slist *hdr = ফাঁকা;
  অক্ষর *s = ফাঁকা;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  যদি(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    যদি(hdr)
      *slp = hdr;
    অন্যথা
      curlx_free(s);
  }

  ফেরত hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
স্থির CURLcode add_content_type(কাঠামো curl_slist **slp,
                                 ধ্রুবক অক্ষর *type, ধ্রুবক অক্ষর *boundary)
{
  ফেরত Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ধ্রুবক অক্ষর *Curl_mime_contenttype(ধ্রুবক অক্ষর *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  কাঠামো ContentType {
    ধ্রুবক অক্ষর *extension;
    ধ্রুবক অক্ষর *type;
  };
  স্থির ধ্রুবক কাঠামো ContentType ctts[] = {
    { ".gif",  "image/gif" },
    { ".jpg",  "image/jpeg" },
    { ".jpeg", "image/jpeg" },
    { ".png",  "image/png" },
    { ".svg",  "image/svg+xml" },
    { ".txt",  "text/plain" },
    { ".htm",  "text/html" },
    { ".html", "text/html" },
    { ".pdf",  "application/pdf" },
    { ".xml",  "application/xml" }
  };

  যদি(filename) {
    size_t len1 = strlen(filename);
    ধ্রুবক অক্ষর *nameend = filename + len1;
    unsigned পূর্ণসংখ্যা i;

    জন্য(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      যদি(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        ফেরত ctts[i].type;
    }
  }
  ফেরত ফাঁকা;
}

স্থির bool content_type_match(ধ্রুবক অক্ষর *contenttype,
                               ধ্রুবক অক্ষর *target, size_t len)
{
  যদি(contenttype && curl_strnequal(contenttype, target, len))
    নির্বাচন(contenttype[len]) {
    ক্ষেত্র '\0':
    ক্ষেত্র '\t':
    ক্ষেত্র '\r':
    ক্ষেত্র '\n':
    ক্ষেত্র ' ':
    ক্ষেত্র ';':
      ফেরত TRUE;
    }
  ফেরত FALSE;
}

স্থির CURLcode add_content_disposition(কাঠামো Curl_easy *তথ্য,
                                        curl_mimepart *part,
                                        ধ্রুবক অক্ষর *disposition,
                                        ধ্রুবক অক্ষর *contenttype,
                                        গণনা mimestrategy strategy)
{
  যদি(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  যদি(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = ফাঁকা;

  যদি(disposition) {
    CURLcode ফলাফল = CURLE_OK;
    অক্ষর *name = ফাঁকা;
    অক্ষর *filename = ফাঁকা;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (তথ্য && তথ্য->set.mime_formescape);
    যদি(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

    যদি(part->name) {
      name = escape_string(তথ্য, part->name, strategy);
      যদি(!name)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
    যদি(part->filename) {
      filename = escape_string(তথ্য, part->filename, strategy);
      যদি(!filename)
        ফলাফল = CURLE_OUT_OF_MEMORY;
    }
    যদি(!ফলাফল)
      ফলাফল = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Disposition: %s%s%s%s%s%s%s",
                                    disposition,
                                    name ? "; name=\"" : "",
                                    name ? name : "",
                                    name ? "\"" : "",
                                    filename ? "; filename=\"" : "",
                                    filename ? filename : "",
                                    filename ? "\"" : "");
    curlx_safefree(name);
    curlx_safefree(filename);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode mime_prepare_headers(কাঠামো Curl_easy *তথ্য,
                                     curl_mimepart *part,
                                     ধ্রুবক অক্ষর *contenttype,
                                     ধ্রুবক অক্ষর *disposition,
                                     গণনা mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = ফাঁকা;
  ধ্রুবক অক্ষর *boundary = ফাঁকা;
  অক্ষর *customct;
  ধ্রুবক অক্ষর *cte = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  যদি(++call_depth > MAX_MIME_LEVELS)
    ফেরত CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = ফাঁকা;

  /* Be sure we will not access old headers later. */
  যদি(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, ফাঁকা);

  /* Check if content type is specified. */
  customct = part->mimetype;
  যদি(!customct)
    customct = অনুসন্ধান_header(part->userheaders, STRCONST("Content-Type"));
  যদি(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  যদি(!contenttype) {
    নির্বাচন(part->kind) {
    ক্ষেত্র MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      বিরতি;
    ক্ষেত্র MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      যদি(!contenttype)
        contenttype = Curl_mime_contenttype(part->তথ্য);
      যদি(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      বিরতি;
    পূর্বনির্ধারিত:
      contenttype = Curl_mime_contenttype(part->filename);
      বিরতি;
    }
  }

  যদি(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    যদি(mime)
      boundary = mime->boundary;
  }
  অন্যথা যদি(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = ফাঁকা;

  /* Issue content-disposition header only if not already set by caller. */
  যদি(!অনুসন্ধান_header(part->userheaders, STRCONST("Content-Disposition"))) {
    ফলাফল = add_content_disposition(তথ্য, part, disposition,
                                     contenttype, strategy);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* Issue Content-Type header. */
  যদি(contenttype) {
    ফলাফল = add_content_type(&part->curlheaders, contenttype, boundary);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* Content-Transfer-Encoding header. */
  যদি(!অনুসন্ধান_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    যদি(part->encoder)
      cte = part->encoder->name;
    অন্যথা যদি(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    যদি(cte) {
      ফলাফল = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  যদি(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  যদি(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = ফাঁকা;
    যদি(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    জন্য(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      ফলাফল = mime_prepare_headers(তথ্য, subpart, ফাঁকা,
                                    disposition, strategy, call_depth);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
  }
  ফেরত ফলাফল;
}

CURLcode Curl_mime_prepare_headers(কাঠামো Curl_easy *তথ্য,
                                   curl_mimepart *part,
                                   ধ্রুবক অক্ষর *contenttype,
                                   ধ্রুবক অক্ষর *disposition,
                                   গণনা mimestrategy strategy)
{
  ফেরত mime_prepare_headers(তথ্য, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
স্থির শূন্য mime_unpause(curl_mimepart *part)
{
  যদি(part) {
    যদি(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    যদি(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      যদি(mime) {
        curl_mimepart *subpart;

        জন্য(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

কাঠামো cr_mime_ctx {
  কাঠামো Curl_creader super;
  curl_mimepart *part;
  curl_off_t যোগ_len;
  curl_off_t read_len;
  CURLcode error_ফলাফল;
  কাঠামো bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

স্থির CURLcode cr_mime_init(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_creader *reader)
{
  কাঠামো cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ctx->যোগ_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  ফেরত CURLE_OK;
}

স্থির শূন্য cr_mime_close(কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_creader *reader)
{
  কাঠামো cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
স্থির CURLcode cr_mime_read(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_creader *reader,
                             অক্ষর *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  কাঠামো cr_mime_ctx *ctx = reader->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t nread;
  অক্ষর tmp[256];

  /* Once we have errored, we will return the same error forever */
  যদি(ctx->errored) {
    CURL_TRC_READ(তথ্য, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (পূর্ণসংখ্যা)ctx->error_ফলাফল);
    *pnread = 0;
    *peos = FALSE;
    ফেরত ctx->error_ফলাফল;
  }
  যদি(ctx->seen_eos) {
    CURL_TRC_READ(তথ্য, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    ফেরত CURLE_OK;
  }
  /* respect length limitations */
  যদি(ctx->যোগ_len >= 0) {
    curl_off_t remain = ctx->যোগ_len - ctx->read_len;
    যদি(remain <= 0)
      blen = 0;
    অন্যথা যদি(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  যদি(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    ফলাফল = Curl_bufq_read(&ctx->tmpbuf, (unsigned অক্ষর *)buf, blen, &nread);
    যদি(ফলাফল) {
      ctx->errored = TRUE;
      ctx->error_ফলাফল = ফলাফল;
      ফেরত ফলাফল;
    }
  }
  অন্যথা যদি(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(তথ্য, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, আকার(tmp), ctx->part);
    যদি(nread <= আকার(tmp)) {
      size_t n;
      ফলাফল = Curl_bufq_write(&ctx->tmpbuf, (unsigned অক্ষর *)tmp, nread, &n);
      যদি(ফলাফল) {
        ctx->errored = TRUE;
        ctx->error_ফলাফল = ফলাফল;
        ফেরত ফলাফল;
      }
      /* stored it, read again */
      ফলাফল = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      যদি(ফলাফল) {
        ctx->errored = TRUE;
        ctx->error_ফলাফল = ফলাফল;
        ফেরত ফলাফল;
      }
    }
  }
  অন্যথা
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(তথ্য, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  নির্বাচন(nread) {
  ক্ষেত্র 0:
    যদি((ctx->যোগ_len >= 0) && (ctx->read_len < ctx->যোগ_len)) {
      failf(তথ্য, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->যোগ_len);
      ফেরত CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    বিরতি;

  ক্ষেত্র CURL_READFUNC_ABORT:
    failf(তথ্য, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ফলাফল = CURLE_ABORTED_BY_CALLBACK;
    ফলাফল = CURLE_ABORTED_BY_CALLBACK;
    বিরতি;

  ক্ষেত্র CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(তথ্য, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    ফলাফল = Curl_xfer_pause_send(তথ্য, TRUE);
    বিরতি; /* nothing was read */

  ক্ষেত্র STOP_FILLING:
  ক্ষেত্র READ_ERROR:
    failf(তথ্য, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ফলাফল = CURLE_READ_ERROR;
    ফলাফল = CURLE_READ_ERROR;
    বিরতি;

  পূর্বনির্ধারিত:
    যদি(nread > blen) {
      /* the read function returned a too large value */
      failf(তথ্য, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_ফলাফল = CURLE_READ_ERROR;
      ফেরত CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    যদি(ctx->যোগ_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->যোগ_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    বিরতি;
  }

  CURL_TRC_READ(তথ্য, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->যোগ_len, ctx->read_len, (পূর্ণসংখ্যা)ফলাফল, *pnread, *peos);
  ফেরত ফলাফল;
}

স্থির bool cr_mime_needs_rewind(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো Curl_creader *reader)
{
  কাঠামো cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ফেরত ctx->read_len > 0;
}

স্থির curl_off_t cr_mime_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_creader *reader)
{
  কাঠামো cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ফেরত ctx->যোগ_len;
}

স্থির CURLcode cr_mime_resume_from(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো Curl_creader *reader,
                                    curl_off_t offset)
{
  কাঠামো cr_mime_ctx *ctx = reader->ctx;

  যদি(offset > 0) {
    curl_off_t passed = 0;

    করো {
      অক্ষর scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)আকার(scratch)) ?
        আকার(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      যদি((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(তথ্য, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        ফেরত CURLE_READ_ERROR;
      }
    } যখন(passed < offset);

    /* now, decrease the size of the read */
    যদি(ctx->যোগ_len > 0) {
      ctx->যোগ_len -= offset;

      যদি(ctx->যোগ_len <= 0) {
        failf(তথ্য, "Mime post already completely uploaded");
        ফেরত CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cr_mime_cntrl(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  কাঠামো cr_mime_ctx *ctx = reader->ctx;
  নির্বাচন(opcode) {
  ক্ষেত্র CURL_CRCNTRL_REWIND: {
    CURLcode ফলাফল = mime_rewind(ctx->part);
    যদি(ফলাফল)
      failf(তথ্য, "Cannot rewind mime/post data");
    ফেরত ফলাফল;
  }
  ক্ষেত্র CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    বিরতি;
  ক্ষেত্র CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির bool cr_mime_is_paused(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_creader *reader)
{
  কাঠামো cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  ফেরত ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

স্থির ধ্রুবক কাঠামো Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_যোগ_দৈর্ঘ্য,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_done,
  আকার(কাঠামো cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(কাঠামো Curl_easy *তথ্য, curl_mimepart *part)
{
  কাঠামো Curl_creader *r;
  কাঠামো cr_mime_ctx *ctx;
  CURLcode ফলাফল;

  ফলাফল = Curl_creader_create(&r, তথ্য, &cr_mime, CURL_CR_CLIENT);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  ফলাফল = mime_rewind(ctx->part);
  যদি(ফলাফল) {
    Curl_creader_free(তথ্য, r);
    ফেরত ফলাফল;
  }
  ctx->যোগ_len = mime_আকার(ctx->part);

  ফেরত Curl_creader_set(তথ্য, r);
}

#অন্যথা /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (শূন্য)easy;
  ফেরত ফাঁকা;
}

শূন্য curl_mime_free(curl_mime *mime)
{
  (শূন্য)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (শূন্য)mime;
  ফেরত ফাঁকা;
}

CURLcode curl_mime_name(curl_mimepart *part, ধ্রুবক অক্ষর *name)
{
  (শূন্য)part;
  (শূন্য)name;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, ধ্রুবক অক্ষর *filename)
{
  (শূন্য)part;
  (শূন্য)filename;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, ধ্রুবক অক্ষর *mimetype)
{
  (শূন্য)part;
  (শূন্য)mimetype;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ধ্রুবক অক্ষর *encoding)
{
  (শূন্য)part;
  (শূন্য)encoding;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_তথ্য(curl_mimepart *part, ধ্রুবক অক্ষর *তথ্য, size_t datasize)
{
  (শূন্য)part;
  (শূন্য)তথ্য;
  (শূন্য)datasize;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ধ্রুবক অক্ষর *filename)
{
  (শূন্য)part;
  (শূন্য)filename;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_তথ্য_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           শূন্য *arg)
{
  (শূন্য)part;
  (শূন্য)datasize;
  (শূন্য)readfunc;
  (শূন্য)seekfunc;
  (শূন্য)freefunc;
  (শূন্য)arg;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (শূন্য)part;
  (শূন্য)subparts;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           কাঠামো curl_slist *headers, পূর্ণসংখ্যা take_ownership)
{
  (শূন্য)part;
  (শূন্য)headers;
  (শূন্য)take_ownership;
  ফেরত CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(কাঠামো curl_slist **slp, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)slp;
  (শূন্য)fmt;
  ফেরত CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

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
#অন্তর্ভাৱ "curl_setup.h"

সংরচনা Curl_easy;

#অন্তর্ভাৱ "mime.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "curlx/basename.h"
#অন্তর্ভাৱ "curlx/strcopy.h"
#অন্তর্ভাৱ "curlx/win32-fopen.h"
#অন্তর্ভাৱ "curlx/base64.h"

#যদি !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#অন্তর্ভাৱ "rand.h"
#অন্তর্ভাৱ "slist.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

স্থির size_t mime_subparts_read(ৱর্ণ *buffer, size_t size, size_t nitems,
                                 শূন্য *instream, bool *hasread);
স্থির curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
স্থির ধ্রুৱ unsigned ৱর্ণ qp_class[] = {
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
স্থির ধ্রুৱ ৱর্ণ aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#অন্যথা

#অন্তর্ভাৱ <fabdef.h>
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
curl_off_t VmsRealFileSize(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ curlx_struct_stat *stat_buf)
{
  ৱর্ণ buffer[8192];
  curl_off_t গুনতি;
  পূর্ণাঙ্ক ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  যদি(!file)
    ঘূৰাই 0;

  গুনতি = 0;
  ret_stat = 1;
  যেতিয়ালৈ(ret_stat > 0) {
    ret_stat = fread(buffer, 1, পরিমাণ(buffer), file);
    যদি(ret_stat)
      গুনতি += ret_stat;
  }
  curlx_fclose(file);

  ঘূৰাই গুনতি;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
স্থির curl_off_t VmsSpecialSize(ধ্রুৱ ৱর্ণ *name,
                                 ধ্রুৱ curlx_struct_stat *stat_buf)
{
  ৱিকল্প(stat_buf->st_fab_rfm) {
  পক্ষ FAB$C_VAR:
  পক্ষ FAB$C_VFC:
    ঘূৰাই VmsRealFileSize(name, stat_buf);
    ভঙ্গ;
  স্ৱভাৱ:
    ঘূৰাই stat_buf->st_size;
  }
}

#define filesize(name, stat_data) VmsSpecialSize(name, &stat_data)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
স্থির FILE *vmsfopenread(ধ্রুৱ ৱর্ণ *file, ধ্রুৱ ৱর্ণ *mode)
{
  curlx_struct_stat statbuf;
  পূর্ণাঙ্ক res = curlx_stat(file, &statbuf);

  যদি(res != -1) {
    ৱিকল্প(statbuf.st_fab_rfm) {
    পক্ষ FAB$C_VAR:
    পক্ষ FAB$C_VFC:
    পক্ষ FAB$C_STMCR:
      ঘূৰাই curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      ভঙ্গ;
    স্ৱভাৱ:
      ঘূৰাই curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  ঘূৰাই রিক্ত;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
স্থির শূন্য mimesetstate(সংরচনা mime_state *state,
                         গণনা mimestate tok, শূন্য *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
স্থির ৱর্ণ *escape_string(সংরচনা Curl_easy *data,
                           ধ্রুৱ ৱর্ণ *src, গণনা mimestrategy strategy)
{
  CURLcode result;
  সংরচনা dynbuf db;
  ধ্রুৱ ৱর্ণ * ধ্রুৱ *table;
  ধ্রুৱ ৱর্ণ * ধ্রুৱ *p;
  /* replace first character by rest of string. */
  স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    রিক্ত
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    রিক্ত
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  যদি(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_প্ৰবেশ_LENGTH);

  বাবে(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    বাবে(p = table; *p && **p != *src; p++)
      ;

    যদি(*p)
      result = curlx_dyn_add(&db, *p + 1);
    অন্যথা
      result = curlx_dyn_addn(&db, src, 1);
  }

  ঘূৰাই curlx_dyn_ptr(&db);
}

/* Check if header matches. */
স্থির ৱর্ণ *match_header(সংরচনা curl_slist *hdr, ধ্রুৱ ৱর্ণ *lbl, size_t len)
{
  ৱর্ণ *মান = রিক্ত;

  যদি(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    বাবে(মান = hdr->data + len + 1; *মান == ' '; মান++)
      ;
  ঘূৰাই মান;
}

/* Get a header from an slist. */
স্থির ৱর্ণ *অনুসন্ধান_header(সংরচনা curl_slist *hdrlist,
                           ধ্রুৱ ৱর্ণ *hdr, size_t len)
{
  ৱর্ণ *মান = রিক্ত;

  বাবে(; !মান && hdrlist; hdrlist = hdrlist->next)
    মান = match_header(hdrlist, hdr, len);

  ঘূৰাই মান;
}

স্থির ৱর্ণ *strippath(ধ্রুৱ ৱর্ণ *fullfile)
{
  ৱর্ণ *filename;
  ৱর্ণ *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  যদি(!filename)
    ঘূৰাই রিক্ত;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  ঘূৰাই base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
স্থির শূন্য cleanup_encoder_state(সংরচনা mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
স্থির size_t encoder_nop_read(ৱর্ণ *buffer, size_t size, bool ateof,
                               সংরচনা curl_mimepart *part)
{
  সংরচনা mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (শূন্য)ateof;

  যদি(!size)
    ঘূৰাই STOP_FILLING;

  যদি(size > insize)
    size = insize;

  যদি(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  ঘূৰাই size;
}

স্থির curl_off_t encoder_nop_size(curl_mimepart *part)
{
  ঘূৰাই part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
স্থির size_t encoder_7bit_read(ৱর্ণ *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  সংরচনা mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (শূন্য)ateof;

  যদি(!size)
    ঘূৰাই STOP_FILLING;

  যদি(size > cursize)
    size = cursize;

  বাবে(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    যদি(*buffer++ & 0x80)
      ঘূৰাই cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  ঘূৰাই cursize;
}

/* Base64 content encoder. */
স্থির size_t encoder_base64_read(ৱর্ণ *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  সংরচনা mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  পূর্ণাঙ্ক i;
  ৱর্ণ *ptr = buffer;

  যেতিয়ালৈ(st->bufbeg < st->bufend) {
    /* Line full ? */
    যদি(st->pos > MAX_ENCODED_সাৰি_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      যদি(size < 2) {
        যদি(!cursize)
          ঘূৰাই STOP_FILLING;
        ভঙ্গ;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    যদি(size < 4) {
      যদি(!cursize)
        ঘূৰাই STOP_FILLING;
      ভঙ্গ;
    }
    যদি(st->bufend - st->bufbeg < 3)
      ভঙ্গ;

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
    size -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  যদি(ateof) {
    যদি(size < 4) {
      যদি(!cursize)
        ঘূৰাই STOP_FILLING;
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

  ঘূৰাই cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_প্ৰবেশ_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_সাৰি_LENGTH + 2)) *                  \
    MAX_ENCODED_সাৰি_LENGTH / 4) * 3 - 3)

স্থির curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  যদি(size <= 0)
    ঘূৰাই size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  যদি(size > BASE64_MAX_প্ৰবেশ_SIZE)
    ঘূৰাই -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  ঘূৰাই size + (2 * ((size - 1) / MAX_ENCODED_সাৰি_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
স্থির পূর্ণাঙ্ক qp_lookahead_eol(সংরচনা mime_encoder_state *st, পূর্ণাঙ্ক ateof, size_t n)
{
  n += st->bufbeg;
  যদি(n >= st->bufend && ateof)
    ঘূৰাই 1;
  যদি(n + 2 > st->bufend)
    ঘূৰাই ateof ? 0 : -1;
  যদি(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    ঘূৰাই 1;
  ঘূৰাই 0;
}

/* Quoted-printable encoder. */
স্থির size_t encoder_qp_read(ৱর্ণ *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  সংরচনা mime_encoder_state *st = &part->encstate;
  ৱর্ণ *ptr = buffer;
  size_t cursize = 0;
  পূর্ণাঙ্ক softlinebreak;
  ৱর্ণ buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  যেতিয়ালৈ(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    পূর্ণাঙ্ক i = (unsigned ৱর্ণ)st->buf[st->bufbeg];
    buf[0] = (ৱর্ণ)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    ৱিকল্প(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    পক্ষ QP_OK:          /* Not a special character. */
      ভঙ্গ;
    পক্ষ QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      ৱিকল্প(qp_lookahead_eol(st, ateof, 1)) {
      পক্ষ -1:          /* More input data needed. */
        ঘূৰাই cursize;
      পক্ষ 0:           /* No encoding needed. */
        ভঙ্গ;
      স্ৱভাৱ:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ভঙ্গ;
      }
      ভঙ্গ;
    পক্ষ QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      ৱিকল্প(qp_lookahead_eol(st, ateof, 0)) {
      পক্ষ -1:          /* Need more data. */
        ঘূৰাই cursize;
      পক্ষ 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        ভঙ্গ;
      স্ৱভাৱ:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ভঙ্গ;
      }
      ভঙ্গ;
    স্ৱভাৱ:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      ভঙ্গ;
    }

    /* Be sure the encoded character fits within maximum line length. */
    যদি(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_সাৰি_LENGTH;
      যদি(!softlinebreak && st->pos + len == MAX_ENCODED_সাৰি_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        ৱিকল্প(qp_lookahead_eol(st, ateof, consumed)) {
        পক্ষ -1:        /* Need more data. */
          ঘূৰাই cursize;
        পক্ষ 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          ভঙ্গ;
        }
      }
      যদি(softlinebreak) {
        curlx_strcopy(buf, পরিমাণ(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    যদি(len > size) {
      যদি(!cursize)
        ঘূৰাই STOP_FILLING;
      ভঙ্গ;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    যদি(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  ঘূৰাই cursize;
}

স্থির curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  ঘূৰাই part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
স্থির size_t mime_mem_read(ৱর্ণ *buffer, size_t size, size_t nitems,
                            শূন্য *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (শূন্য)size;  /* Always 1 */

  যদি(!nitems)
    ঘূৰাই STOP_FILLING;

  যদি(sz > nitems)
    sz = nitems;

  যদি(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  ঘূৰাই sz;
}

স্থির পূর্ণাঙ্ক mime_mem_seek(শূন্য *instream, curl_off_t offset, পূর্ণাঙ্ক whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ৱিকল্প(whence) {
  পক্ষ SEEK_CUR:
    offset += part->state.offset;
    ভঙ্গ;
  পক্ষ SEEK_END:
    offset += part->datasize;
    ভঙ্গ;
  }

  যদি(offset < 0 || offset > part->datasize)
    ঘূৰাই CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  ঘূৰাই CURL_SEEKFUNC_OK;
}

স্থির শূন্য mime_mem_free(শূন্য *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
স্থির bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  যদি(part->fp)
    ঘূৰাই FALSE;
  part->fp = fopen_read(part->data, "rb");
  ঘূৰাই part->fp ? FALSE : TRUE;
}

স্থির size_t mime_file_read(ৱর্ণ *buffer, size_t size, size_t nitems,
                             শূন্য *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  যদি(!nitems)
    ঘূৰাই STOP_FILLING;

  যদি(mime_open_file(part))
    ঘূৰাই READ_ERROR;

  ঘূৰাই fread(buffer, size, nitems, part->fp);
}

স্থির পূর্ণাঙ্ক mime_file_seek(শূন্য *instream, curl_off_t offset, পূর্ণাঙ্ক whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  যদি(whence == SEEK_SET && !offset && !part->fp)
    ঘূৰাই CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  যদি(mime_open_file(part))
    ঘূৰাই CURL_SEEKFUNC_FAIL;

  ঘূৰাই curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

স্থির শূন্য mime_file_free(শূন্য *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  যদি(part->fp) {
    curlx_fclose(part->fp);
    part->fp = রিক্ত;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
স্থির size_t readback_bytes(সংরচনা mime_state *state,
                             ৱর্ণ *buffer, size_t bufsize,
                             ধ্রুৱ ৱর্ণ *bytes, size_t numbytes,
                             ধ্রুৱ ৱর্ণ *trail, size_t traillen)
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
      ঘূৰাই 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  যদি(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  ঘূৰাই sz;
}

/* Read a non-encoded part content. */

স্থির size_t read_part_content(curl_mimepart *part, ৱর্ণ *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  ৱিকল্প(part->lastreadstatus) {
  পক্ষ 0:
  পক্ষ CURL_READFUNC_ABORT:
  পক্ষ CURL_READFUNC_PAUSE:
  পক্ষ READ_ERROR:
    ঘূৰাই part->lastreadstatus;
  স্ৱভাৱ:
    ভঙ্গ;
  }

  /* If we can determine we are at end of part data, spare a read. */
  যদি(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  অন্যথা {
    ৱিকল্প(part->kind) {
    পক্ষ MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      ভঙ্গ;
    পক্ষ MIMEKIND_FILE:
      যদি(part->fp && feof(part->fp))
        ভঙ্গ;  /* At EOF. */
      FALLTHROUGH();
    স্ৱভাৱ:
      যদি(part->readfunc) {
        যদি(!(part->flags & MIME_FAST_READ)) {
          যদি(*hasread)
            ঘূৰাই STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      ভঙ্গ;
    }
  }

  ৱিকল্প(sz) {
  পক্ষ STOP_FILLING:
    ভঙ্গ;
  পক্ষ 0:
  পক্ষ CURL_READFUNC_ABORT:
  পক্ষ CURL_READFUNC_PAUSE:
  পক্ষ READ_ERROR:
    part->lastreadstatus = sz;
    ভঙ্গ;
  স্ৱভাৱ:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    ভঙ্গ;
  }

  ঘূৰাই sz;
}

/* Read and encode part content. */
স্থির size_t read_encoded_part_content(curl_mimepart *part, ৱর্ণ *buffer,
                                        size_t bufsize, bool *hasread)
{
  সংরচনা mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  বাবে(;;) {
    যদি(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      ৱিকল্প(sz) {
      পক্ষ 0:
        যদি(ateof)
          ঘূৰাই cursize;
        ভঙ্গ;
      পক্ষ READ_ERROR:
      পক্ষ STOP_FILLING:
        ঘূৰাই cursize ? cursize : sz;
      স্ৱভাৱ:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        অনুৱর্ত;
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
    যদি(st->bufend >= পরিমাণ(st->buf))
      ঘূৰাই cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           পরিমাণ(st->buf) - st->bufend, hasread);
    ৱিকল্প(sz) {
    পক্ষ 0:
      ateof = TRUE;
      ভঙ্গ;
    পক্ষ CURL_READFUNC_ABORT:
    পক্ষ CURL_READFUNC_PAUSE:
    পক্ষ READ_ERROR:
    পক্ষ STOP_FILLING:
      ঘূৰাই cursize ? cursize : sz;
    স্ৱভাৱ:
      st->bufend += sz;
      ভঙ্গ;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
স্থির size_t readback_part(curl_mimepart *part,
                            ৱর্ণ *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  যেতিয়ালৈ(bufsize) {
    size_t sz = 0;
    সংরচনা curl_slist *hdr = (সংরচনা curl_slist *)part->state.ptr;
    ৱিকল্প(part->state.state) {
    পক্ষ MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      ভঙ্গ;
    পক্ষ MIMESTATE_USERHEADERS:
      যদি(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, রিক্ত);
        ভঙ্গ;
      }
      যদি(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        ভঙ্গ;
      }
      FALLTHROUGH();
    পক্ষ MIMESTATE_CURLHEADERS:
      যদি(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      অন্যথা {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        যদি(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      ভঙ্গ;
    পক্ষ MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      যদি(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, রিক্ত);
      ভঙ্গ;
    পক্ষ MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, রিক্ত);
      ভঙ্গ;
    পক্ষ MIMESTATE_CONTENT:
      যদি(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      অন্যথা
        sz = read_part_content(part, buffer, bufsize, hasread);
      ৱিকল্প(sz) {
      পক্ষ 0:
        mimesetstate(&part->state, MIMESTATE_END, রিক্ত);
        /* Try sparing open file descriptors. */
        যদি(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = রিক্ত;
        }
        FALLTHROUGH();
      পক্ষ CURL_READFUNC_ABORT:
      পক্ষ CURL_READFUNC_PAUSE:
      পক্ষ READ_ERROR:
      পক্ষ STOP_FILLING:
        ঘূৰাই cursize ? cursize : sz;
      }
      ভঙ্গ;
    পক্ষ MIMESTATE_END:
      ঘূৰাই cursize;
    স্ৱভাৱ:
      ভঙ্গ;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  ঘূৰাই cursize;
}

/* Readback from mime. Warning: not a read callback function. */
স্থির size_t mime_subparts_read(ৱর্ণ *buffer, size_t size, size_t nitems,
                                 শূন্য *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (শূন্য)size;  /* Always 1 */

  যেতিয়ালৈ(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    ৱিকল্প(mime->state.state) {
    পক্ষ MIMESTATE_BEGIN:
    পক্ষ MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      ভঙ্গ;
    পক্ষ MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      যদি(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      ভঙ্গ;
    পক্ষ MIMESTATE_BOUNDARY2:
      যদি(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      অন্যথা
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      যদি(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      ভঙ্গ;
    পক্ষ MIMESTATE_CONTENT:
      যদি(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, রিক্ত);
        ভঙ্গ;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      ৱিকল্প(sz) {
      পক্ষ CURL_READFUNC_ABORT:
      পক্ষ CURL_READFUNC_PAUSE:
      পক্ষ READ_ERROR:
      পক্ষ STOP_FILLING:
        ঘূৰাই cursize ? cursize : sz;
      পক্ষ 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        ভঙ্গ;
      }
      ভঙ্গ;
    পক্ষ MIMESTATE_END:
      ঘূৰাই cursize;
    স্ৱভাৱ:
      ভঙ্গ;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  ঘূৰাই cursize;
}

স্থির পূর্ণাঙ্ক mime_part_rewind(curl_mimepart *part)
{
  পূর্ণাঙ্ক res = CURL_SEEKFUNC_OK;
  গণনা mimestate targetstate = MIMESTATE_BEGIN;

  যদি(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  যদি(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    যদি(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      ৱিকল্প(res) {
      পক্ষ CURL_SEEKFUNC_OK:
      পক্ষ CURL_SEEKFUNC_FAIL:
      পক্ষ CURL_SEEKFUNC_CANTSEEK:
        ভঙ্গ;
      পক্ষ -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        ভঙ্গ;
      স্ৱভাৱ:
        res = CURL_SEEKFUNC_FAIL;
        ভঙ্গ;
      }
    }
  }

  যদি(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, রিক্ত);

  part->lastreadstatus = 1; /* Successful read status. */
  ঘূৰাই res;
}

স্থির পূর্ণাঙ্ক mime_subparts_seek(শূন্য *instream, curl_off_t offset, পূর্ণাঙ্ক whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  পূর্ণাঙ্ক rc = CURL_SEEKFUNC_OK;

  যদি(whence != SEEK_SET || offset)
    ঘূৰাই CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  যদি(mime->state.state == MIMESTATE_BEGIN)
    ঘূৰাই CURL_SEEKFUNC_OK;           /* Already rewound. */

  বাবে(part = mime->firstpart; part; part = part->nextpart) {
    পূর্ণাঙ্ক res = mime_part_rewind(part);
    যদি(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  যদি(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, রিক্ত);

  ঘূৰাই rc;
}

/* Release part content. */
স্থির শূন্য cleanup_part_content(curl_mimepart *part)
{
  যদি(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = রিক্ত;
  part->seekfunc = রিক্ত;
  part->freefunc = রিক্ত;
  part->arg = (শূন্য *)part;           /* Defaults to part itself. */
  part->data = রিক্ত;
  part->fp = রিক্ত;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned পূর্ণাঙ্ক)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

স্থির শূন্য mime_subparts_free(শূন্য *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  যদি(mime && mime->parent) {
    mime->parent->freefunc = রিক্ত;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
স্থির শূন্য mime_subparts_unbind(শূন্য *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  যদি(mime && mime->parent) {
    mime->parent->freefunc = রিক্ত;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = রিক্ত;
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
    ঘূৰাই;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  যেতিয়ালৈ(mime) {
    part = mime->firstpart;
    যদি(part) {
      mime->firstpart = part->nextpart;
      যদি(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = রিক্ত;
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

স্থির bool mime_is_too_deep(ধ্রুৱ curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  যদি(++call_depth > MAX_MIME_LEVELS)
    ঘূৰাই TRUE;

  যদি(part->kind == MIMEKIND_MULTIPART && part->arg) {
    ধ্রুৱ curl_mime *mime = (ধ্রুৱ curl_mime *)part->arg;
    ধ্রুৱ curl_mimepart *s;

    বাবে(s = mime->firstpart; s; s = s->nextpart) {
      যদি(mime_is_too_deep(s, call_depth))
        ঘূৰাই TRUE;
    }
  }

  ঘূৰাই FALSE;
}

স্থির CURLcode mime_duppart(সংরচনা Curl_easy *data,
                             curl_mimepart *dst, ধ্রুৱ curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  ধ্রুৱ curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  যদি(++call_depth > MAX_MIME_LEVELS)
    ঘূৰাই CURLE_TOO_LARGE;

  /* Duplicate content. */
  ৱিকল্প(src->kind) {
  পক্ষ MIMEKIND_NONE:
    ভঙ্গ;
  পক্ষ MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    ভঙ্গ;
  পক্ষ MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    যদি(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    ভঙ্গ;
  পক্ষ MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    ভঙ্গ;
  পক্ষ MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    বাবে(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    ভঙ্গ;
  স্ৱভাৱ:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    ভঙ্গ;
  }

  /* Duplicate headers. */
  যদি(!result && src->userheaders) {
    সংরচনা curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    যদি(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    অন্যথা {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      যদি(result)
        curl_slist_free_all(hdrs);
    }
  }

  যদি(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  যদি(!result)
    result = curl_mime_name(dst, src->name);
  যদি(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  যদি(result)
    Curl_mime_cleanpart(dst);

  ঘূৰাই result;
}

CURLcode Curl_mime_duppart(সংরচনা Curl_easy *data,
                           curl_mimepart *dst, ধ্রুৱ curl_mimepart *src)
{
  যদি(!dst || !src)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(mime_is_too_deep(src, 0))
    ঘূৰাই CURLE_TOO_LARGE;

  ঘূৰাই mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(শূন্য *easy)
{
  curl_mime *mime = curlx_malloc(পরিমাণ(*mime));

  যদি(mime) {
    mime->parent = রিক্ত;
    mime->firstpart = রিক্ত;
    mime->lastpart = রিক্ত;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    যদি(Curl_rand_alnum(easy,
                       (unsigned ৱর্ণ *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      ঘূৰাই রিক্ত;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, রিক্ত);
  }

  ঘূৰাই mime;
}

/* Initialize a mime part. */
শূন্য Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, পরিমাণ(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, রিক্ত);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  যদি(!mime)
    ঘূৰাই রিক্ত;

  part = curlx_malloc(পরিমাণ(*part));

  যদি(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    যদি(mime->lastpart)
      mime->lastpart->nextpart = part;
    অন্যথা
      mime->firstpart = part;

    mime->lastpart = part;
  }

  ঘূৰাই part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ধ্রুৱ ৱর্ণ *name)
{
  যদি(!part)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  যদি(name) {
    part->name = curlx_strdup(name);
    যদি(!part->name)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  ঘূৰাই CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ধ্রুৱ ৱর্ণ *filename)
{
  যদি(!part)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  যদি(filename) {
    part->filename = curlx_strdup(filename);
    যদি(!part->filename)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  ঘূৰাই CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, ধ্রুৱ ৱর্ণ *data, size_t datasize)
{
  যদি(!part)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  যদি(data) {
    যদি(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    যদি(!part->data)
      ঘূৰাই CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  ঘূৰাই CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ধ্রুৱ ৱর্ণ *filename)
{
  CURLcode result = CURLE_OK;

  যদি(!part)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  যদি(filename) {
    ৱর্ণ *base;
    curlx_struct_stat sbuf;

    যদি(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    অন্যথা {
      part->data = curlx_strdup(filename);
      যদি(!part->data)
        result = CURLE_OUT_OF_MEMORY;
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
          result = CURLE_OUT_OF_MEMORY;
        অন্যথা {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  ঘূৰাই result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ধ্রুৱ ৱর্ণ *mimetype)
{
  যদি(!part)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  যদি(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    যদি(!part->mimetype)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  ঘূৰাই CURLE_OK;
}

স্থির ধ্রুৱ সংরচনা mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ধ্রুৱ ৱর্ণ *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  ধ্রুৱ সংরচনা mime_encoder *mep;

  যদি(!part)
    ঘূৰাই result;

  part->encoder = রিক্ত;

  যদি(!encoding)
    ঘূৰাই CURLE_OK;    /* Removing current encoder. */

  বাবে(mep = encoders; mep->name; mep++)
    যদি(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  ঘূৰাই result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           সংরচনা curl_slist *headers, পূর্ণাঙ্ক take_ownership)
{
  যদি(!part)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(part->flags & MIME_USERHEADERS_OWNER) {
    যদি(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned পূর্ণাঙ্ক)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  যদি(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  ঘূৰাই CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, শূন্য *arg)
{
  যদি(!part)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  যদি(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  ঘূৰাই CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, পূর্ণাঙ্ক take_ownership)
{
  curl_mime *মূল;

  যদি(!part)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  যদি(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    ঘূৰাই CURLE_OK;

  cleanup_part_content(part);

  যদি(subparts) {
    /* Should not have been attached already. */
    যদি(subparts->parent)
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    মূল = part->parent;
    যদি(মূল) {
      যেতিয়ালৈ(মূল->parent && মূল->parent->parent)
        মূল = মূল->parent->parent;
      যদি(subparts == মূল) {
        /* cannot add as a subpart of itself. */
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    যদি(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      ঘূৰাই CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  ঘূৰাই CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  ঘূৰাই Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(ৱর্ণ *buffer, size_t size, size_t nitems, শূন্য *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (শূন্য)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  কৰা {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } যেতিয়ালৈ(ret == STOP_FILLING);

  ঘূৰাই ret;
}

/* Rewind mime stream. */
স্থির CURLcode mime_rewind(curl_mimepart *part)
{
  ঘূৰাই mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
স্থির size_t slist_size(সংরচনা curl_slist *s,
                         size_t overhead, ধ্রুৱ ৱর্ণ *skip, size_t skiplen)
{
  size_t size = 0;

  বাবে(; s; s = s->next)
    যদি(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  ঘূৰাই size;
}

/* Get/compute multipart size. */
স্থির curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  যদি(!mime)
    ঘূৰাই 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  বাবে(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    যদি(sz < 0)
      size = sz;

    যদি(size >= 0)
      size += boundarysize + sz;
  }

  ঘূৰাই size;
}

/* Get/compute mime size. */
স্থির curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  যদি(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  যদি(part->encoder)
    size = part->encoder->sizefunc(part);

  যদি(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, রিক্ত, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  ঘূৰাই size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(সংরচনা curl_slist **slp, ধ্রুৱ ৱর্ণ *fmt, ...)
{
  সংরচনা curl_slist *hdr = রিক্ত;
  ৱর্ণ *s = রিক্ত;
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

  ঘূৰাই hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
স্থির CURLcode add_content_type(সংরচনা curl_slist **slp,
                                 ধ্রুৱ ৱর্ণ *type, ধ্রুৱ ৱর্ণ *boundary)
{
  ঘূৰাই Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ধ্রুৱ ৱর্ণ *Curl_mime_contenttype(ধ্রুৱ ৱর্ণ *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  সংরচনা ContentType {
    ধ্রুৱ ৱর্ণ *extension;
    ধ্রুৱ ৱর্ণ *type;
  };
  স্থির ধ্রুৱ সংরচনা ContentType ctts[] = {
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
    ধ্রুৱ ৱর্ণ *nameend = filename + len1;
    unsigned পূর্ণাঙ্ক i;

    বাবে(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      যদি(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        ঘূৰাই ctts[i].type;
    }
  }
  ঘূৰাই রিক্ত;
}

স্থির bool content_type_match(ধ্রুৱ ৱর্ণ *contenttype,
                               ধ্রুৱ ৱর্ণ *target, size_t len)
{
  যদি(contenttype && curl_strnequal(contenttype, target, len))
    ৱিকল্প(contenttype[len]) {
    পক্ষ '\0':
    পক্ষ '\t':
    পক্ষ '\r':
    পক্ষ '\n':
    পক্ষ ' ':
    পক্ষ ';':
      ঘূৰাই TRUE;
    }
  ঘূৰাই FALSE;
}

স্থির CURLcode add_content_disposition(সংরচনা Curl_easy *data,
                                        curl_mimepart *part,
                                        ধ্রুৱ ৱর্ণ *disposition,
                                        ধ্রুৱ ৱর্ণ *contenttype,
                                        গণনা mimestrategy strategy)
{
  যদি(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  যদি(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = রিক্ত;

  যদি(disposition) {
    CURLcode result = CURLE_OK;
    ৱর্ণ *name = রিক্ত;
    ৱর্ণ *filename = রিক্ত;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    যদি(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

    যদি(part->name) {
      name = escape_string(data, part->name, strategy);
      যদি(!name)
        ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
    যদি(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      যদি(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    যদি(!result)
      result = Curl_mime_add_header(&part->curlheaders,
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
    যদি(result)
      ঘূৰাই result;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode mime_prepare_headers(সংরচনা Curl_easy *data,
                                     curl_mimepart *part,
                                     ধ্রুৱ ৱর্ণ *contenttype,
                                     ধ্রুৱ ৱর্ণ *disposition,
                                     গণনা mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = রিক্ত;
  ধ্রুৱ ৱর্ণ *boundary = রিক্ত;
  ৱর্ণ *customct;
  ধ্রুৱ ৱর্ণ *cte = রিক্ত;
  CURLcode result = CURLE_OK;

  যদি(++call_depth > MAX_MIME_LEVELS)
    ঘূৰাই CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = রিক্ত;

  /* Be sure we will not access old headers later. */
  যদি(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, রিক্ত);

  /* Check if content type is specified. */
  customct = part->mimetype;
  যদি(!customct)
    customct = অনুসন্ধান_header(part->userheaders, STRCONST("Content-Type"));
  যদি(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  যদি(!contenttype) {
    ৱিকল্প(part->kind) {
    পক্ষ MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      ভঙ্গ;
    পক্ষ MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      যদি(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      যদি(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      ভঙ্গ;
    স্ৱভাৱ:
      contenttype = Curl_mime_contenttype(part->filename);
      ভঙ্গ;
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
    contenttype = রিক্ত;

  /* Issue content-disposition header only if not already set by caller. */
  যদি(!অনুসন্ধান_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    যদি(result)
      ঘূৰাই result;
  }

  /* Issue Content-Type header. */
  যদি(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    যদি(result)
      ঘূৰাই result;
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
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      যদি(result)
        ঘূৰাই result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  যদি(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  যদি(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = রিক্ত;
    যদি(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    বাবে(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, রিক্ত,
                                    disposition, strategy, call_depth);
      যদি(result)
        ঘূৰাই result;
    }
  }
  ঘূৰাই result;
}

CURLcode Curl_mime_prepare_headers(সংরচনা Curl_easy *data,
                                   curl_mimepart *part,
                                   ধ্রুৱ ৱর্ণ *contenttype,
                                   ধ্রুৱ ৱর্ণ *disposition,
                                   গণনা mimestrategy strategy)
{
  ঘূৰাই mime_prepare_headers(data, part, contenttype, disposition,
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

        বাবে(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

সংরচনা cr_mime_ctx {
  সংরচনা Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  সংরচনা bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

স্থির CURLcode cr_mime_init(সংরচনা Curl_easy *data,
                             সংরচনা Curl_creader *reader)
{
  সংরচনা cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cr_mime_close(সংরচনা Curl_easy *data,
                          সংরচনা Curl_creader *reader)
{
  সংরচনা cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
স্থির CURLcode cr_mime_read(সংরচনা Curl_easy *data,
                             সংরচনা Curl_creader *reader,
                             ৱর্ণ *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  সংরচনা cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  ৱর্ণ tmp[256];

  /* Once we have errored, we will return the same error forever */
  যদি(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (পূর্ণাঙ্ক)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    ঘূৰাই ctx->error_result;
  }
  যদি(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    ঘূৰাই CURLE_OK;
  }
  /* respect length limitations */
  যদি(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    যদি(remain <= 0)
      blen = 0;
    অন্যথা যদি(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  যদি(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned ৱর্ণ *)buf, blen, &nread);
    যদি(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      ঘূৰাই result;
    }
  }
  অন্যথা যদি(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, পরিমাণ(tmp), ctx->part);
    যদি(nread <= পরিমাণ(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned ৱর্ণ *)tmp, nread, &n);
      যদি(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ঘূৰাই result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      যদি(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ঘূৰাই result;
      }
    }
  }
  অন্যথা
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  ৱিকল্প(nread) {
  পক্ষ 0:
    যদি((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      ঘূৰাই CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    ভঙ্গ;

  পক্ষ CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    ভঙ্গ;

  পক্ষ CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    ভঙ্গ; /* nothing was read */

  পক্ষ STOP_FILLING:
  পক্ষ READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    ভঙ্গ;

  স্ৱভাৱ:
    যদি(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      ঘূৰাই CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    যদি(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    ভঙ্গ;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (পূর্ণাঙ্ক)result, *pnread, *peos);
  ঘূৰাই result;
}

স্থির bool cr_mime_needs_rewind(সংরচনা Curl_easy *data,
                                 সংরচনা Curl_creader *reader)
{
  সংরচনা cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ঘূৰাই ctx->read_len > 0;
}

স্থির curl_off_t cr_mime_total_length(সংরচনা Curl_easy *data,
                                       সংরচনা Curl_creader *reader)
{
  সংরচনা cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ঘূৰাই ctx->total_len;
}

স্থির CURLcode cr_mime_resume_from(সংরচনা Curl_easy *data,
                                    সংরচনা Curl_creader *reader,
                                    curl_off_t offset)
{
  সংরচনা cr_mime_ctx *ctx = reader->ctx;

  যদি(offset > 0) {
    curl_off_t passed = 0;

    কৰা {
      ৱর্ণ scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)পরিমাণ(scratch)) ?
        পরিমাণ(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      যদি((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        ঘূৰাই CURLE_READ_ERROR;
      }
    } যেতিয়ালৈ(passed < offset);

    /* now, decrease the size of the read */
    যদি(ctx->total_len > 0) {
      ctx->total_len -= offset;

      যদি(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        ঘূৰাই CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cr_mime_cntrl(সংরচনা Curl_easy *data,
                              সংরচনা Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  সংরচনা cr_mime_ctx *ctx = reader->ctx;
  ৱিকল্প(opcode) {
  পক্ষ CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    যদি(result)
      failf(data, "Cannot rewind mime/post data");
    ঘূৰাই result;
  }
  পক্ষ CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    ভঙ্গ;
  পক্ষ CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির bool cr_mime_is_paused(সংরচনা Curl_easy *data,
                              সংরচনা Curl_creader *reader)
{
  সংরচনা cr_mime_ctx *ctx = reader->ctx;
  (শূন্য)data;
  ঘূৰাই ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

স্থির ধ্রুৱ সংরচনা Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_total_length,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_done,
  পরিমাণ(সংরচনা cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(সংরচনা Curl_easy *data, curl_mimepart *part)
{
  সংরচনা Curl_creader *r;
  সংরচনা cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  যদি(result)
    ঘূৰাই result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  যদি(result) {
    Curl_creader_free(data, r);
    ঘূৰাই result;
  }
  ctx->total_len = mime_size(ctx->part);

  ঘূৰাই Curl_creader_set(data, r);
}

#অন্যথা /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (শূন্য)easy;
  ঘূৰাই রিক্ত;
}

শূন্য curl_mime_free(curl_mime *mime)
{
  (শূন্য)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (শূন্য)mime;
  ঘূৰাই রিক্ত;
}

CURLcode curl_mime_name(curl_mimepart *part, ধ্রুৱ ৱর্ণ *name)
{
  (শূন্য)part;
  (শূন্য)name;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, ধ্রুৱ ৱর্ণ *filename)
{
  (শূন্য)part;
  (শূন্য)filename;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, ধ্রুৱ ৱর্ণ *mimetype)
{
  (শূন্য)part;
  (শূন্য)mimetype;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ধ্রুৱ ৱর্ণ *encoding)
{
  (শূন্য)part;
  (শূন্য)encoding;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, ধ্রুৱ ৱর্ণ *data, size_t datasize)
{
  (শূন্য)part;
  (শূন্য)data;
  (শূন্য)datasize;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ধ্রুৱ ৱর্ণ *filename)
{
  (শূন্য)part;
  (শূন্য)filename;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
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
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (শূন্য)part;
  (শূন্য)subparts;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           সংরচনা curl_slist *headers, পূর্ণাঙ্ক take_ownership)
{
  (শূন্য)part;
  (শূন্য)headers;
  (শূন্য)take_ownership;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(সংরচনা curl_slist **slp, ধ্রুৱ ৱর্ণ *fmt, ...)
{
  (শূন্য)slp;
  (শূন্য)fmt;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

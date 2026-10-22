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
#include "curl_setup.h"

sa_nracanaa Curl_easy;

#include "mime.h"
#include "urldata.h"
#include "sendf.h"
#include "curl_trc.h"
#include "transfer.h"
#include "curlx/strdup.h"
#include "curlx/basename.h"
#include "curlx/strcopy.h"
#include "curlx/win32-fopen.h"
#include "curlx/base64.h"

#ᮚᮦᮔ᮪ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#include "rand.h"
#include "slist.h"
#include "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

static size_t mime_subparts_read(var_na *buffer, size_t size, size_t nitems,
                                 void *instream, bool *hasread);
static curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
static const unsigned var_na qp_class[] = {
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
static const var_na aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#ᮜᮤᮚ

#include <fabdef.h>
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
curl_off_t VmsRealFileSize(const var_na *name, const curlx_struct_stat *stat_buf)
{
  var_na buffer[8192];
  curl_off_t count;
  puur_naa_nka ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  ᮚᮦᮔ᮪(!file)
    ᮝᮜᮤ 0;

  count = 0;
  ret_stat = 1;
  ᮞᮨᮓᮨᮜ(ret_stat > 0) {
    ret_stat = fread(buffer, 1, parimaa_na(buffer), file);
    ᮚᮦᮔ᮪(ret_stat)
      count += ret_stat;
  }
  curlx_fclose(file);

  ᮝᮜᮤ count;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
static curl_off_t VmsSpecialSize(const var_na *name,
                                 const curlx_struct_stat *stat_buf)
{
  switch(stat_buf->st_fab_rfm) {
  pak_sa FAB$C_VAR:
  pak_sa FAB$C_VFC:
    ᮝᮜᮤ VmsRealFileSize(name, stat_buf);
    bha_nga;
  default:
    ᮝᮜᮤ stat_buf->st_size;
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
static FILE *vmsfopenread(const var_na *file, const var_na *mode)
{
  curlx_struct_stat statbuf;
  puur_naa_nka res = curlx_stat(file, &statbuf);

  ᮚᮦᮔ᮪(res != -1) {
    switch(statbuf.st_fab_rfm) {
    pak_sa FAB$C_VAR:
    pak_sa FAB$C_VFC:
    pak_sa FAB$C_STMCR:
      ᮝᮜᮤ curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      bha_nga;
    default:
      ᮝᮜᮤ curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  ᮝᮜᮤ NULL;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
static void mimesetstate(sa_nracanaa mime_state *state,
                         ga_nanaa mimestate tok, void *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
static var_na *escape_string(sa_nracanaa Curl_easy *data,
                           const var_na *src, ga_nanaa mimestrategy strategy)
{
  CURLcode result;
  sa_nracanaa dynbuf db;
  const var_na * const *table;
  const var_na * const *p;
  /* replace first character by rest of string. */
  static const var_na * const mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    NULL
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  static const var_na * const formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    NULL
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  ᮚᮦᮔ᮪(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_INPUT_LENGTH);

  for(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    for(p = table; *p && **p != *src; p++)
      ;

    ᮚᮦᮔ᮪(*p)
      result = curlx_dyn_add(&db, *p + 1);
    ᮜᮤᮚ
      result = curlx_dyn_addn(&db, src, 1);
  }

  ᮝᮜᮤ curlx_dyn_ptr(&db);
}

/* Check if header matches. */
static var_na *match_header(sa_nracanaa curl_slist *hdr, const var_na *lbl, size_t len)
{
  var_na *value = NULL;

  ᮚᮦᮔ᮪(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    for(value = hdr->data + len + 1; *value == ' '; value++)
      ;
  ᮝᮜᮤ value;
}

/* Get a header from an slist. */
static var_na *search_header(sa_nracanaa curl_slist *hdrlist,
                           const var_na *hdr, size_t len)
{
  var_na *value = NULL;

  for(; !value && hdrlist; hdrlist = hdrlist->next)
    value = match_header(hdrlist, hdr, len);

  ᮝᮜᮤ value;
}

static var_na *strippath(const var_na *fullfile)
{
  var_na *filename;
  var_na *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  ᮚᮦᮔ᮪(!filename)
    ᮝᮜᮤ NULL;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  ᮝᮜᮤ base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
static void cleanup_encoder_state(sa_nracanaa mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
static size_t encoder_nop_read(var_na *buffer, size_t size, bool ateof,
                               sa_nracanaa curl_mimepart *part)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (void)ateof;

  ᮚᮦᮔ᮪(!size)
    ᮝᮜᮤ STOP_FILLING;

  ᮚᮦᮔ᮪(size > insize)
    size = insize;

  ᮚᮦᮔ᮪(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  ᮝᮜᮤ size;
}

static curl_off_t encoder_nop_size(curl_mimepart *part)
{
  ᮝᮜᮤ part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
static size_t encoder_7bit_read(var_na *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (void)ateof;

  ᮚᮦᮔ᮪(!size)
    ᮝᮜᮤ STOP_FILLING;

  ᮚᮦᮔ᮪(size > cursize)
    size = cursize;

  for(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    ᮚᮦᮔ᮪(*buffer++ & 0x80)
      ᮝᮜᮤ cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  ᮝᮜᮤ cursize;
}

/* Base64 content encoder. */
static size_t encoder_base64_read(var_na *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  puur_naa_nka i;
  var_na *ptr = buffer;

  ᮞᮨᮓᮨᮜ(st->bufbeg < st->bufend) {
    /* Line full ? */
    ᮚᮦᮔ᮪(st->pos > MAX_ENCODED_LINE_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      ᮚᮦᮔ᮪(size < 2) {
        ᮚᮦᮔ᮪(!cursize)
          ᮝᮜᮤ STOP_FILLING;
        bha_nga;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    ᮚᮦᮔ᮪(size < 4) {
      ᮚᮦᮔ᮪(!cursize)
        ᮝᮜᮤ STOP_FILLING;
      bha_nga;
    }
    ᮚᮦᮔ᮪(st->bufend - st->bufbeg < 3)
      bha_nga;

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
  ᮚᮦᮔ᮪(ateof) {
    ᮚᮦᮔ᮪(size < 4) {
      ᮚᮦᮔ᮪(!cursize)
        ᮝᮜᮤ STOP_FILLING;
    }
    ᮜᮤᮚ {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      ᮚᮦᮔ᮪(st->bufend != st->bufbeg) {

        ᮚᮦᮔ᮪(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        ᮚᮦᮔ᮪(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  ᮝᮜᮤ cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_INPUT_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_LINE_LENGTH + 2)) *                  \
    MAX_ENCODED_LINE_LENGTH / 4) * 3 - 3)

static curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  ᮚᮦᮔ᮪(size <= 0)
    ᮝᮜᮤ size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  ᮚᮦᮔ᮪(size > BASE64_MAX_INPUT_SIZE)
    ᮝᮜᮤ -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  ᮝᮜᮤ size + (2 * ((size - 1) / MAX_ENCODED_LINE_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
static puur_naa_nka qp_lookahead_eol(sa_nracanaa mime_encoder_state *st, puur_naa_nka ateof, size_t n)
{
  n += st->bufbeg;
  ᮚᮦᮔ᮪(n >= st->bufend && ateof)
    ᮝᮜᮤ 1;
  ᮚᮦᮔ᮪(n + 2 > st->bufend)
    ᮝᮜᮤ ateof ? 0 : -1;
  ᮚᮦᮔ᮪(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    ᮝᮜᮤ 1;
  ᮝᮜᮤ 0;
}

/* Quoted-printable encoder. */
static size_t encoder_qp_read(var_na *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  var_na *ptr = buffer;
  size_t cursize = 0;
  puur_naa_nka softlinebreak;
  var_na buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  ᮞᮨᮓᮨᮜ(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    puur_naa_nka i = (unsigned var_na)st->buf[st->bufbeg];
    buf[0] = (var_na)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    switch(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    pak_sa QP_OK:          /* Not a special character. */
      bha_nga;
    pak_sa QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      switch(qp_lookahead_eol(st, ateof, 1)) {
      pak_sa -1:          /* More input data needed. */
        ᮝᮜᮤ cursize;
      pak_sa 0:           /* No encoding needed. */
        bha_nga;
      default:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        bha_nga;
      }
      bha_nga;
    pak_sa QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      switch(qp_lookahead_eol(st, ateof, 0)) {
      pak_sa -1:          /* Need more data. */
        ᮝᮜᮤ cursize;
      pak_sa 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        bha_nga;
      default:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        bha_nga;
      }
      bha_nga;
    default:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      bha_nga;
    }

    /* Be sure the encoded character fits within maximum line length. */
    ᮚᮦᮔ᮪(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_LINE_LENGTH;
      ᮚᮦᮔ᮪(!softlinebreak && st->pos + len == MAX_ENCODED_LINE_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        switch(qp_lookahead_eol(st, ateof, consumed)) {
        pak_sa -1:        /* Need more data. */
          ᮝᮜᮤ cursize;
        pak_sa 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          bha_nga;
        }
      }
      ᮚᮦᮔ᮪(softlinebreak) {
        curlx_strcopy(buf, parimaa_na(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    ᮚᮦᮔ᮪(len > size) {
      ᮚᮦᮔ᮪(!cursize)
        ᮝᮜᮤ STOP_FILLING;
      bha_nga;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    ᮚᮦᮔ᮪(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  ᮝᮜᮤ cursize;
}

static curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  ᮝᮜᮤ part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
static size_t mime_mem_read(var_na *buffer, size_t size, size_t nitems,
                            void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (void)size;  /* Always 1 */

  ᮚᮦᮔ᮪(!nitems)
    ᮝᮜᮤ STOP_FILLING;

  ᮚᮦᮔ᮪(sz > nitems)
    sz = nitems;

  ᮚᮦᮔ᮪(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  ᮝᮜᮤ sz;
}

static puur_naa_nka mime_mem_seek(void *instream, curl_off_t offset, puur_naa_nka whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  switch(whence) {
  pak_sa SEEK_CUR:
    offset += part->state.offset;
    bha_nga;
  pak_sa SEEK_END:
    offset += part->datasize;
    bha_nga;
  }

  ᮚᮦᮔ᮪(offset < 0 || offset > part->datasize)
    ᮝᮜᮤ CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  ᮝᮜᮤ CURL_SEEKFUNC_OK;
}

static void mime_mem_free(void *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
static bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  ᮚᮦᮔ᮪(part->fp)
    ᮝᮜᮤ FALSE;
  part->fp = fopen_read(part->data, "rb");
  ᮝᮜᮤ part->fp ? FALSE : TRUE;
}

static size_t mime_file_read(var_na *buffer, size_t size, size_t nitems,
                             void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ᮚᮦᮔ᮪(!nitems)
    ᮝᮜᮤ STOP_FILLING;

  ᮚᮦᮔ᮪(mime_open_file(part))
    ᮝᮜᮤ READ_ERROR;

  ᮝᮜᮤ fread(buffer, size, nitems, part->fp);
}

static puur_naa_nka mime_file_seek(void *instream, curl_off_t offset, puur_naa_nka whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ᮚᮦᮔ᮪(whence == SEEK_SET && !offset && !part->fp)
    ᮝᮜᮤ CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  ᮚᮦᮔ᮪(mime_open_file(part))
    ᮝᮜᮤ CURL_SEEKFUNC_FAIL;

  ᮝᮜᮤ curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

static void mime_file_free(void *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  ᮚᮦᮔ᮪(part->fp) {
    curlx_fclose(part->fp);
    part->fp = NULL;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
static size_t readback_bytes(sa_nracanaa mime_state *state,
                             var_na *buffer, size_t bufsize,
                             const var_na *bytes, size_t numbytes,
                             const var_na *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  ᮚᮦᮔ᮪(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  ᮜᮤᮚ {
    sz = offset - numbytes;
    ᮚᮦᮔ᮪(sz >= traillen)
      ᮝᮜᮤ 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  ᮚᮦᮔ᮪(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  ᮝᮜᮤ sz;
}

/* Read a non-encoded part content. */

static size_t read_part_content(curl_mimepart *part, var_na *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  switch(part->lastreadstatus) {
  pak_sa 0:
  pak_sa CURL_READFUNC_ABORT:
  pak_sa CURL_READFUNC_PAUSE:
  pak_sa READ_ERROR:
    ᮝᮜᮤ part->lastreadstatus;
  default:
    bha_nga;
  }

  /* If we can determine we are at end of part data, spare a read. */
  ᮚᮦᮔ᮪(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  ᮜᮤᮚ {
    switch(part->kind) {
    pak_sa MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      bha_nga;
    pak_sa MIMEKIND_FILE:
      ᮚᮦᮔ᮪(part->fp && feof(part->fp))
        bha_nga;  /* At EOF. */
      FALLTHROUGH();
    default:
      ᮚᮦᮔ᮪(part->readfunc) {
        ᮚᮦᮔ᮪(!(part->flags & MIME_FAST_READ)) {
          ᮚᮦᮔ᮪(*hasread)
            ᮝᮜᮤ STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      bha_nga;
    }
  }

  switch(sz) {
  pak_sa STOP_FILLING:
    bha_nga;
  pak_sa 0:
  pak_sa CURL_READFUNC_ABORT:
  pak_sa CURL_READFUNC_PAUSE:
  pak_sa READ_ERROR:
    part->lastreadstatus = sz;
    bha_nga;
  default:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    bha_nga;
  }

  ᮝᮜᮤ sz;
}

/* Read and encode part content. */
static size_t read_encoded_part_content(curl_mimepart *part, var_na *buffer,
                                        size_t bufsize, bool *hasread)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  for(;;) {
    ᮚᮦᮔ᮪(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      switch(sz) {
      pak_sa 0:
        ᮚᮦᮔ᮪(ateof)
          ᮝᮜᮤ cursize;
        bha_nga;
      pak_sa READ_ERROR:
      pak_sa STOP_FILLING:
        ᮝᮜᮤ cursize ? cursize : sz;
      default:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        continue;
      }
    }

    /* We need more data in input buffer. */
    ᮚᮦᮔ᮪(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      ᮚᮦᮔ᮪(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    ᮚᮦᮔ᮪(st->bufend >= parimaa_na(st->buf))
      ᮝᮜᮤ cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           parimaa_na(st->buf) - st->bufend, hasread);
    switch(sz) {
    pak_sa 0:
      ateof = TRUE;
      bha_nga;
    pak_sa CURL_READFUNC_ABORT:
    pak_sa CURL_READFUNC_PAUSE:
    pak_sa READ_ERROR:
    pak_sa STOP_FILLING:
      ᮝᮜᮤ cursize ? cursize : sz;
    default:
      st->bufend += sz;
      bha_nga;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
static size_t readback_part(curl_mimepart *part,
                            var_na *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  ᮞᮨᮓᮨᮜ(bufsize) {
    size_t sz = 0;
    sa_nracanaa curl_slist *hdr = (sa_nracanaa curl_slist *)part->state.ptr;
    switch(part->state.state) {
    pak_sa MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      bha_nga;
    pak_sa MIMESTATE_USERHEADERS:
      ᮚᮦᮔ᮪(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, NULL);
        bha_nga;
      }
      ᮚᮦᮔ᮪(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        bha_nga;
      }
      FALLTHROUGH();
    pak_sa MIMESTATE_CURLHEADERS:
      ᮚᮦᮔ᮪(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      ᮜᮤᮚ {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        ᮚᮦᮔ᮪(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      bha_nga;
    pak_sa MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      ᮚᮦᮔ᮪(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, NULL);
      bha_nga;
    pak_sa MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, NULL);
      bha_nga;
    pak_sa MIMESTATE_CONTENT:
      ᮚᮦᮔ᮪(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      ᮜᮤᮚ
        sz = read_part_content(part, buffer, bufsize, hasread);
      switch(sz) {
      pak_sa 0:
        mimesetstate(&part->state, MIMESTATE_END, NULL);
        /* Try sparing open file descriptors. */
        ᮚᮦᮔ᮪(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = NULL;
        }
        FALLTHROUGH();
      pak_sa CURL_READFUNC_ABORT:
      pak_sa CURL_READFUNC_PAUSE:
      pak_sa READ_ERROR:
      pak_sa STOP_FILLING:
        ᮝᮜᮤ cursize ? cursize : sz;
      }
      bha_nga;
    pak_sa MIMESTATE_END:
      ᮝᮜᮤ cursize;
    default:
      bha_nga;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  ᮝᮜᮤ cursize;
}

/* Readback from mime. Warning: not a read callback function. */
static size_t mime_subparts_read(var_na *buffer, size_t size, size_t nitems,
                                 void *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (void)size;  /* Always 1 */

  ᮞᮨᮓᮨᮜ(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    switch(mime->state.state) {
    pak_sa MIMESTATE_BEGIN:
    pak_sa MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      bha_nga;
    pak_sa MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      ᮚᮦᮔ᮪(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      bha_nga;
    pak_sa MIMESTATE_BOUNDARY2:
      ᮚᮦᮔ᮪(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      ᮜᮤᮚ
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      ᮚᮦᮔ᮪(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      bha_nga;
    pak_sa MIMESTATE_CONTENT:
      ᮚᮦᮔ᮪(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, NULL);
        bha_nga;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      switch(sz) {
      pak_sa CURL_READFUNC_ABORT:
      pak_sa CURL_READFUNC_PAUSE:
      pak_sa READ_ERROR:
      pak_sa STOP_FILLING:
        ᮝᮜᮤ cursize ? cursize : sz;
      pak_sa 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        bha_nga;
      }
      bha_nga;
    pak_sa MIMESTATE_END:
      ᮝᮜᮤ cursize;
    default:
      bha_nga;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  ᮝᮜᮤ cursize;
}

static puur_naa_nka mime_part_rewind(curl_mimepart *part)
{
  puur_naa_nka res = CURL_SEEKFUNC_OK;
  ga_nanaa mimestate targetstate = MIMESTATE_BEGIN;

  ᮚᮦᮔ᮪(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  ᮚᮦᮔ᮪(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    ᮚᮦᮔ᮪(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      switch(res) {
      pak_sa CURL_SEEKFUNC_OK:
      pak_sa CURL_SEEKFUNC_FAIL:
      pak_sa CURL_SEEKFUNC_CANTSEEK:
        bha_nga;
      pak_sa -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        bha_nga;
      default:
        res = CURL_SEEKFUNC_FAIL;
        bha_nga;
      }
    }
  }

  ᮚᮦᮔ᮪(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, NULL);

  part->lastreadstatus = 1; /* Successful read status. */
  ᮝᮜᮤ res;
}

static puur_naa_nka mime_subparts_seek(void *instream, curl_off_t offset, puur_naa_nka whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  puur_naa_nka rc = CURL_SEEKFUNC_OK;

  ᮚᮦᮔ᮪(whence != SEEK_SET || offset)
    ᮝᮜᮤ CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  ᮚᮦᮔ᮪(mime->state.state == MIMESTATE_BEGIN)
    ᮝᮜᮤ CURL_SEEKFUNC_OK;           /* Already rewound. */

  for(part = mime->firstpart; part; part = part->nextpart) {
    puur_naa_nka res = mime_part_rewind(part);
    ᮚᮦᮔ᮪(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  ᮚᮦᮔ᮪(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, NULL);

  ᮝᮜᮤ rc;
}

/* Release part content. */
static void cleanup_part_content(curl_mimepart *part)
{
  ᮚᮦᮔ᮪(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = NULL;
  part->seekfunc = NULL;
  part->freefunc = NULL;
  part->arg = (void *)part;           /* Defaults to part itself. */
  part->data = NULL;
  part->fp = NULL;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned puur_naa_nka)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

static void mime_subparts_free(void *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ᮚᮦᮔ᮪(mime && mime->parent) {
    mime->parent->freefunc = NULL;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
static void mime_subparts_unbind(void *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ᮚᮦᮔ᮪(mime && mime->parent) {
    mime->parent->freefunc = NULL;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = NULL;
  }
}

void Curl_mime_cleanpart(curl_mimepart *part)
{
  ᮚᮦᮔ᮪(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    ᮚᮦᮔ᮪(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
void curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  ᮚᮦᮔ᮪(!mime)
    ᮝᮜᮤ;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  ᮞᮨᮓᮨᮜ(mime) {
    part = mime->firstpart;
    ᮚᮦᮔ᮪(part) {
      mime->firstpart = part->nextpart;
      ᮚᮦᮔ᮪(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = NULL;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    ᮜᮤᮚ {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

static bool mime_is_too_deep(const curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  ᮚᮦᮔ᮪(++call_depth > MAX_MIME_LEVELS)
    ᮝᮜᮤ TRUE;

  ᮚᮦᮔ᮪(part->kind == MIMEKIND_MULTIPART && part->arg) {
    const curl_mime *mime = (const curl_mime *)part->arg;
    const curl_mimepart *s;

    for(s = mime->firstpart; s; s = s->nextpart) {
      ᮚᮦᮔ᮪(mime_is_too_deep(s, call_depth))
        ᮝᮜᮤ TRUE;
    }
  }

  ᮝᮜᮤ FALSE;
}

static CURLcode mime_duppart(sa_nracanaa Curl_easy *data,
                             curl_mimepart *dst, const curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  const curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  ᮚᮦᮔ᮪(++call_depth > MAX_MIME_LEVELS)
    ᮝᮜᮤ CURLE_TOO_LARGE;

  /* Duplicate content. */
  switch(src->kind) {
  pak_sa MIMEKIND_NONE:
    bha_nga;
  pak_sa MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    bha_nga;
  pak_sa MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    ᮚᮦᮔ᮪(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    bha_nga;
  pak_sa MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    bha_nga;
  pak_sa MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    for(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    bha_nga;
  default:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    bha_nga;
  }

  /* Duplicate headers. */
  ᮚᮦᮔ᮪(!result && src->userheaders) {
    sa_nracanaa curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    ᮚᮦᮔ᮪(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    ᮜᮤᮚ {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      ᮚᮦᮔ᮪(result)
        curl_slist_free_all(hdrs);
    }
  }

  ᮚᮦᮔ᮪(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  ᮚᮦᮔ᮪(!result)
    result = curl_mime_name(dst, src->name);
  ᮚᮦᮔ᮪(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  ᮚᮦᮔ᮪(result)
    Curl_mime_cleanpart(dst);

  ᮝᮜᮤ result;
}

CURLcode Curl_mime_duppart(sa_nracanaa Curl_easy *data,
                           curl_mimepart *dst, const curl_mimepart *src)
{
  ᮚᮦᮔ᮪(!dst || !src)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  ᮚᮦᮔ᮪(mime_is_too_deep(src, 0))
    ᮝᮜᮤ CURLE_TOO_LARGE;

  ᮝᮜᮤ mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(void *easy)
{
  curl_mime *mime = curlx_malloc(parimaa_na(*mime));

  ᮚᮦᮔ᮪(mime) {
    mime->parent = NULL;
    mime->firstpart = NULL;
    mime->lastpart = NULL;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    ᮚᮦᮔ᮪(Curl_rand_alnum(easy,
                       (unsigned var_na *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      ᮝᮜᮤ NULL;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, NULL);
  }

  ᮝᮜᮤ mime;
}

/* Initialize a mime part. */
void Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, parimaa_na(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, NULL);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  ᮚᮦᮔ᮪(!mime)
    ᮝᮜᮤ NULL;

  part = curlx_malloc(parimaa_na(*part));

  ᮚᮦᮔ᮪(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    ᮚᮦᮔ᮪(mime->lastpart)
      mime->lastpart->nextpart = part;
    ᮜᮤᮚ
      mime->firstpart = part;

    mime->lastpart = part;
  }

  ᮝᮜᮤ part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, const var_na *name)
{
  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  ᮚᮦᮔ᮪(name) {
    part->name = curlx_strdup(name);
    ᮚᮦᮔ᮪(!part->name)
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  }

  ᮝᮜᮤ CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, const var_na *filename)
{
  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  ᮚᮦᮔ᮪(filename) {
    part->filename = curlx_strdup(filename);
    ᮚᮦᮔ᮪(!part->filename)
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  }

  ᮝᮜᮤ CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, const var_na *data, size_t datasize)
{
  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ᮚᮦᮔ᮪(data) {
    ᮚᮦᮔ᮪(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    ᮚᮦᮔ᮪(!part->data)
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  ᮝᮜᮤ CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, const var_na *filename)
{
  CURLcode result = CURLE_OK;

  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ᮚᮦᮔ᮪(filename) {
    var_na *base;
    curlx_struct_stat sbuf;

    ᮚᮦᮔ᮪(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    ᮜᮤᮚ {
      part->data = curlx_strdup(filename);
      ᮚᮦᮔ᮪(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      ᮜᮤᮚ {
        part->datasize = -1;
        ᮚᮦᮔ᮪(S_ISREG(sbuf.st_mode)) {
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
        ᮚᮦᮔ᮪(!base)
          result = CURLE_OUT_OF_MEMORY;
        ᮜᮤᮚ {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  ᮝᮜᮤ result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, const var_na *mimetype)
{
  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  ᮚᮦᮔ᮪(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    ᮚᮦᮔ᮪(!part->mimetype)
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  }

  ᮝᮜᮤ CURLE_OK;
}

static const sa_nracanaa mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, const var_na *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  const sa_nracanaa mime_encoder *mep;

  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ result;

  part->encoder = NULL;

  ᮚᮦᮔ᮪(!encoding)
    ᮝᮜᮤ CURLE_OK;    /* Removing current encoder. */

  for(mep = encoders; mep->name; mep++)
    ᮚᮦᮔ᮪(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  ᮝᮜᮤ result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           sa_nracanaa curl_slist *headers, puur_naa_nka take_ownership)
{
  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  ᮚᮦᮔ᮪(part->flags & MIME_USERHEADERS_OWNER) {
    ᮚᮦᮔ᮪(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned puur_naa_nka)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  ᮚᮦᮔ᮪(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  ᮝᮜᮤ CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, void *arg)
{
  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ᮚᮦᮔ᮪(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  ᮝᮜᮤ CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, puur_naa_nka take_ownership)
{
  curl_mime *root;

  ᮚᮦᮔ᮪(!part)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  ᮚᮦᮔ᮪(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    ᮝᮜᮤ CURLE_OK;

  cleanup_part_content(part);

  ᮚᮦᮔ᮪(subparts) {
    /* Should not have been attached already. */
    ᮚᮦᮔ᮪(subparts->parent)
      ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    root = part->parent;
    ᮚᮦᮔ᮪(root) {
      ᮞᮨᮓᮨᮜ(root->parent && root->parent->parent)
        root = root->parent->parent;
      ᮚᮦᮔ᮪(subparts == root) {
        /* cannot add as a subpart of itself. */
        ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    ᮚᮦᮔ᮪(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      ᮝᮜᮤ CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  ᮝᮜᮤ CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  ᮝᮜᮤ Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(var_na *buffer, size_t size, size_t nitems, void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (void)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  do {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } ᮞᮨᮓᮨᮜ(ret == STOP_FILLING);

  ᮝᮜᮤ ret;
}

/* Rewind mime stream. */
static CURLcode mime_rewind(curl_mimepart *part)
{
  ᮝᮜᮤ mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
static size_t slist_size(sa_nracanaa curl_slist *s,
                         size_t overhead, const var_na *skip, size_t skiplen)
{
  size_t size = 0;

  for(; s; s = s->next)
    ᮚᮦᮔ᮪(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  ᮝᮜᮤ size;
}

/* Get/compute multipart size. */
static curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  ᮚᮦᮔ᮪(!mime)
    ᮝᮜᮤ 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  for(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    ᮚᮦᮔ᮪(sz < 0)
      size = sz;

    ᮚᮦᮔ᮪(size >= 0)
      size += boundarysize + sz;
  }

  ᮝᮜᮤ size;
}

/* Get/compute mime size. */
static curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  ᮚᮦᮔ᮪(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  ᮚᮦᮔ᮪(part->encoder)
    size = part->encoder->sizefunc(part);

  ᮚᮦᮔ᮪(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, NULL, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  ᮝᮜᮤ size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(sa_nracanaa curl_slist **slp, const var_na *fmt, ...)
{
  sa_nracanaa curl_slist *hdr = NULL;
  var_na *s = NULL;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  ᮚᮦᮔ᮪(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    ᮚᮦᮔ᮪(hdr)
      *slp = hdr;
    ᮜᮤᮚ
      curlx_free(s);
  }

  ᮝᮜᮤ hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
static CURLcode add_content_type(sa_nracanaa curl_slist **slp,
                                 const var_na *type, const var_na *boundary)
{
  ᮝᮜᮤ Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

const var_na *Curl_mime_contenttype(const var_na *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  sa_nracanaa ContentType {
    const var_na *extension;
    const var_na *type;
  };
  static const sa_nracanaa ContentType ctts[] = {
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

  ᮚᮦᮔ᮪(filename) {
    size_t len1 = strlen(filename);
    const var_na *nameend = filename + len1;
    unsigned puur_naa_nka i;

    for(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      ᮚᮦᮔ᮪(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        ᮝᮜᮤ ctts[i].type;
    }
  }
  ᮝᮜᮤ NULL;
}

static bool content_type_match(const var_na *contenttype,
                               const var_na *target, size_t len)
{
  ᮚᮦᮔ᮪(contenttype && curl_strnequal(contenttype, target, len))
    switch(contenttype[len]) {
    pak_sa '\0':
    pak_sa '\t':
    pak_sa '\r':
    pak_sa '\n':
    pak_sa ' ':
    pak_sa ';':
      ᮝᮜᮤ TRUE;
    }
  ᮝᮜᮤ FALSE;
}

static CURLcode add_content_disposition(sa_nracanaa Curl_easy *data,
                                        curl_mimepart *part,
                                        const var_na *disposition,
                                        const var_na *contenttype,
                                        ga_nanaa mimestrategy strategy)
{
  ᮚᮦᮔ᮪(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  ᮚᮦᮔ᮪(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = NULL;

  ᮚᮦᮔ᮪(disposition) {
    CURLcode result = CURLE_OK;
    var_na *name = NULL;
    var_na *filename = NULL;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    ᮚᮦᮔ᮪(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

    ᮚᮦᮔ᮪(part->name) {
      name = escape_string(data, part->name, strategy);
      ᮚᮦᮔ᮪(!name)
        ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
    }
    ᮚᮦᮔ᮪(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      ᮚᮦᮔ᮪(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    ᮚᮦᮔ᮪(!result)
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
    ᮚᮦᮔ᮪(result)
      ᮝᮜᮤ result;
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode mime_prepare_headers(sa_nracanaa Curl_easy *data,
                                     curl_mimepart *part,
                                     const var_na *contenttype,
                                     const var_na *disposition,
                                     ga_nanaa mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = NULL;
  const var_na *boundary = NULL;
  var_na *customct;
  const var_na *cte = NULL;
  CURLcode result = CURLE_OK;

  ᮚᮦᮔ᮪(++call_depth > MAX_MIME_LEVELS)
    ᮝᮜᮤ CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = NULL;

  /* Be sure we will not access old headers later. */
  ᮚᮦᮔ᮪(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, NULL);

  /* Check if content type is specified. */
  customct = part->mimetype;
  ᮚᮦᮔ᮪(!customct)
    customct = search_header(part->userheaders, STRCONST("Content-Type"));
  ᮚᮦᮔ᮪(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  ᮚᮦᮔ᮪(!contenttype) {
    switch(part->kind) {
    pak_sa MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      bha_nga;
    pak_sa MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      ᮚᮦᮔ᮪(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      ᮚᮦᮔ᮪(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      bha_nga;
    default:
      contenttype = Curl_mime_contenttype(part->filename);
      bha_nga;
    }
  }

  ᮚᮦᮔ᮪(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    ᮚᮦᮔ᮪(mime)
      boundary = mime->boundary;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = NULL;

  /* Issue content-disposition header only if not already set by caller. */
  ᮚᮦᮔ᮪(!search_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    ᮚᮦᮔ᮪(result)
      ᮝᮜᮤ result;
  }

  /* Issue Content-Type header. */
  ᮚᮦᮔ᮪(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    ᮚᮦᮔ᮪(result)
      ᮝᮜᮤ result;
  }

  /* Content-Transfer-Encoding header. */
  ᮚᮦᮔ᮪(!search_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    ᮚᮦᮔ᮪(part->encoder)
      cte = part->encoder->name;
    ᮜᮤᮚ ᮚᮦᮔ᮪(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    ᮚᮦᮔ᮪(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      ᮚᮦᮔ᮪(result)
        ᮝᮜᮤ result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  ᮚᮦᮔ᮪(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  ᮚᮦᮔ᮪(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = NULL;
    ᮚᮦᮔ᮪(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    for(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, NULL,
                                    disposition, strategy, call_depth);
      ᮚᮦᮔ᮪(result)
        ᮝᮜᮤ result;
    }
  }
  ᮝᮜᮤ result;
}

CURLcode Curl_mime_prepare_headers(sa_nracanaa Curl_easy *data,
                                   curl_mimepart *part,
                                   const var_na *contenttype,
                                   const var_na *disposition,
                                   ga_nanaa mimestrategy strategy)
{
  ᮝᮜᮤ mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
static void mime_unpause(curl_mimepart *part)
{
  ᮚᮦᮔ᮪(part) {
    ᮚᮦᮔ᮪(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    ᮚᮦᮔ᮪(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      ᮚᮦᮔ᮪(mime) {
        curl_mimepart *subpart;

        for(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

sa_nracanaa cr_mime_ctx {
  sa_nracanaa Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  sa_nracanaa bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

static CURLcode cr_mime_init(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  ᮝᮜᮤ CURLE_OK;
}

static void cr_mime_close(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
static CURLcode cr_mime_read(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_creader *reader,
                             var_na *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  var_na tmp[256];

  /* Once we have errored, we will return the same error forever */
  ᮚᮦᮔ᮪(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (puur_naa_nka)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    ᮝᮜᮤ ctx->error_result;
  }
  ᮚᮦᮔ᮪(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    ᮝᮜᮤ CURLE_OK;
  }
  /* respect length limitations */
  ᮚᮦᮔ᮪(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    ᮚᮦᮔ᮪(remain <= 0)
      blen = 0;
    ᮜᮤᮚ ᮚᮦᮔ᮪(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  ᮚᮦᮔ᮪(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned var_na *)buf, blen, &nread);
    ᮚᮦᮔ᮪(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      ᮝᮜᮤ result;
    }
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, parimaa_na(tmp), ctx->part);
    ᮚᮦᮔ᮪(nread <= parimaa_na(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned var_na *)tmp, nread, &n);
      ᮚᮦᮔ᮪(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ᮝᮜᮤ result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      ᮚᮦᮔ᮪(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ᮝᮜᮤ result;
      }
    }
  }
  ᮜᮤᮚ
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  switch(nread) {
  pak_sa 0:
    ᮚᮦᮔ᮪((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      ᮝᮜᮤ CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    bha_nga;

  pak_sa CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    bha_nga;

  pak_sa CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    bha_nga; /* nothing was read */

  pak_sa STOP_FILLING:
  pak_sa READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    bha_nga;

  default:
    ᮚᮦᮔ᮪(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      ᮝᮜᮤ CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    ᮚᮦᮔ᮪(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    bha_nga;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (puur_naa_nka)result, *pnread, *peos);
  ᮝᮜᮤ result;
}

static bool cr_mime_needs_rewind(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  ᮝᮜᮤ ctx->read_len > 0;
}

static curl_off_t cr_mime_total_length(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  ᮝᮜᮤ ctx->total_len;
}

static CURLcode cr_mime_resume_from(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa Curl_creader *reader,
                                    curl_off_t offset)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;

  ᮚᮦᮔ᮪(offset > 0) {
    curl_off_t passed = 0;

    do {
      var_na scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)parimaa_na(scratch)) ?
        parimaa_na(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      ᮚᮦᮔ᮪((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        ᮝᮜᮤ CURLE_READ_ERROR;
      }
    } ᮞᮨᮓᮨᮜ(passed < offset);

    /* now, decrease the size of the read */
    ᮚᮦᮔ᮪(ctx->total_len > 0) {
      ctx->total_len -= offset;

      ᮚᮦᮔ᮪(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        ᮝᮜᮤ CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode cr_mime_cntrl(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  switch(opcode) {
  pak_sa CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    ᮚᮦᮔ᮪(result)
      failf(data, "Cannot rewind mime/post data");
    ᮝᮜᮤ result;
  }
  pak_sa CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    bha_nga;
  pak_sa CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    bha_nga;
  default:
    bha_nga;
  }
  ᮝᮜᮤ CURLE_OK;
}

static bool cr_mime_is_paused(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  ᮝᮜᮤ ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

static const sa_nracanaa Curl_crtype cr_mime = {
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
  parimaa_na(sa_nracanaa cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(sa_nracanaa Curl_easy *data, curl_mimepart *part)
{
  sa_nracanaa Curl_creader *r;
  sa_nracanaa cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  ᮚᮦᮔ᮪(result) {
    Curl_creader_free(data, r);
    ᮝᮜᮤ result;
  }
  ctx->total_len = mime_size(ctx->part);

  ᮝᮜᮤ Curl_creader_set(data, r);
}

#ᮜᮤᮚ /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (void)easy;
  ᮝᮜᮤ NULL;
}

void curl_mime_free(curl_mime *mime)
{
  (void)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (void)mime;
  ᮝᮜᮤ NULL;
}

CURLcode curl_mime_name(curl_mimepart *part, const var_na *name)
{
  (void)part;
  (void)name;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, const var_na *filename)
{
  (void)part;
  (void)filename;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, const var_na *mimetype)
{
  (void)part;
  (void)mimetype;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, const var_na *encoding)
{
  (void)part;
  (void)encoding;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, const var_na *data, size_t datasize)
{
  (void)part;
  (void)data;
  (void)datasize;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, const var_na *filename)
{
  (void)part;
  (void)filename;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           void *arg)
{
  (void)part;
  (void)datasize;
  (void)readfunc;
  (void)seekfunc;
  (void)freefunc;
  (void)arg;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (void)part;
  (void)subparts;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           sa_nracanaa curl_slist *headers, puur_naa_nka take_ownership)
{
  (void)part;
  (void)headers;
  (void)take_ownership;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(sa_nracanaa curl_slist **slp, const var_na *fmt, ...)
{
  (void)slp;
  (void)fmt;
  ᮝᮜᮤ CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

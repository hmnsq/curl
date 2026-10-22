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

#အကယ်၍ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
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

#မဟုတ်ပါက

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
  ကိန်းပြည့် ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  အကယ်၍(!file)
    ပြန်ပေး 0;

  count = 0;
  ret_stat = 1;
  နေစဉ်(ret_stat > 0) {
    ret_stat = fread(buffer, 1, parimaa_na(buffer), file);
    အကယ်၍(ret_stat)
      count += ret_stat;
  }
  curlx_fclose(file);

  ပြန်ပေး count;
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
    ပြန်ပေး VmsRealFileSize(name, stat_buf);
    bha_nga;
  default:
    ပြန်ပေး stat_buf->st_size;
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
  ကိန်းပြည့် res = curlx_stat(file, &statbuf);

  အကယ်၍(res != -1) {
    switch(statbuf.st_fab_rfm) {
    pak_sa FAB$C_VAR:
    pak_sa FAB$C_VFC:
    pak_sa FAB$C_STMCR:
      ပြန်ပေး curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      bha_nga;
    default:
      ပြန်ပေး curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  ပြန်ပေး NULL;
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
  အကယ်၍(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_INPUT_LENGTH);

  အတွက်(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    အတွက်(p = table; *p && **p != *src; p++)
      ;

    အကယ်၍(*p)
      result = curlx_dyn_add(&db, *p + 1);
    မဟုတ်ပါက
      result = curlx_dyn_addn(&db, src, 1);
  }

  ပြန်ပေး curlx_dyn_ptr(&db);
}

/* Check if header matches. */
static var_na *match_header(sa_nracanaa curl_slist *hdr, const var_na *lbl, size_t len)
{
  var_na *value = NULL;

  အကယ်၍(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    အတွက်(value = hdr->data + len + 1; *value == ' '; value++)
      ;
  ပြန်ပေး value;
}

/* Get a header from an slist. */
static var_na *search_header(sa_nracanaa curl_slist *hdrlist,
                           const var_na *hdr, size_t len)
{
  var_na *value = NULL;

  အတွက်(; !value && hdrlist; hdrlist = hdrlist->next)
    value = match_header(hdrlist, hdr, len);

  ပြန်ပေး value;
}

static var_na *strippath(const var_na *fullfile)
{
  var_na *filename;
  var_na *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  အကယ်၍(!filename)
    ပြန်ပေး NULL;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  ပြန်ပေး base; /* returns an allocated string or NULL ! */
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

  အကယ်၍(!size)
    ပြန်ပေး STOP_FILLING;

  အကယ်၍(size > insize)
    size = insize;

  အကယ်၍(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  ပြန်ပေး size;
}

static curl_off_t encoder_nop_size(curl_mimepart *part)
{
  ပြန်ပေး part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
static size_t encoder_7bit_read(var_na *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (void)ateof;

  အကယ်၍(!size)
    ပြန်ပေး STOP_FILLING;

  အကယ်၍(size > cursize)
    size = cursize;

  အတွက်(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    အကယ်၍(*buffer++ & 0x80)
      ပြန်ပေး cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  ပြန်ပေး cursize;
}

/* Base64 content encoder. */
static size_t encoder_base64_read(var_na *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  ကိန်းပြည့် i;
  var_na *ptr = buffer;

  နေစဉ်(st->bufbeg < st->bufend) {
    /* Line full ? */
    အကယ်၍(st->pos > MAX_ENCODED_LINE_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      အကယ်၍(size < 2) {
        အကယ်၍(!cursize)
          ပြန်ပေး STOP_FILLING;
        bha_nga;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    အကယ်၍(size < 4) {
      အကယ်၍(!cursize)
        ပြန်ပေး STOP_FILLING;
      bha_nga;
    }
    အကယ်၍(st->bufend - st->bufbeg < 3)
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
  အကယ်၍(ateof) {
    အကယ်၍(size < 4) {
      အကယ်၍(!cursize)
        ပြန်ပေး STOP_FILLING;
    }
    မဟုတ်ပါက {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      အကယ်၍(st->bufend != st->bufbeg) {

        အကယ်၍(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        အကယ်၍(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  ပြန်ပေး cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_INPUT_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_LINE_LENGTH + 2)) *                  \
    MAX_ENCODED_LINE_LENGTH / 4) * 3 - 3)

static curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  အကယ်၍(size <= 0)
    ပြန်ပေး size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  အကယ်၍(size > BASE64_MAX_INPUT_SIZE)
    ပြန်ပေး -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  ပြန်ပေး size + (2 * ((size - 1) / MAX_ENCODED_LINE_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
static ကိန်းပြည့် qp_lookahead_eol(sa_nracanaa mime_encoder_state *st, ကိန်းပြည့် ateof, size_t n)
{
  n += st->bufbeg;
  အကယ်၍(n >= st->bufend && ateof)
    ပြန်ပေး 1;
  အကယ်၍(n + 2 > st->bufend)
    ပြန်ပေး ateof ? 0 : -1;
  အကယ်၍(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    ပြန်ပေး 1;
  ပြန်ပေး 0;
}

/* Quoted-printable encoder. */
static size_t encoder_qp_read(var_na *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  var_na *ptr = buffer;
  size_t cursize = 0;
  ကိန်းပြည့် softlinebreak;
  var_na buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  နေစဉ်(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    ကိန်းပြည့် i = (unsigned var_na)st->buf[st->bufbeg];
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
        ပြန်ပေး cursize;
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
        ပြန်ပေး cursize;
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
    အကယ်၍(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_LINE_LENGTH;
      အကယ်၍(!softlinebreak && st->pos + len == MAX_ENCODED_LINE_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        switch(qp_lookahead_eol(st, ateof, consumed)) {
        pak_sa -1:        /* Need more data. */
          ပြန်ပေး cursize;
        pak_sa 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          bha_nga;
        }
      }
      အကယ်၍(softlinebreak) {
        curlx_strcopy(buf, parimaa_na(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    အကယ်၍(len > size) {
      အကယ်၍(!cursize)
        ပြန်ပေး STOP_FILLING;
      bha_nga;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    အကယ်၍(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  ပြန်ပေး cursize;
}

static curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  ပြန်ပေး part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
static size_t mime_mem_read(var_na *buffer, size_t size, size_t nitems,
                            void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (void)size;  /* Always 1 */

  အကယ်၍(!nitems)
    ပြန်ပေး STOP_FILLING;

  အကယ်၍(sz > nitems)
    sz = nitems;

  အကယ်၍(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  ပြန်ပေး sz;
}

static ကိန်းပြည့် mime_mem_seek(void *instream, curl_off_t offset, ကိန်းပြည့် whence)
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

  အကယ်၍(offset < 0 || offset > part->datasize)
    ပြန်ပေး CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  ပြန်ပေး CURL_SEEKFUNC_OK;
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

  အကယ်၍(part->fp)
    ပြန်ပေး FALSE;
  part->fp = fopen_read(part->data, "rb");
  ပြန်ပေး part->fp ? FALSE : TRUE;
}

static size_t mime_file_read(var_na *buffer, size_t size, size_t nitems,
                             void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  အကယ်၍(!nitems)
    ပြန်ပေး STOP_FILLING;

  အကယ်၍(mime_open_file(part))
    ပြန်ပေး READ_ERROR;

  ပြန်ပေး fread(buffer, size, nitems, part->fp);
}

static ကိန်းပြည့် mime_file_seek(void *instream, curl_off_t offset, ကိန်းပြည့် whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  အကယ်၍(whence == SEEK_SET && !offset && !part->fp)
    ပြန်ပေး CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  အကယ်၍(mime_open_file(part))
    ပြန်ပေး CURL_SEEKFUNC_FAIL;

  ပြန်ပေး curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

static void mime_file_free(void *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  အကယ်၍(part->fp) {
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

  အကယ်၍(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  မဟုတ်ပါက {
    sz = offset - numbytes;
    အကယ်၍(sz >= traillen)
      ပြန်ပေး 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  အကယ်၍(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  ပြန်ပေး sz;
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
    ပြန်ပေး part->lastreadstatus;
  default:
    bha_nga;
  }

  /* If we can determine we are at end of part data, spare a read. */
  အကယ်၍(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  မဟုတ်ပါက {
    switch(part->kind) {
    pak_sa MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      bha_nga;
    pak_sa MIMEKIND_FILE:
      အကယ်၍(part->fp && feof(part->fp))
        bha_nga;  /* At EOF. */
      FALLTHROUGH();
    default:
      အကယ်၍(part->readfunc) {
        အကယ်၍(!(part->flags & MIME_FAST_READ)) {
          အကယ်၍(*hasread)
            ပြန်ပေး STOP_FILLING;
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

  ပြန်ပေး sz;
}

/* Read and encode part content. */
static size_t read_encoded_part_content(curl_mimepart *part, var_na *buffer,
                                        size_t bufsize, bool *hasread)
{
  sa_nracanaa mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  အတွက်(;;) {
    အကယ်၍(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      switch(sz) {
      pak_sa 0:
        အကယ်၍(ateof)
          ပြန်ပေး cursize;
        bha_nga;
      pak_sa READ_ERROR:
      pak_sa STOP_FILLING:
        ပြန်ပေး cursize ? cursize : sz;
      default:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        continue;
      }
    }

    /* We need more data in input buffer. */
    အကယ်၍(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      အကယ်၍(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    အကယ်၍(st->bufend >= parimaa_na(st->buf))
      ပြန်ပေး cursize ? cursize : READ_ERROR;    /* Buffer full. */
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
      ပြန်ပေး cursize ? cursize : sz;
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
  နေစဉ်(bufsize) {
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
      အကယ်၍(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, NULL);
        bha_nga;
      }
      အကယ်၍(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        bha_nga;
      }
      FALLTHROUGH();
    pak_sa MIMESTATE_CURLHEADERS:
      အကယ်၍(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      မဟုတ်ပါက {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        အကယ်၍(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      bha_nga;
    pak_sa MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      အကယ်၍(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, NULL);
      bha_nga;
    pak_sa MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, NULL);
      bha_nga;
    pak_sa MIMESTATE_CONTENT:
      အကယ်၍(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      မဟုတ်ပါက
        sz = read_part_content(part, buffer, bufsize, hasread);
      switch(sz) {
      pak_sa 0:
        mimesetstate(&part->state, MIMESTATE_END, NULL);
        /* Try sparing open file descriptors. */
        အကယ်၍(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = NULL;
        }
        FALLTHROUGH();
      pak_sa CURL_READFUNC_ABORT:
      pak_sa CURL_READFUNC_PAUSE:
      pak_sa READ_ERROR:
      pak_sa STOP_FILLING:
        ပြန်ပေး cursize ? cursize : sz;
      }
      bha_nga;
    pak_sa MIMESTATE_END:
      ပြန်ပေး cursize;
    default:
      bha_nga;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  ပြန်ပေး cursize;
}

/* Readback from mime. Warning: not a read callback function. */
static size_t mime_subparts_read(var_na *buffer, size_t size, size_t nitems,
                                 void *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (void)size;  /* Always 1 */

  နေစဉ်(nitems) {
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
      အကယ်၍(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      bha_nga;
    pak_sa MIMESTATE_BOUNDARY2:
      အကယ်၍(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      မဟုတ်ပါက
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      အကယ်၍(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      bha_nga;
    pak_sa MIMESTATE_CONTENT:
      အကယ်၍(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, NULL);
        bha_nga;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      switch(sz) {
      pak_sa CURL_READFUNC_ABORT:
      pak_sa CURL_READFUNC_PAUSE:
      pak_sa READ_ERROR:
      pak_sa STOP_FILLING:
        ပြန်ပေး cursize ? cursize : sz;
      pak_sa 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        bha_nga;
      }
      bha_nga;
    pak_sa MIMESTATE_END:
      ပြန်ပေး cursize;
    default:
      bha_nga;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  ပြန်ပေး cursize;
}

static ကိန်းပြည့် mime_part_rewind(curl_mimepart *part)
{
  ကိန်းပြည့် res = CURL_SEEKFUNC_OK;
  ga_nanaa mimestate targetstate = MIMESTATE_BEGIN;

  အကယ်၍(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  အကယ်၍(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    အကယ်၍(part->seekfunc) {
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

  အကယ်၍(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, NULL);

  part->lastreadstatus = 1; /* Successful read status. */
  ပြန်ပေး res;
}

static ကိန်းပြည့် mime_subparts_seek(void *instream, curl_off_t offset, ကိန်းပြည့် whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  ကိန်းပြည့် rc = CURL_SEEKFUNC_OK;

  အကယ်၍(whence != SEEK_SET || offset)
    ပြန်ပေး CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  အကယ်၍(mime->state.state == MIMESTATE_BEGIN)
    ပြန်ပေး CURL_SEEKFUNC_OK;           /* Already rewound. */

  အတွက်(part = mime->firstpart; part; part = part->nextpart) {
    ကိန်းပြည့် res = mime_part_rewind(part);
    အကယ်၍(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  အကယ်၍(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, NULL);

  ပြန်ပေး rc;
}

/* Release part content. */
static void cleanup_part_content(curl_mimepart *part)
{
  အကယ်၍(part->freefunc)
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
  part->flags &= ~(unsigned ကိန်းပြည့်)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

static void mime_subparts_free(void *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  အကယ်၍(mime && mime->parent) {
    mime->parent->freefunc = NULL;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
static void mime_subparts_unbind(void *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  အကယ်၍(mime && mime->parent) {
    mime->parent->freefunc = NULL;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = NULL;
  }
}

void Curl_mime_cleanpart(curl_mimepart *part)
{
  အကယ်၍(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    အကယ်၍(part->flags & MIME_USERHEADERS_OWNER)
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

  အကယ်၍(!mime)
    ပြန်ပေး;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  နေစဉ်(mime) {
    part = mime->firstpart;
    အကယ်၍(part) {
      mime->firstpart = part->nextpart;
      အကယ်၍(part->kind == MIMEKIND_MULTIPART && part->arg &&
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
    မဟုတ်ပါက {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

static bool mime_is_too_deep(const curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  အကယ်၍(++call_depth > MAX_MIME_LEVELS)
    ပြန်ပေး TRUE;

  အကယ်၍(part->kind == MIMEKIND_MULTIPART && part->arg) {
    const curl_mime *mime = (const curl_mime *)part->arg;
    const curl_mimepart *s;

    အတွက်(s = mime->firstpart; s; s = s->nextpart) {
      အကယ်၍(mime_is_too_deep(s, call_depth))
        ပြန်ပေး TRUE;
    }
  }

  ပြန်ပေး FALSE;
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

  အကယ်၍(++call_depth > MAX_MIME_LEVELS)
    ပြန်ပေး CURLE_TOO_LARGE;

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
    အကယ်၍(result == CURLE_READ_ERROR)
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
    အတွက်(s = ((curl_mime *)src->arg)->firstpart; !result && s;
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
  အကယ်၍(!result && src->userheaders) {
    sa_nracanaa curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    အကယ်၍(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    မဟုတ်ပါက {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      အကယ်၍(result)
        curl_slist_free_all(hdrs);
    }
  }

  အကယ်၍(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  အကယ်၍(!result)
    result = curl_mime_name(dst, src->name);
  အကယ်၍(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  အကယ်၍(result)
    Curl_mime_cleanpart(dst);

  ပြန်ပေး result;
}

CURLcode Curl_mime_duppart(sa_nracanaa Curl_easy *data,
                           curl_mimepart *dst, const curl_mimepart *src)
{
  အကယ်၍(!dst || !src)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  အကယ်၍(mime_is_too_deep(src, 0))
    ပြန်ပေး CURLE_TOO_LARGE;

  ပြန်ပေး mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(void *easy)
{
  curl_mime *mime = curlx_malloc(parimaa_na(*mime));

  အကယ်၍(mime) {
    mime->parent = NULL;
    mime->firstpart = NULL;
    mime->lastpart = NULL;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    အကယ်၍(Curl_rand_alnum(easy,
                       (unsigned var_na *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      ပြန်ပေး NULL;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, NULL);
  }

  ပြန်ပေး mime;
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

  အကယ်၍(!mime)
    ပြန်ပေး NULL;

  part = curlx_malloc(parimaa_na(*part));

  အကယ်၍(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    အကယ်၍(mime->lastpart)
      mime->lastpart->nextpart = part;
    မဟုတ်ပါက
      mime->firstpart = part;

    mime->lastpart = part;
  }

  ပြန်ပေး part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, const var_na *name)
{
  အကယ်၍(!part)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  အကယ်၍(name) {
    part->name = curlx_strdup(name);
    အကယ်၍(!part->name)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
  }

  ပြန်ပေး CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, const var_na *filename)
{
  အကယ်၍(!part)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  အကယ်၍(filename) {
    part->filename = curlx_strdup(filename);
    အကယ်၍(!part->filename)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
  }

  ပြန်ပေး CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, const var_na *data, size_t datasize)
{
  အကယ်၍(!part)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  အကယ်၍(data) {
    အကယ်၍(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    အကယ်၍(!part->data)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  ပြန်ပေး CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, const var_na *filename)
{
  CURLcode result = CURLE_OK;

  အကယ်၍(!part)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  အကယ်၍(filename) {
    var_na *base;
    curlx_struct_stat sbuf;

    အကယ်၍(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    မဟုတ်ပါက {
      part->data = curlx_strdup(filename);
      အကယ်၍(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      မဟုတ်ပါက {
        part->datasize = -1;
        အကယ်၍(S_ISREG(sbuf.st_mode)) {
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
        အကယ်၍(!base)
          result = CURLE_OUT_OF_MEMORY;
        မဟုတ်ပါက {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  ပြန်ပေး result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, const var_na *mimetype)
{
  အကယ်၍(!part)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  အကယ်၍(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    အကယ်၍(!part->mimetype)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
  }

  ပြန်ပေး CURLE_OK;
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

  အကယ်၍(!part)
    ပြန်ပေး result;

  part->encoder = NULL;

  အကယ်၍(!encoding)
    ပြန်ပေး CURLE_OK;    /* Removing current encoder. */

  အတွက်(mep = encoders; mep->name; mep++)
    အကယ်၍(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  ပြန်ပေး result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           sa_nracanaa curl_slist *headers, ကိန်းပြည့် take_ownership)
{
  အကယ်၍(!part)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  အကယ်၍(part->flags & MIME_USERHEADERS_OWNER) {
    အကယ်၍(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned ကိန်းပြည့်)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  အကယ်၍(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  ပြန်ပေး CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, void *arg)
{
  အကယ်၍(!part)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  အကယ်၍(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  ပြန်ပေး CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, ကိန်းပြည့် take_ownership)
{
  curl_mime *root;

  အကယ်၍(!part)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  အကယ်၍(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    ပြန်ပေး CURLE_OK;

  cleanup_part_content(part);

  အကယ်၍(subparts) {
    /* Should not have been attached already. */
    အကယ်၍(subparts->parent)
      ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    root = part->parent;
    အကယ်၍(root) {
      နေစဉ်(root->parent && root->parent->parent)
        root = root->parent->parent;
      အကယ်၍(subparts == root) {
        /* cannot add as a subpart of itself. */
        ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    အကယ်၍(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      ပြန်ပေး CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  ပြန်ပေး CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  ပြန်ပေး Curl_mime_set_subparts(part, subparts, TRUE);
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
  } နေစဉ်(ret == STOP_FILLING);

  ပြန်ပေး ret;
}

/* Rewind mime stream. */
static CURLcode mime_rewind(curl_mimepart *part)
{
  ပြန်ပေး mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
static size_t slist_size(sa_nracanaa curl_slist *s,
                         size_t overhead, const var_na *skip, size_t skiplen)
{
  size_t size = 0;

  အတွက်(; s; s = s->next)
    အကယ်၍(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  ပြန်ပေး size;
}

/* Get/compute multipart size. */
static curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  အကယ်၍(!mime)
    ပြန်ပေး 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  အတွက်(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    အကယ်၍(sz < 0)
      size = sz;

    အကယ်၍(size >= 0)
      size += boundarysize + sz;
  }

  ပြန်ပေး size;
}

/* Get/compute mime size. */
static curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  အကယ်၍(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  အကယ်၍(part->encoder)
    size = part->encoder->sizefunc(part);

  အကယ်၍(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, NULL, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  ပြန်ပေး size;
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

  အကယ်၍(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    အကယ်၍(hdr)
      *slp = hdr;
    မဟုတ်ပါက
      curlx_free(s);
  }

  ပြန်ပေး hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
static CURLcode add_content_type(sa_nracanaa curl_slist **slp,
                                 const var_na *type, const var_na *boundary)
{
  ပြန်ပေး Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
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

  အကယ်၍(filename) {
    size_t len1 = strlen(filename);
    const var_na *nameend = filename + len1;
    unsigned ကိန်းပြည့် i;

    အတွက်(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      အကယ်၍(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        ပြန်ပေး ctts[i].type;
    }
  }
  ပြန်ပေး NULL;
}

static bool content_type_match(const var_na *contenttype,
                               const var_na *target, size_t len)
{
  အကယ်၍(contenttype && curl_strnequal(contenttype, target, len))
    switch(contenttype[len]) {
    pak_sa '\0':
    pak_sa '\t':
    pak_sa '\r':
    pak_sa '\n':
    pak_sa ' ':
    pak_sa ';':
      ပြန်ပေး TRUE;
    }
  ပြန်ပေး FALSE;
}

static CURLcode add_content_disposition(sa_nracanaa Curl_easy *data,
                                        curl_mimepart *part,
                                        const var_na *disposition,
                                        const var_na *contenttype,
                                        ga_nanaa mimestrategy strategy)
{
  အကယ်၍(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  အကယ်၍(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = NULL;

  အကယ်၍(disposition) {
    CURLcode result = CURLE_OK;
    var_na *name = NULL;
    var_na *filename = NULL;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    အကယ်၍(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

    အကယ်၍(part->name) {
      name = escape_string(data, part->name, strategy);
      အကယ်၍(!name)
        ပြန်ပေး CURLE_OUT_OF_MEMORY;
    }
    အကယ်၍(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      အကယ်၍(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    အကယ်၍(!result)
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
    အကယ်၍(result)
      ပြန်ပေး result;
  }
  ပြန်ပေး CURLE_OK;
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

  အကယ်၍(++call_depth > MAX_MIME_LEVELS)
    ပြန်ပေး CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = NULL;

  /* Be sure we will not access old headers later. */
  အကယ်၍(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, NULL);

  /* Check if content type is specified. */
  customct = part->mimetype;
  အကယ်၍(!customct)
    customct = search_header(part->userheaders, STRCONST("Content-Type"));
  အကယ်၍(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  အကယ်၍(!contenttype) {
    switch(part->kind) {
    pak_sa MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      bha_nga;
    pak_sa MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      အကယ်၍(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      အကယ်၍(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      bha_nga;
    default:
      contenttype = Curl_mime_contenttype(part->filename);
      bha_nga;
    }
  }

  အကယ်၍(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    အကယ်၍(mime)
      boundary = mime->boundary;
  }
  မဟုတ်ပါက အကယ်၍(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = NULL;

  /* Issue content-disposition header only if not already set by caller. */
  အကယ်၍(!search_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    အကယ်၍(result)
      ပြန်ပေး result;
  }

  /* Issue Content-Type header. */
  အကယ်၍(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    အကယ်၍(result)
      ပြန်ပေး result;
  }

  /* Content-Transfer-Encoding header. */
  အကယ်၍(!search_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    အကယ်၍(part->encoder)
      cte = part->encoder->name;
    မဟုတ်ပါက အကယ်၍(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    အကယ်၍(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      အကယ်၍(result)
        ပြန်ပေး result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  အကယ်၍(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  အကယ်၍(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = NULL;
    အကယ်၍(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    အတွက်(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, NULL,
                                    disposition, strategy, call_depth);
      အကယ်၍(result)
        ပြန်ပေး result;
    }
  }
  ပြန်ပေး result;
}

CURLcode Curl_mime_prepare_headers(sa_nracanaa Curl_easy *data,
                                   curl_mimepart *part,
                                   const var_na *contenttype,
                                   const var_na *disposition,
                                   ga_nanaa mimestrategy strategy)
{
  ပြန်ပေး mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
static void mime_unpause(curl_mimepart *part)
{
  အကယ်၍(part) {
    အကယ်၍(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    အကယ်၍(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      အကယ်၍(mime) {
        curl_mimepart *subpart;

        အတွက်(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
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
  ပြန်ပေး CURLE_OK;
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
  အကယ်၍(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (ကိန်းပြည့်)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    ပြန်ပေး ctx->error_result;
  }
  အကယ်၍(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    ပြန်ပေး CURLE_OK;
  }
  /* respect length limitations */
  အကယ်၍(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    အကယ်၍(remain <= 0)
      blen = 0;
    မဟုတ်ပါက အကယ်၍(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  အကယ်၍(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned var_na *)buf, blen, &nread);
    အကယ်၍(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      ပြန်ပေး result;
    }
  }
  မဟုတ်ပါက အကယ်၍(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, parimaa_na(tmp), ctx->part);
    အကယ်၍(nread <= parimaa_na(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned var_na *)tmp, nread, &n);
      အကယ်၍(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ပြန်ပေး result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      အကယ်၍(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ပြန်ပေး result;
      }
    }
  }
  မဟုတ်ပါက
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  switch(nread) {
  pak_sa 0:
    အကယ်၍((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      ပြန်ပေး CURLE_READ_ERROR;
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
    အကယ်၍(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      ပြန်ပေး CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    အကယ်၍(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    bha_nga;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (ကိန်းပြည့်)result, *pnread, *peos);
  ပြန်ပေး result;
}

static bool cr_mime_needs_rewind(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  ပြန်ပေး ctx->read_len > 0;
}

static curl_off_t cr_mime_total_length(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  ပြန်ပေး ctx->total_len;
}

static CURLcode cr_mime_resume_from(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa Curl_creader *reader,
                                    curl_off_t offset)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;

  အကယ်၍(offset > 0) {
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
      အကယ်၍((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        ပြန်ပေး CURLE_READ_ERROR;
      }
    } နေစဉ်(passed < offset);

    /* now, decrease the size of the read */
    အကယ်၍(ctx->total_len > 0) {
      ctx->total_len -= offset;

      အကယ်၍(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        ပြန်ပေး CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  ပြန်ပေး CURLE_OK;
}

static CURLcode cr_mime_cntrl(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  switch(opcode) {
  pak_sa CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    အကယ်၍(result)
      failf(data, "Cannot rewind mime/post data");
    ပြန်ပေး result;
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
  ပြန်ပေး CURLE_OK;
}

static bool cr_mime_is_paused(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_creader *reader)
{
  sa_nracanaa cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  ပြန်ပေး ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
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
  အကယ်၍(result)
    ပြန်ပေး result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  အကယ်၍(result) {
    Curl_creader_free(data, r);
    ပြန်ပေး result;
  }
  ctx->total_len = mime_size(ctx->part);

  ပြန်ပေး Curl_creader_set(data, r);
}

#မဟုတ်ပါက /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (void)easy;
  ပြန်ပေး NULL;
}

void curl_mime_free(curl_mime *mime)
{
  (void)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (void)mime;
  ပြန်ပေး NULL;
}

CURLcode curl_mime_name(curl_mimepart *part, const var_na *name)
{
  (void)part;
  (void)name;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, const var_na *filename)
{
  (void)part;
  (void)filename;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, const var_na *mimetype)
{
  (void)part;
  (void)mimetype;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, const var_na *encoding)
{
  (void)part;
  (void)encoding;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, const var_na *data, size_t datasize)
{
  (void)part;
  (void)data;
  (void)datasize;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, const var_na *filename)
{
  (void)part;
  (void)filename;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
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
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (void)part;
  (void)subparts;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           sa_nracanaa curl_slist *headers, ကိန်းပြည့် take_ownership)
{
  (void)part;
  (void)headers;
  (void)take_ownership;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(sa_nracanaa curl_slist **slp, const var_na *fmt, ...)
{
  (void)slp;
  (void)fmt;
  ပြန်ပေး CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

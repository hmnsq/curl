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

struct Curl_easy;

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

#nếu !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#include "rand.h"
#include "slist.h"
#include "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

static size_t mime_subparts_read(char *buffer, size_t size, size_t nitems,
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
static const unsigned char qp_class[] = {
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
static const char aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#không_thì

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
curl_off_t VmsRealFileSize(const char *name, const curlx_struct_stat *stat_buf)
{
  char buffer[8192];
  curl_off_t count;
  số_nguyên ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  nếu(!file)
    trả_về 0;

  count = 0;
  ret_stat = 1;
  trong_khi(ret_stat > 0) {
    ret_stat = fread(buffer, 1, sizeof(buffer), file);
    nếu(ret_stat)
      count += ret_stat;
  }
  curlx_fclose(file);

  trả_về count;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
static curl_off_t VmsSpecialSize(const char *name,
                                 const curlx_struct_stat *stat_buf)
{
  switch(stat_buf->st_fab_rfm) {
  case FAB$C_VAR:
  case FAB$C_VFC:
    trả_về VmsRealFileSize(name, stat_buf);
    break;
  default:
    trả_về stat_buf->st_size;
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
static FILE *vmsfopenread(const char *file, const char *mode)
{
  curlx_struct_stat statbuf;
  số_nguyên res = curlx_stat(file, &statbuf);

  nếu(res != -1) {
    switch(statbuf.st_fab_rfm) {
    case FAB$C_VAR:
    case FAB$C_VFC:
    case FAB$C_STMCR:
      trả_về curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      break;
    default:
      trả_về curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  trả_về NULL;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
static void mimesetstate(struct mime_state *state,
                         enum mimestate tok, void *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
static char *escape_string(struct Curl_easy *data,
                           const char *src, enum mimestrategy strategy)
{
  CURLcode result;
  struct dynbuf db;
  const char * const *table;
  const char * const *p;
  /* replace first character by rest of string. */
  static const char * const mimetable[] = {
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
  static const char * const formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    NULL
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  nếu(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_INPUT_LENGTH);

  cho(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    cho(p = table; *p && **p != *src; p++)
      ;

    nếu(*p)
      result = curlx_dyn_add(&db, *p + 1);
    không_thì
      result = curlx_dyn_addn(&db, src, 1);
  }

  trả_về curlx_dyn_ptr(&db);
}

/* Check if header matches. */
static char *match_header(struct curl_slist *hdr, const char *lbl, size_t len)
{
  char *value = NULL;

  nếu(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    cho(value = hdr->data + len + 1; *value == ' '; value++)
      ;
  trả_về value;
}

/* Get a header from an slist. */
static char *search_header(struct curl_slist *hdrlist,
                           const char *hdr, size_t len)
{
  char *value = NULL;

  cho(; !value && hdrlist; hdrlist = hdrlist->next)
    value = match_header(hdrlist, hdr, len);

  trả_về value;
}

static char *strippath(const char *fullfile)
{
  char *filename;
  char *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  nếu(!filename)
    trả_về NULL;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  trả_về base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
static void cleanup_encoder_state(struct mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
static size_t encoder_nop_read(char *buffer, size_t size, bool ateof,
                               struct curl_mimepart *part)
{
  struct mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (void)ateof;

  nếu(!size)
    trả_về STOP_FILLING;

  nếu(size > insize)
    size = insize;

  nếu(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  trả_về size;
}

static curl_off_t encoder_nop_size(curl_mimepart *part)
{
  trả_về part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
static size_t encoder_7bit_read(char *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  struct mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (void)ateof;

  nếu(!size)
    trả_về STOP_FILLING;

  nếu(size > cursize)
    size = cursize;

  cho(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    nếu(*buffer++ & 0x80)
      trả_về cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  trả_về cursize;
}

/* Base64 content encoder. */
static size_t encoder_base64_read(char *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  struct mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  số_nguyên i;
  char *ptr = buffer;

  trong_khi(st->bufbeg < st->bufend) {
    /* Line full ? */
    nếu(st->pos > MAX_ENCODED_LINE_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      nếu(size < 2) {
        nếu(!cursize)
          trả_về STOP_FILLING;
        break;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    nếu(size < 4) {
      nếu(!cursize)
        trả_về STOP_FILLING;
      break;
    }
    nếu(st->bufend - st->bufbeg < 3)
      break;

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
  nếu(ateof) {
    nếu(size < 4) {
      nếu(!cursize)
        trả_về STOP_FILLING;
    }
    không_thì {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      nếu(st->bufend != st->bufbeg) {

        nếu(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        nếu(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  trả_về cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_INPUT_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_LINE_LENGTH + 2)) *                  \
    MAX_ENCODED_LINE_LENGTH / 4) * 3 - 3)

static curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  nếu(size <= 0)
    trả_về size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  nếu(size > BASE64_MAX_INPUT_SIZE)
    trả_về -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  trả_về size + (2 * ((size - 1) / MAX_ENCODED_LINE_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
static số_nguyên qp_lookahead_eol(struct mime_encoder_state *st, số_nguyên ateof, size_t n)
{
  n += st->bufbeg;
  nếu(n >= st->bufend && ateof)
    trả_về 1;
  nếu(n + 2 > st->bufend)
    trả_về ateof ? 0 : -1;
  nếu(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    trả_về 1;
  trả_về 0;
}

/* Quoted-printable encoder. */
static size_t encoder_qp_read(char *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  struct mime_encoder_state *st = &part->encstate;
  char *ptr = buffer;
  size_t cursize = 0;
  số_nguyên softlinebreak;
  char buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  trong_khi(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    số_nguyên i = (unsigned char)st->buf[st->bufbeg];
    buf[0] = (char)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    switch(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    case QP_OK:          /* Not a special character. */
      break;
    case QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      switch(qp_lookahead_eol(st, ateof, 1)) {
      case -1:          /* More input data needed. */
        trả_về cursize;
      case 0:           /* No encoding needed. */
        break;
      default:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        break;
      }
      break;
    case QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      switch(qp_lookahead_eol(st, ateof, 0)) {
      case -1:          /* Need more data. */
        trả_về cursize;
      case 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        break;
      default:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        break;
      }
      break;
    default:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      break;
    }

    /* Be sure the encoded character fits within maximum line length. */
    nếu(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_LINE_LENGTH;
      nếu(!softlinebreak && st->pos + len == MAX_ENCODED_LINE_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        switch(qp_lookahead_eol(st, ateof, consumed)) {
        case -1:        /* Need more data. */
          trả_về cursize;
        case 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          break;
        }
      }
      nếu(softlinebreak) {
        curlx_strcopy(buf, sizeof(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    nếu(len > size) {
      nếu(!cursize)
        trả_về STOP_FILLING;
      break;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    nếu(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  trả_về cursize;
}

static curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  trả_về part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
static size_t mime_mem_read(char *buffer, size_t size, size_t nitems,
                            void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (void)size;  /* Always 1 */

  nếu(!nitems)
    trả_về STOP_FILLING;

  nếu(sz > nitems)
    sz = nitems;

  nếu(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  trả_về sz;
}

static số_nguyên mime_mem_seek(void *instream, curl_off_t offset, số_nguyên whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  switch(whence) {
  case SEEK_CUR:
    offset += part->state.offset;
    break;
  case SEEK_END:
    offset += part->datasize;
    break;
  }

  nếu(offset < 0 || offset > part->datasize)
    trả_về CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  trả_về CURL_SEEKFUNC_OK;
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

  nếu(part->fp)
    trả_về FALSE;
  part->fp = fopen_read(part->data, "rb");
  trả_về part->fp ? FALSE : TRUE;
}

static size_t mime_file_read(char *buffer, size_t size, size_t nitems,
                             void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  nếu(!nitems)
    trả_về STOP_FILLING;

  nếu(mime_open_file(part))
    trả_về READ_ERROR;

  trả_về fread(buffer, size, nitems, part->fp);
}

static số_nguyên mime_file_seek(void *instream, curl_off_t offset, số_nguyên whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  nếu(whence == SEEK_SET && !offset && !part->fp)
    trả_về CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  nếu(mime_open_file(part))
    trả_về CURL_SEEKFUNC_FAIL;

  trả_về curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

static void mime_file_free(void *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  nếu(part->fp) {
    curlx_fclose(part->fp);
    part->fp = NULL;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
static size_t readback_bytes(struct mime_state *state,
                             char *buffer, size_t bufsize,
                             const char *bytes, size_t numbytes,
                             const char *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  nếu(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  không_thì {
    sz = offset - numbytes;
    nếu(sz >= traillen)
      trả_về 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  nếu(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  trả_về sz;
}

/* Read a non-encoded part content. */

static size_t read_part_content(curl_mimepart *part, char *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  switch(part->lastreadstatus) {
  case 0:
  case CURL_READFUNC_ABORT:
  case CURL_READFUNC_PAUSE:
  case READ_ERROR:
    trả_về part->lastreadstatus;
  default:
    break;
  }

  /* If we can determine we are at end of part data, spare a read. */
  nếu(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  không_thì {
    switch(part->kind) {
    case MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      break;
    case MIMEKIND_FILE:
      nếu(part->fp && feof(part->fp))
        break;  /* At EOF. */
      FALLTHROUGH();
    default:
      nếu(part->readfunc) {
        nếu(!(part->flags & MIME_FAST_READ)) {
          nếu(*hasread)
            trả_về STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      break;
    }
  }

  switch(sz) {
  case STOP_FILLING:
    break;
  case 0:
  case CURL_READFUNC_ABORT:
  case CURL_READFUNC_PAUSE:
  case READ_ERROR:
    part->lastreadstatus = sz;
    break;
  default:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    break;
  }

  trả_về sz;
}

/* Read and encode part content. */
static size_t read_encoded_part_content(curl_mimepart *part, char *buffer,
                                        size_t bufsize, bool *hasread)
{
  struct mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  cho(;;) {
    nếu(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      switch(sz) {
      case 0:
        nếu(ateof)
          trả_về cursize;
        break;
      case READ_ERROR:
      case STOP_FILLING:
        trả_về cursize ? cursize : sz;
      default:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        continue;
      }
    }

    /* We need more data in input buffer. */
    nếu(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      nếu(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    nếu(st->bufend >= sizeof(st->buf))
      trả_về cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           sizeof(st->buf) - st->bufend, hasread);
    switch(sz) {
    case 0:
      ateof = TRUE;
      break;
    case CURL_READFUNC_ABORT:
    case CURL_READFUNC_PAUSE:
    case READ_ERROR:
    case STOP_FILLING:
      trả_về cursize ? cursize : sz;
    default:
      st->bufend += sz;
      break;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
static size_t readback_part(curl_mimepart *part,
                            char *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  trong_khi(bufsize) {
    size_t sz = 0;
    struct curl_slist *hdr = (struct curl_slist *)part->state.ptr;
    switch(part->state.state) {
    case MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      break;
    case MIMESTATE_USERHEADERS:
      nếu(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, NULL);
        break;
      }
      nếu(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        break;
      }
      FALLTHROUGH();
    case MIMESTATE_CURLHEADERS:
      nếu(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      không_thì {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        nếu(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      break;
    case MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      nếu(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, NULL);
      break;
    case MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, NULL);
      break;
    case MIMESTATE_CONTENT:
      nếu(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      không_thì
        sz = read_part_content(part, buffer, bufsize, hasread);
      switch(sz) {
      case 0:
        mimesetstate(&part->state, MIMESTATE_END, NULL);
        /* Try sparing open file descriptors. */
        nếu(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = NULL;
        }
        FALLTHROUGH();
      case CURL_READFUNC_ABORT:
      case CURL_READFUNC_PAUSE:
      case READ_ERROR:
      case STOP_FILLING:
        trả_về cursize ? cursize : sz;
      }
      break;
    case MIMESTATE_END:
      trả_về cursize;
    default:
      break;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  trả_về cursize;
}

/* Readback from mime. Warning: not a read callback function. */
static size_t mime_subparts_read(char *buffer, size_t size, size_t nitems,
                                 void *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (void)size;  /* Always 1 */

  trong_khi(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    switch(mime->state.state) {
    case MIMESTATE_BEGIN:
    case MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      break;
    case MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      nếu(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      break;
    case MIMESTATE_BOUNDARY2:
      nếu(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      không_thì
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      nếu(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      break;
    case MIMESTATE_CONTENT:
      nếu(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, NULL);
        break;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      switch(sz) {
      case CURL_READFUNC_ABORT:
      case CURL_READFUNC_PAUSE:
      case READ_ERROR:
      case STOP_FILLING:
        trả_về cursize ? cursize : sz;
      case 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        break;
      }
      break;
    case MIMESTATE_END:
      trả_về cursize;
    default:
      break;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  trả_về cursize;
}

static số_nguyên mime_part_rewind(curl_mimepart *part)
{
  số_nguyên res = CURL_SEEKFUNC_OK;
  enum mimestate targetstate = MIMESTATE_BEGIN;

  nếu(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  nếu(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    nếu(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      switch(res) {
      case CURL_SEEKFUNC_OK:
      case CURL_SEEKFUNC_FAIL:
      case CURL_SEEKFUNC_CANTSEEK:
        break;
      case -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        break;
      default:
        res = CURL_SEEKFUNC_FAIL;
        break;
      }
    }
  }

  nếu(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, NULL);

  part->lastreadstatus = 1; /* Successful read status. */
  trả_về res;
}

static số_nguyên mime_subparts_seek(void *instream, curl_off_t offset, số_nguyên whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  số_nguyên rc = CURL_SEEKFUNC_OK;

  nếu(whence != SEEK_SET || offset)
    trả_về CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  nếu(mime->state.state == MIMESTATE_BEGIN)
    trả_về CURL_SEEKFUNC_OK;           /* Already rewound. */

  cho(part = mime->firstpart; part; part = part->nextpart) {
    số_nguyên res = mime_part_rewind(part);
    nếu(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  nếu(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, NULL);

  trả_về rc;
}

/* Release part content. */
static void cleanup_part_content(curl_mimepart *part)
{
  nếu(part->freefunc)
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
  part->flags &= ~(unsigned số_nguyên)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

static void mime_subparts_free(void *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  nếu(mime && mime->parent) {
    mime->parent->freefunc = NULL;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
static void mime_subparts_unbind(void *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  nếu(mime && mime->parent) {
    mime->parent->freefunc = NULL;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = NULL;
  }
}

void Curl_mime_cleanpart(curl_mimepart *part)
{
  nếu(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    nếu(part->flags & MIME_USERHEADERS_OWNER)
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

  nếu(!mime)
    trả_về;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  trong_khi(mime) {
    part = mime->firstpart;
    nếu(part) {
      mime->firstpart = part->nextpart;
      nếu(part->kind == MIMEKIND_MULTIPART && part->arg &&
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
    không_thì {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

static bool mime_is_too_deep(const curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  nếu(++call_depth > MAX_MIME_LEVELS)
    trả_về TRUE;

  nếu(part->kind == MIMEKIND_MULTIPART && part->arg) {
    const curl_mime *mime = (const curl_mime *)part->arg;
    const curl_mimepart *s;

    cho(s = mime->firstpart; s; s = s->nextpart) {
      nếu(mime_is_too_deep(s, call_depth))
        trả_về TRUE;
    }
  }

  trả_về FALSE;
}

static CURLcode mime_duppart(struct Curl_easy *data,
                             curl_mimepart *dst, const curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  const curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  nếu(++call_depth > MAX_MIME_LEVELS)
    trả_về CURLE_TOO_LARGE;

  /* Duplicate content. */
  switch(src->kind) {
  case MIMEKIND_NONE:
    break;
  case MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    break;
  case MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    nếu(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    break;
  case MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    break;
  case MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    cho(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    break;
  default:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    break;
  }

  /* Duplicate headers. */
  nếu(!result && src->userheaders) {
    struct curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    nếu(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    không_thì {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      nếu(result)
        curl_slist_free_all(hdrs);
    }
  }

  nếu(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  nếu(!result)
    result = curl_mime_name(dst, src->name);
  nếu(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  nếu(result)
    Curl_mime_cleanpart(dst);

  trả_về result;
}

CURLcode Curl_mime_duppart(struct Curl_easy *data,
                           curl_mimepart *dst, const curl_mimepart *src)
{
  nếu(!dst || !src)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  nếu(mime_is_too_deep(src, 0))
    trả_về CURLE_TOO_LARGE;

  trả_về mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(void *easy)
{
  curl_mime *mime = curlx_malloc(sizeof(*mime));

  nếu(mime) {
    mime->parent = NULL;
    mime->firstpart = NULL;
    mime->lastpart = NULL;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    nếu(Curl_rand_alnum(easy,
                       (unsigned char *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      trả_về NULL;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, NULL);
  }

  trả_về mime;
}

/* Initialize a mime part. */
void Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, sizeof(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, NULL);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  nếu(!mime)
    trả_về NULL;

  part = curlx_malloc(sizeof(*part));

  nếu(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    nếu(mime->lastpart)
      mime->lastpart->nextpart = part;
    không_thì
      mime->firstpart = part;

    mime->lastpart = part;
  }

  trả_về part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, const char *name)
{
  nếu(!part)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  nếu(name) {
    part->name = curlx_strdup(name);
    nếu(!part->name)
      trả_về CURLE_OUT_OF_MEMORY;
  }

  trả_về CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, const char *filename)
{
  nếu(!part)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  nếu(filename) {
    part->filename = curlx_strdup(filename);
    nếu(!part->filename)
      trả_về CURLE_OUT_OF_MEMORY;
  }

  trả_về CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, const char *data, size_t datasize)
{
  nếu(!part)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  nếu(data) {
    nếu(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    nếu(!part->data)
      trả_về CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  trả_về CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, const char *filename)
{
  CURLcode result = CURLE_OK;

  nếu(!part)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  nếu(filename) {
    char *base;
    curlx_struct_stat sbuf;

    nếu(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    không_thì {
      part->data = curlx_strdup(filename);
      nếu(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      không_thì {
        part->datasize = -1;
        nếu(S_ISREG(sbuf.st_mode)) {
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
        nếu(!base)
          result = CURLE_OUT_OF_MEMORY;
        không_thì {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  trả_về result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, const char *mimetype)
{
  nếu(!part)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  nếu(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    nếu(!part->mimetype)
      trả_về CURLE_OUT_OF_MEMORY;
  }

  trả_về CURLE_OK;
}

static const struct mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, const char *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  const struct mime_encoder *mep;

  nếu(!part)
    trả_về result;

  part->encoder = NULL;

  nếu(!encoding)
    trả_về CURLE_OK;    /* Removing current encoder. */

  cho(mep = encoders; mep->name; mep++)
    nếu(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  trả_về result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           struct curl_slist *headers, số_nguyên take_ownership)
{
  nếu(!part)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  nếu(part->flags & MIME_USERHEADERS_OWNER) {
    nếu(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned số_nguyên)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  nếu(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  trả_về CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, void *arg)
{
  nếu(!part)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  nếu(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  trả_về CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, số_nguyên take_ownership)
{
  curl_mime *root;

  nếu(!part)
    trả_về CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  nếu(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    trả_về CURLE_OK;

  cleanup_part_content(part);

  nếu(subparts) {
    /* Should not have been attached already. */
    nếu(subparts->parent)
      trả_về CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    root = part->parent;
    nếu(root) {
      trong_khi(root->parent && root->parent->parent)
        root = root->parent->parent;
      nếu(subparts == root) {
        /* cannot add as a subpart of itself. */
        trả_về CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    nếu(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      trả_về CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  trả_về CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  trả_về Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(char *buffer, size_t size, size_t nitems, void *instream)
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
  } trong_khi(ret == STOP_FILLING);

  trả_về ret;
}

/* Rewind mime stream. */
static CURLcode mime_rewind(curl_mimepart *part)
{
  trả_về mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
static size_t slist_size(struct curl_slist *s,
                         size_t overhead, const char *skip, size_t skiplen)
{
  size_t size = 0;

  cho(; s; s = s->next)
    nếu(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  trả_về size;
}

/* Get/compute multipart size. */
static curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  nếu(!mime)
    trả_về 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  cho(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    nếu(sz < 0)
      size = sz;

    nếu(size >= 0)
      size += boundarysize + sz;
  }

  trả_về size;
}

/* Get/compute mime size. */
static curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  nếu(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  nếu(part->encoder)
    size = part->encoder->sizefunc(part);

  nếu(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, NULL, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  trả_về size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(struct curl_slist **slp, const char *fmt, ...)
{
  struct curl_slist *hdr = NULL;
  char *s = NULL;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  nếu(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    nếu(hdr)
      *slp = hdr;
    không_thì
      curlx_free(s);
  }

  trả_về hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
static CURLcode add_content_type(struct curl_slist **slp,
                                 const char *type, const char *boundary)
{
  trả_về Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

const char *Curl_mime_contenttype(const char *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  struct ContentType {
    const char *extension;
    const char *type;
  };
  static const struct ContentType ctts[] = {
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

  nếu(filename) {
    size_t len1 = strlen(filename);
    const char *nameend = filename + len1;
    unsigned số_nguyên i;

    cho(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      nếu(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        trả_về ctts[i].type;
    }
  }
  trả_về NULL;
}

static bool content_type_match(const char *contenttype,
                               const char *target, size_t len)
{
  nếu(contenttype && curl_strnequal(contenttype, target, len))
    switch(contenttype[len]) {
    case '\0':
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ';':
      trả_về TRUE;
    }
  trả_về FALSE;
}

static CURLcode add_content_disposition(struct Curl_easy *data,
                                        curl_mimepart *part,
                                        const char *disposition,
                                        const char *contenttype,
                                        enum mimestrategy strategy)
{
  nếu(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  nếu(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = NULL;

  nếu(disposition) {
    CURLcode result = CURLE_OK;
    char *name = NULL;
    char *filename = NULL;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    nếu(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      trả_về CURLE_BAD_FUNCTION_ARGUMENT;

    nếu(part->name) {
      name = escape_string(data, part->name, strategy);
      nếu(!name)
        trả_về CURLE_OUT_OF_MEMORY;
    }
    nếu(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      nếu(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    nếu(!result)
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
    nếu(result)
      trả_về result;
  }
  trả_về CURLE_OK;
}

static CURLcode mime_prepare_headers(struct Curl_easy *data,
                                     curl_mimepart *part,
                                     const char *contenttype,
                                     const char *disposition,
                                     enum mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = NULL;
  const char *boundary = NULL;
  char *customct;
  const char *cte = NULL;
  CURLcode result = CURLE_OK;

  nếu(++call_depth > MAX_MIME_LEVELS)
    trả_về CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = NULL;

  /* Be sure we will not access old headers later. */
  nếu(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, NULL);

  /* Check if content type is specified. */
  customct = part->mimetype;
  nếu(!customct)
    customct = search_header(part->userheaders, STRCONST("Content-Type"));
  nếu(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  nếu(!contenttype) {
    switch(part->kind) {
    case MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      break;
    case MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      nếu(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      nếu(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      break;
    default:
      contenttype = Curl_mime_contenttype(part->filename);
      break;
    }
  }

  nếu(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    nếu(mime)
      boundary = mime->boundary;
  }
  không_thì nếu(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = NULL;

  /* Issue content-disposition header only if not already set by caller. */
  nếu(!search_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    nếu(result)
      trả_về result;
  }

  /* Issue Content-Type header. */
  nếu(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    nếu(result)
      trả_về result;
  }

  /* Content-Transfer-Encoding header. */
  nếu(!search_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    nếu(part->encoder)
      cte = part->encoder->name;
    không_thì nếu(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    nếu(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      nếu(result)
        trả_về result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  nếu(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  nếu(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = NULL;
    nếu(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    cho(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, NULL,
                                    disposition, strategy, call_depth);
      nếu(result)
        trả_về result;
    }
  }
  trả_về result;
}

CURLcode Curl_mime_prepare_headers(struct Curl_easy *data,
                                   curl_mimepart *part,
                                   const char *contenttype,
                                   const char *disposition,
                                   enum mimestrategy strategy)
{
  trả_về mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
static void mime_unpause(curl_mimepart *part)
{
  nếu(part) {
    nếu(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    nếu(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      nếu(mime) {
        curl_mimepart *subpart;

        cho(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

struct cr_mime_ctx {
  struct Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  struct bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

static CURLcode cr_mime_init(struct Curl_easy *data,
                             struct Curl_creader *reader)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  trả_về CURLE_OK;
}

static void cr_mime_close(struct Curl_easy *data,
                          struct Curl_creader *reader)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
static CURLcode cr_mime_read(struct Curl_easy *data,
                             struct Curl_creader *reader,
                             char *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  char tmp[256];

  /* Once we have errored, we will return the same error forever */
  nếu(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (số_nguyên)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    trả_về ctx->error_result;
  }
  nếu(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    trả_về CURLE_OK;
  }
  /* respect length limitations */
  nếu(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    nếu(remain <= 0)
      blen = 0;
    không_thì nếu(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  nếu(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned char *)buf, blen, &nread);
    nếu(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      trả_về result;
    }
  }
  không_thì nếu(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, sizeof(tmp), ctx->part);
    nếu(nread <= sizeof(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned char *)tmp, nread, &n);
      nếu(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        trả_về result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      nếu(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        trả_về result;
      }
    }
  }
  không_thì
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  switch(nread) {
  case 0:
    nếu((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      trả_về CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    break;

  case CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    break;

  case CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    break; /* nothing was read */

  case STOP_FILLING:
  case READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    break;

  default:
    nếu(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      trả_về CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    nếu(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    break;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (số_nguyên)result, *pnread, *peos);
  trả_về result;
}

static bool cr_mime_needs_rewind(struct Curl_easy *data,
                                 struct Curl_creader *reader)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  trả_về ctx->read_len > 0;
}

static curl_off_t cr_mime_total_length(struct Curl_easy *data,
                                       struct Curl_creader *reader)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  trả_về ctx->total_len;
}

static CURLcode cr_mime_resume_from(struct Curl_easy *data,
                                    struct Curl_creader *reader,
                                    curl_off_t offset)
{
  struct cr_mime_ctx *ctx = reader->ctx;

  nếu(offset > 0) {
    curl_off_t passed = 0;

    do {
      char scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)sizeof(scratch)) ?
        sizeof(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      nếu((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        trả_về CURLE_READ_ERROR;
      }
    } trong_khi(passed < offset);

    /* now, decrease the size of the read */
    nếu(ctx->total_len > 0) {
      ctx->total_len -= offset;

      nếu(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        trả_về CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  trả_về CURLE_OK;
}

static CURLcode cr_mime_cntrl(struct Curl_easy *data,
                              struct Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  switch(opcode) {
  case CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    nếu(result)
      failf(data, "Cannot rewind mime/post data");
    trả_về result;
  }
  case CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    break;
  case CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    break;
  default:
    break;
  }
  trả_về CURLE_OK;
}

static bool cr_mime_is_paused(struct Curl_easy *data,
                              struct Curl_creader *reader)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  trả_về ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

static const struct Curl_crtype cr_mime = {
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
  sizeof(struct cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(struct Curl_easy *data, curl_mimepart *part)
{
  struct Curl_creader *r;
  struct cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  nếu(result)
    trả_về result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  nếu(result) {
    Curl_creader_free(data, r);
    trả_về result;
  }
  ctx->total_len = mime_size(ctx->part);

  trả_về Curl_creader_set(data, r);
}

#không_thì /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (void)easy;
  trả_về NULL;
}

void curl_mime_free(curl_mime *mime)
{
  (void)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (void)mime;
  trả_về NULL;
}

CURLcode curl_mime_name(curl_mimepart *part, const char *name)
{
  (void)part;
  (void)name;
  trả_về CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, const char *filename)
{
  (void)part;
  (void)filename;
  trả_về CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, const char *mimetype)
{
  (void)part;
  (void)mimetype;
  trả_về CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, const char *encoding)
{
  (void)part;
  (void)encoding;
  trả_về CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, const char *data, size_t datasize)
{
  (void)part;
  (void)data;
  (void)datasize;
  trả_về CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, const char *filename)
{
  (void)part;
  (void)filename;
  trả_về CURLE_NOT_BUILT_IN;
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
  trả_về CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (void)part;
  (void)subparts;
  trả_về CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           struct curl_slist *headers, số_nguyên take_ownership)
{
  (void)part;
  (void)headers;
  (void)take_ownership;
  trả_về CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(struct curl_slist **slp, const char *fmt, ...)
{
  (void)slp;
  (void)fmt;
  trả_về CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

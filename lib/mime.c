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

#만약 !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
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

#아니면

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
  정수 ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  만약(!file)
    반환 0;

  count = 0;
  ret_stat = 1;
  동안(ret_stat > 0) {
    ret_stat = fread(buffer, 1, sizeof(buffer), file);
    만약(ret_stat)
      count += ret_stat;
  }
  curlx_fclose(file);

  반환 count;
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
    반환 VmsRealFileSize(name, stat_buf);
    break;
  default:
    반환 stat_buf->st_size;
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
  정수 res = curlx_stat(file, &statbuf);

  만약(res != -1) {
    switch(statbuf.st_fab_rfm) {
    case FAB$C_VAR:
    case FAB$C_VFC:
    case FAB$C_STMCR:
      반환 curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      break;
    default:
      반환 curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  반환 NULL;
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
  만약(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_INPUT_LENGTH);

  위해(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    위해(p = table; *p && **p != *src; p++)
      ;

    만약(*p)
      result = curlx_dyn_add(&db, *p + 1);
    아니면
      result = curlx_dyn_addn(&db, src, 1);
  }

  반환 curlx_dyn_ptr(&db);
}

/* Check if header matches. */
static char *match_header(struct curl_slist *hdr, const char *lbl, size_t len)
{
  char *value = NULL;

  만약(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    위해(value = hdr->data + len + 1; *value == ' '; value++)
      ;
  반환 value;
}

/* Get a header from an slist. */
static char *search_header(struct curl_slist *hdrlist,
                           const char *hdr, size_t len)
{
  char *value = NULL;

  위해(; !value && hdrlist; hdrlist = hdrlist->next)
    value = match_header(hdrlist, hdr, len);

  반환 value;
}

static char *strippath(const char *fullfile)
{
  char *filename;
  char *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  만약(!filename)
    반환 NULL;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  반환 base; /* returns an allocated string or NULL ! */
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

  만약(!size)
    반환 STOP_FILLING;

  만약(size > insize)
    size = insize;

  만약(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  반환 size;
}

static curl_off_t encoder_nop_size(curl_mimepart *part)
{
  반환 part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
static size_t encoder_7bit_read(char *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  struct mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (void)ateof;

  만약(!size)
    반환 STOP_FILLING;

  만약(size > cursize)
    size = cursize;

  위해(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    만약(*buffer++ & 0x80)
      반환 cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  반환 cursize;
}

/* Base64 content encoder. */
static size_t encoder_base64_read(char *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  struct mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  정수 i;
  char *ptr = buffer;

  동안(st->bufbeg < st->bufend) {
    /* Line full ? */
    만약(st->pos > MAX_ENCODED_LINE_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      만약(size < 2) {
        만약(!cursize)
          반환 STOP_FILLING;
        break;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    만약(size < 4) {
      만약(!cursize)
        반환 STOP_FILLING;
      break;
    }
    만약(st->bufend - st->bufbeg < 3)
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
  만약(ateof) {
    만약(size < 4) {
      만약(!cursize)
        반환 STOP_FILLING;
    }
    아니면 {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      만약(st->bufend != st->bufbeg) {

        만약(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        만약(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  반환 cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_INPUT_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_LINE_LENGTH + 2)) *                  \
    MAX_ENCODED_LINE_LENGTH / 4) * 3 - 3)

static curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  만약(size <= 0)
    반환 size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  만약(size > BASE64_MAX_INPUT_SIZE)
    반환 -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  반환 size + (2 * ((size - 1) / MAX_ENCODED_LINE_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
static 정수 qp_lookahead_eol(struct mime_encoder_state *st, 정수 ateof, size_t n)
{
  n += st->bufbeg;
  만약(n >= st->bufend && ateof)
    반환 1;
  만약(n + 2 > st->bufend)
    반환 ateof ? 0 : -1;
  만약(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    반환 1;
  반환 0;
}

/* Quoted-printable encoder. */
static size_t encoder_qp_read(char *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  struct mime_encoder_state *st = &part->encstate;
  char *ptr = buffer;
  size_t cursize = 0;
  정수 softlinebreak;
  char buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  동안(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    정수 i = (unsigned char)st->buf[st->bufbeg];
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
        반환 cursize;
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
        반환 cursize;
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
    만약(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_LINE_LENGTH;
      만약(!softlinebreak && st->pos + len == MAX_ENCODED_LINE_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        switch(qp_lookahead_eol(st, ateof, consumed)) {
        case -1:        /* Need more data. */
          반환 cursize;
        case 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          break;
        }
      }
      만약(softlinebreak) {
        curlx_strcopy(buf, sizeof(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    만약(len > size) {
      만약(!cursize)
        반환 STOP_FILLING;
      break;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    만약(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  반환 cursize;
}

static curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  반환 part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
static size_t mime_mem_read(char *buffer, size_t size, size_t nitems,
                            void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (void)size;  /* Always 1 */

  만약(!nitems)
    반환 STOP_FILLING;

  만약(sz > nitems)
    sz = nitems;

  만약(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  반환 sz;
}

static 정수 mime_mem_seek(void *instream, curl_off_t offset, 정수 whence)
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

  만약(offset < 0 || offset > part->datasize)
    반환 CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  반환 CURL_SEEKFUNC_OK;
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

  만약(part->fp)
    반환 FALSE;
  part->fp = fopen_read(part->data, "rb");
  반환 part->fp ? FALSE : TRUE;
}

static size_t mime_file_read(char *buffer, size_t size, size_t nitems,
                             void *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  만약(!nitems)
    반환 STOP_FILLING;

  만약(mime_open_file(part))
    반환 READ_ERROR;

  반환 fread(buffer, size, nitems, part->fp);
}

static 정수 mime_file_seek(void *instream, curl_off_t offset, 정수 whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  만약(whence == SEEK_SET && !offset && !part->fp)
    반환 CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  만약(mime_open_file(part))
    반환 CURL_SEEKFUNC_FAIL;

  반환 curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

static void mime_file_free(void *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  만약(part->fp) {
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

  만약(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  아니면 {
    sz = offset - numbytes;
    만약(sz >= traillen)
      반환 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  만약(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  반환 sz;
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
    반환 part->lastreadstatus;
  default:
    break;
  }

  /* If we can determine we are at end of part data, spare a read. */
  만약(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  아니면 {
    switch(part->kind) {
    case MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      break;
    case MIMEKIND_FILE:
      만약(part->fp && feof(part->fp))
        break;  /* At EOF. */
      FALLTHROUGH();
    default:
      만약(part->readfunc) {
        만약(!(part->flags & MIME_FAST_READ)) {
          만약(*hasread)
            반환 STOP_FILLING;
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

  반환 sz;
}

/* Read and encode part content. */
static size_t read_encoded_part_content(curl_mimepart *part, char *buffer,
                                        size_t bufsize, bool *hasread)
{
  struct mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  위해(;;) {
    만약(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      switch(sz) {
      case 0:
        만약(ateof)
          반환 cursize;
        break;
      case READ_ERROR:
      case STOP_FILLING:
        반환 cursize ? cursize : sz;
      default:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        continue;
      }
    }

    /* We need more data in input buffer. */
    만약(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      만약(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    만약(st->bufend >= sizeof(st->buf))
      반환 cursize ? cursize : READ_ERROR;    /* Buffer full. */
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
      반환 cursize ? cursize : sz;
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
  동안(bufsize) {
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
      만약(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, NULL);
        break;
      }
      만약(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        break;
      }
      FALLTHROUGH();
    case MIMESTATE_CURLHEADERS:
      만약(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      아니면 {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        만약(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      break;
    case MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      만약(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, NULL);
      break;
    case MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, NULL);
      break;
    case MIMESTATE_CONTENT:
      만약(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      아니면
        sz = read_part_content(part, buffer, bufsize, hasread);
      switch(sz) {
      case 0:
        mimesetstate(&part->state, MIMESTATE_END, NULL);
        /* Try sparing open file descriptors. */
        만약(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = NULL;
        }
        FALLTHROUGH();
      case CURL_READFUNC_ABORT:
      case CURL_READFUNC_PAUSE:
      case READ_ERROR:
      case STOP_FILLING:
        반환 cursize ? cursize : sz;
      }
      break;
    case MIMESTATE_END:
      반환 cursize;
    default:
      break;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  반환 cursize;
}

/* Readback from mime. Warning: not a read callback function. */
static size_t mime_subparts_read(char *buffer, size_t size, size_t nitems,
                                 void *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (void)size;  /* Always 1 */

  동안(nitems) {
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
      만약(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      break;
    case MIMESTATE_BOUNDARY2:
      만약(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      아니면
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      만약(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      break;
    case MIMESTATE_CONTENT:
      만약(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, NULL);
        break;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      switch(sz) {
      case CURL_READFUNC_ABORT:
      case CURL_READFUNC_PAUSE:
      case READ_ERROR:
      case STOP_FILLING:
        반환 cursize ? cursize : sz;
      case 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        break;
      }
      break;
    case MIMESTATE_END:
      반환 cursize;
    default:
      break;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  반환 cursize;
}

static 정수 mime_part_rewind(curl_mimepart *part)
{
  정수 res = CURL_SEEKFUNC_OK;
  enum mimestate targetstate = MIMESTATE_BEGIN;

  만약(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  만약(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    만약(part->seekfunc) {
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

  만약(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, NULL);

  part->lastreadstatus = 1; /* Successful read status. */
  반환 res;
}

static 정수 mime_subparts_seek(void *instream, curl_off_t offset, 정수 whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  정수 rc = CURL_SEEKFUNC_OK;

  만약(whence != SEEK_SET || offset)
    반환 CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  만약(mime->state.state == MIMESTATE_BEGIN)
    반환 CURL_SEEKFUNC_OK;           /* Already rewound. */

  위해(part = mime->firstpart; part; part = part->nextpart) {
    정수 res = mime_part_rewind(part);
    만약(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  만약(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, NULL);

  반환 rc;
}

/* Release part content. */
static void cleanup_part_content(curl_mimepart *part)
{
  만약(part->freefunc)
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
  part->flags &= ~(unsigned 정수)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

static void mime_subparts_free(void *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  만약(mime && mime->parent) {
    mime->parent->freefunc = NULL;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
static void mime_subparts_unbind(void *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  만약(mime && mime->parent) {
    mime->parent->freefunc = NULL;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = NULL;
  }
}

void Curl_mime_cleanpart(curl_mimepart *part)
{
  만약(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    만약(part->flags & MIME_USERHEADERS_OWNER)
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

  만약(!mime)
    반환;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  동안(mime) {
    part = mime->firstpart;
    만약(part) {
      mime->firstpart = part->nextpart;
      만약(part->kind == MIMEKIND_MULTIPART && part->arg &&
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
    아니면 {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

static bool mime_is_too_deep(const curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  만약(++call_depth > MAX_MIME_LEVELS)
    반환 TRUE;

  만약(part->kind == MIMEKIND_MULTIPART && part->arg) {
    const curl_mime *mime = (const curl_mime *)part->arg;
    const curl_mimepart *s;

    위해(s = mime->firstpart; s; s = s->nextpart) {
      만약(mime_is_too_deep(s, call_depth))
        반환 TRUE;
    }
  }

  반환 FALSE;
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

  만약(++call_depth > MAX_MIME_LEVELS)
    반환 CURLE_TOO_LARGE;

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
    만약(result == CURLE_READ_ERROR)
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
    위해(s = ((curl_mime *)src->arg)->firstpart; !result && s;
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
  만약(!result && src->userheaders) {
    struct curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    만약(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    아니면 {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      만약(result)
        curl_slist_free_all(hdrs);
    }
  }

  만약(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  만약(!result)
    result = curl_mime_name(dst, src->name);
  만약(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  만약(result)
    Curl_mime_cleanpart(dst);

  반환 result;
}

CURLcode Curl_mime_duppart(struct Curl_easy *data,
                           curl_mimepart *dst, const curl_mimepart *src)
{
  만약(!dst || !src)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  만약(mime_is_too_deep(src, 0))
    반환 CURLE_TOO_LARGE;

  반환 mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(void *easy)
{
  curl_mime *mime = curlx_malloc(sizeof(*mime));

  만약(mime) {
    mime->parent = NULL;
    mime->firstpart = NULL;
    mime->lastpart = NULL;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    만약(Curl_rand_alnum(easy,
                       (unsigned char *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      반환 NULL;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, NULL);
  }

  반환 mime;
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

  만약(!mime)
    반환 NULL;

  part = curlx_malloc(sizeof(*part));

  만약(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    만약(mime->lastpart)
      mime->lastpart->nextpart = part;
    아니면
      mime->firstpart = part;

    mime->lastpart = part;
  }

  반환 part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, const char *name)
{
  만약(!part)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  만약(name) {
    part->name = curlx_strdup(name);
    만약(!part->name)
      반환 CURLE_OUT_OF_MEMORY;
  }

  반환 CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, const char *filename)
{
  만약(!part)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  만약(filename) {
    part->filename = curlx_strdup(filename);
    만약(!part->filename)
      반환 CURLE_OUT_OF_MEMORY;
  }

  반환 CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, const char *data, size_t datasize)
{
  만약(!part)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  만약(data) {
    만약(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    만약(!part->data)
      반환 CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  반환 CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, const char *filename)
{
  CURLcode result = CURLE_OK;

  만약(!part)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  만약(filename) {
    char *base;
    curlx_struct_stat sbuf;

    만약(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    아니면 {
      part->data = curlx_strdup(filename);
      만약(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      아니면 {
        part->datasize = -1;
        만약(S_ISREG(sbuf.st_mode)) {
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
        만약(!base)
          result = CURLE_OUT_OF_MEMORY;
        아니면 {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  반환 result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, const char *mimetype)
{
  만약(!part)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  만약(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    만약(!part->mimetype)
      반환 CURLE_OUT_OF_MEMORY;
  }

  반환 CURLE_OK;
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

  만약(!part)
    반환 result;

  part->encoder = NULL;

  만약(!encoding)
    반환 CURLE_OK;    /* Removing current encoder. */

  위해(mep = encoders; mep->name; mep++)
    만약(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  반환 result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           struct curl_slist *headers, 정수 take_ownership)
{
  만약(!part)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  만약(part->flags & MIME_USERHEADERS_OWNER) {
    만약(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned 정수)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  만약(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  반환 CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, void *arg)
{
  만약(!part)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  만약(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  반환 CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, 정수 take_ownership)
{
  curl_mime *root;

  만약(!part)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  만약(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    반환 CURLE_OK;

  cleanup_part_content(part);

  만약(subparts) {
    /* Should not have been attached already. */
    만약(subparts->parent)
      반환 CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    root = part->parent;
    만약(root) {
      동안(root->parent && root->parent->parent)
        root = root->parent->parent;
      만약(subparts == root) {
        /* cannot add as a subpart of itself. */
        반환 CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    만약(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      반환 CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  반환 CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  반환 Curl_mime_set_subparts(part, subparts, TRUE);
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
  } 동안(ret == STOP_FILLING);

  반환 ret;
}

/* Rewind mime stream. */
static CURLcode mime_rewind(curl_mimepart *part)
{
  반환 mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
static size_t slist_size(struct curl_slist *s,
                         size_t overhead, const char *skip, size_t skiplen)
{
  size_t size = 0;

  위해(; s; s = s->next)
    만약(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  반환 size;
}

/* Get/compute multipart size. */
static curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  만약(!mime)
    반환 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  위해(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    만약(sz < 0)
      size = sz;

    만약(size >= 0)
      size += boundarysize + sz;
  }

  반환 size;
}

/* Get/compute mime size. */
static curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  만약(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  만약(part->encoder)
    size = part->encoder->sizefunc(part);

  만약(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, NULL, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  반환 size;
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

  만약(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    만약(hdr)
      *slp = hdr;
    아니면
      curlx_free(s);
  }

  반환 hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
static CURLcode add_content_type(struct curl_slist **slp,
                                 const char *type, const char *boundary)
{
  반환 Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
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

  만약(filename) {
    size_t len1 = strlen(filename);
    const char *nameend = filename + len1;
    unsigned 정수 i;

    위해(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      만약(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        반환 ctts[i].type;
    }
  }
  반환 NULL;
}

static bool content_type_match(const char *contenttype,
                               const char *target, size_t len)
{
  만약(contenttype && curl_strnequal(contenttype, target, len))
    switch(contenttype[len]) {
    case '\0':
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ';':
      반환 TRUE;
    }
  반환 FALSE;
}

static CURLcode add_content_disposition(struct Curl_easy *data,
                                        curl_mimepart *part,
                                        const char *disposition,
                                        const char *contenttype,
                                        enum mimestrategy strategy)
{
  만약(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  만약(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = NULL;

  만약(disposition) {
    CURLcode result = CURLE_OK;
    char *name = NULL;
    char *filename = NULL;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    만약(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      반환 CURLE_BAD_FUNCTION_ARGUMENT;

    만약(part->name) {
      name = escape_string(data, part->name, strategy);
      만약(!name)
        반환 CURLE_OUT_OF_MEMORY;
    }
    만약(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      만약(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    만약(!result)
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
    만약(result)
      반환 result;
  }
  반환 CURLE_OK;
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

  만약(++call_depth > MAX_MIME_LEVELS)
    반환 CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = NULL;

  /* Be sure we will not access old headers later. */
  만약(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, NULL);

  /* Check if content type is specified. */
  customct = part->mimetype;
  만약(!customct)
    customct = search_header(part->userheaders, STRCONST("Content-Type"));
  만약(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  만약(!contenttype) {
    switch(part->kind) {
    case MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      break;
    case MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      만약(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      만약(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      break;
    default:
      contenttype = Curl_mime_contenttype(part->filename);
      break;
    }
  }

  만약(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    만약(mime)
      boundary = mime->boundary;
  }
  아니면 만약(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = NULL;

  /* Issue content-disposition header only if not already set by caller. */
  만약(!search_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    만약(result)
      반환 result;
  }

  /* Issue Content-Type header. */
  만약(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    만약(result)
      반환 result;
  }

  /* Content-Transfer-Encoding header. */
  만약(!search_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    만약(part->encoder)
      cte = part->encoder->name;
    아니면 만약(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    만약(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      만약(result)
        반환 result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  만약(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  만약(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = NULL;
    만약(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    위해(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, NULL,
                                    disposition, strategy, call_depth);
      만약(result)
        반환 result;
    }
  }
  반환 result;
}

CURLcode Curl_mime_prepare_headers(struct Curl_easy *data,
                                   curl_mimepart *part,
                                   const char *contenttype,
                                   const char *disposition,
                                   enum mimestrategy strategy)
{
  반환 mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
static void mime_unpause(curl_mimepart *part)
{
  만약(part) {
    만약(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    만약(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      만약(mime) {
        curl_mimepart *subpart;

        위해(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
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
  반환 CURLE_OK;
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
  만약(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (정수)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    반환 ctx->error_result;
  }
  만약(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    반환 CURLE_OK;
  }
  /* respect length limitations */
  만약(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    만약(remain <= 0)
      blen = 0;
    아니면 만약(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  만약(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned char *)buf, blen, &nread);
    만약(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      반환 result;
    }
  }
  아니면 만약(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, sizeof(tmp), ctx->part);
    만약(nread <= sizeof(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned char *)tmp, nread, &n);
      만약(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        반환 result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      만약(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        반환 result;
      }
    }
  }
  아니면
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  switch(nread) {
  case 0:
    만약((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      반환 CURLE_READ_ERROR;
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
    만약(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      반환 CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    만약(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    break;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (정수)result, *pnread, *peos);
  반환 result;
}

static bool cr_mime_needs_rewind(struct Curl_easy *data,
                                 struct Curl_creader *reader)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  반환 ctx->read_len > 0;
}

static curl_off_t cr_mime_total_length(struct Curl_easy *data,
                                       struct Curl_creader *reader)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  반환 ctx->total_len;
}

static CURLcode cr_mime_resume_from(struct Curl_easy *data,
                                    struct Curl_creader *reader,
                                    curl_off_t offset)
{
  struct cr_mime_ctx *ctx = reader->ctx;

  만약(offset > 0) {
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
      만약((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        반환 CURLE_READ_ERROR;
      }
    } 동안(passed < offset);

    /* now, decrease the size of the read */
    만약(ctx->total_len > 0) {
      ctx->total_len -= offset;

      만약(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        반환 CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  반환 CURLE_OK;
}

static CURLcode cr_mime_cntrl(struct Curl_easy *data,
                              struct Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  switch(opcode) {
  case CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    만약(result)
      failf(data, "Cannot rewind mime/post data");
    반환 result;
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
  반환 CURLE_OK;
}

static bool cr_mime_is_paused(struct Curl_easy *data,
                              struct Curl_creader *reader)
{
  struct cr_mime_ctx *ctx = reader->ctx;
  (void)data;
  반환 ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
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
  만약(result)
    반환 result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  만약(result) {
    Curl_creader_free(data, r);
    반환 result;
  }
  ctx->total_len = mime_size(ctx->part);

  반환 Curl_creader_set(data, r);
}

#아니면 /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (void)easy;
  반환 NULL;
}

void curl_mime_free(curl_mime *mime)
{
  (void)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (void)mime;
  반환 NULL;
}

CURLcode curl_mime_name(curl_mimepart *part, const char *name)
{
  (void)part;
  (void)name;
  반환 CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, const char *filename)
{
  (void)part;
  (void)filename;
  반환 CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, const char *mimetype)
{
  (void)part;
  (void)mimetype;
  반환 CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, const char *encoding)
{
  (void)part;
  (void)encoding;
  반환 CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, const char *data, size_t datasize)
{
  (void)part;
  (void)data;
  (void)datasize;
  반환 CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, const char *filename)
{
  (void)part;
  (void)filename;
  반환 CURLE_NOT_BUILT_IN;
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
  반환 CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (void)part;
  (void)subparts;
  반환 CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           struct curl_slist *headers, 정수 take_ownership)
{
  (void)part;
  (void)headers;
  (void)take_ownership;
  반환 CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(struct curl_slist **slp, const char *fmt, ...)
{
  (void)slp;
  (void)fmt;
  반환 CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

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

بنية Curl_easy;

#ضمن "mime.h"
#ضمن "urldata.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "transfer.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/basename.h"
#ضمن "curlx/strcopy.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "curlx/base64.h"

#אם !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#ضمن "rand.h"
#ضمن "slist.h"
#ضمن "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

ساكن size_t mime_subparts_read(حرف *buffer, size_t size, size_t nitems,
                                 فراغ *instream, bool *hasread);
ساكن curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
ساكن ثابت unsigned حرف qp_class[] = {
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
ساكن ثابت حرف aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#אחרת

#ضمن <fabdef.h>
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
curl_off_t VmsRealFileSize(ثابت حرف *name, ثابت curlx_struct_stat *stat_buf)
{
  حرف buffer[8192];
  curl_off_t count;
  صحيح ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  אם(!file)
    החזר 0;

  count = 0;
  ret_stat = 1;
  כלעוד(ret_stat > 0) {
    ret_stat = fread(buffer, 1, حجم(buffer), file);
    אם(ret_stat)
      count += ret_stat;
  }
  curlx_fclose(file);

  החזר count;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
ساكن curl_off_t VmsSpecialSize(ثابت حرف *name,
                                 ثابت curlx_struct_stat *stat_buf)
{
  بدل(stat_buf->st_fab_rfm) {
  حالة FAB$C_VAR:
  حالة FAB$C_VFC:
    החזר VmsRealFileSize(name, stat_buf);
    اكسر;
  افتراضي:
    החזר stat_buf->st_size;
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
ساكن FILE *vmsfopenread(ثابت حرف *file, ثابت حرف *mode)
{
  curlx_struct_stat statbuf;
  صحيح res = curlx_stat(file, &statbuf);

  אם(res != -1) {
    بدل(statbuf.st_fab_rfm) {
    حالة FAB$C_VAR:
    حالة FAB$C_VFC:
    حالة FAB$C_STMCR:
      החזר curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      اكسر;
    افتراضي:
      החזר curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  החזר عدم;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
ساكن فراغ mimesetstate(بنية mime_state *state,
                         تعداد mimestate tok, فراغ *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
ساكن حرف *escape_string(بنية Curl_easy *data,
                           ثابت حرف *src, تعداد mimestrategy strategy)
{
  CURLcode result;
  بنية dynbuf db;
  ثابت حرف * ثابت *table;
  ثابت حرف * ثابت *p;
  /* replace first character by rest of string. */
  ساكن ثابت حرف * ثابت mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    عدم
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  ساكن ثابت حرف * ثابت formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    عدم
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  אם(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_INPUT_LENGTH);

  עבור(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    עבור(p = table; *p && **p != *src; p++)
      ;

    אם(*p)
      result = curlx_dyn_add(&db, *p + 1);
    אחרת
      result = curlx_dyn_addn(&db, src, 1);
  }

  החזר curlx_dyn_ptr(&db);
}

/* Check if header matches. */
ساكن حرف *match_header(بنية curl_slist *hdr, ثابت حرف *lbl, size_t len)
{
  حرف *value = عدم;

  אם(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    עבור(value = hdr->data + len + 1; *value == ' '; value++)
      ;
  החזר value;
}

/* Get a header from an slist. */
ساكن حرف *search_header(بنية curl_slist *hdrlist,
                           ثابت حرف *hdr, size_t len)
{
  حرف *value = عدم;

  עבור(; !value && hdrlist; hdrlist = hdrlist->next)
    value = match_header(hdrlist, hdr, len);

  החזר value;
}

ساكن حرف *strippath(ثابت حرف *fullfile)
{
  حرف *filename;
  حرف *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  אם(!filename)
    החזר عدم;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  החזר base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
ساكن فراغ cleanup_encoder_state(بنية mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
ساكن size_t encoder_nop_read(حرف *buffer, size_t size, bool ateof,
                               بنية curl_mimepart *part)
{
  بنية mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (فراغ)ateof;

  אם(!size)
    החזר STOP_FILLING;

  אם(size > insize)
    size = insize;

  אם(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  החזר size;
}

ساكن curl_off_t encoder_nop_size(curl_mimepart *part)
{
  החזר part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
ساكن size_t encoder_7bit_read(حرف *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  بنية mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (فراغ)ateof;

  אם(!size)
    החזר STOP_FILLING;

  אם(size > cursize)
    size = cursize;

  עבור(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    אם(*buffer++ & 0x80)
      החזר cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  החזר cursize;
}

/* Base64 content encoder. */
ساكن size_t encoder_base64_read(حرف *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  بنية mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  صحيح i;
  حرف *ptr = buffer;

  כלעוד(st->bufbeg < st->bufend) {
    /* Line full ? */
    אם(st->pos > MAX_ENCODED_LINE_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      אם(size < 2) {
        אם(!cursize)
          החזר STOP_FILLING;
        اكسر;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    אם(size < 4) {
      אם(!cursize)
        החזר STOP_FILLING;
      اكسر;
    }
    אם(st->bufend - st->bufbeg < 3)
      اكسر;

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
  אם(ateof) {
    אם(size < 4) {
      אם(!cursize)
        החזר STOP_FILLING;
    }
    אחרת {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      אם(st->bufend != st->bufbeg) {

        אם(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        אם(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  החזר cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_INPUT_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_LINE_LENGTH + 2)) *                  \
    MAX_ENCODED_LINE_LENGTH / 4) * 3 - 3)

ساكن curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  אם(size <= 0)
    החזר size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  אם(size > BASE64_MAX_INPUT_SIZE)
    החזר -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  החזר size + (2 * ((size - 1) / MAX_ENCODED_LINE_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
ساكن صحيح qp_lookahead_eol(بنية mime_encoder_state *st, صحيح ateof, size_t n)
{
  n += st->bufbeg;
  אם(n >= st->bufend && ateof)
    החזר 1;
  אם(n + 2 > st->bufend)
    החזר ateof ? 0 : -1;
  אם(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    החזר 1;
  החזר 0;
}

/* Quoted-printable encoder. */
ساكن size_t encoder_qp_read(حرف *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  بنية mime_encoder_state *st = &part->encstate;
  حرف *ptr = buffer;
  size_t cursize = 0;
  صحيح softlinebreak;
  حرف buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  כלעוד(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    صحيح i = (unsigned حرف)st->buf[st->bufbeg];
    buf[0] = (حرف)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    بدل(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    حالة QP_OK:          /* Not a special character. */
      اكسر;
    حالة QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      بدل(qp_lookahead_eol(st, ateof, 1)) {
      حالة -1:          /* More input data needed. */
        החזר cursize;
      حالة 0:           /* No encoding needed. */
        اكسر;
      افتراضي:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        اكسر;
      }
      اكسر;
    حالة QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      بدل(qp_lookahead_eol(st, ateof, 0)) {
      حالة -1:          /* Need more data. */
        החזר cursize;
      حالة 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        اكسر;
      افتراضي:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        اكسر;
      }
      اكسر;
    افتراضي:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      اكسر;
    }

    /* Be sure the encoded character fits within maximum line length. */
    אם(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_LINE_LENGTH;
      אם(!softlinebreak && st->pos + len == MAX_ENCODED_LINE_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        بدل(qp_lookahead_eol(st, ateof, consumed)) {
        حالة -1:        /* Need more data. */
          החזר cursize;
        حالة 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          اكسر;
        }
      }
      אם(softlinebreak) {
        curlx_strcopy(buf, حجم(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    אם(len > size) {
      אם(!cursize)
        החזר STOP_FILLING;
      اكسر;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    אם(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  החזר cursize;
}

ساكن curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  החזר part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
ساكن size_t mime_mem_read(حرف *buffer, size_t size, size_t nitems,
                            فراغ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (فراغ)size;  /* Always 1 */

  אם(!nitems)
    החזר STOP_FILLING;

  אם(sz > nitems)
    sz = nitems;

  אם(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  החזר sz;
}

ساكن صحيح mime_mem_seek(فراغ *instream, curl_off_t offset, صحيح whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  بدل(whence) {
  حالة SEEK_CUR:
    offset += part->state.offset;
    اكسر;
  حالة SEEK_END:
    offset += part->datasize;
    اكسر;
  }

  אם(offset < 0 || offset > part->datasize)
    החזר CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  החזר CURL_SEEKFUNC_OK;
}

ساكن فراغ mime_mem_free(فراغ *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
ساكن bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  אם(part->fp)
    החזר FALSE;
  part->fp = fopen_read(part->data, "rb");
  החזר part->fp ? FALSE : TRUE;
}

ساكن size_t mime_file_read(حرف *buffer, size_t size, size_t nitems,
                             فراغ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  אם(!nitems)
    החזר STOP_FILLING;

  אם(mime_open_file(part))
    החזר READ_ERROR;

  החזר fread(buffer, size, nitems, part->fp);
}

ساكن صحيح mime_file_seek(فراغ *instream, curl_off_t offset, صحيح whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  אם(whence == SEEK_SET && !offset && !part->fp)
    החזר CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  אם(mime_open_file(part))
    החזר CURL_SEEKFUNC_FAIL;

  החזר curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

ساكن فراغ mime_file_free(فراغ *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  אם(part->fp) {
    curlx_fclose(part->fp);
    part->fp = عدم;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
ساكن size_t readback_bytes(بنية mime_state *state,
                             حرف *buffer, size_t bufsize,
                             ثابت حرف *bytes, size_t numbytes,
                             ثابت حرف *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  אם(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  אחרת {
    sz = offset - numbytes;
    אם(sz >= traillen)
      החזר 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  אם(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  החזר sz;
}

/* Read a non-encoded part content. */

ساكن size_t read_part_content(curl_mimepart *part, حرف *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  بدل(part->lastreadstatus) {
  حالة 0:
  حالة CURL_READFUNC_ABORT:
  حالة CURL_READFUNC_PAUSE:
  حالة READ_ERROR:
    החזר part->lastreadstatus;
  افتراضي:
    اكسر;
  }

  /* If we can determine we are at end of part data, spare a read. */
  אם(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  אחרת {
    بدل(part->kind) {
    حالة MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      اكسر;
    حالة MIMEKIND_FILE:
      אם(part->fp && feof(part->fp))
        اكسر;  /* At EOF. */
      FALLTHROUGH();
    افتراضي:
      אם(part->readfunc) {
        אם(!(part->flags & MIME_FAST_READ)) {
          אם(*hasread)
            החזר STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      اكسر;
    }
  }

  بدل(sz) {
  حالة STOP_FILLING:
    اكسر;
  حالة 0:
  حالة CURL_READFUNC_ABORT:
  حالة CURL_READFUNC_PAUSE:
  حالة READ_ERROR:
    part->lastreadstatus = sz;
    اكسر;
  افتراضي:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    اكسر;
  }

  החזר sz;
}

/* Read and encode part content. */
ساكن size_t read_encoded_part_content(curl_mimepart *part, حرف *buffer,
                                        size_t bufsize, bool *hasread)
{
  بنية mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  עבור(;;) {
    אם(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      بدل(sz) {
      حالة 0:
        אם(ateof)
          החזר cursize;
        اكسر;
      حالة READ_ERROR:
      حالة STOP_FILLING:
        החזר cursize ? cursize : sz;
      افتراضي:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        استمر;
      }
    }

    /* We need more data in input buffer. */
    אם(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      אם(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    אם(st->bufend >= حجم(st->buf))
      החזר cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           حجم(st->buf) - st->bufend, hasread);
    بدل(sz) {
    حالة 0:
      ateof = TRUE;
      اكسر;
    حالة CURL_READFUNC_ABORT:
    حالة CURL_READFUNC_PAUSE:
    حالة READ_ERROR:
    حالة STOP_FILLING:
      החזר cursize ? cursize : sz;
    افتراضي:
      st->bufend += sz;
      اكسر;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
ساكن size_t readback_part(curl_mimepart *part,
                            حرف *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  כלעוד(bufsize) {
    size_t sz = 0;
    بنية curl_slist *hdr = (بنية curl_slist *)part->state.ptr;
    بدل(part->state.state) {
    حالة MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      اكسر;
    حالة MIMESTATE_USERHEADERS:
      אם(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, عدم);
        اكسر;
      }
      אם(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        اكسر;
      }
      FALLTHROUGH();
    حالة MIMESTATE_CURLHEADERS:
      אם(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      אחרת {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        אם(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      اكسر;
    حالة MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      אם(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, عدم);
      اكسر;
    حالة MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, عدم);
      اكسر;
    حالة MIMESTATE_CONTENT:
      אם(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      אחרת
        sz = read_part_content(part, buffer, bufsize, hasread);
      بدل(sz) {
      حالة 0:
        mimesetstate(&part->state, MIMESTATE_END, عدم);
        /* Try sparing open file descriptors. */
        אם(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = عدم;
        }
        FALLTHROUGH();
      حالة CURL_READFUNC_ABORT:
      حالة CURL_READFUNC_PAUSE:
      حالة READ_ERROR:
      حالة STOP_FILLING:
        החזר cursize ? cursize : sz;
      }
      اكسر;
    حالة MIMESTATE_END:
      החזר cursize;
    افتراضي:
      اكسر;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  החזר cursize;
}

/* Readback from mime. Warning: not a read callback function. */
ساكن size_t mime_subparts_read(حرف *buffer, size_t size, size_t nitems,
                                 فراغ *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (فراغ)size;  /* Always 1 */

  כלעוד(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    بدل(mime->state.state) {
    حالة MIMESTATE_BEGIN:
    حالة MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      اكسر;
    حالة MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      אם(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      اكسر;
    حالة MIMESTATE_BOUNDARY2:
      אם(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      אחרת
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      אם(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      اكسر;
    حالة MIMESTATE_CONTENT:
      אם(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, عدم);
        اكسر;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      بدل(sz) {
      حالة CURL_READFUNC_ABORT:
      حالة CURL_READFUNC_PAUSE:
      حالة READ_ERROR:
      حالة STOP_FILLING:
        החזר cursize ? cursize : sz;
      حالة 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        اكسر;
      }
      اكسر;
    حالة MIMESTATE_END:
      החזר cursize;
    افتراضي:
      اكسر;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  החזר cursize;
}

ساكن صحيح mime_part_rewind(curl_mimepart *part)
{
  صحيح res = CURL_SEEKFUNC_OK;
  تعداد mimestate targetstate = MIMESTATE_BEGIN;

  אם(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  אם(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    אם(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      بدل(res) {
      حالة CURL_SEEKFUNC_OK:
      حالة CURL_SEEKFUNC_FAIL:
      حالة CURL_SEEKFUNC_CANTSEEK:
        اكسر;
      حالة -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        اكسر;
      افتراضي:
        res = CURL_SEEKFUNC_FAIL;
        اكسر;
      }
    }
  }

  אם(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, عدم);

  part->lastreadstatus = 1; /* Successful read status. */
  החזר res;
}

ساكن صحيح mime_subparts_seek(فراغ *instream, curl_off_t offset, صحيح whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  صحيح rc = CURL_SEEKFUNC_OK;

  אם(whence != SEEK_SET || offset)
    החזר CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  אם(mime->state.state == MIMESTATE_BEGIN)
    החזר CURL_SEEKFUNC_OK;           /* Already rewound. */

  עבור(part = mime->firstpart; part; part = part->nextpart) {
    صحيح res = mime_part_rewind(part);
    אם(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  אם(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, عدم);

  החזר rc;
}

/* Release part content. */
ساكن فراغ cleanup_part_content(curl_mimepart *part)
{
  אם(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = عدم;
  part->seekfunc = عدم;
  part->freefunc = عدم;
  part->arg = (فراغ *)part;           /* Defaults to part itself. */
  part->data = عدم;
  part->fp = عدم;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned صحيح)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

ساكن فراغ mime_subparts_free(فراغ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  אם(mime && mime->parent) {
    mime->parent->freefunc = عدم;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
ساكن فراغ mime_subparts_unbind(فراغ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  אם(mime && mime->parent) {
    mime->parent->freefunc = عدم;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = عدم;
  }
}

فراغ Curl_mime_cleanpart(curl_mimepart *part)
{
  אם(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    אם(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
فراغ curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  אם(!mime)
    החזר;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  כלעוד(mime) {
    part = mime->firstpart;
    אם(part) {
      mime->firstpart = part->nextpart;
      אם(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = عدم;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    אחרת {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

ساكن bool mime_is_too_deep(ثابت curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  אם(++call_depth > MAX_MIME_LEVELS)
    החזר TRUE;

  אם(part->kind == MIMEKIND_MULTIPART && part->arg) {
    ثابت curl_mime *mime = (ثابت curl_mime *)part->arg;
    ثابت curl_mimepart *s;

    עבור(s = mime->firstpart; s; s = s->nextpart) {
      אם(mime_is_too_deep(s, call_depth))
        החזר TRUE;
    }
  }

  החזר FALSE;
}

ساكن CURLcode mime_duppart(بنية Curl_easy *data,
                             curl_mimepart *dst, ثابت curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  ثابت curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  אם(++call_depth > MAX_MIME_LEVELS)
    החזר CURLE_TOO_LARGE;

  /* Duplicate content. */
  بدل(src->kind) {
  حالة MIMEKIND_NONE:
    اكسر;
  حالة MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    اكسر;
  حالة MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    אם(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    اكسر;
  حالة MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    اكسر;
  حالة MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    עבור(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    اكسر;
  افتراضي:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    اكسر;
  }

  /* Duplicate headers. */
  אם(!result && src->userheaders) {
    بنية curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    אם(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    אחרת {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      אם(result)
        curl_slist_free_all(hdrs);
    }
  }

  אם(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  אם(!result)
    result = curl_mime_name(dst, src->name);
  אם(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  אם(result)
    Curl_mime_cleanpart(dst);

  החזר result;
}

CURLcode Curl_mime_duppart(بنية Curl_easy *data,
                           curl_mimepart *dst, ثابت curl_mimepart *src)
{
  אם(!dst || !src)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  אם(mime_is_too_deep(src, 0))
    החזר CURLE_TOO_LARGE;

  החזר mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(فراغ *easy)
{
  curl_mime *mime = curlx_malloc(حجم(*mime));

  אם(mime) {
    mime->parent = عدم;
    mime->firstpart = عدم;
    mime->lastpart = عدم;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    אם(Curl_rand_alnum(easy,
                       (unsigned حرف *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      החזר عدم;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, عدم);
  }

  החזר mime;
}

/* Initialize a mime part. */
فراغ Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, حجم(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, عدم);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  אם(!mime)
    החזר عدم;

  part = curlx_malloc(حجم(*part));

  אם(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    אם(mime->lastpart)
      mime->lastpart->nextpart = part;
    אחרת
      mime->firstpart = part;

    mime->lastpart = part;
  }

  החזר part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ثابت حرف *name)
{
  אם(!part)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  אם(name) {
    part->name = curlx_strdup(name);
    אם(!part->name)
      החזר CURLE_OUT_OF_MEMORY;
  }

  החזר CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ثابت حرف *filename)
{
  אם(!part)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  אם(filename) {
    part->filename = curlx_strdup(filename);
    אם(!part->filename)
      החזר CURLE_OUT_OF_MEMORY;
  }

  החזר CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, ثابت حرف *data, size_t datasize)
{
  אם(!part)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  אם(data) {
    אם(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    אם(!part->data)
      החזר CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  החזר CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ثابت حرف *filename)
{
  CURLcode result = CURLE_OK;

  אם(!part)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  אם(filename) {
    حرف *base;
    curlx_struct_stat sbuf;

    אם(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    אחרת {
      part->data = curlx_strdup(filename);
      אם(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      אחרת {
        part->datasize = -1;
        אם(S_ISREG(sbuf.st_mode)) {
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
        אם(!base)
          result = CURLE_OUT_OF_MEMORY;
        אחרת {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  החזר result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ثابت حرف *mimetype)
{
  אם(!part)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  אם(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    אם(!part->mimetype)
      החזר CURLE_OUT_OF_MEMORY;
  }

  החזר CURLE_OK;
}

ساكن ثابت بنية mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ثابت حرف *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  ثابت بنية mime_encoder *mep;

  אם(!part)
    החזר result;

  part->encoder = عدم;

  אם(!encoding)
    החזר CURLE_OK;    /* Removing current encoder. */

  עבור(mep = encoders; mep->name; mep++)
    אם(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  החזר result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           بنية curl_slist *headers, صحيح take_ownership)
{
  אם(!part)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  אם(part->flags & MIME_USERHEADERS_OWNER) {
    אם(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned صحيح)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  אם(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  החזר CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, فراغ *arg)
{
  אם(!part)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  אם(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  החזר CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, صحيح take_ownership)
{
  curl_mime *root;

  אם(!part)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  אם(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    החזר CURLE_OK;

  cleanup_part_content(part);

  אם(subparts) {
    /* Should not have been attached already. */
    אם(subparts->parent)
      החזר CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    root = part->parent;
    אם(root) {
      כלעוד(root->parent && root->parent->parent)
        root = root->parent->parent;
      אם(subparts == root) {
        /* cannot add as a subpart of itself. */
        החזר CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    אם(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      החזר CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  החזר CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  החזר Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(حرف *buffer, size_t size, size_t nitems, فراغ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (فراغ)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  עשה {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } כלעוד(ret == STOP_FILLING);

  החזר ret;
}

/* Rewind mime stream. */
ساكن CURLcode mime_rewind(curl_mimepart *part)
{
  החזר mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
ساكن size_t slist_size(بنية curl_slist *s,
                         size_t overhead, ثابت حرف *skip, size_t skiplen)
{
  size_t size = 0;

  עבור(; s; s = s->next)
    אם(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  החזר size;
}

/* Get/compute multipart size. */
ساكن curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  אם(!mime)
    החזר 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  עבור(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    אם(sz < 0)
      size = sz;

    אם(size >= 0)
      size += boundarysize + sz;
  }

  החזר size;
}

/* Get/compute mime size. */
ساكن curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  אם(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  אם(part->encoder)
    size = part->encoder->sizefunc(part);

  אם(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, عدم, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  החזר size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(بنية curl_slist **slp, ثابت حرف *fmt, ...)
{
  بنية curl_slist *hdr = عدم;
  حرف *s = عدم;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  אם(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    אם(hdr)
      *slp = hdr;
    אחרת
      curlx_free(s);
  }

  החזר hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
ساكن CURLcode add_content_type(بنية curl_slist **slp,
                                 ثابت حرف *type, ثابت حرف *boundary)
{
  החזר Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ثابت حرف *Curl_mime_contenttype(ثابت حرف *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  بنية ContentType {
    ثابت حرف *extension;
    ثابت حرف *type;
  };
  ساكن ثابت بنية ContentType ctts[] = {
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

  אם(filename) {
    size_t len1 = strlen(filename);
    ثابت حرف *nameend = filename + len1;
    unsigned صحيح i;

    עבור(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      אם(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        החזר ctts[i].type;
    }
  }
  החזר عدم;
}

ساكن bool content_type_match(ثابت حرف *contenttype,
                               ثابت حرف *target, size_t len)
{
  אם(contenttype && curl_strnequal(contenttype, target, len))
    بدل(contenttype[len]) {
    حالة '\0':
    حالة '\t':
    حالة '\r':
    حالة '\n':
    حالة ' ':
    حالة ';':
      החזר TRUE;
    }
  החזר FALSE;
}

ساكن CURLcode add_content_disposition(بنية Curl_easy *data,
                                        curl_mimepart *part,
                                        ثابت حرف *disposition,
                                        ثابت حرف *contenttype,
                                        تعداد mimestrategy strategy)
{
  אם(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  אם(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = عدم;

  אם(disposition) {
    CURLcode result = CURLE_OK;
    حرف *name = عدم;
    حرف *filename = عدم;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    אם(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      החזר CURLE_BAD_FUNCTION_ARGUMENT;

    אם(part->name) {
      name = escape_string(data, part->name, strategy);
      אם(!name)
        החזר CURLE_OUT_OF_MEMORY;
    }
    אם(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      אם(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    אם(!result)
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
    אם(result)
      החזר result;
  }
  החזר CURLE_OK;
}

ساكن CURLcode mime_prepare_headers(بنية Curl_easy *data,
                                     curl_mimepart *part,
                                     ثابت حرف *contenttype,
                                     ثابت حرف *disposition,
                                     تعداد mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = عدم;
  ثابت حرف *boundary = عدم;
  حرف *customct;
  ثابت حرف *cte = عدم;
  CURLcode result = CURLE_OK;

  אם(++call_depth > MAX_MIME_LEVELS)
    החזר CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = عدم;

  /* Be sure we will not access old headers later. */
  אם(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, عدم);

  /* Check if content type is specified. */
  customct = part->mimetype;
  אם(!customct)
    customct = search_header(part->userheaders, STRCONST("Content-Type"));
  אם(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  אם(!contenttype) {
    بدل(part->kind) {
    حالة MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      اكسر;
    حالة MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      אם(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      אם(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      اكسر;
    افتراضي:
      contenttype = Curl_mime_contenttype(part->filename);
      اكسر;
    }
  }

  אם(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    אם(mime)
      boundary = mime->boundary;
  }
  אחרת אם(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = عدم;

  /* Issue content-disposition header only if not already set by caller. */
  אם(!search_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    אם(result)
      החזר result;
  }

  /* Issue Content-Type header. */
  אם(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    אם(result)
      החזר result;
  }

  /* Content-Transfer-Encoding header. */
  אם(!search_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    אם(part->encoder)
      cte = part->encoder->name;
    אחרת אם(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    אם(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      אם(result)
        החזר result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  אם(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  אם(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = عدم;
    אם(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    עבור(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, عدم,
                                    disposition, strategy, call_depth);
      אם(result)
        החזר result;
    }
  }
  החזר result;
}

CURLcode Curl_mime_prepare_headers(بنية Curl_easy *data,
                                   curl_mimepart *part,
                                   ثابت حرف *contenttype,
                                   ثابت حرف *disposition,
                                   تعداد mimestrategy strategy)
{
  החזר mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
ساكن فراغ mime_unpause(curl_mimepart *part)
{
  אם(part) {
    אם(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    אם(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      אם(mime) {
        curl_mimepart *subpart;

        עבור(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

بنية cr_mime_ctx {
  بنية Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  بنية bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

ساكن CURLcode cr_mime_init(بنية Curl_easy *data,
                             بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  החזר CURLE_OK;
}

ساكن فراغ cr_mime_close(بنية Curl_easy *data,
                          بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
ساكن CURLcode cr_mime_read(بنية Curl_easy *data,
                             بنية Curl_creader *reader,
                             حرف *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  حرف tmp[256];

  /* Once we have errored, we will return the same error forever */
  אם(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (صحيح)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    החזר ctx->error_result;
  }
  אם(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    החזר CURLE_OK;
  }
  /* respect length limitations */
  אם(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    אם(remain <= 0)
      blen = 0;
    אחרת אם(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  אם(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned حرف *)buf, blen, &nread);
    אם(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      החזר result;
    }
  }
  אחרת אם(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, حجم(tmp), ctx->part);
    אם(nread <= حجم(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned حرف *)tmp, nread, &n);
      אם(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        החזר result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      אם(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        החזר result;
      }
    }
  }
  אחרת
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  بدل(nread) {
  حالة 0:
    אם((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      החזר CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    اكسر;

  حالة CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    اكسر;

  حالة CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    اكسر; /* nothing was read */

  حالة STOP_FILLING:
  حالة READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    اكسر;

  افتراضي:
    אם(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      החזר CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    אם(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    اكسر;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (صحيح)result, *pnread, *peos);
  החזר result;
}

ساكن bool cr_mime_needs_rewind(بنية Curl_easy *data,
                                 بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)data;
  החזר ctx->read_len > 0;
}

ساكن curl_off_t cr_mime_total_length(بنية Curl_easy *data,
                                       بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)data;
  החזר ctx->total_len;
}

ساكن CURLcode cr_mime_resume_from(بنية Curl_easy *data,
                                    بنية Curl_creader *reader,
                                    curl_off_t offset)
{
  بنية cr_mime_ctx *ctx = reader->ctx;

  אם(offset > 0) {
    curl_off_t passed = 0;

    עשה {
      حرف scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      אם((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        החזר CURLE_READ_ERROR;
      }
    } כלעוד(passed < offset);

    /* now, decrease the size of the read */
    אם(ctx->total_len > 0) {
      ctx->total_len -= offset;

      אם(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        החזר CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  החזר CURLE_OK;
}

ساكن CURLcode cr_mime_cntrl(بنية Curl_easy *data,
                              بنية Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    אם(result)
      failf(data, "Cannot rewind mime/post data");
    החזר result;
  }
  حالة CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    اكسر;
  حالة CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  החזר CURLE_OK;
}

ساكن bool cr_mime_is_paused(بنية Curl_easy *data,
                              بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)data;
  החזר ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

ساكن ثابت بنية Curl_crtype cr_mime = {
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
  حجم(بنية cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(بنية Curl_easy *data, curl_mimepart *part)
{
  بنية Curl_creader *r;
  بنية cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  אם(result)
    החזר result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  אם(result) {
    Curl_creader_free(data, r);
    החזר result;
  }
  ctx->total_len = mime_size(ctx->part);

  החזר Curl_creader_set(data, r);
}

#אחרת /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (فراغ)easy;
  החזר عدم;
}

فراغ curl_mime_free(curl_mime *mime)
{
  (فراغ)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (فراغ)mime;
  החזר عدم;
}

CURLcode curl_mime_name(curl_mimepart *part, ثابت حرف *name)
{
  (فراغ)part;
  (فراغ)name;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, ثابت حرف *filename)
{
  (فراغ)part;
  (فراغ)filename;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, ثابت حرف *mimetype)
{
  (فراغ)part;
  (فراغ)mimetype;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ثابت حرف *encoding)
{
  (فراغ)part;
  (فراغ)encoding;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, ثابت حرف *data, size_t datasize)
{
  (فراغ)part;
  (فراغ)data;
  (فراغ)datasize;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ثابت حرف *filename)
{
  (فراغ)part;
  (فراغ)filename;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           فراغ *arg)
{
  (فراغ)part;
  (فراغ)datasize;
  (فراغ)readfunc;
  (فراغ)seekfunc;
  (فراغ)freefunc;
  (فراغ)arg;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (فراغ)part;
  (فراغ)subparts;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           بنية curl_slist *headers, صحيح take_ownership)
{
  (فراغ)part;
  (فراغ)headers;
  (فراغ)take_ownership;
  החזר CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(بنية curl_slist **slp, ثابت حرف *fmt, ...)
{
  (فراغ)slp;
  (فراغ)fmt;
  החזר CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

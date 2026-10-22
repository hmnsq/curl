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

સંરચના Curl_easy;

#અન્તર્ભાવ "mime.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/basename.h"
#અન્તર્ભાવ "curlx/strcopy.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "curlx/base64.h"

#જો !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "slist.h"
#અન્તર્ભાવ "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

સ્થિર size_t mime_subparts_read(અક્ષર *buffer, size_t size, size_t nitems,
                                 શૂન્ય *instream, bool *hasread);
સ્થિર curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
સ્થિર અચલ unsigned અક્ષર qp_class[] = {
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
સ્થિર અચલ અક્ષર aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#નહીંતર

#અન્તર્ભાવ <fabdef.h>
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
curl_off_t VmsRealFileSize(અચલ અક્ષર *name, અચલ curlx_struct_stat *stat_buf)
{
  અક્ષર buffer[8192];
  curl_off_t ગણતરી;
  પૂર્ણાંક ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  જો(!file)
    પાછા 0;

  ગણતરી = 0;
  ret_stat = 1;
  જ્યારે(ret_stat > 0) {
    ret_stat = fread(buffer, 1, કદ(buffer), file);
    જો(ret_stat)
      ગણતરી += ret_stat;
  }
  curlx_fclose(file);

  પાછા ગણતરી;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
સ્થિર curl_off_t VmsSpecialSize(અચલ અક્ષર *name,
                                 અચલ curlx_struct_stat *stat_buf)
{
  પસંદગી(stat_buf->st_fab_rfm) {
  સ્થિતિ FAB$C_VAR:
  સ્થિતિ FAB$C_VFC:
    પાછા VmsRealFileSize(name, stat_buf);
    તોડો;
  મૂળભૂત:
    પાછા stat_buf->st_size;
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
સ્થિર FILE *vmsfopenread(અચલ અક્ષર *file, અચલ અક્ષર *mode)
{
  curlx_struct_stat statbuf;
  પૂર્ણાંક res = curlx_stat(file, &statbuf);

  જો(res != -1) {
    પસંદગી(statbuf.st_fab_rfm) {
    સ્થિતિ FAB$C_VAR:
    સ્થિતિ FAB$C_VFC:
    સ્થિતિ FAB$C_STMCR:
      પાછા curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      તોડો;
    મૂળભૂત:
      પાછા curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  પાછા ખાલી;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
સ્થિર શૂન્ય mimesetstate(સંરચના mime_state *state,
                         ગણના mimestate tok, શૂન્ય *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
સ્થિર અક્ષર *escape_string(સંરચના Curl_easy *data,
                           અચલ અક્ષર *src, ગણના mimestrategy strategy)
{
  CURLcode result;
  સંરચના dynbuf db;
  અચલ અક્ષર * અચલ *table;
  અચલ અક્ષર * અચલ *p;
  /* replace first character by rest of string. */
  સ્થિર અચલ અક્ષર * અચલ mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    ખાલી
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  સ્થિર અચલ અક્ષર * અચલ formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    ખાલી
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  જો(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_આગત_LENGTH);

  માટે(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    માટે(p = table; *p && **p != *src; p++)
      ;

    જો(*p)
      result = curlx_dyn_add(&db, *p + 1);
    નહીંતર
      result = curlx_dyn_addn(&db, src, 1);
  }

  પાછા curlx_dyn_ptr(&db);
}

/* Check if header matches. */
સ્થિર અક્ષર *match_header(સંરચના curl_slist *hdr, અચલ અક્ષર *lbl, size_t len)
{
  અક્ષર *કિંમત = ખાલી;

  જો(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    માટે(કિંમત = hdr->data + len + 1; *કિંમત == ' '; કિંમત++)
      ;
  પાછા કિંમત;
}

/* Get a header from an slist. */
સ્થિર અક્ષર *શોધ_header(સંરચના curl_slist *hdrlist,
                           અચલ અક્ષર *hdr, size_t len)
{
  અક્ષર *કિંમત = ખાલી;

  માટે(; !કિંમત && hdrlist; hdrlist = hdrlist->next)
    કિંમત = match_header(hdrlist, hdr, len);

  પાછા કિંમત;
}

સ્થિર અક્ષર *strippath(અચલ અક્ષર *fullfile)
{
  અક્ષર *filename;
  અક્ષર *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  જો(!filename)
    પાછા ખાલી;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  પાછા base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
સ્થિર શૂન્ય cleanup_encoder_state(સંરચના mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
સ્થિર size_t encoder_nop_read(અક્ષર *buffer, size_t size, bool ateof,
                               સંરચના curl_mimepart *part)
{
  સંરચના mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (શૂન્ય)ateof;

  જો(!size)
    પાછા STOP_FILLING;

  જો(size > insize)
    size = insize;

  જો(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  પાછા size;
}

સ્થિર curl_off_t encoder_nop_size(curl_mimepart *part)
{
  પાછા part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
સ્થિર size_t encoder_7bit_read(અક્ષર *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  સંરચના mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (શૂન્ય)ateof;

  જો(!size)
    પાછા STOP_FILLING;

  જો(size > cursize)
    size = cursize;

  માટે(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    જો(*buffer++ & 0x80)
      પાછા cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  પાછા cursize;
}

/* Base64 content encoder. */
સ્થિર size_t encoder_base64_read(અક્ષર *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  સંરચના mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  પૂર્ણાંક i;
  અક્ષર *ptr = buffer;

  જ્યારે(st->bufbeg < st->bufend) {
    /* Line full ? */
    જો(st->pos > MAX_ENCODED_લીટી_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      જો(size < 2) {
        જો(!cursize)
          પાછા STOP_FILLING;
        તોડો;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    જો(size < 4) {
      જો(!cursize)
        પાછા STOP_FILLING;
      તોડો;
    }
    જો(st->bufend - st->bufbeg < 3)
      તોડો;

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
  જો(ateof) {
    જો(size < 4) {
      જો(!cursize)
        પાછા STOP_FILLING;
    }
    નહીંતર {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      જો(st->bufend != st->bufbeg) {

        જો(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        જો(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  પાછા cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_આગત_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_લીટી_LENGTH + 2)) *                  \
    MAX_ENCODED_લીટી_LENGTH / 4) * 3 - 3)

સ્થિર curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  જો(size <= 0)
    પાછા size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  જો(size > BASE64_MAX_આગત_SIZE)
    પાછા -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  પાછા size + (2 * ((size - 1) / MAX_ENCODED_લીટી_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
સ્થિર પૂર્ણાંક qp_lookahead_eol(સંરચના mime_encoder_state *st, પૂર્ણાંક ateof, size_t n)
{
  n += st->bufbeg;
  જો(n >= st->bufend && ateof)
    પાછા 1;
  જો(n + 2 > st->bufend)
    પાછા ateof ? 0 : -1;
  જો(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    પાછા 1;
  પાછા 0;
}

/* Quoted-printable encoder. */
સ્થિર size_t encoder_qp_read(અક્ષર *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  સંરચના mime_encoder_state *st = &part->encstate;
  અક્ષર *ptr = buffer;
  size_t cursize = 0;
  પૂર્ણાંક softlinebreak;
  અક્ષર buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  જ્યારે(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    પૂર્ણાંક i = (unsigned અક્ષર)st->buf[st->bufbeg];
    buf[0] = (અક્ષર)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    પસંદગી(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    સ્થિતિ QP_OK:          /* Not a special character. */
      તોડો;
    સ્થિતિ QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      પસંદગી(qp_lookahead_eol(st, ateof, 1)) {
      સ્થિતિ -1:          /* More input data needed. */
        પાછા cursize;
      સ્થિતિ 0:           /* No encoding needed. */
        તોડો;
      મૂળભૂત:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        તોડો;
      }
      તોડો;
    સ્થિતિ QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      પસંદગી(qp_lookahead_eol(st, ateof, 0)) {
      સ્થિતિ -1:          /* Need more data. */
        પાછા cursize;
      સ્થિતિ 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        તોડો;
      મૂળભૂત:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        તોડો;
      }
      તોડો;
    મૂળભૂત:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      તોડો;
    }

    /* Be sure the encoded character fits within maximum line length. */
    જો(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_લીટી_LENGTH;
      જો(!softlinebreak && st->pos + len == MAX_ENCODED_લીટી_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        પસંદગી(qp_lookahead_eol(st, ateof, consumed)) {
        સ્થિતિ -1:        /* Need more data. */
          પાછા cursize;
        સ્થિતિ 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          તોડો;
        }
      }
      જો(softlinebreak) {
        curlx_strcopy(buf, કદ(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    જો(len > size) {
      જો(!cursize)
        પાછા STOP_FILLING;
      તોડો;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    જો(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  પાછા cursize;
}

સ્થિર curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  પાછા part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
સ્થિર size_t mime_mem_read(અક્ષર *buffer, size_t size, size_t nitems,
                            શૂન્ય *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (શૂન્ય)size;  /* Always 1 */

  જો(!nitems)
    પાછા STOP_FILLING;

  જો(sz > nitems)
    sz = nitems;

  જો(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  પાછા sz;
}

સ્થિર પૂર્ણાંક mime_mem_seek(શૂન્ય *instream, curl_off_t offset, પૂર્ણાંક whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  પસંદગી(whence) {
  સ્થિતિ SEEK_CUR:
    offset += part->state.offset;
    તોડો;
  સ્થિતિ SEEK_END:
    offset += part->datasize;
    તોડો;
  }

  જો(offset < 0 || offset > part->datasize)
    પાછા CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  પાછા CURL_SEEKFUNC_OK;
}

સ્થિર શૂન્ય mime_mem_free(શૂન્ય *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
સ્થિર bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  જો(part->fp)
    પાછા FALSE;
  part->fp = fopen_read(part->data, "rb");
  પાછા part->fp ? FALSE : TRUE;
}

સ્થિર size_t mime_file_read(અક્ષર *buffer, size_t size, size_t nitems,
                             શૂન્ય *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  જો(!nitems)
    પાછા STOP_FILLING;

  જો(mime_open_file(part))
    પાછા READ_ERROR;

  પાછા fread(buffer, size, nitems, part->fp);
}

સ્થિર પૂર્ણાંક mime_file_seek(શૂન્ય *instream, curl_off_t offset, પૂર્ણાંક whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  જો(whence == SEEK_SET && !offset && !part->fp)
    પાછા CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  જો(mime_open_file(part))
    પાછા CURL_SEEKFUNC_FAIL;

  પાછા curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

સ્થિર શૂન્ય mime_file_free(શૂન્ય *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  જો(part->fp) {
    curlx_fclose(part->fp);
    part->fp = ખાલી;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
સ્થિર size_t readback_bytes(સંરચના mime_state *state,
                             અક્ષર *buffer, size_t bufsize,
                             અચલ અક્ષર *bytes, size_t numbytes,
                             અચલ અક્ષર *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  જો(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  નહીંતર {
    sz = offset - numbytes;
    જો(sz >= traillen)
      પાછા 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  જો(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  પાછા sz;
}

/* Read a non-encoded part content. */

સ્થિર size_t read_part_content(curl_mimepart *part, અક્ષર *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  પસંદગી(part->lastreadstatus) {
  સ્થિતિ 0:
  સ્થિતિ CURL_READFUNC_ABORT:
  સ્થિતિ CURL_READFUNC_PAUSE:
  સ્થિતિ READ_ERROR:
    પાછા part->lastreadstatus;
  મૂળભૂત:
    તોડો;
  }

  /* If we can determine we are at end of part data, spare a read. */
  જો(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  નહીંતર {
    પસંદગી(part->kind) {
    સ્થિતિ MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      તોડો;
    સ્થિતિ MIMEKIND_FILE:
      જો(part->fp && feof(part->fp))
        તોડો;  /* At EOF. */
      FALLTHROUGH();
    મૂળભૂત:
      જો(part->readfunc) {
        જો(!(part->flags & MIME_FAST_READ)) {
          જો(*hasread)
            પાછા STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      તોડો;
    }
  }

  પસંદગી(sz) {
  સ્થિતિ STOP_FILLING:
    તોડો;
  સ્થિતિ 0:
  સ્થિતિ CURL_READFUNC_ABORT:
  સ્થિતિ CURL_READFUNC_PAUSE:
  સ્થિતિ READ_ERROR:
    part->lastreadstatus = sz;
    તોડો;
  મૂળભૂત:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    તોડો;
  }

  પાછા sz;
}

/* Read and encode part content. */
સ્થિર size_t read_encoded_part_content(curl_mimepart *part, અક્ષર *buffer,
                                        size_t bufsize, bool *hasread)
{
  સંરચના mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  માટે(;;) {
    જો(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      પસંદગી(sz) {
      સ્થિતિ 0:
        જો(ateof)
          પાછા cursize;
        તોડો;
      સ્થિતિ READ_ERROR:
      સ્થિતિ STOP_FILLING:
        પાછા cursize ? cursize : sz;
      મૂળભૂત:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        ચાલુ;
      }
    }

    /* We need more data in input buffer. */
    જો(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      જો(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    જો(st->bufend >= કદ(st->buf))
      પાછા cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           કદ(st->buf) - st->bufend, hasread);
    પસંદગી(sz) {
    સ્થિતિ 0:
      ateof = TRUE;
      તોડો;
    સ્થિતિ CURL_READFUNC_ABORT:
    સ્થિતિ CURL_READFUNC_PAUSE:
    સ્થિતિ READ_ERROR:
    સ્થિતિ STOP_FILLING:
      પાછા cursize ? cursize : sz;
    મૂળભૂત:
      st->bufend += sz;
      તોડો;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
સ્થિર size_t readback_part(curl_mimepart *part,
                            અક્ષર *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  જ્યારે(bufsize) {
    size_t sz = 0;
    સંરચના curl_slist *hdr = (સંરચના curl_slist *)part->state.ptr;
    પસંદગી(part->state.state) {
    સ્થિતિ MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      તોડો;
    સ્થિતિ MIMESTATE_USERHEADERS:
      જો(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, ખાલી);
        તોડો;
      }
      જો(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        તોડો;
      }
      FALLTHROUGH();
    સ્થિતિ MIMESTATE_CURLHEADERS:
      જો(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      નહીંતર {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        જો(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      તોડો;
    સ્થિતિ MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      જો(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, ખાલી);
      તોડો;
    સ્થિતિ MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, ખાલી);
      તોડો;
    સ્થિતિ MIMESTATE_CONTENT:
      જો(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      નહીંતર
        sz = read_part_content(part, buffer, bufsize, hasread);
      પસંદગી(sz) {
      સ્થિતિ 0:
        mimesetstate(&part->state, MIMESTATE_END, ખાલી);
        /* Try sparing open file descriptors. */
        જો(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = ખાલી;
        }
        FALLTHROUGH();
      સ્થિતિ CURL_READFUNC_ABORT:
      સ્થિતિ CURL_READFUNC_PAUSE:
      સ્થિતિ READ_ERROR:
      સ્થિતિ STOP_FILLING:
        પાછા cursize ? cursize : sz;
      }
      તોડો;
    સ્થિતિ MIMESTATE_END:
      પાછા cursize;
    મૂળભૂત:
      તોડો;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  પાછા cursize;
}

/* Readback from mime. Warning: not a read callback function. */
સ્થિર size_t mime_subparts_read(અક્ષર *buffer, size_t size, size_t nitems,
                                 શૂન્ય *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (શૂન્ય)size;  /* Always 1 */

  જ્યારે(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    પસંદગી(mime->state.state) {
    સ્થિતિ MIMESTATE_BEGIN:
    સ્થિતિ MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      તોડો;
    સ્થિતિ MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      જો(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      તોડો;
    સ્થિતિ MIMESTATE_BOUNDARY2:
      જો(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      નહીંતર
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      જો(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      તોડો;
    સ્થિતિ MIMESTATE_CONTENT:
      જો(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, ખાલી);
        તોડો;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      પસંદગી(sz) {
      સ્થિતિ CURL_READFUNC_ABORT:
      સ્થિતિ CURL_READFUNC_PAUSE:
      સ્થિતિ READ_ERROR:
      સ્થિતિ STOP_FILLING:
        પાછા cursize ? cursize : sz;
      સ્થિતિ 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        તોડો;
      }
      તોડો;
    સ્થિતિ MIMESTATE_END:
      પાછા cursize;
    મૂળભૂત:
      તોડો;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  પાછા cursize;
}

સ્થિર પૂર્ણાંક mime_part_rewind(curl_mimepart *part)
{
  પૂર્ણાંક res = CURL_SEEKFUNC_OK;
  ગણના mimestate targetstate = MIMESTATE_BEGIN;

  જો(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  જો(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    જો(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      પસંદગી(res) {
      સ્થિતિ CURL_SEEKFUNC_OK:
      સ્થિતિ CURL_SEEKFUNC_FAIL:
      સ્થિતિ CURL_SEEKFUNC_CANTSEEK:
        તોડો;
      સ્થિતિ -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        તોડો;
      મૂળભૂત:
        res = CURL_SEEKFUNC_FAIL;
        તોડો;
      }
    }
  }

  જો(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, ખાલી);

  part->lastreadstatus = 1; /* Successful read status. */
  પાછા res;
}

સ્થિર પૂર્ણાંક mime_subparts_seek(શૂન્ય *instream, curl_off_t offset, પૂર્ણાંક whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  પૂર્ણાંક rc = CURL_SEEKFUNC_OK;

  જો(whence != SEEK_SET || offset)
    પાછા CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  જો(mime->state.state == MIMESTATE_BEGIN)
    પાછા CURL_SEEKFUNC_OK;           /* Already rewound. */

  માટે(part = mime->firstpart; part; part = part->nextpart) {
    પૂર્ણાંક res = mime_part_rewind(part);
    જો(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  જો(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ખાલી);

  પાછા rc;
}

/* Release part content. */
સ્થિર શૂન્ય cleanup_part_content(curl_mimepart *part)
{
  જો(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = ખાલી;
  part->seekfunc = ખાલી;
  part->freefunc = ખાલી;
  part->arg = (શૂન્ય *)part;           /* Defaults to part itself. */
  part->data = ખાલી;
  part->fp = ખાલી;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned પૂર્ણાંક)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

સ્થિર શૂન્ય mime_subparts_free(શૂન્ય *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  જો(mime && mime->parent) {
    mime->parent->freefunc = ખાલી;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
સ્થિર શૂન્ય mime_subparts_unbind(શૂન્ય *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  જો(mime && mime->parent) {
    mime->parent->freefunc = ખાલી;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = ખાલી;
  }
}

શૂન્ય Curl_mime_cleanpart(curl_mimepart *part)
{
  જો(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    જો(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
શૂન્ય curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  જો(!mime)
    પાછા;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  જ્યારે(mime) {
    part = mime->firstpart;
    જો(part) {
      mime->firstpart = part->nextpart;
      જો(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = ખાલી;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    નહીંતર {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

સ્થિર bool mime_is_too_deep(અચલ curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  જો(++call_depth > MAX_MIME_LEVELS)
    પાછા TRUE;

  જો(part->kind == MIMEKIND_MULTIPART && part->arg) {
    અચલ curl_mime *mime = (અચલ curl_mime *)part->arg;
    અચલ curl_mimepart *s;

    માટે(s = mime->firstpart; s; s = s->nextpart) {
      જો(mime_is_too_deep(s, call_depth))
        પાછા TRUE;
    }
  }

  પાછા FALSE;
}

સ્થિર CURLcode mime_duppart(સંરચના Curl_easy *data,
                             curl_mimepart *dst, અચલ curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  અચલ curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  જો(++call_depth > MAX_MIME_LEVELS)
    પાછા CURLE_TOO_LARGE;

  /* Duplicate content. */
  પસંદગી(src->kind) {
  સ્થિતિ MIMEKIND_NONE:
    તોડો;
  સ્થિતિ MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    તોડો;
  સ્થિતિ MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    જો(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    તોડો;
  સ્થિતિ MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    તોડો;
  સ્થિતિ MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    માટે(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    તોડો;
  મૂળભૂત:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    તોડો;
  }

  /* Duplicate headers. */
  જો(!result && src->userheaders) {
    સંરચના curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    જો(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    નહીંતર {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      જો(result)
        curl_slist_free_all(hdrs);
    }
  }

  જો(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  જો(!result)
    result = curl_mime_name(dst, src->name);
  જો(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  જો(result)
    Curl_mime_cleanpart(dst);

  પાછા result;
}

CURLcode Curl_mime_duppart(સંરચના Curl_easy *data,
                           curl_mimepart *dst, અચલ curl_mimepart *src)
{
  જો(!dst || !src)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(mime_is_too_deep(src, 0))
    પાછા CURLE_TOO_LARGE;

  પાછા mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(શૂન્ય *easy)
{
  curl_mime *mime = curlx_malloc(કદ(*mime));

  જો(mime) {
    mime->parent = ખાલી;
    mime->firstpart = ખાલી;
    mime->lastpart = ખાલી;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    જો(Curl_rand_alnum(easy,
                       (unsigned અક્ષર *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      પાછા ખાલી;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ખાલી);
  }

  પાછા mime;
}

/* Initialize a mime part. */
શૂન્ય Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, કદ(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, ખાલી);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  જો(!mime)
    પાછા ખાલી;

  part = curlx_malloc(કદ(*part));

  જો(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    જો(mime->lastpart)
      mime->lastpart->nextpart = part;
    નહીંતર
      mime->firstpart = part;

    mime->lastpart = part;
  }

  પાછા part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, અચલ અક્ષર *name)
{
  જો(!part)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  જો(name) {
    part->name = curlx_strdup(name);
    જો(!part->name)
      પાછા CURLE_OUT_OF_MEMORY;
  }

  પાછા CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, અચલ અક્ષર *filename)
{
  જો(!part)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  જો(filename) {
    part->filename = curlx_strdup(filename);
    જો(!part->filename)
      પાછા CURLE_OUT_OF_MEMORY;
  }

  પાછા CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, અચલ અક્ષર *data, size_t datasize)
{
  જો(!part)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  જો(data) {
    જો(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    જો(!part->data)
      પાછા CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  પાછા CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, અચલ અક્ષર *filename)
{
  CURLcode result = CURLE_OK;

  જો(!part)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  જો(filename) {
    અક્ષર *base;
    curlx_struct_stat sbuf;

    જો(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    નહીંતર {
      part->data = curlx_strdup(filename);
      જો(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      નહીંતર {
        part->datasize = -1;
        જો(S_ISREG(sbuf.st_mode)) {
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
        જો(!base)
          result = CURLE_OUT_OF_MEMORY;
        નહીંતર {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  પાછા result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, અચલ અક્ષર *mimetype)
{
  જો(!part)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  જો(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    જો(!part->mimetype)
      પાછા CURLE_OUT_OF_MEMORY;
  }

  પાછા CURLE_OK;
}

સ્થિર અચલ સંરચના mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, અચલ અક્ષર *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  અચલ સંરચના mime_encoder *mep;

  જો(!part)
    પાછા result;

  part->encoder = ખાલી;

  જો(!encoding)
    પાછા CURLE_OK;    /* Removing current encoder. */

  માટે(mep = encoders; mep->name; mep++)
    જો(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  પાછા result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           સંરચના curl_slist *headers, પૂર્ણાંક take_ownership)
{
  જો(!part)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(part->flags & MIME_USERHEADERS_OWNER) {
    જો(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned પૂર્ણાંક)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  જો(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  પાછા CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, શૂન્ય *arg)
{
  જો(!part)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  જો(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  પાછા CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, પૂર્ણાંક take_ownership)
{
  curl_mime *મૂળ;

  જો(!part)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  જો(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    પાછા CURLE_OK;

  cleanup_part_content(part);

  જો(subparts) {
    /* Should not have been attached already. */
    જો(subparts->parent)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    મૂળ = part->parent;
    જો(મૂળ) {
      જ્યારે(મૂળ->parent && મૂળ->parent->parent)
        મૂળ = મૂળ->parent->parent;
      જો(subparts == મૂળ) {
        /* cannot add as a subpart of itself. */
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    જો(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      પાછા CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  પાછા CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  પાછા Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(અક્ષર *buffer, size_t size, size_t nitems, શૂન્ય *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (શૂન્ય)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  કરો {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } જ્યારે(ret == STOP_FILLING);

  પાછા ret;
}

/* Rewind mime stream. */
સ્થિર CURLcode mime_rewind(curl_mimepart *part)
{
  પાછા mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
સ્થિર size_t slist_size(સંરચના curl_slist *s,
                         size_t overhead, અચલ અક્ષર *skip, size_t skiplen)
{
  size_t size = 0;

  માટે(; s; s = s->next)
    જો(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  પાછા size;
}

/* Get/compute multipart size. */
સ્થિર curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  જો(!mime)
    પાછા 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  માટે(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    જો(sz < 0)
      size = sz;

    જો(size >= 0)
      size += boundarysize + sz;
  }

  પાછા size;
}

/* Get/compute mime size. */
સ્થિર curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  જો(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  જો(part->encoder)
    size = part->encoder->sizefunc(part);

  જો(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, ખાલી, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  પાછા size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(સંરચના curl_slist **slp, અચલ અક્ષર *fmt, ...)
{
  સંરચના curl_slist *hdr = ખાલી;
  અક્ષર *s = ખાલી;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  જો(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    જો(hdr)
      *slp = hdr;
    નહીંતર
      curlx_free(s);
  }

  પાછા hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
સ્થિર CURLcode add_content_type(સંરચના curl_slist **slp,
                                 અચલ અક્ષર *type, અચલ અક્ષર *boundary)
{
  પાછા Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

અચલ અક્ષર *Curl_mime_contenttype(અચલ અક્ષર *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  સંરચના ContentType {
    અચલ અક્ષર *extension;
    અચલ અક્ષર *type;
  };
  સ્થિર અચલ સંરચના ContentType ctts[] = {
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

  જો(filename) {
    size_t len1 = strlen(filename);
    અચલ અક્ષર *nameend = filename + len1;
    unsigned પૂર્ણાંક i;

    માટે(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      જો(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        પાછા ctts[i].type;
    }
  }
  પાછા ખાલી;
}

સ્થિર bool content_type_match(અચલ અક્ષર *contenttype,
                               અચલ અક્ષર *target, size_t len)
{
  જો(contenttype && curl_strnequal(contenttype, target, len))
    પસંદગી(contenttype[len]) {
    સ્થિતિ '\0':
    સ્થિતિ '\t':
    સ્થિતિ '\r':
    સ્થિતિ '\n':
    સ્થિતિ ' ':
    સ્થિતિ ';':
      પાછા TRUE;
    }
  પાછા FALSE;
}

સ્થિર CURLcode add_content_disposition(સંરચના Curl_easy *data,
                                        curl_mimepart *part,
                                        અચલ અક્ષર *disposition,
                                        અચલ અક્ષર *contenttype,
                                        ગણના mimestrategy strategy)
{
  જો(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  જો(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = ખાલી;

  જો(disposition) {
    CURLcode result = CURLE_OK;
    અક્ષર *name = ખાલી;
    અક્ષર *filename = ખાલી;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    જો(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;

    જો(part->name) {
      name = escape_string(data, part->name, strategy);
      જો(!name)
        પાછા CURLE_OUT_OF_MEMORY;
    }
    જો(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      જો(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    જો(!result)
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
    જો(result)
      પાછા result;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode mime_prepare_headers(સંરચના Curl_easy *data,
                                     curl_mimepart *part,
                                     અચલ અક્ષર *contenttype,
                                     અચલ અક્ષર *disposition,
                                     ગણના mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = ખાલી;
  અચલ અક્ષર *boundary = ખાલી;
  અક્ષર *customct;
  અચલ અક્ષર *cte = ખાલી;
  CURLcode result = CURLE_OK;

  જો(++call_depth > MAX_MIME_LEVELS)
    પાછા CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = ખાલી;

  /* Be sure we will not access old headers later. */
  જો(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, ખાલી);

  /* Check if content type is specified. */
  customct = part->mimetype;
  જો(!customct)
    customct = શોધ_header(part->userheaders, STRCONST("Content-Type"));
  જો(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  જો(!contenttype) {
    પસંદગી(part->kind) {
    સ્થિતિ MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      તોડો;
    સ્થિતિ MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      જો(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      જો(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      તોડો;
    મૂળભૂત:
      contenttype = Curl_mime_contenttype(part->filename);
      તોડો;
    }
  }

  જો(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    જો(mime)
      boundary = mime->boundary;
  }
  નહીંતર જો(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = ખાલી;

  /* Issue content-disposition header only if not already set by caller. */
  જો(!શોધ_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    જો(result)
      પાછા result;
  }

  /* Issue Content-Type header. */
  જો(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    જો(result)
      પાછા result;
  }

  /* Content-Transfer-Encoding header. */
  જો(!શોધ_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    જો(part->encoder)
      cte = part->encoder->name;
    નહીંતર જો(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    જો(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      જો(result)
        પાછા result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  જો(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  જો(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = ખાલી;
    જો(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    માટે(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, ખાલી,
                                    disposition, strategy, call_depth);
      જો(result)
        પાછા result;
    }
  }
  પાછા result;
}

CURLcode Curl_mime_prepare_headers(સંરચના Curl_easy *data,
                                   curl_mimepart *part,
                                   અચલ અક્ષર *contenttype,
                                   અચલ અક્ષર *disposition,
                                   ગણના mimestrategy strategy)
{
  પાછા mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
સ્થિર શૂન્ય mime_unpause(curl_mimepart *part)
{
  જો(part) {
    જો(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    જો(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      જો(mime) {
        curl_mimepart *subpart;

        માટે(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

સંરચના cr_mime_ctx {
  સંરચના Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  સંરચના bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

સ્થિર CURLcode cr_mime_init(સંરચના Curl_easy *data,
                             સંરચના Curl_creader *reader)
{
  સંરચના cr_mime_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cr_mime_close(સંરચના Curl_easy *data,
                          સંરચના Curl_creader *reader)
{
  સંરચના cr_mime_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
સ્થિર CURLcode cr_mime_read(સંરચના Curl_easy *data,
                             સંરચના Curl_creader *reader,
                             અક્ષર *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  સંરચના cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  અક્ષર tmp[256];

  /* Once we have errored, we will return the same error forever */
  જો(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (પૂર્ણાંક)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    પાછા ctx->error_result;
  }
  જો(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    પાછા CURLE_OK;
  }
  /* respect length limitations */
  જો(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    જો(remain <= 0)
      blen = 0;
    નહીંતર જો(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  જો(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned અક્ષર *)buf, blen, &nread);
    જો(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      પાછા result;
    }
  }
  નહીંતર જો(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, કદ(tmp), ctx->part);
    જો(nread <= કદ(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned અક્ષર *)tmp, nread, &n);
      જો(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        પાછા result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      જો(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        પાછા result;
      }
    }
  }
  નહીંતર
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  પસંદગી(nread) {
  સ્થિતિ 0:
    જો((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      પાછા CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    તોડો;

  સ્થિતિ CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    તોડો;

  સ્થિતિ CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    તોડો; /* nothing was read */

  સ્થિતિ STOP_FILLING:
  સ્થિતિ READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    તોડો;

  મૂળભૂત:
    જો(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      પાછા CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    જો(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    તોડો;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (પૂર્ણાંક)result, *pnread, *peos);
  પાછા result;
}

સ્થિર bool cr_mime_needs_rewind(સંરચના Curl_easy *data,
                                 સંરચના Curl_creader *reader)
{
  સંરચના cr_mime_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પાછા ctx->read_len > 0;
}

સ્થિર curl_off_t cr_mime_total_length(સંરચના Curl_easy *data,
                                       સંરચના Curl_creader *reader)
{
  સંરચના cr_mime_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પાછા ctx->total_len;
}

સ્થિર CURLcode cr_mime_resume_from(સંરચના Curl_easy *data,
                                    સંરચના Curl_creader *reader,
                                    curl_off_t offset)
{
  સંરચના cr_mime_ctx *ctx = reader->ctx;

  જો(offset > 0) {
    curl_off_t passed = 0;

    કરો {
      અક્ષર scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)કદ(scratch)) ?
        કદ(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      જો((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        પાછા CURLE_READ_ERROR;
      }
    } જ્યારે(passed < offset);

    /* now, decrease the size of the read */
    જો(ctx->total_len > 0) {
      ctx->total_len -= offset;

      જો(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        પાછા CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode cr_mime_cntrl(સંરચના Curl_easy *data,
                              સંરચના Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  સંરચના cr_mime_ctx *ctx = reader->ctx;
  પસંદગી(opcode) {
  સ્થિતિ CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    જો(result)
      failf(data, "Cannot rewind mime/post data");
    પાછા result;
  }
  સ્થિતિ CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    તોડો;
  સ્થિતિ CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર bool cr_mime_is_paused(સંરચના Curl_easy *data,
                              સંરચના Curl_creader *reader)
{
  સંરચના cr_mime_ctx *ctx = reader->ctx;
  (શૂન્ય)data;
  પાછા ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

સ્થિર અચલ સંરચના Curl_crtype cr_mime = {
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
  કદ(સંરચના cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(સંરચના Curl_easy *data, curl_mimepart *part)
{
  સંરચના Curl_creader *r;
  સંરચના cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  જો(result)
    પાછા result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  જો(result) {
    Curl_creader_free(data, r);
    પાછા result;
  }
  ctx->total_len = mime_size(ctx->part);

  પાછા Curl_creader_set(data, r);
}

#નહીંતર /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (શૂન્ય)easy;
  પાછા ખાલી;
}

શૂન્ય curl_mime_free(curl_mime *mime)
{
  (શૂન્ય)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (શૂન્ય)mime;
  પાછા ખાલી;
}

CURLcode curl_mime_name(curl_mimepart *part, અચલ અક્ષર *name)
{
  (શૂન્ય)part;
  (શૂન્ય)name;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, અચલ અક્ષર *filename)
{
  (શૂન્ય)part;
  (શૂન્ય)filename;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, અચલ અક્ષર *mimetype)
{
  (શૂન્ય)part;
  (શૂન્ય)mimetype;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, અચલ અક્ષર *encoding)
{
  (શૂન્ય)part;
  (શૂન્ય)encoding;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, અચલ અક્ષર *data, size_t datasize)
{
  (શૂન્ય)part;
  (શૂન્ય)data;
  (શૂન્ય)datasize;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, અચલ અક્ષર *filename)
{
  (શૂન્ય)part;
  (શૂન્ય)filename;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           શૂન્ય *arg)
{
  (શૂન્ય)part;
  (શૂન્ય)datasize;
  (શૂન્ય)readfunc;
  (શૂન્ય)seekfunc;
  (શૂન્ય)freefunc;
  (શૂન્ય)arg;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (શૂન્ય)part;
  (શૂન્ય)subparts;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           સંરચના curl_slist *headers, પૂર્ણાંક take_ownership)
{
  (શૂન્ય)part;
  (શૂન્ય)headers;
  (શૂન્ય)take_ownership;
  પાછા CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(સંરચના curl_slist **slp, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)slp;
  (શૂન્ય)fmt;
  પાછા CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

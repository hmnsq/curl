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
#அந்தர்பாவ "curl_setup.h"

கட்டமைப்பு Curl_easy;

#அந்தர்பாவ "mime.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/basename.h"
#அந்தர்பாவ "curlx/strcopy.h"
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "curlx/base64.h"

#எனில் !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "slist.h"
#அந்தர்பாவ "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

நிலையான size_t mime_subparts_read(எழுத்து *buffer, size_t size, size_t nitems,
                                 வெறுமை *instream, bool *hasread);
நிலையான curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
நிலையான மாறிலி unsigned எழுத்து qp_class[] = {
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
நிலையான மாறிலி எழுத்து aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#இல்லையெனில்

#அந்தர்பாவ <fabdef.h>
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
curl_off_t VmsRealFileSize(மாறிலி எழுத்து *name, மாறிலி curlx_struct_stat *stat_buf)
{
  எழுத்து buffer[8192];
  curl_off_t மொத்தம்;
  முழு ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  எனில்(!file)
    திரும்பு 0;

  மொத்தம் = 0;
  ret_stat = 1;
  போது(ret_stat > 0) {
    ret_stat = fread(buffer, 1, அளவு(buffer), file);
    எனில்(ret_stat)
      மொத்தம் += ret_stat;
  }
  curlx_fclose(file);

  திரும்பு மொத்தம்;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
நிலையான curl_off_t VmsSpecialSize(மாறிலி எழுத்து *name,
                                 மாறிலி curlx_struct_stat *stat_buf)
{
  தேர்வு(stat_buf->st_fab_rfm) {
  நிலை FAB$C_VAR:
  நிலை FAB$C_VFC:
    திரும்பு VmsRealFileSize(name, stat_buf);
    நிறுத்து;
  இயல்பு:
    திரும்பு stat_buf->st_size;
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
நிலையான FILE *vmsfopenread(மாறிலி எழுத்து *file, மாறிலி எழுத்து *mode)
{
  curlx_struct_stat statbuf;
  முழு res = curlx_stat(file, &statbuf);

  எனில்(res != -1) {
    தேர்வு(statbuf.st_fab_rfm) {
    நிலை FAB$C_VAR:
    நிலை FAB$C_VFC:
    நிலை FAB$C_STMCR:
      திரும்பு curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      நிறுத்து;
    இயல்பு:
      திரும்பு curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  திரும்பு வெற்று;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
நிலையான வெறுமை mimesetstate(கட்டமைப்பு mime_state *state,
                         எண்ணிக்கை mimestate tok, வெறுமை *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
நிலையான எழுத்து *escape_string(கட்டமைப்பு Curl_easy *data,
                           மாறிலி எழுத்து *src, எண்ணிக்கை mimestrategy strategy)
{
  CURLcode result;
  கட்டமைப்பு dynbuf db;
  மாறிலி எழுத்து * மாறிலி *table;
  மாறிலி எழுத்து * மாறிலி *p;
  /* replace first character by rest of string. */
  நிலையான மாறிலி எழுத்து * மாறிலி mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    வெற்று
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  நிலையான மாறிலி எழுத்து * மாறிலி formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    வெற்று
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  எனில்(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_உள்ளீடு_LENGTH);

  க்கு(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    க்கு(p = table; *p && **p != *src; p++)
      ;

    எனில்(*p)
      result = curlx_dyn_add(&db, *p + 1);
    இல்லையெனில்
      result = curlx_dyn_addn(&db, src, 1);
  }

  திரும்பு curlx_dyn_ptr(&db);
}

/* Check if header matches. */
நிலையான எழுத்து *match_header(கட்டமைப்பு curl_slist *hdr, மாறிலி எழுத்து *lbl, size_t len)
{
  எழுத்து *மதிப்பு = வெற்று;

  எனில்(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    க்கு(மதிப்பு = hdr->data + len + 1; *மதிப்பு == ' '; மதிப்பு++)
      ;
  திரும்பு மதிப்பு;
}

/* Get a header from an slist. */
நிலையான எழுத்து *தேடல்_header(கட்டமைப்பு curl_slist *hdrlist,
                           மாறிலி எழுத்து *hdr, size_t len)
{
  எழுத்து *மதிப்பு = வெற்று;

  க்கு(; !மதிப்பு && hdrlist; hdrlist = hdrlist->next)
    மதிப்பு = match_header(hdrlist, hdr, len);

  திரும்பு மதிப்பு;
}

நிலையான எழுத்து *strippath(மாறிலி எழுத்து *fullfile)
{
  எழுத்து *filename;
  எழுத்து *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  எனில்(!filename)
    திரும்பு வெற்று;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  திரும்பு base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
நிலையான வெறுமை cleanup_encoder_state(கட்டமைப்பு mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
நிலையான size_t encoder_nop_read(எழுத்து *buffer, size_t size, bool ateof,
                               கட்டமைப்பு curl_mimepart *part)
{
  கட்டமைப்பு mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (வெறுமை)ateof;

  எனில்(!size)
    திரும்பு STOP_FILLING;

  எனில்(size > insize)
    size = insize;

  எனில்(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  திரும்பு size;
}

நிலையான curl_off_t encoder_nop_size(curl_mimepart *part)
{
  திரும்பு part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
நிலையான size_t encoder_7bit_read(எழுத்து *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  கட்டமைப்பு mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (வெறுமை)ateof;

  எனில்(!size)
    திரும்பு STOP_FILLING;

  எனில்(size > cursize)
    size = cursize;

  க்கு(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    எனில்(*buffer++ & 0x80)
      திரும்பு cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  திரும்பு cursize;
}

/* Base64 content encoder. */
நிலையான size_t encoder_base64_read(எழுத்து *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  கட்டமைப்பு mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  முழு i;
  எழுத்து *ptr = buffer;

  போது(st->bufbeg < st->bufend) {
    /* Line full ? */
    எனில்(st->pos > MAX_ENCODED_வரி_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      எனில்(size < 2) {
        எனில்(!cursize)
          திரும்பு STOP_FILLING;
        நிறுத்து;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    எனில்(size < 4) {
      எனில்(!cursize)
        திரும்பு STOP_FILLING;
      நிறுத்து;
    }
    எனில்(st->bufend - st->bufbeg < 3)
      நிறுத்து;

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
  எனில்(ateof) {
    எனில்(size < 4) {
      எனில்(!cursize)
        திரும்பு STOP_FILLING;
    }
    இல்லையெனில் {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      எனில்(st->bufend != st->bufbeg) {

        எனில்(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        எனில்(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  திரும்பு cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_உள்ளீடு_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_வரி_LENGTH + 2)) *                  \
    MAX_ENCODED_வரி_LENGTH / 4) * 3 - 3)

நிலையான curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  எனில்(size <= 0)
    திரும்பு size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  எனில்(size > BASE64_MAX_உள்ளீடு_SIZE)
    திரும்பு -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  திரும்பு size + (2 * ((size - 1) / MAX_ENCODED_வரி_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
நிலையான முழு qp_lookahead_eol(கட்டமைப்பு mime_encoder_state *st, முழு ateof, size_t n)
{
  n += st->bufbeg;
  எனில்(n >= st->bufend && ateof)
    திரும்பு 1;
  எனில்(n + 2 > st->bufend)
    திரும்பு ateof ? 0 : -1;
  எனில்(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    திரும்பு 1;
  திரும்பு 0;
}

/* Quoted-printable encoder. */
நிலையான size_t encoder_qp_read(எழுத்து *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  கட்டமைப்பு mime_encoder_state *st = &part->encstate;
  எழுத்து *ptr = buffer;
  size_t cursize = 0;
  முழு softlinebreak;
  எழுத்து buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  போது(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    முழு i = (unsigned எழுத்து)st->buf[st->bufbeg];
    buf[0] = (எழுத்து)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    தேர்வு(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    நிலை QP_OK:          /* Not a special character. */
      நிறுத்து;
    நிலை QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      தேர்வு(qp_lookahead_eol(st, ateof, 1)) {
      நிலை -1:          /* More input data needed. */
        திரும்பு cursize;
      நிலை 0:           /* No encoding needed. */
        நிறுத்து;
      இயல்பு:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        நிறுத்து;
      }
      நிறுத்து;
    நிலை QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      தேர்வு(qp_lookahead_eol(st, ateof, 0)) {
      நிலை -1:          /* Need more data. */
        திரும்பு cursize;
      நிலை 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        நிறுத்து;
      இயல்பு:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        நிறுத்து;
      }
      நிறுத்து;
    இயல்பு:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      நிறுத்து;
    }

    /* Be sure the encoded character fits within maximum line length. */
    எனில்(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_வரி_LENGTH;
      எனில்(!softlinebreak && st->pos + len == MAX_ENCODED_வரி_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        தேர்வு(qp_lookahead_eol(st, ateof, consumed)) {
        நிலை -1:        /* Need more data. */
          திரும்பு cursize;
        நிலை 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          நிறுத்து;
        }
      }
      எனில்(softlinebreak) {
        curlx_strcopy(buf, அளவு(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    எனில்(len > size) {
      எனில்(!cursize)
        திரும்பு STOP_FILLING;
      நிறுத்து;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    எனில்(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  திரும்பு cursize;
}

நிலையான curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  திரும்பு part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
நிலையான size_t mime_mem_read(எழுத்து *buffer, size_t size, size_t nitems,
                            வெறுமை *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (வெறுமை)size;  /* Always 1 */

  எனில்(!nitems)
    திரும்பு STOP_FILLING;

  எனில்(sz > nitems)
    sz = nitems;

  எனில்(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  திரும்பு sz;
}

நிலையான முழு mime_mem_seek(வெறுமை *instream, curl_off_t offset, முழு whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  தேர்வு(whence) {
  நிலை SEEK_CUR:
    offset += part->state.offset;
    நிறுத்து;
  நிலை SEEK_END:
    offset += part->datasize;
    நிறுத்து;
  }

  எனில்(offset < 0 || offset > part->datasize)
    திரும்பு CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  திரும்பு CURL_SEEKFUNC_OK;
}

நிலையான வெறுமை mime_mem_free(வெறுமை *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
நிலையான bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  எனில்(part->fp)
    திரும்பு FALSE;
  part->fp = fopen_read(part->data, "rb");
  திரும்பு part->fp ? FALSE : TRUE;
}

நிலையான size_t mime_file_read(எழுத்து *buffer, size_t size, size_t nitems,
                             வெறுமை *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  எனில்(!nitems)
    திரும்பு STOP_FILLING;

  எனில்(mime_open_file(part))
    திரும்பு READ_ERROR;

  திரும்பு fread(buffer, size, nitems, part->fp);
}

நிலையான முழு mime_file_seek(வெறுமை *instream, curl_off_t offset, முழு whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  எனில்(whence == SEEK_SET && !offset && !part->fp)
    திரும்பு CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  எனில்(mime_open_file(part))
    திரும்பு CURL_SEEKFUNC_FAIL;

  திரும்பு curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

நிலையான வெறுமை mime_file_free(வெறுமை *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  எனில்(part->fp) {
    curlx_fclose(part->fp);
    part->fp = வெற்று;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
நிலையான size_t readback_bytes(கட்டமைப்பு mime_state *state,
                             எழுத்து *buffer, size_t bufsize,
                             மாறிலி எழுத்து *bytes, size_t numbytes,
                             மாறிலி எழுத்து *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  எனில்(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  இல்லையெனில் {
    sz = offset - numbytes;
    எனில்(sz >= traillen)
      திரும்பு 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  எனில்(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  திரும்பு sz;
}

/* Read a non-encoded part content. */

நிலையான size_t read_part_content(curl_mimepart *part, எழுத்து *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  தேர்வு(part->lastreadstatus) {
  நிலை 0:
  நிலை CURL_READFUNC_ABORT:
  நிலை CURL_READFUNC_PAUSE:
  நிலை READ_ERROR:
    திரும்பு part->lastreadstatus;
  இயல்பு:
    நிறுத்து;
  }

  /* If we can determine we are at end of part data, spare a read. */
  எனில்(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  இல்லையெனில் {
    தேர்வு(part->kind) {
    நிலை MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      நிறுத்து;
    நிலை MIMEKIND_FILE:
      எனில்(part->fp && feof(part->fp))
        நிறுத்து;  /* At EOF. */
      FALLTHROUGH();
    இயல்பு:
      எனில்(part->readfunc) {
        எனில்(!(part->flags & MIME_FAST_READ)) {
          எனில்(*hasread)
            திரும்பு STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      நிறுத்து;
    }
  }

  தேர்வு(sz) {
  நிலை STOP_FILLING:
    நிறுத்து;
  நிலை 0:
  நிலை CURL_READFUNC_ABORT:
  நிலை CURL_READFUNC_PAUSE:
  நிலை READ_ERROR:
    part->lastreadstatus = sz;
    நிறுத்து;
  இயல்பு:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    நிறுத்து;
  }

  திரும்பு sz;
}

/* Read and encode part content. */
நிலையான size_t read_encoded_part_content(curl_mimepart *part, எழுத்து *buffer,
                                        size_t bufsize, bool *hasread)
{
  கட்டமைப்பு mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  க்கு(;;) {
    எனில்(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      தேர்வு(sz) {
      நிலை 0:
        எனில்(ateof)
          திரும்பு cursize;
        நிறுத்து;
      நிலை READ_ERROR:
      நிலை STOP_FILLING:
        திரும்பு cursize ? cursize : sz;
      இயல்பு:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        தொடர்;
      }
    }

    /* We need more data in input buffer. */
    எனில்(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      எனில்(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    எனில்(st->bufend >= அளவு(st->buf))
      திரும்பு cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           அளவு(st->buf) - st->bufend, hasread);
    தேர்வு(sz) {
    நிலை 0:
      ateof = TRUE;
      நிறுத்து;
    நிலை CURL_READFUNC_ABORT:
    நிலை CURL_READFUNC_PAUSE:
    நிலை READ_ERROR:
    நிலை STOP_FILLING:
      திரும்பு cursize ? cursize : sz;
    இயல்பு:
      st->bufend += sz;
      நிறுத்து;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
நிலையான size_t readback_part(curl_mimepart *part,
                            எழுத்து *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  போது(bufsize) {
    size_t sz = 0;
    கட்டமைப்பு curl_slist *hdr = (கட்டமைப்பு curl_slist *)part->state.ptr;
    தேர்வு(part->state.state) {
    நிலை MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      நிறுத்து;
    நிலை MIMESTATE_USERHEADERS:
      எனில்(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, வெற்று);
        நிறுத்து;
      }
      எனில்(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        நிறுத்து;
      }
      FALLTHROUGH();
    நிலை MIMESTATE_CURLHEADERS:
      எனில்(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      இல்லையெனில் {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        எனில்(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      நிறுத்து;
    நிலை MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      எனில்(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, வெற்று);
      நிறுத்து;
    நிலை MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, வெற்று);
      நிறுத்து;
    நிலை MIMESTATE_CONTENT:
      எனில்(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      இல்லையெனில்
        sz = read_part_content(part, buffer, bufsize, hasread);
      தேர்வு(sz) {
      நிலை 0:
        mimesetstate(&part->state, MIMESTATE_END, வெற்று);
        /* Try sparing open file descriptors. */
        எனில்(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = வெற்று;
        }
        FALLTHROUGH();
      நிலை CURL_READFUNC_ABORT:
      நிலை CURL_READFUNC_PAUSE:
      நிலை READ_ERROR:
      நிலை STOP_FILLING:
        திரும்பு cursize ? cursize : sz;
      }
      நிறுத்து;
    நிலை MIMESTATE_END:
      திரும்பு cursize;
    இயல்பு:
      நிறுத்து;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  திரும்பு cursize;
}

/* Readback from mime. Warning: not a read callback function. */
நிலையான size_t mime_subparts_read(எழுத்து *buffer, size_t size, size_t nitems,
                                 வெறுமை *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (வெறுமை)size;  /* Always 1 */

  போது(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    தேர்வு(mime->state.state) {
    நிலை MIMESTATE_BEGIN:
    நிலை MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      நிறுத்து;
    நிலை MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      எனில்(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      நிறுத்து;
    நிலை MIMESTATE_BOUNDARY2:
      எனில்(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      இல்லையெனில்
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      எனில்(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      நிறுத்து;
    நிலை MIMESTATE_CONTENT:
      எனில்(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, வெற்று);
        நிறுத்து;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      தேர்வு(sz) {
      நிலை CURL_READFUNC_ABORT:
      நிலை CURL_READFUNC_PAUSE:
      நிலை READ_ERROR:
      நிலை STOP_FILLING:
        திரும்பு cursize ? cursize : sz;
      நிலை 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        நிறுத்து;
      }
      நிறுத்து;
    நிலை MIMESTATE_END:
      திரும்பு cursize;
    இயல்பு:
      நிறுத்து;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  திரும்பு cursize;
}

நிலையான முழு mime_part_rewind(curl_mimepart *part)
{
  முழு res = CURL_SEEKFUNC_OK;
  எண்ணிக்கை mimestate targetstate = MIMESTATE_BEGIN;

  எனில்(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  எனில்(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    எனில்(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      தேர்வு(res) {
      நிலை CURL_SEEKFUNC_OK:
      நிலை CURL_SEEKFUNC_FAIL:
      நிலை CURL_SEEKFUNC_CANTSEEK:
        நிறுத்து;
      நிலை -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        நிறுத்து;
      இயல்பு:
        res = CURL_SEEKFUNC_FAIL;
        நிறுத்து;
      }
    }
  }

  எனில்(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, வெற்று);

  part->lastreadstatus = 1; /* Successful read status. */
  திரும்பு res;
}

நிலையான முழு mime_subparts_seek(வெறுமை *instream, curl_off_t offset, முழு whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  முழு rc = CURL_SEEKFUNC_OK;

  எனில்(whence != SEEK_SET || offset)
    திரும்பு CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  எனில்(mime->state.state == MIMESTATE_BEGIN)
    திரும்பு CURL_SEEKFUNC_OK;           /* Already rewound. */

  க்கு(part = mime->firstpart; part; part = part->nextpart) {
    முழு res = mime_part_rewind(part);
    எனில்(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  எனில்(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, வெற்று);

  திரும்பு rc;
}

/* Release part content. */
நிலையான வெறுமை cleanup_part_content(curl_mimepart *part)
{
  எனில்(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = வெற்று;
  part->seekfunc = வெற்று;
  part->freefunc = வெற்று;
  part->arg = (வெறுமை *)part;           /* Defaults to part itself. */
  part->data = வெற்று;
  part->fp = வெற்று;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned முழு)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

நிலையான வெறுமை mime_subparts_free(வெறுமை *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  எனில்(mime && mime->parent) {
    mime->parent->freefunc = வெற்று;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
நிலையான வெறுமை mime_subparts_unbind(வெறுமை *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  எனில்(mime && mime->parent) {
    mime->parent->freefunc = வெற்று;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = வெற்று;
  }
}

வெறுமை Curl_mime_cleanpart(curl_mimepart *part)
{
  எனில்(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    எனில்(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
வெறுமை curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  எனில்(!mime)
    திரும்பு;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  போது(mime) {
    part = mime->firstpart;
    எனில்(part) {
      mime->firstpart = part->nextpart;
      எனில்(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = வெற்று;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    இல்லையெனில் {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

நிலையான bool mime_is_too_deep(மாறிலி curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  எனில்(++call_depth > MAX_MIME_LEVELS)
    திரும்பு TRUE;

  எனில்(part->kind == MIMEKIND_MULTIPART && part->arg) {
    மாறிலி curl_mime *mime = (மாறிலி curl_mime *)part->arg;
    மாறிலி curl_mimepart *s;

    க்கு(s = mime->firstpart; s; s = s->nextpart) {
      எனில்(mime_is_too_deep(s, call_depth))
        திரும்பு TRUE;
    }
  }

  திரும்பு FALSE;
}

நிலையான CURLcode mime_duppart(கட்டமைப்பு Curl_easy *data,
                             curl_mimepart *dst, மாறிலி curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  மாறிலி curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  எனில்(++call_depth > MAX_MIME_LEVELS)
    திரும்பு CURLE_TOO_LARGE;

  /* Duplicate content. */
  தேர்வு(src->kind) {
  நிலை MIMEKIND_NONE:
    நிறுத்து;
  நிலை MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    நிறுத்து;
  நிலை MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    எனில்(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    நிறுத்து;
  நிலை MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    நிறுத்து;
  நிலை MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    க்கு(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    நிறுத்து;
  இயல்பு:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    நிறுத்து;
  }

  /* Duplicate headers. */
  எனில்(!result && src->userheaders) {
    கட்டமைப்பு curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    எனில்(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    இல்லையெனில் {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      எனில்(result)
        curl_slist_free_all(hdrs);
    }
  }

  எனில்(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  எனில்(!result)
    result = curl_mime_name(dst, src->name);
  எனில்(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  எனில்(result)
    Curl_mime_cleanpart(dst);

  திரும்பு result;
}

CURLcode Curl_mime_duppart(கட்டமைப்பு Curl_easy *data,
                           curl_mimepart *dst, மாறிலி curl_mimepart *src)
{
  எனில்(!dst || !src)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(mime_is_too_deep(src, 0))
    திரும்பு CURLE_TOO_LARGE;

  திரும்பு mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(வெறுமை *easy)
{
  curl_mime *mime = curlx_malloc(அளவு(*mime));

  எனில்(mime) {
    mime->parent = வெற்று;
    mime->firstpart = வெற்று;
    mime->lastpart = வெற்று;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    எனில்(Curl_rand_alnum(easy,
                       (unsigned எழுத்து *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      திரும்பு வெற்று;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, வெற்று);
  }

  திரும்பு mime;
}

/* Initialize a mime part. */
வெறுமை Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, அளவு(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, வெற்று);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  எனில்(!mime)
    திரும்பு வெற்று;

  part = curlx_malloc(அளவு(*part));

  எனில்(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    எனில்(mime->lastpart)
      mime->lastpart->nextpart = part;
    இல்லையெனில்
      mime->firstpart = part;

    mime->lastpart = part;
  }

  திரும்பு part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, மாறிலி எழுத்து *name)
{
  எனில்(!part)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  எனில்(name) {
    part->name = curlx_strdup(name);
    எனில்(!part->name)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  திரும்பு CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, மாறிலி எழுத்து *filename)
{
  எனில்(!part)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  எனில்(filename) {
    part->filename = curlx_strdup(filename);
    எனில்(!part->filename)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  திரும்பு CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, மாறிலி எழுத்து *data, size_t datasize)
{
  எனில்(!part)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  எனில்(data) {
    எனில்(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    எனில்(!part->data)
      திரும்பு CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  திரும்பு CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, மாறிலி எழுத்து *filename)
{
  CURLcode result = CURLE_OK;

  எனில்(!part)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  எனில்(filename) {
    எழுத்து *base;
    curlx_struct_stat sbuf;

    எனில்(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    இல்லையெனில் {
      part->data = curlx_strdup(filename);
      எனில்(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      இல்லையெனில் {
        part->datasize = -1;
        எனில்(S_ISREG(sbuf.st_mode)) {
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
        எனில்(!base)
          result = CURLE_OUT_OF_MEMORY;
        இல்லையெனில் {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  திரும்பு result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, மாறிலி எழுத்து *mimetype)
{
  எனில்(!part)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  எனில்(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    எனில்(!part->mimetype)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  திரும்பு CURLE_OK;
}

நிலையான மாறிலி கட்டமைப்பு mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, மாறிலி எழுத்து *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  மாறிலி கட்டமைப்பு mime_encoder *mep;

  எனில்(!part)
    திரும்பு result;

  part->encoder = வெற்று;

  எனில்(!encoding)
    திரும்பு CURLE_OK;    /* Removing current encoder. */

  க்கு(mep = encoders; mep->name; mep++)
    எனில்(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  திரும்பு result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           கட்டமைப்பு curl_slist *headers, முழு take_ownership)
{
  எனில்(!part)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(part->flags & MIME_USERHEADERS_OWNER) {
    எனில்(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned முழு)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  எனில்(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  திரும்பு CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, வெறுமை *arg)
{
  எனில்(!part)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  எனில்(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  திரும்பு CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, முழு take_ownership)
{
  curl_mime *வேர்;

  எனில்(!part)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  எனில்(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    திரும்பு CURLE_OK;

  cleanup_part_content(part);

  எனில்(subparts) {
    /* Should not have been attached already. */
    எனில்(subparts->parent)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    வேர் = part->parent;
    எனில்(வேர்) {
      போது(வேர்->parent && வேர்->parent->parent)
        வேர் = வேர்->parent->parent;
      எனில்(subparts == வேர்) {
        /* cannot add as a subpart of itself. */
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    எனில்(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      திரும்பு CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  திரும்பு CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  திரும்பு Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(எழுத்து *buffer, size_t size, size_t nitems, வெறுமை *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (வெறுமை)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  செய் {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } போது(ret == STOP_FILLING);

  திரும்பு ret;
}

/* Rewind mime stream. */
நிலையான CURLcode mime_rewind(curl_mimepart *part)
{
  திரும்பு mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
நிலையான size_t slist_size(கட்டமைப்பு curl_slist *s,
                         size_t overhead, மாறிலி எழுத்து *skip, size_t skiplen)
{
  size_t size = 0;

  க்கு(; s; s = s->next)
    எனில்(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  திரும்பு size;
}

/* Get/compute multipart size. */
நிலையான curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  எனில்(!mime)
    திரும்பு 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  க்கு(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    எனில்(sz < 0)
      size = sz;

    எனில்(size >= 0)
      size += boundarysize + sz;
  }

  திரும்பு size;
}

/* Get/compute mime size. */
நிலையான curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  எனில்(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  எனில்(part->encoder)
    size = part->encoder->sizefunc(part);

  எனில்(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, வெற்று, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  திரும்பு size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(கட்டமைப்பு curl_slist **slp, மாறிலி எழுத்து *fmt, ...)
{
  கட்டமைப்பு curl_slist *hdr = வெற்று;
  எழுத்து *s = வெற்று;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  எனில்(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    எனில்(hdr)
      *slp = hdr;
    இல்லையெனில்
      curlx_free(s);
  }

  திரும்பு hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
நிலையான CURLcode add_content_type(கட்டமைப்பு curl_slist **slp,
                                 மாறிலி எழுத்து *type, மாறிலி எழுத்து *boundary)
{
  திரும்பு Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

மாறிலி எழுத்து *Curl_mime_contenttype(மாறிலி எழுத்து *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  கட்டமைப்பு ContentType {
    மாறிலி எழுத்து *extension;
    மாறிலி எழுத்து *type;
  };
  நிலையான மாறிலி கட்டமைப்பு ContentType ctts[] = {
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

  எனில்(filename) {
    size_t len1 = strlen(filename);
    மாறிலி எழுத்து *nameend = filename + len1;
    unsigned முழு i;

    க்கு(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      எனில்(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        திரும்பு ctts[i].type;
    }
  }
  திரும்பு வெற்று;
}

நிலையான bool content_type_match(மாறிலி எழுத்து *contenttype,
                               மாறிலி எழுத்து *target, size_t len)
{
  எனில்(contenttype && curl_strnequal(contenttype, target, len))
    தேர்வு(contenttype[len]) {
    நிலை '\0':
    நிலை '\t':
    நிலை '\r':
    நிலை '\n':
    நிலை ' ':
    நிலை ';':
      திரும்பு TRUE;
    }
  திரும்பு FALSE;
}

நிலையான CURLcode add_content_disposition(கட்டமைப்பு Curl_easy *data,
                                        curl_mimepart *part,
                                        மாறிலி எழுத்து *disposition,
                                        மாறிலி எழுத்து *contenttype,
                                        எண்ணிக்கை mimestrategy strategy)
{
  எனில்(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  எனில்(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = வெற்று;

  எனில்(disposition) {
    CURLcode result = CURLE_OK;
    எழுத்து *name = வெற்று;
    எழுத்து *filename = வெற்று;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    எனில்(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

    எனில்(part->name) {
      name = escape_string(data, part->name, strategy);
      எனில்(!name)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
    எனில்(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      எனில்(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    எனில்(!result)
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
    எனில்(result)
      திரும்பு result;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode mime_prepare_headers(கட்டமைப்பு Curl_easy *data,
                                     curl_mimepart *part,
                                     மாறிலி எழுத்து *contenttype,
                                     மாறிலி எழுத்து *disposition,
                                     எண்ணிக்கை mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = வெற்று;
  மாறிலி எழுத்து *boundary = வெற்று;
  எழுத்து *customct;
  மாறிலி எழுத்து *cte = வெற்று;
  CURLcode result = CURLE_OK;

  எனில்(++call_depth > MAX_MIME_LEVELS)
    திரும்பு CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = வெற்று;

  /* Be sure we will not access old headers later. */
  எனில்(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, வெற்று);

  /* Check if content type is specified. */
  customct = part->mimetype;
  எனில்(!customct)
    customct = தேடல்_header(part->userheaders, STRCONST("Content-Type"));
  எனில்(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  எனில்(!contenttype) {
    தேர்வு(part->kind) {
    நிலை MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      நிறுத்து;
    நிலை MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      எனில்(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      எனில்(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      நிறுத்து;
    இயல்பு:
      contenttype = Curl_mime_contenttype(part->filename);
      நிறுத்து;
    }
  }

  எனில்(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    எனில்(mime)
      boundary = mime->boundary;
  }
  இல்லையெனில் எனில்(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = வெற்று;

  /* Issue content-disposition header only if not already set by caller. */
  எனில்(!தேடல்_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    எனில்(result)
      திரும்பு result;
  }

  /* Issue Content-Type header. */
  எனில்(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    எனில்(result)
      திரும்பு result;
  }

  /* Content-Transfer-Encoding header. */
  எனில்(!தேடல்_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    எனில்(part->encoder)
      cte = part->encoder->name;
    இல்லையெனில் எனில்(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    எனில்(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      எனில்(result)
        திரும்பு result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  எனில்(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  எனில்(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = வெற்று;
    எனில்(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    க்கு(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, வெற்று,
                                    disposition, strategy, call_depth);
      எனில்(result)
        திரும்பு result;
    }
  }
  திரும்பு result;
}

CURLcode Curl_mime_prepare_headers(கட்டமைப்பு Curl_easy *data,
                                   curl_mimepart *part,
                                   மாறிலி எழுத்து *contenttype,
                                   மாறிலி எழுத்து *disposition,
                                   எண்ணிக்கை mimestrategy strategy)
{
  திரும்பு mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
நிலையான வெறுமை mime_unpause(curl_mimepart *part)
{
  எனில்(part) {
    எனில்(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    எனில்(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      எனில்(mime) {
        curl_mimepart *subpart;

        க்கு(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

கட்டமைப்பு cr_mime_ctx {
  கட்டமைப்பு Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  கட்டமைப்பு bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

நிலையான CURLcode cr_mime_init(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_mime_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cr_mime_close(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_mime_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
நிலையான CURLcode cr_mime_read(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_creader *reader,
                             எழுத்து *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  கட்டமைப்பு cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  எழுத்து tmp[256];

  /* Once we have errored, we will return the same error forever */
  எனில்(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (முழு)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    திரும்பு ctx->error_result;
  }
  எனில்(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    திரும்பு CURLE_OK;
  }
  /* respect length limitations */
  எனில்(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    எனில்(remain <= 0)
      blen = 0;
    இல்லையெனில் எனில்(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  எனில்(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned எழுத்து *)buf, blen, &nread);
    எனில்(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      திரும்பு result;
    }
  }
  இல்லையெனில் எனில்(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, அளவு(tmp), ctx->part);
    எனில்(nread <= அளவு(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned எழுத்து *)tmp, nread, &n);
      எனில்(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        திரும்பு result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      எனில்(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        திரும்பு result;
      }
    }
  }
  இல்லையெனில்
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  தேர்வு(nread) {
  நிலை 0:
    எனில்((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      திரும்பு CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    நிறுத்து;

  நிலை CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    நிறுத்து;

  நிலை CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    நிறுத்து; /* nothing was read */

  நிலை STOP_FILLING:
  நிலை READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    நிறுத்து;

  இயல்பு:
    எனில்(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      திரும்பு CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    எனில்(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    நிறுத்து;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (முழு)result, *pnread, *peos);
  திரும்பு result;
}

நிலையான bool cr_mime_needs_rewind(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_mime_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  திரும்பு ctx->read_len > 0;
}

நிலையான curl_off_t cr_mime_total_length(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_mime_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  திரும்பு ctx->total_len;
}

நிலையான CURLcode cr_mime_resume_from(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு Curl_creader *reader,
                                    curl_off_t offset)
{
  கட்டமைப்பு cr_mime_ctx *ctx = reader->ctx;

  எனில்(offset > 0) {
    curl_off_t passed = 0;

    செய் {
      எழுத்து scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)அளவு(scratch)) ?
        அளவு(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      எனில்((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        திரும்பு CURLE_READ_ERROR;
      }
    } போது(passed < offset);

    /* now, decrease the size of the read */
    எனில்(ctx->total_len > 0) {
      ctx->total_len -= offset;

      எனில்(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        திரும்பு CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cr_mime_cntrl(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  கட்டமைப்பு cr_mime_ctx *ctx = reader->ctx;
  தேர்வு(opcode) {
  நிலை CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    எனில்(result)
      failf(data, "Cannot rewind mime/post data");
    திரும்பு result;
  }
  நிலை CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    நிறுத்து;
  நிலை CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான bool cr_mime_is_paused(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_mime_ctx *ctx = reader->ctx;
  (வெறுமை)data;
  திரும்பு ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

நிலையான மாறிலி கட்டமைப்பு Curl_crtype cr_mime = {
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
  அளவு(கட்டமைப்பு cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(கட்டமைப்பு Curl_easy *data, curl_mimepart *part)
{
  கட்டமைப்பு Curl_creader *r;
  கட்டமைப்பு cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  எனில்(result)
    திரும்பு result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  எனில்(result) {
    Curl_creader_free(data, r);
    திரும்பு result;
  }
  ctx->total_len = mime_size(ctx->part);

  திரும்பு Curl_creader_set(data, r);
}

#இல்லையெனில் /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (வெறுமை)easy;
  திரும்பு வெற்று;
}

வெறுமை curl_mime_free(curl_mime *mime)
{
  (வெறுமை)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (வெறுமை)mime;
  திரும்பு வெற்று;
}

CURLcode curl_mime_name(curl_mimepart *part, மாறிலி எழுத்து *name)
{
  (வெறுமை)part;
  (வெறுமை)name;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, மாறிலி எழுத்து *filename)
{
  (வெறுமை)part;
  (வெறுமை)filename;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, மாறிலி எழுத்து *mimetype)
{
  (வெறுமை)part;
  (வெறுமை)mimetype;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, மாறிலி எழுத்து *encoding)
{
  (வெறுமை)part;
  (வெறுமை)encoding;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, மாறிலி எழுத்து *data, size_t datasize)
{
  (வெறுமை)part;
  (வெறுமை)data;
  (வெறுமை)datasize;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, மாறிலி எழுத்து *filename)
{
  (வெறுமை)part;
  (வெறுமை)filename;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           வெறுமை *arg)
{
  (வெறுமை)part;
  (வெறுமை)datasize;
  (வெறுமை)readfunc;
  (வெறுமை)seekfunc;
  (வெறுமை)freefunc;
  (வெறுமை)arg;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (வெறுமை)part;
  (வெறுமை)subparts;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           கட்டமைப்பு curl_slist *headers, முழு take_ownership)
{
  (வெறுமை)part;
  (வெறுமை)headers;
  (வெறுமை)take_ownership;
  திரும்பு CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(கட்டமைப்பு curl_slist **slp, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)slp;
  (வெறுமை)fmt;
  திரும்பு CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

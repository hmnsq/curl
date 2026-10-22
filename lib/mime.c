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
#अन्तर्भाव "curl_setup.h"

संरचना Curl_easy;

#अन्तर्भाव "mime.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/basename.h"
#अन्तर्भाव "curlx/strcopy.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curlx/base64.h"

#सचे !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#अन्तर्भाव "rand.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

स्थिर size_t mime_subparts_read(वर्ण *buffer, size_t size, size_t nitems,
                                 शून्य *instream, bool *hasread);
स्थिर curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
स्थिर ध्रुव unsigned वर्ण qp_class[] = {
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
स्थिर ध्रुव वर्ण aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#अञ्ञथा

#अन्तर्भाव <fabdef.h>
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
curl_off_t VmsRealFileSize(ध्रुव वर्ण *name, ध्रुव curlx_struct_stat *stat_buf)
{
  वर्ण buffer[8192];
  curl_off_t गणनम्;
  पूर्णाङ्क ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  सचे(!file)
    पटिनिवत्त 0;

  गणनम् = 0;
  ret_stat = 1;
  याव(ret_stat > 0) {
    ret_stat = fread(buffer, 1, परिमाण(buffer), file);
    सचे(ret_stat)
      गणनम् += ret_stat;
  }
  curlx_fclose(file);

  पटिनिवत्त गणनम्;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
स्थिर curl_off_t VmsSpecialSize(ध्रुव वर्ण *name,
                                 ध्रुव curlx_struct_stat *stat_buf)
{
  विकल्प(stat_buf->st_fab_rfm) {
  पक्ष FAB$C_VAR:
  पक्ष FAB$C_VFC:
    पटिनिवत्त VmsRealFileSize(name, stat_buf);
    भङ्ग;
  स्वभाव:
    पटिनिवत्त stat_buf->st_size;
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
स्थिर FILE *vmsfopenread(ध्रुव वर्ण *file, ध्रुव वर्ण *mode)
{
  curlx_struct_stat statbuf;
  पूर्णाङ्क res = curlx_stat(file, &statbuf);

  सचे(res != -1) {
    विकल्प(statbuf.st_fab_rfm) {
    पक्ष FAB$C_VAR:
    पक्ष FAB$C_VFC:
    पक्ष FAB$C_STMCR:
      पटिनिवत्त curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      भङ्ग;
    स्वभाव:
      पटिनिवत्त curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  पटिनिवत्त रिक्त;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
स्थिर शून्य mimesetstate(संरचना mime_state *state,
                         गणना mimestate tok, शून्य *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
स्थिर वर्ण *escape_string(संरचना Curl_easy *data,
                           ध्रुव वर्ण *src, गणना mimestrategy strategy)
{
  CURLcode result;
  संरचना dynbuf db;
  ध्रुव वर्ण * ध्रुव *table;
  ध्रुव वर्ण * ध्रुव *p;
  /* replace first character by rest of string. */
  स्थिर ध्रुव वर्ण * ध्रुव mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    रिक्त
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  स्थिर ध्रुव वर्ण * ध्रुव formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    रिक्त
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  सचे(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_निवेशः_LENGTH);

  अत्थाय(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    अत्थाय(p = table; *p && **p != *src; p++)
      ;

    सचे(*p)
      result = curlx_dyn_add(&db, *p + 1);
    अञ्ञथा
      result = curlx_dyn_addn(&db, src, 1);
  }

  पटिनिवत्त curlx_dyn_ptr(&db);
}

/* Check if header matches. */
स्थिर वर्ण *match_header(संरचना curl_slist *hdr, ध्रुव वर्ण *lbl, size_t len)
{
  वर्ण *मानम् = रिक्त;

  सचे(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    अत्थाय(मानम् = hdr->data + len + 1; *मानम् == ' '; मानम्++)
      ;
  पटिनिवत्त मानम्;
}

/* Get a header from an slist. */
स्थिर वर्ण *अन्वेषणम्_header(संरचना curl_slist *hdrlist,
                           ध्रुव वर्ण *hdr, size_t len)
{
  वर्ण *मानम् = रिक्त;

  अत्थाय(; !मानम् && hdrlist; hdrlist = hdrlist->next)
    मानम् = match_header(hdrlist, hdr, len);

  पटिनिवत्त मानम्;
}

स्थिर वर्ण *strippath(ध्रुव वर्ण *fullfile)
{
  वर्ण *filename;
  वर्ण *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  सचे(!filename)
    पटिनिवत्त रिक्त;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  पटिनिवत्त base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
स्थिर शून्य cleanup_encoder_state(संरचना mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
स्थिर size_t encoder_nop_read(वर्ण *buffer, size_t size, bool ateof,
                               संरचना curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (शून्य)ateof;

  सचे(!size)
    पटिनिवत्त STOP_FILLING;

  सचे(size > insize)
    size = insize;

  सचे(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  पटिनिवत्त size;
}

स्थिर curl_off_t encoder_nop_size(curl_mimepart *part)
{
  पटिनिवत्त part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
स्थिर size_t encoder_7bit_read(वर्ण *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (शून्य)ateof;

  सचे(!size)
    पटिनिवत्त STOP_FILLING;

  सचे(size > cursize)
    size = cursize;

  अत्थाय(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    सचे(*buffer++ & 0x80)
      पटिनिवत्त cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  पटिनिवत्त cursize;
}

/* Base64 content encoder. */
स्थिर size_t encoder_base64_read(वर्ण *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  पूर्णाङ्क i;
  वर्ण *ptr = buffer;

  याव(st->bufbeg < st->bufend) {
    /* Line full ? */
    सचे(st->pos > MAX_ENCODED_पङ्क्तिः_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      सचे(size < 2) {
        सचे(!cursize)
          पटिनिवत्त STOP_FILLING;
        भङ्ग;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    सचे(size < 4) {
      सचे(!cursize)
        पटिनिवत्त STOP_FILLING;
      भङ्ग;
    }
    सचे(st->bufend - st->bufbeg < 3)
      भङ्ग;

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
  सचे(ateof) {
    सचे(size < 4) {
      सचे(!cursize)
        पटिनिवत्त STOP_FILLING;
    }
    अञ्ञथा {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      सचे(st->bufend != st->bufbeg) {

        सचे(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        सचे(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  पटिनिवत्त cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_निवेशः_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_पङ्क्तिः_LENGTH + 2)) *                  \
    MAX_ENCODED_पङ्क्तिः_LENGTH / 4) * 3 - 3)

स्थिर curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  सचे(size <= 0)
    पटिनिवत्त size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  सचे(size > BASE64_MAX_निवेशः_SIZE)
    पटिनिवत्त -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  पटिनिवत्त size + (2 * ((size - 1) / MAX_ENCODED_पङ्क्तिः_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
स्थिर पूर्णाङ्क qp_lookahead_eol(संरचना mime_encoder_state *st, पूर्णाङ्क ateof, size_t n)
{
  n += st->bufbeg;
  सचे(n >= st->bufend && ateof)
    पटिनिवत्त 1;
  सचे(n + 2 > st->bufend)
    पटिनिवत्त ateof ? 0 : -1;
  सचे(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    पटिनिवत्त 1;
  पटिनिवत्त 0;
}

/* Quoted-printable encoder. */
स्थिर size_t encoder_qp_read(वर्ण *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  वर्ण *ptr = buffer;
  size_t cursize = 0;
  पूर्णाङ्क softlinebreak;
  वर्ण buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  याव(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    पूर्णाङ्क i = (unsigned वर्ण)st->buf[st->bufbeg];
    buf[0] = (वर्ण)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    विकल्प(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    पक्ष QP_OK:          /* Not a special character. */
      भङ्ग;
    पक्ष QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      विकल्प(qp_lookahead_eol(st, ateof, 1)) {
      पक्ष -1:          /* More input data needed. */
        पटिनिवत्त cursize;
      पक्ष 0:           /* No encoding needed. */
        भङ्ग;
      स्वभाव:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        भङ्ग;
      }
      भङ्ग;
    पक्ष QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      विकल्प(qp_lookahead_eol(st, ateof, 0)) {
      पक्ष -1:          /* Need more data. */
        पटिनिवत्त cursize;
      पक्ष 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        भङ्ग;
      स्वभाव:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        भङ्ग;
      }
      भङ्ग;
    स्वभाव:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      भङ्ग;
    }

    /* Be sure the encoded character fits within maximum line length. */
    सचे(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_पङ्क्तिः_LENGTH;
      सचे(!softlinebreak && st->pos + len == MAX_ENCODED_पङ्क्तिः_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        विकल्प(qp_lookahead_eol(st, ateof, consumed)) {
        पक्ष -1:        /* Need more data. */
          पटिनिवत्त cursize;
        पक्ष 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          भङ्ग;
        }
      }
      सचे(softlinebreak) {
        curlx_strcopy(buf, परिमाण(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    सचे(len > size) {
      सचे(!cursize)
        पटिनिवत्त STOP_FILLING;
      भङ्ग;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    सचे(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  पटिनिवत्त cursize;
}

स्थिर curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  पटिनिवत्त part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
स्थिर size_t mime_mem_read(वर्ण *buffer, size_t size, size_t nitems,
                            शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (शून्य)size;  /* Always 1 */

  सचे(!nitems)
    पटिनिवत्त STOP_FILLING;

  सचे(sz > nitems)
    sz = nitems;

  सचे(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  पटिनिवत्त sz;
}

स्थिर पूर्णाङ्क mime_mem_seek(शून्य *instream, curl_off_t offset, पूर्णाङ्क whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  विकल्प(whence) {
  पक्ष SEEK_CUR:
    offset += part->state.offset;
    भङ्ग;
  पक्ष SEEK_END:
    offset += part->datasize;
    भङ्ग;
  }

  सचे(offset < 0 || offset > part->datasize)
    पटिनिवत्त CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  पटिनिवत्त CURL_SEEKFUNC_OK;
}

स्थिर शून्य mime_mem_free(शून्य *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
स्थिर bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  सचे(part->fp)
    पटिनिवत्त FALSE;
  part->fp = fopen_read(part->data, "rb");
  पटिनिवत्त part->fp ? FALSE : TRUE;
}

स्थिर size_t mime_file_read(वर्ण *buffer, size_t size, size_t nitems,
                             शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  सचे(!nitems)
    पटिनिवत्त STOP_FILLING;

  सचे(mime_open_file(part))
    पटिनिवत्त READ_ERROR;

  पटिनिवत्त fread(buffer, size, nitems, part->fp);
}

स्थिर पूर्णाङ्क mime_file_seek(शून्य *instream, curl_off_t offset, पूर्णाङ्क whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  सचे(whence == SEEK_SET && !offset && !part->fp)
    पटिनिवत्त CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  सचे(mime_open_file(part))
    पटिनिवत्त CURL_SEEKFUNC_FAIL;

  पटिनिवत्त curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

स्थिर शून्य mime_file_free(शून्य *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  सचे(part->fp) {
    curlx_fclose(part->fp);
    part->fp = रिक्त;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
स्थिर size_t readback_bytes(संरचना mime_state *state,
                             वर्ण *buffer, size_t bufsize,
                             ध्रुव वर्ण *bytes, size_t numbytes,
                             ध्रुव वर्ण *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  सचे(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  अञ्ञथा {
    sz = offset - numbytes;
    सचे(sz >= traillen)
      पटिनिवत्त 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  सचे(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  पटिनिवत्त sz;
}

/* Read a non-encoded part content. */

स्थिर size_t read_part_content(curl_mimepart *part, वर्ण *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  विकल्प(part->lastreadstatus) {
  पक्ष 0:
  पक्ष CURL_READFUNC_ABORT:
  पक्ष CURL_READFUNC_PAUSE:
  पक्ष READ_ERROR:
    पटिनिवत्त part->lastreadstatus;
  स्वभाव:
    भङ्ग;
  }

  /* If we can determine we are at end of part data, spare a read. */
  सचे(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  अञ्ञथा {
    विकल्प(part->kind) {
    पक्ष MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      भङ्ग;
    पक्ष MIMEKIND_FILE:
      सचे(part->fp && feof(part->fp))
        भङ्ग;  /* At EOF. */
      FALLTHROUGH();
    स्वभाव:
      सचे(part->readfunc) {
        सचे(!(part->flags & MIME_FAST_READ)) {
          सचे(*hasread)
            पटिनिवत्त STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      भङ्ग;
    }
  }

  विकल्प(sz) {
  पक्ष STOP_FILLING:
    भङ्ग;
  पक्ष 0:
  पक्ष CURL_READFUNC_ABORT:
  पक्ष CURL_READFUNC_PAUSE:
  पक्ष READ_ERROR:
    part->lastreadstatus = sz;
    भङ्ग;
  स्वभाव:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    भङ्ग;
  }

  पटिनिवत्त sz;
}

/* Read and encode part content. */
स्थिर size_t read_encoded_part_content(curl_mimepart *part, वर्ण *buffer,
                                        size_t bufsize, bool *hasread)
{
  संरचना mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  अत्थाय(;;) {
    सचे(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      विकल्प(sz) {
      पक्ष 0:
        सचे(ateof)
          पटिनिवत्त cursize;
        भङ्ग;
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        पटिनिवत्त cursize ? cursize : sz;
      स्वभाव:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        अनुवर्त;
      }
    }

    /* We need more data in input buffer. */
    सचे(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      सचे(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    सचे(st->bufend >= परिमाण(st->buf))
      पटिनिवत्त cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           परिमाण(st->buf) - st->bufend, hasread);
    विकल्प(sz) {
    पक्ष 0:
      ateof = TRUE;
      भङ्ग;
    पक्ष CURL_READFUNC_ABORT:
    पक्ष CURL_READFUNC_PAUSE:
    पक्ष READ_ERROR:
    पक्ष STOP_FILLING:
      पटिनिवत्त cursize ? cursize : sz;
    स्वभाव:
      st->bufend += sz;
      भङ्ग;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
स्थिर size_t readback_part(curl_mimepart *part,
                            वर्ण *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  याव(bufsize) {
    size_t sz = 0;
    संरचना curl_slist *hdr = (संरचना curl_slist *)part->state.ptr;
    विकल्प(part->state.state) {
    पक्ष MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      भङ्ग;
    पक्ष MIMESTATE_USERHEADERS:
      सचे(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, रिक्त);
        भङ्ग;
      }
      सचे(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        भङ्ग;
      }
      FALLTHROUGH();
    पक्ष MIMESTATE_CURLHEADERS:
      सचे(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      अञ्ञथा {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        सचे(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      भङ्ग;
    पक्ष MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      सचे(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, रिक्त);
      भङ्ग;
    पक्ष MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, रिक्त);
      भङ्ग;
    पक्ष MIMESTATE_CONTENT:
      सचे(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      अञ्ञथा
        sz = read_part_content(part, buffer, bufsize, hasread);
      विकल्प(sz) {
      पक्ष 0:
        mimesetstate(&part->state, MIMESTATE_END, रिक्त);
        /* Try sparing open file descriptors. */
        सचे(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = रिक्त;
        }
        FALLTHROUGH();
      पक्ष CURL_READFUNC_ABORT:
      पक्ष CURL_READFUNC_PAUSE:
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        पटिनिवत्त cursize ? cursize : sz;
      }
      भङ्ग;
    पक्ष MIMESTATE_END:
      पटिनिवत्त cursize;
    स्वभाव:
      भङ्ग;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  पटिनिवत्त cursize;
}

/* Readback from mime. Warning: not a read callback function. */
स्थिर size_t mime_subparts_read(वर्ण *buffer, size_t size, size_t nitems,
                                 शून्य *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (शून्य)size;  /* Always 1 */

  याव(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    विकल्प(mime->state.state) {
    पक्ष MIMESTATE_BEGIN:
    पक्ष MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      भङ्ग;
    पक्ष MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      सचे(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      भङ्ग;
    पक्ष MIMESTATE_BOUNDARY2:
      सचे(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      अञ्ञथा
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      सचे(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      भङ्ग;
    पक्ष MIMESTATE_CONTENT:
      सचे(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, रिक्त);
        भङ्ग;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      विकल्प(sz) {
      पक्ष CURL_READFUNC_ABORT:
      पक्ष CURL_READFUNC_PAUSE:
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        पटिनिवत्त cursize ? cursize : sz;
      पक्ष 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        भङ्ग;
      }
      भङ्ग;
    पक्ष MIMESTATE_END:
      पटिनिवत्त cursize;
    स्वभाव:
      भङ्ग;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  पटिनिवत्त cursize;
}

स्थिर पूर्णाङ्क mime_part_rewind(curl_mimepart *part)
{
  पूर्णाङ्क res = CURL_SEEKFUNC_OK;
  गणना mimestate targetstate = MIMESTATE_BEGIN;

  सचे(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  सचे(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    सचे(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      विकल्प(res) {
      पक्ष CURL_SEEKFUNC_OK:
      पक्ष CURL_SEEKFUNC_FAIL:
      पक्ष CURL_SEEKFUNC_CANTSEEK:
        भङ्ग;
      पक्ष -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        भङ्ग;
      स्वभाव:
        res = CURL_SEEKFUNC_FAIL;
        भङ्ग;
      }
    }
  }

  सचे(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, रिक्त);

  part->lastreadstatus = 1; /* Successful read status. */
  पटिनिवत्त res;
}

स्थिर पूर्णाङ्क mime_subparts_seek(शून्य *instream, curl_off_t offset, पूर्णाङ्क whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  पूर्णाङ्क rc = CURL_SEEKFUNC_OK;

  सचे(whence != SEEK_SET || offset)
    पटिनिवत्त CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  सचे(mime->state.state == MIMESTATE_BEGIN)
    पटिनिवत्त CURL_SEEKFUNC_OK;           /* Already rewound. */

  अत्थाय(part = mime->firstpart; part; part = part->nextpart) {
    पूर्णाङ्क res = mime_part_rewind(part);
    सचे(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  सचे(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, रिक्त);

  पटिनिवत्त rc;
}

/* Release part content. */
स्थिर शून्य cleanup_part_content(curl_mimepart *part)
{
  सचे(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = रिक्त;
  part->seekfunc = रिक्त;
  part->freefunc = रिक्त;
  part->arg = (शून्य *)part;           /* Defaults to part itself. */
  part->data = रिक्त;
  part->fp = रिक्त;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned पूर्णाङ्क)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

स्थिर शून्य mime_subparts_free(शून्य *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  सचे(mime && mime->parent) {
    mime->parent->freefunc = रिक्त;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
स्थिर शून्य mime_subparts_unbind(शून्य *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  सचे(mime && mime->parent) {
    mime->parent->freefunc = रिक्त;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = रिक्त;
  }
}

शून्य Curl_mime_cleanpart(curl_mimepart *part)
{
  सचे(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    सचे(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
शून्य curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  सचे(!mime)
    पटिनिवत्त;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  याव(mime) {
    part = mime->firstpart;
    सचे(part) {
      mime->firstpart = part->nextpart;
      सचे(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = रिक्त;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    अञ्ञथा {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

स्थिर bool mime_is_too_deep(ध्रुव curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  सचे(++call_depth > MAX_MIME_LEVELS)
    पटिनिवत्त TRUE;

  सचे(part->kind == MIMEKIND_MULTIPART && part->arg) {
    ध्रुव curl_mime *mime = (ध्रुव curl_mime *)part->arg;
    ध्रुव curl_mimepart *s;

    अत्थाय(s = mime->firstpart; s; s = s->nextpart) {
      सचे(mime_is_too_deep(s, call_depth))
        पटिनिवत्त TRUE;
    }
  }

  पटिनिवत्त FALSE;
}

स्थिर CURLcode mime_duppart(संरचना Curl_easy *data,
                             curl_mimepart *dst, ध्रुव curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  ध्रुव curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  सचे(++call_depth > MAX_MIME_LEVELS)
    पटिनिवत्त CURLE_TOO_LARGE;

  /* Duplicate content. */
  विकल्प(src->kind) {
  पक्ष MIMEKIND_NONE:
    भङ्ग;
  पक्ष MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    भङ्ग;
  पक्ष MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    सचे(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    भङ्ग;
  पक्ष MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    भङ्ग;
  पक्ष MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    अत्थाय(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    भङ्ग;
  स्वभाव:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    भङ्ग;
  }

  /* Duplicate headers. */
  सचे(!result && src->userheaders) {
    संरचना curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    सचे(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    अञ्ञथा {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      सचे(result)
        curl_slist_free_all(hdrs);
    }
  }

  सचे(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  सचे(!result)
    result = curl_mime_name(dst, src->name);
  सचे(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  सचे(result)
    Curl_mime_cleanpart(dst);

  पटिनिवत्त result;
}

CURLcode Curl_mime_duppart(संरचना Curl_easy *data,
                           curl_mimepart *dst, ध्रुव curl_mimepart *src)
{
  सचे(!dst || !src)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  सचे(mime_is_too_deep(src, 0))
    पटिनिवत्त CURLE_TOO_LARGE;

  पटिनिवत्त mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(शून्य *easy)
{
  curl_mime *mime = curlx_malloc(परिमाण(*mime));

  सचे(mime) {
    mime->parent = रिक्त;
    mime->firstpart = रिक्त;
    mime->lastpart = रिक्त;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    सचे(Curl_rand_alnum(easy,
                       (unsigned वर्ण *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      पटिनिवत्त रिक्त;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, रिक्त);
  }

  पटिनिवत्त mime;
}

/* Initialize a mime part. */
शून्य Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, परिमाण(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, रिक्त);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  सचे(!mime)
    पटिनिवत्त रिक्त;

  part = curlx_malloc(परिमाण(*part));

  सचे(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    सचे(mime->lastpart)
      mime->lastpart->nextpart = part;
    अञ्ञथा
      mime->firstpart = part;

    mime->lastpart = part;
  }

  पटिनिवत्त part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ध्रुव वर्ण *name)
{
  सचे(!part)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  सचे(name) {
    part->name = curlx_strdup(name);
    सचे(!part->name)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  पटिनिवत्त CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ध्रुव वर्ण *filename)
{
  सचे(!part)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  सचे(filename) {
    part->filename = curlx_strdup(filename);
    सचे(!part->filename)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  पटिनिवत्त CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, ध्रुव वर्ण *data, size_t datasize)
{
  सचे(!part)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  सचे(data) {
    सचे(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    सचे(!part->data)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  पटिनिवत्त CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ध्रुव वर्ण *filename)
{
  CURLcode result = CURLE_OK;

  सचे(!part)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  सचे(filename) {
    वर्ण *base;
    curlx_struct_stat sbuf;

    सचे(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    अञ्ञथा {
      part->data = curlx_strdup(filename);
      सचे(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      अञ्ञथा {
        part->datasize = -1;
        सचे(S_ISREG(sbuf.st_mode)) {
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
        सचे(!base)
          result = CURLE_OUT_OF_MEMORY;
        अञ्ञथा {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  पटिनिवत्त result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ध्रुव वर्ण *mimetype)
{
  सचे(!part)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  सचे(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    सचे(!part->mimetype)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  पटिनिवत्त CURLE_OK;
}

स्थिर ध्रुव संरचना mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ध्रुव वर्ण *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  ध्रुव संरचना mime_encoder *mep;

  सचे(!part)
    पटिनिवत्त result;

  part->encoder = रिक्त;

  सचे(!encoding)
    पटिनिवत्त CURLE_OK;    /* Removing current encoder. */

  अत्थाय(mep = encoders; mep->name; mep++)
    सचे(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  पटिनिवत्त result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           संरचना curl_slist *headers, पूर्णाङ्क take_ownership)
{
  सचे(!part)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  सचे(part->flags & MIME_USERHEADERS_OWNER) {
    सचे(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned पूर्णाङ्क)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  सचे(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  पटिनिवत्त CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, शून्य *arg)
{
  सचे(!part)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  सचे(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  पटिनिवत्त CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, पूर्णाङ्क take_ownership)
{
  curl_mime *मूलम्;

  सचे(!part)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  सचे(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    पटिनिवत्त CURLE_OK;

  cleanup_part_content(part);

  सचे(subparts) {
    /* Should not have been attached already. */
    सचे(subparts->parent)
      पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    मूलम् = part->parent;
    सचे(मूलम्) {
      याव(मूलम्->parent && मूलम्->parent->parent)
        मूलम् = मूलम्->parent->parent;
      सचे(subparts == मूलम्) {
        /* cannot add as a subpart of itself. */
        पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    सचे(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      पटिनिवत्त CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  पटिनिवत्त CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  पटिनिवत्त Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(वर्ण *buffer, size_t size, size_t nitems, शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (शून्य)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  करोहि {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } याव(ret == STOP_FILLING);

  पटिनिवत्त ret;
}

/* Rewind mime stream. */
स्थिर CURLcode mime_rewind(curl_mimepart *part)
{
  पटिनिवत्त mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
स्थिर size_t slist_size(संरचना curl_slist *s,
                         size_t overhead, ध्रुव वर्ण *skip, size_t skiplen)
{
  size_t size = 0;

  अत्थाय(; s; s = s->next)
    सचे(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  पटिनिवत्त size;
}

/* Get/compute multipart size. */
स्थिर curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  सचे(!mime)
    पटिनिवत्त 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  अत्थाय(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    सचे(sz < 0)
      size = sz;

    सचे(size >= 0)
      size += boundarysize + sz;
  }

  पटिनिवत्त size;
}

/* Get/compute mime size. */
स्थिर curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  सचे(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  सचे(part->encoder)
    size = part->encoder->sizefunc(part);

  सचे(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, रिक्त, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  पटिनिवत्त size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(संरचना curl_slist **slp, ध्रुव वर्ण *fmt, ...)
{
  संरचना curl_slist *hdr = रिक्त;
  वर्ण *s = रिक्त;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  सचे(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    सचे(hdr)
      *slp = hdr;
    अञ्ञथा
      curlx_free(s);
  }

  पटिनिवत्त hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
स्थिर CURLcode add_content_type(संरचना curl_slist **slp,
                                 ध्रुव वर्ण *type, ध्रुव वर्ण *boundary)
{
  पटिनिवत्त Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ध्रुव वर्ण *Curl_mime_contenttype(ध्रुव वर्ण *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  संरचना ContentType {
    ध्रुव वर्ण *extension;
    ध्रुव वर्ण *type;
  };
  स्थिर ध्रुव संरचना ContentType ctts[] = {
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

  सचे(filename) {
    size_t len1 = strlen(filename);
    ध्रुव वर्ण *nameend = filename + len1;
    unsigned पूर्णाङ्क i;

    अत्थाय(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      सचे(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        पटिनिवत्त ctts[i].type;
    }
  }
  पटिनिवत्त रिक्त;
}

स्थिर bool content_type_match(ध्रुव वर्ण *contenttype,
                               ध्रुव वर्ण *target, size_t len)
{
  सचे(contenttype && curl_strnequal(contenttype, target, len))
    विकल्प(contenttype[len]) {
    पक्ष '\0':
    पक्ष '\t':
    पक्ष '\r':
    पक्ष '\n':
    पक्ष ' ':
    पक्ष ';':
      पटिनिवत्त TRUE;
    }
  पटिनिवत्त FALSE;
}

स्थिर CURLcode add_content_disposition(संरचना Curl_easy *data,
                                        curl_mimepart *part,
                                        ध्रुव वर्ण *disposition,
                                        ध्रुव वर्ण *contenttype,
                                        गणना mimestrategy strategy)
{
  सचे(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  सचे(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = रिक्त;

  सचे(disposition) {
    CURLcode result = CURLE_OK;
    वर्ण *name = रिक्त;
    वर्ण *filename = रिक्त;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    सचे(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

    सचे(part->name) {
      name = escape_string(data, part->name, strategy);
      सचे(!name)
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
    }
    सचे(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      सचे(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    सचे(!result)
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
    सचे(result)
      पटिनिवत्त result;
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode mime_prepare_headers(संरचना Curl_easy *data,
                                     curl_mimepart *part,
                                     ध्रुव वर्ण *contenttype,
                                     ध्रुव वर्ण *disposition,
                                     गणना mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = रिक्त;
  ध्रुव वर्ण *boundary = रिक्त;
  वर्ण *customct;
  ध्रुव वर्ण *cte = रिक्त;
  CURLcode result = CURLE_OK;

  सचे(++call_depth > MAX_MIME_LEVELS)
    पटिनिवत्त CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = रिक्त;

  /* Be sure we will not access old headers later. */
  सचे(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, रिक्त);

  /* Check if content type is specified. */
  customct = part->mimetype;
  सचे(!customct)
    customct = अन्वेषणम्_header(part->userheaders, STRCONST("Content-Type"));
  सचे(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  सचे(!contenttype) {
    विकल्प(part->kind) {
    पक्ष MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      भङ्ग;
    पक्ष MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      सचे(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      सचे(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      भङ्ग;
    स्वभाव:
      contenttype = Curl_mime_contenttype(part->filename);
      भङ्ग;
    }
  }

  सचे(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    सचे(mime)
      boundary = mime->boundary;
  }
  अञ्ञथा सचे(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = रिक्त;

  /* Issue content-disposition header only if not already set by caller. */
  सचे(!अन्वेषणम्_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    सचे(result)
      पटिनिवत्त result;
  }

  /* Issue Content-Type header. */
  सचे(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    सचे(result)
      पटिनिवत्त result;
  }

  /* Content-Transfer-Encoding header. */
  सचे(!अन्वेषणम्_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    सचे(part->encoder)
      cte = part->encoder->name;
    अञ्ञथा सचे(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    सचे(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      सचे(result)
        पटिनिवत्त result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  सचे(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  सचे(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = रिक्त;
    सचे(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    अत्थाय(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, रिक्त,
                                    disposition, strategy, call_depth);
      सचे(result)
        पटिनिवत्त result;
    }
  }
  पटिनिवत्त result;
}

CURLcode Curl_mime_prepare_headers(संरचना Curl_easy *data,
                                   curl_mimepart *part,
                                   ध्रुव वर्ण *contenttype,
                                   ध्रुव वर्ण *disposition,
                                   गणना mimestrategy strategy)
{
  पटिनिवत्त mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
स्थिर शून्य mime_unpause(curl_mimepart *part)
{
  सचे(part) {
    सचे(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    सचे(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      सचे(mime) {
        curl_mimepart *subpart;

        अत्थाय(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

संरचना cr_mime_ctx {
  संरचना Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  संरचना bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

स्थिर CURLcode cr_mime_init(संरचना Curl_easy *data,
                             संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य cr_mime_close(संरचना Curl_easy *data,
                          संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
स्थिर CURLcode cr_mime_read(संरचना Curl_easy *data,
                             संरचना Curl_creader *reader,
                             वर्ण *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  वर्ण tmp[256];

  /* Once we have errored, we will return the same error forever */
  सचे(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (पूर्णाङ्क)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    पटिनिवत्त ctx->error_result;
  }
  सचे(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    पटिनिवत्त CURLE_OK;
  }
  /* respect length limitations */
  सचे(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    सचे(remain <= 0)
      blen = 0;
    अञ्ञथा सचे(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  सचे(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned वर्ण *)buf, blen, &nread);
    सचे(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      पटिनिवत्त result;
    }
  }
  अञ्ञथा सचे(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, परिमाण(tmp), ctx->part);
    सचे(nread <= परिमाण(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned वर्ण *)tmp, nread, &n);
      सचे(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        पटिनिवत्त result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      सचे(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        पटिनिवत्त result;
      }
    }
  }
  अञ्ञथा
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  विकल्प(nread) {
  पक्ष 0:
    सचे((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      पटिनिवत्त CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    भङ्ग;

  पक्ष CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    भङ्ग;

  पक्ष CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    भङ्ग; /* nothing was read */

  पक्ष STOP_FILLING:
  पक्ष READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    भङ्ग;

  स्वभाव:
    सचे(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      पटिनिवत्त CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    सचे(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    भङ्ग;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (पूर्णाङ्क)result, *pnread, *peos);
  पटिनिवत्त result;
}

स्थिर bool cr_mime_needs_rewind(संरचना Curl_easy *data,
                                 संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  पटिनिवत्त ctx->read_len > 0;
}

स्थिर curl_off_t cr_mime_total_length(संरचना Curl_easy *data,
                                       संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  पटिनिवत्त ctx->total_len;
}

स्थिर CURLcode cr_mime_resume_from(संरचना Curl_easy *data,
                                    संरचना Curl_creader *reader,
                                    curl_off_t offset)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;

  सचे(offset > 0) {
    curl_off_t passed = 0;

    करोहि {
      वर्ण scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)परिमाण(scratch)) ?
        परिमाण(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      सचे((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        पटिनिवत्त CURLE_READ_ERROR;
      }
    } याव(passed < offset);

    /* now, decrease the size of the read */
    सचे(ctx->total_len > 0) {
      ctx->total_len -= offset;

      सचे(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        पटिनिवत्त CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode cr_mime_cntrl(संरचना Curl_easy *data,
                              संरचना Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  विकल्प(opcode) {
  पक्ष CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    सचे(result)
      failf(data, "Cannot rewind mime/post data");
    पटिनिवत्त result;
  }
  पक्ष CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    भङ्ग;
  पक्ष CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर bool cr_mime_is_paused(संरचना Curl_easy *data,
                              संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  पटिनिवत्त ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

स्थिर ध्रुव संरचना Curl_crtype cr_mime = {
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
  परिमाण(संरचना cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(संरचना Curl_easy *data, curl_mimepart *part)
{
  संरचना Curl_creader *r;
  संरचना cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  सचे(result)
    पटिनिवत्त result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  सचे(result) {
    Curl_creader_free(data, r);
    पटिनिवत्त result;
  }
  ctx->total_len = mime_size(ctx->part);

  पटिनिवत्त Curl_creader_set(data, r);
}

#अञ्ञथा /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (शून्य)easy;
  पटिनिवत्त रिक्त;
}

शून्य curl_mime_free(curl_mime *mime)
{
  (शून्य)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (शून्य)mime;
  पटिनिवत्त रिक्त;
}

CURLcode curl_mime_name(curl_mimepart *part, ध्रुव वर्ण *name)
{
  (शून्य)part;
  (शून्य)name;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, ध्रुव वर्ण *filename)
{
  (शून्य)part;
  (शून्य)filename;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, ध्रुव वर्ण *mimetype)
{
  (शून्य)part;
  (शून्य)mimetype;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ध्रुव वर्ण *encoding)
{
  (शून्य)part;
  (शून्य)encoding;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, ध्रुव वर्ण *data, size_t datasize)
{
  (शून्य)part;
  (शून्य)data;
  (शून्य)datasize;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ध्रुव वर्ण *filename)
{
  (शून्य)part;
  (शून्य)filename;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           शून्य *arg)
{
  (शून्य)part;
  (शून्य)datasize;
  (शून्य)readfunc;
  (शून्य)seekfunc;
  (शून्य)freefunc;
  (शून्य)arg;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (शून्य)part;
  (शून्य)subparts;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           संरचना curl_slist *headers, पूर्णाङ्क take_ownership)
{
  (शून्य)part;
  (शून्य)headers;
  (शून्य)take_ownership;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(संरचना curl_slist **slp, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)slp;
  (शून्य)fmt;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

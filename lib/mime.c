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

#यदि !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#अन्तर्भाव "rand.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((माप_प्रकार)-1)
#define STOP_FILLING ((माप_प्रकार)-2)

स्थिर माप_प्रकार mime_subparts_read(वर्ण *buffer, माप_प्रकार size, माप_प्रकार nitems,
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
स्थिर अचर अचिन्हित वर्ण qp_class[] = {
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
स्थिर अचर वर्ण aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#अन्यथा

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
curl_off_t VmsRealFileSize(अचर वर्ण *name, अचर curlx_struct_stat *stat_buf)
{
  वर्ण buffer[8192];
  curl_off_t गन्ती;
  पूर्णाङ्क ret_stat;
  खाता *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  यदि(!file)
    फर्काऊ 0;

  गन्ती = 0;
  ret_stat = 1;
  जबसम्म(ret_stat > 0) {
    ret_stat = ख_पढ़ो(buffer, 1, परिमाण(buffer), file);
    यदि(ret_stat)
      गन्ती += ret_stat;
  }
  curlx_fclose(file);

  फर्काऊ गन्ती;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
स्थिर curl_off_t VmsSpecialSize(अचर वर्ण *name,
                                 अचर curlx_struct_stat *stat_buf)
{
  विकल्प(stat_buf->st_fab_rfm) {
  पक्ष FAB$C_VAR:
  पक्ष FAB$C_VFC:
    फर्काऊ VmsRealFileSize(name, stat_buf);
    रोक;
  स्वभाव:
    फर्काऊ stat_buf->st_size;
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
स्थिर खाता *vmsfopenread(अचर वर्ण *file, अचर वर्ण *mode)
{
  curlx_struct_stat statbuf;
  पूर्णाङ्क res = curlx_stat(file, &statbuf);

  यदि(res != -1) {
    विकल्प(statbuf.st_fab_rfm) {
    पक्ष FAB$C_VAR:
    पक्ष FAB$C_VFC:
    पक्ष FAB$C_STMCR:
      फर्काऊ curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      रोक;
    स्वभाव:
      फर्काऊ curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  फर्काऊ रिक्त;
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
                           अचर वर्ण *src, गणना mimestrategy strategy)
{
  CURLcode result;
  संरचना dynbuf db;
  अचर वर्ण * अचर *table;
  अचर वर्ण * अचर *p;
  /* replace first character by rest of string. */
  स्थिर अचर वर्ण * अचर mimetable[] = {
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
  स्थिर अचर वर्ण * अचर formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    रिक्त
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  यदि(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_आगत_LENGTH);

  लागि(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    लागि(p = table; *p && **p != *src; p++)
      ;

    यदि(*p)
      result = curlx_dyn_add(&db, *p + 1);
    अन्यथा
      result = curlx_dyn_addn(&db, src, 1);
  }

  फर्काऊ curlx_dyn_ptr(&db);
}

/* Check if header matches. */
स्थिर वर्ण *match_header(संरचना curl_slist *hdr, अचर वर्ण *lbl, माप_प्रकार len)
{
  वर्ण *मान = रिक्त;

  यदि(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    लागि(मान = hdr->data + len + 1; *मान == ' '; मान++)
      ;
  फर्काऊ मान;
}

/* Get a header from an slist. */
स्थिर वर्ण *खोज_header(संरचना curl_slist *hdrlist,
                           अचर वर्ण *hdr, माप_प्रकार len)
{
  वर्ण *मान = रिक्त;

  लागि(; !मान && hdrlist; hdrlist = hdrlist->next)
    मान = match_header(hdrlist, hdr, len);

  फर्काऊ मान;
}

स्थिर वर्ण *strippath(अचर वर्ण *fullfile)
{
  वर्ण *filename;
  वर्ण *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  यदि(!filename)
    फर्काऊ रिक्त;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  फर्काऊ base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
स्थिर शून्य cleanup_encoder_state(संरचना mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
स्थिर माप_प्रकार encoder_nop_read(वर्ण *buffer, माप_प्रकार size, bool ateof,
                               संरचना curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  माप_प्रकार insize = st->bufend - st->bufbeg;

  (शून्य)ateof;

  यदि(!size)
    फर्काऊ STOP_FILLING;

  यदि(size > insize)
    size = insize;

  यदि(size)
    स_नकल(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  फर्काऊ size;
}

स्थिर curl_off_t encoder_nop_size(curl_mimepart *part)
{
  फर्काऊ part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
स्थिर माप_प्रकार encoder_7bit_read(वर्ण *buffer, माप_प्रकार size, bool ateof,
                                curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = st->bufend - st->bufbeg;

  (शून्य)ateof;

  यदि(!size)
    फर्काऊ STOP_FILLING;

  यदि(size > cursize)
    size = cursize;

  लागि(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    यदि(*buffer++ & 0x80)
      फर्काऊ cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  फर्काऊ cursize;
}

/* Base64 content encoder. */
स्थिर माप_प्रकार encoder_base64_read(वर्ण *buffer, माप_प्रकार size, bool ateof,
                                  curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = 0;
  पूर्णाङ्क i;
  वर्ण *ptr = buffer;

  जबसम्म(st->bufbeg < st->bufend) {
    /* Line full ? */
    यदि(st->pos > MAX_ENCODED_पङ्क्ति_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      यदि(size < 2) {
        यदि(!cursize)
          फर्काऊ STOP_FILLING;
        रोक;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    यदि(size < 4) {
      यदि(!cursize)
        फर्काऊ STOP_FILLING;
      रोक;
    }
    यदि(st->bufend - st->bufbeg < 3)
      रोक;

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
  यदि(ateof) {
    यदि(size < 4) {
      यदि(!cursize)
        फर्काऊ STOP_FILLING;
    }
    अन्यथा {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      यदि(st->bufend != st->bufbeg) {

        यदि(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        यदि(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  फर्काऊ cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_आगत_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_पङ्क्ति_LENGTH + 2)) *                  \
    MAX_ENCODED_पङ्क्ति_LENGTH / 4) * 3 - 3)

स्थिर curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  यदि(size <= 0)
    फर्काऊ size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  यदि(size > BASE64_MAX_आगत_SIZE)
    फर्काऊ -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  फर्काऊ size + (2 * ((size - 1) / MAX_ENCODED_पङ्क्ति_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
स्थिर पूर्णाङ्क qp_lookahead_eol(संरचना mime_encoder_state *st, पूर्णाङ्क ateof, माप_प्रकार n)
{
  n += st->bufbeg;
  यदि(n >= st->bufend && ateof)
    फर्काऊ 1;
  यदि(n + 2 > st->bufend)
    फर्काऊ ateof ? 0 : -1;
  यदि(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    फर्काऊ 1;
  फर्काऊ 0;
}

/* Quoted-printable encoder. */
स्थिर माप_प्रकार encoder_qp_read(वर्ण *buffer, माप_प्रकार size, bool ateof,
                              curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  वर्ण *ptr = buffer;
  माप_प्रकार cursize = 0;
  पूर्णाङ्क softlinebreak;
  वर्ण buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  जबसम्म(st->bufbeg < st->bufend) {
    माप_प्रकार len = 1;
    माप_प्रकार consumed = 1;
    पूर्णाङ्क i = (अचिन्हित वर्ण)st->buf[st->bufbeg];
    buf[0] = (वर्ण)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    विकल्प(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    पक्ष QP_OK:          /* Not a special character. */
      रोक;
    पक्ष QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      विकल्प(qp_lookahead_eol(st, ateof, 1)) {
      पक्ष -1:          /* More input data needed. */
        फर्काऊ cursize;
      पक्ष 0:           /* No encoding needed. */
        रोक;
      स्वभाव:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        रोक;
      }
      रोक;
    पक्ष QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      विकल्प(qp_lookahead_eol(st, ateof, 0)) {
      पक्ष -1:          /* Need more data. */
        फर्काऊ cursize;
      पक्ष 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        रोक;
      स्वभाव:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        रोक;
      }
      रोक;
    स्वभाव:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      रोक;
    }

    /* Be sure the encoded character fits within maximum line length. */
    यदि(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_पङ्क्ति_LENGTH;
      यदि(!softlinebreak && st->pos + len == MAX_ENCODED_पङ्क्ति_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        विकल्प(qp_lookahead_eol(st, ateof, consumed)) {
        पक्ष -1:        /* Need more data. */
          फर्काऊ cursize;
        पक्ष 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          रोक;
        }
      }
      यदि(softlinebreak) {
        curlx_strcopy(buf, परिमाण(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    यदि(len > size) {
      यदि(!cursize)
        फर्काऊ STOP_FILLING;
      रोक;
    }

    /* Append to output buffer. */
    स_नकल(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    यदि(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  फर्काऊ cursize;
}

स्थिर curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  फर्काऊ part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
स्थिर माप_प्रकार mime_mem_read(वर्ण *buffer, माप_प्रकार size, माप_प्रकार nitems,
                            शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  माप_प्रकार sz = curlx_sotouz(part->datasize - part->state.offset);
  (शून्य)size;  /* Always 1 */

  यदि(!nitems)
    फर्काऊ STOP_FILLING;

  यदि(sz > nitems)
    sz = nitems;

  यदि(sz)
    स_नकल(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  फर्काऊ sz;
}

स्थिर पूर्णाङ्क mime_mem_seek(शून्य *instream, curl_off_t offset, पूर्णाङ्क whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  विकल्प(whence) {
  पक्ष प्रस्तुत_से:
    offset += part->state.offset;
    रोक;
  पक्ष अंत_से:
    offset += part->datasize;
    रोक;
  }

  यदि(offset < 0 || offset > part->datasize)
    फर्काऊ CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  फर्काऊ CURL_SEEKFUNC_OK;
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

  यदि(part->fp)
    फर्काऊ FALSE;
  part->fp = fopen_read(part->data, "rb");
  फर्काऊ part->fp ? FALSE : TRUE;
}

स्थिर माप_प्रकार mime_file_read(वर्ण *buffer, माप_प्रकार size, माप_प्रकार nitems,
                             शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  यदि(!nitems)
    फर्काऊ STOP_FILLING;

  यदि(mime_open_file(part))
    फर्काऊ READ_ERROR;

  फर्काऊ ख_पढ़ो(buffer, size, nitems, part->fp);
}

स्थिर पूर्णाङ्क mime_file_seek(शून्य *instream, curl_off_t offset, पूर्णाङ्क whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  यदि(whence == शुरू_से && !offset && !part->fp)
    फर्काऊ CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  यदि(mime_open_file(part))
    फर्काऊ CURL_SEEKFUNC_FAIL;

  फर्काऊ curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

स्थिर शून्य mime_file_free(शून्य *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  यदि(part->fp) {
    curlx_fclose(part->fp);
    part->fp = रिक्त;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
स्थिर माप_प्रकार readback_bytes(संरचना mime_state *state,
                             वर्ण *buffer, माप_प्रकार bufsize,
                             अचर वर्ण *bytes, माप_प्रकार numbytes,
                             अचर वर्ण *trail, माप_प्रकार traillen)
{
  माप_प्रकार sz;
  माप_प्रकार offset = curlx_sotouz(state->offset);

  यदि(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  अन्यथा {
    sz = offset - numbytes;
    यदि(sz >= traillen)
      फर्काऊ 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  यदि(sz > bufsize)
    sz = bufsize;

  स_नकल(buffer, bytes, sz);
  state->offset += sz;
  फर्काऊ sz;
}

/* Read a non-encoded part content. */

स्थिर माप_प्रकार read_part_content(curl_mimepart *part, वर्ण *buffer,
                                माप_प्रकार bufsize, bool *hasread)
{
  माप_प्रकार sz = 0;

  विकल्प(part->lastreadstatus) {
  पक्ष 0:
  पक्ष CURL_READFUNC_ABORT:
  पक्ष CURL_READFUNC_PAUSE:
  पक्ष READ_ERROR:
    फर्काऊ part->lastreadstatus;
  स्वभाव:
    रोक;
  }

  /* If we can determine we are at end of part data, spare a read. */
  यदि(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  अन्यथा {
    विकल्प(part->kind) {
    पक्ष MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      रोक;
    पक्ष MIMEKIND_FILE:
      यदि(part->fp && ख_पूर्ण(part->fp))
        रोक;  /* At EOF. */
      FALLTHROUGH();
    स्वभाव:
      यदि(part->readfunc) {
        यदि(!(part->flags & MIME_FAST_READ)) {
          यदि(*hasread)
            फर्काऊ STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      रोक;
    }
  }

  विकल्प(sz) {
  पक्ष STOP_FILLING:
    रोक;
  पक्ष 0:
  पक्ष CURL_READFUNC_ABORT:
  पक्ष CURL_READFUNC_PAUSE:
  पक्ष READ_ERROR:
    part->lastreadstatus = sz;
    रोक;
  स्वभाव:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    रोक;
  }

  फर्काऊ sz;
}

/* Read and encode part content. */
स्थिर माप_प्रकार read_encoded_part_content(curl_mimepart *part, वर्ण *buffer,
                                        माप_प्रकार bufsize, bool *hasread)
{
  संरचना mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = 0;
  माप_प्रकार sz;
  bool ateof = FALSE;

  लागि(;;) {
    यदि(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      विकल्प(sz) {
      पक्ष 0:
        यदि(ateof)
          फर्काऊ cursize;
        रोक;
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        फर्काऊ cursize ? cursize : sz;
      स्वभाव:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        जारी;
      }
    }

    /* We need more data in input buffer. */
    यदि(st->bufbeg) {
      माप_प्रकार len = st->bufend - st->bufbeg;

      यदि(len)
        स_हटाओ(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    यदि(st->bufend >= परिमाण(st->buf))
      फर्काऊ cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           परिमाण(st->buf) - st->bufend, hasread);
    विकल्प(sz) {
    पक्ष 0:
      ateof = TRUE;
      रोक;
    पक्ष CURL_READFUNC_ABORT:
    पक्ष CURL_READFUNC_PAUSE:
    पक्ष READ_ERROR:
    पक्ष STOP_FILLING:
      फर्काऊ cursize ? cursize : sz;
    स्वभाव:
      st->bufend += sz;
      रोक;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
स्थिर माप_प्रकार readback_part(curl_mimepart *part,
                            वर्ण *buffer, माप_प्रकार bufsize, bool *hasread)
{
  माप_प्रकार cursize = 0;

  /* Readback from part. */
  जबसम्म(bufsize) {
    माप_प्रकार sz = 0;
    संरचना curl_slist *hdr = (संरचना curl_slist *)part->state.ptr;
    विकल्प(part->state.state) {
    पक्ष MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      रोक;
    पक्ष MIMESTATE_USERHEADERS:
      यदि(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, रिक्त);
        रोक;
      }
      यदि(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        रोक;
      }
      FALLTHROUGH();
    पक्ष MIMESTATE_CURLHEADERS:
      यदि(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      अन्यथा {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, म_माप(hdr->data), STRCONST("\r\n"));
        यदि(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      रोक;
    पक्ष MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      यदि(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, रिक्त);
      रोक;
    पक्ष MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, रिक्त);
      रोक;
    पक्ष MIMESTATE_CONTENT:
      यदि(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      अन्यथा
        sz = read_part_content(part, buffer, bufsize, hasread);
      विकल्प(sz) {
      पक्ष 0:
        mimesetstate(&part->state, MIMESTATE_END, रिक्त);
        /* Try sparing open file descriptors. */
        यदि(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = रिक्त;
        }
        FALLTHROUGH();
      पक्ष CURL_READFUNC_ABORT:
      पक्ष CURL_READFUNC_PAUSE:
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        फर्काऊ cursize ? cursize : sz;
      }
      रोक;
    पक्ष MIMESTATE_END:
      फर्काऊ cursize;
    स्वभाव:
      रोक;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  फर्काऊ cursize;
}

/* Readback from mime. Warning: not a read callback function. */
स्थिर माप_प्रकार mime_subparts_read(वर्ण *buffer, माप_प्रकार size, माप_प्रकार nitems,
                                 शून्य *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  माप_प्रकार cursize = 0;
  (शून्य)size;  /* Always 1 */

  जबसम्म(nitems) {
    माप_प्रकार sz = 0;
    curl_mimepart *part = mime->state.ptr;
    विकल्प(mime->state.state) {
    पक्ष MIMESTATE_BEGIN:
    पक्ष MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      रोक;
    पक्ष MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      यदि(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      रोक;
    पक्ष MIMESTATE_BOUNDARY2:
      यदि(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      अन्यथा
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      यदि(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      रोक;
    पक्ष MIMESTATE_CONTENT:
      यदि(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, रिक्त);
        रोक;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      विकल्प(sz) {
      पक्ष CURL_READFUNC_ABORT:
      पक्ष CURL_READFUNC_PAUSE:
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        फर्काऊ cursize ? cursize : sz;
      पक्ष 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        रोक;
      }
      रोक;
    पक्ष MIMESTATE_END:
      फर्काऊ cursize;
    स्वभाव:
      रोक;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  फर्काऊ cursize;
}

स्थिर पूर्णाङ्क mime_part_rewind(curl_mimepart *part)
{
  पूर्णाङ्क res = CURL_SEEKFUNC_OK;
  गणना mimestate targetstate = MIMESTATE_BEGIN;

  यदि(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  यदि(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    यदि(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, शुरू_से);
      विकल्प(res) {
      पक्ष CURL_SEEKFUNC_OK:
      पक्ष CURL_SEEKFUNC_FAIL:
      पक्ष CURL_SEEKFUNC_CANTSEEK:
        रोक;
      पक्ष -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        रोक;
      स्वभाव:
        res = CURL_SEEKFUNC_FAIL;
        रोक;
      }
    }
  }

  यदि(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, रिक्त);

  part->lastreadstatus = 1; /* Successful read status. */
  फर्काऊ res;
}

स्थिर पूर्णाङ्क mime_subparts_seek(शून्य *instream, curl_off_t offset, पूर्णाङ्क whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  पूर्णाङ्क rc = CURL_SEEKFUNC_OK;

  यदि(whence != शुरू_से || offset)
    फर्काऊ CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  यदि(mime->state.state == MIMESTATE_BEGIN)
    फर्काऊ CURL_SEEKFUNC_OK;           /* Already rewound. */

  लागि(part = mime->firstpart; part; part = part->nextpart) {
    पूर्णाङ्क res = mime_part_rewind(part);
    यदि(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  यदि(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, रिक्त);

  फर्काऊ rc;
}

/* Release part content. */
स्थिर शून्य cleanup_part_content(curl_mimepart *part)
{
  यदि(part->freefunc)
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
  part->flags &= ~(अचिन्हित पूर्णाङ्क)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

स्थिर शून्य mime_subparts_free(शून्य *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  यदि(mime && mime->parent) {
    mime->parent->freefunc = रिक्त;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
स्थिर शून्य mime_subparts_unbind(शून्य *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  यदि(mime && mime->parent) {
    mime->parent->freefunc = रिक्त;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = रिक्त;
  }
}

शून्य Curl_mime_cleanpart(curl_mimepart *part)
{
  यदि(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    यदि(part->flags & MIME_USERHEADERS_OWNER)
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

  यदि(!mime)
    फर्काऊ;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  जबसम्म(mime) {
    part = mime->firstpart;
    यदि(part) {
      mime->firstpart = part->nextpart;
      यदि(part->kind == MIMEKIND_MULTIPART && part->arg &&
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
    अन्यथा {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

स्थिर bool mime_is_too_deep(अचर curl_mimepart *part, माप_प्रकार call_depth)
{
  DEBUGASSERT(part);

  यदि(++call_depth > MAX_MIME_LEVELS)
    फर्काऊ TRUE;

  यदि(part->kind == MIMEKIND_MULTIPART && part->arg) {
    अचर curl_mime *mime = (अचर curl_mime *)part->arg;
    अचर curl_mimepart *s;

    लागि(s = mime->firstpart; s; s = s->nextpart) {
      यदि(mime_is_too_deep(s, call_depth))
        फर्काऊ TRUE;
    }
  }

  फर्काऊ FALSE;
}

स्थिर CURLcode mime_duppart(संरचना Curl_easy *data,
                             curl_mimepart *dst, अचर curl_mimepart *src,
                             माप_प्रकार call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  अचर curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  यदि(++call_depth > MAX_MIME_LEVELS)
    फर्काऊ CURLE_TOO_LARGE;

  /* Duplicate content. */
  विकल्प(src->kind) {
  पक्ष MIMEKIND_NONE:
    रोक;
  पक्ष MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (माप_प्रकार)src->datasize);
    रोक;
  पक्ष MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    यदि(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    रोक;
  पक्ष MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    रोक;
  पक्ष MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    लागि(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    रोक;
  स्वभाव:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    रोक;
  }

  /* Duplicate headers. */
  यदि(!result && src->userheaders) {
    संरचना curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    यदि(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      यदि(result)
        curl_slist_free_all(hdrs);
    }
  }

  यदि(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  यदि(!result)
    result = curl_mime_name(dst, src->name);
  यदि(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  यदि(result)
    Curl_mime_cleanpart(dst);

  फर्काऊ result;
}

CURLcode Curl_mime_duppart(संरचना Curl_easy *data,
                           curl_mimepart *dst, अचर curl_mimepart *src)
{
  यदि(!dst || !src)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  यदि(mime_is_too_deep(src, 0))
    फर्काऊ CURLE_TOO_LARGE;

  फर्काऊ mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(शून्य *easy)
{
  curl_mime *mime = curlx_malloc(परिमाण(*mime));

  यदि(mime) {
    mime->parent = रिक्त;
    mime->firstpart = रिक्त;
    mime->lastpart = रिक्त;

    स_रखो(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    यदि(Curl_rand_alnum(easy,
                       (अचिन्हित वर्ण *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      फर्काऊ रिक्त;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, रिक्त);
  }

  फर्काऊ mime;
}

/* Initialize a mime part. */
शून्य Curl_mime_initpart(curl_mimepart *part)
{
  स_रखो(part, 0, परिमाण(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, रिक्त);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  यदि(!mime)
    फर्काऊ रिक्त;

  part = curlx_malloc(परिमाण(*part));

  यदि(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    यदि(mime->lastpart)
      mime->lastpart->nextpart = part;
    अन्यथा
      mime->firstpart = part;

    mime->lastpart = part;
  }

  फर्काऊ part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, अचर वर्ण *name)
{
  यदि(!part)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  यदि(name) {
    part->name = curlx_strdup(name);
    यदि(!part->name)
      फर्काऊ CURLE_OUT_OF_MEMORY;
  }

  फर्काऊ CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, अचर वर्ण *filename)
{
  यदि(!part)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  यदि(filename) {
    part->filename = curlx_strdup(filename);
    यदि(!part->filename)
      फर्काऊ CURLE_OUT_OF_MEMORY;
  }

  फर्काऊ CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, अचर वर्ण *data, माप_प्रकार datasize)
{
  यदि(!part)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  यदि(data) {
    यदि(datasize == CURL_ZERO_TERMINATED)
      datasize = म_माप(data);

    part->data = curlx_memdup0(data, datasize);
    यदि(!part->data)
      फर्काऊ CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  फर्काऊ CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, अचर वर्ण *filename)
{
  CURLcode result = CURLE_OK;

  यदि(!part)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  यदि(filename) {
    वर्ण *base;
    curlx_struct_stat sbuf;

    यदि(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    अन्यथा {
      part->data = curlx_strdup(filename);
      यदि(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      अन्यथा {
        part->datasize = -1;
        यदि(S_ISREG(sbuf.st_mode)) {
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
        यदि(!base)
          result = CURLE_OUT_OF_MEMORY;
        अन्यथा {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  फर्काऊ result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, अचर वर्ण *mimetype)
{
  यदि(!part)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  यदि(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    यदि(!part->mimetype)
      फर्काऊ CURLE_OUT_OF_MEMORY;
  }

  फर्काऊ CURLE_OK;
}

स्थिर अचर संरचना mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, अचर वर्ण *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  अचर संरचना mime_encoder *mep;

  यदि(!part)
    फर्काऊ result;

  part->encoder = रिक्त;

  यदि(!encoding)
    फर्काऊ CURLE_OK;    /* Removing current encoder. */

  लागि(mep = encoders; mep->name; mep++)
    यदि(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  फर्काऊ result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           संरचना curl_slist *headers, पूर्णाङ्क take_ownership)
{
  यदि(!part)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  यदि(part->flags & MIME_USERHEADERS_OWNER) {
    यदि(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(अचिन्हित पूर्णाङ्क)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  यदि(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  फर्काऊ CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, शून्य *arg)
{
  यदि(!part)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  यदि(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  फर्काऊ CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, पूर्णाङ्क take_ownership)
{
  curl_mime *जरा;

  यदि(!part)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  यदि(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    फर्काऊ CURLE_OK;

  cleanup_part_content(part);

  यदि(subparts) {
    /* Should not have been attached already. */
    यदि(subparts->parent)
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    जरा = part->parent;
    यदि(जरा) {
      जबसम्म(जरा->parent && जरा->parent->parent)
        जरा = जरा->parent->parent;
      यदि(subparts == जरा) {
        /* cannot add as a subpart of itself. */
        फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    यदि(mime_subparts_seek(subparts, (curl_off_t)0, शुरू_से) !=
       CURL_SEEKFUNC_OK)
      फर्काऊ CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  फर्काऊ CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  फर्काऊ Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
माप_प्रकार Curl_mime_read(वर्ण *buffer, माप_प्रकार size, माप_प्रकार nitems, शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  माप_प्रकार ret;
  bool hasread;

  (शून्य)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  गर {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } जबसम्म(ret == STOP_FILLING);

  फर्काऊ ret;
}

/* Rewind mime stream. */
स्थिर CURLcode mime_rewind(curl_mimepart *part)
{
  फर्काऊ mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
स्थिर माप_प्रकार slist_size(संरचना curl_slist *s,
                         माप_प्रकार overhead, अचर वर्ण *skip, माप_प्रकार skiplen)
{
  माप_प्रकार size = 0;

  लागि(; s; s = s->next)
    यदि(!skip || !match_header(s, skip, skiplen))
      size += म_माप(s->data) + overhead;
  फर्काऊ size;
}

/* Get/compute multipart size. */
स्थिर curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  यदि(!mime)
    फर्काऊ 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  लागि(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    यदि(sz < 0)
      size = sz;

    यदि(size >= 0)
      size += boundarysize + sz;
  }

  फर्काऊ size;
}

/* Get/compute mime size. */
स्थिर curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  यदि(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  यदि(part->encoder)
    size = part->encoder->sizefunc(part);

  यदि(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, रिक्त, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  फर्काऊ size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(संरचना curl_slist **slp, अचर वर्ण *fmt, ...)
{
  संरचना curl_slist *hdr = रिक्त;
  वर्ण *s = रिक्त;
  बहु_सूची ap;

  बहु_शुरू(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  बहु_पूर्ण(ap);

  यदि(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    यदि(hdr)
      *slp = hdr;
    अन्यथा
      curlx_free(s);
  }

  फर्काऊ hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
स्थिर CURLcode add_content_type(संरचना curl_slist **slp,
                                 अचर वर्ण *type, अचर वर्ण *boundary)
{
  फर्काऊ Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

अचर वर्ण *Curl_mime_contenttype(अचर वर्ण *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  संरचना ContentType {
    अचर वर्ण *extension;
    अचर वर्ण *type;
  };
  स्थिर अचर संरचना ContentType ctts[] = {
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

  यदि(filename) {
    माप_प्रकार len1 = म_माप(filename);
    अचर वर्ण *nameend = filename + len1;
    अचिन्हित पूर्णाङ्क i;

    लागि(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      माप_प्रकार len2 = म_माप(ctts[i].extension);

      यदि(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        फर्काऊ ctts[i].type;
    }
  }
  फर्काऊ रिक्त;
}

स्थिर bool content_type_match(अचर वर्ण *contenttype,
                               अचर वर्ण *target, माप_प्रकार len)
{
  यदि(contenttype && curl_strnequal(contenttype, target, len))
    विकल्प(contenttype[len]) {
    पक्ष '\0':
    पक्ष '\t':
    पक्ष '\r':
    पक्ष '\n':
    पक्ष ' ':
    पक्ष ';':
      फर्काऊ TRUE;
    }
  फर्काऊ FALSE;
}

स्थिर CURLcode add_content_disposition(संरचना Curl_easy *data,
                                        curl_mimepart *part,
                                        अचर वर्ण *disposition,
                                        अचर वर्ण *contenttype,
                                        गणना mimestrategy strategy)
{
  यदि(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  यदि(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = रिक्त;

  यदि(disposition) {
    CURLcode result = CURLE_OK;
    वर्ण *name = रिक्त;
    वर्ण *filename = रिक्त;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    यदि(backslash &&
       ((part->name && part->name[म_खोज(part->name, "\r\n")]) ||
        (part->filename && part->filename[म_खोज(part->filename, "\r\n")])))
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

    यदि(part->name) {
      name = escape_string(data, part->name, strategy);
      यदि(!name)
        फर्काऊ CURLE_OUT_OF_MEMORY;
    }
    यदि(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      यदि(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    यदि(!result)
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
    यदि(result)
      फर्काऊ result;
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode mime_prepare_headers(संरचना Curl_easy *data,
                                     curl_mimepart *part,
                                     अचर वर्ण *contenttype,
                                     अचर वर्ण *disposition,
                                     गणना mimestrategy strategy,
                                     माप_प्रकार call_depth)
{
  curl_mime *mime = रिक्त;
  अचर वर्ण *boundary = रिक्त;
  वर्ण *customct;
  अचर वर्ण *cte = रिक्त;
  CURLcode result = CURLE_OK;

  यदि(++call_depth > MAX_MIME_LEVELS)
    फर्काऊ CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = रिक्त;

  /* Be sure we will not access old headers later. */
  यदि(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, रिक्त);

  /* Check if content type is specified. */
  customct = part->mimetype;
  यदि(!customct)
    customct = खोज_header(part->userheaders, STRCONST("Content-Type"));
  यदि(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  यदि(!contenttype) {
    विकल्प(part->kind) {
    पक्ष MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      रोक;
    पक्ष MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      यदि(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      यदि(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      रोक;
    स्वभाव:
      contenttype = Curl_mime_contenttype(part->filename);
      रोक;
    }
  }

  यदि(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    यदि(mime)
      boundary = mime->boundary;
  }
  अन्यथा यदि(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = रिक्त;

  /* Issue content-disposition header only if not already set by caller. */
  यदि(!खोज_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    यदि(result)
      फर्काऊ result;
  }

  /* Issue Content-Type header. */
  यदि(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    यदि(result)
      फर्काऊ result;
  }

  /* Content-Transfer-Encoding header. */
  यदि(!खोज_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    यदि(part->encoder)
      cte = part->encoder->name;
    अन्यथा यदि(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    यदि(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      यदि(result)
        फर्काऊ result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  यदि(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  यदि(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = रिक्त;
    यदि(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    लागि(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, रिक्त,
                                    disposition, strategy, call_depth);
      यदि(result)
        फर्काऊ result;
    }
  }
  फर्काऊ result;
}

CURLcode Curl_mime_prepare_headers(संरचना Curl_easy *data,
                                   curl_mimepart *part,
                                   अचर वर्ण *contenttype,
                                   अचर वर्ण *disposition,
                                   गणना mimestrategy strategy)
{
  फर्काऊ mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
स्थिर शून्य mime_unpause(curl_mimepart *part)
{
  यदि(part) {
    यदि(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    यदि(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      यदि(mime) {
        curl_mimepart *subpart;

        लागि(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
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
  फर्काऊ CURLE_OK;
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
                             वर्ण *buf, माप_प्रकार blen,
                             माप_प्रकार *pnread, bool *peos)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  माप_प्रकार nread;
  वर्ण tmp[256];

  /* Once we have errored, we will return the same error forever */
  यदि(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (पूर्णाङ्क)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    फर्काऊ ctx->error_result;
  }
  यदि(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    फर्काऊ CURLE_OK;
  }
  /* respect length limitations */
  यदि(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    यदि(remain <= 0)
      blen = 0;
    अन्यथा यदि(remain < (curl_off_t)blen)
      blen = (माप_प्रकार)remain;
  }

  यदि(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (अचिन्हित वर्ण *)buf, blen, &nread);
    यदि(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      फर्काऊ result;
    }
  }
  अन्यथा यदि(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, परिमाण(tmp), ctx->part);
    यदि(nread <= परिमाण(tmp)) {
      माप_प्रकार n;
      result = Curl_bufq_write(&ctx->tmpbuf, (अचिन्हित वर्ण *)tmp, nread, &n);
      यदि(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        फर्काऊ result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      यदि(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        फर्काऊ result;
      }
    }
  }
  अन्यथा
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  विकल्प(nread) {
  पक्ष 0:
    यदि((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      फर्काऊ CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    रोक;

  पक्ष CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    रोक;

  पक्ष CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    रोक; /* nothing was read */

  पक्ष STOP_FILLING:
  पक्ष READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    रोक;

  स्वभाव:
    यदि(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      फर्काऊ CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    यदि(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    रोक;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (पूर्णाङ्क)result, *pnread, *peos);
  फर्काऊ result;
}

स्थिर bool cr_mime_needs_rewind(संरचना Curl_easy *data,
                                 संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  फर्काऊ ctx->read_len > 0;
}

स्थिर curl_off_t cr_mime_total_length(संरचना Curl_easy *data,
                                       संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  फर्काऊ ctx->total_len;
}

स्थिर CURLcode cr_mime_resume_from(संरचना Curl_easy *data,
                                    संरचना Curl_creader *reader,
                                    curl_off_t offset)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;

  यदि(offset > 0) {
    curl_off_t passed = 0;

    गर {
      वर्ण scratch[4 * 1024];
      माप_प्रकार readthisamountnow =
        (offset - passed > (curl_off_t)परिमाण(scratch)) ?
        परिमाण(scratch) :
        curlx_sotouz(offset - passed);
      माप_प्रकार nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      यदि((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        फर्काऊ CURLE_READ_ERROR;
      }
    } जबसम्म(passed < offset);

    /* now, decrease the size of the read */
    यदि(ctx->total_len > 0) {
      ctx->total_len -= offset;

      यदि(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        फर्काऊ CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode cr_mime_cntrl(संरचना Curl_easy *data,
                              संरचना Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  विकल्प(opcode) {
  पक्ष CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    यदि(result)
      failf(data, "Cannot rewind mime/post data");
    फर्काऊ result;
  }
  पक्ष CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    रोक;
  पक्ष CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    रोक;
  स्वभाव:
    रोक;
  }
  फर्काऊ CURLE_OK;
}

स्थिर bool cr_mime_is_paused(संरचना Curl_easy *data,
                              संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  फर्काऊ ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

स्थिर अचर संरचना Curl_crtype cr_mime = {
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
  यदि(result)
    फर्काऊ result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  यदि(result) {
    Curl_creader_free(data, r);
    फर्काऊ result;
  }
  ctx->total_len = mime_size(ctx->part);

  फर्काऊ Curl_creader_set(data, r);
}

#अन्यथा /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (शून्य)easy;
  फर्काऊ रिक्त;
}

शून्य curl_mime_free(curl_mime *mime)
{
  (शून्य)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (शून्य)mime;
  फर्काऊ रिक्त;
}

CURLcode curl_mime_name(curl_mimepart *part, अचर वर्ण *name)
{
  (शून्य)part;
  (शून्य)name;
  फर्काऊ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, अचर वर्ण *filename)
{
  (शून्य)part;
  (शून्य)filename;
  फर्काऊ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, अचर वर्ण *mimetype)
{
  (शून्य)part;
  (शून्य)mimetype;
  फर्काऊ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, अचर वर्ण *encoding)
{
  (शून्य)part;
  (शून्य)encoding;
  फर्काऊ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, अचर वर्ण *data, माप_प्रकार datasize)
{
  (शून्य)part;
  (शून्य)data;
  (शून्य)datasize;
  फर्काऊ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, अचर वर्ण *filename)
{
  (शून्य)part;
  (शून्य)filename;
  फर्काऊ CURLE_NOT_BUILT_IN;
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
  फर्काऊ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (शून्य)part;
  (शून्य)subparts;
  फर्काऊ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           संरचना curl_slist *headers, पूर्णाङ्क take_ownership)
{
  (शून्य)part;
  (शून्य)headers;
  (शून्य)take_ownership;
  फर्काऊ CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(संरचना curl_slist **slp, अचर वर्ण *fmt, ...)
{
  (शून्य)slp;
  (शून्य)fmt;
  फर्काऊ CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

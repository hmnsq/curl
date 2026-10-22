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

#जर !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#अन्तर्भाव "rand.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((माप_प्रकार)-1)
#define STOP_FILLING ((माप_प्रकार)-2)

स्थिर माप_प्रकार mime_subparts_read(अक्षर *buffer, माप_प्रकार size, माप_प्रकार nitems,
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
स्थिर स्थिरांक अचिन्हित अक्षर qp_class[] = {
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
स्थिर स्थिरांक अक्षर aschex[] =
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
curl_off_t VmsRealFileSize(स्थिरांक अक्षर *name, स्थिरांक curlx_struct_stat *stat_buf)
{
  अक्षर buffer[8192];
  curl_off_t मोजणी;
  पूर्णांक ret_stat;
  खाता *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  जर(!file)
    परत 0;

  मोजणी = 0;
  ret_stat = 1;
  पर्यंत(ret_stat > 0) {
    ret_stat = ख_पढ़ो(buffer, 1, आकार(buffer), file);
    जर(ret_stat)
      मोजणी += ret_stat;
  }
  curlx_fclose(file);

  परत मोजणी;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
स्थिर curl_off_t VmsSpecialSize(स्थिरांक अक्षर *name,
                                 स्थिरांक curlx_struct_stat *stat_buf)
{
  निवड(stat_buf->st_fab_rfm) {
  स्थिती FAB$C_VAR:
  स्थिती FAB$C_VFC:
    परत VmsRealFileSize(name, stat_buf);
    थांबा;
  पूर्वनिर्धारित:
    परत stat_buf->st_size;
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
स्थिर खाता *vmsfopenread(स्थिरांक अक्षर *file, स्थिरांक अक्षर *mode)
{
  curlx_struct_stat statbuf;
  पूर्णांक res = curlx_stat(file, &statbuf);

  जर(res != -1) {
    निवड(statbuf.st_fab_rfm) {
    स्थिती FAB$C_VAR:
    स्थिती FAB$C_VFC:
    स्थिती FAB$C_STMCR:
      परत curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      थांबा;
    पूर्वनिर्धारित:
      परत curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  परत रिक्त;
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
स्थिर अक्षर *escape_string(संरचना Curl_easy *data,
                           स्थिरांक अक्षर *src, गणना mimestrategy strategy)
{
  CURLcode result;
  संरचना dynbuf db;
  स्थिरांक अक्षर * स्थिरांक *table;
  स्थिरांक अक्षर * स्थिरांक *p;
  /* replace first character by rest of string. */
  स्थिर स्थिरांक अक्षर * स्थिरांक mimetable[] = {
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
  स्थिर स्थिरांक अक्षर * स्थिरांक formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    रिक्त
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  जर(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_आदान_LENGTH);

  साठी(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    साठी(p = table; *p && **p != *src; p++)
      ;

    जर(*p)
      result = curlx_dyn_add(&db, *p + 1);
    अन्यथा
      result = curlx_dyn_addn(&db, src, 1);
  }

  परत curlx_dyn_ptr(&db);
}

/* Check if header matches. */
स्थिर अक्षर *match_header(संरचना curl_slist *hdr, स्थिरांक अक्षर *lbl, माप_प्रकार len)
{
  अक्षर *मूल्य = रिक्त;

  जर(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    साठी(मूल्य = hdr->data + len + 1; *मूल्य == ' '; मूल्य++)
      ;
  परत मूल्य;
}

/* Get a header from an slist. */
स्थिर अक्षर *शोध_header(संरचना curl_slist *hdrlist,
                           स्थिरांक अक्षर *hdr, माप_प्रकार len)
{
  अक्षर *मूल्य = रिक्त;

  साठी(; !मूल्य && hdrlist; hdrlist = hdrlist->next)
    मूल्य = match_header(hdrlist, hdr, len);

  परत मूल्य;
}

स्थिर अक्षर *strippath(स्थिरांक अक्षर *fullfile)
{
  अक्षर *filename;
  अक्षर *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  जर(!filename)
    परत रिक्त;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  परत base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
स्थिर शून्य cleanup_encoder_state(संरचना mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
स्थिर माप_प्रकार encoder_nop_read(अक्षर *buffer, माप_प्रकार size, bool ateof,
                               संरचना curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  माप_प्रकार insize = st->bufend - st->bufbeg;

  (शून्य)ateof;

  जर(!size)
    परत STOP_FILLING;

  जर(size > insize)
    size = insize;

  जर(size)
    स_नकल(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  परत size;
}

स्थिर curl_off_t encoder_nop_size(curl_mimepart *part)
{
  परत part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
स्थिर माप_प्रकार encoder_7bit_read(अक्षर *buffer, माप_प्रकार size, bool ateof,
                                curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = st->bufend - st->bufbeg;

  (शून्य)ateof;

  जर(!size)
    परत STOP_FILLING;

  जर(size > cursize)
    size = cursize;

  साठी(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    जर(*buffer++ & 0x80)
      परत cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  परत cursize;
}

/* Base64 content encoder. */
स्थिर माप_प्रकार encoder_base64_read(अक्षर *buffer, माप_प्रकार size, bool ateof,
                                  curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = 0;
  पूर्णांक i;
  अक्षर *ptr = buffer;

  पर्यंत(st->bufbeg < st->bufend) {
    /* Line full ? */
    जर(st->pos > MAX_ENCODED_ओळ_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      जर(size < 2) {
        जर(!cursize)
          परत STOP_FILLING;
        थांबा;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    जर(size < 4) {
      जर(!cursize)
        परत STOP_FILLING;
      थांबा;
    }
    जर(st->bufend - st->bufbeg < 3)
      थांबा;

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
  जर(ateof) {
    जर(size < 4) {
      जर(!cursize)
        परत STOP_FILLING;
    }
    अन्यथा {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      जर(st->bufend != st->bufbeg) {

        जर(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        जर(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  परत cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_आदान_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_ओळ_LENGTH + 2)) *                  \
    MAX_ENCODED_ओळ_LENGTH / 4) * 3 - 3)

स्थिर curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  जर(size <= 0)
    परत size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  जर(size > BASE64_MAX_आदान_SIZE)
    परत -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  परत size + (2 * ((size - 1) / MAX_ENCODED_ओळ_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
स्थिर पूर्णांक qp_lookahead_eol(संरचना mime_encoder_state *st, पूर्णांक ateof, माप_प्रकार n)
{
  n += st->bufbeg;
  जर(n >= st->bufend && ateof)
    परत 1;
  जर(n + 2 > st->bufend)
    परत ateof ? 0 : -1;
  जर(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    परत 1;
  परत 0;
}

/* Quoted-printable encoder. */
स्थिर माप_प्रकार encoder_qp_read(अक्षर *buffer, माप_प्रकार size, bool ateof,
                              curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  अक्षर *ptr = buffer;
  माप_प्रकार cursize = 0;
  पूर्णांक softlinebreak;
  अक्षर buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  पर्यंत(st->bufbeg < st->bufend) {
    माप_प्रकार len = 1;
    माप_प्रकार consumed = 1;
    पूर्णांक i = (अचिन्हित अक्षर)st->buf[st->bufbeg];
    buf[0] = (अक्षर)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    निवड(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    स्थिती QP_OK:          /* Not a special character. */
      थांबा;
    स्थिती QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      निवड(qp_lookahead_eol(st, ateof, 1)) {
      स्थिती -1:          /* More input data needed. */
        परत cursize;
      स्थिती 0:           /* No encoding needed. */
        थांबा;
      पूर्वनिर्धारित:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        थांबा;
      }
      थांबा;
    स्थिती QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      निवड(qp_lookahead_eol(st, ateof, 0)) {
      स्थिती -1:          /* Need more data. */
        परत cursize;
      स्थिती 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        थांबा;
      पूर्वनिर्धारित:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        थांबा;
      }
      थांबा;
    पूर्वनिर्धारित:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      थांबा;
    }

    /* Be sure the encoded character fits within maximum line length. */
    जर(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_ओळ_LENGTH;
      जर(!softlinebreak && st->pos + len == MAX_ENCODED_ओळ_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        निवड(qp_lookahead_eol(st, ateof, consumed)) {
        स्थिती -1:        /* Need more data. */
          परत cursize;
        स्थिती 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          थांबा;
        }
      }
      जर(softlinebreak) {
        curlx_strcopy(buf, आकार(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    जर(len > size) {
      जर(!cursize)
        परत STOP_FILLING;
      थांबा;
    }

    /* Append to output buffer. */
    स_नकल(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    जर(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  परत cursize;
}

स्थिर curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  परत part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
स्थिर माप_प्रकार mime_mem_read(अक्षर *buffer, माप_प्रकार size, माप_प्रकार nitems,
                            शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  माप_प्रकार sz = curlx_sotouz(part->datasize - part->state.offset);
  (शून्य)size;  /* Always 1 */

  जर(!nitems)
    परत STOP_FILLING;

  जर(sz > nitems)
    sz = nitems;

  जर(sz)
    स_नकल(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  परत sz;
}

स्थिर पूर्णांक mime_mem_seek(शून्य *instream, curl_off_t offset, पूर्णांक whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  निवड(whence) {
  स्थिती प्रस्तुत_से:
    offset += part->state.offset;
    थांबा;
  स्थिती अंत_से:
    offset += part->datasize;
    थांबा;
  }

  जर(offset < 0 || offset > part->datasize)
    परत CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  परत CURL_SEEKFUNC_OK;
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

  जर(part->fp)
    परत FALSE;
  part->fp = fopen_read(part->data, "rb");
  परत part->fp ? FALSE : TRUE;
}

स्थिर माप_प्रकार mime_file_read(अक्षर *buffer, माप_प्रकार size, माप_प्रकार nitems,
                             शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  जर(!nitems)
    परत STOP_FILLING;

  जर(mime_open_file(part))
    परत READ_ERROR;

  परत ख_पढ़ो(buffer, size, nitems, part->fp);
}

स्थिर पूर्णांक mime_file_seek(शून्य *instream, curl_off_t offset, पूर्णांक whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  जर(whence == शुरू_से && !offset && !part->fp)
    परत CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  जर(mime_open_file(part))
    परत CURL_SEEKFUNC_FAIL;

  परत curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

स्थिर शून्य mime_file_free(शून्य *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  जर(part->fp) {
    curlx_fclose(part->fp);
    part->fp = रिक्त;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
स्थिर माप_प्रकार readback_bytes(संरचना mime_state *state,
                             अक्षर *buffer, माप_प्रकार bufsize,
                             स्थिरांक अक्षर *bytes, माप_प्रकार numbytes,
                             स्थिरांक अक्षर *trail, माप_प्रकार traillen)
{
  माप_प्रकार sz;
  माप_प्रकार offset = curlx_sotouz(state->offset);

  जर(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  अन्यथा {
    sz = offset - numbytes;
    जर(sz >= traillen)
      परत 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  जर(sz > bufsize)
    sz = bufsize;

  स_नकल(buffer, bytes, sz);
  state->offset += sz;
  परत sz;
}

/* Read a non-encoded part content. */

स्थिर माप_प्रकार read_part_content(curl_mimepart *part, अक्षर *buffer,
                                माप_प्रकार bufsize, bool *hasread)
{
  माप_प्रकार sz = 0;

  निवड(part->lastreadstatus) {
  स्थिती 0:
  स्थिती CURL_READFUNC_ABORT:
  स्थिती CURL_READFUNC_PAUSE:
  स्थिती READ_ERROR:
    परत part->lastreadstatus;
  पूर्वनिर्धारित:
    थांबा;
  }

  /* If we can determine we are at end of part data, spare a read. */
  जर(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  अन्यथा {
    निवड(part->kind) {
    स्थिती MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      थांबा;
    स्थिती MIMEKIND_FILE:
      जर(part->fp && ख_पूर्ण(part->fp))
        थांबा;  /* At EOF. */
      FALLTHROUGH();
    पूर्वनिर्धारित:
      जर(part->readfunc) {
        जर(!(part->flags & MIME_FAST_READ)) {
          जर(*hasread)
            परत STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      थांबा;
    }
  }

  निवड(sz) {
  स्थिती STOP_FILLING:
    थांबा;
  स्थिती 0:
  स्थिती CURL_READFUNC_ABORT:
  स्थिती CURL_READFUNC_PAUSE:
  स्थिती READ_ERROR:
    part->lastreadstatus = sz;
    थांबा;
  पूर्वनिर्धारित:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    थांबा;
  }

  परत sz;
}

/* Read and encode part content. */
स्थिर माप_प्रकार read_encoded_part_content(curl_mimepart *part, अक्षर *buffer,
                                        माप_प्रकार bufsize, bool *hasread)
{
  संरचना mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = 0;
  माप_प्रकार sz;
  bool ateof = FALSE;

  साठी(;;) {
    जर(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      निवड(sz) {
      स्थिती 0:
        जर(ateof)
          परत cursize;
        थांबा;
      स्थिती READ_ERROR:
      स्थिती STOP_FILLING:
        परत cursize ? cursize : sz;
      पूर्वनिर्धारित:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        सुरूठेवा;
      }
    }

    /* We need more data in input buffer. */
    जर(st->bufbeg) {
      माप_प्रकार len = st->bufend - st->bufbeg;

      जर(len)
        स_हटाओ(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    जर(st->bufend >= आकार(st->buf))
      परत cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           आकार(st->buf) - st->bufend, hasread);
    निवड(sz) {
    स्थिती 0:
      ateof = TRUE;
      थांबा;
    स्थिती CURL_READFUNC_ABORT:
    स्थिती CURL_READFUNC_PAUSE:
    स्थिती READ_ERROR:
    स्थिती STOP_FILLING:
      परत cursize ? cursize : sz;
    पूर्वनिर्धारित:
      st->bufend += sz;
      थांबा;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
स्थिर माप_प्रकार readback_part(curl_mimepart *part,
                            अक्षर *buffer, माप_प्रकार bufsize, bool *hasread)
{
  माप_प्रकार cursize = 0;

  /* Readback from part. */
  पर्यंत(bufsize) {
    माप_प्रकार sz = 0;
    संरचना curl_slist *hdr = (संरचना curl_slist *)part->state.ptr;
    निवड(part->state.state) {
    स्थिती MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      थांबा;
    स्थिती MIMESTATE_USERHEADERS:
      जर(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, रिक्त);
        थांबा;
      }
      जर(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        थांबा;
      }
      FALLTHROUGH();
    स्थिती MIMESTATE_CURLHEADERS:
      जर(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      अन्यथा {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, म_माप(hdr->data), STRCONST("\r\n"));
        जर(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      थांबा;
    स्थिती MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      जर(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, रिक्त);
      थांबा;
    स्थिती MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, रिक्त);
      थांबा;
    स्थिती MIMESTATE_CONTENT:
      जर(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      अन्यथा
        sz = read_part_content(part, buffer, bufsize, hasread);
      निवड(sz) {
      स्थिती 0:
        mimesetstate(&part->state, MIMESTATE_END, रिक्त);
        /* Try sparing open file descriptors. */
        जर(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = रिक्त;
        }
        FALLTHROUGH();
      स्थिती CURL_READFUNC_ABORT:
      स्थिती CURL_READFUNC_PAUSE:
      स्थिती READ_ERROR:
      स्थिती STOP_FILLING:
        परत cursize ? cursize : sz;
      }
      थांबा;
    स्थिती MIMESTATE_END:
      परत cursize;
    पूर्वनिर्धारित:
      थांबा;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  परत cursize;
}

/* Readback from mime. Warning: not a read callback function. */
स्थिर माप_प्रकार mime_subparts_read(अक्षर *buffer, माप_प्रकार size, माप_प्रकार nitems,
                                 शून्य *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  माप_प्रकार cursize = 0;
  (शून्य)size;  /* Always 1 */

  पर्यंत(nitems) {
    माप_प्रकार sz = 0;
    curl_mimepart *part = mime->state.ptr;
    निवड(mime->state.state) {
    स्थिती MIMESTATE_BEGIN:
    स्थिती MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      थांबा;
    स्थिती MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      जर(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      थांबा;
    स्थिती MIMESTATE_BOUNDARY2:
      जर(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      अन्यथा
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      जर(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      थांबा;
    स्थिती MIMESTATE_CONTENT:
      जर(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, रिक्त);
        थांबा;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      निवड(sz) {
      स्थिती CURL_READFUNC_ABORT:
      स्थिती CURL_READFUNC_PAUSE:
      स्थिती READ_ERROR:
      स्थिती STOP_FILLING:
        परत cursize ? cursize : sz;
      स्थिती 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        थांबा;
      }
      थांबा;
    स्थिती MIMESTATE_END:
      परत cursize;
    पूर्वनिर्धारित:
      थांबा;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  परत cursize;
}

स्थिर पूर्णांक mime_part_rewind(curl_mimepart *part)
{
  पूर्णांक res = CURL_SEEKFUNC_OK;
  गणना mimestate targetstate = MIMESTATE_BEGIN;

  जर(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  जर(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    जर(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, शुरू_से);
      निवड(res) {
      स्थिती CURL_SEEKFUNC_OK:
      स्थिती CURL_SEEKFUNC_FAIL:
      स्थिती CURL_SEEKFUNC_CANTSEEK:
        थांबा;
      स्थिती -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        थांबा;
      पूर्वनिर्धारित:
        res = CURL_SEEKFUNC_FAIL;
        थांबा;
      }
    }
  }

  जर(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, रिक्त);

  part->lastreadstatus = 1; /* Successful read status. */
  परत res;
}

स्थिर पूर्णांक mime_subparts_seek(शून्य *instream, curl_off_t offset, पूर्णांक whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  पूर्णांक rc = CURL_SEEKFUNC_OK;

  जर(whence != शुरू_से || offset)
    परत CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  जर(mime->state.state == MIMESTATE_BEGIN)
    परत CURL_SEEKFUNC_OK;           /* Already rewound. */

  साठी(part = mime->firstpart; part; part = part->nextpart) {
    पूर्णांक res = mime_part_rewind(part);
    जर(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  जर(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, रिक्त);

  परत rc;
}

/* Release part content. */
स्थिर शून्य cleanup_part_content(curl_mimepart *part)
{
  जर(part->freefunc)
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
  part->flags &= ~(अचिन्हित पूर्णांक)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

स्थिर शून्य mime_subparts_free(शून्य *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  जर(mime && mime->parent) {
    mime->parent->freefunc = रिक्त;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
स्थिर शून्य mime_subparts_unbind(शून्य *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  जर(mime && mime->parent) {
    mime->parent->freefunc = रिक्त;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = रिक्त;
  }
}

शून्य Curl_mime_cleanpart(curl_mimepart *part)
{
  जर(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    जर(part->flags & MIME_USERHEADERS_OWNER)
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

  जर(!mime)
    परत;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  पर्यंत(mime) {
    part = mime->firstpart;
    जर(part) {
      mime->firstpart = part->nextpart;
      जर(part->kind == MIMEKIND_MULTIPART && part->arg &&
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

स्थिर bool mime_is_too_deep(स्थिरांक curl_mimepart *part, माप_प्रकार call_depth)
{
  DEBUGASSERT(part);

  जर(++call_depth > MAX_MIME_LEVELS)
    परत TRUE;

  जर(part->kind == MIMEKIND_MULTIPART && part->arg) {
    स्थिरांक curl_mime *mime = (स्थिरांक curl_mime *)part->arg;
    स्थिरांक curl_mimepart *s;

    साठी(s = mime->firstpart; s; s = s->nextpart) {
      जर(mime_is_too_deep(s, call_depth))
        परत TRUE;
    }
  }

  परत FALSE;
}

स्थिर CURLcode mime_duppart(संरचना Curl_easy *data,
                             curl_mimepart *dst, स्थिरांक curl_mimepart *src,
                             माप_प्रकार call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  स्थिरांक curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  जर(++call_depth > MAX_MIME_LEVELS)
    परत CURLE_TOO_LARGE;

  /* Duplicate content. */
  निवड(src->kind) {
  स्थिती MIMEKIND_NONE:
    थांबा;
  स्थिती MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (माप_प्रकार)src->datasize);
    थांबा;
  स्थिती MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    जर(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    थांबा;
  स्थिती MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    थांबा;
  स्थिती MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    साठी(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    थांबा;
  पूर्वनिर्धारित:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    थांबा;
  }

  /* Duplicate headers. */
  जर(!result && src->userheaders) {
    संरचना curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    जर(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      जर(result)
        curl_slist_free_all(hdrs);
    }
  }

  जर(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  जर(!result)
    result = curl_mime_name(dst, src->name);
  जर(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  जर(result)
    Curl_mime_cleanpart(dst);

  परत result;
}

CURLcode Curl_mime_duppart(संरचना Curl_easy *data,
                           curl_mimepart *dst, स्थिरांक curl_mimepart *src)
{
  जर(!dst || !src)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  जर(mime_is_too_deep(src, 0))
    परत CURLE_TOO_LARGE;

  परत mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(शून्य *easy)
{
  curl_mime *mime = curlx_malloc(आकार(*mime));

  जर(mime) {
    mime->parent = रिक्त;
    mime->firstpart = रिक्त;
    mime->lastpart = रिक्त;

    स_रखो(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    जर(Curl_rand_alnum(easy,
                       (अचिन्हित अक्षर *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      परत रिक्त;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, रिक्त);
  }

  परत mime;
}

/* Initialize a mime part. */
शून्य Curl_mime_initpart(curl_mimepart *part)
{
  स_रखो(part, 0, आकार(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, रिक्त);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  जर(!mime)
    परत रिक्त;

  part = curlx_malloc(आकार(*part));

  जर(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    जर(mime->lastpart)
      mime->lastpart->nextpart = part;
    अन्यथा
      mime->firstpart = part;

    mime->lastpart = part;
  }

  परत part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, स्थिरांक अक्षर *name)
{
  जर(!part)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  जर(name) {
    part->name = curlx_strdup(name);
    जर(!part->name)
      परत CURLE_OUT_OF_MEMORY;
  }

  परत CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, स्थिरांक अक्षर *filename)
{
  जर(!part)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  जर(filename) {
    part->filename = curlx_strdup(filename);
    जर(!part->filename)
      परत CURLE_OUT_OF_MEMORY;
  }

  परत CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, स्थिरांक अक्षर *data, माप_प्रकार datasize)
{
  जर(!part)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  जर(data) {
    जर(datasize == CURL_ZERO_TERMINATED)
      datasize = म_माप(data);

    part->data = curlx_memdup0(data, datasize);
    जर(!part->data)
      परत CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  परत CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, स्थिरांक अक्षर *filename)
{
  CURLcode result = CURLE_OK;

  जर(!part)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  जर(filename) {
    अक्षर *base;
    curlx_struct_stat sbuf;

    जर(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    अन्यथा {
      part->data = curlx_strdup(filename);
      जर(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      अन्यथा {
        part->datasize = -1;
        जर(S_ISREG(sbuf.st_mode)) {
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
        जर(!base)
          result = CURLE_OUT_OF_MEMORY;
        अन्यथा {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  परत result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, स्थिरांक अक्षर *mimetype)
{
  जर(!part)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  जर(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    जर(!part->mimetype)
      परत CURLE_OUT_OF_MEMORY;
  }

  परत CURLE_OK;
}

स्थिर स्थिरांक संरचना mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, स्थिरांक अक्षर *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  स्थिरांक संरचना mime_encoder *mep;

  जर(!part)
    परत result;

  part->encoder = रिक्त;

  जर(!encoding)
    परत CURLE_OK;    /* Removing current encoder. */

  साठी(mep = encoders; mep->name; mep++)
    जर(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  परत result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           संरचना curl_slist *headers, पूर्णांक take_ownership)
{
  जर(!part)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  जर(part->flags & MIME_USERHEADERS_OWNER) {
    जर(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(अचिन्हित पूर्णांक)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  जर(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  परत CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, शून्य *arg)
{
  जर(!part)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  जर(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  परत CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, पूर्णांक take_ownership)
{
  curl_mime *मूळ;

  जर(!part)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  जर(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    परत CURLE_OK;

  cleanup_part_content(part);

  जर(subparts) {
    /* Should not have been attached already. */
    जर(subparts->parent)
      परत CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    मूळ = part->parent;
    जर(मूळ) {
      पर्यंत(मूळ->parent && मूळ->parent->parent)
        मूळ = मूळ->parent->parent;
      जर(subparts == मूळ) {
        /* cannot add as a subpart of itself. */
        परत CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    जर(mime_subparts_seek(subparts, (curl_off_t)0, शुरू_से) !=
       CURL_SEEKFUNC_OK)
      परत CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  परत CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  परत Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
माप_प्रकार Curl_mime_read(अक्षर *buffer, माप_प्रकार size, माप_प्रकार nitems, शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  माप_प्रकार ret;
  bool hasread;

  (शून्य)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  करा {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } पर्यंत(ret == STOP_FILLING);

  परत ret;
}

/* Rewind mime stream. */
स्थिर CURLcode mime_rewind(curl_mimepart *part)
{
  परत mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
स्थिर माप_प्रकार slist_size(संरचना curl_slist *s,
                         माप_प्रकार overhead, स्थिरांक अक्षर *skip, माप_प्रकार skiplen)
{
  माप_प्रकार size = 0;

  साठी(; s; s = s->next)
    जर(!skip || !match_header(s, skip, skiplen))
      size += म_माप(s->data) + overhead;
  परत size;
}

/* Get/compute multipart size. */
स्थिर curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  जर(!mime)
    परत 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  साठी(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    जर(sz < 0)
      size = sz;

    जर(size >= 0)
      size += boundarysize + sz;
  }

  परत size;
}

/* Get/compute mime size. */
स्थिर curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  जर(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  जर(part->encoder)
    size = part->encoder->sizefunc(part);

  जर(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, रिक्त, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  परत size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(संरचना curl_slist **slp, स्थिरांक अक्षर *fmt, ...)
{
  संरचना curl_slist *hdr = रिक्त;
  अक्षर *s = रिक्त;
  बहु_सूची ap;

  बहु_शुरू(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  बहु_पूर्ण(ap);

  जर(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    जर(hdr)
      *slp = hdr;
    अन्यथा
      curlx_free(s);
  }

  परत hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
स्थिर CURLcode add_content_type(संरचना curl_slist **slp,
                                 स्थिरांक अक्षर *type, स्थिरांक अक्षर *boundary)
{
  परत Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

स्थिरांक अक्षर *Curl_mime_contenttype(स्थिरांक अक्षर *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  संरचना ContentType {
    स्थिरांक अक्षर *extension;
    स्थिरांक अक्षर *type;
  };
  स्थिर स्थिरांक संरचना ContentType ctts[] = {
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

  जर(filename) {
    माप_प्रकार len1 = म_माप(filename);
    स्थिरांक अक्षर *nameend = filename + len1;
    अचिन्हित पूर्णांक i;

    साठी(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      माप_प्रकार len2 = म_माप(ctts[i].extension);

      जर(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        परत ctts[i].type;
    }
  }
  परत रिक्त;
}

स्थिर bool content_type_match(स्थिरांक अक्षर *contenttype,
                               स्थिरांक अक्षर *target, माप_प्रकार len)
{
  जर(contenttype && curl_strnequal(contenttype, target, len))
    निवड(contenttype[len]) {
    स्थिती '\0':
    स्थिती '\t':
    स्थिती '\r':
    स्थिती '\n':
    स्थिती ' ':
    स्थिती ';':
      परत TRUE;
    }
  परत FALSE;
}

स्थिर CURLcode add_content_disposition(संरचना Curl_easy *data,
                                        curl_mimepart *part,
                                        स्थिरांक अक्षर *disposition,
                                        स्थिरांक अक्षर *contenttype,
                                        गणना mimestrategy strategy)
{
  जर(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  जर(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = रिक्त;

  जर(disposition) {
    CURLcode result = CURLE_OK;
    अक्षर *name = रिक्त;
    अक्षर *filename = रिक्त;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    जर(backslash &&
       ((part->name && part->name[म_खोज(part->name, "\r\n")]) ||
        (part->filename && part->filename[म_खोज(part->filename, "\r\n")])))
      परत CURLE_BAD_FUNCTION_ARGUMENT;

    जर(part->name) {
      name = escape_string(data, part->name, strategy);
      जर(!name)
        परत CURLE_OUT_OF_MEMORY;
    }
    जर(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      जर(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    जर(!result)
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
    जर(result)
      परत result;
  }
  परत CURLE_OK;
}

स्थिर CURLcode mime_prepare_headers(संरचना Curl_easy *data,
                                     curl_mimepart *part,
                                     स्थिरांक अक्षर *contenttype,
                                     स्थिरांक अक्षर *disposition,
                                     गणना mimestrategy strategy,
                                     माप_प्रकार call_depth)
{
  curl_mime *mime = रिक्त;
  स्थिरांक अक्षर *boundary = रिक्त;
  अक्षर *customct;
  स्थिरांक अक्षर *cte = रिक्त;
  CURLcode result = CURLE_OK;

  जर(++call_depth > MAX_MIME_LEVELS)
    परत CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = रिक्त;

  /* Be sure we will not access old headers later. */
  जर(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, रिक्त);

  /* Check if content type is specified. */
  customct = part->mimetype;
  जर(!customct)
    customct = शोध_header(part->userheaders, STRCONST("Content-Type"));
  जर(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  जर(!contenttype) {
    निवड(part->kind) {
    स्थिती MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      थांबा;
    स्थिती MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      जर(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      जर(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      थांबा;
    पूर्वनिर्धारित:
      contenttype = Curl_mime_contenttype(part->filename);
      थांबा;
    }
  }

  जर(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    जर(mime)
      boundary = mime->boundary;
  }
  अन्यथा जर(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = रिक्त;

  /* Issue content-disposition header only if not already set by caller. */
  जर(!शोध_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    जर(result)
      परत result;
  }

  /* Issue Content-Type header. */
  जर(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    जर(result)
      परत result;
  }

  /* Content-Transfer-Encoding header. */
  जर(!शोध_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    जर(part->encoder)
      cte = part->encoder->name;
    अन्यथा जर(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    जर(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      जर(result)
        परत result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  जर(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  जर(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = रिक्त;
    जर(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    साठी(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, रिक्त,
                                    disposition, strategy, call_depth);
      जर(result)
        परत result;
    }
  }
  परत result;
}

CURLcode Curl_mime_prepare_headers(संरचना Curl_easy *data,
                                   curl_mimepart *part,
                                   स्थिरांक अक्षर *contenttype,
                                   स्थिरांक अक्षर *disposition,
                                   गणना mimestrategy strategy)
{
  परत mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
स्थिर शून्य mime_unpause(curl_mimepart *part)
{
  जर(part) {
    जर(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    जर(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      जर(mime) {
        curl_mimepart *subpart;

        साठी(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
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
  परत CURLE_OK;
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
                             अक्षर *buf, माप_प्रकार blen,
                             माप_प्रकार *pnread, bool *peos)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  माप_प्रकार nread;
  अक्षर tmp[256];

  /* Once we have errored, we will return the same error forever */
  जर(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (पूर्णांक)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    परत ctx->error_result;
  }
  जर(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    परत CURLE_OK;
  }
  /* respect length limitations */
  जर(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    जर(remain <= 0)
      blen = 0;
    अन्यथा जर(remain < (curl_off_t)blen)
      blen = (माप_प्रकार)remain;
  }

  जर(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (अचिन्हित अक्षर *)buf, blen, &nread);
    जर(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      परत result;
    }
  }
  अन्यथा जर(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, आकार(tmp), ctx->part);
    जर(nread <= आकार(tmp)) {
      माप_प्रकार n;
      result = Curl_bufq_write(&ctx->tmpbuf, (अचिन्हित अक्षर *)tmp, nread, &n);
      जर(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        परत result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      जर(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        परत result;
      }
    }
  }
  अन्यथा
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  निवड(nread) {
  स्थिती 0:
    जर((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      परत CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    थांबा;

  स्थिती CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    थांबा;

  स्थिती CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    थांबा; /* nothing was read */

  स्थिती STOP_FILLING:
  स्थिती READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    थांबा;

  पूर्वनिर्धारित:
    जर(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      परत CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    जर(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    थांबा;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (पूर्णांक)result, *pnread, *peos);
  परत result;
}

स्थिर bool cr_mime_needs_rewind(संरचना Curl_easy *data,
                                 संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  परत ctx->read_len > 0;
}

स्थिर curl_off_t cr_mime_total_length(संरचना Curl_easy *data,
                                       संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  परत ctx->total_len;
}

स्थिर CURLcode cr_mime_resume_from(संरचना Curl_easy *data,
                                    संरचना Curl_creader *reader,
                                    curl_off_t offset)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;

  जर(offset > 0) {
    curl_off_t passed = 0;

    करा {
      अक्षर scratch[4 * 1024];
      माप_प्रकार readthisamountnow =
        (offset - passed > (curl_off_t)आकार(scratch)) ?
        आकार(scratch) :
        curlx_sotouz(offset - passed);
      माप_प्रकार nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      जर((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        परत CURLE_READ_ERROR;
      }
    } पर्यंत(passed < offset);

    /* now, decrease the size of the read */
    जर(ctx->total_len > 0) {
      ctx->total_len -= offset;

      जर(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        परत CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  परत CURLE_OK;
}

स्थिर CURLcode cr_mime_cntrl(संरचना Curl_easy *data,
                              संरचना Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  निवड(opcode) {
  स्थिती CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    जर(result)
      failf(data, "Cannot rewind mime/post data");
    परत result;
  }
  स्थिती CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    थांबा;
  स्थिती CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत CURLE_OK;
}

स्थिर bool cr_mime_is_paused(संरचना Curl_easy *data,
                              संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)data;
  परत ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

स्थिर स्थिरांक संरचना Curl_crtype cr_mime = {
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
  आकार(संरचना cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(संरचना Curl_easy *data, curl_mimepart *part)
{
  संरचना Curl_creader *r;
  संरचना cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  जर(result)
    परत result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  जर(result) {
    Curl_creader_free(data, r);
    परत result;
  }
  ctx->total_len = mime_size(ctx->part);

  परत Curl_creader_set(data, r);
}

#अन्यथा /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (शून्य)easy;
  परत रिक्त;
}

शून्य curl_mime_free(curl_mime *mime)
{
  (शून्य)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (शून्य)mime;
  परत रिक्त;
}

CURLcode curl_mime_name(curl_mimepart *part, स्थिरांक अक्षर *name)
{
  (शून्य)part;
  (शून्य)name;
  परत CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, स्थिरांक अक्षर *filename)
{
  (शून्य)part;
  (शून्य)filename;
  परत CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, स्थिरांक अक्षर *mimetype)
{
  (शून्य)part;
  (शून्य)mimetype;
  परत CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, स्थिरांक अक्षर *encoding)
{
  (शून्य)part;
  (शून्य)encoding;
  परत CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, स्थिरांक अक्षर *data, माप_प्रकार datasize)
{
  (शून्य)part;
  (शून्य)data;
  (शून्य)datasize;
  परत CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, स्थिरांक अक्षर *filename)
{
  (शून्य)part;
  (शून्य)filename;
  परत CURLE_NOT_BUILT_IN;
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
  परत CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (शून्य)part;
  (शून्य)subparts;
  परत CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           संरचना curl_slist *headers, पूर्णांक take_ownership)
{
  (शून्य)part;
  (शून्य)headers;
  (शून्य)take_ownership;
  परत CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(संरचना curl_slist **slp, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)slp;
  (शून्य)fmt;
  परत CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

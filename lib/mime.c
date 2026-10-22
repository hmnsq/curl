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
#समावेश "curl_setup.h"

काष्ठा Curl_easy;

#समावेश "mime.h"
#समावेश "urldata.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "transfer.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/basename.h"
#समावेश "curlx/strcopy.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "curlx/base64.h"

#अगर !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#समावेश "rand.h"
#समावेश "slist.h"
#समावेश "curlx/dynbuf.h"

#घोषणा MAX_MIME_LEVELS 40 /* avoid deep nesting */

#घोषणा READ_ERROR   ((माप_प्रकार)-1)
#घोषणा STOP_FILLING ((माप_प्रकार)-2)

अटल माप_प्रकार mime_subparts_read(अक्षर *बफर, माप_प्रकार आकार, माप_प्रकार nitems,
                                 व्योम *instream, bool *hasread);
अटल curl_off_t mime_आकार(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#घोषणा QP_OK           1       /* Can be represented by itself. */
#घोषणा QP_SP           2       /* Space or tab. */
#घोषणा QP_CR           3       /* Carriage return. */
#घोषणा QP_LF           4       /* Line-feed. */
अटल स्थिर अचिन्हित अक्षर qp_class[] = {
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
अटल स्थिर अक्षर aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#अगर_अघोषित __VMS
#घोषणा filesize(name, stat_तथ्य) stat_तथ्य.st_आकार
#घोषणा fopen_read                curlx_fopen

#अन्यथा

#समावेश <fabdef.h>
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
curl_off_t Vms_Real_File_आकार(स्थिर अक्षर *name, स्थिर curlx_struct_stat *stat_buf)
{
  अक्षर बफर[8192];
  curl_off_t गिनती;
  पूर्णांक ret_stat;
  खाता *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  अगर(!file)
    वापस 0;

  गिनती = 0;
  ret_stat = 1;
  जबतक(ret_stat > 0) {
    ret_stat = ख_पढ़ो(बफर, 1, माप(बफर), file);
    अगर(ret_stat)
      गिनती += ret_stat;
  }
  curlx_fclose(file);

  वापस गिनती;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
अटल curl_off_t Vms_Special_आकार(स्थिर अक्षर *name,
                                 स्थिर curlx_struct_stat *stat_buf)
{
  चयन(stat_buf->st_fab_rfm) {
  हाल FAB$C_VAR:
  हाल FAB$C_VFC:
    वापस Vms_Real_File_आकार(name, stat_buf);
    अवरोध;
  शेष:
    वापस stat_buf->st_आकार;
  }
}

#घोषणा filesize(name, stat_तथ्य) Vms_Special_आकार(name, &stat_तथ्य)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
अटल खाता *vmsfopenread(स्थिर अक्षर *file, स्थिर अक्षर *mode)
{
  curlx_struct_stat statbuf;
  पूर्णांक res = curlx_stat(file, &statbuf);

  अगर(res != -1) {
    चयन(statbuf.st_fab_rfm) {
    हाल FAB$C_VAR:
    हाल FAB$C_VFC:
    हाल FAB$C_STMCR:
      वापस curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      अवरोध;
    शेष:
      वापस curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  वापस शून्य;
}

#घोषणा fopen_read vmsfopenread
#पूर्ण_अगर /* !__VMS */

/* Set readback state. */
अटल व्योम mimesetstate(काष्ठा mime_state *state,
                         क्रमागत mimestate tok, व्योम *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
अटल अक्षर *escape_string(काष्ठा Curl_easy *तथ्य,
                           स्थिर अक्षर *src, क्रमागत mimestrategy strategy)
{
  CURLcode परिणाम;
  काष्ठा dynbuf db;
  स्थिर अक्षर * स्थिर *table;
  स्थिर अक्षर * स्थिर *p;
  /* replace first character by rest of string. */
  अटल स्थिर अक्षर * स्थिर mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    शून्य
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  अटल स्थिर अक्षर * स्थिर formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    शून्य
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  अगर(strategy == MIMESTRATEGY_MAIL || (तथ्य && तथ्य->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_निवेश_लंबाई);

  क्रम(परिणाम = curlx_dyn_addn(&db, STRCONST("")); !परिणाम && *src; src++) {
    क्रम(p = table; *p && **p != *src; p++)
      ;

    अगर(*p)
      परिणाम = curlx_dyn_add(&db, *p + 1);
    अन्यथा
      परिणाम = curlx_dyn_addn(&db, src, 1);
  }

  वापस curlx_dyn_ptr(&db);
}

/* Check if header matches. */
अटल अक्षर *match_header(काष्ठा curl_slist *hdr, स्थिर अक्षर *lbl, माप_प्रकार len)
{
  अक्षर *मूल्य = शून्य;

  अगर(curl_strnequal(hdr->तथ्य, lbl, len) && hdr->तथ्य[len] == ':')
    क्रम(मूल्य = hdr->तथ्य + len + 1; *मूल्य == ' '; मूल्य++)
      ;
  वापस मूल्य;
}

/* Get a header from an slist. */
अटल अक्षर *खोज_header(काष्ठा curl_slist *hdrlist,
                           स्थिर अक्षर *hdr, माप_प्रकार len)
{
  अक्षर *मूल्य = शून्य;

  क्रम(; !मूल्य && hdrlist; hdrlist = hdrlist->अग्र)
    मूल्य = match_header(hdrlist, hdr, len);

  वापस मूल्य;
}

अटल अक्षर *strippath(स्थिर अक्षर *fullfile)
{
  अक्षर *filename;
  अक्षर *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  अगर(!filename)
    वापस शून्य;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  वापस base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
अटल व्योम cleanup_encoder_state(काष्ठा mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
अटल माप_प्रकार encoder_nop_read(अक्षर *बफर, माप_प्रकार आकार, bool ateof,
                               काष्ठा curl_mimepart *part)
{
  काष्ठा mime_encoder_state *st = &part->encstate;
  माप_प्रकार insize = st->bufend - st->bufbeg;

  (व्योम)ateof;

  अगर(!आकार)
    वापस STOP_FILLING;

  अगर(आकार > insize)
    आकार = insize;

  अगर(आकार)
    स_नकल(बफर, st->buf + st->bufbeg, आकार);

  st->bufbeg += आकार;
  वापस आकार;
}

अटल curl_off_t encoder_nop_आकार(curl_mimepart *part)
{
  वापस part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
अटल माप_प्रकार encoder_7bit_read(अक्षर *बफर, माप_प्रकार आकार, bool ateof,
                                curl_mimepart *part)
{
  काष्ठा mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = st->bufend - st->bufbeg;

  (व्योम)ateof;

  अगर(!आकार)
    वापस STOP_FILLING;

  अगर(आकार > cursize)
    आकार = cursize;

  क्रम(cursize = 0; cursize < आकार; cursize++) {
    *बफर = st->buf[st->bufbeg];
    अगर(*बफर++ & 0x80)
      वापस cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  वापस cursize;
}

/* Base64 content encoder. */
अटल माप_प्रकार encoder_base64_read(अक्षर *बफर, माप_प्रकार आकार, bool ateof,
                                  curl_mimepart *part)
{
  काष्ठा mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = 0;
  पूर्णांक i;
  अक्षर *ptr = बफर;

  जबतक(st->bufbeg < st->bufend) {
    /* Line full ? */
    अगर(st->pos > MAX_ENCODED_पंक्ति_लंबाई - 4) {
      /* Yes, we need 2 characters for CRLF. */
      अगर(आकार < 2) {
        अगर(!cursize)
          वापस STOP_FILLING;
        अवरोध;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      आकार -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    अगर(आकार < 4) {
      अगर(!cursize)
        वापस STOP_FILLING;
      अवरोध;
    }
    अगर(st->bufend - st->bufbeg < 3)
      अवरोध;

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
    आकार -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  अगर(ateof) {
    अगर(आकार < 4) {
      अगर(!cursize)
        वापस STOP_FILLING;
    }
    अन्यथा {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      अगर(st->bufend != st->bufbeg) {

        अगर(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        अगर(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  वापस cursize;
}

/* The maximum input size that does not cause an overflow. */
#घोषणा BASE64_MAX_निवेश_आकार                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_पंक्ति_लंबाई + 2)) *                  \
    MAX_ENCODED_पंक्ति_लंबाई / 4) * 3 - 3)

अटल curl_off_t encoder_base64_आकार(curl_mimepart *part)
{
  curl_off_t आकार = part->datasize;

  अगर(आकार <= 0)
    वापस आकार;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  अगर(आकार > BASE64_MAX_निवेश_आकार)
    वापस -1;

  /* Compute base64 character count. */
  आकार = 4 * (1 + ((आकार - 1) / 3));

  /* Effective character count must include CRLFs. */
  वापस आकार + (2 * ((आकार - 1) / MAX_ENCODED_पंक्ति_लंबाई));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
अटल पूर्णांक qp_lookahead_eol(काष्ठा mime_encoder_state *st, पूर्णांक ateof, माप_प्रकार n)
{
  n += st->bufbeg;
  अगर(n >= st->bufend && ateof)
    वापस 1;
  अगर(n + 2 > st->bufend)
    वापस ateof ? 0 : -1;
  अगर(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    वापस 1;
  वापस 0;
}

/* Quoted-printable encoder. */
अटल माप_प्रकार encoder_qp_read(अक्षर *बफर, माप_प्रकार आकार, bool ateof,
                              curl_mimepart *part)
{
  काष्ठा mime_encoder_state *st = &part->encstate;
  अक्षर *ptr = बफर;
  माप_प्रकार cursize = 0;
  पूर्णांक softlinebreak;
  अक्षर buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  जबतक(st->bufbeg < st->bufend) {
    माप_प्रकार len = 1;
    माप_प्रकार consumed = 1;
    पूर्णांक i = (अचिन्हित अक्षर)st->buf[st->bufbeg];
    buf[0] = (अक्षर)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    चयन(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    हाल QP_OK:          /* Not a special character. */
      अवरोध;
    हाल QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      चयन(qp_lookahead_eol(st, ateof, 1)) {
      हाल -1:          /* More input data needed. */
        वापस cursize;
      हाल 0:           /* No encoding needed. */
        अवरोध;
      शेष:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        अवरोध;
      }
      अवरोध;
    हाल QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      चयन(qp_lookahead_eol(st, ateof, 0)) {
      हाल -1:          /* Need more data. */
        वापस cursize;
      हाल 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        अवरोध;
      शेष:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        अवरोध;
      }
      अवरोध;
    शेष:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      अवरोध;
    }

    /* Be sure the encoded character fits within maximum line length. */
    अगर(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_पंक्ति_लंबाई;
      अगर(!softlinebreak && st->pos + len == MAX_ENCODED_पंक्ति_लंबाई) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        चयन(qp_lookahead_eol(st, ateof, consumed)) {
        हाल -1:        /* Need more data. */
          वापस cursize;
        हाल 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          अवरोध;
        }
      }
      अगर(softlinebreak) {
        curlx_strcopy(buf, माप(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    अगर(len > आकार) {
      अगर(!cursize)
        वापस STOP_FILLING;
      अवरोध;
    }

    /* Append to output buffer. */
    स_नकल(ptr, buf, len);
    cursize += len;
    ptr += len;
    आकार -= len;
    st->pos += len;
    अगर(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  वापस cursize;
}

अटल curl_off_t encoder_qp_आकार(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  वापस part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
अटल माप_प्रकार mime_mem_read(अक्षर *बफर, माप_प्रकार आकार, माप_प्रकार nitems,
                            व्योम *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  माप_प्रकार sz = curlx_sotouz(part->datasize - part->state.offset);
  (व्योम)आकार;  /* Always 1 */

  अगर(!nitems)
    वापस STOP_FILLING;

  अगर(sz > nitems)
    sz = nitems;

  अगर(sz)
    स_नकल(बफर, part->तथ्य + curlx_sotouz(part->state.offset), sz);

  वापस sz;
}

अटल पूर्णांक mime_mem_seek(व्योम *instream, curl_off_t offset, पूर्णांक whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  चयन(whence) {
  हाल प्रस्तुत_से:
    offset += part->state.offset;
    अवरोध;
  हाल अंत_से:
    offset += part->datasize;
    अवरोध;
  }

  अगर(offset < 0 || offset > part->datasize)
    वापस CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  वापस CURL_SEEKFUNC_OK;
}

अटल व्योम mime_mem_free(व्योम *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->तथ्य);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
अटल bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  अगर(part->fp)
    वापस FALSE;
  part->fp = fopen_read(part->तथ्य, "rb");
  वापस part->fp ? FALSE : TRUE;
}

अटल माप_प्रकार mime_file_read(अक्षर *बफर, माप_प्रकार आकार, माप_प्रकार nitems,
                             व्योम *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  अगर(!nitems)
    वापस STOP_FILLING;

  अगर(mime_open_file(part))
    वापस READ_ERROR;

  वापस ख_पढ़ो(बफर, आकार, nitems, part->fp);
}

अटल पूर्णांक mime_file_seek(व्योम *instream, curl_off_t offset, पूर्णांक whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  अगर(whence == शुरू_से && !offset && !part->fp)
    वापस CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  अगर(mime_open_file(part))
    वापस CURL_SEEKFUNC_FAIL;

  वापस curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

अटल व्योम mime_file_free(व्योम *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  अगर(part->fp) {
    curlx_fclose(part->fp);
    part->fp = शून्य;
  }
  curlx_safefree(part->तथ्य);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
अटल माप_प्रकार readback_bytes(काष्ठा mime_state *state,
                             अक्षर *बफर, माप_प्रकार bufsize,
                             स्थिर अक्षर *bytes, माप_प्रकार numbytes,
                             स्थिर अक्षर *trail, माप_प्रकार traillen)
{
  माप_प्रकार sz;
  माप_प्रकार offset = curlx_sotouz(state->offset);

  अगर(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  अन्यथा {
    sz = offset - numbytes;
    अगर(sz >= traillen)
      वापस 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  अगर(sz > bufsize)
    sz = bufsize;

  स_नकल(बफर, bytes, sz);
  state->offset += sz;
  वापस sz;
}

/* Read a non-encoded part content. */

अटल माप_प्रकार read_part_content(curl_mimepart *part, अक्षर *बफर,
                                माप_प्रकार bufsize, bool *hasread)
{
  माप_प्रकार sz = 0;

  चयन(part->lastreadstatus) {
  हाल 0:
  हाल CURL_READFUNC_ABORT:
  हाल CURL_READFUNC_PAUSE:
  हाल READ_ERROR:
    वापस part->lastreadstatus;
  शेष:
    अवरोध;
  }

  /* If we can determine we are at end of part data, spare a read. */
  अगर(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  अन्यथा {
    चयन(part->kind) {
    हाल MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(बफर, 1, bufsize, part->arg, hasread);
      अवरोध;
    हाल MIMEKIND_FILE:
      अगर(part->fp && ख_पूर्ण(part->fp))
        अवरोध;  /* At EOF. */
      FALLTHROUGH();
    शेष:
      अगर(part->readfunc) {
        अगर(!(part->flags & MIME_FAST_READ)) {
          अगर(*hasread)
            वापस STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(बफर, 1, bufsize, part->arg);
      }
      अवरोध;
    }
  }

  चयन(sz) {
  हाल STOP_FILLING:
    अवरोध;
  हाल 0:
  हाल CURL_READFUNC_ABORT:
  हाल CURL_READFUNC_PAUSE:
  हाल READ_ERROR:
    part->lastreadstatus = sz;
    अवरोध;
  शेष:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    अवरोध;
  }

  वापस sz;
}

/* Read and encode part content. */
अटल माप_प्रकार read_encoded_part_content(curl_mimepart *part, अक्षर *बफर,
                                        माप_प्रकार bufsize, bool *hasread)
{
  काष्ठा mime_encoder_state *st = &part->encstate;
  माप_प्रकार cursize = 0;
  माप_प्रकार sz;
  bool ateof = FALSE;

  क्रम(;;) {
    अगर(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(बफर, bufsize, ateof, part);
      चयन(sz) {
      हाल 0:
        अगर(ateof)
          वापस cursize;
        अवरोध;
      हाल READ_ERROR:
      हाल STOP_FILLING:
        वापस cursize ? cursize : sz;
      शेष:
        cursize += sz;
        बफर += sz;
        bufsize -= sz;
        जारी;
      }
    }

    /* We need more data in input buffer. */
    अगर(st->bufbeg) {
      माप_प्रकार len = st->bufend - st->bufbeg;

      अगर(len)
        स_हटाओ(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    अगर(st->bufend >= माप(st->buf))
      वापस cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           माप(st->buf) - st->bufend, hasread);
    चयन(sz) {
    हाल 0:
      ateof = TRUE;
      अवरोध;
    हाल CURL_READFUNC_ABORT:
    हाल CURL_READFUNC_PAUSE:
    हाल READ_ERROR:
    हाल STOP_FILLING:
      वापस cursize ? cursize : sz;
    शेष:
      st->bufend += sz;
      अवरोध;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
अटल माप_प्रकार readback_part(curl_mimepart *part,
                            अक्षर *बफर, माप_प्रकार bufsize, bool *hasread)
{
  माप_प्रकार cursize = 0;

  /* Readback from part. */
  जबतक(bufsize) {
    माप_प्रकार sz = 0;
    काष्ठा curl_slist *hdr = (काष्ठा curl_slist *)part->state.ptr;
    चयन(part->state.state) {
    हाल MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      अवरोध;
    हाल MIMESTATE_USERHEADERS:
      अगर(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, शून्य);
        अवरोध;
      }
      अगर(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->अग्र);
        अवरोध;
      }
      FALLTHROUGH();
    हाल MIMESTATE_CURLHEADERS:
      अगर(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      अन्यथा {
        sz = readback_bytes(&part->state, बफर, bufsize,
                            hdr->तथ्य, म_माप(hdr->तथ्य), STRCONST("\r\n"));
        अगर(!sz)
          mimesetstate(&part->state, part->state.state, hdr->अग्र);
      }
      अवरोध;
    हाल MIMESTATE_EOH:
      sz = readback_bytes(&part->state, बफर, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      अगर(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, शून्य);
      अवरोध;
    हाल MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, शून्य);
      अवरोध;
    हाल MIMESTATE_CONTENT:
      अगर(part->encoder)
        sz = read_encoded_part_content(part, बफर, bufsize, hasread);
      अन्यथा
        sz = read_part_content(part, बफर, bufsize, hasread);
      चयन(sz) {
      हाल 0:
        mimesetstate(&part->state, MIMESTATE_END, शून्य);
        /* Try sparing open file descriptors. */
        अगर(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = शून्य;
        }
        FALLTHROUGH();
      हाल CURL_READFUNC_ABORT:
      हाल CURL_READFUNC_PAUSE:
      हाल READ_ERROR:
      हाल STOP_FILLING:
        वापस cursize ? cursize : sz;
      }
      अवरोध;
    हाल MIMESTATE_END:
      वापस cursize;
    शेष:
      अवरोध;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    बफर += sz;
    bufsize -= sz;
  }

  वापस cursize;
}

/* Readback from mime. Warning: not a read callback function. */
अटल माप_प्रकार mime_subparts_read(अक्षर *बफर, माप_प्रकार आकार, माप_प्रकार nitems,
                                 व्योम *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  माप_प्रकार cursize = 0;
  (व्योम)आकार;  /* Always 1 */

  जबतक(nitems) {
    माप_प्रकार sz = 0;
    curl_mimepart *part = mime->state.ptr;
    चयन(mime->state.state) {
    हाल MIMESTATE_BEGIN:
    हाल MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      अवरोध;
    हाल MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, बफर, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      अगर(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      अवरोध;
    हाल MIMESTATE_BOUNDARY2:
      अगर(part)
        sz = readback_bytes(&mime->state, बफर, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      अन्यथा
        sz = readback_bytes(&mime->state, बफर, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      अगर(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      अवरोध;
    हाल MIMESTATE_CONTENT:
      अगर(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, शून्य);
        अवरोध;
      }
      sz = readback_part(part, बफर, nitems, hasread);
      चयन(sz) {
      हाल CURL_READFUNC_ABORT:
      हाल CURL_READFUNC_PAUSE:
      हाल READ_ERROR:
      हाल STOP_FILLING:
        वापस cursize ? cursize : sz;
      हाल 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        अवरोध;
      }
      अवरोध;
    हाल MIMESTATE_END:
      वापस cursize;
    शेष:
      अवरोध;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    बफर += sz;
    nitems -= sz;
  }

  वापस cursize;
}

अटल पूर्णांक mime_part_rewind(curl_mimepart *part)
{
  पूर्णांक res = CURL_SEEKFUNC_OK;
  क्रमागत mimestate targetstate = MIMESTATE_BEGIN;

  अगर(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  अगर(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    अगर(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, शुरू_से);
      चयन(res) {
      हाल CURL_SEEKFUNC_OK:
      हाल CURL_SEEKFUNC_FAIL:
      हाल CURL_SEEKFUNC_CANTSEEK:
        अवरोध;
      हाल -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        अवरोध;
      शेष:
        res = CURL_SEEKFUNC_FAIL;
        अवरोध;
      }
    }
  }

  अगर(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, शून्य);

  part->lastreadstatus = 1; /* Successful read status. */
  वापस res;
}

अटल पूर्णांक mime_subparts_seek(व्योम *instream, curl_off_t offset, पूर्णांक whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  पूर्णांक rc = CURL_SEEKFUNC_OK;

  अगर(whence != शुरू_से || offset)
    वापस CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  अगर(mime->state.state == MIMESTATE_BEGIN)
    वापस CURL_SEEKFUNC_OK;           /* Already rewound. */

  क्रम(part = mime->firstpart; part; part = part->nextpart) {
    पूर्णांक res = mime_part_rewind(part);
    अगर(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  अगर(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, शून्य);

  वापस rc;
}

/* Release part content. */
अटल व्योम cleanup_part_content(curl_mimepart *part)
{
  अगर(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = शून्य;
  part->seekfunc = शून्य;
  part->freefunc = शून्य;
  part->arg = (व्योम *)part;           /* Defaults to part itself. */
  part->तथ्य = शून्य;
  part->fp = शून्य;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(अचिन्हित पूर्णांक)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

अटल व्योम mime_subparts_free(व्योम *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  अगर(mime && mime->parent) {
    mime->parent->freefunc = शून्य;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
अटल व्योम mime_subparts_unbind(व्योम *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  अगर(mime && mime->parent) {
    mime->parent->freefunc = शून्य;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = शून्य;
  }
}

व्योम Curl_mime_cleanpart(curl_mimepart *part)
{
  अगर(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    अगर(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
व्योम curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  अगर(!mime)
    वापस;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  जबतक(mime) {
    part = mime->firstpart;
    अगर(part) {
      mime->firstpart = part->nextpart;
      अगर(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = शून्य;
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

अटल bool mime_is_too_deep(स्थिर curl_mimepart *part, माप_प्रकार call_depth)
{
  DEBUGASSERT(part);

  अगर(++call_depth > MAX_MIME_LEVELS)
    वापस TRUE;

  अगर(part->kind == MIMEKIND_MULTIPART && part->arg) {
    स्थिर curl_mime *mime = (स्थिर curl_mime *)part->arg;
    स्थिर curl_mimepart *s;

    क्रम(s = mime->firstpart; s; s = s->nextpart) {
      अगर(mime_is_too_deep(s, call_depth))
        वापस TRUE;
    }
  }

  वापस FALSE;
}

अटल CURLcode mime_duppart(काष्ठा Curl_easy *तथ्य,
                             curl_mimepart *dst, स्थिर curl_mimepart *src,
                             माप_प्रकार call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  स्थिर curl_mimepart *s;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  अगर(++call_depth > MAX_MIME_LEVELS)
    वापस CURLE_TOO_LARGE;

  /* Duplicate content. */
  चयन(src->kind) {
  हाल MIMEKIND_NONE:
    अवरोध;
  हाल MIMEKIND_तथ्य:
    परिणाम = curl_mime_तथ्य(dst, src->तथ्य, (माप_प्रकार)src->datasize);
    अवरोध;
  हाल MIMEKIND_FILE:
    परिणाम = curl_mime_filedata(dst, src->तथ्य);
    /* Do not abort duplication if file is not readable. */
    अगर(परिणाम == CURLE_READ_ERROR)
      परिणाम = CURLE_OK;
    अवरोध;
  हाल MIMEKIND_CALLBACK:
    परिणाम = curl_mime_तथ्य_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    अवरोध;
  हाल MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(तथ्य);
    परिणाम = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    क्रम(s = ((curl_mime *)src->arg)->firstpart; !परिणाम && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      परिणाम = d ? mime_duppart(तथ्य, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    अवरोध;
  शेष:  /* Invalid kind: should not occur. */
    DEBUGF(infof(तथ्य, "invalid MIMEKIND* attempt"));
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    अवरोध;
  }

  /* Duplicate headers. */
  अगर(!परिणाम && src->userheaders) {
    काष्ठा curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    अगर(!hdrs)
      परिणाम = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      परिणाम = curl_mime_headers(dst, hdrs, TRUE);
      अगर(परिणाम)
        curl_slist_free_all(hdrs);
    }
  }

  अगर(!परिणाम) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    परिणाम = curl_mime_type(dst, src->mimetype);
  }
  अगर(!परिणाम)
    परिणाम = curl_mime_name(dst, src->name);
  अगर(!परिणाम)
    परिणाम = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  अगर(परिणाम)
    Curl_mime_cleanpart(dst);

  वापस परिणाम;
}

CURLcode Curl_mime_duppart(काष्ठा Curl_easy *तथ्य,
                           curl_mimepart *dst, स्थिर curl_mimepart *src)
{
  अगर(!dst || !src)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(mime_is_too_deep(src, 0))
    वापस CURLE_TOO_LARGE;

  वापस mime_duppart(तथ्य, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(व्योम *easy)
{
  curl_mime *mime = curlx_malloc(माप(*mime));

  अगर(mime) {
    mime->parent = शून्य;
    mime->firstpart = शून्य;
    mime->lastpart = शून्य;

    स_रखो(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    अगर(Curl_rand_alnum(easy,
                       (अचिन्हित अक्षर *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      वापस शून्य;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, शून्य);
  }

  वापस mime;
}

/* Initialize a mime part. */
व्योम Curl_mime_initpart(curl_mimepart *part)
{
  स_रखो(part, 0, माप(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, शून्य);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  अगर(!mime)
    वापस शून्य;

  part = curlx_malloc(माप(*part));

  अगर(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    अगर(mime->lastpart)
      mime->lastpart->nextpart = part;
    अन्यथा
      mime->firstpart = part;

    mime->lastpart = part;
  }

  वापस part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, स्थिर अक्षर *name)
{
  अगर(!part)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  अगर(name) {
    part->name = curlx_strdup(name);
    अगर(!part->name)
      वापस CURLE_OUT_OF_MEMORY;
  }

  वापस CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, स्थिर अक्षर *filename)
{
  अगर(!part)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  अगर(filename) {
    part->filename = curlx_strdup(filename);
    अगर(!part->filename)
      वापस CURLE_OUT_OF_MEMORY;
  }

  वापस CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_तथ्य(curl_mimepart *part, स्थिर अक्षर *तथ्य, माप_प्रकार datasize)
{
  अगर(!part)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  अगर(तथ्य) {
    अगर(datasize == CURL_ZERO_TERMINATED)
      datasize = म_माप(तथ्य);

    part->तथ्य = curlx_memdup0(तथ्य, datasize);
    अगर(!part->तथ्य)
      वापस CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_तथ्य;
  }

  वापस CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, स्थिर अक्षर *filename)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(!part)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  अगर(filename) {
    अक्षर *base;
    curlx_struct_stat sbuf;

    अगर(curlx_stat(filename, &sbuf))
      परिणाम = CURLE_READ_ERROR;
    अन्यथा {
      part->तथ्य = curlx_strdup(filename);
      अगर(!part->तथ्य)
        परिणाम = CURLE_OUT_OF_MEMORY;
      अन्यथा {
        part->datasize = -1;
        अगर(S_ISREG(sbuf.st_mode)) {
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
        अगर(!base)
          परिणाम = CURLE_OUT_OF_MEMORY;
        अन्यथा {
          परिणाम = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  वापस परिणाम;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, स्थिर अक्षर *mimetype)
{
  अगर(!part)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  अगर(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    अगर(!part->mimetype)
      वापस CURLE_OUT_OF_MEMORY;
  }

  वापस CURLE_OK;
}

अटल स्थिर काष्ठा mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_आकार },
  { "8bit", encoder_nop_read, encoder_nop_आकार },
  { "7bit", encoder_7bit_read, encoder_nop_आकार },
  { "base64", encoder_base64_read, encoder_base64_आकार },
  { "quoted-printable", encoder_qp_read, encoder_qp_आकार },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, स्थिर अक्षर *encoding)
{
  CURLcode परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
  स्थिर काष्ठा mime_encoder *mep;

  अगर(!part)
    वापस परिणाम;

  part->encoder = शून्य;

  अगर(!encoding)
    वापस CURLE_OK;    /* Removing current encoder. */

  क्रम(mep = encoders; mep->name; mep++)
    अगर(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      परिणाम = CURLE_OK;
    }

  वापस परिणाम;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           काष्ठा curl_slist *headers, पूर्णांक take_ownership)
{
  अगर(!part)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(part->flags & MIME_USERHEADERS_OWNER) {
    अगर(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(अचिन्हित पूर्णांक)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  अगर(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  वापस CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_तथ्य_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, व्योम *arg)
{
  अगर(!part)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  अगर(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  वापस CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, पूर्णांक take_ownership)
{
  curl_mime *मूल;

  अगर(!part)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  अगर(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    वापस CURLE_OK;

  cleanup_part_content(part);

  अगर(subparts) {
    /* Should not have been attached already. */
    अगर(subparts->parent)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    मूल = part->parent;
    अगर(मूल) {
      जबतक(मूल->parent && मूल->parent->parent)
        मूल = मूल->parent->parent;
      अगर(subparts == मूल) {
        /* cannot add as a subpart of itself. */
        वापस CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    अगर(mime_subparts_seek(subparts, (curl_off_t)0, शुरू_से) !=
       CURL_SEEKFUNC_OK)
      वापस CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  वापस CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  वापस Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
माप_प्रकार Curl_mime_read(अक्षर *बफर, माप_प्रकार आकार, माप_प्रकार nitems, व्योम *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  माप_प्रकार ret;
  bool hasread;

  (व्योम)आकार;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  करो {
    hasread = FALSE;
    ret = readback_part(part, बफर, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } जबतक(ret == STOP_FILLING);

  वापस ret;
}

/* Rewind mime stream. */
अटल CURLcode mime_rewind(curl_mimepart *part)
{
  वापस mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
अटल माप_प्रकार slist_आकार(काष्ठा curl_slist *s,
                         माप_प्रकार overhead, स्थिर अक्षर *skip, माप_प्रकार skiplen)
{
  माप_प्रकार आकार = 0;

  क्रम(; s; s = s->अग्र)
    अगर(!skip || !match_header(s, skip, skiplen))
      आकार += म_माप(s->तथ्य) + overhead;
  वापस आकार;
}

/* Get/compute multipart size. */
अटल curl_off_t multipart_आकार(curl_mime *mime)
{
  curl_off_t आकार;
  curl_off_t boundarysize;
  curl_mimepart *part;

  अगर(!mime)
    वापस 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  आकार = boundarysize;  /* Final boundary - CRLF after headers. */

  क्रम(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_आकार(part);

    अगर(sz < 0)
      आकार = sz;

    अगर(आकार >= 0)
      आकार += boundarysize + sz;
  }

  वापस आकार;
}

/* Get/compute mime size. */
अटल curl_off_t mime_आकार(curl_mimepart *part)
{
  curl_off_t आकार;

  अगर(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_आकार(part->arg);

  आकार = part->datasize;

  अगर(part->encoder)
    आकार = part->encoder->sizefunc(part);

  अगर(आकार >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    आकार += slist_आकार(part->curlheaders, 2, शून्य, 0);
    आकार += slist_आकार(part->userheaders, 2, STRCONST("Content-Type"));
    आकार += 2;    /* CRLF after headers. */
  }
  वापस आकार;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(काष्ठा curl_slist **slp, स्थिर अक्षर *fmt, ...)
{
  काष्ठा curl_slist *hdr = शून्य;
  अक्षर *s = शून्य;
  बहु_सूची ap;

  बहु_शुरू(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  बहु_पूर्ण(ap);

  अगर(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    अगर(hdr)
      *slp = hdr;
    अन्यथा
      curlx_free(s);
  }

  वापस hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
अटल CURLcode add_content_type(काष्ठा curl_slist **slp,
                                 स्थिर अक्षर *type, स्थिर अक्षर *boundary)
{
  वापस Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

स्थिर अक्षर *Curl_mime_contenttype(स्थिर अक्षर *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  काष्ठा ContentType {
    स्थिर अक्षर *extension;
    स्थिर अक्षर *type;
  };
  अटल स्थिर काष्ठा ContentType ctts[] = {
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

  अगर(filename) {
    माप_प्रकार len1 = म_माप(filename);
    स्थिर अक्षर *nameend = filename + len1;
    अचिन्हित पूर्णांक i;

    क्रम(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      माप_प्रकार len2 = म_माप(ctts[i].extension);

      अगर(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        वापस ctts[i].type;
    }
  }
  वापस शून्य;
}

अटल bool content_type_match(स्थिर अक्षर *contenttype,
                               स्थिर अक्षर *target, माप_प्रकार len)
{
  अगर(contenttype && curl_strnequal(contenttype, target, len))
    चयन(contenttype[len]) {
    हाल '\0':
    हाल '\t':
    हाल '\r':
    हाल '\n':
    हाल ' ':
    हाल ';':
      वापस TRUE;
    }
  वापस FALSE;
}

अटल CURLcode add_content_disposition(काष्ठा Curl_easy *तथ्य,
                                        curl_mimepart *part,
                                        स्थिर अक्षर *disposition,
                                        स्थिर अक्षर *contenttype,
                                        क्रमागत mimestrategy strategy)
{
  अगर(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  अगर(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = शून्य;

  अगर(disposition) {
    CURLcode परिणाम = CURLE_OK;
    अक्षर *name = शून्य;
    अक्षर *filename = शून्य;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (तथ्य && तथ्य->set.mime_formescape);
    अगर(backslash &&
       ((part->name && part->name[म_खोज(part->name, "\r\n")]) ||
        (part->filename && part->filename[म_खोज(part->filename, "\r\n")])))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;

    अगर(part->name) {
      name = escape_string(तथ्य, part->name, strategy);
      अगर(!name)
        वापस CURLE_OUT_OF_MEMORY;
    }
    अगर(part->filename) {
      filename = escape_string(तथ्य, part->filename, strategy);
      अगर(!filename)
        परिणाम = CURLE_OUT_OF_MEMORY;
    }
    अगर(!परिणाम)
      परिणाम = Curl_mime_add_header(&part->curlheaders,
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
    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस CURLE_OK;
}

अटल CURLcode mime_prepare_headers(काष्ठा Curl_easy *तथ्य,
                                     curl_mimepart *part,
                                     स्थिर अक्षर *contenttype,
                                     स्थिर अक्षर *disposition,
                                     क्रमागत mimestrategy strategy,
                                     माप_प्रकार call_depth)
{
  curl_mime *mime = शून्य;
  स्थिर अक्षर *boundary = शून्य;
  अक्षर *customct;
  स्थिर अक्षर *cte = शून्य;
  CURLcode परिणाम = CURLE_OK;

  अगर(++call_depth > MAX_MIME_LEVELS)
    वापस CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = शून्य;

  /* Be sure we will not access old headers later. */
  अगर(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, शून्य);

  /* Check if content type is specified. */
  customct = part->mimetype;
  अगर(!customct)
    customct = खोज_header(part->userheaders, STRCONST("Content-Type"));
  अगर(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  अगर(!contenttype) {
    चयन(part->kind) {
    हाल MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      अवरोध;
    हाल MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      अगर(!contenttype)
        contenttype = Curl_mime_contenttype(part->तथ्य);
      अगर(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      अवरोध;
    शेष:
      contenttype = Curl_mime_contenttype(part->filename);
      अवरोध;
    }
  }

  अगर(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    अगर(mime)
      boundary = mime->boundary;
  }
  अन्यथा अगर(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = शून्य;

  /* Issue content-disposition header only if not already set by caller. */
  अगर(!खोज_header(part->userheaders, STRCONST("Content-Disposition"))) {
    परिणाम = add_content_disposition(तथ्य, part, disposition,
                                     contenttype, strategy);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* Issue Content-Type header. */
  अगर(contenttype) {
    परिणाम = add_content_type(&part->curlheaders, contenttype, boundary);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* Content-Transfer-Encoding header. */
  अगर(!खोज_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    अगर(part->encoder)
      cte = part->encoder->name;
    अन्यथा अगर(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    अगर(cte) {
      परिणाम = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      अगर(परिणाम)
        वापस परिणाम;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  अगर(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  अगर(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = शून्य;
    अगर(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    क्रम(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      परिणाम = mime_prepare_headers(तथ्य, subpart, शून्य,
                                    disposition, strategy, call_depth);
      अगर(परिणाम)
        वापस परिणाम;
    }
  }
  वापस परिणाम;
}

CURLcode Curl_mime_prepare_headers(काष्ठा Curl_easy *तथ्य,
                                   curl_mimepart *part,
                                   स्थिर अक्षर *contenttype,
                                   स्थिर अक्षर *disposition,
                                   क्रमागत mimestrategy strategy)
{
  वापस mime_prepare_headers(तथ्य, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
अटल व्योम mime_unpause(curl_mimepart *part)
{
  अगर(part) {
    अगर(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    अगर(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      अगर(mime) {
        curl_mimepart *subpart;

        क्रम(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

काष्ठा cr_mime_ctx {
  काष्ठा Curl_creader super;
  curl_mimepart *part;
  curl_off_t योग_len;
  curl_off_t read_len;
  CURLcode error_परिणाम;
  काष्ठा bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

अटल CURLcode cr_mime_init(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_creader *reader)
{
  काष्ठा cr_mime_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  ctx->योग_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  वापस CURLE_OK;
}

अटल व्योम cr_mime_close(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_creader *reader)
{
  काष्ठा cr_mime_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
अटल CURLcode cr_mime_read(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_creader *reader,
                             अक्षर *buf, माप_प्रकार blen,
                             माप_प्रकार *pnread, bool *peos)
{
  काष्ठा cr_mime_ctx *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nread;
  अक्षर tmp[256];

  /* Once we have errored, we will return the same error forever */
  अगर(ctx->errored) {
    CURL_TRC_READ(तथ्य, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (पूर्णांक)ctx->error_परिणाम);
    *pnread = 0;
    *peos = FALSE;
    वापस ctx->error_परिणाम;
  }
  अगर(ctx->seen_eos) {
    CURL_TRC_READ(तथ्य, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    वापस CURLE_OK;
  }
  /* respect length limitations */
  अगर(ctx->योग_len >= 0) {
    curl_off_t remain = ctx->योग_len - ctx->read_len;
    अगर(remain <= 0)
      blen = 0;
    अन्यथा अगर(remain < (curl_off_t)blen)
      blen = (माप_प्रकार)remain;
  }

  अगर(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    परिणाम = Curl_bufq_read(&ctx->tmpbuf, (अचिन्हित अक्षर *)buf, blen, &nread);
    अगर(परिणाम) {
      ctx->errored = TRUE;
      ctx->error_परिणाम = परिणाम;
      वापस परिणाम;
    }
  }
  अन्यथा अगर(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(तथ्य, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, माप(tmp), ctx->part);
    अगर(nread <= माप(tmp)) {
      माप_प्रकार n;
      परिणाम = Curl_bufq_write(&ctx->tmpbuf, (अचिन्हित अक्षर *)tmp, nread, &n);
      अगर(परिणाम) {
        ctx->errored = TRUE;
        ctx->error_परिणाम = परिणाम;
        वापस परिणाम;
      }
      /* stored it, read again */
      परिणाम = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      अगर(परिणाम) {
        ctx->errored = TRUE;
        ctx->error_परिणाम = परिणाम;
        वापस परिणाम;
      }
    }
  }
  अन्यथा
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(तथ्य, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  चयन(nread) {
  हाल 0:
    अगर((ctx->योग_len >= 0) && (ctx->read_len < ctx->योग_len)) {
      failf(तथ्य, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->योग_len);
      वापस CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    अवरोध;

  हाल CURL_READFUNC_ABORT:
    failf(तथ्य, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_परिणाम = CURLE_ABORTED_BY_CALLBACK;
    परिणाम = CURLE_ABORTED_BY_CALLBACK;
    अवरोध;

  हाल CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(तथ्य, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    परिणाम = Curl_xfer_pause_send(तथ्य, TRUE);
    अवरोध; /* nothing was read */

  हाल STOP_FILLING:
  हाल READ_ERROR:
    failf(तथ्य, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_परिणाम = CURLE_READ_ERROR;
    परिणाम = CURLE_READ_ERROR;
    अवरोध;

  शेष:
    अगर(nread > blen) {
      /* the read function returned a too large value */
      failf(तथ्य, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_परिणाम = CURLE_READ_ERROR;
      वापस CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    अगर(ctx->योग_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->योग_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    अवरोध;
  }

  CURL_TRC_READ(तथ्य, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->योग_len, ctx->read_len, (पूर्णांक)परिणाम, *pnread, *peos);
  वापस परिणाम;
}

अटल bool cr_mime_needs_rewind(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_creader *reader)
{
  काष्ठा cr_mime_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  वापस ctx->read_len > 0;
}

अटल curl_off_t cr_mime_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_creader *reader)
{
  काष्ठा cr_mime_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  वापस ctx->योग_len;
}

अटल CURLcode cr_mime_resume_from(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा Curl_creader *reader,
                                    curl_off_t offset)
{
  काष्ठा cr_mime_ctx *ctx = reader->ctx;

  अगर(offset > 0) {
    curl_off_t passed = 0;

    करो {
      अक्षर scratch[4 * 1024];
      माप_प्रकार readthisamountnow =
        (offset - passed > (curl_off_t)माप(scratch)) ?
        माप(scratch) :
        curlx_sotouz(offset - passed);
      माप_प्रकार nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      अगर((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(तथ्य, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        वापस CURLE_READ_ERROR;
      }
    } जबतक(passed < offset);

    /* now, decrease the size of the read */
    अगर(ctx->योग_len > 0) {
      ctx->योग_len -= offset;

      अगर(ctx->योग_len <= 0) {
        failf(तथ्य, "Mime post already completely uploaded");
        वापस CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  वापस CURLE_OK;
}

अटल CURLcode cr_mime_cntrl(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  काष्ठा cr_mime_ctx *ctx = reader->ctx;
  चयन(opcode) {
  हाल CURL_CRCNTRL_REWIND: {
    CURLcode परिणाम = mime_rewind(ctx->part);
    अगर(परिणाम)
      failf(तथ्य, "Cannot rewind mime/post data");
    वापस परिणाम;
  }
  हाल CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    अवरोध;
  हाल CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल bool cr_mime_is_paused(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_creader *reader)
{
  काष्ठा cr_mime_ctx *ctx = reader->ctx;
  (व्योम)तथ्य;
  वापस ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

अटल स्थिर काष्ठा Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_योग_लंबाई,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_done,
  माप(काष्ठा cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(काष्ठा Curl_easy *तथ्य, curl_mimepart *part)
{
  काष्ठा Curl_creader *r;
  काष्ठा cr_mime_ctx *ctx;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&r, तथ्य, &cr_mime, CURL_CR_CLIENT);
  अगर(परिणाम)
    वापस परिणाम;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  परिणाम = mime_rewind(ctx->part);
  अगर(परिणाम) {
    Curl_creader_free(तथ्य, r);
    वापस परिणाम;
  }
  ctx->योग_len = mime_आकार(ctx->part);

  वापस Curl_creader_set(तथ्य, r);
}

#अन्यथा /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (व्योम)easy;
  वापस शून्य;
}

व्योम curl_mime_free(curl_mime *mime)
{
  (व्योम)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (व्योम)mime;
  वापस शून्य;
}

CURLcode curl_mime_name(curl_mimepart *part, स्थिर अक्षर *name)
{
  (व्योम)part;
  (व्योम)name;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, स्थिर अक्षर *filename)
{
  (व्योम)part;
  (व्योम)filename;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, स्थिर अक्षर *mimetype)
{
  (व्योम)part;
  (व्योम)mimetype;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, स्थिर अक्षर *encoding)
{
  (व्योम)part;
  (व्योम)encoding;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_तथ्य(curl_mimepart *part, स्थिर अक्षर *तथ्य, माप_प्रकार datasize)
{
  (व्योम)part;
  (व्योम)तथ्य;
  (व्योम)datasize;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, स्थिर अक्षर *filename)
{
  (व्योम)part;
  (व्योम)filename;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_तथ्य_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           व्योम *arg)
{
  (व्योम)part;
  (व्योम)datasize;
  (व्योम)readfunc;
  (व्योम)seekfunc;
  (व्योम)freefunc;
  (व्योम)arg;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (व्योम)part;
  (व्योम)subparts;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           काष्ठा curl_slist *headers, पूर्णांक take_ownership)
{
  (व्योम)part;
  (व्योम)headers;
  (व्योम)take_ownership;
  वापस CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(काष्ठा curl_slist **slp, स्थिर अक्षर *fmt, ...)
{
  (व्योम)slp;
  (व्योम)fmt;
  वापस CURLE_NOT_BUILT_IN;
}

#पूर्ण_अगर /* if disabled */

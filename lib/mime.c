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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ସଂରଚନା Curl_easy;

#ଅନ୍ତର୍ଭାଵ "mime.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/basename.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"

#ଯଦି !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "slist.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

ସ୍ଥିର size_t mime_subparts_read(ଅକ୍ଷର *buffer, size_t size, size_t nitems,
                                 ଶୂନ୍ୟ *instream, bool *hasread);
ସ୍ଥିର curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର qp_class[] = {
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
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#ଅନ୍ୟଥା

#ଅନ୍ତର୍ଭାଵ <fabdef.h>
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
curl_off_t VmsRealFileSize(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ curlx_struct_stat *stat_buf)
{
  ଅକ୍ଷର buffer[8192];
  curl_off_t ଗଣନା_2;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  ଯଦି(!file)
    ଫେରନ୍ତୁ 0;

  ଗଣନା_2 = 0;
  ret_stat = 1;
  ଯେତେବେଳେ(ret_stat > 0) {
    ret_stat = fread(buffer, 1, ଆକାର(buffer), file);
    ଯଦି(ret_stat)
      ଗଣନା_2 += ret_stat;
  }
  curlx_fclose(file);

  ଫେରନ୍ତୁ ଗଣନା_2;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
ସ୍ଥିର curl_off_t VmsSpecialSize(ଅଚଳ ଅକ୍ଷର *name,
                                 ଅଚଳ curlx_struct_stat *stat_buf)
{
  ବାଛ(stat_buf->st_fab_rfm) {
  ସ୍ଥିତି FAB$C_VAR:
  ସ୍ଥିତି FAB$C_VFC:
    ଫେରନ୍ତୁ VmsRealFileSize(name, stat_buf);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ stat_buf->st_size;
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
ସ୍ଥିର FILE *vmsfopenread(ଅଚଳ ଅକ୍ଷର *file, ଅଚଳ ଅକ୍ଷର *mode)
{
  curlx_struct_stat statbuf;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = curlx_stat(file, &statbuf);

  ଯଦି(res != -1) {
    ବାଛ(statbuf.st_fab_rfm) {
    ସ୍ଥିତି FAB$C_VAR:
    ସ୍ଥିତି FAB$C_VFC:
    ସ୍ଥିତି FAB$C_STMCR:
      ଫେରନ୍ତୁ curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
ସ୍ଥିର ଶୂନ୍ୟ mimesetstate(ସଂରଚନା mime_state *state,
                         ଗଣନା mimestate tok, ଶୂନ୍ୟ *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
ସ୍ଥିର ଅକ୍ଷର *escape_string(ସଂରଚନା Curl_easy *data,
                           ଅଚଳ ଅକ୍ଷର *src, ଗଣନା mimestrategy strategy)
{
  CURLcode result;
  ସଂରଚନା dynbuf db;
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table;
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ *p;
  /* replace first character by rest of string. */
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    ଖାଲି
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    ଖାଲି
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  ଯଦି(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_ନିବେଶ_LENGTH);

  ପାଇଁ(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    ପାଇଁ(p = table; *p && **p != *src; p++)
      ;

    ଯଦି(*p)
      result = curlx_dyn_add(&db, *p + 1);
    ଅନ୍ୟଥା
      result = curlx_dyn_addn(&db, src, 1);
  }

  ଫେରନ୍ତୁ curlx_dyn_ptr(&db);
}

/* Check if header matches. */
ସ୍ଥିର ଅକ୍ଷର *match_header(ସଂରଚନା curl_slist *hdr, ଅଚଳ ଅକ୍ଷର *lbl, size_t len)
{
  ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;

  ଯଦି(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    ପାଇଁ(ମୂଲ୍ୟ = hdr->data + len + 1; *ମୂଲ୍ୟ == ' '; ମୂଲ୍ୟ++)
      ;
  ଫେରନ୍ତୁ ମୂଲ୍ୟ;
}

/* Get a header from an slist. */
ସ୍ଥିର ଅକ୍ଷର *ସନ୍ଧାନ_header(ସଂରଚନା curl_slist *hdrlist,
                           ଅଚଳ ଅକ୍ଷର *hdr, size_t len)
{
  ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;

  ପାଇଁ(; !ମୂଲ୍ୟ && hdrlist; hdrlist = hdrlist->next)
    ମୂଲ୍ୟ = match_header(hdrlist, hdr, len);

  ଫେରନ୍ତୁ ମୂଲ୍ୟ;
}

ସ୍ଥିର ଅକ୍ଷର *strippath(ଅଚଳ ଅକ୍ଷର *fullfile)
{
  ଅକ୍ଷର *filename;
  ଅକ୍ଷର *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  ଯଦି(!filename)
    ଫେରନ୍ତୁ ଖାଲି;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  ଫେରନ୍ତୁ base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
ସ୍ଥିର ଶୂନ୍ୟ cleanup_encoder_state(ସଂରଚନା mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
ସ୍ଥିର size_t encoder_nop_read(ଅକ୍ଷର *buffer, size_t size, bool ateof,
                               ସଂରଚନା curl_mimepart *part)
{
  ସଂରଚନା mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (ଶୂନ୍ୟ)ateof;

  ଯଦି(!size)
    ଫେରନ୍ତୁ STOP_FILLING;

  ଯଦି(size > insize)
    size = insize;

  ଯଦି(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  ଫେରନ୍ତୁ size;
}

ସ୍ଥିର curl_off_t encoder_nop_size(curl_mimepart *part)
{
  ଫେରନ୍ତୁ part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
ସ୍ଥିର size_t encoder_7bit_read(ଅକ୍ଷର *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  ସଂରଚନା mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (ଶୂନ୍ୟ)ateof;

  ଯଦି(!size)
    ଫେରନ୍ତୁ STOP_FILLING;

  ଯଦି(size > cursize)
    size = cursize;

  ପାଇଁ(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    ଯଦି(*buffer++ & 0x80)
      ଫେରନ୍ତୁ cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  ଫେରନ୍ତୁ cursize;
}

/* Base64 content encoder. */
ସ୍ଥିର size_t encoder_base64_read(ଅକ୍ଷର *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  ସଂରଚନା mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ଅକ୍ଷର *ptr = buffer;

  ଯେତେବେଳେ(st->bufbeg < st->bufend) {
    /* Line full ? */
    ଯଦି(st->pos > MAX_ENCODED_ଧାଡ଼ି_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      ଯଦି(size < 2) {
        ଯଦି(!cursize)
          ଫେରନ୍ତୁ STOP_FILLING;
        ଭାଙ୍ଗ;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    ଯଦି(size < 4) {
      ଯଦି(!cursize)
        ଫେରନ୍ତୁ STOP_FILLING;
      ଭାଙ୍ଗ;
    }
    ଯଦି(st->bufend - st->bufbeg < 3)
      ଭାଙ୍ଗ;

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
  ଯଦି(ateof) {
    ଯଦି(size < 4) {
      ଯଦି(!cursize)
        ଫେରନ୍ତୁ STOP_FILLING;
    }
    ଅନ୍ୟଥା {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      ଯଦି(st->bufend != st->bufbeg) {

        ଯଦି(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        ଯଦି(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  ଫେରନ୍ତୁ cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_ନିବେଶ_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_ଧାଡ଼ି_LENGTH + 2)) *                  \
    MAX_ENCODED_ଧାଡ଼ି_LENGTH / 4) * 3 - 3)

ସ୍ଥିର curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  ଯଦି(size <= 0)
    ଫେରନ୍ତୁ size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  ଯଦି(size > BASE64_MAX_ନିବେଶ_SIZE)
    ଫେରନ୍ତୁ -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  ଫେରନ୍ତୁ size + (2 * ((size - 1) / MAX_ENCODED_ଧାଡ଼ି_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା qp_lookahead_eol(ସଂରଚନା mime_encoder_state *st, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ateof, size_t n)
{
  n += st->bufbeg;
  ଯଦି(n >= st->bufend && ateof)
    ଫେରନ୍ତୁ 1;
  ଯଦି(n + 2 > st->bufend)
    ଫେରନ୍ତୁ ateof ? 0 : -1;
  ଯଦି(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    ଫେରନ୍ତୁ 1;
  ଫେରନ୍ତୁ 0;
}

/* Quoted-printable encoder. */
ସ୍ଥିର size_t encoder_qp_read(ଅକ୍ଷର *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  ସଂରଚନା mime_encoder_state *st = &part->encstate;
  ଅକ୍ଷର *ptr = buffer;
  size_t cursize = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା softlinebreak;
  ଅକ୍ଷର buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  ଯେତେବେଳେ(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = (unsigned ଅକ୍ଷର)st->buf[st->bufbeg];
    buf[0] = (ଅକ୍ଷର)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    ବାଛ(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    ସ୍ଥିତି QP_OK:          /* Not a special character. */
      ଭାଙ୍ଗ;
    ସ୍ଥିତି QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      ବାଛ(qp_lookahead_eol(st, ateof, 1)) {
      ସ୍ଥିତି -1:          /* More input data needed. */
        ଫେରନ୍ତୁ cursize;
      ସ୍ଥିତି 0:           /* No encoding needed. */
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      ବାଛ(qp_lookahead_eol(st, ateof, 0)) {
      ସ୍ଥିତି -1:          /* Need more data. */
        ଫେରନ୍ତୁ cursize;
      ସ୍ଥିତି 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      ଭାଙ୍ଗ;
    }

    /* Be sure the encoded character fits within maximum line length. */
    ଯଦି(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_ଧାଡ଼ି_LENGTH;
      ଯଦି(!softlinebreak && st->pos + len == MAX_ENCODED_ଧାଡ଼ି_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        ବାଛ(qp_lookahead_eol(st, ateof, consumed)) {
        ସ୍ଥିତି -1:        /* Need more data. */
          ଫେରନ୍ତୁ cursize;
        ସ୍ଥିତି 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          ଭାଙ୍ଗ;
        }
      }
      ଯଦି(softlinebreak) {
        curlx_strcopy(buf, ଆକାର(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    ଯଦି(len > size) {
      ଯଦି(!cursize)
        ଫେରନ୍ତୁ STOP_FILLING;
      ଭାଙ୍ଗ;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    ଯଦି(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  ଫେରନ୍ତୁ cursize;
}

ସ୍ଥିର curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  ଫେରନ୍ତୁ part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
ସ୍ଥିର size_t mime_mem_read(ଅକ୍ଷର *buffer, size_t size, size_t nitems,
                            ଶୂନ୍ୟ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (ଶୂନ୍ୟ)size;  /* Always 1 */

  ଯଦି(!nitems)
    ଫେରନ୍ତୁ STOP_FILLING;

  ଯଦି(sz > nitems)
    sz = nitems;

  ଯଦି(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  ଫେରନ୍ତୁ sz;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mime_mem_seek(ଶୂନ୍ୟ *instream, curl_off_t offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ବାଛ(whence) {
  ସ୍ଥିତି SEEK_CUR:
    offset += part->state.offset;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SEEK_END:
    offset += part->datasize;
    ଭାଙ୍ଗ;
  }

  ଯଦି(offset < 0 || offset > part->datasize)
    ଫେରନ୍ତୁ CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  ଫେରନ୍ତୁ CURL_SEEKFUNC_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ mime_mem_free(ଶୂନ୍ୟ *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
ସ୍ଥିର bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  ଯଦି(part->fp)
    ଫେରନ୍ତୁ FALSE;
  part->fp = fopen_read(part->data, "rb");
  ଫେରନ୍ତୁ part->fp ? FALSE : TRUE;
}

ସ୍ଥିର size_t mime_file_read(ଅକ୍ଷର *buffer, size_t size, size_t nitems,
                             ଶୂନ୍ୟ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ଯଦି(!nitems)
    ଫେରନ୍ତୁ STOP_FILLING;

  ଯଦି(mime_open_file(part))
    ଫେରନ୍ତୁ READ_ERROR;

  ଫେରନ୍ତୁ fread(buffer, size, nitems, part->fp);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mime_file_seek(ଶୂନ୍ୟ *instream, curl_off_t offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ଯଦି(whence == SEEK_SET && !offset && !part->fp)
    ଫେରନ୍ତୁ CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  ଯଦି(mime_open_file(part))
    ଫେରନ୍ତୁ CURL_SEEKFUNC_FAIL;

  ଫେରନ୍ତୁ curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ mime_file_free(ଶୂନ୍ୟ *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  ଯଦି(part->fp) {
    curlx_fclose(part->fp);
    part->fp = ଖାଲି;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
ସ୍ଥିର size_t readback_bytes(ସଂରଚନା mime_state *state,
                             ଅକ୍ଷର *buffer, size_t bufsize,
                             ଅଚଳ ଅକ୍ଷର *bytes, size_t numbytes,
                             ଅଚଳ ଅକ୍ଷର *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  ଯଦି(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  ଅନ୍ୟଥା {
    sz = offset - numbytes;
    ଯଦି(sz >= traillen)
      ଫେରନ୍ତୁ 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  ଯଦି(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  ଫେରନ୍ତୁ sz;
}

/* Read a non-encoded part content. */

ସ୍ଥିର size_t read_part_content(curl_mimepart *part, ଅକ୍ଷର *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  ବାଛ(part->lastreadstatus) {
  ସ୍ଥିତି 0:
  ସ୍ଥିତି CURL_READFUNC_ABORT:
  ସ୍ଥିତି CURL_READFUNC_PAUSE:
  ସ୍ଥିତି READ_ERROR:
    ଫେରନ୍ତୁ part->lastreadstatus;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }

  /* If we can determine we are at end of part data, spare a read. */
  ଯଦି(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  ଅନ୍ୟଥା {
    ବାଛ(part->kind) {
    ସ୍ଥିତି MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMEKIND_FILE:
      ଯଦି(part->fp && feof(part->fp))
        ଭାଙ୍ଗ;  /* At EOF. */
      FALLTHROUGH();
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଯଦି(part->readfunc) {
        ଯଦି(!(part->flags & MIME_FAST_READ)) {
          ଯଦି(*hasread)
            ଫେରନ୍ତୁ STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      ଭାଙ୍ଗ;
    }
  }

  ବାଛ(sz) {
  ସ୍ଥିତି STOP_FILLING:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 0:
  ସ୍ଥିତି CURL_READFUNC_ABORT:
  ସ୍ଥିତି CURL_READFUNC_PAUSE:
  ସ୍ଥିତି READ_ERROR:
    part->lastreadstatus = sz;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ sz;
}

/* Read and encode part content. */
ସ୍ଥିର size_t read_encoded_part_content(curl_mimepart *part, ଅକ୍ଷର *buffer,
                                        size_t bufsize, bool *hasread)
{
  ସଂରଚନା mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  ପାଇଁ(;;) {
    ଯଦି(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      ବାଛ(sz) {
      ସ୍ଥିତି 0:
        ଯଦି(ateof)
          ଫେରନ୍ତୁ cursize;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି READ_ERROR:
      ସ୍ଥିତି STOP_FILLING:
        ଫେରନ୍ତୁ cursize ? cursize : sz;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        ଜାରି;
      }
    }

    /* We need more data in input buffer. */
    ଯଦି(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      ଯଦି(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    ଯଦି(st->bufend >= ଆକାର(st->buf))
      ଫେରନ୍ତୁ cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           ଆକାର(st->buf) - st->bufend, hasread);
    ବାଛ(sz) {
    ସ୍ଥିତି 0:
      ateof = TRUE;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_READFUNC_ABORT:
    ସ୍ଥିତି CURL_READFUNC_PAUSE:
    ସ୍ଥିତି READ_ERROR:
    ସ୍ଥିତି STOP_FILLING:
      ଫେରନ୍ତୁ cursize ? cursize : sz;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      st->bufend += sz;
      ଭାଙ୍ଗ;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
ସ୍ଥିର size_t readback_part(curl_mimepart *part,
                            ଅକ୍ଷର *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  ଯେତେବେଳେ(bufsize) {
    size_t sz = 0;
    ସଂରଚନା curl_slist *hdr = (ସଂରଚନା curl_slist *)part->state.ptr;
    ବାଛ(part->state.state) {
    ସ୍ଥିତି MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_USERHEADERS:
      ଯଦି(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, ଖାଲି);
        ଭାଙ୍ଗ;
      }
      ଯଦି(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        ଭାଙ୍ଗ;
      }
      FALLTHROUGH();
    ସ୍ଥିତି MIMESTATE_CURLHEADERS:
      ଯଦି(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      ଅନ୍ୟଥା {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        ଯଦି(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      ଯଦି(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, ଖାଲି);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, ଖାଲି);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_CONTENT:
      ଯଦି(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      ଅନ୍ୟଥା
        sz = read_part_content(part, buffer, bufsize, hasread);
      ବାଛ(sz) {
      ସ୍ଥିତି 0:
        mimesetstate(&part->state, MIMESTATE_END, ଖାଲି);
        /* Try sparing open file descriptors. */
        ଯଦି(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = ଖାଲି;
        }
        FALLTHROUGH();
      ସ୍ଥିତି CURL_READFUNC_ABORT:
      ସ୍ଥିତି CURL_READFUNC_PAUSE:
      ସ୍ଥିତି READ_ERROR:
      ସ୍ଥିତି STOP_FILLING:
        ଫେରନ୍ତୁ cursize ? cursize : sz;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_END:
      ଫେରନ୍ତୁ cursize;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  ଫେରନ୍ତୁ cursize;
}

/* Readback from mime. Warning: not a read callback function. */
ସ୍ଥିର size_t mime_subparts_read(ଅକ୍ଷର *buffer, size_t size, size_t nitems,
                                 ଶୂନ୍ୟ *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (ଶୂନ୍ୟ)size;  /* Always 1 */

  ଯେତେବେଳେ(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    ବାଛ(mime->state.state) {
    ସ୍ଥିତି MIMESTATE_BEGIN:
    ସ୍ଥିତି MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      ଯଦି(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_BOUNDARY2:
      ଯଦି(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      ଅନ୍ୟଥା
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      ଯଦି(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_CONTENT:
      ଯଦି(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, ଖାଲି);
        ଭାଙ୍ଗ;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      ବାଛ(sz) {
      ସ୍ଥିତି CURL_READFUNC_ABORT:
      ସ୍ଥିତି CURL_READFUNC_PAUSE:
      ସ୍ଥିତି READ_ERROR:
      ସ୍ଥିତି STOP_FILLING:
        ଫେରନ୍ତୁ cursize ? cursize : sz;
      ସ୍ଥିତି 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMESTATE_END:
      ଫେରନ୍ତୁ cursize;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  ଫେରନ୍ତୁ cursize;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mime_part_rewind(curl_mimepart *part)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = CURL_SEEKFUNC_OK;
  ଗଣନା mimestate targetstate = MIMESTATE_BEGIN;

  ଯଦି(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  ଯଦି(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    ଯଦି(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      ବାଛ(res) {
      ସ୍ଥିତି CURL_SEEKFUNC_OK:
      ସ୍ଥିତି CURL_SEEKFUNC_FAIL:
      ସ୍ଥିତି CURL_SEEKFUNC_CANTSEEK:
        ଭାଙ୍ଗ;
      ସ୍ଥିତି -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        res = CURL_SEEKFUNC_FAIL;
        ଭାଙ୍ଗ;
      }
    }
  }

  ଯଦି(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, ଖାଲି);

  part->lastreadstatus = 1; /* Successful read status. */
  ଫେରନ୍ତୁ res;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mime_subparts_seek(ଶୂନ୍ୟ *instream, curl_off_t offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = CURL_SEEKFUNC_OK;

  ଯଦି(whence != SEEK_SET || offset)
    ଫେରନ୍ତୁ CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  ଯଦି(mime->state.state == MIMESTATE_BEGIN)
    ଫେରନ୍ତୁ CURL_SEEKFUNC_OK;           /* Already rewound. */

  ପାଇଁ(part = mime->firstpart; part; part = part->nextpart) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = mime_part_rewind(part);
    ଯଦି(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  ଯଦି(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ଖାଲି);

  ଫେରନ୍ତୁ rc;
}

/* Release part content. */
ସ୍ଥିର ଶୂନ୍ୟ cleanup_part_content(curl_mimepart *part)
{
  ଯଦି(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = ଖାଲି;
  part->seekfunc = ଖାଲି;
  part->freefunc = ଖାଲି;
  part->arg = (ଶୂନ୍ୟ *)part;           /* Defaults to part itself. */
  part->data = ଖାଲି;
  part->fp = ଖାଲି;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

ସ୍ଥିର ଶୂନ୍ୟ mime_subparts_free(ଶୂନ୍ୟ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ଯଦି(mime && mime->parent) {
    mime->parent->freefunc = ଖାଲି;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
ସ୍ଥିର ଶୂନ୍ୟ mime_subparts_unbind(ଶୂନ୍ୟ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ଯଦି(mime && mime->parent) {
    mime->parent->freefunc = ଖାଲି;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = ଖାଲି;
  }
}

ଶୂନ୍ୟ Curl_mime_cleanpart(curl_mimepart *part)
{
  ଯଦି(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    ଯଦି(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
ଶୂନ୍ୟ curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  ଯଦି(!mime)
    ଫେରନ୍ତୁ;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  ଯେତେବେଳେ(mime) {
    part = mime->firstpart;
    ଯଦି(part) {
      mime->firstpart = part->nextpart;
      ଯଦି(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = ଖାଲି;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    ଅନ୍ୟଥା {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

ସ୍ଥିର bool mime_is_too_deep(ଅଚଳ curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  ଯଦି(++call_depth > MAX_MIME_LEVELS)
    ଫେରନ୍ତୁ TRUE;

  ଯଦି(part->kind == MIMEKIND_MULTIPART && part->arg) {
    ଅଚଳ curl_mime *mime = (ଅଚଳ curl_mime *)part->arg;
    ଅଚଳ curl_mimepart *s;

    ପାଇଁ(s = mime->firstpart; s; s = s->nextpart) {
      ଯଦି(mime_is_too_deep(s, call_depth))
        ଫେରନ୍ତୁ TRUE;
    }
  }

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode mime_duppart(ସଂରଚନା Curl_easy *data,
                             curl_mimepart *dst, ଅଚଳ curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  ଅଚଳ curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  ଯଦି(++call_depth > MAX_MIME_LEVELS)
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;

  /* Duplicate content. */
  ବାଛ(src->kind) {
  ସ୍ଥିତି MIMEKIND_NONE:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    ଯଦି(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    ପାଇଁ(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    ଭାଙ୍ଗ;
  }

  /* Duplicate headers. */
  ଯଦି(!result && src->userheaders) {
    ସଂରଚନା curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    ଯଦି(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    ଅନ୍ୟଥା {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      ଯଦି(result)
        curl_slist_free_all(hdrs);
    }
  }

  ଯଦି(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  ଯଦି(!result)
    result = curl_mime_name(dst, src->name);
  ଯଦି(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  ଯଦି(result)
    Curl_mime_cleanpart(dst);

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_mime_duppart(ସଂରଚନା Curl_easy *data,
                           curl_mimepart *dst, ଅଚଳ curl_mimepart *src)
{
  ଯଦି(!dst || !src)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(mime_is_too_deep(src, 0))
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;

  ଫେରନ୍ତୁ mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(ଶୂନ୍ୟ *easy)
{
  curl_mime *mime = curlx_malloc(ଆକାର(*mime));

  ଯଦି(mime) {
    mime->parent = ଖାଲି;
    mime->firstpart = ଖାଲି;
    mime->lastpart = ଖାଲି;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    ଯଦି(Curl_rand_alnum(easy,
                       (unsigned ଅକ୍ଷର *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      ଫେରନ୍ତୁ ଖାଲି;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ଖାଲି);
  }

  ଫେରନ୍ତୁ mime;
}

/* Initialize a mime part. */
ଶୂନ୍ୟ Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, ଆକାର(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, ଖାଲି);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  ଯଦି(!mime)
    ଫେରନ୍ତୁ ଖାଲି;

  part = curlx_malloc(ଆକାର(*part));

  ଯଦି(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    ଯଦି(mime->lastpart)
      mime->lastpart->nextpart = part;
    ଅନ୍ୟଥା
      mime->firstpart = part;

    mime->lastpart = part;
  }

  ଫେରନ୍ତୁ part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *name)
{
  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  ଯଦି(name) {
    part->name = curlx_strdup(name);
    ଯଦି(!part->name)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *filename)
{
  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  ଯଦି(filename) {
    part->filename = curlx_strdup(filename);
    ଯଦି(!part->filename)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *data, size_t datasize)
{
  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ଯଦି(data) {
    ଯଦି(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    ଯଦି(!part->data)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *filename)
{
  CURLcode result = CURLE_OK;

  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ଯଦି(filename) {
    ଅକ୍ଷର *base;
    curlx_struct_stat sbuf;

    ଯଦି(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    ଅନ୍ୟଥା {
      part->data = curlx_strdup(filename);
      ଯଦି(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      ଅନ୍ୟଥା {
        part->datasize = -1;
        ଯଦି(S_ISREG(sbuf.st_mode)) {
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
        ଯଦି(!base)
          result = CURLE_OUT_OF_MEMORY;
        ଅନ୍ୟଥା {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  ଫେରନ୍ତୁ result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *mimetype)
{
  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  ଯଦି(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    ଯଦି(!part->mimetype)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  ଅଚଳ ସଂରଚନା mime_encoder *mep;

  ଯଦି(!part)
    ଫେରନ୍ତୁ result;

  part->encoder = ଖାଲି;

  ଯଦି(!encoding)
    ଫେରନ୍ତୁ CURLE_OK;    /* Removing current encoder. */

  ପାଇଁ(mep = encoders; mep->name; mep++)
    ଯଦି(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  ଫେରନ୍ତୁ result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           ସଂରଚନା curl_slist *headers, ପୂର୍ଣ୍ଣସଂଖ୍ୟା take_ownership)
{
  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(part->flags & MIME_USERHEADERS_OWNER) {
    ଯଦି(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  ଯଦି(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, ଶୂନ୍ୟ *arg)
{
  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ଯଦି(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, ପୂର୍ଣ୍ଣସଂଖ୍ୟା take_ownership)
{
  curl_mime *ମୂଳ;

  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  ଯଦି(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    ଫେରନ୍ତୁ CURLE_OK;

  cleanup_part_content(part);

  ଯଦି(subparts) {
    /* Should not have been attached already. */
    ଯଦି(subparts->parent)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    ମୂଳ = part->parent;
    ଯଦି(ମୂଳ) {
      ଯେତେବେଳେ(ମୂଳ->parent && ମୂଳ->parent->parent)
        ମୂଳ = ମୂଳ->parent->parent;
      ଯଦି(subparts == ମୂଳ) {
        /* cannot add as a subpart of itself. */
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    ଯଦି(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      ଫେରନ୍ତୁ CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  ଫେରନ୍ତୁ Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(ଅକ୍ଷର *buffer, size_t size, size_t nitems, ଶୂନ୍ୟ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (ଶୂନ୍ୟ)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  କର {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } ଯେତେବେଳେ(ret == STOP_FILLING);

  ଫେରନ୍ତୁ ret;
}

/* Rewind mime stream. */
ସ୍ଥିର CURLcode mime_rewind(curl_mimepart *part)
{
  ଫେରନ୍ତୁ mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
ସ୍ଥିର size_t slist_size(ସଂରଚନା curl_slist *s,
                         size_t overhead, ଅଚଳ ଅକ୍ଷର *skip, size_t skiplen)
{
  size_t size = 0;

  ପାଇଁ(; s; s = s->next)
    ଯଦି(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  ଫେରନ୍ତୁ size;
}

/* Get/compute multipart size. */
ସ୍ଥିର curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  ଯଦି(!mime)
    ଫେରନ୍ତୁ 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  ପାଇଁ(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    ଯଦି(sz < 0)
      size = sz;

    ଯଦି(size >= 0)
      size += boundarysize + sz;
  }

  ଫେରନ୍ତୁ size;
}

/* Get/compute mime size. */
ସ୍ଥିର curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  ଯଦି(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  ଯଦି(part->encoder)
    size = part->encoder->sizefunc(part);

  ଯଦି(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, ଖାଲି, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  ଫେରନ୍ତୁ size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(ସଂରଚନା curl_slist **slp, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  ସଂରଚନା curl_slist *hdr = ଖାଲି;
  ଅକ୍ଷର *s = ଖାଲି;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  ଯଦି(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    ଯଦି(hdr)
      *slp = hdr;
    ଅନ୍ୟଥା
      curlx_free(s);
  }

  ଫେରନ୍ତୁ hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
ସ୍ଥିର CURLcode add_content_type(ସଂରଚନା curl_slist **slp,
                                 ଅଚଳ ଅକ୍ଷର *type, ଅଚଳ ଅକ୍ଷର *boundary)
{
  ଫେରନ୍ତୁ Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ଅଚଳ ଅକ୍ଷର *Curl_mime_contenttype(ଅଚଳ ଅକ୍ଷର *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  ସଂରଚନା ContentType {
    ଅଚଳ ଅକ୍ଷର *extension;
    ଅଚଳ ଅକ୍ଷର *type;
  };
  ସ୍ଥିର ଅଚଳ ସଂରଚନା ContentType ctts[] = {
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

  ଯଦି(filename) {
    size_t len1 = strlen(filename);
    ଅଚଳ ଅକ୍ଷର *nameend = filename + len1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      ଯଦି(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        ଫେରନ୍ତୁ ctts[i].type;
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର bool content_type_match(ଅଚଳ ଅକ୍ଷର *contenttype,
                               ଅଚଳ ଅକ୍ଷର *target, size_t len)
{
  ଯଦି(contenttype && curl_strnequal(contenttype, target, len))
    ବାଛ(contenttype[len]) {
    ସ୍ଥିତି '\0':
    ସ୍ଥିତି '\t':
    ସ୍ଥିତି '\r':
    ସ୍ଥିତି '\n':
    ସ୍ଥିତି ' ':
    ସ୍ଥିତି ';':
      ଫେରନ୍ତୁ TRUE;
    }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode add_content_disposition(ସଂରଚନା Curl_easy *data,
                                        curl_mimepart *part,
                                        ଅଚଳ ଅକ୍ଷର *disposition,
                                        ଅଚଳ ଅକ୍ଷର *contenttype,
                                        ଗଣନା mimestrategy strategy)
{
  ଯଦି(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  ଯଦି(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = ଖାଲି;

  ଯଦି(disposition) {
    CURLcode result = CURLE_OK;
    ଅକ୍ଷର *name = ଖାଲି;
    ଅକ୍ଷର *filename = ଖାଲି;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    ଯଦି(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

    ଯଦି(part->name) {
      name = escape_string(data, part->name, strategy);
      ଯଦି(!name)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ଯଦି(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      ଯଦି(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    ଯଦି(!result)
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
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode mime_prepare_headers(ସଂରଚନା Curl_easy *data,
                                     curl_mimepart *part,
                                     ଅଚଳ ଅକ୍ଷର *contenttype,
                                     ଅଚଳ ଅକ୍ଷର *disposition,
                                     ଗଣନା mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *boundary = ଖାଲି;
  ଅକ୍ଷର *customct;
  ଅଚଳ ଅକ୍ଷର *cte = ଖାଲି;
  CURLcode result = CURLE_OK;

  ଯଦି(++call_depth > MAX_MIME_LEVELS)
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = ଖାଲି;

  /* Be sure we will not access old headers later. */
  ଯଦି(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, ଖାଲି);

  /* Check if content type is specified. */
  customct = part->mimetype;
  ଯଦି(!customct)
    customct = ସନ୍ଧାନ_header(part->userheaders, STRCONST("Content-Type"));
  ଯଦି(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  ଯଦି(!contenttype) {
    ବାଛ(part->kind) {
    ସ୍ଥିତି MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      ଯଦି(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      ଯଦି(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      contenttype = Curl_mime_contenttype(part->filename);
      ଭାଙ୍ଗ;
    }
  }

  ଯଦି(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    ଯଦି(mime)
      boundary = mime->boundary;
  }
  ଅନ୍ୟଥା ଯଦି(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = ଖାଲି;

  /* Issue content-disposition header only if not already set by caller. */
  ଯଦି(!ସନ୍ଧାନ_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* Issue Content-Type header. */
  ଯଦି(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* Content-Transfer-Encoding header. */
  ଯଦି(!ସନ୍ଧାନ_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    ଯଦି(part->encoder)
      cte = part->encoder->name;
    ଅନ୍ୟଥା ଯଦି(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    ଯଦି(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  ଯଦି(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  ଯଦି(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = ଖାଲି;
    ଯଦି(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    ପାଇଁ(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, ଖାଲି,
                                    disposition, strategy, call_depth);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_mime_prepare_headers(ସଂରଚନା Curl_easy *data,
                                   curl_mimepart *part,
                                   ଅଚଳ ଅକ୍ଷର *contenttype,
                                   ଅଚଳ ଅକ୍ଷର *disposition,
                                   ଗଣନା mimestrategy strategy)
{
  ଫେରନ୍ତୁ mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
ସ୍ଥିର ଶୂନ୍ୟ mime_unpause(curl_mimepart *part)
{
  ଯଦି(part) {
    ଯଦି(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    ଯଦି(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      ଯଦି(mime) {
        curl_mimepart *subpart;

        ପାଇଁ(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

ସଂରଚନା cr_mime_ctx {
  ସଂରଚନା Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  ସଂରଚନା bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

ସ୍ଥିର CURLcode cr_mime_init(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_mime_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cr_mime_close(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_mime_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
ସ୍ଥିର CURLcode cr_mime_read(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_creader *reader,
                             ଅକ୍ଷର *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  ସଂରଚନା cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  ଅକ୍ଷର tmp[256];

  /* Once we have errored, we will return the same error forever */
  ଯଦି(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    ଫେରନ୍ତୁ ctx->error_result;
  }
  ଯଦି(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  /* respect length limitations */
  ଯଦି(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    ଯଦି(remain <= 0)
      blen = 0;
    ଅନ୍ୟଥା ଯଦି(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  ଯଦି(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned ଅକ୍ଷର *)buf, blen, &nread);
    ଯଦି(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      ଫେରନ୍ତୁ result;
    }
  }
  ଅନ୍ୟଥା ଯଦି(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, ଆକାର(tmp), ctx->part);
    ଯଦି(nread <= ଆକାର(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned ଅକ୍ଷର *)tmp, nread, &n);
      ଯଦି(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ଫେରନ୍ତୁ result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      ଯଦି(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ଫେରନ୍ତୁ result;
      }
    }
  }
  ଅନ୍ୟଥା
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  ବାଛ(nread) {
  ସ୍ଥିତି 0:
    ଯଦି((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      ଫେରନ୍ତୁ CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    ଭାଙ୍ଗ; /* nothing was read */

  ସ୍ଥିତି STOP_FILLING:
  ସ୍ଥିତି READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      ଫେରନ୍ତୁ CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    ଯଦି(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    ଭାଙ୍ଗ;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread, *peos);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cr_mime_needs_rewind(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_mime_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ ctx->read_len > 0;
}

ସ୍ଥିର curl_off_t cr_mime_total_length(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_mime_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ ctx->total_len;
}

ସ୍ଥିର CURLcode cr_mime_resume_from(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା Curl_creader *reader,
                                    curl_off_t offset)
{
  ସଂରଚନା cr_mime_ctx *ctx = reader->ctx;

  ଯଦି(offset > 0) {
    curl_off_t passed = 0;

    କର {
      ଅକ୍ଷର scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)ଆକାର(scratch)) ?
        ଆକାର(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      ଯଦି((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        ଫେରନ୍ତୁ CURLE_READ_ERROR;
      }
    } ଯେତେବେଳେ(passed < offset);

    /* now, decrease the size of the read */
    ଯଦି(ctx->total_len > 0) {
      ctx->total_len -= offset;

      ଯଦି(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        ଫେରନ୍ତୁ CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cr_mime_cntrl(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  ସଂରଚନା cr_mime_ctx *ctx = reader->ctx;
  ବାଛ(opcode) {
  ସ୍ଥିତି CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    ଯଦି(result)
      failf(data, "Cannot rewind mime/post data");
    ଫେରନ୍ତୁ result;
  }
  ସ୍ଥିତି CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool cr_mime_is_paused(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_mime_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_crtype cr_mime = {
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
  ଆକାର(ସଂରଚନା cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(ସଂରଚନା Curl_easy *data, curl_mimepart *part)
{
  ସଂରଚନା Curl_creader *r;
  ସଂରଚନା cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  ଯଦି(result) {
    Curl_creader_free(data, r);
    ଫେରନ୍ତୁ result;
  }
  ctx->total_len = mime_size(ctx->part);

  ଫେରନ୍ତୁ Curl_creader_set(data, r);
}

#ଅନ୍ୟଥା /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (ଶୂନ୍ୟ)easy;
  ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ curl_mime_free(curl_mime *mime)
{
  (ଶୂନ୍ୟ)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (ଶୂନ୍ୟ)mime;
  ଫେରନ୍ତୁ ଖାଲି;
}

CURLcode curl_mime_name(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *name)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)name;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *filename)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)filename;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *mimetype)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)mimetype;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *encoding)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)encoding;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *data, size_t datasize)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)datasize;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *filename)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)filename;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           ଶୂନ୍ୟ *arg)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)datasize;
  (ଶୂନ୍ୟ)readfunc;
  (ଶୂନ୍ୟ)seekfunc;
  (ଶୂନ୍ୟ)freefunc;
  (ଶୂନ୍ୟ)arg;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)subparts;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           ସଂରଚନା curl_slist *headers, ପୂର୍ଣ୍ଣସଂଖ୍ୟା take_ownership)
{
  (ଶୂନ୍ୟ)part;
  (ଶୂନ୍ୟ)headers;
  (ଶୂନ୍ୟ)take_ownership;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(ସଂରଚନା curl_slist **slp, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)slp;
  (ଶୂନ୍ୟ)fmt;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

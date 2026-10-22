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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ಸಂರಚನಾ Curl_easy;

#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/basename.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"

#ಯದಿ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "slist.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

ಸ್ಥಿರ size_t mime_subparts_read(ವರ್ಣ *buffer, size_t size, size_t nitems,
                                 ಶೂನ್ಯ *instream, bool *hasread);
ಸ್ಥಿರ curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ qp_class[] = {
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
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#ಅನ್ಯಥಾ

#ಅನ್ತರ್ಭಾವ <fabdef.h>
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
curl_off_t VmsRealFileSize(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ curlx_struct_stat *stat_buf)
{
  ವರ್ಣ buffer[8192];
  curl_off_t ಎಣಿಕೆ;
  ಪೂರ್ಣಾಙ್ಕ ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  ಯದಿ(!file)
    ಪ್ರತ್ಯಾಗಮ 0;

  ಎಣಿಕೆ = 0;
  ret_stat = 1;
  ಯಾವತ್(ret_stat > 0) {
    ret_stat = fread(buffer, 1, ಪರಿಮಾಣ(buffer), file);
    ಯದಿ(ret_stat)
      ಎಣಿಕೆ += ret_stat;
  }
  curlx_fclose(file);

  ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
ಸ್ಥಿರ curl_off_t VmsSpecialSize(ಧ್ರುವ ವರ್ಣ *name,
                                 ಧ್ರುವ curlx_struct_stat *stat_buf)
{
  ವಿಕಲ್ಪ(stat_buf->st_fab_rfm) {
  ಪಕ್ಷ FAB$C_VAR:
  ಪಕ್ಷ FAB$C_VFC:
    ಪ್ರತ್ಯಾಗಮ VmsRealFileSize(name, stat_buf);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ stat_buf->st_size;
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
ಸ್ಥಿರ FILE *vmsfopenread(ಧ್ರುವ ವರ್ಣ *file, ಧ್ರುವ ವರ್ಣ *mode)
{
  curlx_struct_stat statbuf;
  ಪೂರ್ಣಾಙ್ಕ res = curlx_stat(file, &statbuf);

  ಯದಿ(res != -1) {
    ವಿಕಲ್ಪ(statbuf.st_fab_rfm) {
    ಪಕ್ಷ FAB$C_VAR:
    ಪಕ್ಷ FAB$C_VFC:
    ಪಕ್ಷ FAB$C_STMCR:
      ಪ್ರತ್ಯಾಗಮ curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
ಸ್ಥಿರ ಶೂನ್ಯ mimesetstate(ಸಂರಚನಾ mime_state *state,
                         ಗಣನಾ mimestate tok, ಶೂನ್ಯ *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
ಸ್ಥಿರ ವರ್ಣ *escape_string(ಸಂರಚನಾ Curl_easy *data,
                           ಧ್ರುವ ವರ್ಣ *src, ಗಣನಾ mimestrategy strategy)
{
  CURLcode result;
  ಸಂರಚನಾ dynbuf db;
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *table;
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *p;
  /* replace first character by rest of string. */
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    ರಿಕ್ತ
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    ರಿಕ್ತ
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  ಯದಿ(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_ಒಳಹರಿವು_LENGTH);

  ಪ್ರತಿ(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    ಪ್ರತಿ(p = table; *p && **p != *src; p++)
      ;

    ಯದಿ(*p)
      result = curlx_dyn_add(&db, *p + 1);
    ಅನ್ಯಥಾ
      result = curlx_dyn_addn(&db, src, 1);
  }

  ಪ್ರತ್ಯಾಗಮ curlx_dyn_ptr(&db);
}

/* Check if header matches. */
ಸ್ಥಿರ ವರ್ಣ *match_header(ಸಂರಚನಾ curl_slist *hdr, ಧ್ರುವ ವರ್ಣ *lbl, size_t len)
{
  ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;

  ಯದಿ(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    ಪ್ರತಿ(ಮೌಲ್ಯ = hdr->data + len + 1; *ಮೌಲ್ಯ == ' '; ಮೌಲ್ಯ++)
      ;
  ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
}

/* Get a header from an slist. */
ಸ್ಥಿರ ವರ್ಣ *ಹುಡುಕು_header(ಸಂರಚನಾ curl_slist *hdrlist,
                           ಧ್ರುವ ವರ್ಣ *hdr, size_t len)
{
  ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;

  ಪ್ರತಿ(; !ಮೌಲ್ಯ && hdrlist; hdrlist = hdrlist->next)
    ಮೌಲ್ಯ = match_header(hdrlist, hdr, len);

  ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
}

ಸ್ಥಿರ ವರ್ಣ *strippath(ಧ್ರುವ ವರ್ಣ *fullfile)
{
  ವರ್ಣ *filename;
  ವರ್ಣ *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  ಯದಿ(!filename)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  ಪ್ರತ್ಯಾಗಮ base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
ಸ್ಥಿರ ಶೂನ್ಯ cleanup_encoder_state(ಸಂರಚನಾ mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
ಸ್ಥಿರ size_t encoder_nop_read(ವರ್ಣ *buffer, size_t size, bool ateof,
                               ಸಂರಚನಾ curl_mimepart *part)
{
  ಸಂರಚನಾ mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (ಶೂನ್ಯ)ateof;

  ಯದಿ(!size)
    ಪ್ರತ್ಯಾಗಮ STOP_FILLING;

  ಯದಿ(size > insize)
    size = insize;

  ಯದಿ(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  ಪ್ರತ್ಯಾಗಮ size;
}

ಸ್ಥಿರ curl_off_t encoder_nop_size(curl_mimepart *part)
{
  ಪ್ರತ್ಯಾಗಮ part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
ಸ್ಥಿರ size_t encoder_7bit_read(ವರ್ಣ *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  ಸಂರಚನಾ mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (ಶೂನ್ಯ)ateof;

  ಯದಿ(!size)
    ಪ್ರತ್ಯಾಗಮ STOP_FILLING;

  ಯದಿ(size > cursize)
    size = cursize;

  ಪ್ರತಿ(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    ಯದಿ(*buffer++ & 0x80)
      ಪ್ರತ್ಯಾಗಮ cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  ಪ್ರತ್ಯಾಗಮ cursize;
}

/* Base64 content encoder. */
ಸ್ಥಿರ size_t encoder_base64_read(ವರ್ಣ *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  ಸಂರಚನಾ mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  ಪೂರ್ಣಾಙ್ಕ i;
  ವರ್ಣ *ptr = buffer;

  ಯಾವತ್(st->bufbeg < st->bufend) {
    /* Line full ? */
    ಯದಿ(st->pos > MAX_ENCODED_ಸಾಲು_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      ಯದಿ(size < 2) {
        ಯದಿ(!cursize)
          ಪ್ರತ್ಯಾಗಮ STOP_FILLING;
        ಭಙ್ಗ;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    ಯದಿ(size < 4) {
      ಯದಿ(!cursize)
        ಪ್ರತ್ಯಾಗಮ STOP_FILLING;
      ಭಙ್ಗ;
    }
    ಯದಿ(st->bufend - st->bufbeg < 3)
      ಭಙ್ಗ;

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
  ಯದಿ(ateof) {
    ಯದಿ(size < 4) {
      ಯದಿ(!cursize)
        ಪ್ರತ್ಯಾಗಮ STOP_FILLING;
    }
    ಅನ್ಯಥಾ {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      ಯದಿ(st->bufend != st->bufbeg) {

        ಯದಿ(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        ಯದಿ(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_ಒಳಹರಿವು_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_ಸಾಲು_LENGTH + 2)) *                  \
    MAX_ENCODED_ಸಾಲು_LENGTH / 4) * 3 - 3)

ಸ್ಥಿರ curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  ಯದಿ(size <= 0)
    ಪ್ರತ್ಯಾಗಮ size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  ಯದಿ(size > BASE64_MAX_ಒಳಹರಿವು_SIZE)
    ಪ್ರತ್ಯಾಗಮ -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  ಪ್ರತ್ಯಾಗಮ size + (2 * ((size - 1) / MAX_ENCODED_ಸಾಲು_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ qp_lookahead_eol(ಸಂರಚನಾ mime_encoder_state *st, ಪೂರ್ಣಾಙ್ಕ ateof, size_t n)
{
  n += st->bufbeg;
  ಯದಿ(n >= st->bufend && ateof)
    ಪ್ರತ್ಯಾಗಮ 1;
  ಯದಿ(n + 2 > st->bufend)
    ಪ್ರತ್ಯಾಗಮ ateof ? 0 : -1;
  ಯದಿ(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* Quoted-printable encoder. */
ಸ್ಥಿರ size_t encoder_qp_read(ವರ್ಣ *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  ಸಂರಚನಾ mime_encoder_state *st = &part->encstate;
  ವರ್ಣ *ptr = buffer;
  size_t cursize = 0;
  ಪೂರ್ಣಾಙ್ಕ softlinebreak;
  ವರ್ಣ buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  ಯಾವತ್(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    ಪೂರ್ಣಾಙ್ಕ i = (unsigned ವರ್ಣ)st->buf[st->bufbeg];
    buf[0] = (ವರ್ಣ)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    ವಿಕಲ್ಪ(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    ಪಕ್ಷ QP_OK:          /* Not a special character. */
      ಭಙ್ಗ;
    ಪಕ್ಷ QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      ವಿಕಲ್ಪ(qp_lookahead_eol(st, ateof, 1)) {
      ಪಕ್ಷ -1:          /* More input data needed. */
        ಪ್ರತ್ಯಾಗಮ cursize;
      ಪಕ್ಷ 0:           /* No encoding needed. */
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      ವಿಕಲ್ಪ(qp_lookahead_eol(st, ateof, 0)) {
      ಪಕ್ಷ -1:          /* Need more data. */
        ಪ್ರತ್ಯಾಗಮ cursize;
      ಪಕ್ಷ 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      ಭಙ್ಗ;
    }

    /* Be sure the encoded character fits within maximum line length. */
    ಯದಿ(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_ಸಾಲು_LENGTH;
      ಯದಿ(!softlinebreak && st->pos + len == MAX_ENCODED_ಸಾಲು_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        ವಿಕಲ್ಪ(qp_lookahead_eol(st, ateof, consumed)) {
        ಪಕ್ಷ -1:        /* Need more data. */
          ಪ್ರತ್ಯಾಗಮ cursize;
        ಪಕ್ಷ 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          ಭಙ್ಗ;
        }
      }
      ಯದಿ(softlinebreak) {
        curlx_strcopy(buf, ಪರಿಮಾಣ(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    ಯದಿ(len > size) {
      ಯದಿ(!cursize)
        ಪ್ರತ್ಯಾಗಮ STOP_FILLING;
      ಭಙ್ಗ;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    ಯದಿ(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  ಪ್ರತ್ಯಾಗಮ cursize;
}

ಸ್ಥಿರ curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  ಪ್ರತ್ಯಾಗಮ part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
ಸ್ಥಿರ size_t mime_mem_read(ವರ್ಣ *buffer, size_t size, size_t nitems,
                            ಶೂನ್ಯ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (ಶೂನ್ಯ)size;  /* Always 1 */

  ಯದಿ(!nitems)
    ಪ್ರತ್ಯಾಗಮ STOP_FILLING;

  ಯದಿ(sz > nitems)
    sz = nitems;

  ಯದಿ(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  ಪ್ರತ್ಯಾಗಮ sz;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mime_mem_seek(ಶೂನ್ಯ *instream, curl_off_t offset, ಪೂರ್ಣಾಙ್ಕ whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ವಿಕಲ್ಪ(whence) {
  ಪಕ್ಷ SEEK_CUR:
    offset += part->state.offset;
    ಭಙ್ಗ;
  ಪಕ್ಷ SEEK_END:
    offset += part->datasize;
    ಭಙ್ಗ;
  }

  ಯದಿ(offset < 0 || offset > part->datasize)
    ಪ್ರತ್ಯಾಗಮ CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  ಪ್ರತ್ಯಾಗಮ CURL_SEEKFUNC_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ mime_mem_free(ಶೂನ್ಯ *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
ಸ್ಥಿರ bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  ಯದಿ(part->fp)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  part->fp = fopen_read(part->data, "rb");
  ಪ್ರತ್ಯಾಗಮ part->fp ? FALSE : TRUE;
}

ಸ್ಥಿರ size_t mime_file_read(ವರ್ಣ *buffer, size_t size, size_t nitems,
                             ಶೂನ್ಯ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ಯದಿ(!nitems)
    ಪ್ರತ್ಯಾಗಮ STOP_FILLING;

  ಯದಿ(mime_open_file(part))
    ಪ್ರತ್ಯಾಗಮ READ_ERROR;

  ಪ್ರತ್ಯಾಗಮ fread(buffer, size, nitems, part->fp);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mime_file_seek(ಶೂನ್ಯ *instream, curl_off_t offset, ಪೂರ್ಣಾಙ್ಕ whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ಯದಿ(whence == SEEK_SET && !offset && !part->fp)
    ಪ್ರತ್ಯಾಗಮ CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  ಯದಿ(mime_open_file(part))
    ಪ್ರತ್ಯಾಗಮ CURL_SEEKFUNC_FAIL;

  ಪ್ರತ್ಯಾಗಮ curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ mime_file_free(ಶೂನ್ಯ *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  ಯದಿ(part->fp) {
    curlx_fclose(part->fp);
    part->fp = ರಿಕ್ತ;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
ಸ್ಥಿರ size_t readback_bytes(ಸಂರಚನಾ mime_state *state,
                             ವರ್ಣ *buffer, size_t bufsize,
                             ಧ್ರುವ ವರ್ಣ *bytes, size_t numbytes,
                             ಧ್ರುವ ವರ್ಣ *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  ಯದಿ(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  ಅನ್ಯಥಾ {
    sz = offset - numbytes;
    ಯದಿ(sz >= traillen)
      ಪ್ರತ್ಯಾಗಮ 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  ಯದಿ(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  ಪ್ರತ್ಯಾಗಮ sz;
}

/* Read a non-encoded part content. */

ಸ್ಥಿರ size_t read_part_content(curl_mimepart *part, ವರ್ಣ *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  ವಿಕಲ್ಪ(part->lastreadstatus) {
  ಪಕ್ಷ 0:
  ಪಕ್ಷ CURL_READFUNC_ABORT:
  ಪಕ್ಷ CURL_READFUNC_PAUSE:
  ಪಕ್ಷ READ_ERROR:
    ಪ್ರತ್ಯಾಗಮ part->lastreadstatus;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  /* If we can determine we are at end of part data, spare a read. */
  ಯದಿ(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  ಅನ್ಯಥಾ {
    ವಿಕಲ್ಪ(part->kind) {
    ಪಕ್ಷ MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMEKIND_FILE:
      ಯದಿ(part->fp && feof(part->fp))
        ಭಙ್ಗ;  /* At EOF. */
      FALLTHROUGH();
    ಸ್ವಭಾವ:
      ಯದಿ(part->readfunc) {
        ಯದಿ(!(part->flags & MIME_FAST_READ)) {
          ಯದಿ(*hasread)
            ಪ್ರತ್ಯಾಗಮ STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      ಭಙ್ಗ;
    }
  }

  ವಿಕಲ್ಪ(sz) {
  ಪಕ್ಷ STOP_FILLING:
    ಭಙ್ಗ;
  ಪಕ್ಷ 0:
  ಪಕ್ಷ CURL_READFUNC_ABORT:
  ಪಕ್ಷ CURL_READFUNC_PAUSE:
  ಪಕ್ಷ READ_ERROR:
    part->lastreadstatus = sz;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ sz;
}

/* Read and encode part content. */
ಸ್ಥಿರ size_t read_encoded_part_content(curl_mimepart *part, ವರ್ಣ *buffer,
                                        size_t bufsize, bool *hasread)
{
  ಸಂರಚನಾ mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  ಪ್ರತಿ(;;) {
    ಯದಿ(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      ವಿಕಲ್ಪ(sz) {
      ಪಕ್ಷ 0:
        ಯದಿ(ateof)
          ಪ್ರತ್ಯಾಗಮ cursize;
        ಭಙ್ಗ;
      ಪಕ್ಷ READ_ERROR:
      ಪಕ್ಷ STOP_FILLING:
        ಪ್ರತ್ಯಾಗಮ cursize ? cursize : sz;
      ಸ್ವಭಾವ:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        ಅನುವರ್ತ;
      }
    }

    /* We need more data in input buffer. */
    ಯದಿ(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      ಯದಿ(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    ಯದಿ(st->bufend >= ಪರಿಮಾಣ(st->buf))
      ಪ್ರತ್ಯಾಗಮ cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           ಪರಿಮಾಣ(st->buf) - st->bufend, hasread);
    ವಿಕಲ್ಪ(sz) {
    ಪಕ್ಷ 0:
      ateof = TRUE;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_READFUNC_ABORT:
    ಪಕ್ಷ CURL_READFUNC_PAUSE:
    ಪಕ್ಷ READ_ERROR:
    ಪಕ್ಷ STOP_FILLING:
      ಪ್ರತ್ಯಾಗಮ cursize ? cursize : sz;
    ಸ್ವಭಾವ:
      st->bufend += sz;
      ಭಙ್ಗ;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
ಸ್ಥಿರ size_t readback_part(curl_mimepart *part,
                            ವರ್ಣ *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  ಯಾವತ್(bufsize) {
    size_t sz = 0;
    ಸಂರಚನಾ curl_slist *hdr = (ಸಂರಚನಾ curl_slist *)part->state.ptr;
    ವಿಕಲ್ಪ(part->state.state) {
    ಪಕ್ಷ MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_USERHEADERS:
      ಯದಿ(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, ರಿಕ್ತ);
        ಭಙ್ಗ;
      }
      ಯದಿ(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        ಭಙ್ಗ;
      }
      FALLTHROUGH();
    ಪಕ್ಷ MIMESTATE_CURLHEADERS:
      ಯದಿ(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      ಅನ್ಯಥಾ {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        ಯದಿ(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      ಯದಿ(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, ರಿಕ್ತ);
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, ರಿಕ್ತ);
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_CONTENT:
      ಯದಿ(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      ಅನ್ಯಥಾ
        sz = read_part_content(part, buffer, bufsize, hasread);
      ವಿಕಲ್ಪ(sz) {
      ಪಕ್ಷ 0:
        mimesetstate(&part->state, MIMESTATE_END, ರಿಕ್ತ);
        /* Try sparing open file descriptors. */
        ಯದಿ(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = ರಿಕ್ತ;
        }
        FALLTHROUGH();
      ಪಕ್ಷ CURL_READFUNC_ABORT:
      ಪಕ್ಷ CURL_READFUNC_PAUSE:
      ಪಕ್ಷ READ_ERROR:
      ಪಕ್ಷ STOP_FILLING:
        ಪ್ರತ್ಯಾಗಮ cursize ? cursize : sz;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_END:
      ಪ್ರತ್ಯಾಗಮ cursize;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  ಪ್ರತ್ಯಾಗಮ cursize;
}

/* Readback from mime. Warning: not a read callback function. */
ಸ್ಥಿರ size_t mime_subparts_read(ವರ್ಣ *buffer, size_t size, size_t nitems,
                                 ಶೂನ್ಯ *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (ಶೂನ್ಯ)size;  /* Always 1 */

  ಯಾವತ್(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    ವಿಕಲ್ಪ(mime->state.state) {
    ಪಕ್ಷ MIMESTATE_BEGIN:
    ಪಕ್ಷ MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      ಯದಿ(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_BOUNDARY2:
      ಯದಿ(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      ಅನ್ಯಥಾ
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      ಯದಿ(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_CONTENT:
      ಯದಿ(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, ರಿಕ್ತ);
        ಭಙ್ಗ;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      ವಿಕಲ್ಪ(sz) {
      ಪಕ್ಷ CURL_READFUNC_ABORT:
      ಪಕ್ಷ CURL_READFUNC_PAUSE:
      ಪಕ್ಷ READ_ERROR:
      ಪಕ್ಷ STOP_FILLING:
        ಪ್ರತ್ಯಾಗಮ cursize ? cursize : sz;
      ಪಕ್ಷ 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMESTATE_END:
      ಪ್ರತ್ಯಾಗಮ cursize;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  ಪ್ರತ್ಯಾಗಮ cursize;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mime_part_rewind(curl_mimepart *part)
{
  ಪೂರ್ಣಾಙ್ಕ res = CURL_SEEKFUNC_OK;
  ಗಣನಾ mimestate targetstate = MIMESTATE_BEGIN;

  ಯದಿ(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  ಯದಿ(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    ಯದಿ(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      ವಿಕಲ್ಪ(res) {
      ಪಕ್ಷ CURL_SEEKFUNC_OK:
      ಪಕ್ಷ CURL_SEEKFUNC_FAIL:
      ಪಕ್ಷ CURL_SEEKFUNC_CANTSEEK:
        ಭಙ್ಗ;
      ಪಕ್ಷ -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        res = CURL_SEEKFUNC_FAIL;
        ಭಙ್ಗ;
      }
    }
  }

  ಯದಿ(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, ರಿಕ್ತ);

  part->lastreadstatus = 1; /* Successful read status. */
  ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mime_subparts_seek(ಶೂನ್ಯ *instream, curl_off_t offset, ಪೂರ್ಣಾಙ್ಕ whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  ಪೂರ್ಣಾಙ್ಕ rc = CURL_SEEKFUNC_OK;

  ಯದಿ(whence != SEEK_SET || offset)
    ಪ್ರತ್ಯಾಗಮ CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  ಯದಿ(mime->state.state == MIMESTATE_BEGIN)
    ಪ್ರತ್ಯಾಗಮ CURL_SEEKFUNC_OK;           /* Already rewound. */

  ಪ್ರತಿ(part = mime->firstpart; part; part = part->nextpart) {
    ಪೂರ್ಣಾಙ್ಕ res = mime_part_rewind(part);
    ಯದಿ(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  ಯದಿ(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ರಿಕ್ತ);

  ಪ್ರತ್ಯಾಗಮ rc;
}

/* Release part content. */
ಸ್ಥಿರ ಶೂನ್ಯ cleanup_part_content(curl_mimepart *part)
{
  ಯದಿ(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = ರಿಕ್ತ;
  part->seekfunc = ರಿಕ್ತ;
  part->freefunc = ರಿಕ್ತ;
  part->arg = (ಶೂನ್ಯ *)part;           /* Defaults to part itself. */
  part->data = ರಿಕ್ತ;
  part->fp = ರಿಕ್ತ;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned ಪೂರ್ಣಾಙ್ಕ)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

ಸ್ಥಿರ ಶೂನ್ಯ mime_subparts_free(ಶೂನ್ಯ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ಯದಿ(mime && mime->parent) {
    mime->parent->freefunc = ರಿಕ್ತ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
ಸ್ಥಿರ ಶೂನ್ಯ mime_subparts_unbind(ಶೂನ್ಯ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ಯದಿ(mime && mime->parent) {
    mime->parent->freefunc = ರಿಕ್ತ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = ರಿಕ್ತ;
  }
}

ಶೂನ್ಯ Curl_mime_cleanpart(curl_mimepart *part)
{
  ಯದಿ(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    ಯದಿ(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
ಶೂನ್ಯ curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  ಯದಿ(!mime)
    ಪ್ರತ್ಯಾಗಮ;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  ಯಾವತ್(mime) {
    part = mime->firstpart;
    ಯದಿ(part) {
      mime->firstpart = part->nextpart;
      ಯದಿ(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = ರಿಕ್ತ;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    ಅನ್ಯಥಾ {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

ಸ್ಥಿರ bool mime_is_too_deep(ಧ್ರುವ curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  ಯದಿ(++call_depth > MAX_MIME_LEVELS)
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಯದಿ(part->kind == MIMEKIND_MULTIPART && part->arg) {
    ಧ್ರುವ curl_mime *mime = (ಧ್ರುವ curl_mime *)part->arg;
    ಧ್ರುವ curl_mimepart *s;

    ಪ್ರತಿ(s = mime->firstpart; s; s = s->nextpart) {
      ಯದಿ(mime_is_too_deep(s, call_depth))
        ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode mime_duppart(ಸಂರಚನಾ Curl_easy *data,
                             curl_mimepart *dst, ಧ್ರುವ curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  ಧ್ರುವ curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  ಯದಿ(++call_depth > MAX_MIME_LEVELS)
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;

  /* Duplicate content. */
  ವಿಕಲ್ಪ(src->kind) {
  ಪಕ್ಷ MIMEKIND_NONE:
    ಭಙ್ಗ;
  ಪಕ್ಷ MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    ಭಙ್ಗ;
  ಪಕ್ಷ MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    ಯದಿ(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    ಭಙ್ಗ;
  ಪಕ್ಷ MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    ಭಙ್ಗ;
  ಪಕ್ಷ MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    ಪ್ರತಿ(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    ಭಙ್ಗ;
  }

  /* Duplicate headers. */
  ಯದಿ(!result && src->userheaders) {
    ಸಂರಚನಾ curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    ಯದಿ(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    ಅನ್ಯಥಾ {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      ಯದಿ(result)
        curl_slist_free_all(hdrs);
    }
  }

  ಯದಿ(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  ಯದಿ(!result)
    result = curl_mime_name(dst, src->name);
  ಯದಿ(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  ಯದಿ(result)
    Curl_mime_cleanpart(dst);

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_mime_duppart(ಸಂರಚನಾ Curl_easy *data,
                           curl_mimepart *dst, ಧ್ರುವ curl_mimepart *src)
{
  ಯದಿ(!dst || !src)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(mime_is_too_deep(src, 0))
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;

  ಪ್ರತ್ಯಾಗಮ mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(ಶೂನ್ಯ *easy)
{
  curl_mime *mime = curlx_malloc(ಪರಿಮಾಣ(*mime));

  ಯದಿ(mime) {
    mime->parent = ರಿಕ್ತ;
    mime->firstpart = ರಿಕ್ತ;
    mime->lastpart = ರಿಕ್ತ;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    ಯದಿ(Curl_rand_alnum(easy,
                       (unsigned ವರ್ಣ *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ರಿಕ್ತ);
  }

  ಪ್ರತ್ಯಾಗಮ mime;
}

/* Initialize a mime part. */
ಶೂನ್ಯ Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, ಪರಿಮಾಣ(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, ರಿಕ್ತ);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  ಯದಿ(!mime)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  part = curlx_malloc(ಪರಿಮಾಣ(*part));

  ಯದಿ(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    ಯದಿ(mime->lastpart)
      mime->lastpart->nextpart = part;
    ಅನ್ಯಥಾ
      mime->firstpart = part;

    mime->lastpart = part;
  }

  ಪ್ರತ್ಯಾಗಮ part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *name)
{
  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  ಯದಿ(name) {
    part->name = curlx_strdup(name);
    ಯದಿ(!part->name)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *filename)
{
  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  ಯದಿ(filename) {
    part->filename = curlx_strdup(filename);
    ಯದಿ(!part->filename)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *data, size_t datasize)
{
  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ಯದಿ(data) {
    ಯದಿ(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    ಯದಿ(!part->data)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *filename)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ಯದಿ(filename) {
    ವರ್ಣ *base;
    curlx_struct_stat sbuf;

    ಯದಿ(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    ಅನ್ಯಥಾ {
      part->data = curlx_strdup(filename);
      ಯದಿ(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      ಅನ್ಯಥಾ {
        part->datasize = -1;
        ಯದಿ(S_ISREG(sbuf.st_mode)) {
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
        ಯದಿ(!base)
          result = CURLE_OUT_OF_MEMORY;
        ಅನ್ಯಥಾ {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *mimetype)
{
  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  ಯದಿ(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    ಯದಿ(!part->mimetype)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  ಧ್ರುವ ಸಂರಚನಾ mime_encoder *mep;

  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ result;

  part->encoder = ರಿಕ್ತ;

  ಯದಿ(!encoding)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;    /* Removing current encoder. */

  ಪ್ರತಿ(mep = encoders; mep->name; mep++)
    ಯದಿ(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           ಸಂರಚನಾ curl_slist *headers, ಪೂರ್ಣಾಙ್ಕ take_ownership)
{
  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(part->flags & MIME_USERHEADERS_OWNER) {
    ಯದಿ(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned ಪೂರ್ಣಾಙ್ಕ)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  ಯದಿ(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, ಶೂನ್ಯ *arg)
{
  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  ಯದಿ(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, ಪೂರ್ಣಾಙ್ಕ take_ownership)
{
  curl_mime *ಬೇರು;

  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  ಯದಿ(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  cleanup_part_content(part);

  ಯದಿ(subparts) {
    /* Should not have been attached already. */
    ಯದಿ(subparts->parent)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    ಬೇರು = part->parent;
    ಯದಿ(ಬೇರು) {
      ಯಾವತ್(ಬೇರು->parent && ಬೇರು->parent->parent)
        ಬೇರು = ಬೇರು->parent->parent;
      ಯದಿ(subparts == ಬೇರು) {
        /* cannot add as a subpart of itself. */
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    ಯದಿ(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  ಪ್ರತ್ಯಾಗಮ Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(ವರ್ಣ *buffer, size_t size, size_t nitems, ಶೂನ್ಯ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (ಶೂನ್ಯ)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  ಕುರು {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } ಯಾವತ್(ret == STOP_FILLING);

  ಪ್ರತ್ಯಾಗಮ ret;
}

/* Rewind mime stream. */
ಸ್ಥಿರ CURLcode mime_rewind(curl_mimepart *part)
{
  ಪ್ರತ್ಯಾಗಮ mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
ಸ್ಥಿರ size_t slist_size(ಸಂರಚನಾ curl_slist *s,
                         size_t overhead, ಧ್ರುವ ವರ್ಣ *skip, size_t skiplen)
{
  size_t size = 0;

  ಪ್ರತಿ(; s; s = s->next)
    ಯದಿ(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  ಪ್ರತ್ಯಾಗಮ size;
}

/* Get/compute multipart size. */
ಸ್ಥಿರ curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  ಯದಿ(!mime)
    ಪ್ರತ್ಯಾಗಮ 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  ಪ್ರತಿ(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    ಯದಿ(sz < 0)
      size = sz;

    ಯದಿ(size >= 0)
      size += boundarysize + sz;
  }

  ಪ್ರತ್ಯಾಗಮ size;
}

/* Get/compute mime size. */
ಸ್ಥಿರ curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  ಯದಿ(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  ಯದಿ(part->encoder)
    size = part->encoder->sizefunc(part);

  ಯದಿ(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, ರಿಕ್ತ, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  ಪ್ರತ್ಯಾಗಮ size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(ಸಂರಚನಾ curl_slist **slp, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  ಸಂರಚನಾ curl_slist *hdr = ರಿಕ್ತ;
  ವರ್ಣ *s = ರಿಕ್ತ;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  ಯದಿ(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    ಯದಿ(hdr)
      *slp = hdr;
    ಅನ್ಯಥಾ
      curlx_free(s);
  }

  ಪ್ರತ್ಯಾಗಮ hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
ಸ್ಥಿರ CURLcode add_content_type(ಸಂರಚನಾ curl_slist **slp,
                                 ಧ್ರುವ ವರ್ಣ *type, ಧ್ರುವ ವರ್ಣ *boundary)
{
  ಪ್ರತ್ಯಾಗಮ Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ಧ್ರುವ ವರ್ಣ *Curl_mime_contenttype(ಧ್ರುವ ವರ್ಣ *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  ಸಂರಚನಾ ContentType {
    ಧ್ರುವ ವರ್ಣ *extension;
    ಧ್ರುವ ವರ್ಣ *type;
  };
  ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ ContentType ctts[] = {
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

  ಯದಿ(filename) {
    size_t len1 = strlen(filename);
    ಧ್ರುವ ವರ್ಣ *nameend = filename + len1;
    unsigned ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      ಯದಿ(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        ಪ್ರತ್ಯಾಗಮ ctts[i].type;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ bool content_type_match(ಧ್ರುವ ವರ್ಣ *contenttype,
                               ಧ್ರುವ ವರ್ಣ *target, size_t len)
{
  ಯದಿ(contenttype && curl_strnequal(contenttype, target, len))
    ವಿಕಲ್ಪ(contenttype[len]) {
    ಪಕ್ಷ '\0':
    ಪಕ್ಷ '\t':
    ಪಕ್ಷ '\r':
    ಪಕ್ಷ '\n':
    ಪಕ್ಷ ' ':
    ಪಕ್ಷ ';':
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode add_content_disposition(ಸಂರಚನಾ Curl_easy *data,
                                        curl_mimepart *part,
                                        ಧ್ರುವ ವರ್ಣ *disposition,
                                        ಧ್ರುವ ವರ್ಣ *contenttype,
                                        ಗಣನಾ mimestrategy strategy)
{
  ಯದಿ(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  ಯದಿ(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = ರಿಕ್ತ;

  ಯದಿ(disposition) {
    CURLcode result = CURLE_OK;
    ವರ್ಣ *name = ರಿಕ್ತ;
    ವರ್ಣ *filename = ರಿಕ್ತ;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    ಯದಿ(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

    ಯದಿ(part->name) {
      name = escape_string(data, part->name, strategy);
      ಯದಿ(!name)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ಯದಿ(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      ಯದಿ(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    ಯದಿ(!result)
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
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mime_prepare_headers(ಸಂರಚನಾ Curl_easy *data,
                                     curl_mimepart *part,
                                     ಧ್ರುವ ವರ್ಣ *contenttype,
                                     ಧ್ರುವ ವರ್ಣ *disposition,
                                     ಗಣನಾ mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *boundary = ರಿಕ್ತ;
  ವರ್ಣ *customct;
  ಧ್ರುವ ವರ್ಣ *cte = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  ಯದಿ(++call_depth > MAX_MIME_LEVELS)
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = ರಿಕ್ತ;

  /* Be sure we will not access old headers later. */
  ಯದಿ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, ರಿಕ್ತ);

  /* Check if content type is specified. */
  customct = part->mimetype;
  ಯದಿ(!customct)
    customct = ಹುಡುಕು_header(part->userheaders, STRCONST("Content-Type"));
  ಯದಿ(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  ಯದಿ(!contenttype) {
    ವಿಕಲ್ಪ(part->kind) {
    ಪಕ್ಷ MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      ಭಙ್ಗ;
    ಪಕ್ಷ MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      ಯದಿ(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      ಯದಿ(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      contenttype = Curl_mime_contenttype(part->filename);
      ಭಙ್ಗ;
    }
  }

  ಯದಿ(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    ಯದಿ(mime)
      boundary = mime->boundary;
  }
  ಅನ್ಯಥಾ ಯದಿ(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = ರಿಕ್ತ;

  /* Issue content-disposition header only if not already set by caller. */
  ಯದಿ(!ಹುಡುಕು_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Issue Content-Type header. */
  ಯದಿ(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Content-Transfer-Encoding header. */
  ಯದಿ(!ಹುಡುಕು_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    ಯದಿ(part->encoder)
      cte = part->encoder->name;
    ಅನ್ಯಥಾ ಯದಿ(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    ಯದಿ(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  ಯದಿ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  ಯದಿ(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = ರಿಕ್ತ;
    ಯದಿ(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    ಪ್ರತಿ(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, ರಿಕ್ತ,
                                    disposition, strategy, call_depth);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_mime_prepare_headers(ಸಂರಚನಾ Curl_easy *data,
                                   curl_mimepart *part,
                                   ಧ್ರುವ ವರ್ಣ *contenttype,
                                   ಧ್ರುವ ವರ್ಣ *disposition,
                                   ಗಣನಾ mimestrategy strategy)
{
  ಪ್ರತ್ಯಾಗಮ mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
ಸ್ಥಿರ ಶೂನ್ಯ mime_unpause(curl_mimepart *part)
{
  ಯದಿ(part) {
    ಯದಿ(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    ಯದಿ(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      ಯದಿ(mime) {
        curl_mimepart *subpart;

        ಪ್ರತಿ(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

ಸಂರಚನಾ cr_mime_ctx {
  ಸಂರಚನಾ Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  ಸಂರಚನಾ bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

ಸ್ಥಿರ CURLcode cr_mime_init(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_mime_close(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
ಸ್ಥಿರ CURLcode cr_mime_read(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_creader *reader,
                             ವರ್ಣ *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  ಸಂರಚನಾ cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  ವರ್ಣ tmp[256];

  /* Once we have errored, we will return the same error forever */
  ಯದಿ(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (ಪೂರ್ಣಾಙ್ಕ)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    ಪ್ರತ್ಯಾಗಮ ctx->error_result;
  }
  ಯದಿ(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  /* respect length limitations */
  ಯದಿ(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    ಯದಿ(remain <= 0)
      blen = 0;
    ಅನ್ಯಥಾ ಯದಿ(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  ಯದಿ(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned ವರ್ಣ *)buf, blen, &nread);
    ಯದಿ(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, ಪರಿಮಾಣ(tmp), ctx->part);
    ಯದಿ(nread <= ಪರಿಮಾಣ(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned ವರ್ಣ *)tmp, nread, &n);
      ಯದಿ(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ಪ್ರತ್ಯಾಗಮ result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      ಯದಿ(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        ಪ್ರತ್ಯಾಗಮ result;
      }
    }
  }
  ಅನ್ಯಥಾ
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  ವಿಕಲ್ಪ(nread) {
  ಪಕ್ಷ 0:
    ಯದಿ((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    ಭಙ್ಗ; /* nothing was read */

  ಪಕ್ಷ STOP_FILLING:
  ಪಕ್ಷ READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    ಯದಿ(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    ಯದಿ(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    ಭಙ್ಗ;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread, *peos);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cr_mime_needs_rewind(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ ctx->read_len > 0;
}

ಸ್ಥಿರ curl_off_t cr_mime_total_length(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ ctx->total_len;
}

ಸ್ಥಿರ CURLcode cr_mime_resume_from(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ Curl_creader *reader,
                                    curl_off_t offset)
{
  ಸಂರಚನಾ cr_mime_ctx *ctx = reader->ctx;

  ಯದಿ(offset > 0) {
    curl_off_t passed = 0;

    ಕುರು {
      ವರ್ಣ scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)ಪರಿಮಾಣ(scratch)) ?
        ಪರಿಮಾಣ(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      ಯದಿ((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
      }
    } ಯಾವತ್(passed < offset);

    /* now, decrease the size of the read */
    ಯದಿ(ctx->total_len > 0) {
      ctx->total_len -= offset;

      ಯದಿ(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        ಪ್ರತ್ಯಾಗಮ CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cr_mime_cntrl(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  ಸಂರಚನಾ cr_mime_ctx *ctx = reader->ctx;
  ವಿಕಲ್ಪ(opcode) {
  ಪಕ್ಷ CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    ಯದಿ(result)
      failf(data, "Cannot rewind mime/post data");
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪಕ್ಷ CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool cr_mime_is_paused(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_crtype cr_mime = {
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
  ಪರಿಮಾಣ(ಸಂರಚನಾ cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(ಸಂರಚನಾ Curl_easy *data, curl_mimepart *part)
{
  ಸಂರಚನಾ Curl_creader *r;
  ಸಂರಚನಾ cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  ಯದಿ(result) {
    Curl_creader_free(data, r);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ctx->total_len = mime_size(ctx->part);

  ಪ್ರತ್ಯಾಗಮ Curl_creader_set(data, r);
}

#ಅನ್ಯಥಾ /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (ಶೂನ್ಯ)easy;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ curl_mime_free(curl_mime *mime)
{
  (ಶೂನ್ಯ)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (ಶೂನ್ಯ)mime;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

CURLcode curl_mime_name(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *name)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)name;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *filename)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)filename;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *mimetype)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)mimetype;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *encoding)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)encoding;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *data, size_t datasize)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)datasize;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *filename)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)filename;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           ಶೂನ್ಯ *arg)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)datasize;
  (ಶೂನ್ಯ)readfunc;
  (ಶೂನ್ಯ)seekfunc;
  (ಶೂನ್ಯ)freefunc;
  (ಶೂನ್ಯ)arg;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)subparts;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           ಸಂರಚನಾ curl_slist *headers, ಪೂರ್ಣಾಙ್ಕ take_ownership)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)headers;
  (ಶೂನ್ಯ)take_ownership;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(ಸಂರಚನಾ curl_slist **slp, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)slp;
  (ಶೂನ್ಯ)fmt;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

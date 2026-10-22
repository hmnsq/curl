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
#അന്തര്ഭാവ "curl_setup.h"

ഘടന Curl_easy;

#അന്തര്ഭാവ "mime.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/basename.h"
#അന്തര്ഭാവ "curlx/strcopy.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "curlx/base64.h"

#എങ്കിൽ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "slist.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

സ്ഥിതം size_t mime_subparts_read(അക്ഷരം *buffer, size_t size, size_t nitems,
                                 ശൂന്യം *instream, bool *hasread);
സ്ഥിതം curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം qp_class[] = {
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
സ്ഥിതം സ്ഥിരം അക്ഷരം aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#അല്ലെങ്കിൽ

#അന്തര്ഭാവ <fabdef.h>
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
curl_off_t VmsRealFileSize(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം curlx_struct_stat *stat_buf)
{
  അക്ഷരം buffer[8192];
  curl_off_t എണ്ണം;
  പൂർണ്ണസംഖ്യ ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  എങ്കിൽ(!file)
    തിരികെ 0;

  എണ്ണം = 0;
  ret_stat = 1;
  ആകുമ്പോൾ(ret_stat > 0) {
    ret_stat = fread(buffer, 1, വലുപ്പം(buffer), file);
    എങ്കിൽ(ret_stat)
      എണ്ണം += ret_stat;
  }
  curlx_fclose(file);

  തിരികെ എണ്ണം;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
സ്ഥിതം curl_off_t VmsSpecialSize(സ്ഥിരം അക്ഷരം *name,
                                 സ്ഥിരം curlx_struct_stat *stat_buf)
{
  തിരഞ്ഞെടുപ്പ്(stat_buf->st_fab_rfm) {
  അവസ്ഥ FAB$C_VAR:
  അവസ്ഥ FAB$C_VFC:
    തിരികെ VmsRealFileSize(name, stat_buf);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ stat_buf->st_size;
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
സ്ഥിതം FILE *vmsfopenread(സ്ഥിരം അക്ഷരം *file, സ്ഥിരം അക്ഷരം *mode)
{
  curlx_struct_stat statbuf;
  പൂർണ്ണസംഖ്യ res = curlx_stat(file, &statbuf);

  എങ്കിൽ(res != -1) {
    തിരഞ്ഞെടുപ്പ്(statbuf.st_fab_rfm) {
    അവസ്ഥ FAB$C_VAR:
    അവസ്ഥ FAB$C_VFC:
    അവസ്ഥ FAB$C_STMCR:
      തിരികെ curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      തിരികെ curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  തിരികെ ശൂന്യ;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
സ്ഥിതം ശൂന്യം mimesetstate(ഘടന mime_state *state,
                         എണ്ണൽ mimestate tok, ശൂന്യം *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
സ്ഥിതം അക്ഷരം *escape_string(ഘടന Curl_easy *data,
                           സ്ഥിരം അക്ഷരം *src, എണ്ണൽ mimestrategy strategy)
{
  CURLcode result;
  ഘടന dynbuf db;
  സ്ഥിരം അക്ഷരം * സ്ഥിരം *table;
  സ്ഥിരം അക്ഷരം * സ്ഥിരം *p;
  /* replace first character by rest of string. */
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    ശൂന്യ
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    ശൂന്യ
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  എങ്കിൽ(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_നിവേശം_LENGTH);

  വേണ്ടി(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    വേണ്ടി(p = table; *p && **p != *src; p++)
      ;

    എങ്കിൽ(*p)
      result = curlx_dyn_add(&db, *p + 1);
    അല്ലെങ്കിൽ
      result = curlx_dyn_addn(&db, src, 1);
  }

  തിരികെ curlx_dyn_ptr(&db);
}

/* Check if header matches. */
സ്ഥിതം അക്ഷരം *match_header(ഘടന curl_slist *hdr, സ്ഥിരം അക്ഷരം *lbl, size_t len)
{
  അക്ഷരം *മൂല്യം = ശൂന്യ;

  എങ്കിൽ(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    വേണ്ടി(മൂല്യം = hdr->data + len + 1; *മൂല്യം == ' '; മൂല്യം++)
      ;
  തിരികെ മൂല്യം;
}

/* Get a header from an slist. */
സ്ഥിതം അക്ഷരം *തിരയൽ_header(ഘടന curl_slist *hdrlist,
                           സ്ഥിരം അക്ഷരം *hdr, size_t len)
{
  അക്ഷരം *മൂല്യം = ശൂന്യ;

  വേണ്ടി(; !മൂല്യം && hdrlist; hdrlist = hdrlist->next)
    മൂല്യം = match_header(hdrlist, hdr, len);

  തിരികെ മൂല്യം;
}

സ്ഥിതം അക്ഷരം *strippath(സ്ഥിരം അക്ഷരം *fullfile)
{
  അക്ഷരം *filename;
  അക്ഷരം *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  എങ്കിൽ(!filename)
    തിരികെ ശൂന്യ;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  തിരികെ base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
സ്ഥിതം ശൂന്യം cleanup_encoder_state(ഘടന mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
സ്ഥിതം size_t encoder_nop_read(അക്ഷരം *buffer, size_t size, bool ateof,
                               ഘടന curl_mimepart *part)
{
  ഘടന mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (ശൂന്യം)ateof;

  എങ്കിൽ(!size)
    തിരികെ STOP_FILLING;

  എങ്കിൽ(size > insize)
    size = insize;

  എങ്കിൽ(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  തിരികെ size;
}

സ്ഥിതം curl_off_t encoder_nop_size(curl_mimepart *part)
{
  തിരികെ part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
സ്ഥിതം size_t encoder_7bit_read(അക്ഷരം *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  ഘടന mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (ശൂന്യം)ateof;

  എങ്കിൽ(!size)
    തിരികെ STOP_FILLING;

  എങ്കിൽ(size > cursize)
    size = cursize;

  വേണ്ടി(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    എങ്കിൽ(*buffer++ & 0x80)
      തിരികെ cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  തിരികെ cursize;
}

/* Base64 content encoder. */
സ്ഥിതം size_t encoder_base64_read(അക്ഷരം *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  ഘടന mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  പൂർണ്ണസംഖ്യ i;
  അക്ഷരം *ptr = buffer;

  ആകുമ്പോൾ(st->bufbeg < st->bufend) {
    /* Line full ? */
    എങ്കിൽ(st->pos > MAX_ENCODED_വരി_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      എങ്കിൽ(size < 2) {
        എങ്കിൽ(!cursize)
          തിരികെ STOP_FILLING;
        നിർത്തുക;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    എങ്കിൽ(size < 4) {
      എങ്കിൽ(!cursize)
        തിരികെ STOP_FILLING;
      നിർത്തുക;
    }
    എങ്കിൽ(st->bufend - st->bufbeg < 3)
      നിർത്തുക;

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
  എങ്കിൽ(ateof) {
    എങ്കിൽ(size < 4) {
      എങ്കിൽ(!cursize)
        തിരികെ STOP_FILLING;
    }
    അല്ലെങ്കിൽ {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      എങ്കിൽ(st->bufend != st->bufbeg) {

        എങ്കിൽ(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        എങ്കിൽ(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  തിരികെ cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_നിവേശം_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_വരി_LENGTH + 2)) *                  \
    MAX_ENCODED_വരി_LENGTH / 4) * 3 - 3)

സ്ഥിതം curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  എങ്കിൽ(size <= 0)
    തിരികെ size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  എങ്കിൽ(size > BASE64_MAX_നിവേശം_SIZE)
    തിരികെ -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  തിരികെ size + (2 * ((size - 1) / MAX_ENCODED_വരി_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ qp_lookahead_eol(ഘടന mime_encoder_state *st, പൂർണ്ണസംഖ്യ ateof, size_t n)
{
  n += st->bufbeg;
  എങ്കിൽ(n >= st->bufend && ateof)
    തിരികെ 1;
  എങ്കിൽ(n + 2 > st->bufend)
    തിരികെ ateof ? 0 : -1;
  എങ്കിൽ(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    തിരികെ 1;
  തിരികെ 0;
}

/* Quoted-printable encoder. */
സ്ഥിതം size_t encoder_qp_read(അക്ഷരം *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  ഘടന mime_encoder_state *st = &part->encstate;
  അക്ഷരം *ptr = buffer;
  size_t cursize = 0;
  പൂർണ്ണസംഖ്യ softlinebreak;
  അക്ഷരം buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  ആകുമ്പോൾ(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    പൂർണ്ണസംഖ്യ i = (unsigned അക്ഷരം)st->buf[st->bufbeg];
    buf[0] = (അക്ഷരം)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    തിരഞ്ഞെടുപ്പ്(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    അവസ്ഥ QP_OK:          /* Not a special character. */
      നിർത്തുക;
    അവസ്ഥ QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      തിരഞ്ഞെടുപ്പ്(qp_lookahead_eol(st, ateof, 1)) {
      അവസ്ഥ -1:          /* More input data needed. */
        തിരികെ cursize;
      അവസ്ഥ 0:           /* No encoding needed. */
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        നിർത്തുക;
      }
      നിർത്തുക;
    അവസ്ഥ QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      തിരഞ്ഞെടുപ്പ്(qp_lookahead_eol(st, ateof, 0)) {
      അവസ്ഥ -1:          /* Need more data. */
        തിരികെ cursize;
      അവസ്ഥ 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        നിർത്തുക;
      }
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      നിർത്തുക;
    }

    /* Be sure the encoded character fits within maximum line length. */
    എങ്കിൽ(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_വരി_LENGTH;
      എങ്കിൽ(!softlinebreak && st->pos + len == MAX_ENCODED_വരി_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        തിരഞ്ഞെടുപ്പ്(qp_lookahead_eol(st, ateof, consumed)) {
        അവസ്ഥ -1:        /* Need more data. */
          തിരികെ cursize;
        അവസ്ഥ 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          നിർത്തുക;
        }
      }
      എങ്കിൽ(softlinebreak) {
        curlx_strcopy(buf, വലുപ്പം(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    എങ്കിൽ(len > size) {
      എങ്കിൽ(!cursize)
        തിരികെ STOP_FILLING;
      നിർത്തുക;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    എങ്കിൽ(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  തിരികെ cursize;
}

സ്ഥിതം curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  തിരികെ part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
സ്ഥിതം size_t mime_mem_read(അക്ഷരം *buffer, size_t size, size_t nitems,
                            ശൂന്യം *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (ശൂന്യം)size;  /* Always 1 */

  എങ്കിൽ(!nitems)
    തിരികെ STOP_FILLING;

  എങ്കിൽ(sz > nitems)
    sz = nitems;

  എങ്കിൽ(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  തിരികെ sz;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mime_mem_seek(ശൂന്യം *instream, curl_off_t offset, പൂർണ്ണസംഖ്യ whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  തിരഞ്ഞെടുപ്പ്(whence) {
  അവസ്ഥ SEEK_CUR:
    offset += part->state.offset;
    നിർത്തുക;
  അവസ്ഥ SEEK_END:
    offset += part->datasize;
    നിർത്തുക;
  }

  എങ്കിൽ(offset < 0 || offset > part->datasize)
    തിരികെ CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  തിരികെ CURL_SEEKFUNC_OK;
}

സ്ഥിതം ശൂന്യം mime_mem_free(ശൂന്യം *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
സ്ഥിതം bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  എങ്കിൽ(part->fp)
    തിരികെ FALSE;
  part->fp = fopen_read(part->data, "rb");
  തിരികെ part->fp ? FALSE : TRUE;
}

സ്ഥിതം size_t mime_file_read(അക്ഷരം *buffer, size_t size, size_t nitems,
                             ശൂന്യം *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  എങ്കിൽ(!nitems)
    തിരികെ STOP_FILLING;

  എങ്കിൽ(mime_open_file(part))
    തിരികെ READ_ERROR;

  തിരികെ fread(buffer, size, nitems, part->fp);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mime_file_seek(ശൂന്യം *instream, curl_off_t offset, പൂർണ്ണസംഖ്യ whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  എങ്കിൽ(whence == SEEK_SET && !offset && !part->fp)
    തിരികെ CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  എങ്കിൽ(mime_open_file(part))
    തിരികെ CURL_SEEKFUNC_FAIL;

  തിരികെ curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

സ്ഥിതം ശൂന്യം mime_file_free(ശൂന്യം *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  എങ്കിൽ(part->fp) {
    curlx_fclose(part->fp);
    part->fp = ശൂന്യ;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
സ്ഥിതം size_t readback_bytes(ഘടന mime_state *state,
                             അക്ഷരം *buffer, size_t bufsize,
                             സ്ഥിരം അക്ഷരം *bytes, size_t numbytes,
                             സ്ഥിരം അക്ഷരം *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  എങ്കിൽ(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  അല്ലെങ്കിൽ {
    sz = offset - numbytes;
    എങ്കിൽ(sz >= traillen)
      തിരികെ 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  എങ്കിൽ(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  തിരികെ sz;
}

/* Read a non-encoded part content. */

സ്ഥിതം size_t read_part_content(curl_mimepart *part, അക്ഷരം *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  തിരഞ്ഞെടുപ്പ്(part->lastreadstatus) {
  അവസ്ഥ 0:
  അവസ്ഥ CURL_READFUNC_ABORT:
  അവസ്ഥ CURL_READFUNC_PAUSE:
  അവസ്ഥ READ_ERROR:
    തിരികെ part->lastreadstatus;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }

  /* If we can determine we are at end of part data, spare a read. */
  എങ്കിൽ(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  അല്ലെങ്കിൽ {
    തിരഞ്ഞെടുപ്പ്(part->kind) {
    അവസ്ഥ MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      നിർത്തുക;
    അവസ്ഥ MIMEKIND_FILE:
      എങ്കിൽ(part->fp && feof(part->fp))
        നിർത്തുക;  /* At EOF. */
      FALLTHROUGH();
    സ്ഥിരസ്ഥിതി:
      എങ്കിൽ(part->readfunc) {
        എങ്കിൽ(!(part->flags & MIME_FAST_READ)) {
          എങ്കിൽ(*hasread)
            തിരികെ STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      നിർത്തുക;
    }
  }

  തിരഞ്ഞെടുപ്പ്(sz) {
  അവസ്ഥ STOP_FILLING:
    നിർത്തുക;
  അവസ്ഥ 0:
  അവസ്ഥ CURL_READFUNC_ABORT:
  അവസ്ഥ CURL_READFUNC_PAUSE:
  അവസ്ഥ READ_ERROR:
    part->lastreadstatus = sz;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    നിർത്തുക;
  }

  തിരികെ sz;
}

/* Read and encode part content. */
സ്ഥിതം size_t read_encoded_part_content(curl_mimepart *part, അക്ഷരം *buffer,
                                        size_t bufsize, bool *hasread)
{
  ഘടന mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  വേണ്ടി(;;) {
    എങ്കിൽ(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      തിരഞ്ഞെടുപ്പ്(sz) {
      അവസ്ഥ 0:
        എങ്കിൽ(ateof)
          തിരികെ cursize;
        നിർത്തുക;
      അവസ്ഥ READ_ERROR:
      അവസ്ഥ STOP_FILLING:
        തിരികെ cursize ? cursize : sz;
      സ്ഥിരസ്ഥിതി:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        തുടരുക;
      }
    }

    /* We need more data in input buffer. */
    എങ്കിൽ(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      എങ്കിൽ(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    എങ്കിൽ(st->bufend >= വലുപ്പം(st->buf))
      തിരികെ cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           വലുപ്പം(st->buf) - st->bufend, hasread);
    തിരഞ്ഞെടുപ്പ്(sz) {
    അവസ്ഥ 0:
      ateof = TRUE;
      നിർത്തുക;
    അവസ്ഥ CURL_READFUNC_ABORT:
    അവസ്ഥ CURL_READFUNC_PAUSE:
    അവസ്ഥ READ_ERROR:
    അവസ്ഥ STOP_FILLING:
      തിരികെ cursize ? cursize : sz;
    സ്ഥിരസ്ഥിതി:
      st->bufend += sz;
      നിർത്തുക;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
സ്ഥിതം size_t readback_part(curl_mimepart *part,
                            അക്ഷരം *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  ആകുമ്പോൾ(bufsize) {
    size_t sz = 0;
    ഘടന curl_slist *hdr = (ഘടന curl_slist *)part->state.ptr;
    തിരഞ്ഞെടുപ്പ്(part->state.state) {
    അവസ്ഥ MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_USERHEADERS:
      എങ്കിൽ(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, ശൂന്യ);
        നിർത്തുക;
      }
      എങ്കിൽ(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        നിർത്തുക;
      }
      FALLTHROUGH();
    അവസ്ഥ MIMESTATE_CURLHEADERS:
      എങ്കിൽ(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      അല്ലെങ്കിൽ {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        എങ്കിൽ(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      എങ്കിൽ(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, ശൂന്യ);
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, ശൂന്യ);
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_CONTENT:
      എങ്കിൽ(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      അല്ലെങ്കിൽ
        sz = read_part_content(part, buffer, bufsize, hasread);
      തിരഞ്ഞെടുപ്പ്(sz) {
      അവസ്ഥ 0:
        mimesetstate(&part->state, MIMESTATE_END, ശൂന്യ);
        /* Try sparing open file descriptors. */
        എങ്കിൽ(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = ശൂന്യ;
        }
        FALLTHROUGH();
      അവസ്ഥ CURL_READFUNC_ABORT:
      അവസ്ഥ CURL_READFUNC_PAUSE:
      അവസ്ഥ READ_ERROR:
      അവസ്ഥ STOP_FILLING:
        തിരികെ cursize ? cursize : sz;
      }
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_END:
      തിരികെ cursize;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  തിരികെ cursize;
}

/* Readback from mime. Warning: not a read callback function. */
സ്ഥിതം size_t mime_subparts_read(അക്ഷരം *buffer, size_t size, size_t nitems,
                                 ശൂന്യം *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (ശൂന്യം)size;  /* Always 1 */

  ആകുമ്പോൾ(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    തിരഞ്ഞെടുപ്പ്(mime->state.state) {
    അവസ്ഥ MIMESTATE_BEGIN:
    അവസ്ഥ MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      എങ്കിൽ(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_BOUNDARY2:
      എങ്കിൽ(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      അല്ലെങ്കിൽ
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      എങ്കിൽ(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_CONTENT:
      എങ്കിൽ(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, ശൂന്യ);
        നിർത്തുക;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      തിരഞ്ഞെടുപ്പ്(sz) {
      അവസ്ഥ CURL_READFUNC_ABORT:
      അവസ്ഥ CURL_READFUNC_PAUSE:
      അവസ്ഥ READ_ERROR:
      അവസ്ഥ STOP_FILLING:
        തിരികെ cursize ? cursize : sz;
      അവസ്ഥ 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        നിർത്തുക;
      }
      നിർത്തുക;
    അവസ്ഥ MIMESTATE_END:
      തിരികെ cursize;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  തിരികെ cursize;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mime_part_rewind(curl_mimepart *part)
{
  പൂർണ്ണസംഖ്യ res = CURL_SEEKFUNC_OK;
  എണ്ണൽ mimestate targetstate = MIMESTATE_BEGIN;

  എങ്കിൽ(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  എങ്കിൽ(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    എങ്കിൽ(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      തിരഞ്ഞെടുപ്പ്(res) {
      അവസ്ഥ CURL_SEEKFUNC_OK:
      അവസ്ഥ CURL_SEEKFUNC_FAIL:
      അവസ്ഥ CURL_SEEKFUNC_CANTSEEK:
        നിർത്തുക;
      അവസ്ഥ -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        res = CURL_SEEKFUNC_FAIL;
        നിർത്തുക;
      }
    }
  }

  എങ്കിൽ(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, ശൂന്യ);

  part->lastreadstatus = 1; /* Successful read status. */
  തിരികെ res;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mime_subparts_seek(ശൂന്യം *instream, curl_off_t offset, പൂർണ്ണസംഖ്യ whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  പൂർണ്ണസംഖ്യ rc = CURL_SEEKFUNC_OK;

  എങ്കിൽ(whence != SEEK_SET || offset)
    തിരികെ CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  എങ്കിൽ(mime->state.state == MIMESTATE_BEGIN)
    തിരികെ CURL_SEEKFUNC_OK;           /* Already rewound. */

  വേണ്ടി(part = mime->firstpart; part; part = part->nextpart) {
    പൂർണ്ണസംഖ്യ res = mime_part_rewind(part);
    എങ്കിൽ(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  എങ്കിൽ(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ശൂന്യ);

  തിരികെ rc;
}

/* Release part content. */
സ്ഥിതം ശൂന്യം cleanup_part_content(curl_mimepart *part)
{
  എങ്കിൽ(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = ശൂന്യ;
  part->seekfunc = ശൂന്യ;
  part->freefunc = ശൂന്യ;
  part->arg = (ശൂന്യം *)part;           /* Defaults to part itself. */
  part->data = ശൂന്യ;
  part->fp = ശൂന്യ;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned പൂർണ്ണസംഖ്യ)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

സ്ഥിതം ശൂന്യം mime_subparts_free(ശൂന്യം *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  എങ്കിൽ(mime && mime->parent) {
    mime->parent->freefunc = ശൂന്യ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
സ്ഥിതം ശൂന്യം mime_subparts_unbind(ശൂന്യം *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  എങ്കിൽ(mime && mime->parent) {
    mime->parent->freefunc = ശൂന്യ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = ശൂന്യ;
  }
}

ശൂന്യം Curl_mime_cleanpart(curl_mimepart *part)
{
  എങ്കിൽ(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    എങ്കിൽ(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
ശൂന്യം curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  എങ്കിൽ(!mime)
    തിരികെ;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  ആകുമ്പോൾ(mime) {
    part = mime->firstpart;
    എങ്കിൽ(part) {
      mime->firstpart = part->nextpart;
      എങ്കിൽ(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = ശൂന്യ;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    അല്ലെങ്കിൽ {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

സ്ഥിതം bool mime_is_too_deep(സ്ഥിരം curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  എങ്കിൽ(++call_depth > MAX_MIME_LEVELS)
    തിരികെ TRUE;

  എങ്കിൽ(part->kind == MIMEKIND_MULTIPART && part->arg) {
    സ്ഥിരം curl_mime *mime = (സ്ഥിരം curl_mime *)part->arg;
    സ്ഥിരം curl_mimepart *s;

    വേണ്ടി(s = mime->firstpart; s; s = s->nextpart) {
      എങ്കിൽ(mime_is_too_deep(s, call_depth))
        തിരികെ TRUE;
    }
  }

  തിരികെ FALSE;
}

സ്ഥിതം CURLcode mime_duppart(ഘടന Curl_easy *data,
                             curl_mimepart *dst, സ്ഥിരം curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  സ്ഥിരം curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  എങ്കിൽ(++call_depth > MAX_MIME_LEVELS)
    തിരികെ CURLE_TOO_LARGE;

  /* Duplicate content. */
  തിരഞ്ഞെടുപ്പ്(src->kind) {
  അവസ്ഥ MIMEKIND_NONE:
    നിർത്തുക;
  അവസ്ഥ MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    നിർത്തുക;
  അവസ്ഥ MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    എങ്കിൽ(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    നിർത്തുക;
  അവസ്ഥ MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    നിർത്തുക;
  അവസ്ഥ MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    വേണ്ടി(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    നിർത്തുക;
  }

  /* Duplicate headers. */
  എങ്കിൽ(!result && src->userheaders) {
    ഘടന curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    എങ്കിൽ(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    അല്ലെങ്കിൽ {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      എങ്കിൽ(result)
        curl_slist_free_all(hdrs);
    }
  }

  എങ്കിൽ(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  എങ്കിൽ(!result)
    result = curl_mime_name(dst, src->name);
  എങ്കിൽ(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  എങ്കിൽ(result)
    Curl_mime_cleanpart(dst);

  തിരികെ result;
}

CURLcode Curl_mime_duppart(ഘടന Curl_easy *data,
                           curl_mimepart *dst, സ്ഥിരം curl_mimepart *src)
{
  എങ്കിൽ(!dst || !src)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(mime_is_too_deep(src, 0))
    തിരികെ CURLE_TOO_LARGE;

  തിരികെ mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(ശൂന്യം *easy)
{
  curl_mime *mime = curlx_malloc(വലുപ്പം(*mime));

  എങ്കിൽ(mime) {
    mime->parent = ശൂന്യ;
    mime->firstpart = ശൂന്യ;
    mime->lastpart = ശൂന്യ;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    എങ്കിൽ(Curl_rand_alnum(easy,
                       (unsigned അക്ഷരം *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      തിരികെ ശൂന്യ;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ശൂന്യ);
  }

  തിരികെ mime;
}

/* Initialize a mime part. */
ശൂന്യം Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, വലുപ്പം(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, ശൂന്യ);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  എങ്കിൽ(!mime)
    തിരികെ ശൂന്യ;

  part = curlx_malloc(വലുപ്പം(*part));

  എങ്കിൽ(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    എങ്കിൽ(mime->lastpart)
      mime->lastpart->nextpart = part;
    അല്ലെങ്കിൽ
      mime->firstpart = part;

    mime->lastpart = part;
  }

  തിരികെ part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, സ്ഥിരം അക്ഷരം *name)
{
  എങ്കിൽ(!part)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  എങ്കിൽ(name) {
    part->name = curlx_strdup(name);
    എങ്കിൽ(!part->name)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  തിരികെ CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, സ്ഥിരം അക്ഷരം *filename)
{
  എങ്കിൽ(!part)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  എങ്കിൽ(filename) {
    part->filename = curlx_strdup(filename);
    എങ്കിൽ(!part->filename)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  തിരികെ CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, സ്ഥിരം അക്ഷരം *data, size_t datasize)
{
  എങ്കിൽ(!part)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  എങ്കിൽ(data) {
    എങ്കിൽ(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    എങ്കിൽ(!part->data)
      തിരികെ CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  തിരികെ CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, സ്ഥിരം അക്ഷരം *filename)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!part)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  എങ്കിൽ(filename) {
    അക്ഷരം *base;
    curlx_struct_stat sbuf;

    എങ്കിൽ(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    അല്ലെങ്കിൽ {
      part->data = curlx_strdup(filename);
      എങ്കിൽ(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      അല്ലെങ്കിൽ {
        part->datasize = -1;
        എങ്കിൽ(S_ISREG(sbuf.st_mode)) {
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
        എങ്കിൽ(!base)
          result = CURLE_OUT_OF_MEMORY;
        അല്ലെങ്കിൽ {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  തിരികെ result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, സ്ഥിരം അക്ഷരം *mimetype)
{
  എങ്കിൽ(!part)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  എങ്കിൽ(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    എങ്കിൽ(!part->mimetype)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം സ്ഥിരം ഘടന mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, സ്ഥിരം അക്ഷരം *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  സ്ഥിരം ഘടന mime_encoder *mep;

  എങ്കിൽ(!part)
    തിരികെ result;

  part->encoder = ശൂന്യ;

  എങ്കിൽ(!encoding)
    തിരികെ CURLE_OK;    /* Removing current encoder. */

  വേണ്ടി(mep = encoders; mep->name; mep++)
    എങ്കിൽ(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  തിരികെ result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           ഘടന curl_slist *headers, പൂർണ്ണസംഖ്യ take_ownership)
{
  എങ്കിൽ(!part)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(part->flags & MIME_USERHEADERS_OWNER) {
    എങ്കിൽ(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned പൂർണ്ണസംഖ്യ)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  എങ്കിൽ(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  തിരികെ CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, ശൂന്യം *arg)
{
  എങ്കിൽ(!part)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  എങ്കിൽ(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  തിരികെ CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, പൂർണ്ണസംഖ്യ take_ownership)
{
  curl_mime *വേര്;

  എങ്കിൽ(!part)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  എങ്കിൽ(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    തിരികെ CURLE_OK;

  cleanup_part_content(part);

  എങ്കിൽ(subparts) {
    /* Should not have been attached already. */
    എങ്കിൽ(subparts->parent)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    വേര് = part->parent;
    എങ്കിൽ(വേര്) {
      ആകുമ്പോൾ(വേര്->parent && വേര്->parent->parent)
        വേര് = വേര്->parent->parent;
      എങ്കിൽ(subparts == വേര്) {
        /* cannot add as a subpart of itself. */
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    എങ്കിൽ(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      തിരികെ CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  തിരികെ CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  തിരികെ Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(അക്ഷരം *buffer, size_t size, size_t nitems, ശൂന്യം *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (ശൂന്യം)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  ചെയ്യുക {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } ആകുമ്പോൾ(ret == STOP_FILLING);

  തിരികെ ret;
}

/* Rewind mime stream. */
സ്ഥിതം CURLcode mime_rewind(curl_mimepart *part)
{
  തിരികെ mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
സ്ഥിതം size_t slist_size(ഘടന curl_slist *s,
                         size_t overhead, സ്ഥിരം അക്ഷരം *skip, size_t skiplen)
{
  size_t size = 0;

  വേണ്ടി(; s; s = s->next)
    എങ്കിൽ(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  തിരികെ size;
}

/* Get/compute multipart size. */
സ്ഥിതം curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  എങ്കിൽ(!mime)
    തിരികെ 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  വേണ്ടി(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    എങ്കിൽ(sz < 0)
      size = sz;

    എങ്കിൽ(size >= 0)
      size += boundarysize + sz;
  }

  തിരികെ size;
}

/* Get/compute mime size. */
സ്ഥിതം curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  എങ്കിൽ(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  എങ്കിൽ(part->encoder)
    size = part->encoder->sizefunc(part);

  എങ്കിൽ(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, ശൂന്യ, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  തിരികെ size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(ഘടന curl_slist **slp, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  ഘടന curl_slist *hdr = ശൂന്യ;
  അക്ഷരം *s = ശൂന്യ;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  എങ്കിൽ(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    എങ്കിൽ(hdr)
      *slp = hdr;
    അല്ലെങ്കിൽ
      curlx_free(s);
  }

  തിരികെ hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
സ്ഥിതം CURLcode add_content_type(ഘടന curl_slist **slp,
                                 സ്ഥിരം അക്ഷരം *type, സ്ഥിരം അക്ഷരം *boundary)
{
  തിരികെ Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

സ്ഥിരം അക്ഷരം *Curl_mime_contenttype(സ്ഥിരം അക്ഷരം *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  ഘടന ContentType {
    സ്ഥിരം അക്ഷരം *extension;
    സ്ഥിരം അക്ഷരം *type;
  };
  സ്ഥിതം സ്ഥിരം ഘടന ContentType ctts[] = {
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

  എങ്കിൽ(filename) {
    size_t len1 = strlen(filename);
    സ്ഥിരം അക്ഷരം *nameend = filename + len1;
    unsigned പൂർണ്ണസംഖ്യ i;

    വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      എങ്കിൽ(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        തിരികെ ctts[i].type;
    }
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം bool content_type_match(സ്ഥിരം അക്ഷരം *contenttype,
                               സ്ഥിരം അക്ഷരം *target, size_t len)
{
  എങ്കിൽ(contenttype && curl_strnequal(contenttype, target, len))
    തിരഞ്ഞെടുപ്പ്(contenttype[len]) {
    അവസ്ഥ '\0':
    അവസ്ഥ '\t':
    അവസ്ഥ '\r':
    അവസ്ഥ '\n':
    അവസ്ഥ ' ':
    അവസ്ഥ ';':
      തിരികെ TRUE;
    }
  തിരികെ FALSE;
}

സ്ഥിതം CURLcode add_content_disposition(ഘടന Curl_easy *data,
                                        curl_mimepart *part,
                                        സ്ഥിരം അക്ഷരം *disposition,
                                        സ്ഥിരം അക്ഷരം *contenttype,
                                        എണ്ണൽ mimestrategy strategy)
{
  എങ്കിൽ(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  എങ്കിൽ(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = ശൂന്യ;

  എങ്കിൽ(disposition) {
    CURLcode result = CURLE_OK;
    അക്ഷരം *name = ശൂന്യ;
    അക്ഷരം *filename = ശൂന്യ;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    എങ്കിൽ(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

    എങ്കിൽ(part->name) {
      name = escape_string(data, part->name, strategy);
      എങ്കിൽ(!name)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
    എങ്കിൽ(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      എങ്കിൽ(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    എങ്കിൽ(!result)
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
    എങ്കിൽ(result)
      തിരികെ result;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode mime_prepare_headers(ഘടന Curl_easy *data,
                                     curl_mimepart *part,
                                     സ്ഥിരം അക്ഷരം *contenttype,
                                     സ്ഥിരം അക്ഷരം *disposition,
                                     എണ്ണൽ mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *boundary = ശൂന്യ;
  അക്ഷരം *customct;
  സ്ഥിരം അക്ഷരം *cte = ശൂന്യ;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(++call_depth > MAX_MIME_LEVELS)
    തിരികെ CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = ശൂന്യ;

  /* Be sure we will not access old headers later. */
  എങ്കിൽ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, ശൂന്യ);

  /* Check if content type is specified. */
  customct = part->mimetype;
  എങ്കിൽ(!customct)
    customct = തിരയൽ_header(part->userheaders, STRCONST("Content-Type"));
  എങ്കിൽ(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  എങ്കിൽ(!contenttype) {
    തിരഞ്ഞെടുപ്പ്(part->kind) {
    അവസ്ഥ MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      നിർത്തുക;
    അവസ്ഥ MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      എങ്കിൽ(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      എങ്കിൽ(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      contenttype = Curl_mime_contenttype(part->filename);
      നിർത്തുക;
    }
  }

  എങ്കിൽ(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    എങ്കിൽ(mime)
      boundary = mime->boundary;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = ശൂന്യ;

  /* Issue content-disposition header only if not already set by caller. */
  എങ്കിൽ(!തിരയൽ_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* Issue Content-Type header. */
  എങ്കിൽ(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* Content-Transfer-Encoding header. */
  എങ്കിൽ(!തിരയൽ_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    എങ്കിൽ(part->encoder)
      cte = part->encoder->name;
    അല്ലെങ്കിൽ എങ്കിൽ(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    എങ്കിൽ(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      എങ്കിൽ(result)
        തിരികെ result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  എങ്കിൽ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  എങ്കിൽ(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = ശൂന്യ;
    എങ്കിൽ(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    വേണ്ടി(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, ശൂന്യ,
                                    disposition, strategy, call_depth);
      എങ്കിൽ(result)
        തിരികെ result;
    }
  }
  തിരികെ result;
}

CURLcode Curl_mime_prepare_headers(ഘടന Curl_easy *data,
                                   curl_mimepart *part,
                                   സ്ഥിരം അക്ഷരം *contenttype,
                                   സ്ഥിരം അക്ഷരം *disposition,
                                   എണ്ണൽ mimestrategy strategy)
{
  തിരികെ mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
സ്ഥിതം ശൂന്യം mime_unpause(curl_mimepart *part)
{
  എങ്കിൽ(part) {
    എങ്കിൽ(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    എങ്കിൽ(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      എങ്കിൽ(mime) {
        curl_mimepart *subpart;

        വേണ്ടി(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

ഘടന cr_mime_ctx {
  ഘടന Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  ഘടന bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

സ്ഥിതം CURLcode cr_mime_init(ഘടന Curl_easy *data,
                             ഘടന Curl_creader *reader)
{
  ഘടന cr_mime_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cr_mime_close(ഘടന Curl_easy *data,
                          ഘടന Curl_creader *reader)
{
  ഘടന cr_mime_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
സ്ഥിതം CURLcode cr_mime_read(ഘടന Curl_easy *data,
                             ഘടന Curl_creader *reader,
                             അക്ഷരം *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  ഘടന cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  അക്ഷരം tmp[256];

  /* Once we have errored, we will return the same error forever */
  എങ്കിൽ(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (പൂർണ്ണസംഖ്യ)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    തിരികെ ctx->error_result;
  }
  എങ്കിൽ(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    തിരികെ CURLE_OK;
  }
  /* respect length limitations */
  എങ്കിൽ(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    എങ്കിൽ(remain <= 0)
      blen = 0;
    അല്ലെങ്കിൽ എങ്കിൽ(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned അക്ഷരം *)buf, blen, &nread);
    എങ്കിൽ(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      തിരികെ result;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, വലുപ്പം(tmp), ctx->part);
    എങ്കിൽ(nread <= വലുപ്പം(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned അക്ഷരം *)tmp, nread, &n);
      എങ്കിൽ(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        തിരികെ result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      എങ്കിൽ(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        തിരികെ result;
      }
    }
  }
  അല്ലെങ്കിൽ
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  തിരഞ്ഞെടുപ്പ്(nread) {
  അവസ്ഥ 0:
    എങ്കിൽ((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      തിരികെ CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    നിർത്തുക;

  അവസ്ഥ CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    നിർത്തുക;

  അവസ്ഥ CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    നിർത്തുക; /* nothing was read */

  അവസ്ഥ STOP_FILLING:
  അവസ്ഥ READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    എങ്കിൽ(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      തിരികെ CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    എങ്കിൽ(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    നിർത്തുക;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (പൂർണ്ണസംഖ്യ)result, *pnread, *peos);
  തിരികെ result;
}

സ്ഥിതം bool cr_mime_needs_rewind(ഘടന Curl_easy *data,
                                 ഘടന Curl_creader *reader)
{
  ഘടന cr_mime_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരികെ ctx->read_len > 0;
}

സ്ഥിതം curl_off_t cr_mime_total_length(ഘടന Curl_easy *data,
                                       ഘടന Curl_creader *reader)
{
  ഘടന cr_mime_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരികെ ctx->total_len;
}

സ്ഥിതം CURLcode cr_mime_resume_from(ഘടന Curl_easy *data,
                                    ഘടന Curl_creader *reader,
                                    curl_off_t offset)
{
  ഘടന cr_mime_ctx *ctx = reader->ctx;

  എങ്കിൽ(offset > 0) {
    curl_off_t passed = 0;

    ചെയ്യുക {
      അക്ഷരം scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)വലുപ്പം(scratch)) ?
        വലുപ്പം(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      എങ്കിൽ((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        തിരികെ CURLE_READ_ERROR;
      }
    } ആകുമ്പോൾ(passed < offset);

    /* now, decrease the size of the read */
    എങ്കിൽ(ctx->total_len > 0) {
      ctx->total_len -= offset;

      എങ്കിൽ(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        തിരികെ CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cr_mime_cntrl(ഘടന Curl_easy *data,
                              ഘടന Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  ഘടന cr_mime_ctx *ctx = reader->ctx;
  തിരഞ്ഞെടുപ്പ്(opcode) {
  അവസ്ഥ CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    എങ്കിൽ(result)
      failf(data, "Cannot rewind mime/post data");
    തിരികെ result;
  }
  അവസ്ഥ CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    നിർത്തുക;
  അവസ്ഥ CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool cr_mime_is_paused(ഘടന Curl_easy *data,
                              ഘടന Curl_creader *reader)
{
  ഘടന cr_mime_ctx *ctx = reader->ctx;
  (ശൂന്യം)data;
  തിരികെ ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_crtype cr_mime = {
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
  വലുപ്പം(ഘടന cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(ഘടന Curl_easy *data, curl_mimepart *part)
{
  ഘടന Curl_creader *r;
  ഘടന cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  എങ്കിൽ(result)
    തിരികെ result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  എങ്കിൽ(result) {
    Curl_creader_free(data, r);
    തിരികെ result;
  }
  ctx->total_len = mime_size(ctx->part);

  തിരികെ Curl_creader_set(data, r);
}

#അല്ലെങ്കിൽ /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (ശൂന്യം)easy;
  തിരികെ ശൂന്യ;
}

ശൂന്യം curl_mime_free(curl_mime *mime)
{
  (ശൂന്യം)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (ശൂന്യം)mime;
  തിരികെ ശൂന്യ;
}

CURLcode curl_mime_name(curl_mimepart *part, സ്ഥിരം അക്ഷരം *name)
{
  (ശൂന്യം)part;
  (ശൂന്യം)name;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, സ്ഥിരം അക്ഷരം *filename)
{
  (ശൂന്യം)part;
  (ശൂന്യം)filename;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, സ്ഥിരം അക്ഷരം *mimetype)
{
  (ശൂന്യം)part;
  (ശൂന്യം)mimetype;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, സ്ഥിരം അക്ഷരം *encoding)
{
  (ശൂന്യം)part;
  (ശൂന്യം)encoding;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, സ്ഥിരം അക്ഷരം *data, size_t datasize)
{
  (ശൂന്യം)part;
  (ശൂന്യം)data;
  (ശൂന്യം)datasize;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, സ്ഥിരം അക്ഷരം *filename)
{
  (ശൂന്യം)part;
  (ശൂന്യം)filename;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           ശൂന്യം *arg)
{
  (ശൂന്യം)part;
  (ശൂന്യം)datasize;
  (ശൂന്യം)readfunc;
  (ശൂന്യം)seekfunc;
  (ശൂന്യം)freefunc;
  (ശൂന്യം)arg;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (ശൂന്യം)part;
  (ശൂന്യം)subparts;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           ഘടന curl_slist *headers, പൂർണ്ണസംഖ്യ take_ownership)
{
  (ശൂന്യം)part;
  (ശൂന്യം)headers;
  (ശൂന്യം)take_ownership;
  തിരികെ CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(ഘടന curl_slist **slp, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)slp;
  (ശൂന്യം)fmt;
  തിരികെ CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

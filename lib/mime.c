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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

ਬਣਤਰ Curl_easy;

#ਅਨ੍ਤਰ੍ਭਾਵ "mime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/basename.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"

#ਜੇ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "slist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_MIME_ਪੱਧਰ 40 /* avoid deep nesting */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_ERROR   ((size_t)-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STOP_FILLING ((size_t)-2)

ਸਥਿਰ size_t mime_subparts_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, size_t nitems,
                                 ਖਾਲੀ *instream, bool *hasread);
ਸਥਿਰ curl_off_t mime_ਮਾਪ(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QP_OK           1       /* Can be represented by itself. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QP_SP           2       /* Space or tab. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QP_CR           3       /* Carriage return. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QP_LF           4       /* Line-feed. */
ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ qp_class[] = {
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
ਸਥਿਰ ਅਚਲ ਅੱਖਰ aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __VMS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ filesize(name, stat_ਅੰਕੜੇ) stat_ਅੰਕੜੇ.st_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fopen_read                curlx_fopen

#ਨਹੀਂਤਾਂ

#ਅਨ੍ਤਰ੍ਭਾਵ <fabdef.h>
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
curl_off_t Vms_Real_File_ਮਾਪ(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ curlx_struct_stat *stat_buf)
{
  ਅੱਖਰ ਬਫਰ[8192];
  curl_off_t ਗਿਣਤੀ_2;
  ਪੂਰਨ ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  ਜੇ(!file)
    ਵਾਪਸ 0;

  ਗਿਣਤੀ_2 = 0;
  ret_stat = 1;
  ਜਦੋਂ(ret_stat > 0) {
    ret_stat = fread(ਬਫਰ, 1, ਆਕਾਰ(ਬਫਰ), file);
    ਜੇ(ret_stat)
      ਗਿਣਤੀ_2 += ret_stat;
  }
  curlx_fclose(file);

  ਵਾਪਸ ਗਿਣਤੀ_2;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
ਸਥਿਰ curl_off_t Vms_Special_ਮਾਪ(ਅਚਲ ਅੱਖਰ *name,
                                 ਅਚਲ curlx_struct_stat *stat_buf)
{
  ਚੋਣ(stat_buf->st_fab_rfm) {
  ਹਾਲਤ FAB$C_VAR:
  ਹਾਲਤ FAB$C_VFC:
    ਵਾਪਸ Vms_Real_File_ਮਾਪ(name, stat_buf);
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ stat_buf->st_ਮਾਪ;
  }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ filesize(name, stat_ਅੰਕੜੇ) Vms_Special_ਮਾਪ(name, &stat_ਅੰਕੜੇ)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
ਸਥਿਰ FILE *vmsfopenread(ਅਚਲ ਅੱਖਰ *file, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ)
{
  curlx_struct_stat statbuf;
  ਪੂਰਨ res = curlx_stat(file, &statbuf);

  ਜੇ(res != -1) {
    ਚੋਣ(statbuf.st_fab_rfm) {
    ਹਾਲਤ FAB$C_VAR:
    ਹਾਲਤ FAB$C_VFC:
    ਹਾਲਤ FAB$C_STMCR:
      ਵਾਪਸ curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      ਰੋਕੋ;
    ਮੂਲ:
      ਵਾਪਸ curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fopen_read vmsfopenread
#ਜੇ_ਅੰਤ /* !__VMS */

/* Set readback state. */
ਸਥਿਰ ਖਾਲੀ mimesetstate(ਬਣਤਰ mime_state *state,
                         ਗਿਣਤੀ mimestate tok, ਖਾਲੀ *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->ਵਿੱਥ = 0;
}

/* Escape header string into allocated memory. */
ਸਥਿਰ ਅੱਖਰ *escape_string(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਅਚਲ ਅੱਖਰ *src, ਗਿਣਤੀ mimestrategy strategy)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ dynbuf db;
  ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ;
  ਅਚਲ ਅੱਖਰ * ਅਚਲ *p;
  /* replace first character by rest of string. */
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    ਨਿਰਾ
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    ਨਿਰਾ
  };

  ਸਾਰਣੀ = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  ਜੇ(strategy == MIMESTRATEGY_MAIL || (ਅੰਕੜੇ && ਅੰਕੜੇ->ਸੈੱਟ.mime_formescape))
    ਸਾਰਣੀ = mimetable;

  curlx_dyn_init(&db, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

  ਲਈ(ਨਤੀਜਾ = curlx_dyn_addn(&db, STRCONST("")); !ਨਤੀਜਾ && *src; src++) {
    ਲਈ(p = ਸਾਰਣੀ; *p && **p != *src; p++)
      ;

    ਜੇ(*p)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&db, *p + 1);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = curlx_dyn_addn(&db, src, 1);
  }

  ਵਾਪਸ curlx_dyn_ptr(&db);
}

/* Check if header matches. */
ਸਥਿਰ ਅੱਖਰ *ਮੇਲ_ਸਿਰਲੇਖ(ਬਣਤਰ curl_slist *hdr, ਅਚਲ ਅੱਖਰ *lbl, size_t len)
{
  ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;

  ਜੇ(curl_strnequal(hdr->ਅੰਕੜੇ, lbl, len) && hdr->ਅੰਕੜੇ[len] == ':')
    ਲਈ(ਮੁੱਲ = hdr->ਅੰਕੜੇ + len + 1; *ਮੁੱਲ == ' '; ਮੁੱਲ++)
      ;
  ਵਾਪਸ ਮੁੱਲ;
}

/* Get a header from an slist. */
ਸਥਿਰ ਅੱਖਰ *ਖੋਜ_ਸਿਰਲੇਖ(ਬਣਤਰ curl_slist *hdrlist,
                           ਅਚਲ ਅੱਖਰ *hdr, size_t len)
{
  ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;

  ਲਈ(; !ਮੁੱਲ && hdrlist; hdrlist = hdrlist->ਅਗਲਾ)
    ਮੁੱਲ = ਮੇਲ_ਸਿਰਲੇਖ(hdrlist, hdr, len);

  ਵਾਪਸ ਮੁੱਲ;
}

ਸਥਿਰ ਅੱਖਰ *strippath(ਅਚਲ ਅੱਖਰ *fullfile)
{
  ਅੱਖਰ *filename;
  ਅੱਖਰ *ਅਧਾਰ;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  ਜੇ(!filename)
    ਵਾਪਸ ਨਿਰਾ;
  ਅਧਾਰ = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  ਵਾਪਸ ਅਧਾਰ; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
ਸਥਿਰ ਖਾਲੀ cleanup_encoder_state(ਬਣਤਰ mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
ਸਥਿਰ size_t encoder_nop_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, bool ateof,
                               ਬਣਤਰ curl_mimepart *part)
{
  ਬਣਤਰ mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (ਖਾਲੀ)ateof;

  ਜੇ(!ਮਾਪ)
    ਵਾਪਸ STOP_FILLING;

  ਜੇ(ਮਾਪ > insize)
    ਮਾਪ = insize;

  ਜੇ(ਮਾਪ)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਫਰ, st->buf + st->bufbeg, ਮਾਪ);

  st->bufbeg += ਮਾਪ;
  ਵਾਪਸ ਮਾਪ;
}

ਸਥਿਰ curl_off_t encoder_nop_ਮਾਪ(curl_mimepart *part)
{
  ਵਾਪਸ part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
ਸਥਿਰ size_t encoder_7bit_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, bool ateof,
                                curl_mimepart *part)
{
  ਬਣਤਰ mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (ਖਾਲੀ)ateof;

  ਜੇ(!ਮਾਪ)
    ਵਾਪਸ STOP_FILLING;

  ਜੇ(ਮਾਪ > cursize)
    ਮਾਪ = cursize;

  ਲਈ(cursize = 0; cursize < ਮਾਪ; cursize++) {
    *ਬਫਰ = st->buf[st->bufbeg];
    ਜੇ(*ਬਫਰ++ & 0x80)
      ਵਾਪਸ cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  ਵਾਪਸ cursize;
}

/* Base64 content encoder. */
ਸਥਿਰ size_t encoder_base64_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, bool ateof,
                                  curl_mimepart *part)
{
  ਬਣਤਰ mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  ਪੂਰਨ i;
  ਅੱਖਰ *ptr = ਬਫਰ;

  ਜਦੋਂ(st->bufbeg < st->bufend) {
    /* Line full ? */
    ਜੇ(st->pos > MAX_ENCODED_ਸਤਰ_ਲੰਮਾਈ - 4) {
      /* Yes, we need 2 characters for CRLF. */
      ਜੇ(ਮਾਪ < 2) {
        ਜੇ(!cursize)
          ਵਾਪਸ STOP_FILLING;
        ਰੋਕੋ;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      ਮਾਪ -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    ਜੇ(ਮਾਪ < 4) {
      ਜੇ(!cursize)
        ਵਾਪਸ STOP_FILLING;
      ਰੋਕੋ;
    }
    ਜੇ(st->bufend - st->bufbeg < 3)
      ਰੋਕੋ;

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
    ਮਾਪ -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  ਜੇ(ateof) {
    ਜੇ(ਮਾਪ < 4) {
      ਜੇ(!cursize)
        ਵਾਪਸ STOP_FILLING;
    }
    ਨਹੀਂਤਾਂ {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      ਜੇ(st->bufend != st->bufbeg) {

        ਜੇ(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        ਜੇ(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  ਵਾਪਸ cursize;
}

/* The maximum input size that does not cause an overflow. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BASE64_MAX_ਨਿਵੇਸ਼_ਮਾਪ                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_ਸਤਰ_ਲੰਮਾਈ + 2)) *                  \
    MAX_ENCODED_ਸਤਰ_ਲੰਮਾਈ / 4) * 3 - 3)

ਸਥਿਰ curl_off_t encoder_base64_ਮਾਪ(curl_mimepart *part)
{
  curl_off_t ਮਾਪ = part->datasize;

  ਜੇ(ਮਾਪ <= 0)
    ਵਾਪਸ ਮਾਪ;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  ਜੇ(ਮਾਪ > BASE64_MAX_ਨਿਵੇਸ਼_ਮਾਪ)
    ਵਾਪਸ -1;

  /* Compute base64 character count. */
  ਮਾਪ = 4 * (1 + ((ਮਾਪ - 1) / 3));

  /* Effective character count must include CRLFs. */
  ਵਾਪਸ ਮਾਪ + (2 * ((ਮਾਪ - 1) / MAX_ENCODED_ਸਤਰ_ਲੰਮਾਈ));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
ਸਥਿਰ ਪੂਰਨ qp_lookahead_eol(ਬਣਤਰ mime_encoder_state *st, ਪੂਰਨ ateof, size_t n)
{
  n += st->bufbeg;
  ਜੇ(n >= st->bufend && ateof)
    ਵਾਪਸ 1;
  ਜੇ(n + 2 > st->bufend)
    ਵਾਪਸ ateof ? 0 : -1;
  ਜੇ(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    ਵਾਪਸ 1;
  ਵਾਪਸ 0;
}

/* Quoted-printable encoder. */
ਸਥਿਰ size_t encoder_qp_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, bool ateof,
                              curl_mimepart *part)
{
  ਬਣਤਰ mime_encoder_state *st = &part->encstate;
  ਅੱਖਰ *ptr = ਬਫਰ;
  size_t cursize = 0;
  ਪੂਰਨ softlinebreak;
  ਅੱਖਰ buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  ਜਦੋਂ(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    ਪੂਰਨ i = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)st->buf[st->bufbeg];
    buf[0] = (ਅੱਖਰ)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    ਚੋਣ(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    ਹਾਲਤ QP_OK:          /* Not a special character. */
      ਰੋਕੋ;
    ਹਾਲਤ QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      ਚੋਣ(qp_lookahead_eol(st, ateof, 1)) {
      ਹਾਲਤ -1:          /* More input data needed. */
        ਵਾਪਸ cursize;
      ਹਾਲਤ 0:           /* No encoding needed. */
        ਰੋਕੋ;
      ਮੂਲ:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ਰੋਕੋ;
      }
      ਰੋਕੋ;
    ਹਾਲਤ QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      ਚੋਣ(qp_lookahead_eol(st, ateof, 0)) {
      ਹਾਲਤ -1:          /* Need more data. */
        ਵਾਪਸ cursize;
      ਹਾਲਤ 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        ਰੋਕੋ;
      ਮੂਲ:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ਰੋਕੋ;
      }
      ਰੋਕੋ;
    ਮੂਲ:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      ਰੋਕੋ;
    }

    /* Be sure the encoded character fits within maximum line length. */
    ਜੇ(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_ਸਤਰ_ਲੰਮਾਈ;
      ਜੇ(!softlinebreak && st->pos + len == MAX_ENCODED_ਸਤਰ_ਲੰਮਾਈ) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        ਚੋਣ(qp_lookahead_eol(st, ateof, consumed)) {
        ਹਾਲਤ -1:        /* Need more data. */
          ਵਾਪਸ cursize;
        ਹਾਲਤ 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          ਰੋਕੋ;
        }
      }
      ਜੇ(softlinebreak) {
        curlx_strcopy(buf, ਆਕਾਰ(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    ਜੇ(len > ਮਾਪ) {
      ਜੇ(!cursize)
        ਵਾਪਸ STOP_FILLING;
      ਰੋਕੋ;
    }

    /* Append to output buffer. */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr, buf, len);
    cursize += len;
    ptr += len;
    ਮਾਪ -= len;
    st->pos += len;
    ਜੇ(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  ਵਾਪਸ cursize;
}

ਸਥਿਰ curl_off_t encoder_qp_ਮਾਪ(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  ਵਾਪਸ part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
ਸਥਿਰ size_t mime_mem_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, size_t nitems,
                            ਖਾਲੀ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.ਵਿੱਥ);
  (ਖਾਲੀ)ਮਾਪ;  /* Always 1 */

  ਜੇ(!nitems)
    ਵਾਪਸ STOP_FILLING;

  ਜੇ(sz > nitems)
    sz = nitems;

  ਜੇ(sz)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਫਰ, part->ਅੰਕੜੇ + curlx_sotouz(part->state.ਵਿੱਥ), sz);

  ਵਾਪਸ sz;
}

ਸਥਿਰ ਪੂਰਨ mime_mem_seek(ਖਾਲੀ *instream, curl_off_t ਵਿੱਥ, ਪੂਰਨ whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ਚੋਣ(whence) {
  ਹਾਲਤ SEEK_CUR:
    ਵਿੱਥ += part->state.ਵਿੱਥ;
    ਰੋਕੋ;
  ਹਾਲਤ SEEK_END:
    ਵਿੱਥ += part->datasize;
    ਰੋਕੋ;
  }

  ਜੇ(ਵਿੱਥ < 0 || ਵਿੱਥ > part->datasize)
    ਵਾਪਸ CURL_SEEKFUNC_FAIL;

  part->state.ਵਿੱਥ = ਵਿੱਥ;
  ਵਾਪਸ CURL_SEEKFUNC_OK;
}

ਸਥਿਰ ਖਾਲੀ mime_mem_free(ਖਾਲੀ *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->ਅੰਕੜੇ);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
ਸਥਿਰ bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  ਜੇ(part->fp)
    ਵਾਪਸ FALSE;
  part->fp = fopen_read(part->ਅੰਕੜੇ, "rb");
  ਵਾਪਸ part->fp ? FALSE : TRUE;
}

ਸਥਿਰ size_t mime_file_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, size_t nitems,
                             ਖਾਲੀ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ਜੇ(!nitems)
    ਵਾਪਸ STOP_FILLING;

  ਜੇ(mime_open_file(part))
    ਵਾਪਸ READ_ERROR;

  ਵਾਪਸ fread(ਬਫਰ, ਮਾਪ, nitems, part->fp);
}

ਸਥਿਰ ਪੂਰਨ mime_file_seek(ਖਾਲੀ *instream, curl_off_t ਵਿੱਥ, ਪੂਰਨ whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ਜੇ(whence == SEEK_SET && !ਵਿੱਥ && !part->fp)
    ਵਾਪਸ CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  ਜੇ(mime_open_file(part))
    ਵਾਪਸ CURL_SEEKFUNC_FAIL;

  ਵਾਪਸ curlx_fseek(part->fp, ਵਿੱਥ, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

ਸਥਿਰ ਖਾਲੀ mime_file_free(ਖਾਲੀ *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  ਜੇ(part->fp) {
    curlx_fclose(part->fp);
    part->fp = ਨਿਰਾ;
  }
  curlx_safefree(part->ਅੰਕੜੇ);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
ਸਥਿਰ size_t readback_ਬਾਈਟਾਂ(ਬਣਤਰ mime_state *state,
                             ਅੱਖਰ *ਬਫਰ, size_t bufsize,
                             ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t numbytes,
                             ਅਚਲ ਅੱਖਰ *trail, size_t traillen)
{
  size_t sz;
  size_t ਵਿੱਥ = curlx_sotouz(state->ਵਿੱਥ);

  ਜੇ(numbytes > ਵਿੱਥ) {
    sz = numbytes - ਵਿੱਥ;
    ਬਾਈਟਾਂ += ਵਿੱਥ;
  }
  ਨਹੀਂਤਾਂ {
    sz = ਵਿੱਥ - numbytes;
    ਜੇ(sz >= traillen)
      ਵਾਪਸ 0;
    ਬਾਈਟਾਂ = trail + sz;
    sz = traillen - sz;
  }

  ਜੇ(sz > bufsize)
    sz = bufsize;

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਫਰ, ਬਾਈਟਾਂ, sz);
  state->ਵਿੱਥ += sz;
  ਵਾਪਸ sz;
}

/* Read a non-encoded part content. */

ਸਥਿਰ size_t read_part_content(curl_mimepart *part, ਅੱਖਰ *ਬਫਰ,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  ਚੋਣ(part->lastreadstatus) {
  ਹਾਲਤ 0:
  ਹਾਲਤ CURL_READFUNC_ABORT:
  ਹਾਲਤ CURL_READFUNC_PAUSE:
  ਹਾਲਤ READ_ERROR:
    ਵਾਪਸ part->lastreadstatus;
  ਮੂਲ:
    ਰੋਕੋ;
  }

  /* If we can determine we are at end of part data, spare a read. */
  ਜੇ(part->datasize != (curl_off_t)-1 &&
     part->state.ਵਿੱਥ >= part->datasize) {
    /* sz is already zero. */
  }
  ਨਹੀਂਤਾਂ {
    ਚੋਣ(part->ਕਿਸਮ) {
    ਹਾਲਤ MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(ਬਫਰ, 1, bufsize, part->arg, hasread);
      ਰੋਕੋ;
    ਹਾਲਤ MIMEKIND_FILE:
      ਜੇ(part->fp && feof(part->fp))
        ਰੋਕੋ;  /* At EOF. */
      FALLTHROUGH();
    ਮੂਲ:
      ਜੇ(part->readfunc) {
        ਜੇ(!(part->flags & MIME_FAST_READ)) {
          ਜੇ(*hasread)
            ਵਾਪਸ STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(ਬਫਰ, 1, bufsize, part->arg);
      }
      ਰੋਕੋ;
    }
  }

  ਚੋਣ(sz) {
  ਹਾਲਤ STOP_FILLING:
    ਰੋਕੋ;
  ਹਾਲਤ 0:
  ਹਾਲਤ CURL_READFUNC_ABORT:
  ਹਾਲਤ CURL_READFUNC_PAUSE:
  ਹਾਲਤ READ_ERROR:
    part->lastreadstatus = sz;
    ਰੋਕੋ;
  ਮੂਲ:
    part->state.ਵਿੱਥ += sz;
    part->lastreadstatus = sz;
    ਰੋਕੋ;
  }

  ਵਾਪਸ sz;
}

/* Read and encode part content. */
ਸਥਿਰ size_t read_encoded_part_content(curl_mimepart *part, ਅੱਖਰ *ਬਫਰ,
                                        size_t bufsize, bool *hasread)
{
  ਬਣਤਰ mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  ਲਈ(;;) {
    ਜੇ(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(ਬਫਰ, bufsize, ateof, part);
      ਚੋਣ(sz) {
      ਹਾਲਤ 0:
        ਜੇ(ateof)
          ਵਾਪਸ cursize;
        ਰੋਕੋ;
      ਹਾਲਤ READ_ERROR:
      ਹਾਲਤ STOP_FILLING:
        ਵਾਪਸ cursize ? cursize : sz;
      ਮੂਲ:
        cursize += sz;
        ਬਫਰ += sz;
        bufsize -= sz;
        ਜਾਰੀ;
      }
    }

    /* We need more data in input buffer. */
    ਜੇ(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      ਜੇ(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    ਜੇ(st->bufend >= ਆਕਾਰ(st->buf))
      ਵਾਪਸ cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           ਆਕਾਰ(st->buf) - st->bufend, hasread);
    ਚੋਣ(sz) {
    ਹਾਲਤ 0:
      ateof = TRUE;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_READFUNC_ABORT:
    ਹਾਲਤ CURL_READFUNC_PAUSE:
    ਹਾਲਤ READ_ERROR:
    ਹਾਲਤ STOP_FILLING:
      ਵਾਪਸ cursize ? cursize : sz;
    ਮੂਲ:
      st->bufend += sz;
      ਰੋਕੋ;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
ਸਥਿਰ size_t readback_part(curl_mimepart *part,
                            ਅੱਖਰ *ਬਫਰ, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  ਜਦੋਂ(bufsize) {
    size_t sz = 0;
    ਬਣਤਰ curl_slist *hdr = (ਬਣਤਰ curl_slist *)part->state.ptr;
    ਚੋਣ(part->state.state) {
    ਹਾਲਤ MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_USERHEADERS:
      ਜੇ(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, ਨਿਰਾ);
        ਰੋਕੋ;
      }
      ਜੇ(ਮੇਲ_ਸਿਰਲੇਖ(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->ਅਗਲਾ);
        ਰੋਕੋ;
      }
      FALLTHROUGH();
    ਹਾਲਤ MIMESTATE_CURLHEADERS:
      ਜੇ(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      ਨਹੀਂਤਾਂ {
        sz = readback_ਬਾਈਟਾਂ(&part->state, ਬਫਰ, bufsize,
                            hdr->ਅੰਕੜੇ, ਲੜੀ_ਲੰਬਾਈ(hdr->ਅੰਕੜੇ), STRCONST("\r\n"));
        ਜੇ(!sz)
          mimesetstate(&part->state, part->state.state, hdr->ਅਗਲਾ);
      }
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_EOH:
      sz = readback_ਬਾਈਟਾਂ(&part->state, ਬਫਰ, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      ਜੇ(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, ਨਿਰਾ);
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, ਨਿਰਾ);
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_CONTENT:
      ਜੇ(part->encoder)
        sz = read_encoded_part_content(part, ਬਫਰ, bufsize, hasread);
      ਨਹੀਂਤਾਂ
        sz = read_part_content(part, ਬਫਰ, bufsize, hasread);
      ਚੋਣ(sz) {
      ਹਾਲਤ 0:
        mimesetstate(&part->state, MIMESTATE_ਸਿਰਾ, ਨਿਰਾ);
        /* Try sparing open file descriptors. */
        ਜੇ(part->ਕਿਸਮ == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = ਨਿਰਾ;
        }
        FALLTHROUGH();
      ਹਾਲਤ CURL_READFUNC_ABORT:
      ਹਾਲਤ CURL_READFUNC_PAUSE:
      ਹਾਲਤ READ_ERROR:
      ਹਾਲਤ STOP_FILLING:
        ਵਾਪਸ cursize ? cursize : sz;
      }
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_ਸਿਰਾ:
      ਵਾਪਸ cursize;
    ਮੂਲ:
      ਰੋਕੋ;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    ਬਫਰ += sz;
    bufsize -= sz;
  }

  ਵਾਪਸ cursize;
}

/* Readback from mime. Warning: not a read callback function. */
ਸਥਿਰ size_t mime_subparts_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, size_t nitems,
                                 ਖਾਲੀ *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (ਖਾਲੀ)ਮਾਪ;  /* Always 1 */

  ਜਦੋਂ(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    ਚੋਣ(mime->state.state) {
    ਹਾਲਤ MIMESTATE_BEGIN:
    ਹਾਲਤ MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.ਵਿੱਥ += 2;
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_BOUNDARY1:
      sz = readback_ਬਾਈਟਾਂ(&mime->state, ਬਫਰ, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      ਜੇ(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_BOUNDARY2:
      ਜੇ(part)
        sz = readback_ਬਾਈਟਾਂ(&mime->state, ਬਫਰ, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      ਨਹੀਂਤਾਂ
        sz = readback_ਬਾਈਟਾਂ(&mime->state, ਬਫਰ, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      ਜੇ(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_CONTENT:
      ਜੇ(!part) {
        mimesetstate(&mime->state, MIMESTATE_ਸਿਰਾ, ਨਿਰਾ);
        ਰੋਕੋ;
      }
      sz = readback_part(part, ਬਫਰ, nitems, hasread);
      ਚੋਣ(sz) {
      ਹਾਲਤ CURL_READFUNC_ABORT:
      ਹਾਲਤ CURL_READFUNC_PAUSE:
      ਹਾਲਤ READ_ERROR:
      ਹਾਲਤ STOP_FILLING:
        ਵਾਪਸ cursize ? cursize : sz;
      ਹਾਲਤ 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        ਰੋਕੋ;
      }
      ਰੋਕੋ;
    ਹਾਲਤ MIMESTATE_ਸਿਰਾ:
      ਵਾਪਸ cursize;
    ਮੂਲ:
      ਰੋਕੋ;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    ਬਫਰ += sz;
    nitems -= sz;
  }

  ਵਾਪਸ cursize;
}

ਸਥਿਰ ਪੂਰਨ mime_part_rewind(curl_mimepart *part)
{
  ਪੂਰਨ res = CURL_SEEKFUNC_OK;
  ਗਿਣਤੀ mimestate targetstate = MIMESTATE_BEGIN;

  ਜੇ(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  ਜੇ(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    ਜੇ(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      ਚੋਣ(res) {
      ਹਾਲਤ CURL_SEEKFUNC_OK:
      ਹਾਲਤ CURL_SEEKFUNC_FAIL:
      ਹਾਲਤ CURL_SEEKFUNC_CANTSEEK:
        ਰੋਕੋ;
      ਹਾਲਤ -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        ਰੋਕੋ;
      ਮੂਲ:
        res = CURL_SEEKFUNC_FAIL;
        ਰੋਕੋ;
      }
    }
  }

  ਜੇ(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, ਨਿਰਾ);

  part->lastreadstatus = 1; /* Successful read status. */
  ਵਾਪਸ res;
}

ਸਥਿਰ ਪੂਰਨ mime_subparts_seek(ਖਾਲੀ *instream, curl_off_t ਵਿੱਥ, ਪੂਰਨ whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  ਪੂਰਨ rc = CURL_SEEKFUNC_OK;

  ਜੇ(whence != SEEK_SET || ਵਿੱਥ)
    ਵਾਪਸ CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  ਜੇ(mime->state.state == MIMESTATE_BEGIN)
    ਵਾਪਸ CURL_SEEKFUNC_OK;           /* Already rewound. */

  ਲਈ(part = mime->firstpart; part; part = part->nextpart) {
    ਪੂਰਨ res = mime_part_rewind(part);
    ਜੇ(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  ਜੇ(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ਨਿਰਾ);

  ਵਾਪਸ rc;
}

/* Release part content. */
ਸਥਿਰ ਖਾਲੀ cleanup_part_content(curl_mimepart *part)
{
  ਜੇ(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = ਨਿਰਾ;
  part->seekfunc = ਨਿਰਾ;
  part->freefunc = ਨਿਰਾ;
  part->arg = (ਖਾਲੀ *)part;           /* Defaults to part itself. */
  part->ਅੰਕੜੇ = ਨਿਰਾ;
  part->fp = ਨਿਰਾ;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->ਕਿਸਮ = MIMEKIND_NONE;
  part->flags &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

ਸਥਿਰ ਖਾਲੀ mime_subparts_free(ਖਾਲੀ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ਜੇ(mime && mime->ਮਾਪਾ) {
    mime->ਮਾਪਾ->freefunc = ਨਿਰਾ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->ਮਾਪਾ);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
ਸਥਿਰ ਖਾਲੀ mime_subparts_unbind(ਖਾਲੀ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ਜੇ(mime && mime->ਮਾਪਾ) {
    mime->ਮਾਪਾ->freefunc = ਨਿਰਾ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->ਮਾਪਾ);  /* Avoid dangling pointer in part. */
    mime->ਮਾਪਾ = ਨਿਰਾ;
  }
}

ਖਾਲੀ Curl_mime_cleanpart(curl_mimepart *part)
{
  ਜੇ(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    ਜੇ(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
ਖਾਲੀ curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  ਜੇ(!mime)
    ਵਾਪਸ;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  ਜਦੋਂ(mime) {
    part = mime->firstpart;
    ਜੇ(part) {
      mime->firstpart = part->nextpart;
      ਜੇ(part->ਕਿਸਮ == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = ਨਿਰਾ;
        cleanup_part_content(part);
        subparts->ਮਾਪਾ = mime->ਮਾਪਾ;
        mime->ਮਾਪਾ = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    ਨਹੀਂਤਾਂ {
      curl_mime *ਮਾਪਾ = (curl_mime *)mime->ਮਾਪਾ;
      curlx_free(mime);
      mime = ਮਾਪਾ;
    }
  }
}

ਸਥਿਰ bool mime_is_too_deep(ਅਚਲ curl_mimepart *part, size_t call_ਡੂੰਘਾਈ)
{
  DEBUGASSERT(part);

  ਜੇ(++call_ਡੂੰਘਾਈ > MAX_MIME_ਪੱਧਰ)
    ਵਾਪਸ TRUE;

  ਜੇ(part->ਕਿਸਮ == MIMEKIND_MULTIPART && part->arg) {
    ਅਚਲ curl_mime *mime = (ਅਚਲ curl_mime *)part->arg;
    ਅਚਲ curl_mimepart *s;

    ਲਈ(s = mime->firstpart; s; s = s->nextpart) {
      ਜੇ(mime_is_too_deep(s, call_ਡੂੰਘਾਈ))
        ਵਾਪਸ TRUE;
    }
  }

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode mime_duppart(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             curl_mimepart *dst, ਅਚਲ curl_mimepart *src,
                             size_t call_ਡੂੰਘਾਈ)
{
  curl_mime *mime;
  curl_mimepart *d;
  ਅਚਲ curl_mimepart *s;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  ਜੇ(++call_ਡੂੰਘਾਈ > MAX_MIME_ਪੱਧਰ)
    ਵਾਪਸ CURLE_TOO_LARGE;

  /* Duplicate content. */
  ਚੋਣ(src->ਕਿਸਮ) {
  ਹਾਲਤ MIMEKIND_NONE:
    ਰੋਕੋ;
  ਹਾਲਤ MIMEKIND_ਅੰਕੜੇ:
    ਨਤੀਜਾ = curl_mime_ਅੰਕੜੇ(dst, src->ਅੰਕੜੇ, (size_t)src->datasize);
    ਰੋਕੋ;
  ਹਾਲਤ MIMEKIND_FILE:
    ਨਤੀਜਾ = curl_mime_filedata(dst, src->ਅੰਕੜੇ);
    /* Do not abort duplication if file is not readable. */
    ਜੇ(ਨਤੀਜਾ == CURLE_READ_ERROR)
      ਨਤੀਜਾ = CURLE_OK;
    ਰੋਕੋ;
  ਹਾਲਤ MIMEKIND_CALLBACK:
    ਨਤੀਜਾ = curl_mime_ਅੰਕੜੇ_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    ਰੋਕੋ;
  ਹਾਲਤ MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(ਅੰਕੜੇ);
    ਨਤੀਜਾ = mime ? curl_mime_subparts(dst, mime) : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    /* Duplicate subparts. */
    ਲਈ(s = ((curl_mime *)src->arg)->firstpart; !ਨਤੀਜਾ && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      ਨਤੀਜਾ = d ? mime_duppart(ਅੰਕੜੇ, d, s, call_ਡੂੰਘਾਈ) : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਰੋਕੋ;
  ਮੂਲ:  /* Invalid kind: should not occur. */
    DEBUGF(infof(ਅੰਕੜੇ, "invalid MIMEKIND* attempt"));
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;  /* Internal error? */
    ਰੋਕੋ;
  }

  /* Duplicate headers. */
  ਜੇ(!ਨਤੀਜਾ && src->userheaders) {
    ਬਣਤਰ curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    ਜੇ(!hdrs)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      ਨਤੀਜਾ = curl_mime_ਸਿਰਲੇਖਾਂ(dst, hdrs, TRUE);
      ਜੇ(ਨਤੀਜਾ)
        curl_slist_free_all(hdrs);
    }
  }

  ਜੇ(!ਨਤੀਜਾ) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    ਨਤੀਜਾ = curl_mime_type(dst, src->mimetype);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curl_mime_name(dst, src->name);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  ਜੇ(ਨਤੀਜਾ)
    Curl_mime_cleanpart(dst);

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_mime_duppart(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           curl_mimepart *dst, ਅਚਲ curl_mimepart *src)
{
  ਜੇ(!dst || !src)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(mime_is_too_deep(src, 0))
    ਵਾਪਸ CURLE_TOO_LARGE;

  ਵਾਪਸ mime_duppart(ਅੰਕੜੇ, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(ਖਾਲੀ *easy)
{
  curl_mime *mime = curlx_malloc(ਆਕਾਰ(*mime));

  ਜੇ(mime) {
    mime->ਮਾਪਾ = ਨਿਰਾ;
    mime->firstpart = ਨਿਰਾ;
    mime->lastpart = ਨਿਰਾ;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    ਜੇ(Curl_rand_alnum(easy,
                       (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      ਵਾਪਸ ਨਿਰਾ;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ਨਿਰਾ);
  }

  ਵਾਪਸ mime;
}

/* Initialize a mime part. */
ਖਾਲੀ Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, ਆਕਾਰ(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, ਨਿਰਾ);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  ਜੇ(!mime)
    ਵਾਪਸ ਨਿਰਾ;

  part = curlx_malloc(ਆਕਾਰ(*part));

  ਜੇ(part) {
    Curl_mime_initpart(part);
    part->ਮਾਪਾ = mime;

    ਜੇ(mime->lastpart)
      mime->lastpart->nextpart = part;
    ਨਹੀਂਤਾਂ
      mime->firstpart = part;

    mime->lastpart = part;
  }

  ਵਾਪਸ part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *name)
{
  ਜੇ(!part)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  curlx_safefree(part->name);

  ਜੇ(name) {
    part->name = curlx_strdup(name);
    ਜੇ(!part->name)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਵਾਪਸ CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *filename)
{
  ਜੇ(!part)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  curlx_safefree(part->filename);

  ਜੇ(filename) {
    part->filename = curlx_strdup(filename);
    ਜੇ(!part->filename)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਵਾਪਸ CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_ਅੰਕੜੇ(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, size_t datasize)
{
  ਜੇ(!part)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  cleanup_part_content(part);

  ਜੇ(ਅੰਕੜੇ) {
    ਜੇ(datasize == CURL_ZERO_TERMINATED)
      datasize = ਲੜੀ_ਲੰਬਾਈ(ਅੰਕੜੇ);

    part->ਅੰਕੜੇ = curlx_memdup0(ਅੰਕੜੇ, datasize);
    ਜੇ(!part->ਅੰਕੜੇ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->ਕਿਸਮ = MIMEKIND_ਅੰਕੜੇ;
  }

  ਵਾਪਸ CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *filename)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!part)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  cleanup_part_content(part);

  ਜੇ(filename) {
    ਅੱਖਰ *ਅਧਾਰ;
    curlx_struct_stat sbuf;

    ਜੇ(curlx_stat(filename, &sbuf))
      ਨਤੀਜਾ = CURLE_READ_ERROR;
    ਨਹੀਂਤਾਂ {
      part->ਅੰਕੜੇ = curlx_strdup(filename);
      ਜੇ(!part->ਅੰਕੜੇ)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਨਹੀਂਤਾਂ {
        part->datasize = -1;
        ਜੇ(S_ISREG(sbuf.st_ਬਹੁਲਕ)) {
          part->datasize = filesize(filename, sbuf);
          part->seekfunc = mime_file_seek;
        }

        part->readfunc = mime_file_read;
        part->freefunc = mime_file_free;
        part->ਕਿਸਮ = MIMEKIND_FILE;

        /* As a side effect, set the filename to the current file's base name.
           It is possible to withdraw this by explicitly calling
           curl_mime_filename() with a NULL filename argument after the current
           call. */
        ਅਧਾਰ = strippath(filename);
        ਜੇ(!ਅਧਾਰ)
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਨਹੀਂਤਾਂ {
          ਨਤੀਜਾ = curl_mime_filename(part, ਅਧਾਰ);
          curlx_free(ਅਧਾਰ);
        }
      }
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *mimetype)
{
  ਜੇ(!part)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  curlx_safefree(part->mimetype);

  ਜੇ(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    ਜੇ(!part->mimetype)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_ਮਾਪ },
  { "8bit", encoder_nop_read, encoder_nop_ਮਾਪ },
  { "7bit", encoder_7bit_read, encoder_nop_ਮਾਪ },
  { "base64", encoder_base64_read, encoder_base64_ਮਾਪ },
  { "quoted-printable", encoder_qp_read, encoder_qp_ਮਾਪ },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *encoding)
{
  CURLcode ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਅਚਲ ਬਣਤਰ mime_encoder *mep;

  ਜੇ(!part)
    ਵਾਪਸ ਨਤੀਜਾ;

  part->encoder = ਨਿਰਾ;

  ਜੇ(!encoding)
    ਵਾਪਸ CURLE_OK;    /* Removing current encoder. */

  ਲਈ(mep = encoders; mep->name; mep++)
    ਜੇ(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      ਨਤੀਜਾ = CURLE_OK;
    }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Set mime part headers. */
CURLcode curl_mime_ਸਿਰਲੇਖਾਂ(curl_mimepart *part,
                           ਬਣਤਰ curl_slist *ਸਿਰਲੇਖਾਂ, ਪੂਰਨ take_ownership)
{
  ਜੇ(!part)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(part->flags & MIME_USERHEADERS_OWNER) {
    ਜੇ(part->userheaders != ਸਿਰਲੇਖਾਂ)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = ਸਿਰਲੇਖਾਂ;
  ਜੇ(ਸਿਰਲੇਖਾਂ && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  ਵਾਪਸ CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_ਅੰਕੜੇ_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, ਖਾਲੀ *arg)
{
  ਜੇ(!part)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  cleanup_part_content(part);

  ਜੇ(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->ਕਿਸਮ = MIMEKIND_CALLBACK;
  }

  ਵਾਪਸ CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_ਸੈੱਟ_subparts(curl_mimepart *part,
                                curl_mime *subparts, ਪੂਰਨ take_ownership)
{
  curl_mime *ਜੜ੍ਹ;

  ਜੇ(!part)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  /* Accept setting twice the same subparts. */
  ਜੇ(part->ਕਿਸਮ == MIMEKIND_MULTIPART && part->arg == subparts)
    ਵਾਪਸ CURLE_OK;

  cleanup_part_content(part);

  ਜੇ(subparts) {
    /* Should not have been attached already. */
    ਜੇ(subparts->ਮਾਪਾ)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

    /* Should not be the part's root. */
    ਜੜ੍ਹ = part->ਮਾਪਾ;
    ਜੇ(ਜੜ੍ਹ) {
      ਜਦੋਂ(ਜੜ੍ਹ->ਮਾਪਾ && ਜੜ੍ਹ->ਮਾਪਾ->ਮਾਪਾ)
        ਜੜ੍ਹ = ਜੜ੍ਹ->ਮਾਪਾ->ਮਾਪਾ;
      ਜੇ(subparts == ਜੜ੍ਹ) {
        /* cannot add as a subpart of itself. */
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    ਜੇ(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      ਵਾਪਸ CURLE_SEND_FAIL_REWIND;

    subparts->ਮਾਪਾ = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->ਕਿਸਮ = MIMEKIND_MULTIPART;
  }

  ਵਾਪਸ CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  ਵਾਪਸ Curl_mime_ਸੈੱਟ_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, size_t nitems, ਖਾਲੀ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (ਖਾਲੀ)ਮਾਪ;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  ਕਰੋ {
    hasread = FALSE;
    ret = readback_part(part, ਬਫਰ, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } ਜਦੋਂ(ret == STOP_FILLING);

  ਵਾਪਸ ret;
}

/* Rewind mime stream. */
ਸਥਿਰ CURLcode mime_rewind(curl_mimepart *part)
{
  ਵਾਪਸ mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
ਸਥਿਰ size_t slist_ਮਾਪ(ਬਣਤਰ curl_slist *s,
                         size_t overhead, ਅਚਲ ਅੱਖਰ *skip, size_t skiplen)
{
  size_t ਮਾਪ = 0;

  ਲਈ(; s; s = s->ਅਗਲਾ)
    ਜੇ(!skip || !ਮੇਲ_ਸਿਰਲੇਖ(s, skip, skiplen))
      ਮਾਪ += ਲੜੀ_ਲੰਬਾਈ(s->ਅੰਕੜੇ) + overhead;
  ਵਾਪਸ ਮਾਪ;
}

/* Get/compute multipart size. */
ਸਥਿਰ curl_off_t multipart_ਮਾਪ(curl_mime *mime)
{
  curl_off_t ਮਾਪ;
  curl_off_t boundarysize;
  curl_mimepart *part;

  ਜੇ(!mime)
    ਵਾਪਸ 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  ਮਾਪ = boundarysize;  /* Final boundary - CRLF after headers. */

  ਲਈ(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_ਮਾਪ(part);

    ਜੇ(sz < 0)
      ਮਾਪ = sz;

    ਜੇ(ਮਾਪ >= 0)
      ਮਾਪ += boundarysize + sz;
  }

  ਵਾਪਸ ਮਾਪ;
}

/* Get/compute mime size. */
ਸਥਿਰ curl_off_t mime_ਮਾਪ(curl_mimepart *part)
{
  curl_off_t ਮਾਪ;

  ਜੇ(part->ਕਿਸਮ == MIMEKIND_MULTIPART)
    part->datasize = multipart_ਮਾਪ(part->arg);

  ਮਾਪ = part->datasize;

  ਜੇ(part->encoder)
    ਮਾਪ = part->encoder->sizefunc(part);

  ਜੇ(ਮਾਪ >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    ਮਾਪ += slist_ਮਾਪ(part->curlheaders, 2, ਨਿਰਾ, 0);
    ਮਾਪ += slist_ਮਾਪ(part->userheaders, 2, STRCONST("Content-Type"));
    ਮਾਪ += 2;    /* CRLF after headers. */
  }
  ਵਾਪਸ ਮਾਪ;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_ਜੋੜੋ_ਸਿਰਲੇਖ(ਬਣਤਰ curl_slist **slp, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  ਬਣਤਰ curl_slist *hdr = ਨਿਰਾ;
  ਅੱਖਰ *s = ਨਿਰਾ;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  ਜੇ(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    ਜੇ(hdr)
      *slp = hdr;
    ਨਹੀਂਤਾਂ
      curlx_free(s);
  }

  ਵਾਪਸ hdr ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}

/* Add a content type header. */
ਸਥਿਰ CURLcode ਜੋੜੋ_content_type(ਬਣਤਰ curl_slist **slp,
                                 ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *boundary)
{
  ਵਾਪਸ Curl_mime_ਜੋੜੋ_ਸਿਰਲੇਖ(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ਅਚਲ ਅੱਖਰ *Curl_mime_contenttype(ਅਚਲ ਅੱਖਰ *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  ਬਣਤਰ ContentType {
    ਅਚਲ ਅੱਖਰ *extension;
    ਅਚਲ ਅੱਖਰ *type;
  };
  ਸਥਿਰ ਅਚਲ ਬਣਤਰ ContentType ctts[] = {
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

  ਜੇ(filename) {
    size_t len1 = ਲੜੀ_ਲੰਬਾਈ(filename);
    ਅਚਲ ਅੱਖਰ *nameend = filename + len1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;

    ਲਈ(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = ਲੜੀ_ਲੰਬਾਈ(ctts[i].extension);

      ਜੇ(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        ਵਾਪਸ ctts[i].type;
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ bool content_type_ਮੇਲ(ਅਚਲ ਅੱਖਰ *contenttype,
                               ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, size_t len)
{
  ਜੇ(contenttype && curl_strnequal(contenttype, ਨਿਸ਼ਾਨਾ, len))
    ਚੋਣ(contenttype[len]) {
    ਹਾਲਤ '\0':
    ਹਾਲਤ '\t':
    ਹਾਲਤ '\r':
    ਹਾਲਤ '\n':
    ਹਾਲਤ ' ':
    ਹਾਲਤ ';':
      ਵਾਪਸ TRUE;
    }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode ਜੋੜੋ_content_disposition(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        curl_mimepart *part,
                                        ਅਚਲ ਅੱਖਰ *disposition,
                                        ਅਚਲ ਅੱਖਰ *contenttype,
                                        ਗਿਣਤੀ mimestrategy strategy)
{
  ਜੇ(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  ਜੇ(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = ਨਿਰਾ;

  ਜੇ(disposition) {
    CURLcode ਨਤੀਜਾ = CURLE_OK;
    ਅੱਖਰ *name = ਨਿਰਾ;
    ਅੱਖਰ *filename = ਨਿਰਾ;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (ਅੰਕੜੇ && ਅੰਕੜੇ->ਸੈੱਟ.mime_formescape);
    ਜੇ(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

    ਜੇ(part->name) {
      name = escape_string(ਅੰਕੜੇ, part->name, strategy);
      ਜੇ(!name)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਜੇ(part->filename) {
      filename = escape_string(ਅੰਕੜੇ, part->filename, strategy);
      ਜੇ(!filename)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_mime_ਜੋੜੋ_ਸਿਰਲੇਖ(&part->curlheaders,
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
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode mime_prepare_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     curl_mimepart *part,
                                     ਅਚਲ ਅੱਖਰ *contenttype,
                                     ਅਚਲ ਅੱਖਰ *disposition,
                                     ਗਿਣਤੀ mimestrategy strategy,
                                     size_t call_ਡੂੰਘਾਈ)
{
  curl_mime *mime = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *boundary = ਨਿਰਾ;
  ਅੱਖਰ *customct;
  ਅਚਲ ਅੱਖਰ *cte = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(++call_ਡੂੰਘਾਈ > MAX_MIME_ਪੱਧਰ)
    ਵਾਪਸ CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = ਨਿਰਾ;

  /* Be sure we will not access old headers later. */
  ਜੇ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, ਨਿਰਾ);

  /* Check if content type is specified. */
  customct = part->mimetype;
  ਜੇ(!customct)
    customct = ਖੋਜ_ਸਿਰਲੇਖ(part->userheaders, STRCONST("Content-Type"));
  ਜੇ(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  ਜੇ(!contenttype) {
    ਚੋਣ(part->ਕਿਸਮ) {
    ਹਾਲਤ MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      ਰੋਕੋ;
    ਹਾਲਤ MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      ਜੇ(!contenttype)
        contenttype = Curl_mime_contenttype(part->ਅੰਕੜੇ);
      ਜੇ(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      ਰੋਕੋ;
    ਮੂਲ:
      contenttype = Curl_mime_contenttype(part->filename);
      ਰੋਕੋ;
    }
  }

  ਜੇ(part->ਕਿਸਮ == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    ਜੇ(mime)
      boundary = mime->boundary;
  }
  ਨਹੀਂਤਾਂ ਜੇ(contenttype && !customct &&
          content_type_ਮੇਲ(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = ਨਿਰਾ;

  /* Issue content-disposition header only if not already set by caller. */
  ਜੇ(!ਖੋਜ_ਸਿਰਲੇਖ(part->userheaders, STRCONST("Content-Disposition"))) {
    ਨਤੀਜਾ = ਜੋੜੋ_content_disposition(ਅੰਕੜੇ, part, disposition,
                                     contenttype, strategy);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Issue Content-Type header. */
  ਜੇ(contenttype) {
    ਨਤੀਜਾ = ਜੋੜੋ_content_type(&part->curlheaders, contenttype, boundary);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Content-Transfer-Encoding header. */
  ਜੇ(!ਖੋਜ_ਸਿਰਲੇਖ(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    ਜੇ(part->encoder)
      cte = part->encoder->name;
    ਨਹੀਂਤਾਂ ਜੇ(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->ਕਿਸਮ != MIMEKIND_MULTIPART)
      cte = "8bit";
    ਜੇ(cte) {
      ਨਤੀਜਾ = Curl_mime_ਜੋੜੋ_ਸਿਰਲੇਖ(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  ਜੇ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  ਜੇ(part->ਕਿਸਮ == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = ਨਿਰਾ;
    ਜੇ(content_type_ਮੇਲ(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    ਲਈ(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      ਨਤੀਜਾ = mime_prepare_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, subpart, ਨਿਰਾ,
                                    disposition, strategy, call_ਡੂੰਘਾਈ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_mime_prepare_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   curl_mimepart *part,
                                   ਅਚਲ ਅੱਖਰ *contenttype,
                                   ਅਚਲ ਅੱਖਰ *disposition,
                                   ਗਿਣਤੀ mimestrategy strategy)
{
  ਵਾਪਸ mime_prepare_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
ਸਥਿਰ ਖਾਲੀ mime_unpause(curl_mimepart *part)
{
  ਜੇ(part) {
    ਜੇ(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    ਜੇ(part->ਕਿਸਮ == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      ਜੇ(mime) {
        curl_mimepart *subpart;

        ਲਈ(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

ਬਣਤਰ cr_mime_ctx {
  ਬਣਤਰ Curl_creader super;
  curl_mimepart *part;
  curl_off_t ਕੁੱਲ_len;
  curl_off_t read_len;
  CURLcode error_ਨਤੀਜਾ;
  ਬਣਤਰ bufq tmpbuf;
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਵੇਖਿਆ_eos);
  ਦੋਆਧਾਰੀ_ਅੰਕ(errored);
};

ਸਥਿਰ CURLcode cr_mime_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_mime_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx->ਕੁੱਲ_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cr_mime_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_mime_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
ਸਥਿਰ CURLcode cr_mime_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_creader *reader,
                             ਅੱਖਰ *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  ਬਣਤਰ cr_mime_ctx *ctx = reader->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nread;
  ਅੱਖਰ tmp[256];

  /* Once we have errored, we will return the same error forever */
  ਜੇ(ctx->errored) {
    CURL_TRC_READ(ਅੰਕੜੇ, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (ਪੂਰਨ)ctx->error_ਨਤੀਜਾ);
    *pnread = 0;
    *peos = FALSE;
    ਵਾਪਸ ctx->error_ਨਤੀਜਾ;
  }
  ਜੇ(ctx->ਵੇਖਿਆ_eos) {
    CURL_TRC_READ(ਅੰਕੜੇ, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    ਵਾਪਸ CURLE_OK;
  }
  /* respect length limitations */
  ਜੇ(ctx->ਕੁੱਲ_len >= 0) {
    curl_off_t remain = ctx->ਕੁੱਲ_len - ctx->read_len;
    ਜੇ(remain <= 0)
      blen = 0;
    ਨਹੀਂਤਾਂ ਜੇ(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tmpbuf)) {
    ਨਤੀਜਾ = Curl_bufq_read(&ctx->tmpbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf, blen, &nread);
    ਜੇ(ਨਤੀਜਾ) {
      ctx->errored = TRUE;
      ctx->error_ਨਤੀਜਾ = ਨਤੀਜਾ;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(ਅੰਕੜੇ, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, ਆਕਾਰ(tmp), ctx->part);
    ਜੇ(nread <= ਆਕਾਰ(tmp)) {
      size_t n;
      ਨਤੀਜਾ = Curl_bufq_write(&ctx->tmpbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)tmp, nread, &n);
      ਜੇ(ਨਤੀਜਾ) {
        ctx->errored = TRUE;
        ctx->error_ਨਤੀਜਾ = ਨਤੀਜਾ;
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      /* stored it, read again */
      ਨਤੀਜਾ = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      ਜੇ(ਨਤੀਜਾ) {
        ctx->errored = TRUE;
        ctx->error_ਨਤੀਜਾ = ਨਤੀਜਾ;
        ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
  }
  ਨਹੀਂਤਾਂ
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(ਅੰਕੜੇ, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  ਚੋਣ(nread) {
  ਹਾਲਤ 0:
    ਜੇ((ctx->ਕੁੱਲ_len >= 0) && (ctx->read_len < ctx->ਕੁੱਲ_len)) {
      failf(ਅੰਕੜੇ, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->ਕੁੱਲ_len);
      ਵਾਪਸ CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->ਵੇਖਿਆ_eos = TRUE;
    ਰੋਕੋ;

  ਹਾਲਤ CURL_READFUNC_ABORT:
    failf(ਅੰਕੜੇ, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
    ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
    ਰੋਕੋ;

  ਹਾਲਤ CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(ਅੰਕੜੇ, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    ਨਤੀਜਾ = Curl_xfer_pause_send(ਅੰਕੜੇ, TRUE);
    ਰੋਕੋ; /* nothing was read */

  ਹਾਲਤ STOP_FILLING:
  ਹਾਲਤ READ_ERROR:
    failf(ਅੰਕੜੇ, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ਨਤੀਜਾ = CURLE_READ_ERROR;
    ਨਤੀਜਾ = CURLE_READ_ERROR;
    ਰੋਕੋ;

  ਮੂਲ:
    ਜੇ(nread > blen) {
      /* the read function returned a too large value */
      failf(ਅੰਕੜੇ, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਵਾਪਸ CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    ਜੇ(ctx->ਕੁੱਲ_len >= 0)
      ctx->ਵੇਖਿਆ_eos = (ctx->read_len >= ctx->ਕੁੱਲ_len);
    *pnread = nread;
    *peos = (bool)ctx->ਵੇਖਿਆ_eos;
    ਰੋਕੋ;
  }

  CURL_TRC_READ(ਅੰਕੜੇ, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->ਕੁੱਲ_len, ctx->read_len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread, *peos);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cr_mime_needs_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_mime_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ ctx->read_len > 0;
}

ਸਥਿਰ curl_off_t cr_mime_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_mime_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ ctx->ਕੁੱਲ_len;
}

ਸਥਿਰ CURLcode cr_mime_resume_from(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ Curl_creader *reader,
                                    curl_off_t ਵਿੱਥ)
{
  ਬਣਤਰ cr_mime_ctx *ctx = reader->ctx;

  ਜੇ(ਵਿੱਥ > 0) {
    curl_off_t passed = 0;

    ਕਰੋ {
      ਅੱਖਰ scratch[4 * 1024];
      size_t readthisamountnow =
        (ਵਿੱਥ - passed > (curl_off_t)ਆਕਾਰ(scratch)) ?
        ਆਕਾਰ(scratch) :
        curlx_sotouz(ਵਿੱਥ - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      ਜੇ((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(ਅੰਕੜੇ, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        ਵਾਪਸ CURLE_READ_ERROR;
      }
    } ਜਦੋਂ(passed < ਵਿੱਥ);

    /* now, decrease the size of the read */
    ਜੇ(ctx->ਕੁੱਲ_len > 0) {
      ctx->ਕੁੱਲ_len -= ਵਿੱਥ;

      ਜੇ(ctx->ਕੁੱਲ_len <= 0) {
        failf(ਅੰਕੜੇ, "Mime post already completely uploaded");
        ਵਾਪਸ CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cr_mime_cntrl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  ਬਣਤਰ cr_mime_ctx *ctx = reader->ctx;
  ਚੋਣ(opcode) {
  ਹਾਲਤ CURL_CRCNTRL_REWIND: {
    CURLcode ਨਤੀਜਾ = mime_rewind(ctx->part);
    ਜੇ(ਨਤੀਜਾ)
      failf(ਅੰਕੜੇ, "Cannot rewind mime/post data");
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਹਾਲਤ CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_CRCNTRL_CLEAR_EOS:
    ctx->ਵੇਖਿਆ_eos = FALSE;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool cr_mime_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_mime_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_ਕੁੱਲ_ਲੰਮਾਈ,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ cr_mime_ctx)
};

CURLcode Curl_creader_ਸੈੱਟ_mime(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_mimepart *part)
{
  ਬਣਤਰ Curl_creader *r;
  ਬਣਤਰ cr_mime_ctx *ctx;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_creader_create(&r, ਅੰਕੜੇ, &cr_mime, CURL_CR_CLIENT);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  ਨਤੀਜਾ = mime_rewind(ctx->part);
  ਜੇ(ਨਤੀਜਾ) {
    Curl_creader_free(ਅੰਕੜੇ, r);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ctx->ਕੁੱਲ_len = mime_ਮਾਪ(ctx->part);

  ਵਾਪਸ Curl_creader_ਸੈੱਟ(ਅੰਕੜੇ, r);
}

#ਨਹੀਂਤਾਂ /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (ਖਾਲੀ)easy;
  ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ curl_mime_free(curl_mime *mime)
{
  (ਖਾਲੀ)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (ਖਾਲੀ)mime;
  ਵਾਪਸ ਨਿਰਾ;
}

CURLcode curl_mime_name(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *name)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)name;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_mime_filename(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *filename)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)filename;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_mime_type(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *mimetype)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)mimetype;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *encoding)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)encoding;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_mime_ਅੰਕੜੇ(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, size_t datasize)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)datasize;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *filename)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)filename;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_mime_ਅੰਕੜੇ_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           ਖਾਲੀ *arg)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)datasize;
  (ਖਾਲੀ)readfunc;
  (ਖਾਲੀ)seekfunc;
  (ਖਾਲੀ)freefunc;
  (ਖਾਲੀ)arg;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)subparts;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode curl_mime_ਸਿਰਲੇਖਾਂ(curl_mimepart *part,
                           ਬਣਤਰ curl_slist *ਸਿਰਲੇਖਾਂ, ਪੂਰਨ take_ownership)
{
  (ਖਾਲੀ)part;
  (ਖਾਲੀ)ਸਿਰਲੇਖਾਂ;
  (ਖਾਲੀ)take_ownership;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

CURLcode Curl_mime_ਜੋੜੋ_ਸਿਰਲੇਖ(ਬਣਤਰ curl_slist **slp, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)slp;
  (ਖਾਲੀ)fmt;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

#ਜੇ_ਅੰਤ /* if disabled */

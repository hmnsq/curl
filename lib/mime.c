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

ರಚನೆ Curl_easy;

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

#ಆದರೆ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "slist.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_MIME_ಮಟ್ಟಗಳು 40 /* avoid deep nesting */

#ವ್ಯಾಖ್ಯಾನಿಸು READ_ERROR   ((size_t)-1)
#ವ್ಯಾಖ್ಯಾನಿಸು STOP_FILLING ((size_t)-2)

ಸ್ಥಿರ size_t mime_subparts_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, size_t nitems,
                                 ಶೂನ್ಯ *instream, bool *hasread);
ಸ್ಥಿರ curl_off_t mime_ಗಾತ್ರ(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#ವ್ಯಾಖ್ಯಾನಿಸು QP_OK           1       /* Can be represented by itself. */
#ವ್ಯಾಖ್ಯಾನಿಸು QP_SP           2       /* Space or tab. */
#ವ್ಯಾಖ್ಯಾನಿಸು QP_CR           3       /* Carriage return. */
#ವ್ಯಾಖ್ಯಾನಿಸು QP_LF           4       /* Line-feed. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ qp_class[] = {
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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __VMS
#ವ್ಯಾಖ್ಯಾನಿಸು filesize(name, stat_ದತ್ತಾಂಶ) stat_ದತ್ತಾಂಶ.st_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು fopen_read                curlx_fopen

#ಇಲ್ಲದಿದ್ದರೆ

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
curl_off_t Vms_Real_File_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ curlx_struct_stat *stat_buf)
{
  ಅಕ್ಷರ ಬಫರ್[8192];
  curl_off_t ಎಣಿಕೆ_2;
  ಪೂರ್ಣಾಂಕ ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  ಆದರೆ(!file)
    ಹಿಂತಿರುಗಿ 0;

  ಎಣಿಕೆ_2 = 0;
  ret_stat = 1;
  ಆಗ(ret_stat > 0) {
    ret_stat = fread(ಬಫರ್, 1, ಗಾತ್ರ(ಬಫರ್), file);
    ಆದರೆ(ret_stat)
      ಎಣಿಕೆ_2 += ret_stat;
  }
  curlx_fclose(file);

  ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
ಸ್ಥಿರ curl_off_t Vms_Special_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                 ಸ್ಥಿರಾಂಕ curlx_struct_stat *stat_buf)
{
  ಆಯ್ಕೆ(stat_buf->st_fab_rfm) {
  ಸ್ಥಿತಿ FAB$C_VAR:
  ಸ್ಥಿತಿ FAB$C_VFC:
    ಹಿಂತಿರುಗಿ Vms_Real_File_ಗಾತ್ರ(name, stat_buf);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ stat_buf->st_ಗಾತ್ರ;
  }
}

#ವ್ಯಾಖ್ಯಾನಿಸು filesize(name, stat_ದತ್ತಾಂಶ) Vms_Special_ಗಾತ್ರ(name, &stat_ದತ್ತಾಂಶ)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
ಸ್ಥಿರ FILE *vmsfopenread(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ)
{
  curlx_struct_stat statbuf;
  ಪೂರ್ಣಾಂಕ res = curlx_stat(file, &statbuf);

  ಆದರೆ(res != -1) {
    ಆಯ್ಕೆ(statbuf.st_fab_rfm) {
    ಸ್ಥಿತಿ FAB$C_VAR:
    ಸ್ಥಿತಿ FAB$C_VFC:
    ಸ್ಥಿತಿ FAB$C_STMCR:
      ಹಿಂತಿರುಗಿ curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು fopen_read vmsfopenread
#ಆದರೆ_ಅಂತ್ಯ /* !__VMS */

/* Set readback state. */
ಸ್ಥಿರ ಶೂನ್ಯ mimesetstate(ರಚನೆ mime_state *state,
                         ಎಣಿಕೆ mimestate tok, ಶೂನ್ಯ *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->ಸ್ಥಾನಾಂತರ = 0;
}

/* Escape header string into allocated memory. */
ಸ್ಥಿರ ಅಕ್ಷರ *escape_string(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಎಣಿಕೆ mimestrategy strategy)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ dynbuf db;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *p;
  /* replace first character by rest of string. */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    ಖಾಲಿ
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    ಖಾಲಿ
  };

  ಕೋಷ್ಟಕ = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  ಆದರೆ(strategy == MIMESTRATEGY_MAIL || (ದತ್ತಾಂಶ && ದತ್ತಾಂಶ->ಗಣ.mime_formescape))
    ಕೋಷ್ಟಕ = mimetable;

  curlx_dyn_init(&db, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

  ಗಾಗಿ(ಫಲಿತಾಂಶ = curlx_dyn_addn(&db, STRCONST("")); !ಫಲಿತಾಂಶ && *src; src++) {
    ಗಾಗಿ(p = ಕೋಷ್ಟಕ; *p && **p != *src; p++)
      ;

    ಆದರೆ(*p)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&db, *p + 1);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&db, src, 1);
  }

  ಹಿಂತಿರುಗಿ curlx_dyn_ptr(&db);
}

/* Check if header matches. */
ಸ್ಥಿರ ಅಕ್ಷರ *ಹೊಂದಿಕೆ_ಶೀರ್ಷಿಕೆ(ರಚನೆ curl_slist *hdr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lbl, size_t len)
{
  ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;

  ಆದರೆ(curl_strnequal(hdr->ದತ್ತಾಂಶ, lbl, len) && hdr->ದತ್ತಾಂಶ[len] == ':')
    ಗಾಗಿ(ಮೌಲ್ಯ = hdr->ದತ್ತಾಂಶ + len + 1; *ಮೌಲ್ಯ == ' '; ಮೌಲ್ಯ++)
      ;
  ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
}

/* Get a header from an slist. */
ಸ್ಥಿರ ಅಕ್ಷರ *ಹುಡುಕು_ಶೀರ್ಷಿಕೆ(ರಚನೆ curl_slist *hdrlist,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hdr, size_t len)
{
  ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;

  ಗಾಗಿ(; !ಮೌಲ್ಯ && hdrlist; hdrlist = hdrlist->ಮುಂದಿನ)
    ಮೌಲ್ಯ = ಹೊಂದಿಕೆ_ಶೀರ್ಷಿಕೆ(hdrlist, hdr, len);

  ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
}

ಸ್ಥಿರ ಅಕ್ಷರ *strippath(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fullfile)
{
  ಅಕ್ಷರ *filename;
  ಅಕ್ಷರ *ತಳ;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  ಆದರೆ(!filename)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ತಳ = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  ಹಿಂತಿರುಗಿ ತಳ; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
ಸ್ಥಿರ ಶೂನ್ಯ cleanup_encoder_state(ರಚನೆ mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
ಸ್ಥಿರ size_t encoder_nop_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, bool ateof,
                               ರಚನೆ curl_mimepart *part)
{
  ರಚನೆ mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (ಶೂನ್ಯ)ateof;

  ಆದರೆ(!ಗಾತ್ರ_2)
    ಹಿಂತಿರುಗಿ STOP_FILLING;

  ಆದರೆ(ಗಾತ್ರ_2 > insize)
    ಗಾತ್ರ_2 = insize;

  ಆದರೆ(ಗಾತ್ರ_2)
    ಸ್ಮರಣೆ_ನಕಲು(ಬಫರ್, st->buf + st->bufbeg, ಗಾತ್ರ_2);

  st->bufbeg += ಗಾತ್ರ_2;
  ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

ಸ್ಥಿರ curl_off_t encoder_nop_ಗಾತ್ರ(curl_mimepart *part)
{
  ಹಿಂತಿರುಗಿ part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
ಸ್ಥಿರ size_t encoder_7bit_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, bool ateof,
                                curl_mimepart *part)
{
  ರಚನೆ mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (ಶೂನ್ಯ)ateof;

  ಆದರೆ(!ಗಾತ್ರ_2)
    ಹಿಂತಿರುಗಿ STOP_FILLING;

  ಆದರೆ(ಗಾತ್ರ_2 > cursize)
    ಗಾತ್ರ_2 = cursize;

  ಗಾಗಿ(cursize = 0; cursize < ಗಾತ್ರ_2; cursize++) {
    *ಬಫರ್ = st->buf[st->bufbeg];
    ಆದರೆ(*ಬಫರ್++ & 0x80)
      ಹಿಂತಿರುಗಿ cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  ಹಿಂತಿರುಗಿ cursize;
}

/* Base64 content encoder. */
ಸ್ಥಿರ size_t encoder_base64_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, bool ateof,
                                  curl_mimepart *part)
{
  ರಚನೆ mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  ಪೂರ್ಣಾಂಕ i;
  ಅಕ್ಷರ *ptr = ಬಫರ್;

  ಆಗ(st->bufbeg < st->bufend) {
    /* Line full ? */
    ಆದರೆ(st->pos > MAX_ENCODED_ಸಾಲು_ಉದ್ದಳತೆ - 4) {
      /* Yes, we need 2 characters for CRLF. */
      ಆದರೆ(ಗಾತ್ರ_2 < 2) {
        ಆದರೆ(!cursize)
          ಹಿಂತಿರುಗಿ STOP_FILLING;
        ನಿಲ್ಲಿಸು;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      ಗಾತ್ರ_2 -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    ಆದರೆ(ಗಾತ್ರ_2 < 4) {
      ಆದರೆ(!cursize)
        ಹಿಂತಿರುಗಿ STOP_FILLING;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(st->bufend - st->bufbeg < 3)
      ನಿಲ್ಲಿಸು;

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
    ಗಾತ್ರ_2 -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  ಆದರೆ(ateof) {
    ಆದರೆ(ಗಾತ್ರ_2 < 4) {
      ಆದರೆ(!cursize)
        ಹಿಂತಿರುಗಿ STOP_FILLING;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      ಆದರೆ(st->bufend != st->bufbeg) {

        ಆದರೆ(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        ಆದರೆ(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  ಹಿಂತಿರುಗಿ cursize;
}

/* The maximum input size that does not cause an overflow. */
#ವ್ಯಾಖ್ಯಾನಿಸು BASE64_MAX_ಒಳಹರಿವು_ಗಾತ್ರ                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_ಸಾಲು_ಉದ್ದಳತೆ + 2)) *                  \
    MAX_ENCODED_ಸಾಲು_ಉದ್ದಳತೆ / 4) * 3 - 3)

ಸ್ಥಿರ curl_off_t encoder_base64_ಗಾತ್ರ(curl_mimepart *part)
{
  curl_off_t ಗಾತ್ರ_2 = part->datasize;

  ಆದರೆ(ಗಾತ್ರ_2 <= 0)
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  ಆದರೆ(ಗಾತ್ರ_2 > BASE64_MAX_ಒಳಹರಿವು_ಗಾತ್ರ)
    ಹಿಂತಿರುಗಿ -1;

  /* Compute base64 character count. */
  ಗಾತ್ರ_2 = 4 * (1 + ((ಗಾತ್ರ_2 - 1) / 3));

  /* Effective character count must include CRLFs. */
  ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 + (2 * ((ಗಾತ್ರ_2 - 1) / MAX_ENCODED_ಸಾಲು_ಉದ್ದಳತೆ));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ qp_lookahead_eol(ರಚನೆ mime_encoder_state *st, ಪೂರ್ಣಾಂಕ ateof, size_t n)
{
  n += st->bufbeg;
  ಆದರೆ(n >= st->bufend && ateof)
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ(n + 2 > st->bufend)
    ಹಿಂತಿರುಗಿ ateof ? 0 : -1;
  ಆದರೆ(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    ಹಿಂತಿರುಗಿ 1;
  ಹಿಂತಿರುಗಿ 0;
}

/* Quoted-printable encoder. */
ಸ್ಥಿರ size_t encoder_qp_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, bool ateof,
                              curl_mimepart *part)
{
  ರಚನೆ mime_encoder_state *st = &part->encstate;
  ಅಕ್ಷರ *ptr = ಬಫರ್;
  size_t cursize = 0;
  ಪೂರ್ಣಾಂಕ softlinebreak;
  ಅಕ್ಷರ buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  ಆಗ(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    ಪೂರ್ಣಾಂಕ i = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)st->buf[st->bufbeg];
    buf[0] = (ಅಕ್ಷರ)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    ಆಯ್ಕೆ(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    ಸ್ಥಿತಿ QP_OK:          /* Not a special character. */
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      ಆಯ್ಕೆ(qp_lookahead_eol(st, ateof, 1)) {
      ಸ್ಥಿತಿ -1:          /* More input data needed. */
        ಹಿಂತಿರುಗಿ cursize;
      ಸ್ಥಿತಿ 0:           /* No encoding needed. */
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      ಆಯ್ಕೆ(qp_lookahead_eol(st, ateof, 0)) {
      ಸ್ಥಿತಿ -1:          /* Need more data. */
        ಹಿಂತಿರುಗಿ cursize;
      ಸ್ಥಿತಿ 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      ನಿಲ್ಲಿಸು;
    }

    /* Be sure the encoded character fits within maximum line length. */
    ಆದರೆ(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_ಸಾಲು_ಉದ್ದಳತೆ;
      ಆದರೆ(!softlinebreak && st->pos + len == MAX_ENCODED_ಸಾಲು_ಉದ್ದಳತೆ) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        ಆಯ್ಕೆ(qp_lookahead_eol(st, ateof, consumed)) {
        ಸ್ಥಿತಿ -1:        /* Need more data. */
          ಹಿಂತಿರುಗಿ cursize;
        ಸ್ಥಿತಿ 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          ನಿಲ್ಲಿಸು;
        }
      }
      ಆದರೆ(softlinebreak) {
        curlx_strcopy(buf, ಗಾತ್ರ(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    ಆದರೆ(len > ಗಾತ್ರ_2) {
      ಆದರೆ(!cursize)
        ಹಿಂತಿರುಗಿ STOP_FILLING;
      ನಿಲ್ಲಿಸು;
    }

    /* Append to output buffer. */
    ಸ್ಮರಣೆ_ನಕಲು(ptr, buf, len);
    cursize += len;
    ptr += len;
    ಗಾತ್ರ_2 -= len;
    st->pos += len;
    ಆದರೆ(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  ಹಿಂತಿರುಗಿ cursize;
}

ಸ್ಥಿರ curl_off_t encoder_qp_ಗಾತ್ರ(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  ಹಿಂತಿರುಗಿ part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
ಸ್ಥಿರ size_t mime_mem_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, size_t nitems,
                            ಶೂನ್ಯ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.ಸ್ಥಾನಾಂತರ);
  (ಶೂನ್ಯ)ಗಾತ್ರ_2;  /* Always 1 */

  ಆದರೆ(!nitems)
    ಹಿಂತಿರುಗಿ STOP_FILLING;

  ಆದರೆ(sz > nitems)
    sz = nitems;

  ಆದರೆ(sz)
    ಸ್ಮರಣೆ_ನಕಲು(ಬಫರ್, part->ದತ್ತಾಂಶ + curlx_sotouz(part->state.ಸ್ಥಾನಾಂತರ), sz);

  ಹಿಂತಿರುಗಿ sz;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mime_mem_seek(ಶೂನ್ಯ *instream, curl_off_t ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ಆಯ್ಕೆ(whence) {
  ಸ್ಥಿತಿ SEEK_CUR:
    ಸ್ಥಾನಾಂತರ += part->state.ಸ್ಥಾನಾಂತರ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SEEK_END:
    ಸ್ಥಾನಾಂತರ += part->datasize;
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ಸ್ಥಾನಾಂತರ < 0 || ಸ್ಥಾನಾಂತರ > part->datasize)
    ಹಿಂತಿರುಗಿ CURL_SEEKFUNC_FAIL;

  part->state.ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ;
  ಹಿಂತಿರುಗಿ CURL_SEEKFUNC_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ mime_mem_free(ಶೂನ್ಯ *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->ದತ್ತಾಂಶ);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
ಸ್ಥಿರ bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  ಆದರೆ(part->fp)
    ಹಿಂತಿರುಗಿ FALSE;
  part->fp = fopen_read(part->ದತ್ತಾಂಶ, "rb");
  ಹಿಂತಿರುಗಿ part->fp ? FALSE : TRUE;
}

ಸ್ಥಿರ size_t mime_file_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, size_t nitems,
                             ಶೂನ್ಯ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ಆದರೆ(!nitems)
    ಹಿಂತಿರುಗಿ STOP_FILLING;

  ಆದರೆ(mime_open_file(part))
    ಹಿಂತಿರುಗಿ READ_ERROR;

  ಹಿಂತಿರುಗಿ fread(ಬಫರ್, ಗಾತ್ರ_2, nitems, part->fp);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mime_file_seek(ಶೂನ್ಯ *instream, curl_off_t ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ಆದರೆ(whence == SEEK_SET && !ಸ್ಥಾನಾಂತರ && !part->fp)
    ಹಿಂತಿರುಗಿ CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  ಆದರೆ(mime_open_file(part))
    ಹಿಂತಿರುಗಿ CURL_SEEKFUNC_FAIL;

  ಹಿಂತಿರುಗಿ curlx_fseek(part->fp, ಸ್ಥಾನಾಂತರ, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ mime_file_free(ಶೂನ್ಯ *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  ಆದರೆ(part->fp) {
    curlx_fclose(part->fp);
    part->fp = ಖಾಲಿ;
  }
  curlx_safefree(part->ದತ್ತಾಂಶ);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
ಸ್ಥಿರ size_t readback_ಬೈಟುಗಳು(ರಚನೆ mime_state *state,
                             ಅಕ್ಷರ *ಬಫರ್, size_t bufsize,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t numbytes,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *trail, size_t traillen)
{
  size_t sz;
  size_t ಸ್ಥಾನಾಂತರ = curlx_sotouz(state->ಸ್ಥಾನಾಂತರ);

  ಆದರೆ(numbytes > ಸ್ಥಾನಾಂತರ) {
    sz = numbytes - ಸ್ಥಾನಾಂತರ;
    ಬೈಟುಗಳು += ಸ್ಥಾನಾಂತರ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    sz = ಸ್ಥಾನಾಂತರ - numbytes;
    ಆದರೆ(sz >= traillen)
      ಹಿಂತಿರುಗಿ 0;
    ಬೈಟುಗಳು = trail + sz;
    sz = traillen - sz;
  }

  ಆದರೆ(sz > bufsize)
    sz = bufsize;

  ಸ್ಮರಣೆ_ನಕಲು(ಬಫರ್, ಬೈಟುಗಳು, sz);
  state->ಸ್ಥಾನಾಂತರ += sz;
  ಹಿಂತಿರುಗಿ sz;
}

/* Read a non-encoded part content. */

ಸ್ಥಿರ size_t read_part_content(curl_mimepart *part, ಅಕ್ಷರ *ಬಫರ್,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  ಆಯ್ಕೆ(part->lastreadstatus) {
  ಸ್ಥಿತಿ 0:
  ಸ್ಥಿತಿ CURL_READFUNC_ABORT:
  ಸ್ಥಿತಿ CURL_READFUNC_PAUSE:
  ಸ್ಥಿತಿ READ_ERROR:
    ಹಿಂತಿರುಗಿ part->lastreadstatus;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  /* If we can determine we are at end of part data, spare a read. */
  ಆದರೆ(part->datasize != (curl_off_t)-1 &&
     part->state.ಸ್ಥಾನಾಂತರ >= part->datasize) {
    /* sz is already zero. */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆಯ್ಕೆ(part->ಬಗೆ) {
    ಸ್ಥಿತಿ MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(ಬಫರ್, 1, bufsize, part->arg, hasread);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMEKIND_FILE:
      ಆದರೆ(part->fp && feof(part->fp))
        ನಿಲ್ಲಿಸು;  /* At EOF. */
      FALLTHROUGH();
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಆದರೆ(part->readfunc) {
        ಆದರೆ(!(part->flags & MIME_FAST_READ)) {
          ಆದರೆ(*hasread)
            ಹಿಂತಿರುಗಿ STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(ಬಫರ್, 1, bufsize, part->arg);
      }
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆಯ್ಕೆ(sz) {
  ಸ್ಥಿತಿ STOP_FILLING:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 0:
  ಸ್ಥಿತಿ CURL_READFUNC_ABORT:
  ಸ್ಥಿತಿ CURL_READFUNC_PAUSE:
  ಸ್ಥಿತಿ READ_ERROR:
    part->lastreadstatus = sz;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    part->state.ಸ್ಥಾನಾಂತರ += sz;
    part->lastreadstatus = sz;
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ sz;
}

/* Read and encode part content. */
ಸ್ಥಿರ size_t read_encoded_part_content(curl_mimepart *part, ಅಕ್ಷರ *ಬಫರ್,
                                        size_t bufsize, bool *hasread)
{
  ರಚನೆ mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  ಗಾಗಿ(;;) {
    ಆದರೆ(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(ಬಫರ್, bufsize, ateof, part);
      ಆಯ್ಕೆ(sz) {
      ಸ್ಥಿತಿ 0:
        ಆದರೆ(ateof)
          ಹಿಂತಿರುಗಿ cursize;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ READ_ERROR:
      ಸ್ಥಿತಿ STOP_FILLING:
        ಹಿಂತಿರುಗಿ cursize ? cursize : sz;
      ಪೂರ್ವನಿಯೋಜಿತ:
        cursize += sz;
        ಬಫರ್ += sz;
        bufsize -= sz;
        ಮುಂದುವರಿ;
      }
    }

    /* We need more data in input buffer. */
    ಆದರೆ(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      ಆದರೆ(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    ಆದರೆ(st->bufend >= ಗಾತ್ರ(st->buf))
      ಹಿಂತಿರುಗಿ cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           ಗಾತ್ರ(st->buf) - st->bufend, hasread);
    ಆಯ್ಕೆ(sz) {
    ಸ್ಥಿತಿ 0:
      ateof = TRUE;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_READFUNC_ABORT:
    ಸ್ಥಿತಿ CURL_READFUNC_PAUSE:
    ಸ್ಥಿತಿ READ_ERROR:
    ಸ್ಥಿತಿ STOP_FILLING:
      ಹಿಂತಿರುಗಿ cursize ? cursize : sz;
    ಪೂರ್ವನಿಯೋಜಿತ:
      st->bufend += sz;
      ನಿಲ್ಲಿಸು;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
ಸ್ಥಿರ size_t readback_part(curl_mimepart *part,
                            ಅಕ್ಷರ *ಬಫರ್, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  ಆಗ(bufsize) {
    size_t sz = 0;
    ರಚನೆ curl_slist *hdr = (ರಚನೆ curl_slist *)part->state.ptr;
    ಆಯ್ಕೆ(part->state.state) {
    ಸ್ಥಿತಿ MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_USERHEADERS:
      ಆದರೆ(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, ಖಾಲಿ);
        ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(ಹೊಂದಿಕೆ_ಶೀರ್ಷಿಕೆ(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->ಮುಂದಿನ);
        ನಿಲ್ಲಿಸು;
      }
      FALLTHROUGH();
    ಸ್ಥಿತಿ MIMESTATE_CURLHEADERS:
      ಆದರೆ(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      ಇಲ್ಲದಿದ್ದರೆ {
        sz = readback_ಬೈಟುಗಳು(&part->state, ಬಫರ್, bufsize,
                            hdr->ದತ್ತಾಂಶ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hdr->ದತ್ತಾಂಶ), STRCONST("\r\n"));
        ಆದರೆ(!sz)
          mimesetstate(&part->state, part->state.state, hdr->ಮುಂದಿನ);
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_EOH:
      sz = readback_ಬೈಟುಗಳು(&part->state, ಬಫರ್, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      ಆದರೆ(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, ಖಾಲಿ);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, ಖಾಲಿ);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_CONTENT:
      ಆದರೆ(part->encoder)
        sz = read_encoded_part_content(part, ಬಫರ್, bufsize, hasread);
      ಇಲ್ಲದಿದ್ದರೆ
        sz = read_part_content(part, ಬಫರ್, bufsize, hasread);
      ಆಯ್ಕೆ(sz) {
      ಸ್ಥಿತಿ 0:
        mimesetstate(&part->state, MIMESTATE_ಕೊನೆ, ಖಾಲಿ);
        /* Try sparing open file descriptors. */
        ಆದರೆ(part->ಬಗೆ == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = ಖಾಲಿ;
        }
        FALLTHROUGH();
      ಸ್ಥಿತಿ CURL_READFUNC_ABORT:
      ಸ್ಥಿತಿ CURL_READFUNC_PAUSE:
      ಸ್ಥಿತಿ READ_ERROR:
      ಸ್ಥಿತಿ STOP_FILLING:
        ಹಿಂತಿರುಗಿ cursize ? cursize : sz;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_ಕೊನೆ:
      ಹಿಂತಿರುಗಿ cursize;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    ಬಫರ್ += sz;
    bufsize -= sz;
  }

  ಹಿಂತಿರುಗಿ cursize;
}

/* Readback from mime. Warning: not a read callback function. */
ಸ್ಥಿರ size_t mime_subparts_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, size_t nitems,
                                 ಶೂನ್ಯ *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (ಶೂನ್ಯ)ಗಾತ್ರ_2;  /* Always 1 */

  ಆಗ(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    ಆಯ್ಕೆ(mime->state.state) {
    ಸ್ಥಿತಿ MIMESTATE_BEGIN:
    ಸ್ಥಿತಿ MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.ಸ್ಥಾನಾಂತರ += 2;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_BOUNDARY1:
      sz = readback_ಬೈಟುಗಳು(&mime->state, ಬಫರ್, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      ಆದರೆ(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_BOUNDARY2:
      ಆದರೆ(part)
        sz = readback_ಬೈಟುಗಳು(&mime->state, ಬಫರ್, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      ಇಲ್ಲದಿದ್ದರೆ
        sz = readback_ಬೈಟುಗಳು(&mime->state, ಬಫರ್, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      ಆದರೆ(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_CONTENT:
      ಆದರೆ(!part) {
        mimesetstate(&mime->state, MIMESTATE_ಕೊನೆ, ಖಾಲಿ);
        ನಿಲ್ಲಿಸು;
      }
      sz = readback_part(part, ಬಫರ್, nitems, hasread);
      ಆಯ್ಕೆ(sz) {
      ಸ್ಥಿತಿ CURL_READFUNC_ABORT:
      ಸ್ಥಿತಿ CURL_READFUNC_PAUSE:
      ಸ್ಥಿತಿ READ_ERROR:
      ಸ್ಥಿತಿ STOP_FILLING:
        ಹಿಂತಿರುಗಿ cursize ? cursize : sz;
      ಸ್ಥಿತಿ 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMESTATE_ಕೊನೆ:
      ಹಿಂತಿರುಗಿ cursize;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    ಬಫರ್ += sz;
    nitems -= sz;
  }

  ಹಿಂತಿರುಗಿ cursize;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mime_part_rewind(curl_mimepart *part)
{
  ಪೂರ್ಣಾಂಕ res = CURL_SEEKFUNC_OK;
  ಎಣಿಕೆ mimestate targetstate = MIMESTATE_BEGIN;

  ಆದರೆ(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  ಆದರೆ(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    ಆದರೆ(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      ಆಯ್ಕೆ(res) {
      ಸ್ಥಿತಿ CURL_SEEKFUNC_OK:
      ಸ್ಥಿತಿ CURL_SEEKFUNC_FAIL:
      ಸ್ಥಿತಿ CURL_SEEKFUNC_CANTSEEK:
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        res = CURL_SEEKFUNC_FAIL;
        ನಿಲ್ಲಿಸು;
      }
    }
  }

  ಆದರೆ(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, ಖಾಲಿ);

  part->lastreadstatus = 1; /* Successful read status. */
  ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mime_subparts_seek(ಶೂನ್ಯ *instream, curl_off_t ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  ಪೂರ್ಣಾಂಕ rc = CURL_SEEKFUNC_OK;

  ಆದರೆ(whence != SEEK_SET || ಸ್ಥಾನಾಂತರ)
    ಹಿಂತಿರುಗಿ CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  ಆದರೆ(mime->state.state == MIMESTATE_BEGIN)
    ಹಿಂತಿರುಗಿ CURL_SEEKFUNC_OK;           /* Already rewound. */

  ಗಾಗಿ(part = mime->firstpart; part; part = part->nextpart) {
    ಪೂರ್ಣಾಂಕ res = mime_part_rewind(part);
    ಆದರೆ(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  ಆದರೆ(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ಖಾಲಿ);

  ಹಿಂತಿರುಗಿ rc;
}

/* Release part content. */
ಸ್ಥಿರ ಶೂನ್ಯ cleanup_part_content(curl_mimepart *part)
{
  ಆದರೆ(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = ಖಾಲಿ;
  part->seekfunc = ಖಾಲಿ;
  part->freefunc = ಖಾಲಿ;
  part->arg = (ಶೂನ್ಯ *)part;           /* Defaults to part itself. */
  part->ದತ್ತಾಂಶ = ಖಾಲಿ;
  part->fp = ಖಾಲಿ;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->ಬಗೆ = MIMEKIND_NONE;
  part->flags &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

ಸ್ಥಿರ ಶೂನ್ಯ mime_subparts_free(ಶೂನ್ಯ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ಆದರೆ(mime && mime->ಪೋಷಕ) {
    mime->ಪೋಷಕ->freefunc = ಖಾಲಿ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->ಪೋಷಕ);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
ಸ್ಥಿರ ಶೂನ್ಯ mime_subparts_unbind(ಶೂನ್ಯ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ಆದರೆ(mime && mime->ಪೋಷಕ) {
    mime->ಪೋಷಕ->freefunc = ಖಾಲಿ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->ಪೋಷಕ);  /* Avoid dangling pointer in part. */
    mime->ಪೋಷಕ = ಖಾಲಿ;
  }
}

ಶೂನ್ಯ Curl_mime_cleanpart(curl_mimepart *part)
{
  ಆದರೆ(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    ಆದರೆ(part->flags & MIME_USERHEADERS_OWNER)
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

  ಆದರೆ(!mime)
    ಹಿಂತಿರುಗಿ;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  ಆಗ(mime) {
    part = mime->firstpart;
    ಆದರೆ(part) {
      mime->firstpart = part->nextpart;
      ಆದರೆ(part->ಬಗೆ == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = ಖಾಲಿ;
        cleanup_part_content(part);
        subparts->ಪೋಷಕ = mime->ಪೋಷಕ;
        mime->ಪೋಷಕ = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      curl_mime *ಪೋಷಕ = (curl_mime *)mime->ಪೋಷಕ;
      curlx_free(mime);
      mime = ಪೋಷಕ;
    }
  }
}

ಸ್ಥಿರ bool mime_is_too_deep(ಸ್ಥಿರಾಂಕ curl_mimepart *part, size_t call_ಆಳ)
{
  DEBUGASSERT(part);

  ಆದರೆ(++call_ಆಳ > MAX_MIME_ಮಟ್ಟಗಳು)
    ಹಿಂತಿರುಗಿ TRUE;

  ಆದರೆ(part->ಬಗೆ == MIMEKIND_MULTIPART && part->arg) {
    ಸ್ಥಿರಾಂಕ curl_mime *mime = (ಸ್ಥಿರಾಂಕ curl_mime *)part->arg;
    ಸ್ಥಿರಾಂಕ curl_mimepart *s;

    ಗಾಗಿ(s = mime->firstpart; s; s = s->nextpart) {
      ಆದರೆ(mime_is_too_deep(s, call_ಆಳ))
        ಹಿಂತಿರುಗಿ TRUE;
    }
  }

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode mime_duppart(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             curl_mimepart *dst, ಸ್ಥಿರಾಂಕ curl_mimepart *src,
                             size_t call_ಆಳ)
{
  curl_mime *mime;
  curl_mimepart *d;
  ಸ್ಥಿರಾಂಕ curl_mimepart *s;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  ಆದರೆ(++call_ಆಳ > MAX_MIME_ಮಟ್ಟಗಳು)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;

  /* Duplicate content. */
  ಆಯ್ಕೆ(src->ಬಗೆ) {
  ಸ್ಥಿತಿ MIMEKIND_NONE:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ MIMEKIND_ದತ್ತಾಂಶ:
    ಫಲಿತಾಂಶ = curl_mime_ದತ್ತಾಂಶ(dst, src->ದತ್ತಾಂಶ, (size_t)src->datasize);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ MIMEKIND_FILE:
    ಫಲಿತಾಂಶ = curl_mime_filedata(dst, src->ದತ್ತಾಂಶ);
    /* Do not abort duplication if file is not readable. */
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_READ_ERROR)
      ಫಲಿತಾಂಶ = CURLE_OK;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ MIMEKIND_CALLBACK:
    ಫಲಿತಾಂಶ = curl_mime_ದತ್ತಾಂಶ_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(ದತ್ತಾಂಶ);
    ಫಲಿತಾಂಶ = mime ? curl_mime_subparts(dst, mime) : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    /* Duplicate subparts. */
    ಗಾಗಿ(s = ((curl_mime *)src->arg)->firstpart; !ಫಲಿತಾಂಶ && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      ಫಲಿತಾಂಶ = d ? mime_duppart(ದತ್ತಾಂಶ, d, s, call_ಆಳ) : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:  /* Invalid kind: should not occur. */
    DEBUGF(infof(ದತ್ತಾಂಶ, "invalid MIMEKIND* attempt"));
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;  /* Internal error? */
    ನಿಲ್ಲಿಸು;
  }

  /* Duplicate headers. */
  ಆದರೆ(!ಫಲಿತಾಂಶ && src->userheaders) {
    ರಚನೆ curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    ಆದರೆ(!hdrs)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      ಫಲಿತಾಂಶ = curl_mime_ಶೀರ್ಷಿಕೆಗಳು(dst, hdrs, TRUE);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        curl_slist_free_all(hdrs);
    }
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    ಫಲಿತಾಂಶ = curl_mime_type(dst, src->mimetype);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curl_mime_name(dst, src->name);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  ಆದರೆ(ಫಲಿತಾಂಶ)
    Curl_mime_cleanpart(dst);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_mime_duppart(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           curl_mimepart *dst, ಸ್ಥಿರಾಂಕ curl_mimepart *src)
{
  ಆದರೆ(!dst || !src)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(mime_is_too_deep(src, 0))
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;

  ಹಿಂತಿರುಗಿ mime_duppart(ದತ್ತಾಂಶ, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(ಶೂನ್ಯ *easy)
{
  curl_mime *mime = curlx_malloc(ಗಾತ್ರ(*mime));

  ಆದರೆ(mime) {
    mime->ಪೋಷಕ = ಖಾಲಿ;
    mime->firstpart = ಖಾಲಿ;
    mime->lastpart = ಖಾಲಿ;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    ಆದರೆ(Curl_rand_alnum(easy,
                       (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ಖಾಲಿ);
  }

  ಹಿಂತಿರುಗಿ mime;
}

/* Initialize a mime part. */
ಶೂನ್ಯ Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, ಗಾತ್ರ(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, ಖಾಲಿ);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  ಆದರೆ(!mime)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  part = curlx_malloc(ಗಾತ್ರ(*part));

  ಆದರೆ(part) {
    Curl_mime_initpart(part);
    part->ಪೋಷಕ = mime;

    ಆದರೆ(mime->lastpart)
      mime->lastpart->nextpart = part;
    ಇಲ್ಲದಿದ್ದರೆ
      mime->firstpart = part;

    mime->lastpart = part;
  }

  ಹಿಂತಿರುಗಿ part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  curlx_safefree(part->name);

  ಆದರೆ(name) {
    part->name = curlx_strdup(name);
    ಆದರೆ(!part->name)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename)
{
  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  curlx_safefree(part->filename);

  ಆದರೆ(filename) {
    part->filename = curlx_strdup(filename);
    ಆದರೆ(!part->filename)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_ದತ್ತಾಂಶ(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, size_t datasize)
{
  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  cleanup_part_content(part);

  ಆದರೆ(ದತ್ತಾಂಶ) {
    ಆದರೆ(datasize == CURL_ZERO_TERMINATED)
      datasize = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ದತ್ತಾಂಶ);

    part->ದತ್ತಾಂಶ = curlx_memdup0(ದತ್ತಾಂಶ, datasize);
    ಆದರೆ(!part->ದತ್ತಾಂಶ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->ಬಗೆ = MIMEKIND_ದತ್ತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  cleanup_part_content(part);

  ಆದರೆ(filename) {
    ಅಕ್ಷರ *ತಳ;
    curlx_struct_stat sbuf;

    ಆದರೆ(curlx_stat(filename, &sbuf))
      ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
    ಇಲ್ಲದಿದ್ದರೆ {
      part->ದತ್ತಾಂಶ = curlx_strdup(filename);
      ಆದರೆ(!part->ದತ್ತಾಂಶ)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಇಲ್ಲದಿದ್ದರೆ {
        part->datasize = -1;
        ಆದರೆ(S_ISREG(sbuf.st_ಬಹುಲಕ)) {
          part->datasize = filesize(filename, sbuf);
          part->seekfunc = mime_file_seek;
        }

        part->readfunc = mime_file_read;
        part->freefunc = mime_file_free;
        part->ಬಗೆ = MIMEKIND_FILE;

        /* As a side effect, set the filename to the current file's base name.
           It is possible to withdraw this by explicitly calling
           curl_mime_filename() with a NULL filename argument after the current
           call. */
        ತಳ = strippath(filename);
        ಆದರೆ(!ತಳ)
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಇಲ್ಲದಿದ್ದರೆ {
          ಫಲಿತಾಂಶ = curl_mime_filename(part, ತಳ);
          curlx_free(ತಳ);
        }
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mimetype)
{
  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  curlx_safefree(part->mimetype);

  ಆದರೆ(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    ಆದರೆ(!part->mimetype)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_ಗಾತ್ರ },
  { "8bit", encoder_nop_read, encoder_nop_ಗಾತ್ರ },
  { "7bit", encoder_7bit_read, encoder_nop_ಗಾತ್ರ },
  { "base64", encoder_base64_read, encoder_base64_ಗಾತ್ರ },
  { "quoted-printable", encoder_qp_read, encoder_qp_ಗಾತ್ರ },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ mime_encoder *mep;

  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  part->encoder = ಖಾಲಿ;

  ಆದರೆ(!encoding)
    ಹಿಂತಿರುಗಿ CURLE_OK;    /* Removing current encoder. */

  ಗಾಗಿ(mep = encoders; mep->name; mep++)
    ಆದರೆ(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      ಫಲಿತಾಂಶ = CURLE_OK;
    }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Set mime part headers. */
CURLcode curl_mime_ಶೀರ್ಷಿಕೆಗಳು(curl_mimepart *part,
                           ರಚನೆ curl_slist *ಶೀರ್ಷಿಕೆಗಳು, ಪೂರ್ಣಾಂಕ take_ownership)
{
  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(part->flags & MIME_USERHEADERS_OWNER) {
    ಆದರೆ(part->userheaders != ಶೀರ್ಷಿಕೆಗಳು)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = ಶೀರ್ಷಿಕೆಗಳು;
  ಆದರೆ(ಶೀರ್ಷಿಕೆಗಳು && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_ದತ್ತಾಂಶ_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, ಶೂನ್ಯ *arg)
{
  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  cleanup_part_content(part);

  ಆದರೆ(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->ಬಗೆ = MIMEKIND_CALLBACK;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_ಗಣ_subparts(curl_mimepart *part,
                                curl_mime *subparts, ಪೂರ್ಣಾಂಕ take_ownership)
{
  curl_mime *ಬೇರು;

  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  /* Accept setting twice the same subparts. */
  ಆದರೆ(part->ಬಗೆ == MIMEKIND_MULTIPART && part->arg == subparts)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  cleanup_part_content(part);

  ಆದರೆ(subparts) {
    /* Should not have been attached already. */
    ಆದರೆ(subparts->ಪೋಷಕ)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

    /* Should not be the part's root. */
    ಬೇರು = part->ಪೋಷಕ;
    ಆದರೆ(ಬೇರು) {
      ಆಗ(ಬೇರು->ಪೋಷಕ && ಬೇರು->ಪೋಷಕ->ಪೋಷಕ)
        ಬೇರು = ಬೇರು->ಪೋಷಕ->ಪೋಷಕ;
      ಆದರೆ(subparts == ಬೇರು) {
        /* cannot add as a subpart of itself. */
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    ಆದರೆ(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      ಹಿಂತಿರುಗಿ CURLE_SEND_FAIL_REWIND;

    subparts->ಪೋಷಕ = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->ಬಗೆ = MIMEKIND_MULTIPART;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  ಹಿಂತಿರುಗಿ Curl_mime_ಗಣ_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, size_t nitems, ಶೂನ್ಯ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (ಶೂನ್ಯ)ಗಾತ್ರ_2;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  ಮಾಡು {
    hasread = FALSE;
    ret = readback_part(part, ಬಫರ್, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } ಆಗ(ret == STOP_FILLING);

  ಹಿಂತಿರುಗಿ ret;
}

/* Rewind mime stream. */
ಸ್ಥಿರ CURLcode mime_rewind(curl_mimepart *part)
{
  ಹಿಂತಿರುಗಿ mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
ಸ್ಥಿರ size_t slist_ಗಾತ್ರ(ರಚನೆ curl_slist *s,
                         size_t overhead, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *skip, size_t skiplen)
{
  size_t ಗಾತ್ರ_2 = 0;

  ಗಾಗಿ(; s; s = s->ಮುಂದಿನ)
    ಆದರೆ(!skip || !ಹೊಂದಿಕೆ_ಶೀರ್ಷಿಕೆ(s, skip, skiplen))
      ಗಾತ್ರ_2 += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s->ದತ್ತಾಂಶ) + overhead;
  ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/* Get/compute multipart size. */
ಸ್ಥಿರ curl_off_t multipart_ಗಾತ್ರ(curl_mime *mime)
{
  curl_off_t ಗಾತ್ರ_2;
  curl_off_t boundarysize;
  curl_mimepart *part;

  ಆದರೆ(!mime)
    ಹಿಂತಿರುಗಿ 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  ಗಾತ್ರ_2 = boundarysize;  /* Final boundary - CRLF after headers. */

  ಗಾಗಿ(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_ಗಾತ್ರ(part);

    ಆದರೆ(sz < 0)
      ಗಾತ್ರ_2 = sz;

    ಆದರೆ(ಗಾತ್ರ_2 >= 0)
      ಗಾತ್ರ_2 += boundarysize + sz;
  }

  ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/* Get/compute mime size. */
ಸ್ಥಿರ curl_off_t mime_ಗಾತ್ರ(curl_mimepart *part)
{
  curl_off_t ಗಾತ್ರ_2;

  ಆದರೆ(part->ಬಗೆ == MIMEKIND_MULTIPART)
    part->datasize = multipart_ಗಾತ್ರ(part->arg);

  ಗಾತ್ರ_2 = part->datasize;

  ಆದರೆ(part->encoder)
    ಗಾತ್ರ_2 = part->encoder->sizefunc(part);

  ಆದರೆ(ಗಾತ್ರ_2 >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    ಗಾತ್ರ_2 += slist_ಗಾತ್ರ(part->curlheaders, 2, ಖಾಲಿ, 0);
    ಗಾತ್ರ_2 += slist_ಗಾತ್ರ(part->userheaders, 2, STRCONST("Content-Type"));
    ಗಾತ್ರ_2 += 2;    /* CRLF after headers. */
  }
  ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_ಕೂಡಿಸು_ಶೀರ್ಷಿಕೆ(ರಚನೆ curl_slist **slp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  ರಚನೆ curl_slist *hdr = ಖಾಲಿ;
  ಅಕ್ಷರ *s = ಖಾಲಿ;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  ಆದರೆ(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    ಆದರೆ(hdr)
      *slp = hdr;
    ಇಲ್ಲದಿದ್ದರೆ
      curlx_free(s);
  }

  ಹಿಂತಿರುಗಿ hdr ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
}

/* Add a content type header. */
ಸ್ಥಿರ CURLcode ಕೂಡಿಸು_content_type(ರಚನೆ curl_slist **slp,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *boundary)
{
  ಹಿಂತಿರುಗಿ Curl_mime_ಕೂಡಿಸು_ಶೀರ್ಷಿಕೆ(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_mime_contenttype(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  ರಚನೆ ContentType {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type;
  };
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ ContentType ctts[] = {
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

  ಆದರೆ(filename) {
    size_t len1 = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(filename);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nameend = filename + len1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ctts[i].extension);

      ಆದರೆ(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        ಹಿಂತಿರುಗಿ ctts[i].type;
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ bool content_type_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *contenttype,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, size_t len)
{
  ಆದರೆ(contenttype && curl_strnequal(contenttype, ಗುರಿ, len))
    ಆಯ್ಕೆ(contenttype[len]) {
    ಸ್ಥಿತಿ '\0':
    ಸ್ಥಿತಿ '\t':
    ಸ್ಥಿತಿ '\r':
    ಸ್ಥಿತಿ '\n':
    ಸ್ಥಿತಿ ' ':
    ಸ್ಥಿತಿ ';':
      ಹಿಂತಿರುಗಿ TRUE;
    }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode ಕೂಡಿಸು_content_disposition(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        curl_mimepart *part,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *disposition,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *contenttype,
                                        ಎಣಿಕೆ mimestrategy strategy)
{
  ಆದರೆ(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  ಆದರೆ(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = ಖಾಲಿ;

  ಆದರೆ(disposition) {
    CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
    ಅಕ್ಷರ *name = ಖಾಲಿ;
    ಅಕ್ಷರ *filename = ಖಾಲಿ;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (ದತ್ತಾಂಶ && ದತ್ತಾಂಶ->ಗಣ.mime_formescape);
    ಆದರೆ(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

    ಆದರೆ(part->name) {
      name = escape_string(ದತ್ತಾಂಶ, part->name, strategy);
      ಆದರೆ(!name)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಆದರೆ(part->filename) {
      filename = escape_string(ದತ್ತಾಂಶ, part->filename, strategy);
      ಆದರೆ(!filename)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_mime_ಕೂಡಿಸು_ಶೀರ್ಷಿಕೆ(&part->curlheaders,
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
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     curl_mimepart *part,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *contenttype,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *disposition,
                                     ಎಣಿಕೆ mimestrategy strategy,
                                     size_t call_ಆಳ)
{
  curl_mime *mime = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *boundary = ಖಾಲಿ;
  ಅಕ್ಷರ *customct;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cte = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(++call_ಆಳ > MAX_MIME_ಮಟ್ಟಗಳು)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = ಖಾಲಿ;

  /* Be sure we will not access old headers later. */
  ಆದರೆ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, ಖಾಲಿ);

  /* Check if content type is specified. */
  customct = part->mimetype;
  ಆದರೆ(!customct)
    customct = ಹುಡುಕು_ಶೀರ್ಷಿಕೆ(part->userheaders, STRCONST("Content-Type"));
  ಆದರೆ(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  ಆದರೆ(!contenttype) {
    ಆಯ್ಕೆ(part->ಬಗೆ) {
    ಸ್ಥಿತಿ MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      ಆದರೆ(!contenttype)
        contenttype = Curl_mime_contenttype(part->ದತ್ತಾಂಶ);
      ಆದರೆ(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      contenttype = Curl_mime_contenttype(part->filename);
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(part->ಬಗೆ == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    ಆದರೆ(mime)
      boundary = mime->boundary;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(contenttype && !customct &&
          content_type_ಹೊಂದಿಕೆ(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = ಖಾಲಿ;

  /* Issue content-disposition header only if not already set by caller. */
  ಆದರೆ(!ಹುಡುಕು_ಶೀರ್ಷಿಕೆ(part->userheaders, STRCONST("Content-Disposition"))) {
    ಫಲಿತಾಂಶ = ಕೂಡಿಸು_content_disposition(ದತ್ತಾಂಶ, part, disposition,
                                     contenttype, strategy);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Issue Content-Type header. */
  ಆದರೆ(contenttype) {
    ಫಲಿತಾಂಶ = ಕೂಡಿಸು_content_type(&part->curlheaders, contenttype, boundary);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Content-Transfer-Encoding header. */
  ಆದರೆ(!ಹುಡುಕು_ಶೀರ್ಷಿಕೆ(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    ಆದರೆ(part->encoder)
      cte = part->encoder->name;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->ಬಗೆ != MIMEKIND_MULTIPART)
      cte = "8bit";
    ಆದರೆ(cte) {
      ಫಲಿತಾಂಶ = Curl_mime_ಕೂಡಿಸು_ಶೀರ್ಷಿಕೆ(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  ಆದರೆ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  ಆದರೆ(part->ಬಗೆ == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = ಖಾಲಿ;
    ಆದರೆ(content_type_ಹೊಂದಿಕೆ(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    ಗಾಗಿ(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      ಫಲಿತಾಂಶ = mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, subpart, ಖಾಲಿ,
                                    disposition, strategy, call_ಆಳ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   curl_mimepart *part,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *contenttype,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *disposition,
                                   ಎಣಿಕೆ mimestrategy strategy)
{
  ಹಿಂತಿರುಗಿ mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
ಸ್ಥಿರ ಶೂನ್ಯ mime_unpause(curl_mimepart *part)
{
  ಆದರೆ(part) {
    ಆದರೆ(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    ಆದರೆ(part->ಬಗೆ == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      ಆದರೆ(mime) {
        curl_mimepart *subpart;

        ಗಾಗಿ(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

ರಚನೆ cr_mime_ctx {
  ರಚನೆ Curl_creader super;
  curl_mimepart *part;
  curl_off_t ಒಟ್ಟು_len;
  curl_off_t read_len;
  CURLcode error_ಫಲಿತಾಂಶ;
  ರಚನೆ bufq tmpbuf;
  ಬಿಟ್(ನೋಡಿದ_eos);
  ಬಿಟ್(errored);
};

ಸ್ಥಿರ CURLcode cr_mime_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx->ಒಟ್ಟು_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_mime_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
ಸ್ಥಿರ CURLcode cr_mime_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_creader *reader,
                             ಅಕ್ಷರ *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  ರಚನೆ cr_mime_ctx *ctx = reader->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nread;
  ಅಕ್ಷರ tmp[256];

  /* Once we have errored, we will return the same error forever */
  ಆದರೆ(ctx->errored) {
    CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (ಪೂರ್ಣಾಂಕ)ctx->error_ಫಲಿತಾಂಶ);
    *pnread = 0;
    *peos = FALSE;
    ಹಿಂತಿರುಗಿ ctx->error_ಫಲಿತಾಂಶ;
  }
  ಆದರೆ(ctx->ನೋಡಿದ_eos) {
    CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  /* respect length limitations */
  ಆದರೆ(ctx->ಒಟ್ಟು_len >= 0) {
    curl_off_t remain = ctx->ಒಟ್ಟು_len - ctx->read_len;
    ಆದರೆ(remain <= 0)
      blen = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tmpbuf)) {
    ಫಲಿತಾಂಶ = Curl_bufq_read(&ctx->tmpbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf, blen, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ctx->errored = TRUE;
      ctx->error_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, ಗಾತ್ರ(tmp), ctx->part);
    ಆದರೆ(nread <= ಗಾತ್ರ(tmp)) {
      size_t n;
      ಫಲಿತಾಂಶ = Curl_bufq_write(&ctx->tmpbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)tmp, nread, &n);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        ctx->errored = TRUE;
        ctx->error_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      /* stored it, read again */
      ಫಲಿತಾಂಶ = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        ctx->errored = TRUE;
        ctx->error_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  ಆಯ್ಕೆ(nread) {
  ಸ್ಥಿತಿ 0:
    ಆದರೆ((ctx->ಒಟ್ಟು_len >= 0) && (ctx->read_len < ctx->ಒಟ್ಟು_len)) {
      failf(ದತ್ತಾಂಶ, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->ಒಟ್ಟು_len);
      ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->ನೋಡಿದ_eos = TRUE;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_READFUNC_ABORT:
    failf(ದತ್ತಾಂಶ, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
    ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    ಫಲಿತಾಂಶ = Curl_xfer_pause_send(ದತ್ತಾಂಶ, TRUE);
    ನಿಲ್ಲಿಸು; /* nothing was read */

  ಸ್ಥಿತಿ STOP_FILLING:
  ಸ್ಥಿತಿ READ_ERROR:
    failf(ದತ್ತಾಂಶ, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
    ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(nread > blen) {
      /* the read function returned a too large value */
      failf(ದತ್ತಾಂಶ, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    ಆದರೆ(ctx->ಒಟ್ಟು_len >= 0)
      ctx->ನೋಡಿದ_eos = (ctx->read_len >= ctx->ಒಟ್ಟು_len);
    *pnread = nread;
    *peos = (bool)ctx->ನೋಡಿದ_eos;
    ನಿಲ್ಲಿಸು;
  }

  CURL_TRC_READ(ದತ್ತಾಂಶ, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->ಒಟ್ಟು_len, ctx->read_len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread, *peos);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cr_mime_needs_rewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ ctx->read_len > 0;
}

ಸ್ಥಿರ curl_off_t cr_mime_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ ctx->ಒಟ್ಟು_len;
}

ಸ್ಥಿರ CURLcode cr_mime_resume_from(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ Curl_creader *reader,
                                    curl_off_t ಸ್ಥಾನಾಂತರ)
{
  ರಚನೆ cr_mime_ctx *ctx = reader->ctx;

  ಆದರೆ(ಸ್ಥಾನಾಂತರ > 0) {
    curl_off_t passed = 0;

    ಮಾಡು {
      ಅಕ್ಷರ scratch[4 * 1024];
      size_t readthisamountnow =
        (ಸ್ಥಾನಾಂತರ - passed > (curl_off_t)ಗಾತ್ರ(scratch)) ?
        ಗಾತ್ರ(scratch) :
        curlx_sotouz(ಸ್ಥಾನಾಂತರ - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      ಆದರೆ((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(ದತ್ತಾಂಶ, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
      }
    } ಆಗ(passed < ಸ್ಥಾನಾಂತರ);

    /* now, decrease the size of the read */
    ಆದರೆ(ctx->ಒಟ್ಟು_len > 0) {
      ctx->ಒಟ್ಟು_len -= ಸ್ಥಾನಾಂತರ;

      ಆದರೆ(ctx->ಒಟ್ಟು_len <= 0) {
        failf(ದತ್ತಾಂಶ, "Mime post already completely uploaded");
        ಹಿಂತಿರುಗಿ CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cr_mime_cntrl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  ರಚನೆ cr_mime_ctx *ctx = reader->ctx;
  ಆಯ್ಕೆ(opcode) {
  ಸ್ಥಿತಿ CURL_CRCNTRL_REWIND: {
    CURLcode ಫಲಿತಾಂಶ = mime_rewind(ctx->part);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      failf(ದತ್ತಾಂಶ, "Cannot rewind mime/post data");
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಸ್ಥಿತಿ CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_CRCNTRL_CLEAR_EOS:
    ctx->ನೋಡಿದ_eos = FALSE;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool cr_mime_is_paused(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_mime_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ cr_mime_ctx)
};

CURLcode Curl_creader_ಗಣ_mime(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_mimepart *part)
{
  ರಚನೆ Curl_creader *r;
  ರಚನೆ cr_mime_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_creader_create(&r, ದತ್ತಾಂಶ, &cr_mime, CURL_CR_CLIENT);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  ಫಲಿತಾಂಶ = mime_rewind(ctx->part);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    Curl_creader_free(ದತ್ತಾಂಶ, r);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ctx->ಒಟ್ಟು_len = mime_ಗಾತ್ರ(ctx->part);

  ಹಿಂತಿರುಗಿ Curl_creader_ಗಣ(ದತ್ತಾಂಶ, r);
}

#ಇಲ್ಲದಿದ್ದರೆ /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (ಶೂನ್ಯ)easy;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ curl_mime_free(curl_mime *mime)
{
  (ಶೂನ್ಯ)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (ಶೂನ್ಯ)mime;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURLcode curl_mime_name(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)name;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_mime_filename(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)filename;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_mime_type(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mimetype)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)mimetype;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *encoding)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)encoding;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_mime_ದತ್ತಾಂಶ(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, size_t datasize)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)datasize;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)filename;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_mime_ದತ್ತಾಂಶ_cb(curl_mimepart *part, curl_off_t datasize,
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
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)subparts;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode curl_mime_ಶೀರ್ಷಿಕೆಗಳು(curl_mimepart *part,
                           ರಚನೆ curl_slist *ಶೀರ್ಷಿಕೆಗಳು, ಪೂರ್ಣಾಂಕ take_ownership)
{
  (ಶೂನ್ಯ)part;
  (ಶೂನ್ಯ)ಶೀರ್ಷಿಕೆಗಳು;
  (ಶೂನ್ಯ)take_ownership;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

CURLcode Curl_mime_ಕೂಡಿಸು_ಶೀರ್ಷಿಕೆ(ರಚನೆ curl_slist **slp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)slp;
  (ಶೂನ್ಯ)fmt;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

#ಆದರೆ_ಅಂತ್ಯ /* if disabled */

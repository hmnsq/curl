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
#شامل "curl_setup.h"

ساخت Curl_easy;

#شامل "mime.h"
#شامل "urldata.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "transfer.h"
#شامل "curlx/strdup.h"
#شامل "curlx/basename.h"
#شامل "curlx/strcopy.h"
#شامل "curlx/win32-fopen.h"
#شامل "curlx/base64.h"

#اگر !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#شامل "rand.h"
#شامل "slist.h"
#شامل "curlx/dynbuf.h"

#تعریف_کرو MAX_MIME_سطحیں 40 /* avoid deep nesting */

#تعریف_کرو READ_ERROR   ((size_t)-1)
#تعریف_کرو STOP_FILLING ((size_t)-2)

ساکن size_t mime_subparts_read(حرف *بفر, size_t سائز, size_t nitems,
                                 خالی *instream, bool *hasread);
ساکن curl_off_t mime_سائز(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#تعریف_کرو QP_OK           1       /* Can be represented by itself. */
#تعریف_کرو QP_SP           2       /* Space or tab. */
#تعریف_کرو QP_CR           3       /* Carriage return. */
#تعریف_کرو QP_LF           4       /* Line-feed. */
ساکن ثابت غیرعلامتی حرف qp_class[] = {
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
ساکن ثابت حرف aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#اگر_غیر_تعریف_شدہ __VMS
#تعریف_کرو filesize(name, stat_ڈیٹا) stat_ڈیٹا.st_سائز
#تعریف_کرو fopen_read                curlx_fopen

#ورنہ

#شامل <fabdef.h>
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
curl_off_t Vms_Real_File_سائز(ثابت حرف *name, ثابت curlx_struct_stat *stat_buf)
{
  حرف بفر[8192];
  curl_off_t گنتی;
  صحیح ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  اگر(!file)
    واپس 0;

  گنتی = 0;
  ret_stat = 1;
  جب(ret_stat > 0) {
    ret_stat = fread(بفر, 1, حجم(بفر), file);
    اگر(ret_stat)
      گنتی += ret_stat;
  }
  curlx_fclose(file);

  واپس گنتی;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
ساکن curl_off_t Vms_Special_سائز(ثابت حرف *name,
                                 ثابت curlx_struct_stat *stat_buf)
{
  انتخاب(stat_buf->st_fab_rfm) {
  حالت FAB$C_VAR:
  حالت FAB$C_VFC:
    واپس Vms_Real_File_سائز(name, stat_buf);
    توڑو;
  طےشدہ:
    واپس stat_buf->st_سائز;
  }
}

#تعریف_کرو filesize(name, stat_ڈیٹا) Vms_Special_سائز(name, &stat_ڈیٹا)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
ساکن FILE *vmsfopenread(ثابت حرف *file, ثابت حرف *منوال)
{
  curlx_struct_stat statbuf;
  صحیح res = curlx_stat(file, &statbuf);

  اگر(res != -1) {
    انتخاب(statbuf.st_fab_rfm) {
    حالت FAB$C_VAR:
    حالت FAB$C_VFC:
    حالت FAB$C_STMCR:
      واپس curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      توڑو;
    طےشدہ:
      واپس curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  واپس تہی;
}

#تعریف_کرو fopen_read vmsfopenread
#اگر_ختم /* !__VMS */

/* Set readback state. */
ساکن خالی mimesetstate(ساخت mime_state *state,
                         شمار mimestate tok, خالی *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->انحراف = 0;
}

/* Escape header string into allocated memory. */
ساکن حرف *escape_string(ساخت Curl_easy *ڈیٹا,
                           ثابت حرف *src, شمار mimestrategy strategy)
{
  CURLcode نتیجہ;
  ساخت dynbuf db;
  ثابت حرف * ثابت *جدول;
  ثابت حرف * ثابت *p;
  /* replace first character by rest of string. */
  ساکن ثابت حرف * ثابت mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    تہی
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  ساکن ثابت حرف * ثابت formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    تہی
  };

  جدول = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  اگر(strategy == MIMESTRATEGY_MAIL || (ڈیٹا && ڈیٹا->سیٹ.mime_formescape))
    جدول = mimetable;

  curlx_dyn_init(&db, CURL_MAX_داخلہ_لمبائی);

  برائے(نتیجہ = curlx_dyn_addn(&db, STRCONST("")); !نتیجہ && *src; src++) {
    برائے(p = جدول; *p && **p != *src; p++)
      ;

    اگر(*p)
      نتیجہ = curlx_dyn_جوڑو(&db, *p + 1);
    ورنہ
      نتیجہ = curlx_dyn_addn(&db, src, 1);
  }

  واپس curlx_dyn_ptr(&db);
}

/* Check if header matches. */
ساکن حرف *مطابقت_سرخی(ساخت curl_slist *hdr, ثابت حرف *lbl, size_t len)
{
  حرف *قدر = تہی;

  اگر(curl_strnequal(hdr->ڈیٹا, lbl, len) && hdr->ڈیٹا[len] == ':')
    برائے(قدر = hdr->ڈیٹا + len + 1; *قدر == ' '; قدر++)
      ;
  واپس قدر;
}

/* Get a header from an slist. */
ساکن حرف *تلاش_سرخی(ساخت curl_slist *hdrlist,
                           ثابت حرف *hdr, size_t len)
{
  حرف *قدر = تہی;

  برائے(; !قدر && hdrlist; hdrlist = hdrlist->اگلا)
    قدر = مطابقت_سرخی(hdrlist, hdr, len);

  واپس قدر;
}

ساکن حرف *strippath(ثابت حرف *fullfile)
{
  حرف *filename;
  حرف *بنیاد;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  اگر(!filename)
    واپس تہی;
  بنیاد = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  واپس بنیاد; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
ساکن خالی cleanup_encoder_state(ساخت mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
ساکن size_t encoder_nop_read(حرف *بفر, size_t سائز, bool ateof,
                               ساخت curl_mimepart *part)
{
  ساخت mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (خالی)ateof;

  اگر(!سائز)
    واپس STOP_FILLING;

  اگر(سائز > insize)
    سائز = insize;

  اگر(سائز)
    حافظہ_نقل(بفر, st->buf + st->bufbeg, سائز);

  st->bufbeg += سائز;
  واپس سائز;
}

ساکن curl_off_t encoder_nop_سائز(curl_mimepart *part)
{
  واپس part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
ساکن size_t encoder_7bit_read(حرف *بفر, size_t سائز, bool ateof,
                                curl_mimepart *part)
{
  ساخت mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (خالی)ateof;

  اگر(!سائز)
    واپس STOP_FILLING;

  اگر(سائز > cursize)
    سائز = cursize;

  برائے(cursize = 0; cursize < سائز; cursize++) {
    *بفر = st->buf[st->bufbeg];
    اگر(*بفر++ & 0x80)
      واپس cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  واپس cursize;
}

/* Base64 content encoder. */
ساکن size_t encoder_base64_read(حرف *بفر, size_t سائز, bool ateof,
                                  curl_mimepart *part)
{
  ساخت mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  صحیح i;
  حرف *ptr = بفر;

  جب(st->bufbeg < st->bufend) {
    /* Line full ? */
    اگر(st->pos > MAX_ENCODED_سطر_لمبائی - 4) {
      /* Yes, we need 2 characters for CRLF. */
      اگر(سائز < 2) {
        اگر(!cursize)
          واپس STOP_FILLING;
        توڑو;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      سائز -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    اگر(سائز < 4) {
      اگر(!cursize)
        واپس STOP_FILLING;
      توڑو;
    }
    اگر(st->bufend - st->bufbeg < 3)
      توڑو;

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
    سائز -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  اگر(ateof) {
    اگر(سائز < 4) {
      اگر(!cursize)
        واپس STOP_FILLING;
    }
    ورنہ {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      اگر(st->bufend != st->bufbeg) {

        اگر(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        اگر(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  واپس cursize;
}

/* The maximum input size that does not cause an overflow. */
#تعریف_کرو BASE64_MAX_داخلہ_سائز                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_سطر_لمبائی + 2)) *                  \
    MAX_ENCODED_سطر_لمبائی / 4) * 3 - 3)

ساکن curl_off_t encoder_base64_سائز(curl_mimepart *part)
{
  curl_off_t سائز = part->datasize;

  اگر(سائز <= 0)
    واپس سائز;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  اگر(سائز > BASE64_MAX_داخلہ_سائز)
    واپس -1;

  /* Compute base64 character count. */
  سائز = 4 * (1 + ((سائز - 1) / 3));

  /* Effective character count must include CRLFs. */
  واپس سائز + (2 * ((سائز - 1) / MAX_ENCODED_سطر_لمبائی));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
ساکن صحیح qp_lookahead_eol(ساخت mime_encoder_state *st, صحیح ateof, size_t n)
{
  n += st->bufbeg;
  اگر(n >= st->bufend && ateof)
    واپس 1;
  اگر(n + 2 > st->bufend)
    واپس ateof ? 0 : -1;
  اگر(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    واپس 1;
  واپس 0;
}

/* Quoted-printable encoder. */
ساکن size_t encoder_qp_read(حرف *بفر, size_t سائز, bool ateof,
                              curl_mimepart *part)
{
  ساخت mime_encoder_state *st = &part->encstate;
  حرف *ptr = بفر;
  size_t cursize = 0;
  صحیح softlinebreak;
  حرف buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  جب(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    صحیح i = (غیرعلامتی حرف)st->buf[st->bufbeg];
    buf[0] = (حرف)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    انتخاب(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    حالت QP_OK:          /* Not a special character. */
      توڑو;
    حالت QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      انتخاب(qp_lookahead_eol(st, ateof, 1)) {
      حالت -1:          /* More input data needed. */
        واپس cursize;
      حالت 0:           /* No encoding needed. */
        توڑو;
      طےشدہ:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        توڑو;
      }
      توڑو;
    حالت QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      انتخاب(qp_lookahead_eol(st, ateof, 0)) {
      حالت -1:          /* Need more data. */
        واپس cursize;
      حالت 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        توڑو;
      طےشدہ:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        توڑو;
      }
      توڑو;
    طےشدہ:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      توڑو;
    }

    /* Be sure the encoded character fits within maximum line length. */
    اگر(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_سطر_لمبائی;
      اگر(!softlinebreak && st->pos + len == MAX_ENCODED_سطر_لمبائی) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        انتخاب(qp_lookahead_eol(st, ateof, consumed)) {
        حالت -1:        /* Need more data. */
          واپس cursize;
        حالت 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          توڑو;
        }
      }
      اگر(softlinebreak) {
        curlx_strcopy(buf, حجم(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    اگر(len > سائز) {
      اگر(!cursize)
        واپس STOP_FILLING;
      توڑو;
    }

    /* Append to output buffer. */
    حافظہ_نقل(ptr, buf, len);
    cursize += len;
    ptr += len;
    سائز -= len;
    st->pos += len;
    اگر(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  واپس cursize;
}

ساکن curl_off_t encoder_qp_سائز(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  واپس part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
ساکن size_t mime_mem_read(حرف *بفر, size_t سائز, size_t nitems,
                            خالی *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.انحراف);
  (خالی)سائز;  /* Always 1 */

  اگر(!nitems)
    واپس STOP_FILLING;

  اگر(sz > nitems)
    sz = nitems;

  اگر(sz)
    حافظہ_نقل(بفر, part->ڈیٹا + curlx_sotouz(part->state.انحراف), sz);

  واپس sz;
}

ساکن صحیح mime_mem_seek(خالی *instream, curl_off_t انحراف, صحیح whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  انتخاب(whence) {
  حالت SEEK_CUR:
    انحراف += part->state.انحراف;
    توڑو;
  حالت SEEK_END:
    انحراف += part->datasize;
    توڑو;
  }

  اگر(انحراف < 0 || انحراف > part->datasize)
    واپس CURL_SEEKFUNC_FAIL;

  part->state.انحراف = انحراف;
  واپس CURL_SEEKFUNC_OK;
}

ساکن خالی mime_mem_free(خالی *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->ڈیٹا);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
ساکن bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  اگر(part->fp)
    واپس FALSE;
  part->fp = fopen_read(part->ڈیٹا, "rb");
  واپس part->fp ? FALSE : TRUE;
}

ساکن size_t mime_file_read(حرف *بفر, size_t سائز, size_t nitems,
                             خالی *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  اگر(!nitems)
    واپس STOP_FILLING;

  اگر(mime_open_file(part))
    واپس READ_ERROR;

  واپس fread(بفر, سائز, nitems, part->fp);
}

ساکن صحیح mime_file_seek(خالی *instream, curl_off_t انحراف, صحیح whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  اگر(whence == SEEK_SET && !انحراف && !part->fp)
    واپس CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  اگر(mime_open_file(part))
    واپس CURL_SEEKFUNC_FAIL;

  واپس curlx_fseek(part->fp, انحراف, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

ساکن خالی mime_file_free(خالی *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  اگر(part->fp) {
    curlx_fclose(part->fp);
    part->fp = تہی;
  }
  curlx_safefree(part->ڈیٹا);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
ساکن size_t readback_بائٹیں(ساخت mime_state *state,
                             حرف *بفر, size_t bufsize,
                             ثابت حرف *بائٹیں, size_t numbytes,
                             ثابت حرف *trail, size_t traillen)
{
  size_t sz;
  size_t انحراف = curlx_sotouz(state->انحراف);

  اگر(numbytes > انحراف) {
    sz = numbytes - انحراف;
    بائٹیں += انحراف;
  }
  ورنہ {
    sz = انحراف - numbytes;
    اگر(sz >= traillen)
      واپس 0;
    بائٹیں = trail + sz;
    sz = traillen - sz;
  }

  اگر(sz > bufsize)
    sz = bufsize;

  حافظہ_نقل(بفر, بائٹیں, sz);
  state->انحراف += sz;
  واپس sz;
}

/* Read a non-encoded part content. */

ساکن size_t read_part_content(curl_mimepart *part, حرف *بفر,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  انتخاب(part->lastreadstatus) {
  حالت 0:
  حالت CURL_READFUNC_ABORT:
  حالت CURL_READFUNC_PAUSE:
  حالت READ_ERROR:
    واپس part->lastreadstatus;
  طےشدہ:
    توڑو;
  }

  /* If we can determine we are at end of part data, spare a read. */
  اگر(part->datasize != (curl_off_t)-1 &&
     part->state.انحراف >= part->datasize) {
    /* sz is already zero. */
  }
  ورنہ {
    انتخاب(part->قسم) {
    حالت MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(بفر, 1, bufsize, part->arg, hasread);
      توڑو;
    حالت MIMEKIND_FILE:
      اگر(part->fp && feof(part->fp))
        توڑو;  /* At EOF. */
      FALLTHROUGH();
    طےشدہ:
      اگر(part->readfunc) {
        اگر(!(part->flags & MIME_FAST_READ)) {
          اگر(*hasread)
            واپس STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(بفر, 1, bufsize, part->arg);
      }
      توڑو;
    }
  }

  انتخاب(sz) {
  حالت STOP_FILLING:
    توڑو;
  حالت 0:
  حالت CURL_READFUNC_ABORT:
  حالت CURL_READFUNC_PAUSE:
  حالت READ_ERROR:
    part->lastreadstatus = sz;
    توڑو;
  طےشدہ:
    part->state.انحراف += sz;
    part->lastreadstatus = sz;
    توڑو;
  }

  واپس sz;
}

/* Read and encode part content. */
ساکن size_t read_encoded_part_content(curl_mimepart *part, حرف *بفر,
                                        size_t bufsize, bool *hasread)
{
  ساخت mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  برائے(;;) {
    اگر(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(بفر, bufsize, ateof, part);
      انتخاب(sz) {
      حالت 0:
        اگر(ateof)
          واپس cursize;
        توڑو;
      حالت READ_ERROR:
      حالت STOP_FILLING:
        واپس cursize ? cursize : sz;
      طےشدہ:
        cursize += sz;
        بفر += sz;
        bufsize -= sz;
        جاری;
      }
    }

    /* We need more data in input buffer. */
    اگر(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      اگر(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    اگر(st->bufend >= حجم(st->buf))
      واپس cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           حجم(st->buf) - st->bufend, hasread);
    انتخاب(sz) {
    حالت 0:
      ateof = TRUE;
      توڑو;
    حالت CURL_READFUNC_ABORT:
    حالت CURL_READFUNC_PAUSE:
    حالت READ_ERROR:
    حالت STOP_FILLING:
      واپس cursize ? cursize : sz;
    طےشدہ:
      st->bufend += sz;
      توڑو;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
ساکن size_t readback_part(curl_mimepart *part,
                            حرف *بفر, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  جب(bufsize) {
    size_t sz = 0;
    ساخت curl_slist *hdr = (ساخت curl_slist *)part->state.ptr;
    انتخاب(part->state.state) {
    حالت MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      توڑو;
    حالت MIMESTATE_USERHEADERS:
      اگر(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, تہی);
        توڑو;
      }
      اگر(مطابقت_سرخی(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->اگلا);
        توڑو;
      }
      FALLTHROUGH();
    حالت MIMESTATE_CURLHEADERS:
      اگر(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      ورنہ {
        sz = readback_بائٹیں(&part->state, بفر, bufsize,
                            hdr->ڈیٹا, لڑی_لمبائی(hdr->ڈیٹا), STRCONST("\r\n"));
        اگر(!sz)
          mimesetstate(&part->state, part->state.state, hdr->اگلا);
      }
      توڑو;
    حالت MIMESTATE_EOH:
      sz = readback_بائٹیں(&part->state, بفر, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      اگر(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, تہی);
      توڑو;
    حالت MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, تہی);
      توڑو;
    حالت MIMESTATE_CONTENT:
      اگر(part->encoder)
        sz = read_encoded_part_content(part, بفر, bufsize, hasread);
      ورنہ
        sz = read_part_content(part, بفر, bufsize, hasread);
      انتخاب(sz) {
      حالت 0:
        mimesetstate(&part->state, MIMESTATE_انتہا, تہی);
        /* Try sparing open file descriptors. */
        اگر(part->قسم == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = تہی;
        }
        FALLTHROUGH();
      حالت CURL_READFUNC_ABORT:
      حالت CURL_READFUNC_PAUSE:
      حالت READ_ERROR:
      حالت STOP_FILLING:
        واپس cursize ? cursize : sz;
      }
      توڑو;
    حالت MIMESTATE_انتہا:
      واپس cursize;
    طےشدہ:
      توڑو;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    بفر += sz;
    bufsize -= sz;
  }

  واپس cursize;
}

/* Readback from mime. Warning: not a read callback function. */
ساکن size_t mime_subparts_read(حرف *بفر, size_t سائز, size_t nitems,
                                 خالی *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (خالی)سائز;  /* Always 1 */

  جب(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    انتخاب(mime->state.state) {
    حالت MIMESTATE_BEGIN:
    حالت MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.انحراف += 2;
      توڑو;
    حالت MIMESTATE_BOUNDARY1:
      sz = readback_بائٹیں(&mime->state, بفر, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      اگر(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      توڑو;
    حالت MIMESTATE_BOUNDARY2:
      اگر(part)
        sz = readback_بائٹیں(&mime->state, بفر, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      ورنہ
        sz = readback_بائٹیں(&mime->state, بفر, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      اگر(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      توڑو;
    حالت MIMESTATE_CONTENT:
      اگر(!part) {
        mimesetstate(&mime->state, MIMESTATE_انتہا, تہی);
        توڑو;
      }
      sz = readback_part(part, بفر, nitems, hasread);
      انتخاب(sz) {
      حالت CURL_READFUNC_ABORT:
      حالت CURL_READFUNC_PAUSE:
      حالت READ_ERROR:
      حالت STOP_FILLING:
        واپس cursize ? cursize : sz;
      حالت 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        توڑو;
      }
      توڑو;
    حالت MIMESTATE_انتہا:
      واپس cursize;
    طےشدہ:
      توڑو;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    بفر += sz;
    nitems -= sz;
  }

  واپس cursize;
}

ساکن صحیح mime_part_rewind(curl_mimepart *part)
{
  صحیح res = CURL_SEEKFUNC_OK;
  شمار mimestate targetstate = MIMESTATE_BEGIN;

  اگر(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  اگر(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    اگر(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      انتخاب(res) {
      حالت CURL_SEEKFUNC_OK:
      حالت CURL_SEEKFUNC_FAIL:
      حالت CURL_SEEKFUNC_CANTSEEK:
        توڑو;
      حالت -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        توڑو;
      طےشدہ:
        res = CURL_SEEKFUNC_FAIL;
        توڑو;
      }
    }
  }

  اگر(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, تہی);

  part->lastreadstatus = 1; /* Successful read status. */
  واپس res;
}

ساکن صحیح mime_subparts_seek(خالی *instream, curl_off_t انحراف, صحیح whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  صحیح rc = CURL_SEEKFUNC_OK;

  اگر(whence != SEEK_SET || انحراف)
    واپس CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  اگر(mime->state.state == MIMESTATE_BEGIN)
    واپس CURL_SEEKFUNC_OK;           /* Already rewound. */

  برائے(part = mime->firstpart; part; part = part->nextpart) {
    صحیح res = mime_part_rewind(part);
    اگر(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  اگر(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, تہی);

  واپس rc;
}

/* Release part content. */
ساکن خالی cleanup_part_content(curl_mimepart *part)
{
  اگر(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = تہی;
  part->seekfunc = تہی;
  part->freefunc = تہی;
  part->arg = (خالی *)part;           /* Defaults to part itself. */
  part->ڈیٹا = تہی;
  part->fp = تہی;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->قسم = MIMEKIND_NONE;
  part->flags &= ~(غیرعلامتی صحیح)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

ساکن خالی mime_subparts_free(خالی *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  اگر(mime && mime->والد) {
    mime->والد->freefunc = تہی;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->والد);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
ساکن خالی mime_subparts_unbind(خالی *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  اگر(mime && mime->والد) {
    mime->والد->freefunc = تہی;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->والد);  /* Avoid dangling pointer in part. */
    mime->والد = تہی;
  }
}

خالی Curl_mime_cleanpart(curl_mimepart *part)
{
  اگر(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    اگر(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
خالی curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  اگر(!mime)
    واپس;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  جب(mime) {
    part = mime->firstpart;
    اگر(part) {
      mime->firstpart = part->nextpart;
      اگر(part->قسم == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = تہی;
        cleanup_part_content(part);
        subparts->والد = mime->والد;
        mime->والد = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    ورنہ {
      curl_mime *والد = (curl_mime *)mime->والد;
      curlx_free(mime);
      mime = والد;
    }
  }
}

ساکن bool mime_is_too_deep(ثابت curl_mimepart *part, size_t call_گہرائی)
{
  DEBUGASSERT(part);

  اگر(++call_گہرائی > MAX_MIME_سطحیں)
    واپس TRUE;

  اگر(part->قسم == MIMEKIND_MULTIPART && part->arg) {
    ثابت curl_mime *mime = (ثابت curl_mime *)part->arg;
    ثابت curl_mimepart *s;

    برائے(s = mime->firstpart; s; s = s->nextpart) {
      اگر(mime_is_too_deep(s, call_گہرائی))
        واپس TRUE;
    }
  }

  واپس FALSE;
}

ساکن CURLcode mime_duppart(ساخت Curl_easy *ڈیٹا,
                             curl_mimepart *dst, ثابت curl_mimepart *src,
                             size_t call_گہرائی)
{
  curl_mime *mime;
  curl_mimepart *d;
  ثابت curl_mimepart *s;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  اگر(++call_گہرائی > MAX_MIME_سطحیں)
    واپس CURLE_TOO_LARGE;

  /* Duplicate content. */
  انتخاب(src->قسم) {
  حالت MIMEKIND_NONE:
    توڑو;
  حالت MIMEKIND_ڈیٹا:
    نتیجہ = curl_mime_ڈیٹا(dst, src->ڈیٹا, (size_t)src->datasize);
    توڑو;
  حالت MIMEKIND_FILE:
    نتیجہ = curl_mime_filedata(dst, src->ڈیٹا);
    /* Do not abort duplication if file is not readable. */
    اگر(نتیجہ == CURLE_READ_ERROR)
      نتیجہ = CURLE_OK;
    توڑو;
  حالت MIMEKIND_CALLBACK:
    نتیجہ = curl_mime_ڈیٹا_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    توڑو;
  حالت MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(ڈیٹا);
    نتیجہ = mime ? curl_mime_subparts(dst, mime) : CURLE_برآمد_کا_حافظہ;

    /* Duplicate subparts. */
    برائے(s = ((curl_mime *)src->arg)->firstpart; !نتیجہ && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      نتیجہ = d ? mime_duppart(ڈیٹا, d, s, call_گہرائی) : CURLE_برآمد_کا_حافظہ;
    }
    توڑو;
  طےشدہ:  /* Invalid kind: should not occur. */
    DEBUGF(infof(ڈیٹا, "invalid MIMEKIND* attempt"));
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;  /* Internal error? */
    توڑو;
  }

  /* Duplicate headers. */
  اگر(!نتیجہ && src->userheaders) {
    ساخت curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    اگر(!hdrs)
      نتیجہ = CURLE_برآمد_کا_حافظہ;
    ورنہ {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      نتیجہ = curl_mime_سرخیاں(dst, hdrs, TRUE);
      اگر(نتیجہ)
        curl_slist_free_all(hdrs);
    }
  }

  اگر(!نتیجہ) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    نتیجہ = curl_mime_type(dst, src->mimetype);
  }
  اگر(!نتیجہ)
    نتیجہ = curl_mime_name(dst, src->name);
  اگر(!نتیجہ)
    نتیجہ = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  اگر(نتیجہ)
    Curl_mime_cleanpart(dst);

  واپس نتیجہ;
}

CURLcode Curl_mime_duppart(ساخت Curl_easy *ڈیٹا,
                           curl_mimepart *dst, ثابت curl_mimepart *src)
{
  اگر(!dst || !src)
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(mime_is_too_deep(src, 0))
    واپس CURLE_TOO_LARGE;

  واپس mime_duppart(ڈیٹا, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(خالی *easy)
{
  curl_mime *mime = curlx_malloc(حجم(*mime));

  اگر(mime) {
    mime->والد = تہی;
    mime->firstpart = تہی;
    mime->lastpart = تہی;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    اگر(Curl_rand_alnum(easy,
                       (غیرعلامتی حرف *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      واپس تہی;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, تہی);
  }

  واپس mime;
}

/* Initialize a mime part. */
خالی Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, حجم(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, تہی);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  اگر(!mime)
    واپس تہی;

  part = curlx_malloc(حجم(*part));

  اگر(part) {
    Curl_mime_initpart(part);
    part->والد = mime;

    اگر(mime->lastpart)
      mime->lastpart->nextpart = part;
    ورنہ
      mime->firstpart = part;

    mime->lastpart = part;
  }

  واپس part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ثابت حرف *name)
{
  اگر(!part)
    واپس CURLE_BAD_FUNCTION_دلیل;

  curlx_safefree(part->name);

  اگر(name) {
    part->name = curlx_strdup(name);
    اگر(!part->name)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  واپس CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ثابت حرف *filename)
{
  اگر(!part)
    واپس CURLE_BAD_FUNCTION_دلیل;

  curlx_safefree(part->filename);

  اگر(filename) {
    part->filename = curlx_strdup(filename);
    اگر(!part->filename)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  واپس CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_ڈیٹا(curl_mimepart *part, ثابت حرف *ڈیٹا, size_t datasize)
{
  اگر(!part)
    واپس CURLE_BAD_FUNCTION_دلیل;

  cleanup_part_content(part);

  اگر(ڈیٹا) {
    اگر(datasize == CURL_ZERO_TERMINATED)
      datasize = لڑی_لمبائی(ڈیٹا);

    part->ڈیٹا = curlx_memdup0(ڈیٹا, datasize);
    اگر(!part->ڈیٹا)
      واپس CURLE_برآمد_کا_حافظہ;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->قسم = MIMEKIND_ڈیٹا;
  }

  واپس CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ثابت حرف *filename)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(!part)
    واپس CURLE_BAD_FUNCTION_دلیل;

  cleanup_part_content(part);

  اگر(filename) {
    حرف *بنیاد;
    curlx_struct_stat sbuf;

    اگر(curlx_stat(filename, &sbuf))
      نتیجہ = CURLE_READ_ERROR;
    ورنہ {
      part->ڈیٹا = curlx_strdup(filename);
      اگر(!part->ڈیٹا)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      ورنہ {
        part->datasize = -1;
        اگر(S_ISREG(sbuf.st_منوال)) {
          part->datasize = filesize(filename, sbuf);
          part->seekfunc = mime_file_seek;
        }

        part->readfunc = mime_file_read;
        part->freefunc = mime_file_free;
        part->قسم = MIMEKIND_FILE;

        /* As a side effect, set the filename to the current file's base name.
           It is possible to withdraw this by explicitly calling
           curl_mime_filename() with a NULL filename argument after the current
           call. */
        بنیاد = strippath(filename);
        اگر(!بنیاد)
          نتیجہ = CURLE_برآمد_کا_حافظہ;
        ورنہ {
          نتیجہ = curl_mime_filename(part, بنیاد);
          curlx_free(بنیاد);
        }
      }
    }
  }
  واپس نتیجہ;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ثابت حرف *mimetype)
{
  اگر(!part)
    واپس CURLE_BAD_FUNCTION_دلیل;

  curlx_safefree(part->mimetype);

  اگر(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    اگر(!part->mimetype)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  واپس CURLE_OK;
}

ساکن ثابت ساخت mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_سائز },
  { "8bit", encoder_nop_read, encoder_nop_سائز },
  { "7bit", encoder_7bit_read, encoder_nop_سائز },
  { "base64", encoder_base64_read, encoder_base64_سائز },
  { "quoted-printable", encoder_qp_read, encoder_qp_سائز },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ثابت حرف *encoding)
{
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
  ثابت ساخت mime_encoder *mep;

  اگر(!part)
    واپس نتیجہ;

  part->encoder = تہی;

  اگر(!encoding)
    واپس CURLE_OK;    /* Removing current encoder. */

  برائے(mep = encoders; mep->name; mep++)
    اگر(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      نتیجہ = CURLE_OK;
    }

  واپس نتیجہ;
}

/* Set mime part headers. */
CURLcode curl_mime_سرخیاں(curl_mimepart *part,
                           ساخت curl_slist *سرخیاں, صحیح take_ownership)
{
  اگر(!part)
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(part->flags & MIME_USERHEADERS_OWNER) {
    اگر(part->userheaders != سرخیاں)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(غیرعلامتی صحیح)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = سرخیاں;
  اگر(سرخیاں && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  واپس CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_ڈیٹا_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, خالی *arg)
{
  اگر(!part)
    واپس CURLE_BAD_FUNCTION_دلیل;

  cleanup_part_content(part);

  اگر(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->قسم = MIMEKIND_CALLBACK;
  }

  واپس CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_سیٹ_subparts(curl_mimepart *part,
                                curl_mime *subparts, صحیح take_ownership)
{
  curl_mime *جڑ;

  اگر(!part)
    واپس CURLE_BAD_FUNCTION_دلیل;

  /* Accept setting twice the same subparts. */
  اگر(part->قسم == MIMEKIND_MULTIPART && part->arg == subparts)
    واپس CURLE_OK;

  cleanup_part_content(part);

  اگر(subparts) {
    /* Should not have been attached already. */
    اگر(subparts->والد)
      واپس CURLE_BAD_FUNCTION_دلیل;

    /* Should not be the part's root. */
    جڑ = part->والد;
    اگر(جڑ) {
      جب(جڑ->والد && جڑ->والد->والد)
        جڑ = جڑ->والد->والد;
      اگر(subparts == جڑ) {
        /* cannot add as a subpart of itself. */
        واپس CURLE_BAD_FUNCTION_دلیل;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    اگر(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      واپس CURLE_SEND_FAIL_REWIND;

    subparts->والد = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->قسم = MIMEKIND_MULTIPART;
  }

  واپس CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  واپس Curl_mime_سیٹ_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(حرف *بفر, size_t سائز, size_t nitems, خالی *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (خالی)سائز;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  کرو {
    hasread = FALSE;
    ret = readback_part(part, بفر, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } جب(ret == STOP_FILLING);

  واپس ret;
}

/* Rewind mime stream. */
ساکن CURLcode mime_rewind(curl_mimepart *part)
{
  واپس mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
ساکن size_t slist_سائز(ساخت curl_slist *s,
                         size_t overhead, ثابت حرف *skip, size_t skiplen)
{
  size_t سائز = 0;

  برائے(; s; s = s->اگلا)
    اگر(!skip || !مطابقت_سرخی(s, skip, skiplen))
      سائز += لڑی_لمبائی(s->ڈیٹا) + overhead;
  واپس سائز;
}

/* Get/compute multipart size. */
ساکن curl_off_t multipart_سائز(curl_mime *mime)
{
  curl_off_t سائز;
  curl_off_t boundarysize;
  curl_mimepart *part;

  اگر(!mime)
    واپس 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  سائز = boundarysize;  /* Final boundary - CRLF after headers. */

  برائے(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_سائز(part);

    اگر(sz < 0)
      سائز = sz;

    اگر(سائز >= 0)
      سائز += boundarysize + sz;
  }

  واپس سائز;
}

/* Get/compute mime size. */
ساکن curl_off_t mime_سائز(curl_mimepart *part)
{
  curl_off_t سائز;

  اگر(part->قسم == MIMEKIND_MULTIPART)
    part->datasize = multipart_سائز(part->arg);

  سائز = part->datasize;

  اگر(part->encoder)
    سائز = part->encoder->sizefunc(part);

  اگر(سائز >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    سائز += slist_سائز(part->curlheaders, 2, تہی, 0);
    سائز += slist_سائز(part->userheaders, 2, STRCONST("Content-Type"));
    سائز += 2;    /* CRLF after headers. */
  }
  واپس سائز;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_جوڑو_سرخی(ساخت curl_slist **slp, ثابت حرف *fmt, ...)
{
  ساخت curl_slist *hdr = تہی;
  حرف *s = تہی;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  اگر(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    اگر(hdr)
      *slp = hdr;
    ورنہ
      curlx_free(s);
  }

  واپس hdr ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
}

/* Add a content type header. */
ساکن CURLcode جوڑو_content_type(ساخت curl_slist **slp,
                                 ثابت حرف *type, ثابت حرف *boundary)
{
  واپس Curl_mime_جوڑو_سرخی(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ثابت حرف *Curl_mime_contenttype(ثابت حرف *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  ساخت ContentType {
    ثابت حرف *extension;
    ثابت حرف *type;
  };
  ساکن ثابت ساخت ContentType ctts[] = {
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

  اگر(filename) {
    size_t len1 = لڑی_لمبائی(filename);
    ثابت حرف *nameend = filename + len1;
    غیرعلامتی صحیح i;

    برائے(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = لڑی_لمبائی(ctts[i].extension);

      اگر(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        واپس ctts[i].type;
    }
  }
  واپس تہی;
}

ساکن bool content_type_مطابقت(ثابت حرف *contenttype,
                               ثابت حرف *ہدف, size_t len)
{
  اگر(contenttype && curl_strnequal(contenttype, ہدف, len))
    انتخاب(contenttype[len]) {
    حالت '\0':
    حالت '\t':
    حالت '\r':
    حالت '\n':
    حالت ' ':
    حالت ';':
      واپس TRUE;
    }
  واپس FALSE;
}

ساکن CURLcode جوڑو_content_disposition(ساخت Curl_easy *ڈیٹا,
                                        curl_mimepart *part,
                                        ثابت حرف *disposition,
                                        ثابت حرف *contenttype,
                                        شمار mimestrategy strategy)
{
  اگر(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  اگر(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = تہی;

  اگر(disposition) {
    CURLcode نتیجہ = CURLE_OK;
    حرف *name = تہی;
    حرف *filename = تہی;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (ڈیٹا && ڈیٹا->سیٹ.mime_formescape);
    اگر(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      واپس CURLE_BAD_FUNCTION_دلیل;

    اگر(part->name) {
      name = escape_string(ڈیٹا, part->name, strategy);
      اگر(!name)
        واپس CURLE_برآمد_کا_حافظہ;
    }
    اگر(part->filename) {
      filename = escape_string(ڈیٹا, part->filename, strategy);
      اگر(!filename)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
    }
    اگر(!نتیجہ)
      نتیجہ = Curl_mime_جوڑو_سرخی(&part->curlheaders,
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
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس CURLE_OK;
}

ساکن CURLcode mime_prepare_سرخیاں(ساخت Curl_easy *ڈیٹا,
                                     curl_mimepart *part,
                                     ثابت حرف *contenttype,
                                     ثابت حرف *disposition,
                                     شمار mimestrategy strategy,
                                     size_t call_گہرائی)
{
  curl_mime *mime = تہی;
  ثابت حرف *boundary = تہی;
  حرف *customct;
  ثابت حرف *cte = تہی;
  CURLcode نتیجہ = CURLE_OK;

  اگر(++call_گہرائی > MAX_MIME_سطحیں)
    واپس CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = تہی;

  /* Be sure we will not access old headers later. */
  اگر(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, تہی);

  /* Check if content type is specified. */
  customct = part->mimetype;
  اگر(!customct)
    customct = تلاش_سرخی(part->userheaders, STRCONST("Content-Type"));
  اگر(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  اگر(!contenttype) {
    انتخاب(part->قسم) {
    حالت MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      توڑو;
    حالت MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      اگر(!contenttype)
        contenttype = Curl_mime_contenttype(part->ڈیٹا);
      اگر(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      توڑو;
    طےشدہ:
      contenttype = Curl_mime_contenttype(part->filename);
      توڑو;
    }
  }

  اگر(part->قسم == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    اگر(mime)
      boundary = mime->boundary;
  }
  ورنہ اگر(contenttype && !customct &&
          content_type_مطابقت(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = تہی;

  /* Issue content-disposition header only if not already set by caller. */
  اگر(!تلاش_سرخی(part->userheaders, STRCONST("Content-Disposition"))) {
    نتیجہ = جوڑو_content_disposition(ڈیٹا, part, disposition,
                                     contenttype, strategy);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* Issue Content-Type header. */
  اگر(contenttype) {
    نتیجہ = جوڑو_content_type(&part->curlheaders, contenttype, boundary);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* Content-Transfer-Encoding header. */
  اگر(!تلاش_سرخی(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    اگر(part->encoder)
      cte = part->encoder->name;
    ورنہ اگر(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->قسم != MIMEKIND_MULTIPART)
      cte = "8bit";
    اگر(cte) {
      نتیجہ = Curl_mime_جوڑو_سرخی(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  اگر(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  اگر(part->قسم == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = تہی;
    اگر(content_type_مطابقت(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    برائے(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      نتیجہ = mime_prepare_سرخیاں(ڈیٹا, subpart, تہی,
                                    disposition, strategy, call_گہرائی);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }
  واپس نتیجہ;
}

CURLcode Curl_mime_prepare_سرخیاں(ساخت Curl_easy *ڈیٹا,
                                   curl_mimepart *part,
                                   ثابت حرف *contenttype,
                                   ثابت حرف *disposition,
                                   شمار mimestrategy strategy)
{
  واپس mime_prepare_سرخیاں(ڈیٹا, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
ساکن خالی mime_unpause(curl_mimepart *part)
{
  اگر(part) {
    اگر(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    اگر(part->قسم == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      اگر(mime) {
        curl_mimepart *subpart;

        برائے(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

ساخت cr_mime_ctx {
  ساخت Curl_creader super;
  curl_mimepart *part;
  curl_off_t کل_len;
  curl_off_t read_len;
  CURLcode error_نتیجہ;
  ساخت bufq tmpbuf;
  ثنائی_ہندسہ(نظر_آیا_eos);
  ثنائی_ہندسہ(errored);
};

ساکن CURLcode cr_mime_init(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_creader *reader)
{
  ساخت cr_mime_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  ctx->کل_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  واپس CURLE_OK;
}

ساکن خالی cr_mime_close(ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_creader *reader)
{
  ساخت cr_mime_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
ساکن CURLcode cr_mime_read(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_creader *reader,
                             حرف *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  ساخت cr_mime_ctx *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;
  حرف tmp[256];

  /* Once we have errored, we will return the same error forever */
  اگر(ctx->errored) {
    CURL_TRC_READ(ڈیٹا, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (صحیح)ctx->error_نتیجہ);
    *pnread = 0;
    *peos = FALSE;
    واپس ctx->error_نتیجہ;
  }
  اگر(ctx->نظر_آیا_eos) {
    CURL_TRC_READ(ڈیٹا, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    واپس CURLE_OK;
  }
  /* respect length limitations */
  اگر(ctx->کل_len >= 0) {
    curl_off_t remain = ctx->کل_len - ctx->read_len;
    اگر(remain <= 0)
      blen = 0;
    ورنہ اگر(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->tmpbuf)) {
    نتیجہ = Curl_bufq_read(&ctx->tmpbuf, (غیرعلامتی حرف *)buf, blen, &nread);
    اگر(نتیجہ) {
      ctx->errored = TRUE;
      ctx->error_نتیجہ = نتیجہ;
      واپس نتیجہ;
    }
  }
  ورنہ اگر(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(ڈیٹا, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, حجم(tmp), ctx->part);
    اگر(nread <= حجم(tmp)) {
      size_t n;
      نتیجہ = Curl_bufq_write(&ctx->tmpbuf, (غیرعلامتی حرف *)tmp, nread, &n);
      اگر(نتیجہ) {
        ctx->errored = TRUE;
        ctx->error_نتیجہ = نتیجہ;
        واپس نتیجہ;
      }
      /* stored it, read again */
      نتیجہ = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      اگر(نتیجہ) {
        ctx->errored = TRUE;
        ctx->error_نتیجہ = نتیجہ;
        واپس نتیجہ;
      }
    }
  }
  ورنہ
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(ڈیٹا, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  انتخاب(nread) {
  حالت 0:
    اگر((ctx->کل_len >= 0) && (ctx->read_len < ctx->کل_len)) {
      failf(ڈیٹا, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->کل_len);
      واپس CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->نظر_آیا_eos = TRUE;
    توڑو;

  حالت CURL_READFUNC_ABORT:
    failf(ڈیٹا, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    توڑو;

  حالت CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(ڈیٹا, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    نتیجہ = Curl_xfer_pause_send(ڈیٹا, TRUE);
    توڑو; /* nothing was read */

  حالت STOP_FILLING:
  حالت READ_ERROR:
    failf(ڈیٹا, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_نتیجہ = CURLE_READ_ERROR;
    نتیجہ = CURLE_READ_ERROR;
    توڑو;

  طےشدہ:
    اگر(nread > blen) {
      /* the read function returned a too large value */
      failf(ڈیٹا, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_نتیجہ = CURLE_READ_ERROR;
      واپس CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    اگر(ctx->کل_len >= 0)
      ctx->نظر_آیا_eos = (ctx->read_len >= ctx->کل_len);
    *pnread = nread;
    *peos = (bool)ctx->نظر_آیا_eos;
    توڑو;
  }

  CURL_TRC_READ(ڈیٹا, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->کل_len, ctx->read_len, (صحیح)نتیجہ, *pnread, *peos);
  واپس نتیجہ;
}

ساکن bool cr_mime_needs_rewind(ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_creader *reader)
{
  ساخت cr_mime_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  واپس ctx->read_len > 0;
}

ساکن curl_off_t cr_mime_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_creader *reader)
{
  ساخت cr_mime_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  واپس ctx->کل_len;
}

ساکن CURLcode cr_mime_resume_from(ساخت Curl_easy *ڈیٹا,
                                    ساخت Curl_creader *reader,
                                    curl_off_t انحراف)
{
  ساخت cr_mime_ctx *ctx = reader->ctx;

  اگر(انحراف > 0) {
    curl_off_t passed = 0;

    کرو {
      حرف scratch[4 * 1024];
      size_t readthisamountnow =
        (انحراف - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(انحراف - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      اگر((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(ڈیٹا, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        واپس CURLE_READ_ERROR;
      }
    } جب(passed < انحراف);

    /* now, decrease the size of the read */
    اگر(ctx->کل_len > 0) {
      ctx->کل_len -= انحراف;

      اگر(ctx->کل_len <= 0) {
        failf(ڈیٹا, "Mime post already completely uploaded");
        واپس CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  واپس CURLE_OK;
}

ساکن CURLcode cr_mime_cntrl(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  ساخت cr_mime_ctx *ctx = reader->ctx;
  انتخاب(opcode) {
  حالت CURL_CRCNTRL_REWIND: {
    CURLcode نتیجہ = mime_rewind(ctx->part);
    اگر(نتیجہ)
      failf(ڈیٹا, "Cannot rewind mime/post data");
    واپس نتیجہ;
  }
  حالت CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    توڑو;
  حالت CURL_CRCNTRL_CLEAR_EOS:
    ctx->نظر_آیا_eos = FALSE;
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن bool cr_mime_is_paused(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_creader *reader)
{
  ساخت cr_mime_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  واپس ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

ساکن ثابت ساخت Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_کل_لمبائی,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_مکمل,
  حجم(ساخت cr_mime_ctx)
};

CURLcode Curl_creader_سیٹ_mime(ساخت Curl_easy *ڈیٹا, curl_mimepart *part)
{
  ساخت Curl_creader *r;
  ساخت cr_mime_ctx *ctx;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&r, ڈیٹا, &cr_mime, CURL_CR_CLIENT);
  اگر(نتیجہ)
    واپس نتیجہ;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  نتیجہ = mime_rewind(ctx->part);
  اگر(نتیجہ) {
    Curl_creader_free(ڈیٹا, r);
    واپس نتیجہ;
  }
  ctx->کل_len = mime_سائز(ctx->part);

  واپس Curl_creader_سیٹ(ڈیٹا, r);
}

#ورنہ /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (خالی)easy;
  واپس تہی;
}

خالی curl_mime_free(curl_mime *mime)
{
  (خالی)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (خالی)mime;
  واپس تہی;
}

CURLcode curl_mime_name(curl_mimepart *part, ثابت حرف *name)
{
  (خالی)part;
  (خالی)name;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_mime_filename(curl_mimepart *part, ثابت حرف *filename)
{
  (خالی)part;
  (خالی)filename;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_mime_type(curl_mimepart *part, ثابت حرف *mimetype)
{
  (خالی)part;
  (خالی)mimetype;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ثابت حرف *encoding)
{
  (خالی)part;
  (خالی)encoding;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_mime_ڈیٹا(curl_mimepart *part, ثابت حرف *ڈیٹا, size_t datasize)
{
  (خالی)part;
  (خالی)ڈیٹا;
  (خالی)datasize;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ثابت حرف *filename)
{
  (خالی)part;
  (خالی)filename;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_mime_ڈیٹا_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           خالی *arg)
{
  (خالی)part;
  (خالی)datasize;
  (خالی)readfunc;
  (خالی)seekfunc;
  (خالی)freefunc;
  (خالی)arg;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (خالی)part;
  (خالی)subparts;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode curl_mime_سرخیاں(curl_mimepart *part,
                           ساخت curl_slist *سرخیاں, صحیح take_ownership)
{
  (خالی)part;
  (خالی)سرخیاں;
  (خالی)take_ownership;
  واپس CURLE_NOT_BUILT_اندر;
}

CURLcode Curl_mime_جوڑو_سرخی(ساخت curl_slist **slp, ثابت حرف *fmt, ...)
{
  (خالی)slp;
  (خالی)fmt;
  واپس CURLE_NOT_BUILT_اندر;
}

#اگر_ختم /* if disabled */

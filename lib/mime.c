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
#ضمن "curl_setup.h"

بنية Curl_easy;

#ضمن "mime.h"
#ضمن "urldata.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "transfer.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/basename.h"
#ضمن "curlx/strcopy.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "curlx/base64.h"

#إذا !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#ضمن "rand.h"
#ضمن "slist.h"
#ضمن "curlx/dynbuf.h"

#وصف_ڪريو MAX_MIME_سطحون 40 /* avoid deep nesting */

#وصف_ڪريو READ_ERROR   ((size_t)-1)
#وصف_ڪريو STOP_FILLING ((size_t)-2)

ساكن size_t mime_subparts_read(حرف *بفر, size_t سائيز, size_t nitems,
                                 فراغ *instream, bool *hasread);
ساكن curl_off_t mime_سائيز(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#وصف_ڪريو QP_OK           1       /* Can be represented by itself. */
#وصف_ڪريو QP_SP           2       /* Space or tab. */
#وصف_ڪريو QP_CR           3       /* Carriage return. */
#وصف_ڪريو QP_LF           4       /* Line-feed. */
ساكن ثابت بنانشان حرف qp_class[] = {
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
ساكن ثابت حرف aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#جيڪڏهن_وصف_نه_ٿيل __VMS
#وصف_ڪريو filesize(name, stat_معلومات) stat_معلومات.st_سائيز
#وصف_ڪريو fopen_read                curlx_fopen

#وإلا

#ضمن <fabdef.h>
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
curl_off_t Vms_Real_File_سائيز(ثابت حرف *name, ثابت curlx_struct_stat *stat_buf)
{
  حرف بفر[8192];
  curl_off_t ڳڻپ;
  صحيح ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  إذا(!file)
    أرجع 0;

  ڳڻپ = 0;
  ret_stat = 1;
  طالما(ret_stat > 0) {
    ret_stat = fread(بفر, 1, حجم(بفر), file);
    إذا(ret_stat)
      ڳڻپ += ret_stat;
  }
  curlx_fclose(file);

  أرجع ڳڻپ;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
ساكن curl_off_t Vms_Special_سائيز(ثابت حرف *name,
                                 ثابت curlx_struct_stat *stat_buf)
{
  بدل(stat_buf->st_fab_rfm) {
  حالة FAB$C_VAR:
  حالة FAB$C_VFC:
    أرجع Vms_Real_File_سائيز(name, stat_buf);
    اكسر;
  افتراضي:
    أرجع stat_buf->st_سائيز;
  }
}

#وصف_ڪريو filesize(name, stat_معلومات) Vms_Special_سائيز(name, &stat_معلومات)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
ساكن FILE *vmsfopenread(ثابت حرف *file, ثابت حرف *گهڻائي)
{
  curlx_struct_stat statbuf;
  صحيح res = curlx_stat(file, &statbuf);

  إذا(res != -1) {
    بدل(statbuf.st_fab_rfm) {
    حالة FAB$C_VAR:
    حالة FAB$C_VFC:
    حالة FAB$C_STMCR:
      أرجع curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      اكسر;
    افتراضي:
      أرجع curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  أرجع عدم;
}

#وصف_ڪريو fopen_read vmsfopenread
#جيڪڏهن_پورو /* !__VMS */

/* Set readback state. */
ساكن فراغ mimesetstate(بنية mime_state *state,
                         تعداد mimestate tok, فراغ *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->وٿي = 0;
}

/* Escape header string into allocated memory. */
ساكن حرف *escape_string(بنية Curl_easy *معلومات,
                           ثابت حرف *src, تعداد mimestrategy strategy)
{
  CURLcode نتيجو;
  بنية dynbuf db;
  ثابت حرف * ثابت *جدول;
  ثابت حرف * ثابت *p;
  /* replace first character by rest of string. */
  ساكن ثابت حرف * ثابت mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    عدم
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  ساكن ثابت حرف * ثابت formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    عدم
  };

  جدول = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  إذا(strategy == MIMESTRATEGY_MAIL || (معلومات && معلومات->سيٽ.mime_formescape))
    جدول = mimetable;

  curlx_dyn_init(&db, CURL_MAX_داخلا_ڊيگهائي);

  لكل(نتيجو = curlx_dyn_addn(&db, STRCONST("")); !نتيجو && *src; src++) {
    لكل(p = جدول; *p && **p != *src; p++)
      ;

    إذا(*p)
      نتيجو = curlx_dyn_جوڙيو(&db, *p + 1);
    وإلا
      نتيجو = curlx_dyn_addn(&db, src, 1);
  }

  أرجع curlx_dyn_ptr(&db);
}

/* Check if header matches. */
ساكن حرف *ميلاپ_سرو(بنية curl_slist *hdr, ثابت حرف *lbl, size_t len)
{
  حرف *قدر = عدم;

  إذا(curl_strnequal(hdr->معلومات, lbl, len) && hdr->معلومات[len] == ':')
    لكل(قدر = hdr->معلومات + len + 1; *قدر == ' '; قدر++)
      ;
  أرجع قدر;
}

/* Get a header from an slist. */
ساكن حرف *ڳولا_سرو(بنية curl_slist *hdrlist,
                           ثابت حرف *hdr, size_t len)
{
  حرف *قدر = عدم;

  لكل(; !قدر && hdrlist; hdrlist = hdrlist->ايندڙ)
    قدر = ميلاپ_سرو(hdrlist, hdr, len);

  أرجع قدر;
}

ساكن حرف *strippath(ثابت حرف *fullfile)
{
  حرف *filename;
  حرف *بنياد;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  إذا(!filename)
    أرجع عدم;
  بنياد = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  أرجع بنياد; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
ساكن فراغ cleanup_encoder_state(بنية mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
ساكن size_t encoder_nop_read(حرف *بفر, size_t سائيز, bool ateof,
                               بنية curl_mimepart *part)
{
  بنية mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (فراغ)ateof;

  إذا(!سائيز)
    أرجع STOP_FILLING;

  إذا(سائيز > insize)
    سائيز = insize;

  إذا(سائيز)
    يادگيري_نقل(بفر, st->buf + st->bufbeg, سائيز);

  st->bufbeg += سائيز;
  أرجع سائيز;
}

ساكن curl_off_t encoder_nop_سائيز(curl_mimepart *part)
{
  أرجع part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
ساكن size_t encoder_7bit_read(حرف *بفر, size_t سائيز, bool ateof,
                                curl_mimepart *part)
{
  بنية mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (فراغ)ateof;

  إذا(!سائيز)
    أرجع STOP_FILLING;

  إذا(سائيز > cursize)
    سائيز = cursize;

  لكل(cursize = 0; cursize < سائيز; cursize++) {
    *بفر = st->buf[st->bufbeg];
    إذا(*بفر++ & 0x80)
      أرجع cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  أرجع cursize;
}

/* Base64 content encoder. */
ساكن size_t encoder_base64_read(حرف *بفر, size_t سائيز, bool ateof,
                                  curl_mimepart *part)
{
  بنية mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  صحيح i;
  حرف *ptr = بفر;

  طالما(st->bufbeg < st->bufend) {
    /* Line full ? */
    إذا(st->pos > MAX_ENCODED_سٽ_ڊيگهائي - 4) {
      /* Yes, we need 2 characters for CRLF. */
      إذا(سائيز < 2) {
        إذا(!cursize)
          أرجع STOP_FILLING;
        اكسر;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      سائيز -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    إذا(سائيز < 4) {
      إذا(!cursize)
        أرجع STOP_FILLING;
      اكسر;
    }
    إذا(st->bufend - st->bufbeg < 3)
      اكسر;

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
    سائيز -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  إذا(ateof) {
    إذا(سائيز < 4) {
      إذا(!cursize)
        أرجع STOP_FILLING;
    }
    وإلا {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      إذا(st->bufend != st->bufbeg) {

        إذا(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        إذا(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  أرجع cursize;
}

/* The maximum input size that does not cause an overflow. */
#وصف_ڪريو BASE64_MAX_داخلا_سائيز                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_سٽ_ڊيگهائي + 2)) *                  \
    MAX_ENCODED_سٽ_ڊيگهائي / 4) * 3 - 3)

ساكن curl_off_t encoder_base64_سائيز(curl_mimepart *part)
{
  curl_off_t سائيز = part->datasize;

  إذا(سائيز <= 0)
    أرجع سائيز;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  إذا(سائيز > BASE64_MAX_داخلا_سائيز)
    أرجع -1;

  /* Compute base64 character count. */
  سائيز = 4 * (1 + ((سائيز - 1) / 3));

  /* Effective character count must include CRLFs. */
  أرجع سائيز + (2 * ((سائيز - 1) / MAX_ENCODED_سٽ_ڊيگهائي));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
ساكن صحيح qp_lookahead_eol(بنية mime_encoder_state *st, صحيح ateof, size_t n)
{
  n += st->bufbeg;
  إذا(n >= st->bufend && ateof)
    أرجع 1;
  إذا(n + 2 > st->bufend)
    أرجع ateof ? 0 : -1;
  إذا(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    أرجع 1;
  أرجع 0;
}

/* Quoted-printable encoder. */
ساكن size_t encoder_qp_read(حرف *بفر, size_t سائيز, bool ateof,
                              curl_mimepart *part)
{
  بنية mime_encoder_state *st = &part->encstate;
  حرف *ptr = بفر;
  size_t cursize = 0;
  صحيح softlinebreak;
  حرف buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  طالما(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    صحيح i = (بنانشان حرف)st->buf[st->bufbeg];
    buf[0] = (حرف)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    بدل(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    حالة QP_OK:          /* Not a special character. */
      اكسر;
    حالة QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      بدل(qp_lookahead_eol(st, ateof, 1)) {
      حالة -1:          /* More input data needed. */
        أرجع cursize;
      حالة 0:           /* No encoding needed. */
        اكسر;
      افتراضي:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        اكسر;
      }
      اكسر;
    حالة QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      بدل(qp_lookahead_eol(st, ateof, 0)) {
      حالة -1:          /* Need more data. */
        أرجع cursize;
      حالة 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        اكسر;
      افتراضي:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        اكسر;
      }
      اكسر;
    افتراضي:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      اكسر;
    }

    /* Be sure the encoded character fits within maximum line length. */
    إذا(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_سٽ_ڊيگهائي;
      إذا(!softlinebreak && st->pos + len == MAX_ENCODED_سٽ_ڊيگهائي) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        بدل(qp_lookahead_eol(st, ateof, consumed)) {
        حالة -1:        /* Need more data. */
          أرجع cursize;
        حالة 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          اكسر;
        }
      }
      إذا(softlinebreak) {
        curlx_strcopy(buf, حجم(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    إذا(len > سائيز) {
      إذا(!cursize)
        أرجع STOP_FILLING;
      اكسر;
    }

    /* Append to output buffer. */
    يادگيري_نقل(ptr, buf, len);
    cursize += len;
    ptr += len;
    سائيز -= len;
    st->pos += len;
    إذا(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  أرجع cursize;
}

ساكن curl_off_t encoder_qp_سائيز(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  أرجع part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
ساكن size_t mime_mem_read(حرف *بفر, size_t سائيز, size_t nitems,
                            فراغ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.وٿي);
  (فراغ)سائيز;  /* Always 1 */

  إذا(!nitems)
    أرجع STOP_FILLING;

  إذا(sz > nitems)
    sz = nitems;

  إذا(sz)
    يادگيري_نقل(بفر, part->معلومات + curlx_sotouz(part->state.وٿي), sz);

  أرجع sz;
}

ساكن صحيح mime_mem_seek(فراغ *instream, curl_off_t وٿي, صحيح whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  بدل(whence) {
  حالة SEEK_CUR:
    وٿي += part->state.وٿي;
    اكسر;
  حالة SEEK_END:
    وٿي += part->datasize;
    اكسر;
  }

  إذا(وٿي < 0 || وٿي > part->datasize)
    أرجع CURL_SEEKFUNC_FAIL;

  part->state.وٿي = وٿي;
  أرجع CURL_SEEKFUNC_OK;
}

ساكن فراغ mime_mem_free(فراغ *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->معلومات);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
ساكن bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  إذا(part->fp)
    أرجع FALSE;
  part->fp = fopen_read(part->معلومات, "rb");
  أرجع part->fp ? FALSE : TRUE;
}

ساكن size_t mime_file_read(حرف *بفر, size_t سائيز, size_t nitems,
                             فراغ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  إذا(!nitems)
    أرجع STOP_FILLING;

  إذا(mime_open_file(part))
    أرجع READ_ERROR;

  أرجع fread(بفر, سائيز, nitems, part->fp);
}

ساكن صحيح mime_file_seek(فراغ *instream, curl_off_t وٿي, صحيح whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  إذا(whence == SEEK_SET && !وٿي && !part->fp)
    أرجع CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  إذا(mime_open_file(part))
    أرجع CURL_SEEKFUNC_FAIL;

  أرجع curlx_fseek(part->fp, وٿي, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

ساكن فراغ mime_file_free(فراغ *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  إذا(part->fp) {
    curlx_fclose(part->fp);
    part->fp = عدم;
  }
  curlx_safefree(part->معلومات);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
ساكن size_t readback_بائيٽون(بنية mime_state *state,
                             حرف *بفر, size_t bufsize,
                             ثابت حرف *بائيٽون, size_t numbytes,
                             ثابت حرف *trail, size_t traillen)
{
  size_t sz;
  size_t وٿي = curlx_sotouz(state->وٿي);

  إذا(numbytes > وٿي) {
    sz = numbytes - وٿي;
    بائيٽون += وٿي;
  }
  وإلا {
    sz = وٿي - numbytes;
    إذا(sz >= traillen)
      أرجع 0;
    بائيٽون = trail + sz;
    sz = traillen - sz;
  }

  إذا(sz > bufsize)
    sz = bufsize;

  يادگيري_نقل(بفر, بائيٽون, sz);
  state->وٿي += sz;
  أرجع sz;
}

/* Read a non-encoded part content. */

ساكن size_t read_part_content(curl_mimepart *part, حرف *بفر,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  بدل(part->lastreadstatus) {
  حالة 0:
  حالة CURL_READFUNC_ABORT:
  حالة CURL_READFUNC_PAUSE:
  حالة READ_ERROR:
    أرجع part->lastreadstatus;
  افتراضي:
    اكسر;
  }

  /* If we can determine we are at end of part data, spare a read. */
  إذا(part->datasize != (curl_off_t)-1 &&
     part->state.وٿي >= part->datasize) {
    /* sz is already zero. */
  }
  وإلا {
    بدل(part->قسم) {
    حالة MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(بفر, 1, bufsize, part->arg, hasread);
      اكسر;
    حالة MIMEKIND_FILE:
      إذا(part->fp && feof(part->fp))
        اكسر;  /* At EOF. */
      FALLTHROUGH();
    افتراضي:
      إذا(part->readfunc) {
        إذا(!(part->flags & MIME_FAST_READ)) {
          إذا(*hasread)
            أرجع STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(بفر, 1, bufsize, part->arg);
      }
      اكسر;
    }
  }

  بدل(sz) {
  حالة STOP_FILLING:
    اكسر;
  حالة 0:
  حالة CURL_READFUNC_ABORT:
  حالة CURL_READFUNC_PAUSE:
  حالة READ_ERROR:
    part->lastreadstatus = sz;
    اكسر;
  افتراضي:
    part->state.وٿي += sz;
    part->lastreadstatus = sz;
    اكسر;
  }

  أرجع sz;
}

/* Read and encode part content. */
ساكن size_t read_encoded_part_content(curl_mimepart *part, حرف *بفر,
                                        size_t bufsize, bool *hasread)
{
  بنية mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  لكل(;;) {
    إذا(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(بفر, bufsize, ateof, part);
      بدل(sz) {
      حالة 0:
        إذا(ateof)
          أرجع cursize;
        اكسر;
      حالة READ_ERROR:
      حالة STOP_FILLING:
        أرجع cursize ? cursize : sz;
      افتراضي:
        cursize += sz;
        بفر += sz;
        bufsize -= sz;
        استمر;
      }
    }

    /* We need more data in input buffer. */
    إذا(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      إذا(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    إذا(st->bufend >= حجم(st->buf))
      أرجع cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           حجم(st->buf) - st->bufend, hasread);
    بدل(sz) {
    حالة 0:
      ateof = TRUE;
      اكسر;
    حالة CURL_READFUNC_ABORT:
    حالة CURL_READFUNC_PAUSE:
    حالة READ_ERROR:
    حالة STOP_FILLING:
      أرجع cursize ? cursize : sz;
    افتراضي:
      st->bufend += sz;
      اكسر;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
ساكن size_t readback_part(curl_mimepart *part,
                            حرف *بفر, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  طالما(bufsize) {
    size_t sz = 0;
    بنية curl_slist *hdr = (بنية curl_slist *)part->state.ptr;
    بدل(part->state.state) {
    حالة MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      اكسر;
    حالة MIMESTATE_USERHEADERS:
      إذا(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, عدم);
        اكسر;
      }
      إذا(ميلاپ_سرو(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->ايندڙ);
        اكسر;
      }
      FALLTHROUGH();
    حالة MIMESTATE_CURLHEADERS:
      إذا(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      وإلا {
        sz = readback_بائيٽون(&part->state, بفر, bufsize,
                            hdr->معلومات, لڙي_ڊيگهه(hdr->معلومات), STRCONST("\r\n"));
        إذا(!sz)
          mimesetstate(&part->state, part->state.state, hdr->ايندڙ);
      }
      اكسر;
    حالة MIMESTATE_EOH:
      sz = readback_بائيٽون(&part->state, بفر, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      إذا(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, عدم);
      اكسر;
    حالة MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, عدم);
      اكسر;
    حالة MIMESTATE_CONTENT:
      إذا(part->encoder)
        sz = read_encoded_part_content(part, بفر, bufsize, hasread);
      وإلا
        sz = read_part_content(part, بفر, bufsize, hasread);
      بدل(sz) {
      حالة 0:
        mimesetstate(&part->state, MIMESTATE_پڇاڙي, عدم);
        /* Try sparing open file descriptors. */
        إذا(part->قسم == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = عدم;
        }
        FALLTHROUGH();
      حالة CURL_READFUNC_ABORT:
      حالة CURL_READFUNC_PAUSE:
      حالة READ_ERROR:
      حالة STOP_FILLING:
        أرجع cursize ? cursize : sz;
      }
      اكسر;
    حالة MIMESTATE_پڇاڙي:
      أرجع cursize;
    افتراضي:
      اكسر;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    بفر += sz;
    bufsize -= sz;
  }

  أرجع cursize;
}

/* Readback from mime. Warning: not a read callback function. */
ساكن size_t mime_subparts_read(حرف *بفر, size_t سائيز, size_t nitems,
                                 فراغ *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (فراغ)سائيز;  /* Always 1 */

  طالما(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    بدل(mime->state.state) {
    حالة MIMESTATE_BEGIN:
    حالة MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.وٿي += 2;
      اكسر;
    حالة MIMESTATE_BOUNDARY1:
      sz = readback_بائيٽون(&mime->state, بفر, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      إذا(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      اكسر;
    حالة MIMESTATE_BOUNDARY2:
      إذا(part)
        sz = readback_بائيٽون(&mime->state, بفر, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      وإلا
        sz = readback_بائيٽون(&mime->state, بفر, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      إذا(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      اكسر;
    حالة MIMESTATE_CONTENT:
      إذا(!part) {
        mimesetstate(&mime->state, MIMESTATE_پڇاڙي, عدم);
        اكسر;
      }
      sz = readback_part(part, بفر, nitems, hasread);
      بدل(sz) {
      حالة CURL_READFUNC_ABORT:
      حالة CURL_READFUNC_PAUSE:
      حالة READ_ERROR:
      حالة STOP_FILLING:
        أرجع cursize ? cursize : sz;
      حالة 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        اكسر;
      }
      اكسر;
    حالة MIMESTATE_پڇاڙي:
      أرجع cursize;
    افتراضي:
      اكسر;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    بفر += sz;
    nitems -= sz;
  }

  أرجع cursize;
}

ساكن صحيح mime_part_rewind(curl_mimepart *part)
{
  صحيح res = CURL_SEEKFUNC_OK;
  تعداد mimestate targetstate = MIMESTATE_BEGIN;

  إذا(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  إذا(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    إذا(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      بدل(res) {
      حالة CURL_SEEKFUNC_OK:
      حالة CURL_SEEKFUNC_FAIL:
      حالة CURL_SEEKFUNC_CANTSEEK:
        اكسر;
      حالة -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        اكسر;
      افتراضي:
        res = CURL_SEEKFUNC_FAIL;
        اكسر;
      }
    }
  }

  إذا(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, عدم);

  part->lastreadstatus = 1; /* Successful read status. */
  أرجع res;
}

ساكن صحيح mime_subparts_seek(فراغ *instream, curl_off_t وٿي, صحيح whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  صحيح rc = CURL_SEEKFUNC_OK;

  إذا(whence != SEEK_SET || وٿي)
    أرجع CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  إذا(mime->state.state == MIMESTATE_BEGIN)
    أرجع CURL_SEEKFUNC_OK;           /* Already rewound. */

  لكل(part = mime->firstpart; part; part = part->nextpart) {
    صحيح res = mime_part_rewind(part);
    إذا(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  إذا(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, عدم);

  أرجع rc;
}

/* Release part content. */
ساكن فراغ cleanup_part_content(curl_mimepart *part)
{
  إذا(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = عدم;
  part->seekfunc = عدم;
  part->freefunc = عدم;
  part->arg = (فراغ *)part;           /* Defaults to part itself. */
  part->معلومات = عدم;
  part->fp = عدم;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->قسم = MIMEKIND_NONE;
  part->flags &= ~(بنانشان صحيح)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

ساكن فراغ mime_subparts_free(فراغ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  إذا(mime && mime->والد) {
    mime->والد->freefunc = عدم;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->والد);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
ساكن فراغ mime_subparts_unbind(فراغ *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  إذا(mime && mime->والد) {
    mime->والد->freefunc = عدم;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->والد);  /* Avoid dangling pointer in part. */
    mime->والد = عدم;
  }
}

فراغ Curl_mime_cleanpart(curl_mimepart *part)
{
  إذا(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    إذا(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
فراغ curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  إذا(!mime)
    أرجع;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  طالما(mime) {
    part = mime->firstpart;
    إذا(part) {
      mime->firstpart = part->nextpart;
      إذا(part->قسم == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = عدم;
        cleanup_part_content(part);
        subparts->والد = mime->والد;
        mime->والد = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    وإلا {
      curl_mime *والد = (curl_mime *)mime->والد;
      curlx_free(mime);
      mime = والد;
    }
  }
}

ساكن bool mime_is_too_deep(ثابت curl_mimepart *part, size_t call_اونهائي)
{
  DEBUGASSERT(part);

  إذا(++call_اونهائي > MAX_MIME_سطحون)
    أرجع TRUE;

  إذا(part->قسم == MIMEKIND_MULTIPART && part->arg) {
    ثابت curl_mime *mime = (ثابت curl_mime *)part->arg;
    ثابت curl_mimepart *s;

    لكل(s = mime->firstpart; s; s = s->nextpart) {
      إذا(mime_is_too_deep(s, call_اونهائي))
        أرجع TRUE;
    }
  }

  أرجع FALSE;
}

ساكن CURLcode mime_duppart(بنية Curl_easy *معلومات,
                             curl_mimepart *dst, ثابت curl_mimepart *src,
                             size_t call_اونهائي)
{
  curl_mime *mime;
  curl_mimepart *d;
  ثابت curl_mimepart *s;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  إذا(++call_اونهائي > MAX_MIME_سطحون)
    أرجع CURLE_TOO_LARGE;

  /* Duplicate content. */
  بدل(src->قسم) {
  حالة MIMEKIND_NONE:
    اكسر;
  حالة MIMEKIND_معلومات:
    نتيجو = curl_mime_معلومات(dst, src->معلومات, (size_t)src->datasize);
    اكسر;
  حالة MIMEKIND_FILE:
    نتيجو = curl_mime_filedata(dst, src->معلومات);
    /* Do not abort duplication if file is not readable. */
    إذا(نتيجو == CURLE_READ_ERROR)
      نتيجو = CURLE_OK;
    اكسر;
  حالة MIMEKIND_CALLBACK:
    نتيجو = curl_mime_معلومات_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    اكسر;
  حالة MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(معلومات);
    نتيجو = mime ? curl_mime_subparts(dst, mime) : CURLE_ٻاهريون_جو_يادگيري;

    /* Duplicate subparts. */
    لكل(s = ((curl_mime *)src->arg)->firstpart; !نتيجو && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      نتيجو = d ? mime_duppart(معلومات, d, s, call_اونهائي) : CURLE_ٻاهريون_جو_يادگيري;
    }
    اكسر;
  افتراضي:  /* Invalid kind: should not occur. */
    DEBUGF(infof(معلومات, "invalid MIMEKIND* attempt"));
    نتيجو = CURLE_BAD_FUNCTION_دليل;  /* Internal error? */
    اكسر;
  }

  /* Duplicate headers. */
  إذا(!نتيجو && src->userheaders) {
    بنية curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    إذا(!hdrs)
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    وإلا {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      نتيجو = curl_mime_سرا(dst, hdrs, TRUE);
      إذا(نتيجو)
        curl_slist_free_all(hdrs);
    }
  }

  إذا(!نتيجو) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    نتيجو = curl_mime_type(dst, src->mimetype);
  }
  إذا(!نتيجو)
    نتيجو = curl_mime_name(dst, src->name);
  إذا(!نتيجو)
    نتيجو = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  إذا(نتيجو)
    Curl_mime_cleanpart(dst);

  أرجع نتيجو;
}

CURLcode Curl_mime_duppart(بنية Curl_easy *معلومات,
                           curl_mimepart *dst, ثابت curl_mimepart *src)
{
  إذا(!dst || !src)
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(mime_is_too_deep(src, 0))
    أرجع CURLE_TOO_LARGE;

  أرجع mime_duppart(معلومات, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(فراغ *easy)
{
  curl_mime *mime = curlx_malloc(حجم(*mime));

  إذا(mime) {
    mime->والد = عدم;
    mime->firstpart = عدم;
    mime->lastpart = عدم;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    إذا(Curl_rand_alnum(easy,
                       (بنانشان حرف *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      أرجع عدم;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, عدم);
  }

  أرجع mime;
}

/* Initialize a mime part. */
فراغ Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, حجم(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, عدم);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  إذا(!mime)
    أرجع عدم;

  part = curlx_malloc(حجم(*part));

  إذا(part) {
    Curl_mime_initpart(part);
    part->والد = mime;

    إذا(mime->lastpart)
      mime->lastpart->nextpart = part;
    وإلا
      mime->firstpart = part;

    mime->lastpart = part;
  }

  أرجع part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ثابت حرف *name)
{
  إذا(!part)
    أرجع CURLE_BAD_FUNCTION_دليل;

  curlx_safefree(part->name);

  إذا(name) {
    part->name = curlx_strdup(name);
    إذا(!part->name)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  أرجع CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ثابت حرف *filename)
{
  إذا(!part)
    أرجع CURLE_BAD_FUNCTION_دليل;

  curlx_safefree(part->filename);

  إذا(filename) {
    part->filename = curlx_strdup(filename);
    إذا(!part->filename)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  أرجع CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_معلومات(curl_mimepart *part, ثابت حرف *معلومات, size_t datasize)
{
  إذا(!part)
    أرجع CURLE_BAD_FUNCTION_دليل;

  cleanup_part_content(part);

  إذا(معلومات) {
    إذا(datasize == CURL_ZERO_TERMINATED)
      datasize = لڙي_ڊيگهه(معلومات);

    part->معلومات = curlx_memdup0(معلومات, datasize);
    إذا(!part->معلومات)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->قسم = MIMEKIND_معلومات;
  }

  أرجع CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ثابت حرف *filename)
{
  CURLcode نتيجو = CURLE_OK;

  إذا(!part)
    أرجع CURLE_BAD_FUNCTION_دليل;

  cleanup_part_content(part);

  إذا(filename) {
    حرف *بنياد;
    curlx_struct_stat sbuf;

    إذا(curlx_stat(filename, &sbuf))
      نتيجو = CURLE_READ_ERROR;
    وإلا {
      part->معلومات = curlx_strdup(filename);
      إذا(!part->معلومات)
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      وإلا {
        part->datasize = -1;
        إذا(S_ISREG(sbuf.st_گهڻائي)) {
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
        بنياد = strippath(filename);
        إذا(!بنياد)
          نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        وإلا {
          نتيجو = curl_mime_filename(part, بنياد);
          curlx_free(بنياد);
        }
      }
    }
  }
  أرجع نتيجو;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ثابت حرف *mimetype)
{
  إذا(!part)
    أرجع CURLE_BAD_FUNCTION_دليل;

  curlx_safefree(part->mimetype);

  إذا(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    إذا(!part->mimetype)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  أرجع CURLE_OK;
}

ساكن ثابت بنية mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_سائيز },
  { "8bit", encoder_nop_read, encoder_nop_سائيز },
  { "7bit", encoder_7bit_read, encoder_nop_سائيز },
  { "base64", encoder_base64_read, encoder_base64_سائيز },
  { "quoted-printable", encoder_qp_read, encoder_qp_سائيز },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ثابت حرف *encoding)
{
  CURLcode نتيجو = CURLE_BAD_FUNCTION_دليل;
  ثابت بنية mime_encoder *mep;

  إذا(!part)
    أرجع نتيجو;

  part->encoder = عدم;

  إذا(!encoding)
    أرجع CURLE_OK;    /* Removing current encoder. */

  لكل(mep = encoders; mep->name; mep++)
    إذا(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      نتيجو = CURLE_OK;
    }

  أرجع نتيجو;
}

/* Set mime part headers. */
CURLcode curl_mime_سرا(curl_mimepart *part,
                           بنية curl_slist *سرا, صحيح take_ownership)
{
  إذا(!part)
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(part->flags & MIME_USERHEADERS_OWNER) {
    إذا(part->userheaders != سرا)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(بنانشان صحيح)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = سرا;
  إذا(سرا && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  أرجع CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_معلومات_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, فراغ *arg)
{
  إذا(!part)
    أرجع CURLE_BAD_FUNCTION_دليل;

  cleanup_part_content(part);

  إذا(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->قسم = MIMEKIND_CALLBACK;
  }

  أرجع CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_سيٽ_subparts(curl_mimepart *part,
                                curl_mime *subparts, صحيح take_ownership)
{
  curl_mime *پاڙ;

  إذا(!part)
    أرجع CURLE_BAD_FUNCTION_دليل;

  /* Accept setting twice the same subparts. */
  إذا(part->قسم == MIMEKIND_MULTIPART && part->arg == subparts)
    أرجع CURLE_OK;

  cleanup_part_content(part);

  إذا(subparts) {
    /* Should not have been attached already. */
    إذا(subparts->والد)
      أرجع CURLE_BAD_FUNCTION_دليل;

    /* Should not be the part's root. */
    پاڙ = part->والد;
    إذا(پاڙ) {
      طالما(پاڙ->والد && پاڙ->والد->والد)
        پاڙ = پاڙ->والد->والد;
      إذا(subparts == پاڙ) {
        /* cannot add as a subpart of itself. */
        أرجع CURLE_BAD_FUNCTION_دليل;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    إذا(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      أرجع CURLE_SEND_FAIL_REWIND;

    subparts->والد = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->قسم = MIMEKIND_MULTIPART;
  }

  أرجع CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  أرجع Curl_mime_سيٽ_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(حرف *بفر, size_t سائيز, size_t nitems, فراغ *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (فراغ)سائيز;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  افعل {
    hasread = FALSE;
    ret = readback_part(part, بفر, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } طالما(ret == STOP_FILLING);

  أرجع ret;
}

/* Rewind mime stream. */
ساكن CURLcode mime_rewind(curl_mimepart *part)
{
  أرجع mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
ساكن size_t slist_سائيز(بنية curl_slist *s,
                         size_t overhead, ثابت حرف *skip, size_t skiplen)
{
  size_t سائيز = 0;

  لكل(; s; s = s->ايندڙ)
    إذا(!skip || !ميلاپ_سرو(s, skip, skiplen))
      سائيز += لڙي_ڊيگهه(s->معلومات) + overhead;
  أرجع سائيز;
}

/* Get/compute multipart size. */
ساكن curl_off_t multipart_سائيز(curl_mime *mime)
{
  curl_off_t سائيز;
  curl_off_t boundarysize;
  curl_mimepart *part;

  إذا(!mime)
    أرجع 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  سائيز = boundarysize;  /* Final boundary - CRLF after headers. */

  لكل(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_سائيز(part);

    إذا(sz < 0)
      سائيز = sz;

    إذا(سائيز >= 0)
      سائيز += boundarysize + sz;
  }

  أرجع سائيز;
}

/* Get/compute mime size. */
ساكن curl_off_t mime_سائيز(curl_mimepart *part)
{
  curl_off_t سائيز;

  إذا(part->قسم == MIMEKIND_MULTIPART)
    part->datasize = multipart_سائيز(part->arg);

  سائيز = part->datasize;

  إذا(part->encoder)
    سائيز = part->encoder->sizefunc(part);

  إذا(سائيز >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    سائيز += slist_سائيز(part->curlheaders, 2, عدم, 0);
    سائيز += slist_سائيز(part->userheaders, 2, STRCONST("Content-Type"));
    سائيز += 2;    /* CRLF after headers. */
  }
  أرجع سائيز;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_جوڙيو_سرو(بنية curl_slist **slp, ثابت حرف *fmt, ...)
{
  بنية curl_slist *hdr = عدم;
  حرف *s = عدم;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  إذا(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    إذا(hdr)
      *slp = hdr;
    وإلا
      curlx_free(s);
  }

  أرجع hdr ? CURLE_OK : CURLE_ٻاهريون_جو_يادگيري;
}

/* Add a content type header. */
ساكن CURLcode جوڙيو_content_type(بنية curl_slist **slp,
                                 ثابت حرف *type, ثابت حرف *boundary)
{
  أرجع Curl_mime_جوڙيو_سرو(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ثابت حرف *Curl_mime_contenttype(ثابت حرف *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  بنية ContentType {
    ثابت حرف *extension;
    ثابت حرف *type;
  };
  ساكن ثابت بنية ContentType ctts[] = {
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

  إذا(filename) {
    size_t len1 = لڙي_ڊيگهه(filename);
    ثابت حرف *nameend = filename + len1;
    بنانشان صحيح i;

    لكل(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = لڙي_ڊيگهه(ctts[i].extension);

      إذا(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        أرجع ctts[i].type;
    }
  }
  أرجع عدم;
}

ساكن bool content_type_ميلاپ(ثابت حرف *contenttype,
                               ثابت حرف *هدف, size_t len)
{
  إذا(contenttype && curl_strnequal(contenttype, هدف, len))
    بدل(contenttype[len]) {
    حالة '\0':
    حالة '\t':
    حالة '\r':
    حالة '\n':
    حالة ' ':
    حالة ';':
      أرجع TRUE;
    }
  أرجع FALSE;
}

ساكن CURLcode جوڙيو_content_disposition(بنية Curl_easy *معلومات,
                                        curl_mimepart *part,
                                        ثابت حرف *disposition,
                                        ثابت حرف *contenttype,
                                        تعداد mimestrategy strategy)
{
  إذا(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  إذا(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = عدم;

  إذا(disposition) {
    CURLcode نتيجو = CURLE_OK;
    حرف *name = عدم;
    حرف *filename = عدم;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (معلومات && معلومات->سيٽ.mime_formescape);
    إذا(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      أرجع CURLE_BAD_FUNCTION_دليل;

    إذا(part->name) {
      name = escape_string(معلومات, part->name, strategy);
      إذا(!name)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    إذا(part->filename) {
      filename = escape_string(معلومات, part->filename, strategy);
      إذا(!filename)
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    }
    إذا(!نتيجو)
      نتيجو = Curl_mime_جوڙيو_سرو(&part->curlheaders,
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
    إذا(نتيجو)
      أرجع نتيجو;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode mime_prepare_سرا(بنية Curl_easy *معلومات,
                                     curl_mimepart *part,
                                     ثابت حرف *contenttype,
                                     ثابت حرف *disposition,
                                     تعداد mimestrategy strategy,
                                     size_t call_اونهائي)
{
  curl_mime *mime = عدم;
  ثابت حرف *boundary = عدم;
  حرف *customct;
  ثابت حرف *cte = عدم;
  CURLcode نتيجو = CURLE_OK;

  إذا(++call_اونهائي > MAX_MIME_سطحون)
    أرجع CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = عدم;

  /* Be sure we will not access old headers later. */
  إذا(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, عدم);

  /* Check if content type is specified. */
  customct = part->mimetype;
  إذا(!customct)
    customct = ڳولا_سرو(part->userheaders, STRCONST("Content-Type"));
  إذا(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  إذا(!contenttype) {
    بدل(part->قسم) {
    حالة MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      اكسر;
    حالة MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      إذا(!contenttype)
        contenttype = Curl_mime_contenttype(part->معلومات);
      إذا(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      اكسر;
    افتراضي:
      contenttype = Curl_mime_contenttype(part->filename);
      اكسر;
    }
  }

  إذا(part->قسم == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    إذا(mime)
      boundary = mime->boundary;
  }
  وإلا إذا(contenttype && !customct &&
          content_type_ميلاپ(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = عدم;

  /* Issue content-disposition header only if not already set by caller. */
  إذا(!ڳولا_سرو(part->userheaders, STRCONST("Content-Disposition"))) {
    نتيجو = جوڙيو_content_disposition(معلومات, part, disposition,
                                     contenttype, strategy);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* Issue Content-Type header. */
  إذا(contenttype) {
    نتيجو = جوڙيو_content_type(&part->curlheaders, contenttype, boundary);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* Content-Transfer-Encoding header. */
  إذا(!ڳولا_سرو(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    إذا(part->encoder)
      cte = part->encoder->name;
    وإلا إذا(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->قسم != MIMEKIND_MULTIPART)
      cte = "8bit";
    إذا(cte) {
      نتيجو = Curl_mime_جوڙيو_سرو(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      إذا(نتيجو)
        أرجع نتيجو;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  إذا(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  إذا(part->قسم == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = عدم;
    إذا(content_type_ميلاپ(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    لكل(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      نتيجو = mime_prepare_سرا(معلومات, subpart, عدم,
                                    disposition, strategy, call_اونهائي);
      إذا(نتيجو)
        أرجع نتيجو;
    }
  }
  أرجع نتيجو;
}

CURLcode Curl_mime_prepare_سرا(بنية Curl_easy *معلومات,
                                   curl_mimepart *part,
                                   ثابت حرف *contenttype,
                                   ثابت حرف *disposition,
                                   تعداد mimestrategy strategy)
{
  أرجع mime_prepare_سرا(معلومات, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
ساكن فراغ mime_unpause(curl_mimepart *part)
{
  إذا(part) {
    إذا(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    إذا(part->قسم == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      إذا(mime) {
        curl_mimepart *subpart;

        لكل(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

بنية cr_mime_ctx {
  بنية Curl_creader super;
  curl_mimepart *part;
  curl_off_t ڪل_len;
  curl_off_t read_len;
  CURLcode error_نتيجو;
  بنية bufq tmpbuf;
  ٻٽو_انگ(ڏٺل_eos);
  ٻٽو_انگ(errored);
};

ساكن CURLcode cr_mime_init(بنية Curl_easy *معلومات,
                             بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  ctx->ڪل_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  أرجع CURLE_OK;
}

ساكن فراغ cr_mime_close(بنية Curl_easy *معلومات,
                          بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
ساكن CURLcode cr_mime_read(بنية Curl_easy *معلومات,
                             بنية Curl_creader *reader,
                             حرف *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t nread;
  حرف tmp[256];

  /* Once we have errored, we will return the same error forever */
  إذا(ctx->errored) {
    CURL_TRC_READ(معلومات, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (صحيح)ctx->error_نتيجو);
    *pnread = 0;
    *peos = FALSE;
    أرجع ctx->error_نتيجو;
  }
  إذا(ctx->ڏٺل_eos) {
    CURL_TRC_READ(معلومات, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    أرجع CURLE_OK;
  }
  /* respect length limitations */
  إذا(ctx->ڪل_len >= 0) {
    curl_off_t remain = ctx->ڪل_len - ctx->read_len;
    إذا(remain <= 0)
      blen = 0;
    وإلا إذا(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  إذا(!Curl_bufq_is_خالي_جايون(&ctx->tmpbuf)) {
    نتيجو = Curl_bufq_read(&ctx->tmpbuf, (بنانشان حرف *)buf, blen, &nread);
    إذا(نتيجو) {
      ctx->errored = TRUE;
      ctx->error_نتيجو = نتيجو;
      أرجع نتيجو;
    }
  }
  وإلا إذا(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(معلومات, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, حجم(tmp), ctx->part);
    إذا(nread <= حجم(tmp)) {
      size_t n;
      نتيجو = Curl_bufq_write(&ctx->tmpbuf, (بنانشان حرف *)tmp, nread, &n);
      إذا(نتيجو) {
        ctx->errored = TRUE;
        ctx->error_نتيجو = نتيجو;
        أرجع نتيجو;
      }
      /* stored it, read again */
      نتيجو = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      إذا(نتيجو) {
        ctx->errored = TRUE;
        ctx->error_نتيجو = نتيجو;
        أرجع نتيجو;
      }
    }
  }
  وإلا
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(معلومات, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  بدل(nread) {
  حالة 0:
    إذا((ctx->ڪل_len >= 0) && (ctx->read_len < ctx->ڪل_len)) {
      failf(معلومات, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->ڪل_len);
      أرجع CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->ڏٺل_eos = TRUE;
    اكسر;

  حالة CURL_READFUNC_ABORT:
    failf(معلومات, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_نتيجو = CURLE_ABORTED_BY_CALLBACK;
    نتيجو = CURLE_ABORTED_BY_CALLBACK;
    اكسر;

  حالة CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(معلومات, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    نتيجو = Curl_xfer_pause_send(معلومات, TRUE);
    اكسر; /* nothing was read */

  حالة STOP_FILLING:
  حالة READ_ERROR:
    failf(معلومات, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_نتيجو = CURLE_READ_ERROR;
    نتيجو = CURLE_READ_ERROR;
    اكسر;

  افتراضي:
    إذا(nread > blen) {
      /* the read function returned a too large value */
      failf(معلومات, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_نتيجو = CURLE_READ_ERROR;
      أرجع CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    إذا(ctx->ڪل_len >= 0)
      ctx->ڏٺل_eos = (ctx->read_len >= ctx->ڪل_len);
    *pnread = nread;
    *peos = (bool)ctx->ڏٺل_eos;
    اكسر;
  }

  CURL_TRC_READ(معلومات, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->ڪل_len, ctx->read_len, (صحيح)نتيجو, *pnread, *peos);
  أرجع نتيجو;
}

ساكن bool cr_mime_needs_rewind(بنية Curl_easy *معلومات,
                                 بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع ctx->read_len > 0;
}

ساكن curl_off_t cr_mime_ڪل_ڊيگهائي(بنية Curl_easy *معلومات,
                                       بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع ctx->ڪل_len;
}

ساكن CURLcode cr_mime_resume_from(بنية Curl_easy *معلومات,
                                    بنية Curl_creader *reader,
                                    curl_off_t وٿي)
{
  بنية cr_mime_ctx *ctx = reader->ctx;

  إذا(وٿي > 0) {
    curl_off_t passed = 0;

    افعل {
      حرف scratch[4 * 1024];
      size_t readthisamountnow =
        (وٿي - passed > (curl_off_t)حجم(scratch)) ?
        حجم(scratch) :
        curlx_sotouz(وٿي - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      إذا((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(معلومات, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        أرجع CURLE_READ_ERROR;
      }
    } طالما(passed < وٿي);

    /* now, decrease the size of the read */
    إذا(ctx->ڪل_len > 0) {
      ctx->ڪل_len -= وٿي;

      إذا(ctx->ڪل_len <= 0) {
        failf(معلومات, "Mime post already completely uploaded");
        أرجع CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cr_mime_cntrl(بنية Curl_easy *معلومات,
                              بنية Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  بدل(opcode) {
  حالة CURL_CRCNTRL_REWIND: {
    CURLcode نتيجو = mime_rewind(ctx->part);
    إذا(نتيجو)
      failf(معلومات, "Cannot rewind mime/post data");
    أرجع نتيجو;
  }
  حالة CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    اكسر;
  حالة CURL_CRCNTRL_CLEAR_EOS:
    ctx->ڏٺل_eos = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن bool cr_mime_is_paused(بنية Curl_easy *معلومات,
                              بنية Curl_creader *reader)
{
  بنية cr_mime_ctx *ctx = reader->ctx;
  (فراغ)معلومات;
  أرجع ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

ساكن ثابت بنية Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_ڪل_ڊيگهائي,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_پورو,
  حجم(بنية cr_mime_ctx)
};

CURLcode Curl_creader_سيٽ_mime(بنية Curl_easy *معلومات, curl_mimepart *part)
{
  بنية Curl_creader *r;
  بنية cr_mime_ctx *ctx;
  CURLcode نتيجو;

  نتيجو = Curl_creader_create(&r, معلومات, &cr_mime, CURL_CR_CLIENT);
  إذا(نتيجو)
    أرجع نتيجو;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  نتيجو = mime_rewind(ctx->part);
  إذا(نتيجو) {
    Curl_creader_free(معلومات, r);
    أرجع نتيجو;
  }
  ctx->ڪل_len = mime_سائيز(ctx->part);

  أرجع Curl_creader_سيٽ(معلومات, r);
}

#وإلا /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (فراغ)easy;
  أرجع عدم;
}

فراغ curl_mime_free(curl_mime *mime)
{
  (فراغ)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (فراغ)mime;
  أرجع عدم;
}

CURLcode curl_mime_name(curl_mimepart *part, ثابت حرف *name)
{
  (فراغ)part;
  (فراغ)name;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_mime_filename(curl_mimepart *part, ثابت حرف *filename)
{
  (فراغ)part;
  (فراغ)filename;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_mime_type(curl_mimepart *part, ثابت حرف *mimetype)
{
  (فراغ)part;
  (فراغ)mimetype;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ثابت حرف *encoding)
{
  (فراغ)part;
  (فراغ)encoding;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_mime_معلومات(curl_mimepart *part, ثابت حرف *معلومات, size_t datasize)
{
  (فراغ)part;
  (فراغ)معلومات;
  (فراغ)datasize;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ثابت حرف *filename)
{
  (فراغ)part;
  (فراغ)filename;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_mime_معلومات_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           فراغ *arg)
{
  (فراغ)part;
  (فراغ)datasize;
  (فراغ)readfunc;
  (فراغ)seekfunc;
  (فراغ)freefunc;
  (فراغ)arg;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (فراغ)part;
  (فراغ)subparts;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode curl_mime_سرا(curl_mimepart *part,
                           بنية curl_slist *سرا, صحيح take_ownership)
{
  (فراغ)part;
  (فراغ)سرا;
  (فراغ)take_ownership;
  أرجع CURLE_NOT_BUILT_اندريون;
}

CURLcode Curl_mime_جوڙيو_سرو(بنية curl_slist **slp, ثابت حرف *fmt, ...)
{
  (فراغ)slp;
  (فراغ)fmt;
  أرجع CURLE_NOT_BUILT_اندريون;
}

#جيڪڏهن_پورو /* if disabled */

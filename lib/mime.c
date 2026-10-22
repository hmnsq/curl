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

ساختار Curl_easy;

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

#define MAX_MIME_LEVELS 40 /* avoid deep nesting */

#define READ_ERROR   ((size_t)-1)
#define STOP_FILLING ((size_t)-2)

ایستا size_t mime_subparts_read(نویسه *buffer, size_t size, size_t nitems,
                                 تهی *instream, bool *hasread);
ایستا curl_off_t mime_size(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#define QP_OK           1       /* Can be represented by itself. */
#define QP_SP           2       /* Space or tab. */
#define QP_CR           3       /* Carriage return. */
#define QP_LF           4       /* Line-feed. */
ایستا ثابت unsigned نویسه qp_class[] = {
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
ایستا ثابت نویسه aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#ifndef __VMS
#define filesize(name, stat_data) stat_data.st_size
#define fopen_read                curlx_fopen

#وگرنه

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
curl_off_t VmsRealFileSize(ثابت نویسه *name, ثابت curlx_struct_stat *stat_buf)
{
  نویسه buffer[8192];
  curl_off_t شمار;
  صحیح ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  اگر(!file)
    بازگشت 0;

  شمار = 0;
  ret_stat = 1;
  وقتی(ret_stat > 0) {
    ret_stat = fread(buffer, 1, اندازه(buffer), file);
    اگر(ret_stat)
      شمار += ret_stat;
  }
  curlx_fclose(file);

  بازگشت شمار;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
ایستا curl_off_t VmsSpecialSize(ثابت نویسه *name,
                                 ثابت curlx_struct_stat *stat_buf)
{
  انتخاب(stat_buf->st_fab_rfm) {
  حالت FAB$C_VAR:
  حالت FAB$C_VFC:
    بازگشت VmsRealFileSize(name, stat_buf);
    بشکن;
  پیشفرض:
    بازگشت stat_buf->st_size;
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
ایستا FILE *vmsfopenread(ثابت نویسه *file, ثابت نویسه *mode)
{
  curlx_struct_stat statbuf;
  صحیح res = curlx_stat(file, &statbuf);

  اگر(res != -1) {
    انتخاب(statbuf.st_fab_rfm) {
    حالت FAB$C_VAR:
    حالت FAB$C_VFC:
    حالت FAB$C_STMCR:
      بازگشت curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      بشکن;
    پیشفرض:
      بازگشت curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  بازگشت پوچ;
}

#define fopen_read vmsfopenread
#endif /* !__VMS */

/* Set readback state. */
ایستا تهی mimesetstate(ساختار mime_state *state,
                         شمارش mimestate tok, تهی *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->offset = 0;
}

/* Escape header string into allocated memory. */
ایستا نویسه *escape_string(ساختار Curl_easy *data,
                           ثابت نویسه *src, شمارش mimestrategy strategy)
{
  CURLcode result;
  ساختار dynbuf db;
  ثابت نویسه * ثابت *table;
  ثابت نویسه * ثابت *p;
  /* replace first character by rest of string. */
  ایستا ثابت نویسه * ثابت mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    پوچ
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  ایستا ثابت نویسه * ثابت formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    پوچ
  };

  table = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  اگر(strategy == MIMESTRATEGY_MAIL || (data && data->set.mime_formescape))
    table = mimetable;

  curlx_dyn_init(&db, CURL_MAX_درون_داد_LENGTH);

  برای(result = curlx_dyn_addn(&db, STRCONST("")); !result && *src; src++) {
    برای(p = table; *p && **p != *src; p++)
      ;

    اگر(*p)
      result = curlx_dyn_add(&db, *p + 1);
    وگرنه
      result = curlx_dyn_addn(&db, src, 1);
  }

  بازگشت curlx_dyn_ptr(&db);
}

/* Check if header matches. */
ایستا نویسه *match_header(ساختار curl_slist *hdr, ثابت نویسه *lbl, size_t len)
{
  نویسه *مقدار = پوچ;

  اگر(curl_strnequal(hdr->data, lbl, len) && hdr->data[len] == ':')
    برای(مقدار = hdr->data + len + 1; *مقدار == ' '; مقدار++)
      ;
  بازگشت مقدار;
}

/* Get a header from an slist. */
ایستا نویسه *جستجو_header(ساختار curl_slist *hdrlist,
                           ثابت نویسه *hdr, size_t len)
{
  نویسه *مقدار = پوچ;

  برای(; !مقدار && hdrlist; hdrlist = hdrlist->next)
    مقدار = match_header(hdrlist, hdr, len);

  بازگشت مقدار;
}

ایستا نویسه *strippath(ثابت نویسه *fullfile)
{
  نویسه *filename;
  نویسه *base;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  اگر(!filename)
    بازگشت پوچ;
  base = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  بازگشت base; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
ایستا تهی cleanup_encoder_state(ساختار mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
ایستا size_t encoder_nop_read(نویسه *buffer, size_t size, bool ateof,
                               ساختار curl_mimepart *part)
{
  ساختار mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (تهی)ateof;

  اگر(!size)
    بازگشت STOP_FILLING;

  اگر(size > insize)
    size = insize;

  اگر(size)
    memcpy(buffer, st->buf + st->bufbeg, size);

  st->bufbeg += size;
  بازگشت size;
}

ایستا curl_off_t encoder_nop_size(curl_mimepart *part)
{
  بازگشت part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
ایستا size_t encoder_7bit_read(نویسه *buffer, size_t size, bool ateof,
                                curl_mimepart *part)
{
  ساختار mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (تهی)ateof;

  اگر(!size)
    بازگشت STOP_FILLING;

  اگر(size > cursize)
    size = cursize;

  برای(cursize = 0; cursize < size; cursize++) {
    *buffer = st->buf[st->bufbeg];
    اگر(*buffer++ & 0x80)
      بازگشت cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  بازگشت cursize;
}

/* Base64 content encoder. */
ایستا size_t encoder_base64_read(نویسه *buffer, size_t size, bool ateof,
                                  curl_mimepart *part)
{
  ساختار mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  صحیح i;
  نویسه *ptr = buffer;

  وقتی(st->bufbeg < st->bufend) {
    /* Line full ? */
    اگر(st->pos > MAX_ENCODED_سطر_LENGTH - 4) {
      /* Yes, we need 2 characters for CRLF. */
      اگر(size < 2) {
        اگر(!cursize)
          بازگشت STOP_FILLING;
        بشکن;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      size -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    اگر(size < 4) {
      اگر(!cursize)
        بازگشت STOP_FILLING;
      بشکن;
    }
    اگر(st->bufend - st->bufbeg < 3)
      بشکن;

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
  اگر(ateof) {
    اگر(size < 4) {
      اگر(!cursize)
        بازگشت STOP_FILLING;
    }
    وگرنه {
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

  بازگشت cursize;
}

/* The maximum input size that does not cause an overflow. */
#define BASE64_MAX_درون_داد_SIZE                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_سطر_LENGTH + 2)) *                  \
    MAX_ENCODED_سطر_LENGTH / 4) * 3 - 3)

ایستا curl_off_t encoder_base64_size(curl_mimepart *part)
{
  curl_off_t size = part->datasize;

  اگر(size <= 0)
    بازگشت size;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  اگر(size > BASE64_MAX_درون_داد_SIZE)
    بازگشت -1;

  /* Compute base64 character count. */
  size = 4 * (1 + ((size - 1) / 3));

  /* Effective character count must include CRLFs. */
  بازگشت size + (2 * ((size - 1) / MAX_ENCODED_سطر_LENGTH));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
ایستا صحیح qp_lookahead_eol(ساختار mime_encoder_state *st, صحیح ateof, size_t n)
{
  n += st->bufbeg;
  اگر(n >= st->bufend && ateof)
    بازگشت 1;
  اگر(n + 2 > st->bufend)
    بازگشت ateof ? 0 : -1;
  اگر(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    بازگشت 1;
  بازگشت 0;
}

/* Quoted-printable encoder. */
ایستا size_t encoder_qp_read(نویسه *buffer, size_t size, bool ateof,
                              curl_mimepart *part)
{
  ساختار mime_encoder_state *st = &part->encstate;
  نویسه *ptr = buffer;
  size_t cursize = 0;
  صحیح softlinebreak;
  نویسه buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  وقتی(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    صحیح i = (unsigned نویسه)st->buf[st->bufbeg];
    buf[0] = (نویسه)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    انتخاب(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    حالت QP_OK:          /* Not a special character. */
      بشکن;
    حالت QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      انتخاب(qp_lookahead_eol(st, ateof, 1)) {
      حالت -1:          /* More input data needed. */
        بازگشت cursize;
      حالت 0:           /* No encoding needed. */
        بشکن;
      پیشفرض:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        بشکن;
      }
      بشکن;
    حالت QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      انتخاب(qp_lookahead_eol(st, ateof, 0)) {
      حالت -1:          /* Need more data. */
        بازگشت cursize;
      حالت 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        بشکن;
      پیشفرض:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        بشکن;
      }
      بشکن;
    پیشفرض:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      بشکن;
    }

    /* Be sure the encoded character fits within maximum line length. */
    اگر(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_سطر_LENGTH;
      اگر(!softlinebreak && st->pos + len == MAX_ENCODED_سطر_LENGTH) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        انتخاب(qp_lookahead_eol(st, ateof, consumed)) {
        حالت -1:        /* Need more data. */
          بازگشت cursize;
        حالت 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          بشکن;
        }
      }
      اگر(softlinebreak) {
        curlx_strcopy(buf, اندازه(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    اگر(len > size) {
      اگر(!cursize)
        بازگشت STOP_FILLING;
      بشکن;
    }

    /* Append to output buffer. */
    memcpy(ptr, buf, len);
    cursize += len;
    ptr += len;
    size -= len;
    st->pos += len;
    اگر(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  بازگشت cursize;
}

ایستا curl_off_t encoder_qp_size(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  بازگشت part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
ایستا size_t mime_mem_read(نویسه *buffer, size_t size, size_t nitems,
                            تهی *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.offset);
  (تهی)size;  /* Always 1 */

  اگر(!nitems)
    بازگشت STOP_FILLING;

  اگر(sz > nitems)
    sz = nitems;

  اگر(sz)
    memcpy(buffer, part->data + curlx_sotouz(part->state.offset), sz);

  بازگشت sz;
}

ایستا صحیح mime_mem_seek(تهی *instream, curl_off_t offset, صحیح whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  انتخاب(whence) {
  حالت SEEK_CUR:
    offset += part->state.offset;
    بشکن;
  حالت SEEK_END:
    offset += part->datasize;
    بشکن;
  }

  اگر(offset < 0 || offset > part->datasize)
    بازگشت CURL_SEEKFUNC_FAIL;

  part->state.offset = offset;
  بازگشت CURL_SEEKFUNC_OK;
}

ایستا تهی mime_mem_free(تهی *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->data);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
ایستا bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  اگر(part->fp)
    بازگشت FALSE;
  part->fp = fopen_read(part->data, "rb");
  بازگشت part->fp ? FALSE : TRUE;
}

ایستا size_t mime_file_read(نویسه *buffer, size_t size, size_t nitems,
                             تهی *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  اگر(!nitems)
    بازگشت STOP_FILLING;

  اگر(mime_open_file(part))
    بازگشت READ_ERROR;

  بازگشت fread(buffer, size, nitems, part->fp);
}

ایستا صحیح mime_file_seek(تهی *instream, curl_off_t offset, صحیح whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  اگر(whence == SEEK_SET && !offset && !part->fp)
    بازگشت CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  اگر(mime_open_file(part))
    بازگشت CURL_SEEKFUNC_FAIL;

  بازگشت curlx_fseek(part->fp, offset, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

ایستا تهی mime_file_free(تهی *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  اگر(part->fp) {
    curlx_fclose(part->fp);
    part->fp = پوچ;
  }
  curlx_safefree(part->data);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
ایستا size_t readback_bytes(ساختار mime_state *state,
                             نویسه *buffer, size_t bufsize,
                             ثابت نویسه *bytes, size_t numbytes,
                             ثابت نویسه *trail, size_t traillen)
{
  size_t sz;
  size_t offset = curlx_sotouz(state->offset);

  اگر(numbytes > offset) {
    sz = numbytes - offset;
    bytes += offset;
  }
  وگرنه {
    sz = offset - numbytes;
    اگر(sz >= traillen)
      بازگشت 0;
    bytes = trail + sz;
    sz = traillen - sz;
  }

  اگر(sz > bufsize)
    sz = bufsize;

  memcpy(buffer, bytes, sz);
  state->offset += sz;
  بازگشت sz;
}

/* Read a non-encoded part content. */

ایستا size_t read_part_content(curl_mimepart *part, نویسه *buffer,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  انتخاب(part->lastreadstatus) {
  حالت 0:
  حالت CURL_READFUNC_ABORT:
  حالت CURL_READFUNC_PAUSE:
  حالت READ_ERROR:
    بازگشت part->lastreadstatus;
  پیشفرض:
    بشکن;
  }

  /* If we can determine we are at end of part data, spare a read. */
  اگر(part->datasize != (curl_off_t)-1 &&
     part->state.offset >= part->datasize) {
    /* sz is already zero. */
  }
  وگرنه {
    انتخاب(part->kind) {
    حالت MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(buffer, 1, bufsize, part->arg, hasread);
      بشکن;
    حالت MIMEKIND_FILE:
      اگر(part->fp && feof(part->fp))
        بشکن;  /* At EOF. */
      FALLTHROUGH();
    پیشفرض:
      اگر(part->readfunc) {
        اگر(!(part->flags & MIME_FAST_READ)) {
          اگر(*hasread)
            بازگشت STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(buffer, 1, bufsize, part->arg);
      }
      بشکن;
    }
  }

  انتخاب(sz) {
  حالت STOP_FILLING:
    بشکن;
  حالت 0:
  حالت CURL_READFUNC_ABORT:
  حالت CURL_READFUNC_PAUSE:
  حالت READ_ERROR:
    part->lastreadstatus = sz;
    بشکن;
  پیشفرض:
    part->state.offset += sz;
    part->lastreadstatus = sz;
    بشکن;
  }

  بازگشت sz;
}

/* Read and encode part content. */
ایستا size_t read_encoded_part_content(curl_mimepart *part, نویسه *buffer,
                                        size_t bufsize, bool *hasread)
{
  ساختار mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  برای(;;) {
    اگر(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(buffer, bufsize, ateof, part);
      انتخاب(sz) {
      حالت 0:
        اگر(ateof)
          بازگشت cursize;
        بشکن;
      حالت READ_ERROR:
      حالت STOP_FILLING:
        بازگشت cursize ? cursize : sz;
      پیشفرض:
        cursize += sz;
        buffer += sz;
        bufsize -= sz;
        ادامه;
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
    اگر(st->bufend >= اندازه(st->buf))
      بازگشت cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           اندازه(st->buf) - st->bufend, hasread);
    انتخاب(sz) {
    حالت 0:
      ateof = TRUE;
      بشکن;
    حالت CURL_READFUNC_ABORT:
    حالت CURL_READFUNC_PAUSE:
    حالت READ_ERROR:
    حالت STOP_FILLING:
      بازگشت cursize ? cursize : sz;
    پیشفرض:
      st->bufend += sz;
      بشکن;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
ایستا size_t readback_part(curl_mimepart *part,
                            نویسه *buffer, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  وقتی(bufsize) {
    size_t sz = 0;
    ساختار curl_slist *hdr = (ساختار curl_slist *)part->state.ptr;
    انتخاب(part->state.state) {
    حالت MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      بشکن;
    حالت MIMESTATE_USERHEADERS:
      اگر(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, پوچ);
        بشکن;
      }
      اگر(match_header(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->next);
        بشکن;
      }
      FALLTHROUGH();
    حالت MIMESTATE_CURLHEADERS:
      اگر(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      وگرنه {
        sz = readback_bytes(&part->state, buffer, bufsize,
                            hdr->data, strlen(hdr->data), STRCONST("\r\n"));
        اگر(!sz)
          mimesetstate(&part->state, part->state.state, hdr->next);
      }
      بشکن;
    حالت MIMESTATE_EOH:
      sz = readback_bytes(&part->state, buffer, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      اگر(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, پوچ);
      بشکن;
    حالت MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, پوچ);
      بشکن;
    حالت MIMESTATE_CONTENT:
      اگر(part->encoder)
        sz = read_encoded_part_content(part, buffer, bufsize, hasread);
      وگرنه
        sz = read_part_content(part, buffer, bufsize, hasread);
      انتخاب(sz) {
      حالت 0:
        mimesetstate(&part->state, MIMESTATE_END, پوچ);
        /* Try sparing open file descriptors. */
        اگر(part->kind == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = پوچ;
        }
        FALLTHROUGH();
      حالت CURL_READFUNC_ABORT:
      حالت CURL_READFUNC_PAUSE:
      حالت READ_ERROR:
      حالت STOP_FILLING:
        بازگشت cursize ? cursize : sz;
      }
      بشکن;
    حالت MIMESTATE_END:
      بازگشت cursize;
    پیشفرض:
      بشکن;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    bufsize -= sz;
  }

  بازگشت cursize;
}

/* Readback from mime. Warning: not a read callback function. */
ایستا size_t mime_subparts_read(نویسه *buffer, size_t size, size_t nitems,
                                 تهی *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (تهی)size;  /* Always 1 */

  وقتی(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    انتخاب(mime->state.state) {
    حالت MIMESTATE_BEGIN:
    حالت MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.offset += 2;
      بشکن;
    حالت MIMESTATE_BOUNDARY1:
      sz = readback_bytes(&mime->state, buffer, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      اگر(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      بشکن;
    حالت MIMESTATE_BOUNDARY2:
      اگر(part)
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      وگرنه
        sz = readback_bytes(&mime->state, buffer, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      اگر(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      بشکن;
    حالت MIMESTATE_CONTENT:
      اگر(!part) {
        mimesetstate(&mime->state, MIMESTATE_END, پوچ);
        بشکن;
      }
      sz = readback_part(part, buffer, nitems, hasread);
      انتخاب(sz) {
      حالت CURL_READFUNC_ABORT:
      حالت CURL_READFUNC_PAUSE:
      حالت READ_ERROR:
      حالت STOP_FILLING:
        بازگشت cursize ? cursize : sz;
      حالت 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        بشکن;
      }
      بشکن;
    حالت MIMESTATE_END:
      بازگشت cursize;
    پیشفرض:
      بشکن;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    buffer += sz;
    nitems -= sz;
  }

  بازگشت cursize;
}

ایستا صحیح mime_part_rewind(curl_mimepart *part)
{
  صحیح res = CURL_SEEKFUNC_OK;
  شمارش mimestate targetstate = MIMESTATE_BEGIN;

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
        بشکن;
      حالت -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        بشکن;
      پیشفرض:
        res = CURL_SEEKFUNC_FAIL;
        بشکن;
      }
    }
  }

  اگر(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, پوچ);

  part->lastreadstatus = 1; /* Successful read status. */
  بازگشت res;
}

ایستا صحیح mime_subparts_seek(تهی *instream, curl_off_t offset, صحیح whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  صحیح rc = CURL_SEEKFUNC_OK;

  اگر(whence != SEEK_SET || offset)
    بازگشت CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  اگر(mime->state.state == MIMESTATE_BEGIN)
    بازگشت CURL_SEEKFUNC_OK;           /* Already rewound. */

  برای(part = mime->firstpart; part; part = part->nextpart) {
    صحیح res = mime_part_rewind(part);
    اگر(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  اگر(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, پوچ);

  بازگشت rc;
}

/* Release part content. */
ایستا تهی cleanup_part_content(curl_mimepart *part)
{
  اگر(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = پوچ;
  part->seekfunc = پوچ;
  part->freefunc = پوچ;
  part->arg = (تهی *)part;           /* Defaults to part itself. */
  part->data = پوچ;
  part->fp = پوچ;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->kind = MIMEKIND_NONE;
  part->flags &= ~(unsigned صحیح)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

ایستا تهی mime_subparts_free(تهی *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  اگر(mime && mime->parent) {
    mime->parent->freefunc = پوچ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
ایستا تهی mime_subparts_unbind(تهی *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  اگر(mime && mime->parent) {
    mime->parent->freefunc = پوچ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = پوچ;
  }
}

تهی Curl_mime_cleanpart(curl_mimepart *part)
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
تهی curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  اگر(!mime)
    بازگشت;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  وقتی(mime) {
    part = mime->firstpart;
    اگر(part) {
      mime->firstpart = part->nextpart;
      اگر(part->kind == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = پوچ;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    وگرنه {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

ایستا bool mime_is_too_deep(ثابت curl_mimepart *part, size_t call_depth)
{
  DEBUGASSERT(part);

  اگر(++call_depth > MAX_MIME_LEVELS)
    بازگشت TRUE;

  اگر(part->kind == MIMEKIND_MULTIPART && part->arg) {
    ثابت curl_mime *mime = (ثابت curl_mime *)part->arg;
    ثابت curl_mimepart *s;

    برای(s = mime->firstpart; s; s = s->nextpart) {
      اگر(mime_is_too_deep(s, call_depth))
        بازگشت TRUE;
    }
  }

  بازگشت FALSE;
}

ایستا CURLcode mime_duppart(ساختار Curl_easy *data,
                             curl_mimepart *dst, ثابت curl_mimepart *src,
                             size_t call_depth)
{
  curl_mime *mime;
  curl_mimepart *d;
  ثابت curl_mimepart *s;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  اگر(++call_depth > MAX_MIME_LEVELS)
    بازگشت CURLE_TOO_LARGE;

  /* Duplicate content. */
  انتخاب(src->kind) {
  حالت MIMEKIND_NONE:
    بشکن;
  حالت MIMEKIND_DATA:
    result = curl_mime_data(dst, src->data, (size_t)src->datasize);
    بشکن;
  حالت MIMEKIND_FILE:
    result = curl_mime_filedata(dst, src->data);
    /* Do not abort duplication if file is not readable. */
    اگر(result == CURLE_READ_ERROR)
      result = CURLE_OK;
    بشکن;
  حالت MIMEKIND_CALLBACK:
    result = curl_mime_data_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    بشکن;
  حالت MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(data);
    result = mime ? curl_mime_subparts(dst, mime) : CURLE_OUT_OF_MEMORY;

    /* Duplicate subparts. */
    برای(s = ((curl_mime *)src->arg)->firstpart; !result && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      result = d ? mime_duppart(data, d, s, call_depth) : CURLE_OUT_OF_MEMORY;
    }
    بشکن;
  پیشفرض:  /* Invalid kind: should not occur. */
    DEBUGF(infof(data, "invalid MIMEKIND* attempt"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;  /* Internal error? */
    بشکن;
  }

  /* Duplicate headers. */
  اگر(!result && src->userheaders) {
    ساختار curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    اگر(!hdrs)
      result = CURLE_OUT_OF_MEMORY;
    وگرنه {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      result = curl_mime_headers(dst, hdrs, TRUE);
      اگر(result)
        curl_slist_free_all(hdrs);
    }
  }

  اگر(!result) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    result = curl_mime_type(dst, src->mimetype);
  }
  اگر(!result)
    result = curl_mime_name(dst, src->name);
  اگر(!result)
    result = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  اگر(result)
    Curl_mime_cleanpart(dst);

  بازگشت result;
}

CURLcode Curl_mime_duppart(ساختار Curl_easy *data,
                           curl_mimepart *dst, ثابت curl_mimepart *src)
{
  اگر(!dst || !src)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(mime_is_too_deep(src, 0))
    بازگشت CURLE_TOO_LARGE;

  بازگشت mime_duppart(data, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(تهی *easy)
{
  curl_mime *mime = curlx_malloc(اندازه(*mime));

  اگر(mime) {
    mime->parent = پوچ;
    mime->firstpart = پوچ;
    mime->lastpart = پوچ;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    اگر(Curl_rand_alnum(easy,
                       (unsigned نویسه *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      بازگشت پوچ;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, پوچ);
  }

  بازگشت mime;
}

/* Initialize a mime part. */
تهی Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, اندازه(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, پوچ);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  اگر(!mime)
    بازگشت پوچ;

  part = curlx_malloc(اندازه(*part));

  اگر(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    اگر(mime->lastpart)
      mime->lastpart->nextpart = part;
    وگرنه
      mime->firstpart = part;

    mime->lastpart = part;
  }

  بازگشت part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ثابت نویسه *name)
{
  اگر(!part)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->name);

  اگر(name) {
    part->name = curlx_strdup(name);
    اگر(!part->name)
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  بازگشت CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ثابت نویسه *filename)
{
  اگر(!part)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->filename);

  اگر(filename) {
    part->filename = curlx_strdup(filename);
    اگر(!part->filename)
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  بازگشت CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_data(curl_mimepart *part, ثابت نویسه *data, size_t datasize)
{
  اگر(!part)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  اگر(data) {
    اگر(datasize == CURL_ZERO_TERMINATED)
      datasize = strlen(data);

    part->data = curlx_memdup0(data, datasize);
    اگر(!part->data)
      بازگشت CURLE_OUT_OF_MEMORY;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->kind = MIMEKIND_DATA;
  }

  بازگشت CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ثابت نویسه *filename)
{
  CURLcode result = CURLE_OK;

  اگر(!part)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  اگر(filename) {
    نویسه *base;
    curlx_struct_stat sbuf;

    اگر(curlx_stat(filename, &sbuf))
      result = CURLE_READ_ERROR;
    وگرنه {
      part->data = curlx_strdup(filename);
      اگر(!part->data)
        result = CURLE_OUT_OF_MEMORY;
      وگرنه {
        part->datasize = -1;
        اگر(S_ISREG(sbuf.st_mode)) {
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
        اگر(!base)
          result = CURLE_OUT_OF_MEMORY;
        وگرنه {
          result = curl_mime_filename(part, base);
          curlx_free(base);
        }
      }
    }
  }
  بازگشت result;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ثابت نویسه *mimetype)
{
  اگر(!part)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  curlx_safefree(part->mimetype);

  اگر(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    اگر(!part->mimetype)
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  بازگشت CURLE_OK;
}

ایستا ثابت ساختار mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_size },
  { "8bit", encoder_nop_read, encoder_nop_size },
  { "7bit", encoder_7bit_read, encoder_nop_size },
  { "base64", encoder_base64_read, encoder_base64_size },
  { "quoted-printable", encoder_qp_read, encoder_qp_size },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ثابت نویسه *encoding)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  ثابت ساختار mime_encoder *mep;

  اگر(!part)
    بازگشت result;

  part->encoder = پوچ;

  اگر(!encoding)
    بازگشت CURLE_OK;    /* Removing current encoder. */

  برای(mep = encoders; mep->name; mep++)
    اگر(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      result = CURLE_OK;
    }

  بازگشت result;
}

/* Set mime part headers. */
CURLcode curl_mime_headers(curl_mimepart *part,
                           ساختار curl_slist *headers, صحیح take_ownership)
{
  اگر(!part)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(part->flags & MIME_USERHEADERS_OWNER) {
    اگر(part->userheaders != headers)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(unsigned صحیح)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = headers;
  اگر(headers && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  بازگشت CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, تهی *arg)
{
  اگر(!part)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  cleanup_part_content(part);

  اگر(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->kind = MIMEKIND_CALLBACK;
  }

  بازگشت CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_set_subparts(curl_mimepart *part,
                                curl_mime *subparts, صحیح take_ownership)
{
  curl_mime *ریشه;

  اگر(!part)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  /* Accept setting twice the same subparts. */
  اگر(part->kind == MIMEKIND_MULTIPART && part->arg == subparts)
    بازگشت CURLE_OK;

  cleanup_part_content(part);

  اگر(subparts) {
    /* Should not have been attached already. */
    اگر(subparts->parent)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

    /* Should not be the part's root. */
    ریشه = part->parent;
    اگر(ریشه) {
      وقتی(ریشه->parent && ریشه->parent->parent)
        ریشه = ریشه->parent->parent;
      اگر(subparts == ریشه) {
        /* cannot add as a subpart of itself. */
        بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    اگر(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      بازگشت CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->kind = MIMEKIND_MULTIPART;
  }

  بازگشت CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  بازگشت Curl_mime_set_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(نویسه *buffer, size_t size, size_t nitems, تهی *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (تهی)size;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  انجامبده {
    hasread = FALSE;
    ret = readback_part(part, buffer, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } وقتی(ret == STOP_FILLING);

  بازگشت ret;
}

/* Rewind mime stream. */
ایستا CURLcode mime_rewind(curl_mimepart *part)
{
  بازگشت mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
ایستا size_t slist_size(ساختار curl_slist *s,
                         size_t overhead, ثابت نویسه *skip, size_t skiplen)
{
  size_t size = 0;

  برای(; s; s = s->next)
    اگر(!skip || !match_header(s, skip, skiplen))
      size += strlen(s->data) + overhead;
  بازگشت size;
}

/* Get/compute multipart size. */
ایستا curl_off_t multipart_size(curl_mime *mime)
{
  curl_off_t size;
  curl_off_t boundarysize;
  curl_mimepart *part;

  اگر(!mime)
    بازگشت 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  size = boundarysize;  /* Final boundary - CRLF after headers. */

  برای(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_size(part);

    اگر(sz < 0)
      size = sz;

    اگر(size >= 0)
      size += boundarysize + sz;
  }

  بازگشت size;
}

/* Get/compute mime size. */
ایستا curl_off_t mime_size(curl_mimepart *part)
{
  curl_off_t size;

  اگر(part->kind == MIMEKIND_MULTIPART)
    part->datasize = multipart_size(part->arg);

  size = part->datasize;

  اگر(part->encoder)
    size = part->encoder->sizefunc(part);

  اگر(size >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    size += slist_size(part->curlheaders, 2, پوچ, 0);
    size += slist_size(part->userheaders, 2, STRCONST("Content-Type"));
    size += 2;    /* CRLF after headers. */
  }
  بازگشت size;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_add_header(ساختار curl_slist **slp, ثابت نویسه *fmt, ...)
{
  ساختار curl_slist *hdr = پوچ;
  نویسه *s = پوچ;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  اگر(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    اگر(hdr)
      *slp = hdr;
    وگرنه
      curlx_free(s);
  }

  بازگشت hdr ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

/* Add a content type header. */
ایستا CURLcode add_content_type(ساختار curl_slist **slp,
                                 ثابت نویسه *type, ثابت نویسه *boundary)
{
  بازگشت Curl_mime_add_header(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ثابت نویسه *Curl_mime_contenttype(ثابت نویسه *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  ساختار ContentType {
    ثابت نویسه *extension;
    ثابت نویسه *type;
  };
  ایستا ثابت ساختار ContentType ctts[] = {
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
    size_t len1 = strlen(filename);
    ثابت نویسه *nameend = filename + len1;
    unsigned صحیح i;

    برای(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = strlen(ctts[i].extension);

      اگر(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        بازگشت ctts[i].type;
    }
  }
  بازگشت پوچ;
}

ایستا bool content_type_match(ثابت نویسه *contenttype,
                               ثابت نویسه *target, size_t len)
{
  اگر(contenttype && curl_strnequal(contenttype, target, len))
    انتخاب(contenttype[len]) {
    حالت '\0':
    حالت '\t':
    حالت '\r':
    حالت '\n':
    حالت ' ':
    حالت ';':
      بازگشت TRUE;
    }
  بازگشت FALSE;
}

ایستا CURLcode add_content_disposition(ساختار Curl_easy *data,
                                        curl_mimepart *part,
                                        ثابت نویسه *disposition,
                                        ثابت نویسه *contenttype,
                                        شمارش mimestrategy strategy)
{
  اگر(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  اگر(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = پوچ;

  اگر(disposition) {
    CURLcode result = CURLE_OK;
    نویسه *name = پوچ;
    نویسه *filename = پوچ;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (data && data->set.mime_formescape);
    اگر(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

    اگر(part->name) {
      name = escape_string(data, part->name, strategy);
      اگر(!name)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
    اگر(part->filename) {
      filename = escape_string(data, part->filename, strategy);
      اگر(!filename)
        result = CURLE_OUT_OF_MEMORY;
    }
    اگر(!result)
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
    اگر(result)
      بازگشت result;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode mime_prepare_headers(ساختار Curl_easy *data,
                                     curl_mimepart *part,
                                     ثابت نویسه *contenttype,
                                     ثابت نویسه *disposition,
                                     شمارش mimestrategy strategy,
                                     size_t call_depth)
{
  curl_mime *mime = پوچ;
  ثابت نویسه *boundary = پوچ;
  نویسه *customct;
  ثابت نویسه *cte = پوچ;
  CURLcode result = CURLE_OK;

  اگر(++call_depth > MAX_MIME_LEVELS)
    بازگشت CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = پوچ;

  /* Be sure we will not access old headers later. */
  اگر(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, پوچ);

  /* Check if content type is specified. */
  customct = part->mimetype;
  اگر(!customct)
    customct = جستجو_header(part->userheaders, STRCONST("Content-Type"));
  اگر(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  اگر(!contenttype) {
    انتخاب(part->kind) {
    حالت MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      بشکن;
    حالت MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      اگر(!contenttype)
        contenttype = Curl_mime_contenttype(part->data);
      اگر(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      بشکن;
    پیشفرض:
      contenttype = Curl_mime_contenttype(part->filename);
      بشکن;
    }
  }

  اگر(part->kind == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    اگر(mime)
      boundary = mime->boundary;
  }
  وگرنه اگر(contenttype && !customct &&
          content_type_match(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = پوچ;

  /* Issue content-disposition header only if not already set by caller. */
  اگر(!جستجو_header(part->userheaders, STRCONST("Content-Disposition"))) {
    result = add_content_disposition(data, part, disposition,
                                     contenttype, strategy);
    اگر(result)
      بازگشت result;
  }

  /* Issue Content-Type header. */
  اگر(contenttype) {
    result = add_content_type(&part->curlheaders, contenttype, boundary);
    اگر(result)
      بازگشت result;
  }

  /* Content-Transfer-Encoding header. */
  اگر(!جستجو_header(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    اگر(part->encoder)
      cte = part->encoder->name;
    وگرنه اگر(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->kind != MIMEKIND_MULTIPART)
      cte = "8bit";
    اگر(cte) {
      result = Curl_mime_add_header(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      اگر(result)
        بازگشت result;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  اگر(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  اگر(part->kind == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = پوچ;
    اگر(content_type_match(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    برای(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      result = mime_prepare_headers(data, subpart, پوچ,
                                    disposition, strategy, call_depth);
      اگر(result)
        بازگشت result;
    }
  }
  بازگشت result;
}

CURLcode Curl_mime_prepare_headers(ساختار Curl_easy *data,
                                   curl_mimepart *part,
                                   ثابت نویسه *contenttype,
                                   ثابت نویسه *disposition,
                                   شمارش mimestrategy strategy)
{
  بازگشت mime_prepare_headers(data, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
ایستا تهی mime_unpause(curl_mimepart *part)
{
  اگر(part) {
    اگر(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    اگر(part->kind == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      اگر(mime) {
        curl_mimepart *subpart;

        برای(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

ساختار cr_mime_ctx {
  ساختار Curl_creader super;
  curl_mimepart *part;
  curl_off_t total_len;
  curl_off_t read_len;
  CURLcode error_result;
  ساختار bufq tmpbuf;
  BIT(seen_eos);
  BIT(errored);
};

ایستا CURLcode cr_mime_init(ساختار Curl_easy *data,
                             ساختار Curl_creader *reader)
{
  ساختار cr_mime_ctx *ctx = reader->ctx;
  (تهی)data;
  ctx->total_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  بازگشت CURLE_OK;
}

ایستا تهی cr_mime_close(ساختار Curl_easy *data,
                          ساختار Curl_creader *reader)
{
  ساختار cr_mime_ctx *ctx = reader->ctx;
  (تهی)data;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
ایستا CURLcode cr_mime_read(ساختار Curl_easy *data,
                             ساختار Curl_creader *reader,
                             نویسه *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  ساختار cr_mime_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;
  نویسه tmp[256];

  /* Once we have errored, we will return the same error forever */
  اگر(ctx->errored) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (صحیح)ctx->error_result);
    *pnread = 0;
    *peos = FALSE;
    بازگشت ctx->error_result;
  }
  اگر(ctx->seen_eos) {
    CURL_TRC_READ(data, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    بازگشت CURLE_OK;
  }
  /* respect length limitations */
  اگر(ctx->total_len >= 0) {
    curl_off_t remain = ctx->total_len - ctx->read_len;
    اگر(remain <= 0)
      blen = 0;
    وگرنه اگر(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  اگر(!Curl_bufq_is_empty(&ctx->tmpbuf)) {
    result = Curl_bufq_read(&ctx->tmpbuf, (unsigned نویسه *)buf, blen, &nread);
    اگر(result) {
      ctx->errored = TRUE;
      ctx->error_result = result;
      بازگشت result;
    }
  }
  وگرنه اگر(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, اندازه(tmp), ctx->part);
    اگر(nread <= اندازه(tmp)) {
      size_t n;
      result = Curl_bufq_write(&ctx->tmpbuf, (unsigned نویسه *)tmp, nread, &n);
      اگر(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        بازگشت result;
      }
      /* stored it, read again */
      result = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      اگر(result) {
        ctx->errored = TRUE;
        ctx->error_result = result;
        بازگشت result;
      }
    }
  }
  وگرنه
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(data, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  انتخاب(nread) {
  حالت 0:
    اگر((ctx->total_len >= 0) && (ctx->read_len < ctx->total_len)) {
      failf(data, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->total_len);
      بازگشت CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->seen_eos = TRUE;
    بشکن;

  حالت CURL_READFUNC_ABORT:
    failf(data, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_ABORTED_BY_CALLBACK;
    result = CURLE_ABORTED_BY_CALLBACK;
    بشکن;

  حالت CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(data, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    result = Curl_xfer_pause_send(data, TRUE);
    بشکن; /* nothing was read */

  حالت STOP_FILLING:
  حالت READ_ERROR:
    failf(data, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_result = CURLE_READ_ERROR;
    result = CURLE_READ_ERROR;
    بشکن;

  پیشفرض:
    اگر(nread > blen) {
      /* the read function returned a too large value */
      failf(data, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_result = CURLE_READ_ERROR;
      بازگشت CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    اگر(ctx->total_len >= 0)
      ctx->seen_eos = (ctx->read_len >= ctx->total_len);
    *pnread = nread;
    *peos = (bool)ctx->seen_eos;
    بشکن;
  }

  CURL_TRC_READ(data, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->total_len, ctx->read_len, (صحیح)result, *pnread, *peos);
  بازگشت result;
}

ایستا bool cr_mime_needs_rewind(ساختار Curl_easy *data,
                                 ساختار Curl_creader *reader)
{
  ساختار cr_mime_ctx *ctx = reader->ctx;
  (تهی)data;
  بازگشت ctx->read_len > 0;
}

ایستا curl_off_t cr_mime_total_length(ساختار Curl_easy *data,
                                       ساختار Curl_creader *reader)
{
  ساختار cr_mime_ctx *ctx = reader->ctx;
  (تهی)data;
  بازگشت ctx->total_len;
}

ایستا CURLcode cr_mime_resume_from(ساختار Curl_easy *data,
                                    ساختار Curl_creader *reader,
                                    curl_off_t offset)
{
  ساختار cr_mime_ctx *ctx = reader->ctx;

  اگر(offset > 0) {
    curl_off_t passed = 0;

    انجامبده {
      نویسه scratch[4 * 1024];
      size_t readthisamountnow =
        (offset - passed > (curl_off_t)اندازه(scratch)) ?
        اندازه(scratch) :
        curlx_sotouz(offset - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      اگر((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(data, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        بازگشت CURLE_READ_ERROR;
      }
    } وقتی(passed < offset);

    /* now, decrease the size of the read */
    اگر(ctx->total_len > 0) {
      ctx->total_len -= offset;

      اگر(ctx->total_len <= 0) {
        failf(data, "Mime post already completely uploaded");
        بازگشت CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode cr_mime_cntrl(ساختار Curl_easy *data,
                              ساختار Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  ساختار cr_mime_ctx *ctx = reader->ctx;
  انتخاب(opcode) {
  حالت CURL_CRCNTRL_REWIND: {
    CURLcode result = mime_rewind(ctx->part);
    اگر(result)
      failf(data, "Cannot rewind mime/post data");
    بازگشت result;
  }
  حالت CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    بشکن;
  حالت CURL_CRCNTRL_CLEAR_EOS:
    ctx->seen_eos = FALSE;
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا bool cr_mime_is_paused(ساختار Curl_easy *data,
                              ساختار Curl_creader *reader)
{
  ساختار cr_mime_ctx *ctx = reader->ctx;
  (تهی)data;
  بازگشت ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

ایستا ثابت ساختار Curl_crtype cr_mime = {
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
  اندازه(ساختار cr_mime_ctx)
};

CURLcode Curl_creader_set_mime(ساختار Curl_easy *data, curl_mimepart *part)
{
  ساختار Curl_creader *r;
  ساختار cr_mime_ctx *ctx;
  CURLcode result;

  result = Curl_creader_create(&r, data, &cr_mime, CURL_CR_CLIENT);
  اگر(result)
    بازگشت result;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  result = mime_rewind(ctx->part);
  اگر(result) {
    Curl_creader_free(data, r);
    بازگشت result;
  }
  ctx->total_len = mime_size(ctx->part);

  بازگشت Curl_creader_set(data, r);
}

#وگرنه /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (تهی)easy;
  بازگشت پوچ;
}

تهی curl_mime_free(curl_mime *mime)
{
  (تهی)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (تهی)mime;
  بازگشت پوچ;
}

CURLcode curl_mime_name(curl_mimepart *part, ثابت نویسه *name)
{
  (تهی)part;
  (تهی)name;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filename(curl_mimepart *part, ثابت نویسه *filename)
{
  (تهی)part;
  (تهی)filename;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_type(curl_mimepart *part, ثابت نویسه *mimetype)
{
  (تهی)part;
  (تهی)mimetype;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ثابت نویسه *encoding)
{
  (تهی)part;
  (تهی)encoding;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data(curl_mimepart *part, ثابت نویسه *data, size_t datasize)
{
  (تهی)part;
  (تهی)data;
  (تهی)datasize;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ثابت نویسه *filename)
{
  (تهی)part;
  (تهی)filename;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_data_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           تهی *arg)
{
  (تهی)part;
  (تهی)datasize;
  (تهی)readfunc;
  (تهی)seekfunc;
  (تهی)freefunc;
  (تهی)arg;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (تهی)part;
  (تهی)subparts;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode curl_mime_headers(curl_mimepart *part,
                           ساختار curl_slist *headers, صحیح take_ownership)
{
  (تهی)part;
  (تهی)headers;
  (تهی)take_ownership;
  بازگشت CURLE_NOT_BUILT_IN;
}

CURLcode Curl_mime_add_header(ساختار curl_slist **slp, ثابت نویسه *fmt, ...)
{
  (تهی)slp;
  (تهی)fmt;
  بازگشت CURLE_NOT_BUILT_IN;
}

#endif /* if disabled */

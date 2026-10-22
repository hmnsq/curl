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
#అన్తర్భావ "curl_setup.h"

నిర్మాణం Curl_easy;

#అన్తర్భావ "mime.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/basename.h"
#అన్తర్భావ "curlx/strcopy.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curlx/base64.h"

#ఒకవేళ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#అన్తర్భావ "rand.h"
#అన్తర్భావ "slist.h"
#అన్తర్భావ "curlx/dynbuf.h"

#నిర్వచించు MAX_MIME_స్థాయిలు 40 /* avoid deep nesting */

#నిర్వచించు READ_ERROR   ((size_t)-1)
#నిర్వచించు STOP_FILLING ((size_t)-2)

స్థిర size_t mime_subparts_read(అక్షరం *బఫరు, size_t కొలత, size_t nitems,
                                 శూన్య *instream, bool *hasread);
స్థిర curl_off_t mime_కొలత(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#నిర్వచించు QP_OK           1       /* Can be represented by itself. */
#నిర్వచించు QP_SP           2       /* Space or tab. */
#నిర్వచించు QP_CR           3       /* Carriage return. */
#నిర్వచించు QP_LF           4       /* Line-feed. */
స్థిర స్థిరరాశి గుర్తులేని అక్షరం qp_class[] = {
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
స్థిర స్థిరరాశి అక్షరం aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#నిర్వచితం_కాకపోతే __VMS
#నిర్వచించు filesize(name, stat_దత్తాంశం) stat_దత్తాంశం.st_కొలత
#నిర్వచించు fopen_read                curlx_fopen

#లేకపోతే

#అన్తర్భావ <fabdef.h>
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
curl_off_t Vms_Real_File_కొలత(స్థిరరాశి అక్షరం *name, స్థిరరాశి curlx_struct_stat *stat_buf)
{
  అక్షరం బఫరు[8192];
  curl_off_t లెక్క;
  పూర్ణాంకం ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  ఒకవేళ(!file)
    తిరిగి 0;

  లెక్క = 0;
  ret_stat = 1;
  అయితే(ret_stat > 0) {
    ret_stat = fread(బఫరు, 1, పరిమాణం(బఫరు), file);
    ఒకవేళ(ret_stat)
      లెక్క += ret_stat;
  }
  curlx_fclose(file);

  తిరిగి లెక్క;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
స్థిర curl_off_t Vms_Special_కొలత(స్థిరరాశి అక్షరం *name,
                                 స్థిరరాశి curlx_struct_stat *stat_buf)
{
  ఎంపిక(stat_buf->st_fab_rfm) {
  స్థితి FAB$C_VAR:
  స్థితి FAB$C_VFC:
    తిరిగి Vms_Real_File_కొలత(name, stat_buf);
    ఆపు;
  అప్రమేయ:
    తిరిగి stat_buf->st_కొలత;
  }
}

#నిర్వచించు filesize(name, stat_దత్తాంశం) Vms_Special_కొలత(name, &stat_దత్తాంశం)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
స్థిర FILE *vmsfopenread(స్థిరరాశి అక్షరం *file, స్థిరరాశి అక్షరం *బహుళకం)
{
  curlx_struct_stat statbuf;
  పూర్ణాంకం res = curlx_stat(file, &statbuf);

  ఒకవేళ(res != -1) {
    ఎంపిక(statbuf.st_fab_rfm) {
    స్థితి FAB$C_VAR:
    స్థితి FAB$C_VFC:
    స్థితి FAB$C_STMCR:
      తిరిగి curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      ఆపు;
    అప్రమేయ:
      తిరిగి curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  తిరిగి ఖాళీ;
}

#నిర్వచించు fopen_read vmsfopenread
#అయితే_ముగింపు /* !__VMS */

/* Set readback state. */
స్థిర శూన్య mimesetstate(నిర్మాణం mime_state *state,
                         గణన mimestate tok, శూన్య *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->స్థానభ్రంశం = 0;
}

/* Escape header string into allocated memory. */
స్థిర అక్షరం *escape_string(నిర్మాణం Curl_easy *దత్తాంశం,
                           స్థిరరాశి అక్షరం *src, గణన mimestrategy strategy)
{
  CURLcode ఫలితం;
  నిర్మాణం dynbuf db;
  స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక;
  స్థిరరాశి అక్షరం * స్థిరరాశి *p;
  /* replace first character by rest of string. */
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    ఖాళీ
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    ఖాళీ
  };

  పట్టిక = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  ఒకవేళ(strategy == MIMESTRATEGY_MAIL || (దత్తాంశం && దత్తాంశం->సమితి.mime_formescape))
    పట్టిక = mimetable;

  curlx_dyn_init(&db, CURL_MAX_ఆదానం_నిడివి);

  కొరకు(ఫలితం = curlx_dyn_addn(&db, STRCONST("")); !ఫలితం && *src; src++) {
    కొరకు(p = పట్టిక; *p && **p != *src; p++)
      ;

    ఒకవేళ(*p)
      ఫలితం = curlx_dyn_కూడు(&db, *p + 1);
    లేకపోతే
      ఫలితం = curlx_dyn_addn(&db, src, 1);
  }

  తిరిగి curlx_dyn_ptr(&db);
}

/* Check if header matches. */
స్థిర అక్షరం *సరిపోలిక_శీర్షిక(నిర్మాణం curl_slist *hdr, స్థిరరాశి అక్షరం *lbl, size_t len)
{
  అక్షరం *విలువ = ఖాళీ;

  ఒకవేళ(curl_strnequal(hdr->దత్తాంశం, lbl, len) && hdr->దత్తాంశం[len] == ':')
    కొరకు(విలువ = hdr->దత్తాంశం + len + 1; *విలువ == ' '; విలువ++)
      ;
  తిరిగి విలువ;
}

/* Get a header from an slist. */
స్థిర అక్షరం *శోధన_శీర్షిక(నిర్మాణం curl_slist *hdrlist,
                           స్థిరరాశి అక్షరం *hdr, size_t len)
{
  అక్షరం *విలువ = ఖాళీ;

  కొరకు(; !విలువ && hdrlist; hdrlist = hdrlist->తదుపరి)
    విలువ = సరిపోలిక_శీర్షిక(hdrlist, hdr, len);

  తిరిగి విలువ;
}

స్థిర అక్షరం *strippath(స్థిరరాశి అక్షరం *fullfile)
{
  అక్షరం *filename;
  అక్షరం *పునాది;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  ఒకవేళ(!filename)
    తిరిగి ఖాళీ;
  పునాది = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  తిరిగి పునాది; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
స్థిర శూన్య cleanup_encoder_state(నిర్మాణం mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
స్థిర size_t encoder_nop_read(అక్షరం *బఫరు, size_t కొలత, bool ateof,
                               నిర్మాణం curl_mimepart *part)
{
  నిర్మాణం mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (శూన్య)ateof;

  ఒకవేళ(!కొలత)
    తిరిగి STOP_FILLING;

  ఒకవేళ(కొలత > insize)
    కొలత = insize;

  ఒకవేళ(కొలత)
    జ్ఞాపకం_నకలు(బఫరు, st->buf + st->bufbeg, కొలత);

  st->bufbeg += కొలత;
  తిరిగి కొలత;
}

స్థిర curl_off_t encoder_nop_కొలత(curl_mimepart *part)
{
  తిరిగి part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
స్థిర size_t encoder_7bit_read(అక్షరం *బఫరు, size_t కొలత, bool ateof,
                                curl_mimepart *part)
{
  నిర్మాణం mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (శూన్య)ateof;

  ఒకవేళ(!కొలత)
    తిరిగి STOP_FILLING;

  ఒకవేళ(కొలత > cursize)
    కొలత = cursize;

  కొరకు(cursize = 0; cursize < కొలత; cursize++) {
    *బఫరు = st->buf[st->bufbeg];
    ఒకవేళ(*బఫరు++ & 0x80)
      తిరిగి cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  తిరిగి cursize;
}

/* Base64 content encoder. */
స్థిర size_t encoder_base64_read(అక్షరం *బఫరు, size_t కొలత, bool ateof,
                                  curl_mimepart *part)
{
  నిర్మాణం mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  పూర్ణాంకం i;
  అక్షరం *ptr = బఫరు;

  అయితే(st->bufbeg < st->bufend) {
    /* Line full ? */
    ఒకవేళ(st->pos > MAX_ENCODED_పంక్తి_నిడివి - 4) {
      /* Yes, we need 2 characters for CRLF. */
      ఒకవేళ(కొలత < 2) {
        ఒకవేళ(!cursize)
          తిరిగి STOP_FILLING;
        ఆపు;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      కొలత -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    ఒకవేళ(కొలత < 4) {
      ఒకవేళ(!cursize)
        తిరిగి STOP_FILLING;
      ఆపు;
    }
    ఒకవేళ(st->bufend - st->bufbeg < 3)
      ఆపు;

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
    కొలత -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  ఒకవేళ(ateof) {
    ఒకవేళ(కొలత < 4) {
      ఒకవేళ(!cursize)
        తిరిగి STOP_FILLING;
    }
    లేకపోతే {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      ఒకవేళ(st->bufend != st->bufbeg) {

        ఒకవేళ(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        ఒకవేళ(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  తిరిగి cursize;
}

/* The maximum input size that does not cause an overflow. */
#నిర్వచించు BASE64_MAX_ఆదానం_కొలత                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_పంక్తి_నిడివి + 2)) *                  \
    MAX_ENCODED_పంక్తి_నిడివి / 4) * 3 - 3)

స్థిర curl_off_t encoder_base64_కొలత(curl_mimepart *part)
{
  curl_off_t కొలత = part->datasize;

  ఒకవేళ(కొలత <= 0)
    తిరిగి కొలత;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  ఒకవేళ(కొలత > BASE64_MAX_ఆదానం_కొలత)
    తిరిగి -1;

  /* Compute base64 character count. */
  కొలత = 4 * (1 + ((కొలత - 1) / 3));

  /* Effective character count must include CRLFs. */
  తిరిగి కొలత + (2 * ((కొలత - 1) / MAX_ENCODED_పంక్తి_నిడివి));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
స్థిర పూర్ణాంకం qp_lookahead_eol(నిర్మాణం mime_encoder_state *st, పూర్ణాంకం ateof, size_t n)
{
  n += st->bufbeg;
  ఒకవేళ(n >= st->bufend && ateof)
    తిరిగి 1;
  ఒకవేళ(n + 2 > st->bufend)
    తిరిగి ateof ? 0 : -1;
  ఒకవేళ(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    తిరిగి 1;
  తిరిగి 0;
}

/* Quoted-printable encoder. */
స్థిర size_t encoder_qp_read(అక్షరం *బఫరు, size_t కొలత, bool ateof,
                              curl_mimepart *part)
{
  నిర్మాణం mime_encoder_state *st = &part->encstate;
  అక్షరం *ptr = బఫరు;
  size_t cursize = 0;
  పూర్ణాంకం softlinebreak;
  అక్షరం buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  అయితే(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    పూర్ణాంకం i = (గుర్తులేని అక్షరం)st->buf[st->bufbeg];
    buf[0] = (అక్షరం)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    ఎంపిక(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    స్థితి QP_OK:          /* Not a special character. */
      ఆపు;
    స్థితి QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      ఎంపిక(qp_lookahead_eol(st, ateof, 1)) {
      స్థితి -1:          /* More input data needed. */
        తిరిగి cursize;
      స్థితి 0:           /* No encoding needed. */
        ఆపు;
      అప్రమేయ:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ఆపు;
      }
      ఆపు;
    స్థితి QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      ఎంపిక(qp_lookahead_eol(st, ateof, 0)) {
      స్థితి -1:          /* Need more data. */
        తిరిగి cursize;
      స్థితి 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        ఆపు;
      అప్రమేయ:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        ఆపు;
      }
      ఆపు;
    అప్రమేయ:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      ఆపు;
    }

    /* Be sure the encoded character fits within maximum line length. */
    ఒకవేళ(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_పంక్తి_నిడివి;
      ఒకవేళ(!softlinebreak && st->pos + len == MAX_ENCODED_పంక్తి_నిడివి) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        ఎంపిక(qp_lookahead_eol(st, ateof, consumed)) {
        స్థితి -1:        /* Need more data. */
          తిరిగి cursize;
        స్థితి 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          ఆపు;
        }
      }
      ఒకవేళ(softlinebreak) {
        curlx_strcopy(buf, పరిమాణం(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    ఒకవేళ(len > కొలత) {
      ఒకవేళ(!cursize)
        తిరిగి STOP_FILLING;
      ఆపు;
    }

    /* Append to output buffer. */
    జ్ఞాపకం_నకలు(ptr, buf, len);
    cursize += len;
    ptr += len;
    కొలత -= len;
    st->pos += len;
    ఒకవేళ(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  తిరిగి cursize;
}

స్థిర curl_off_t encoder_qp_కొలత(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  తిరిగి part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
స్థిర size_t mime_mem_read(అక్షరం *బఫరు, size_t కొలత, size_t nitems,
                            శూన్య *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.స్థానభ్రంశం);
  (శూన్య)కొలత;  /* Always 1 */

  ఒకవేళ(!nitems)
    తిరిగి STOP_FILLING;

  ఒకవేళ(sz > nitems)
    sz = nitems;

  ఒకవేళ(sz)
    జ్ఞాపకం_నకలు(బఫరు, part->దత్తాంశం + curlx_sotouz(part->state.స్థానభ్రంశం), sz);

  తిరిగి sz;
}

స్థిర పూర్ణాంకం mime_mem_seek(శూన్య *instream, curl_off_t స్థానభ్రంశం, పూర్ణాంకం whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ఎంపిక(whence) {
  స్థితి SEEK_CUR:
    స్థానభ్రంశం += part->state.స్థానభ్రంశం;
    ఆపు;
  స్థితి SEEK_END:
    స్థానభ్రంశం += part->datasize;
    ఆపు;
  }

  ఒకవేళ(స్థానభ్రంశం < 0 || స్థానభ్రంశం > part->datasize)
    తిరిగి CURL_SEEKFUNC_FAIL;

  part->state.స్థానభ్రంశం = స్థానభ్రంశం;
  తిరిగి CURL_SEEKFUNC_OK;
}

స్థిర శూన్య mime_mem_free(శూన్య *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->దత్తాంశం);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
స్థిర bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  ఒకవేళ(part->fp)
    తిరిగి FALSE;
  part->fp = fopen_read(part->దత్తాంశం, "rb");
  తిరిగి part->fp ? FALSE : TRUE;
}

స్థిర size_t mime_file_read(అక్షరం *బఫరు, size_t కొలత, size_t nitems,
                             శూన్య *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ఒకవేళ(!nitems)
    తిరిగి STOP_FILLING;

  ఒకవేళ(mime_open_file(part))
    తిరిగి READ_ERROR;

  తిరిగి fread(బఫరు, కొలత, nitems, part->fp);
}

స్థిర పూర్ణాంకం mime_file_seek(శూన్య *instream, curl_off_t స్థానభ్రంశం, పూర్ణాంకం whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  ఒకవేళ(whence == SEEK_SET && !స్థానభ్రంశం && !part->fp)
    తిరిగి CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  ఒకవేళ(mime_open_file(part))
    తిరిగి CURL_SEEKFUNC_FAIL;

  తిరిగి curlx_fseek(part->fp, స్థానభ్రంశం, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

స్థిర శూన్య mime_file_free(శూన్య *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  ఒకవేళ(part->fp) {
    curlx_fclose(part->fp);
    part->fp = ఖాళీ;
  }
  curlx_safefree(part->దత్తాంశం);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
స్థిర size_t readback_బైట్లు(నిర్మాణం mime_state *state,
                             అక్షరం *బఫరు, size_t bufsize,
                             స్థిరరాశి అక్షరం *బైట్లు, size_t numbytes,
                             స్థిరరాశి అక్షరం *trail, size_t traillen)
{
  size_t sz;
  size_t స్థానభ్రంశం = curlx_sotouz(state->స్థానభ్రంశం);

  ఒకవేళ(numbytes > స్థానభ్రంశం) {
    sz = numbytes - స్థానభ్రంశం;
    బైట్లు += స్థానభ్రంశం;
  }
  లేకపోతే {
    sz = స్థానభ్రంశం - numbytes;
    ఒకవేళ(sz >= traillen)
      తిరిగి 0;
    బైట్లు = trail + sz;
    sz = traillen - sz;
  }

  ఒకవేళ(sz > bufsize)
    sz = bufsize;

  జ్ఞాపకం_నకలు(బఫరు, బైట్లు, sz);
  state->స్థానభ్రంశం += sz;
  తిరిగి sz;
}

/* Read a non-encoded part content. */

స్థిర size_t read_part_content(curl_mimepart *part, అక్షరం *బఫరు,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  ఎంపిక(part->lastreadstatus) {
  స్థితి 0:
  స్థితి CURL_READFUNC_ABORT:
  స్థితి CURL_READFUNC_PAUSE:
  స్థితి READ_ERROR:
    తిరిగి part->lastreadstatus;
  అప్రమేయ:
    ఆపు;
  }

  /* If we can determine we are at end of part data, spare a read. */
  ఒకవేళ(part->datasize != (curl_off_t)-1 &&
     part->state.స్థానభ్రంశం >= part->datasize) {
    /* sz is already zero. */
  }
  లేకపోతే {
    ఎంపిక(part->రకం) {
    స్థితి MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(బఫరు, 1, bufsize, part->arg, hasread);
      ఆపు;
    స్థితి MIMEKIND_FILE:
      ఒకవేళ(part->fp && feof(part->fp))
        ఆపు;  /* At EOF. */
      FALLTHROUGH();
    అప్రమేయ:
      ఒకవేళ(part->readfunc) {
        ఒకవేళ(!(part->flags & MIME_FAST_READ)) {
          ఒకవేళ(*hasread)
            తిరిగి STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(బఫరు, 1, bufsize, part->arg);
      }
      ఆపు;
    }
  }

  ఎంపిక(sz) {
  స్థితి STOP_FILLING:
    ఆపు;
  స్థితి 0:
  స్థితి CURL_READFUNC_ABORT:
  స్థితి CURL_READFUNC_PAUSE:
  స్థితి READ_ERROR:
    part->lastreadstatus = sz;
    ఆపు;
  అప్రమేయ:
    part->state.స్థానభ్రంశం += sz;
    part->lastreadstatus = sz;
    ఆపు;
  }

  తిరిగి sz;
}

/* Read and encode part content. */
స్థిర size_t read_encoded_part_content(curl_mimepart *part, అక్షరం *బఫరు,
                                        size_t bufsize, bool *hasread)
{
  నిర్మాణం mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  కొరకు(;;) {
    ఒకవేళ(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(బఫరు, bufsize, ateof, part);
      ఎంపిక(sz) {
      స్థితి 0:
        ఒకవేళ(ateof)
          తిరిగి cursize;
        ఆపు;
      స్థితి READ_ERROR:
      స్థితి STOP_FILLING:
        తిరిగి cursize ? cursize : sz;
      అప్రమేయ:
        cursize += sz;
        బఫరు += sz;
        bufsize -= sz;
        కొనసాగు;
      }
    }

    /* We need more data in input buffer. */
    ఒకవేళ(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      ఒకవేళ(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    ఒకవేళ(st->bufend >= పరిమాణం(st->buf))
      తిరిగి cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           పరిమాణం(st->buf) - st->bufend, hasread);
    ఎంపిక(sz) {
    స్థితి 0:
      ateof = TRUE;
      ఆపు;
    స్థితి CURL_READFUNC_ABORT:
    స్థితి CURL_READFUNC_PAUSE:
    స్థితి READ_ERROR:
    స్థితి STOP_FILLING:
      తిరిగి cursize ? cursize : sz;
    అప్రమేయ:
      st->bufend += sz;
      ఆపు;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
స్థిర size_t readback_part(curl_mimepart *part,
                            అక్షరం *బఫరు, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  అయితే(bufsize) {
    size_t sz = 0;
    నిర్మాణం curl_slist *hdr = (నిర్మాణం curl_slist *)part->state.ptr;
    ఎంపిక(part->state.state) {
    స్థితి MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      ఆపు;
    స్థితి MIMESTATE_USERHEADERS:
      ఒకవేళ(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, ఖాళీ);
        ఆపు;
      }
      ఒకవేళ(సరిపోలిక_శీర్షిక(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->తదుపరి);
        ఆపు;
      }
      FALLTHROUGH();
    స్థితి MIMESTATE_CURLHEADERS:
      ఒకవేళ(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      లేకపోతే {
        sz = readback_బైట్లు(&part->state, బఫరు, bufsize,
                            hdr->దత్తాంశం, అక్షరశ్రేణి_పొడవు(hdr->దత్తాంశం), STRCONST("\r\n"));
        ఒకవేళ(!sz)
          mimesetstate(&part->state, part->state.state, hdr->తదుపరి);
      }
      ఆపు;
    స్థితి MIMESTATE_EOH:
      sz = readback_బైట్లు(&part->state, బఫరు, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      ఒకవేళ(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, ఖాళీ);
      ఆపు;
    స్థితి MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, ఖాళీ);
      ఆపు;
    స్థితి MIMESTATE_CONTENT:
      ఒకవేళ(part->encoder)
        sz = read_encoded_part_content(part, బఫరు, bufsize, hasread);
      లేకపోతే
        sz = read_part_content(part, బఫరు, bufsize, hasread);
      ఎంపిక(sz) {
      స్థితి 0:
        mimesetstate(&part->state, MIMESTATE_చివర, ఖాళీ);
        /* Try sparing open file descriptors. */
        ఒకవేళ(part->రకం == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = ఖాళీ;
        }
        FALLTHROUGH();
      స్థితి CURL_READFUNC_ABORT:
      స్థితి CURL_READFUNC_PAUSE:
      స్థితి READ_ERROR:
      స్థితి STOP_FILLING:
        తిరిగి cursize ? cursize : sz;
      }
      ఆపు;
    స్థితి MIMESTATE_చివర:
      తిరిగి cursize;
    అప్రమేయ:
      ఆపు;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    బఫరు += sz;
    bufsize -= sz;
  }

  తిరిగి cursize;
}

/* Readback from mime. Warning: not a read callback function. */
స్థిర size_t mime_subparts_read(అక్షరం *బఫరు, size_t కొలత, size_t nitems,
                                 శూన్య *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (శూన్య)కొలత;  /* Always 1 */

  అయితే(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    ఎంపిక(mime->state.state) {
    స్థితి MIMESTATE_BEGIN:
    స్థితి MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.స్థానభ్రంశం += 2;
      ఆపు;
    స్థితి MIMESTATE_BOUNDARY1:
      sz = readback_బైట్లు(&mime->state, బఫరు, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      ఒకవేళ(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      ఆపు;
    స్థితి MIMESTATE_BOUNDARY2:
      ఒకవేళ(part)
        sz = readback_బైట్లు(&mime->state, బఫరు, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      లేకపోతే
        sz = readback_బైట్లు(&mime->state, బఫరు, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      ఒకవేళ(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      ఆపు;
    స్థితి MIMESTATE_CONTENT:
      ఒకవేళ(!part) {
        mimesetstate(&mime->state, MIMESTATE_చివర, ఖాళీ);
        ఆపు;
      }
      sz = readback_part(part, బఫరు, nitems, hasread);
      ఎంపిక(sz) {
      స్థితి CURL_READFUNC_ABORT:
      స్థితి CURL_READFUNC_PAUSE:
      స్థితి READ_ERROR:
      స్థితి STOP_FILLING:
        తిరిగి cursize ? cursize : sz;
      స్థితి 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        ఆపు;
      }
      ఆపు;
    స్థితి MIMESTATE_చివర:
      తిరిగి cursize;
    అప్రమేయ:
      ఆపు;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    బఫరు += sz;
    nitems -= sz;
  }

  తిరిగి cursize;
}

స్థిర పూర్ణాంకం mime_part_rewind(curl_mimepart *part)
{
  పూర్ణాంకం res = CURL_SEEKFUNC_OK;
  గణన mimestate targetstate = MIMESTATE_BEGIN;

  ఒకవేళ(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  ఒకవేళ(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    ఒకవేళ(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      ఎంపిక(res) {
      స్థితి CURL_SEEKFUNC_OK:
      స్థితి CURL_SEEKFUNC_FAIL:
      స్థితి CURL_SEEKFUNC_CANTSEEK:
        ఆపు;
      స్థితి -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        ఆపు;
      అప్రమేయ:
        res = CURL_SEEKFUNC_FAIL;
        ఆపు;
      }
    }
  }

  ఒకవేళ(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, ఖాళీ);

  part->lastreadstatus = 1; /* Successful read status. */
  తిరిగి res;
}

స్థిర పూర్ణాంకం mime_subparts_seek(శూన్య *instream, curl_off_t స్థానభ్రంశం, పూర్ణాంకం whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  పూర్ణాంకం rc = CURL_SEEKFUNC_OK;

  ఒకవేళ(whence != SEEK_SET || స్థానభ్రంశం)
    తిరిగి CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  ఒకవేళ(mime->state.state == MIMESTATE_BEGIN)
    తిరిగి CURL_SEEKFUNC_OK;           /* Already rewound. */

  కొరకు(part = mime->firstpart; part; part = part->nextpart) {
    పూర్ణాంకం res = mime_part_rewind(part);
    ఒకవేళ(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  ఒకవేళ(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ఖాళీ);

  తిరిగి rc;
}

/* Release part content. */
స్థిర శూన్య cleanup_part_content(curl_mimepart *part)
{
  ఒకవేళ(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = ఖాళీ;
  part->seekfunc = ఖాళీ;
  part->freefunc = ఖాళీ;
  part->arg = (శూన్య *)part;           /* Defaults to part itself. */
  part->దత్తాంశం = ఖాళీ;
  part->fp = ఖాళీ;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->రకం = MIMEKIND_NONE;
  part->flags &= ~(గుర్తులేని పూర్ణాంకం)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

స్థిర శూన్య mime_subparts_free(శూన్య *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ఒకవేళ(mime && mime->మాతృ) {
    mime->మాతృ->freefunc = ఖాళీ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->మాతృ);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
స్థిర శూన్య mime_subparts_unbind(శూన్య *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  ఒకవేళ(mime && mime->మాతృ) {
    mime->మాతృ->freefunc = ఖాళీ;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->మాతృ);  /* Avoid dangling pointer in part. */
    mime->మాతృ = ఖాళీ;
  }
}

శూన్య Curl_mime_cleanpart(curl_mimepart *part)
{
  ఒకవేళ(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    ఒకవేళ(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
శూన్య curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  ఒకవేళ(!mime)
    తిరిగి;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  అయితే(mime) {
    part = mime->firstpart;
    ఒకవేళ(part) {
      mime->firstpart = part->nextpart;
      ఒకవేళ(part->రకం == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = ఖాళీ;
        cleanup_part_content(part);
        subparts->మాతృ = mime->మాతృ;
        mime->మాతృ = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    లేకపోతే {
      curl_mime *మాతృ = (curl_mime *)mime->మాతృ;
      curlx_free(mime);
      mime = మాతృ;
    }
  }
}

స్థిర bool mime_is_too_deep(స్థిరరాశి curl_mimepart *part, size_t call_లోతు)
{
  DEBUGASSERT(part);

  ఒకవేళ(++call_లోతు > MAX_MIME_స్థాయిలు)
    తిరిగి TRUE;

  ఒకవేళ(part->రకం == MIMEKIND_MULTIPART && part->arg) {
    స్థిరరాశి curl_mime *mime = (స్థిరరాశి curl_mime *)part->arg;
    స్థిరరాశి curl_mimepart *s;

    కొరకు(s = mime->firstpart; s; s = s->nextpart) {
      ఒకవేళ(mime_is_too_deep(s, call_లోతు))
        తిరిగి TRUE;
    }
  }

  తిరిగి FALSE;
}

స్థిర CURLcode mime_duppart(నిర్మాణం Curl_easy *దత్తాంశం,
                             curl_mimepart *dst, స్థిరరాశి curl_mimepart *src,
                             size_t call_లోతు)
{
  curl_mime *mime;
  curl_mimepart *d;
  స్థిరరాశి curl_mimepart *s;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  ఒకవేళ(++call_లోతు > MAX_MIME_స్థాయిలు)
    తిరిగి CURLE_TOO_LARGE;

  /* Duplicate content. */
  ఎంపిక(src->రకం) {
  స్థితి MIMEKIND_NONE:
    ఆపు;
  స్థితి MIMEKIND_దత్తాంశం:
    ఫలితం = curl_mime_దత్తాంశం(dst, src->దత్తాంశం, (size_t)src->datasize);
    ఆపు;
  స్థితి MIMEKIND_FILE:
    ఫలితం = curl_mime_filedata(dst, src->దత్తాంశం);
    /* Do not abort duplication if file is not readable. */
    ఒకవేళ(ఫలితం == CURLE_READ_ERROR)
      ఫలితం = CURLE_OK;
    ఆపు;
  స్థితి MIMEKIND_CALLBACK:
    ఫలితం = curl_mime_దత్తాంశం_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    ఆపు;
  స్థితి MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(దత్తాంశం);
    ఫలితం = mime ? curl_mime_subparts(dst, mime) : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    /* Duplicate subparts. */
    కొరకు(s = ((curl_mime *)src->arg)->firstpart; !ఫలితం && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      ఫలితం = d ? mime_duppart(దత్తాంశం, d, s, call_లోతు) : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ఆపు;
  అప్రమేయ:  /* Invalid kind: should not occur. */
    DEBUGF(infof(దత్తాంశం, "invalid MIMEKIND* attempt"));
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;  /* Internal error? */
    ఆపు;
  }

  /* Duplicate headers. */
  ఒకవేళ(!ఫలితం && src->userheaders) {
    నిర్మాణం curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    ఒకవేళ(!hdrs)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    లేకపోతే {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      ఫలితం = curl_mime_శీర్షికలు(dst, hdrs, TRUE);
      ఒకవేళ(ఫలితం)
        curl_slist_free_all(hdrs);
    }
  }

  ఒకవేళ(!ఫలితం) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    ఫలితం = curl_mime_type(dst, src->mimetype);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = curl_mime_name(dst, src->name);
  ఒకవేళ(!ఫలితం)
    ఫలితం = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  ఒకవేళ(ఫలితం)
    Curl_mime_cleanpart(dst);

  తిరిగి ఫలితం;
}

CURLcode Curl_mime_duppart(నిర్మాణం Curl_easy *దత్తాంశం,
                           curl_mimepart *dst, స్థిరరాశి curl_mimepart *src)
{
  ఒకవేళ(!dst || !src)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(mime_is_too_deep(src, 0))
    తిరిగి CURLE_TOO_LARGE;

  తిరిగి mime_duppart(దత్తాంశం, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(శూన్య *easy)
{
  curl_mime *mime = curlx_malloc(పరిమాణం(*mime));

  ఒకవేళ(mime) {
    mime->మాతృ = ఖాళీ;
    mime->firstpart = ఖాళీ;
    mime->lastpart = ఖాళీ;

    memset(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    ఒకవేళ(Curl_rand_alnum(easy,
                       (గుర్తులేని అక్షరం *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      తిరిగి ఖాళీ;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, ఖాళీ);
  }

  తిరిగి mime;
}

/* Initialize a mime part. */
శూన్య Curl_mime_initpart(curl_mimepart *part)
{
  memset(part, 0, పరిమాణం(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, ఖాళీ);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  ఒకవేళ(!mime)
    తిరిగి ఖాళీ;

  part = curlx_malloc(పరిమాణం(*part));

  ఒకవేళ(part) {
    Curl_mime_initpart(part);
    part->మాతృ = mime;

    ఒకవేళ(mime->lastpart)
      mime->lastpart->nextpart = part;
    లేకపోతే
      mime->firstpart = part;

    mime->lastpart = part;
  }

  తిరిగి part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, స్థిరరాశి అక్షరం *name)
{
  ఒకవేళ(!part)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  curlx_safefree(part->name);

  ఒకవేళ(name) {
    part->name = curlx_strdup(name);
    ఒకవేళ(!part->name)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  తిరిగి CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, స్థిరరాశి అక్షరం *filename)
{
  ఒకవేళ(!part)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  curlx_safefree(part->filename);

  ఒకవేళ(filename) {
    part->filename = curlx_strdup(filename);
    ఒకవేళ(!part->filename)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  తిరిగి CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_దత్తాంశం(curl_mimepart *part, స్థిరరాశి అక్షరం *దత్తాంశం, size_t datasize)
{
  ఒకవేళ(!part)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  cleanup_part_content(part);

  ఒకవేళ(దత్తాంశం) {
    ఒకవేళ(datasize == CURL_ZERO_TERMINATED)
      datasize = అక్షరశ్రేణి_పొడవు(దత్తాంశం);

    part->దత్తాంశం = curlx_memdup0(దత్తాంశం, datasize);
    ఒకవేళ(!part->దత్తాంశం)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->రకం = MIMEKIND_దత్తాంశం;
  }

  తిరిగి CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, స్థిరరాశి అక్షరం *filename)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!part)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  cleanup_part_content(part);

  ఒకవేళ(filename) {
    అక్షరం *పునాది;
    curlx_struct_stat sbuf;

    ఒకవేళ(curlx_stat(filename, &sbuf))
      ఫలితం = CURLE_READ_ERROR;
    లేకపోతే {
      part->దత్తాంశం = curlx_strdup(filename);
      ఒకవేళ(!part->దత్తాంశం)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      లేకపోతే {
        part->datasize = -1;
        ఒకవేళ(S_ISREG(sbuf.st_బహుళకం)) {
          part->datasize = filesize(filename, sbuf);
          part->seekfunc = mime_file_seek;
        }

        part->readfunc = mime_file_read;
        part->freefunc = mime_file_free;
        part->రకం = MIMEKIND_FILE;

        /* As a side effect, set the filename to the current file's base name.
           It is possible to withdraw this by explicitly calling
           curl_mime_filename() with a NULL filename argument after the current
           call. */
        పునాది = strippath(filename);
        ఒకవేళ(!పునాది)
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        లేకపోతే {
          ఫలితం = curl_mime_filename(part, పునాది);
          curlx_free(పునాది);
        }
      }
    }
  }
  తిరిగి ఫలితం;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, స్థిరరాశి అక్షరం *mimetype)
{
  ఒకవేళ(!part)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  curlx_safefree(part->mimetype);

  ఒకవేళ(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    ఒకవేళ(!part->mimetype)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  తిరిగి CURLE_OK;
}

స్థిర స్థిరరాశి నిర్మాణం mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_కొలత },
  { "8bit", encoder_nop_read, encoder_nop_కొలత },
  { "7bit", encoder_7bit_read, encoder_nop_కొలత },
  { "base64", encoder_base64_read, encoder_base64_కొలత },
  { "quoted-printable", encoder_qp_read, encoder_qp_కొలత },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, స్థిరరాశి అక్షరం *encoding)
{
  CURLcode ఫలితం = CURLE_BAD_FUNCTION_వాదం;
  స్థిరరాశి నిర్మాణం mime_encoder *mep;

  ఒకవేళ(!part)
    తిరిగి ఫలితం;

  part->encoder = ఖాళీ;

  ఒకవేళ(!encoding)
    తిరిగి CURLE_OK;    /* Removing current encoder. */

  కొరకు(mep = encoders; mep->name; mep++)
    ఒకవేళ(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      ఫలితం = CURLE_OK;
    }

  తిరిగి ఫలితం;
}

/* Set mime part headers. */
CURLcode curl_mime_శీర్షికలు(curl_mimepart *part,
                           నిర్మాణం curl_slist *శీర్షికలు, పూర్ణాంకం take_ownership)
{
  ఒకవేళ(!part)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(part->flags & MIME_USERHEADERS_OWNER) {
    ఒకవేళ(part->userheaders != శీర్షికలు)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(గుర్తులేని పూర్ణాంకం)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = శీర్షికలు;
  ఒకవేళ(శీర్షికలు && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  తిరిగి CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_దత్తాంశం_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, శూన్య *arg)
{
  ఒకవేళ(!part)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  cleanup_part_content(part);

  ఒకవేళ(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->రకం = MIMEKIND_CALLBACK;
  }

  తిరిగి CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_సమితి_subparts(curl_mimepart *part,
                                curl_mime *subparts, పూర్ణాంకం take_ownership)
{
  curl_mime *మూలం;

  ఒకవేళ(!part)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  /* Accept setting twice the same subparts. */
  ఒకవేళ(part->రకం == MIMEKIND_MULTIPART && part->arg == subparts)
    తిరిగి CURLE_OK;

  cleanup_part_content(part);

  ఒకవేళ(subparts) {
    /* Should not have been attached already. */
    ఒకవేళ(subparts->మాతృ)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;

    /* Should not be the part's root. */
    మూలం = part->మాతృ;
    ఒకవేళ(మూలం) {
      అయితే(మూలం->మాతృ && మూలం->మాతృ->మాతృ)
        మూలం = మూలం->మాతృ->మాతృ;
      ఒకవేళ(subparts == మూలం) {
        /* cannot add as a subpart of itself. */
        తిరిగి CURLE_BAD_FUNCTION_వాదం;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    ఒకవేళ(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      తిరిగి CURLE_SEND_FAIL_REWIND;

    subparts->మాతృ = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->రకం = MIMEKIND_MULTIPART;
  }

  తిరిగి CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  తిరిగి Curl_mime_సమితి_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(అక్షరం *బఫరు, size_t కొలత, size_t nitems, శూన్య *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (శూన్య)కొలత;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  చేయి {
    hasread = FALSE;
    ret = readback_part(part, బఫరు, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } అయితే(ret == STOP_FILLING);

  తిరిగి ret;
}

/* Rewind mime stream. */
స్థిర CURLcode mime_rewind(curl_mimepart *part)
{
  తిరిగి mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
స్థిర size_t slist_కొలత(నిర్మాణం curl_slist *s,
                         size_t overhead, స్థిరరాశి అక్షరం *skip, size_t skiplen)
{
  size_t కొలత = 0;

  కొరకు(; s; s = s->తదుపరి)
    ఒకవేళ(!skip || !సరిపోలిక_శీర్షిక(s, skip, skiplen))
      కొలత += అక్షరశ్రేణి_పొడవు(s->దత్తాంశం) + overhead;
  తిరిగి కొలత;
}

/* Get/compute multipart size. */
స్థిర curl_off_t multipart_కొలత(curl_mime *mime)
{
  curl_off_t కొలత;
  curl_off_t boundarysize;
  curl_mimepart *part;

  ఒకవేళ(!mime)
    తిరిగి 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  కొలత = boundarysize;  /* Final boundary - CRLF after headers. */

  కొరకు(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_కొలత(part);

    ఒకవేళ(sz < 0)
      కొలత = sz;

    ఒకవేళ(కొలత >= 0)
      కొలత += boundarysize + sz;
  }

  తిరిగి కొలత;
}

/* Get/compute mime size. */
స్థిర curl_off_t mime_కొలత(curl_mimepart *part)
{
  curl_off_t కొలత;

  ఒకవేళ(part->రకం == MIMEKIND_MULTIPART)
    part->datasize = multipart_కొలత(part->arg);

  కొలత = part->datasize;

  ఒకవేళ(part->encoder)
    కొలత = part->encoder->sizefunc(part);

  ఒకవేళ(కొలత >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    కొలత += slist_కొలత(part->curlheaders, 2, ఖాళీ, 0);
    కొలత += slist_కొలత(part->userheaders, 2, STRCONST("Content-Type"));
    కొలత += 2;    /* CRLF after headers. */
  }
  తిరిగి కొలత;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_కూడు_శీర్షిక(నిర్మాణం curl_slist **slp, స్థిరరాశి అక్షరం *fmt, ...)
{
  నిర్మాణం curl_slist *hdr = ఖాళీ;
  అక్షరం *s = ఖాళీ;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  ఒకవేళ(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    ఒకవేళ(hdr)
      *slp = hdr;
    లేకపోతే
      curlx_free(s);
  }

  తిరిగి hdr ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}

/* Add a content type header. */
స్థిర CURLcode కూడు_content_type(నిర్మాణం curl_slist **slp,
                                 స్థిరరాశి అక్షరం *type, స్థిరరాశి అక్షరం *boundary)
{
  తిరిగి Curl_mime_కూడు_శీర్షిక(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

స్థిరరాశి అక్షరం *Curl_mime_contenttype(స్థిరరాశి అక్షరం *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  నిర్మాణం ContentType {
    స్థిరరాశి అక్షరం *extension;
    స్థిరరాశి అక్షరం *type;
  };
  స్థిర స్థిరరాశి నిర్మాణం ContentType ctts[] = {
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

  ఒకవేళ(filename) {
    size_t len1 = అక్షరశ్రేణి_పొడవు(filename);
    స్థిరరాశి అక్షరం *nameend = filename + len1;
    గుర్తులేని పూర్ణాంకం i;

    కొరకు(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = అక్షరశ్రేణి_పొడవు(ctts[i].extension);

      ఒకవేళ(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        తిరిగి ctts[i].type;
    }
  }
  తిరిగి ఖాళీ;
}

స్థిర bool content_type_సరిపోలిక(స్థిరరాశి అక్షరం *contenttype,
                               స్థిరరాశి అక్షరం *లక్ష్యం, size_t len)
{
  ఒకవేళ(contenttype && curl_strnequal(contenttype, లక్ష్యం, len))
    ఎంపిక(contenttype[len]) {
    స్థితి '\0':
    స్థితి '\t':
    స్థితి '\r':
    స్థితి '\n':
    స్థితి ' ':
    స్థితి ';':
      తిరిగి TRUE;
    }
  తిరిగి FALSE;
}

స్థిర CURLcode కూడు_content_disposition(నిర్మాణం Curl_easy *దత్తాంశం,
                                        curl_mimepart *part,
                                        స్థిరరాశి అక్షరం *disposition,
                                        స్థిరరాశి అక్షరం *contenttype,
                                        గణన mimestrategy strategy)
{
  ఒకవేళ(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  ఒకవేళ(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = ఖాళీ;

  ఒకవేళ(disposition) {
    CURLcode ఫలితం = CURLE_OK;
    అక్షరం *name = ఖాళీ;
    అక్షరం *filename = ఖాళీ;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (దత్తాంశం && దత్తాంశం->సమితి.mime_formescape);
    ఒకవేళ(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;

    ఒకవేళ(part->name) {
      name = escape_string(దత్తాంశం, part->name, strategy);
      ఒకవేళ(!name)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ఒకవేళ(part->filename) {
      filename = escape_string(దత్తాంశం, part->filename, strategy);
      ఒకవేళ(!filename)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_mime_కూడు_శీర్షిక(&part->curlheaders,
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
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode mime_prepare_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం,
                                     curl_mimepart *part,
                                     స్థిరరాశి అక్షరం *contenttype,
                                     స్థిరరాశి అక్షరం *disposition,
                                     గణన mimestrategy strategy,
                                     size_t call_లోతు)
{
  curl_mime *mime = ఖాళీ;
  స్థిరరాశి అక్షరం *boundary = ఖాళీ;
  అక్షరం *customct;
  స్థిరరాశి అక్షరం *cte = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(++call_లోతు > MAX_MIME_స్థాయిలు)
    తిరిగి CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = ఖాళీ;

  /* Be sure we will not access old headers later. */
  ఒకవేళ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, ఖాళీ);

  /* Check if content type is specified. */
  customct = part->mimetype;
  ఒకవేళ(!customct)
    customct = శోధన_శీర్షిక(part->userheaders, STRCONST("Content-Type"));
  ఒకవేళ(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  ఒకవేళ(!contenttype) {
    ఎంపిక(part->రకం) {
    స్థితి MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      ఆపు;
    స్థితి MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      ఒకవేళ(!contenttype)
        contenttype = Curl_mime_contenttype(part->దత్తాంశం);
      ఒకవేళ(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      ఆపు;
    అప్రమేయ:
      contenttype = Curl_mime_contenttype(part->filename);
      ఆపు;
    }
  }

  ఒకవేళ(part->రకం == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    ఒకవేళ(mime)
      boundary = mime->boundary;
  }
  లేకపోతే ఒకవేళ(contenttype && !customct &&
          content_type_సరిపోలిక(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = ఖాళీ;

  /* Issue content-disposition header only if not already set by caller. */
  ఒకవేళ(!శోధన_శీర్షిక(part->userheaders, STRCONST("Content-Disposition"))) {
    ఫలితం = కూడు_content_disposition(దత్తాంశం, part, disposition,
                                     contenttype, strategy);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* Issue Content-Type header. */
  ఒకవేళ(contenttype) {
    ఫలితం = కూడు_content_type(&part->curlheaders, contenttype, boundary);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* Content-Transfer-Encoding header. */
  ఒకవేళ(!శోధన_శీర్షిక(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    ఒకవేళ(part->encoder)
      cte = part->encoder->name;
    లేకపోతే ఒకవేళ(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->రకం != MIMEKIND_MULTIPART)
      cte = "8bit";
    ఒకవేళ(cte) {
      ఫలితం = Curl_mime_కూడు_శీర్షిక(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  ఒకవేళ(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  ఒకవేళ(part->రకం == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = ఖాళీ;
    ఒకవేళ(content_type_సరిపోలిక(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    కొరకు(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      ఫలితం = mime_prepare_శీర్షికలు(దత్తాంశం, subpart, ఖాళీ,
                                    disposition, strategy, call_లోతు);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_mime_prepare_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం,
                                   curl_mimepart *part,
                                   స్థిరరాశి అక్షరం *contenttype,
                                   స్థిరరాశి అక్షరం *disposition,
                                   గణన mimestrategy strategy)
{
  తిరిగి mime_prepare_శీర్షికలు(దత్తాంశం, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
స్థిర శూన్య mime_unpause(curl_mimepart *part)
{
  ఒకవేళ(part) {
    ఒకవేళ(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    ఒకవేళ(part->రకం == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      ఒకవేళ(mime) {
        curl_mimepart *subpart;

        కొరకు(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

నిర్మాణం cr_mime_ctx {
  నిర్మాణం Curl_creader super;
  curl_mimepart *part;
  curl_off_t మొత్తం_len;
  curl_off_t read_len;
  CURLcode error_ఫలితం;
  నిర్మాణం bufq tmpbuf;
  బిట్టు(చూసిన_eos);
  బిట్టు(errored);
};

స్థిర CURLcode cr_mime_init(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_mime_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  ctx->మొత్తం_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cr_mime_close(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_mime_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
స్థిర CURLcode cr_mime_read(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_creader *reader,
                             అక్షరం *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  నిర్మాణం cr_mime_ctx *ctx = reader->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t nread;
  అక్షరం tmp[256];

  /* Once we have errored, we will return the same error forever */
  ఒకవేళ(ctx->errored) {
    CURL_TRC_READ(దత్తాంశం, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (పూర్ణాంకం)ctx->error_ఫలితం);
    *pnread = 0;
    *peos = FALSE;
    తిరిగి ctx->error_ఫలితం;
  }
  ఒకవేళ(ctx->చూసిన_eos) {
    CURL_TRC_READ(దత్తాంశం, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    తిరిగి CURLE_OK;
  }
  /* respect length limitations */
  ఒకవేళ(ctx->మొత్తం_len >= 0) {
    curl_off_t remain = ctx->మొత్తం_len - ctx->read_len;
    ఒకవేళ(remain <= 0)
      blen = 0;
    లేకపోతే ఒకవేళ(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->tmpbuf)) {
    ఫలితం = Curl_bufq_read(&ctx->tmpbuf, (గుర్తులేని అక్షరం *)buf, blen, &nread);
    ఒకవేళ(ఫలితం) {
      ctx->errored = TRUE;
      ctx->error_ఫలితం = ఫలితం;
      తిరిగి ఫలితం;
    }
  }
  లేకపోతే ఒకవేళ(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(దత్తాంశం, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, పరిమాణం(tmp), ctx->part);
    ఒకవేళ(nread <= పరిమాణం(tmp)) {
      size_t n;
      ఫలితం = Curl_bufq_write(&ctx->tmpbuf, (గుర్తులేని అక్షరం *)tmp, nread, &n);
      ఒకవేళ(ఫలితం) {
        ctx->errored = TRUE;
        ctx->error_ఫలితం = ఫలితం;
        తిరిగి ఫలితం;
      }
      /* stored it, read again */
      ఫలితం = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      ఒకవేళ(ఫలితం) {
        ctx->errored = TRUE;
        ctx->error_ఫలితం = ఫలితం;
        తిరిగి ఫలితం;
      }
    }
  }
  లేకపోతే
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(దత్తాంశం, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  ఎంపిక(nread) {
  స్థితి 0:
    ఒకవేళ((ctx->మొత్తం_len >= 0) && (ctx->read_len < ctx->మొత్తం_len)) {
      failf(దత్తాంశం, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->మొత్తం_len);
      తిరిగి CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->చూసిన_eos = TRUE;
    ఆపు;

  స్థితి CURL_READFUNC_ABORT:
    failf(దత్తాంశం, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ఫలితం = CURLE_ABORTED_BY_CALLBACK;
    ఫలితం = CURLE_ABORTED_BY_CALLBACK;
    ఆపు;

  స్థితి CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(దత్తాంశం, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    ఫలితం = Curl_xfer_pause_send(దత్తాంశం, TRUE);
    ఆపు; /* nothing was read */

  స్థితి STOP_FILLING:
  స్థితి READ_ERROR:
    failf(దత్తాంశం, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_ఫలితం = CURLE_READ_ERROR;
    ఫలితం = CURLE_READ_ERROR;
    ఆపు;

  అప్రమేయ:
    ఒకవేళ(nread > blen) {
      /* the read function returned a too large value */
      failf(దత్తాంశం, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_ఫలితం = CURLE_READ_ERROR;
      తిరిగి CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    ఒకవేళ(ctx->మొత్తం_len >= 0)
      ctx->చూసిన_eos = (ctx->read_len >= ctx->మొత్తం_len);
    *pnread = nread;
    *peos = (bool)ctx->చూసిన_eos;
    ఆపు;
  }

  CURL_TRC_READ(దత్తాంశం, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->మొత్తం_len, ctx->read_len, (పూర్ణాంకం)ఫలితం, *pnread, *peos);
  తిరిగి ఫలితం;
}

స్థిర bool cr_mime_needs_rewind(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_mime_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి ctx->read_len > 0;
}

స్థిర curl_off_t cr_mime_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_mime_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి ctx->మొత్తం_len;
}

స్థిర CURLcode cr_mime_resume_from(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం Curl_creader *reader,
                                    curl_off_t స్థానభ్రంశం)
{
  నిర్మాణం cr_mime_ctx *ctx = reader->ctx;

  ఒకవేళ(స్థానభ్రంశం > 0) {
    curl_off_t passed = 0;

    చేయి {
      అక్షరం scratch[4 * 1024];
      size_t readthisamountnow =
        (స్థానభ్రంశం - passed > (curl_off_t)పరిమాణం(scratch)) ?
        పరిమాణం(scratch) :
        curlx_sotouz(స్థానభ్రంశం - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      ఒకవేళ((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(దత్తాంశం, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        తిరిగి CURLE_READ_ERROR;
      }
    } అయితే(passed < స్థానభ్రంశం);

    /* now, decrease the size of the read */
    ఒకవేళ(ctx->మొత్తం_len > 0) {
      ctx->మొత్తం_len -= స్థానభ్రంశం;

      ఒకవేళ(ctx->మొత్తం_len <= 0) {
        failf(దత్తాంశం, "Mime post already completely uploaded");
        తిరిగి CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cr_mime_cntrl(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  నిర్మాణం cr_mime_ctx *ctx = reader->ctx;
  ఎంపిక(opcode) {
  స్థితి CURL_CRCNTRL_REWIND: {
    CURLcode ఫలితం = mime_rewind(ctx->part);
    ఒకవేళ(ఫలితం)
      failf(దత్తాంశం, "Cannot rewind mime/post data");
    తిరిగి ఫలితం;
  }
  స్థితి CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    ఆపు;
  స్థితి CURL_CRCNTRL_CLEAR_EOS:
    ctx->చూసిన_eos = FALSE;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర bool cr_mime_is_paused(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_mime_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_మొత్తం_నిడివి,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_పూర్తి,
  పరిమాణం(నిర్మాణం cr_mime_ctx)
};

CURLcode Curl_creader_సమితి_mime(నిర్మాణం Curl_easy *దత్తాంశం, curl_mimepart *part)
{
  నిర్మాణం Curl_creader *r;
  నిర్మాణం cr_mime_ctx *ctx;
  CURLcode ఫలితం;

  ఫలితం = Curl_creader_create(&r, దత్తాంశం, &cr_mime, CURL_CR_CLIENT);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  ఫలితం = mime_rewind(ctx->part);
  ఒకవేళ(ఫలితం) {
    Curl_creader_free(దత్తాంశం, r);
    తిరిగి ఫలితం;
  }
  ctx->మొత్తం_len = mime_కొలత(ctx->part);

  తిరిగి Curl_creader_సమితి(దత్తాంశం, r);
}

#లేకపోతే /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (శూన్య)easy;
  తిరిగి ఖాళీ;
}

శూన్య curl_mime_free(curl_mime *mime)
{
  (శూన్య)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (శూన్య)mime;
  తిరిగి ఖాళీ;
}

CURLcode curl_mime_name(curl_mimepart *part, స్థిరరాశి అక్షరం *name)
{
  (శూన్య)part;
  (శూన్య)name;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_mime_filename(curl_mimepart *part, స్థిరరాశి అక్షరం *filename)
{
  (శూన్య)part;
  (శూన్య)filename;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_mime_type(curl_mimepart *part, స్థిరరాశి అక్షరం *mimetype)
{
  (శూన్య)part;
  (శూన్య)mimetype;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_mime_encoder(curl_mimepart *part, స్థిరరాశి అక్షరం *encoding)
{
  (శూన్య)part;
  (శూన్య)encoding;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_mime_దత్తాంశం(curl_mimepart *part, స్థిరరాశి అక్షరం *దత్తాంశం, size_t datasize)
{
  (శూన్య)part;
  (శూన్య)దత్తాంశం;
  (శూన్య)datasize;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_mime_filedata(curl_mimepart *part, స్థిరరాశి అక్షరం *filename)
{
  (శూన్య)part;
  (శూన్య)filename;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_mime_దత్తాంశం_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           శూన్య *arg)
{
  (శూన్య)part;
  (శూన్య)datasize;
  (శూన్య)readfunc;
  (శూన్య)seekfunc;
  (శూన్య)freefunc;
  (శూన్య)arg;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (శూన్య)part;
  (శూన్య)subparts;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode curl_mime_శీర్షికలు(curl_mimepart *part,
                           నిర్మాణం curl_slist *శీర్షికలు, పూర్ణాంకం take_ownership)
{
  (శూన్య)part;
  (శూన్య)శీర్షికలు;
  (శూన్య)take_ownership;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

CURLcode Curl_mime_కూడు_శీర్షిక(నిర్మాణం curl_slist **slp, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)slp;
  (శూన్య)fmt;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

#అయితే_ముగింపు /* if disabled */

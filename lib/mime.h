#اگر_غیر_تعریف_شدہ سرخی_CURL_MIME_H
#تعریف_کرو سرخی_CURL_MIME_H
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

#تعریف_کرو MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#تعریف_کرو MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#تعریف_کرو MAX_ENCODED_سطر_لمبائی         76  /* Maximum encoded line length. */
#تعریف_کرو ENCODING_بفر_سائز            256 /* Encoding temp buffers size. */

/* Part flags. */
#تعریف_کرو MIME_USERHEADERS_OWNER  (1 << 0)
#تعریف_کرو MIME_BODY_ONLY          (1 << 1)
#تعریف_کرو MIME_FAST_READ          (1 << 2)

#تعریف_کرو FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#تعریف_کرو MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#تعریف_کرو DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
شمار mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_ڈیٹا,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_آخری
};

/* Readback state tokens. */
شمار mimestate {
  MIMESTATE_BEGIN,              /* Readback has not yet started. */
  MIMESTATE_CURLHEADERS,        /* In curl-generated headers. */
  MIMESTATE_USERHEADERS,        /* In caller's supplied headers. */
  MIMESTATE_EOH,                /* End of headers. */
  MIMESTATE_BODY,               /* Placeholder. */
  MIMESTATE_BOUNDARY1,          /* In boundary prefix. */
  MIMESTATE_BOUNDARY2,          /* In boundary. */
  MIMESTATE_CONTENT,            /* In content. */
  MIMESTATE_انتہا,                /* End of part reached. */
  MIMESTATE_آخری
};

/* Mime headers strategies. */
شمار mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_آخری
};

/* Content transfer encoder. */
ساخت mime_encoder {
  ثابت حرف *   name;          /* Encoding name. */
  size_t         (*encodefunc)(حرف *بفر, size_t سائز, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
ساخت mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  حرف           buf[ENCODING_بفر_سائز]; /* Input buffer. */
};

/* Mime readback state. */
ساخت mime_state {
  شمار mimestate state;       /* Current state token. */
  خالی *ptr;                  /* State-dependent pointer. */
  curl_off_t انحراف;          /* State-dependent offset. */
};

/* Boundary string length. */
#تعریف_کرو MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
ساخت curl_mime {
  curl_mimepart *والد;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  حرف boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  ساخت mime_state state;         /* Current readback state. */
};

/* A mime part. */
ساخت curl_mimepart {
  curl_mime *والد;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  شمار mimekind قسم;              /* The part kind. */
  غیرعلامتی صحیح flags;              /* Flags. */
  حرف *ڈیٹا;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  خالی *arg;                       /* Argument to callback functions. */
  FILE *fp;                        /* File pointer. */
  ساخت curl_slist *curlheaders;  /* Part headers. */
  ساخت curl_slist *userheaders;  /* Part headers. */
  حرف *mimetype;                  /* Part mime type. */
  حرف *filename;                  /* Remote filename. */
  حرف *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  ساخت mime_state state;         /* Current readback state. */
  ثابت ساخت mime_encoder *encoder; /* Content data encoder. */
  ساخت mime_encoder_state encstate; /* Data encoder state. */
  size_t lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_جوڑو_سرخی(ساخت curl_slist **slp, ثابت حرف *fmt, ...)
  CURL_PRINTF(2, 3);

#اگر !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
خالی Curl_mime_initpart(ساخت curl_mimepart *part);
خالی Curl_mime_cleanpart(ساخت curl_mimepart *part);
CURLcode Curl_mime_duppart(ساخت Curl_easy *ڈیٹا,
                           ساخت curl_mimepart *dst,
                           ثابت curl_mimepart *src);
CURLcode Curl_mime_سیٹ_subparts(ساخت curl_mimepart *part,
                                ساخت curl_mime *subparts,
                                صحیح take_ownership);
CURLcode Curl_mime_prepare_سرخیاں(ساخت Curl_easy *ڈیٹا,
                                   ساخت curl_mimepart *part,
                                   ثابت حرف *contenttype,
                                   ثابت حرف *disposition,
                                   شمار mimestrategy strategy);
size_t Curl_mime_read(حرف *بفر, size_t سائز, size_t nitems,
                      خالی *instream);
ثابت حرف *Curl_mime_contenttype(ثابت حرف *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_سیٹ_mime(ساخت Curl_easy *ڈیٹا, curl_mimepart *part);

#ورنہ /* if disabled */
#تعریف_کرو Curl_mime_initpart(x)
#تعریف_کرو Curl_mime_cleanpart(x)
#تعریف_کرو Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#تعریف_کرو Curl_mime_سیٹ_subparts(a, b, c)          CURLE_NOT_BUILT_اندر
#تعریف_کرو Curl_mime_prepare_سرخیاں(a, b, c, d, e) CURLE_NOT_BUILT_اندر
#تعریف_کرو Curl_mime_read                           تہی
#تعریف_کرو Curl_creader_سیٹ_mime(x, y) ((خالی)(x), CURLE_NOT_BUILT_اندر)
#اگر_ختم

#اگر_ختم /* HEADER_CURL_MIME_H */

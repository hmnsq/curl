#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_MIME_H
#وصف_ڪريو سرو_CURL_MIME_H
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

#وصف_ڪريو MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#وصف_ڪريو MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#وصف_ڪريو MAX_ENCODED_سٽ_ڊيگهائي         76  /* Maximum encoded line length. */
#وصف_ڪريو ENCODING_بفر_سائيز            256 /* Encoding temp buffers size. */

/* Part flags. */
#وصف_ڪريو MIME_USERHEADERS_OWNER  (1 << 0)
#وصف_ڪريو MIME_BODY_ONLY          (1 << 1)
#وصف_ڪريو MIME_FAST_READ          (1 << 2)

#وصف_ڪريو FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#وصف_ڪريو MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#وصف_ڪريو DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
تعداد mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_معلومات,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_آخري
};

/* Readback state tokens. */
تعداد mimestate {
  MIMESTATE_BEGIN,              /* Readback has not yet started. */
  MIMESTATE_CURLHEADERS,        /* In curl-generated headers. */
  MIMESTATE_USERHEADERS,        /* In caller's supplied headers. */
  MIMESTATE_EOH,                /* End of headers. */
  MIMESTATE_BODY,               /* Placeholder. */
  MIMESTATE_BOUNDARY1,          /* In boundary prefix. */
  MIMESTATE_BOUNDARY2,          /* In boundary. */
  MIMESTATE_CONTENT,            /* In content. */
  MIMESTATE_پڇاڙي,                /* End of part reached. */
  MIMESTATE_آخري
};

/* Mime headers strategies. */
تعداد mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_آخري
};

/* Content transfer encoder. */
بنية mime_encoder {
  ثابت حرف *   name;          /* Encoding name. */
  size_t         (*encodefunc)(حرف *بفر, size_t سائيز, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
بنية mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  حرف           buf[ENCODING_بفر_سائيز]; /* Input buffer. */
};

/* Mime readback state. */
بنية mime_state {
  تعداد mimestate state;       /* Current state token. */
  فراغ *ptr;                  /* State-dependent pointer. */
  curl_off_t وٿي;          /* State-dependent offset. */
};

/* Boundary string length. */
#وصف_ڪريو MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
بنية curl_mime {
  curl_mimepart *والد;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  حرف boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  بنية mime_state state;         /* Current readback state. */
};

/* A mime part. */
بنية curl_mimepart {
  curl_mime *والد;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  تعداد mimekind قسم;              /* The part kind. */
  بنانشان صحيح flags;              /* Flags. */
  حرف *معلومات;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  فراغ *arg;                       /* Argument to callback functions. */
  FILE *fp;                        /* File pointer. */
  بنية curl_slist *curlheaders;  /* Part headers. */
  بنية curl_slist *userheaders;  /* Part headers. */
  حرف *mimetype;                  /* Part mime type. */
  حرف *filename;                  /* Remote filename. */
  حرف *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  بنية mime_state state;         /* Current readback state. */
  ثابت بنية mime_encoder *encoder; /* Content data encoder. */
  بنية mime_encoder_state encstate; /* Data encoder state. */
  size_t lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_جوڙيو_سرو(بنية curl_slist **slp, ثابت حرف *fmt, ...)
  CURL_PRINTF(2, 3);

#إذا !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
فراغ Curl_mime_initpart(بنية curl_mimepart *part);
فراغ Curl_mime_cleanpart(بنية curl_mimepart *part);
CURLcode Curl_mime_duppart(بنية Curl_easy *معلومات,
                           بنية curl_mimepart *dst,
                           ثابت curl_mimepart *src);
CURLcode Curl_mime_سيٽ_subparts(بنية curl_mimepart *part,
                                بنية curl_mime *subparts,
                                صحيح take_ownership);
CURLcode Curl_mime_prepare_سرا(بنية Curl_easy *معلومات,
                                   بنية curl_mimepart *part,
                                   ثابت حرف *contenttype,
                                   ثابت حرف *disposition,
                                   تعداد mimestrategy strategy);
size_t Curl_mime_read(حرف *بفر, size_t سائيز, size_t nitems,
                      فراغ *instream);
ثابت حرف *Curl_mime_contenttype(ثابت حرف *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_سيٽ_mime(بنية Curl_easy *معلومات, curl_mimepart *part);

#وإلا /* if disabled */
#وصف_ڪريو Curl_mime_initpart(x)
#وصف_ڪريو Curl_mime_cleanpart(x)
#وصف_ڪريو Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#وصف_ڪريو Curl_mime_سيٽ_subparts(a, b, c)          CURLE_NOT_BUILT_اندريون
#وصف_ڪريو Curl_mime_prepare_سرا(a, b, c, d, e) CURLE_NOT_BUILT_اندريون
#وصف_ڪريو Curl_mime_read                           عدم
#وصف_ڪريو Curl_creader_سيٽ_mime(x, y) ((فراغ)(x), CURLE_NOT_BUILT_اندريون)
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_MIME_H */

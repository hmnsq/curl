#که_نه_تعریف_شوی سرلیک_CURL_MIME_H
#تعریف_کړه سرلیک_CURL_MIME_H
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

#تعریف_کړه MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#تعریف_کړه MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#تعریف_کړه MAX_ENCODED_کرښه_اوږدتیا         76  /* Maximum encoded line length. */
#تعریف_کړه ENCODING_بفر_اندازه            256 /* Encoding temp buffers size. */

/* Part flags. */
#تعریف_کړه MIME_USERHEADERS_OWNER  (1 << 0)
#تعریف_کړه MIME_BODY_ONLY          (1 << 1)
#تعریف_کړه MIME_FAST_READ          (1 << 2)

#تعریف_کړه FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#تعریف_کړه MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#تعریف_کړه DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
تعداد mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_معلومات,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_وروستی
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
  MIMESTATE_پای_ټکی,                /* End of part reached. */
  MIMESTATE_وروستی
};

/* Mime headers strategies. */
تعداد mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_وروستی
};

/* Content transfer encoder. */
بنية mime_encoder {
  ثابت حرف *   name;          /* Encoding name. */
  size_t         (*encodefunc)(حرف *بفر, size_t اندازه, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
بنية mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  حرف           buf[ENCODING_بفر_اندازه]; /* Input buffer. */
};

/* Mime readback state. */
بنية mime_state {
  تعداد mimestate state;       /* Current state token. */
  فراغ *ptr;                  /* State-dependent pointer. */
  curl_off_t دننه_واټن;          /* State-dependent offset. */
};

/* Boundary string length. */
#تعریف_کړه MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
بنية curl_mime {
  curl_mimepart *پلار;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  حرف boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  بنية mime_state state;         /* Current readback state. */
};

/* A mime part. */
بنية curl_mimepart {
  curl_mime *پلار;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  تعداد mimekind ډول;              /* The part kind. */
  بینښه صحيح flags;              /* Flags. */
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

CURLcode Curl_mime_جمع_کړه_سرلیک(بنية curl_slist **slp, ثابت حرف *fmt, ...)
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
CURLcode Curl_mime_سېټ_subparts(بنية curl_mimepart *part,
                                بنية curl_mime *subparts,
                                صحيح take_ownership);
CURLcode Curl_mime_prepare_سرلیکونه(بنية Curl_easy *معلومات,
                                   بنية curl_mimepart *part,
                                   ثابت حرف *contenttype,
                                   ثابت حرف *disposition,
                                   تعداد mimestrategy strategy);
size_t Curl_mime_read(حرف *بفر, size_t اندازه, size_t nitems,
                      فراغ *instream);
ثابت حرف *Curl_mime_contenttype(ثابت حرف *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_سېټ_mime(بنية Curl_easy *معلومات, curl_mimepart *part);

#وإلا /* if disabled */
#تعریف_کړه Curl_mime_initpart(x)
#تعریف_کړه Curl_mime_cleanpart(x)
#تعریف_کړه Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#تعریف_کړه Curl_mime_سېټ_subparts(a, b, c)          CURLE_NOT_BUILT_دننه
#تعریف_کړه Curl_mime_prepare_سرلیکونه(a, b, c, d, e) CURLE_NOT_BUILT_دننه
#تعریف_کړه Curl_mime_read                           عدم
#تعریف_کړه Curl_creader_سېټ_mime(x, y) ((فراغ)(x), CURLE_NOT_BUILT_دننه)
#د_که_پای

#د_که_پای /* HEADER_CURL_MIME_H */

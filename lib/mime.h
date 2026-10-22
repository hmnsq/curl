#ifndef HEADER_CURL_MIME_H
#define HEADER_CURL_MIME_H
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
#अन्तर्भाव "curl_setup.h"

#define MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#define MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#define MAX_ENCODED_ओळ_LENGTH         76  /* Maximum encoded line length. */
#define ENCODING_BUFFER_SIZE            256 /* Encoding temp buffers size. */

/* Part flags. */
#define MIME_USERHEADERS_OWNER  (1 << 0)
#define MIME_BODY_ONLY          (1 << 1)
#define MIME_FAST_READ          (1 << 2)

#define FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#define MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#define DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
गणना mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_DATA,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_LAST
};

/* Readback state tokens. */
गणना mimestate {
  MIMESTATE_BEGIN,              /* Readback has not yet started. */
  MIMESTATE_CURLHEADERS,        /* In curl-generated headers. */
  MIMESTATE_USERHEADERS,        /* In caller's supplied headers. */
  MIMESTATE_EOH,                /* End of headers. */
  MIMESTATE_BODY,               /* Placeholder. */
  MIMESTATE_BOUNDARY1,          /* In boundary prefix. */
  MIMESTATE_BOUNDARY2,          /* In boundary. */
  MIMESTATE_CONTENT,            /* In content. */
  MIMESTATE_END,                /* End of part reached. */
  MIMESTATE_LAST
};

/* Mime headers strategies. */
गणना mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_LAST
};

/* Content transfer encoder. */
संरचना mime_encoder {
  स्थिरांक अक्षर *   name;          /* Encoding name. */
  माप_प्रकार         (*encodefunc)(अक्षर *buffer, माप_प्रकार size, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
संरचना mime_encoder_state {
  माप_प्रकार         pos;           /* Position on output line. */
  माप_प्रकार         bufbeg;        /* Next data index in input buffer. */
  माप_प्रकार         bufend;        /* First unused byte index in input buffer. */
  अक्षर           buf[ENCODING_BUFFER_SIZE]; /* Input buffer. */
};

/* Mime readback state. */
संरचना mime_state {
  गणना mimestate state;       /* Current state token. */
  शून्य *ptr;                  /* State-dependent pointer. */
  curl_off_t offset;          /* State-dependent offset. */
};

/* Boundary string length. */
#define MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
संरचना curl_mime {
  curl_mimepart *parent;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  अक्षर boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  संरचना mime_state state;         /* Current readback state. */
};

/* A mime part. */
संरचना curl_mimepart {
  curl_mime *parent;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  गणना mimekind kind;              /* The part kind. */
  अचिन्हित पूर्णांक flags;              /* Flags. */
  अक्षर *data;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  शून्य *arg;                       /* Argument to callback functions. */
  खाता *fp;                        /* File pointer. */
  संरचना curl_slist *curlheaders;  /* Part headers. */
  संरचना curl_slist *userheaders;  /* Part headers. */
  अक्षर *mimetype;                  /* Part mime type. */
  अक्षर *filename;                  /* Remote filename. */
  अक्षर *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  संरचना mime_state state;         /* Current readback state. */
  स्थिरांक संरचना mime_encoder *encoder; /* Content data encoder. */
  संरचना mime_encoder_state encstate; /* Data encoder state. */
  माप_प्रकार lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_add_header(संरचना curl_slist **slp, स्थिरांक अक्षर *fmt, ...)
  CURL_PRINTF(2, 3);

#जर !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
शून्य Curl_mime_initpart(संरचना curl_mimepart *part);
शून्य Curl_mime_cleanpart(संरचना curl_mimepart *part);
CURLcode Curl_mime_duppart(संरचना Curl_easy *data,
                           संरचना curl_mimepart *dst,
                           स्थिरांक curl_mimepart *src);
CURLcode Curl_mime_set_subparts(संरचना curl_mimepart *part,
                                संरचना curl_mime *subparts,
                                पूर्णांक take_ownership);
CURLcode Curl_mime_prepare_headers(संरचना Curl_easy *data,
                                   संरचना curl_mimepart *part,
                                   स्थिरांक अक्षर *contenttype,
                                   स्थिरांक अक्षर *disposition,
                                   गणना mimestrategy strategy);
माप_प्रकार Curl_mime_read(अक्षर *buffer, माप_प्रकार size, माप_प्रकार nitems,
                      शून्य *instream);
स्थिरांक अक्षर *Curl_mime_contenttype(स्थिरांक अक्षर *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_set_mime(संरचना Curl_easy *data, curl_mimepart *part);

#अन्यथा /* if disabled */
#define Curl_mime_initpart(x)
#define Curl_mime_cleanpart(x)
#define Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#define Curl_mime_set_subparts(a, b, c)          CURLE_NOT_BUILT_IN
#define Curl_mime_prepare_headers(a, b, c, d, e) CURLE_NOT_BUILT_IN
#define Curl_mime_read                           रिक्त
#define Curl_creader_set_mime(x, y) ((शून्य)(x), CURLE_NOT_BUILT_IN)
#endif

#endif /* HEADER_CURL_MIME_H */

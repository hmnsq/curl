#अगर_अघोषित HEADER_CURL_MIME_H
#घोषणा HEADER_CURL_MIME_H
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
#समावेश "curl_setup.h"

#घोषणा MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#घोषणा MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#घोषणा MAX_ENCODED_पंक्ति_लंबाई         76  /* Maximum encoded line length. */
#घोषणा ENCODING_बफर_आकार            256 /* Encoding temp buffers size. */

/* Part flags. */
#घोषणा MIME_USERHEADERS_OWNER  (1 << 0)
#घोषणा MIME_BODY_ONLY          (1 << 1)
#घोषणा MIME_FAST_READ          (1 << 2)

#घोषणा FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#घोषणा MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#घोषणा DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
क्रमागत mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_तथ्य,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_LAST
};

/* Readback state tokens. */
क्रमागत mimestate {
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
क्रमागत mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_LAST
};

/* Content transfer encoder. */
काष्ठा mime_encoder {
  स्थिर अक्षर *   name;          /* Encoding name. */
  माप_प्रकार         (*encodefunc)(अक्षर *बफर, माप_प्रकार आकार, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
काष्ठा mime_encoder_state {
  माप_प्रकार         pos;           /* Position on output line. */
  माप_प्रकार         bufbeg;        /* Next data index in input buffer. */
  माप_प्रकार         bufend;        /* First unused byte index in input buffer. */
  अक्षर           buf[ENCODING_बफर_आकार]; /* Input buffer. */
};

/* Mime readback state. */
काष्ठा mime_state {
  क्रमागत mimestate state;       /* Current state token. */
  व्योम *ptr;                  /* State-dependent pointer. */
  curl_off_t offset;          /* State-dependent offset. */
};

/* Boundary string length. */
#घोषणा MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
काष्ठा curl_mime {
  curl_mimepart *parent;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  अक्षर boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  काष्ठा mime_state state;         /* Current readback state. */
};

/* A mime part. */
काष्ठा curl_mimepart {
  curl_mime *parent;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  क्रमागत mimekind kind;              /* The part kind. */
  अचिन्हित पूर्णांक flags;              /* Flags. */
  अक्षर *तथ्य;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  व्योम *arg;                       /* Argument to callback functions. */
  खाता *fp;                        /* File pointer. */
  काष्ठा curl_slist *curlheaders;  /* Part headers. */
  काष्ठा curl_slist *userheaders;  /* Part headers. */
  अक्षर *mimetype;                  /* Part mime type. */
  अक्षर *filename;                  /* Remote filename. */
  अक्षर *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  काष्ठा mime_state state;         /* Current readback state. */
  स्थिर काष्ठा mime_encoder *encoder; /* Content data encoder. */
  काष्ठा mime_encoder_state encstate; /* Data encoder state. */
  माप_प्रकार lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_add_header(काष्ठा curl_slist **slp, स्थिर अक्षर *fmt, ...)
  CURL_PRINTF(2, 3);

#अगर !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
व्योम Curl_mime_initpart(काष्ठा curl_mimepart *part);
व्योम Curl_mime_cleanpart(काष्ठा curl_mimepart *part);
CURLcode Curl_mime_duppart(काष्ठा Curl_easy *तथ्य,
                           काष्ठा curl_mimepart *dst,
                           स्थिर curl_mimepart *src);
CURLcode Curl_mime_set_subparts(काष्ठा curl_mimepart *part,
                                काष्ठा curl_mime *subparts,
                                पूर्णांक take_ownership);
CURLcode Curl_mime_prepare_headers(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा curl_mimepart *part,
                                   स्थिर अक्षर *contenttype,
                                   स्थिर अक्षर *disposition,
                                   क्रमागत mimestrategy strategy);
माप_प्रकार Curl_mime_read(अक्षर *बफर, माप_प्रकार आकार, माप_प्रकार nitems,
                      व्योम *instream);
स्थिर अक्षर *Curl_mime_contenttype(स्थिर अक्षर *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_set_mime(काष्ठा Curl_easy *तथ्य, curl_mimepart *part);

#अन्यथा /* if disabled */
#घोषणा Curl_mime_initpart(x)
#घोषणा Curl_mime_cleanpart(x)
#घोषणा Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#घोषणा Curl_mime_set_subparts(a, b, c)          CURLE_NOT_BUILT_IN
#घोषणा Curl_mime_prepare_headers(a, b, c, d, e) CURLE_NOT_BUILT_IN
#घोषणा Curl_mime_read                           शून्य
#घोषणा Curl_creader_set_mime(x, y) ((व्योम)(x), CURLE_NOT_BUILT_IN)
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_MIME_H */

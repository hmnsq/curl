#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_MIME_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_MIME_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ENCODED_ਸਤਰ_ਲੰਮਾਈ         76  /* Maximum encoded line length. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENCODING_ਬਫਰ_ਮਾਪ            256 /* Encoding temp buffers size. */

/* Part flags. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIME_USERHEADERS_OWNER  (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIME_BODY_ONLY          (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIME_FAST_READ          (1 << 2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
ਗਿਣਤੀ mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_ਅੰਕੜੇ,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_ਆਖਰੀ
};

/* Readback state tokens. */
ਗਿਣਤੀ mimestate {
  MIMESTATE_BEGIN,              /* Readback has not yet started. */
  MIMESTATE_CURLHEADERS,        /* In curl-generated headers. */
  MIMESTATE_USERHEADERS,        /* In caller's supplied headers. */
  MIMESTATE_EOH,                /* End of headers. */
  MIMESTATE_BODY,               /* Placeholder. */
  MIMESTATE_BOUNDARY1,          /* In boundary prefix. */
  MIMESTATE_BOUNDARY2,          /* In boundary. */
  MIMESTATE_CONTENT,            /* In content. */
  MIMESTATE_ਸਿਰਾ,                /* End of part reached. */
  MIMESTATE_ਆਖਰੀ
};

/* Mime headers strategies. */
ਗਿਣਤੀ mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_ਆਖਰੀ
};

/* Content transfer encoder. */
ਬਣਤਰ mime_encoder {
  ਅਚਲ ਅੱਖਰ *   name;          /* Encoding name. */
  size_t         (*encodefunc)(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
ਬਣਤਰ mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  ਅੱਖਰ           buf[ENCODING_ਬਫਰ_ਮਾਪ]; /* Input buffer. */
};

/* Mime readback state. */
ਬਣਤਰ mime_state {
  ਗਿਣਤੀ mimestate state;       /* Current state token. */
  ਖਾਲੀ *ptr;                  /* State-dependent pointer. */
  curl_off_t ਵਿੱਥ;          /* State-dependent offset. */
};

/* Boundary string length. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
ਬਣਤਰ curl_mime {
  curl_mimepart *ਮਾਪਾ;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  ਅੱਖਰ boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  ਬਣਤਰ mime_state state;         /* Current readback state. */
};

/* A mime part. */
ਬਣਤਰ curl_mimepart {
  curl_mime *ਮਾਪਾ;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  ਗਿਣਤੀ mimekind ਕਿਸਮ;              /* The part kind. */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags;              /* Flags. */
  ਅੱਖਰ *ਅੰਕੜੇ;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  ਖਾਲੀ *arg;                       /* Argument to callback functions. */
  FILE *fp;                        /* File pointer. */
  ਬਣਤਰ curl_slist *curlheaders;  /* Part headers. */
  ਬਣਤਰ curl_slist *userheaders;  /* Part headers. */
  ਅੱਖਰ *mimetype;                  /* Part mime type. */
  ਅੱਖਰ *filename;                  /* Remote filename. */
  ਅੱਖਰ *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  ਬਣਤਰ mime_state state;         /* Current readback state. */
  ਅਚਲ ਬਣਤਰ mime_encoder *encoder; /* Content data encoder. */
  ਬਣਤਰ mime_encoder_state encstate; /* Data encoder state. */
  size_t lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_ਜੋੜੋ_ਸਿਰਲੇਖ(ਬਣਤਰ curl_slist **slp, ਅਚਲ ਅੱਖਰ *fmt, ...)
  CURL_PRINTF(2, 3);

#ਜੇ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
ਖਾਲੀ Curl_mime_initpart(ਬਣਤਰ curl_mimepart *part);
ਖਾਲੀ Curl_mime_cleanpart(ਬਣਤਰ curl_mimepart *part);
CURLcode Curl_mime_duppart(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ curl_mimepart *dst,
                           ਅਚਲ curl_mimepart *src);
CURLcode Curl_mime_ਸੈੱਟ_subparts(ਬਣਤਰ curl_mimepart *part,
                                ਬਣਤਰ curl_mime *subparts,
                                ਪੂਰਨ take_ownership);
CURLcode Curl_mime_prepare_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ curl_mimepart *part,
                                   ਅਚਲ ਅੱਖਰ *contenttype,
                                   ਅਚਲ ਅੱਖਰ *disposition,
                                   ਗਿਣਤੀ mimestrategy strategy);
size_t Curl_mime_read(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, size_t nitems,
                      ਖਾਲੀ *instream);
ਅਚਲ ਅੱਖਰ *Curl_mime_contenttype(ਅਚਲ ਅੱਖਰ *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_ਸੈੱਟ_mime(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_mimepart *part);

#ਨਹੀਂਤਾਂ /* if disabled */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_mime_initpart(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_mime_cleanpart(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_mime_ਸੈੱਟ_subparts(a, b, c)          CURLE_NOT_BUILT_ਵਿੱਚ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_mime_prepare_ਸਿਰਲੇਖਾਂ(a, b, c, d, e) CURLE_NOT_BUILT_ਵਿੱਚ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_mime_read                           ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_creader_ਸੈੱਟ_mime(x, y) ((ਖਾਲੀ)(x), CURLE_NOT_BUILT_ਵਿੱਚ)
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_MIME_H */

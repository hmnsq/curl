#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_MIME_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_MIME_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#ವ್ಯಾಖ್ಯಾನಿಸು MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ENCODED_ಸಾಲು_ಉದ್ದಳತೆ         76  /* Maximum encoded line length. */
#ವ್ಯಾಖ್ಯಾನಿಸು ENCODING_ಬಫರ್_ಗಾತ್ರ            256 /* Encoding temp buffers size. */

/* Part flags. */
#ವ್ಯಾಖ್ಯಾನಿಸು MIME_USERHEADERS_OWNER  (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು MIME_BODY_ONLY          (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು MIME_FAST_READ          (1 << 2)

#ವ್ಯಾಖ್ಯಾನಿಸು FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#ವ್ಯಾಖ್ಯಾನಿಸು MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#ವ್ಯಾಖ್ಯಾನಿಸು DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
ಎಣಿಕೆ mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_ದತ್ತಾಂಶ,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_ಕೊನೆಯ
};

/* Readback state tokens. */
ಎಣಿಕೆ mimestate {
  MIMESTATE_BEGIN,              /* Readback has not yet started. */
  MIMESTATE_CURLHEADERS,        /* In curl-generated headers. */
  MIMESTATE_USERHEADERS,        /* In caller's supplied headers. */
  MIMESTATE_EOH,                /* End of headers. */
  MIMESTATE_BODY,               /* Placeholder. */
  MIMESTATE_BOUNDARY1,          /* In boundary prefix. */
  MIMESTATE_BOUNDARY2,          /* In boundary. */
  MIMESTATE_CONTENT,            /* In content. */
  MIMESTATE_ಕೊನೆ,                /* End of part reached. */
  MIMESTATE_ಕೊನೆಯ
};

/* Mime headers strategies. */
ಎಣಿಕೆ mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_ಕೊನೆಯ
};

/* Content transfer encoder. */
ರಚನೆ mime_encoder {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *   name;          /* Encoding name. */
  size_t         (*encodefunc)(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
ರಚನೆ mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  ಅಕ್ಷರ           buf[ENCODING_ಬಫರ್_ಗಾತ್ರ]; /* Input buffer. */
};

/* Mime readback state. */
ರಚನೆ mime_state {
  ಎಣಿಕೆ mimestate state;       /* Current state token. */
  ಶೂನ್ಯ *ptr;                  /* State-dependent pointer. */
  curl_off_t ಸ್ಥಾನಾಂತರ;          /* State-dependent offset. */
};

/* Boundary string length. */
#ವ್ಯಾಖ್ಯಾನಿಸು MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
ರಚನೆ curl_mime {
  curl_mimepart *ಪೋಷಕ;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  ಅಕ್ಷರ boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  ರಚನೆ mime_state state;         /* Current readback state. */
};

/* A mime part. */
ರಚನೆ curl_mimepart {
  curl_mime *ಪೋಷಕ;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  ಎಣಿಕೆ mimekind ಬಗೆ;              /* The part kind. */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags;              /* Flags. */
  ಅಕ್ಷರ *ದತ್ತಾಂಶ;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  ಶೂನ್ಯ *arg;                       /* Argument to callback functions. */
  FILE *fp;                        /* File pointer. */
  ರಚನೆ curl_slist *curlheaders;  /* Part headers. */
  ರಚನೆ curl_slist *userheaders;  /* Part headers. */
  ಅಕ್ಷರ *mimetype;                  /* Part mime type. */
  ಅಕ್ಷರ *filename;                  /* Remote filename. */
  ಅಕ್ಷರ *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  ರಚನೆ mime_state state;         /* Current readback state. */
  ಸ್ಥಿರಾಂಕ ರಚನೆ mime_encoder *encoder; /* Content data encoder. */
  ರಚನೆ mime_encoder_state encstate; /* Data encoder state. */
  size_t lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_ಕೂಡಿಸು_ಶೀರ್ಷಿಕೆ(ರಚನೆ curl_slist **slp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
  CURL_PRINTF(2, 3);

#ಆದರೆ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
ಶೂನ್ಯ Curl_mime_initpart(ರಚನೆ curl_mimepart *part);
ಶೂನ್ಯ Curl_mime_cleanpart(ರಚನೆ curl_mimepart *part);
CURLcode Curl_mime_duppart(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ curl_mimepart *dst,
                           ಸ್ಥಿರಾಂಕ curl_mimepart *src);
CURLcode Curl_mime_ಗಣ_subparts(ರಚನೆ curl_mimepart *part,
                                ರಚನೆ curl_mime *subparts,
                                ಪೂರ್ಣಾಂಕ take_ownership);
CURLcode Curl_mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ curl_mimepart *part,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *contenttype,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *disposition,
                                   ಎಣಿಕೆ mimestrategy strategy);
size_t Curl_mime_read(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, size_t nitems,
                      ಶೂನ್ಯ *instream);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_mime_contenttype(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_ಗಣ_mime(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_mimepart *part);

#ಇಲ್ಲದಿದ್ದರೆ /* if disabled */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_mime_initpart(x)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_mime_cleanpart(x)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_mime_ಗಣ_subparts(a, b, c)          CURLE_NOT_BUILT_ಒಳ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(a, b, c, d, e) CURLE_NOT_BUILT_ಒಳ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_mime_read                           ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_creader_ಗಣ_mime(x, y) ((ಶೂನ್ಯ)(x), CURLE_NOT_BUILT_ಒಳ)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_MIME_H */

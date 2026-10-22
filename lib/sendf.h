#अगर_अघोषित HEADER_CURL_SENDF_H
#घोषणा HEADER_CURL_SENDF_H
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
#समावेश "cw-out.h"

/**
 * Type of data that is being written to the client (application)
 * - data written can be either BODY or META data
 * - META data is either INFO or HEADER
 * - INFO is meta information, e.g. not BODY, that cannot be interpreted
 *   as headers of a response. Example FTP/IMAP pingpong answers.
 * - HEADER can have additional bits set (more than one)
 *   - STATUS special "header", e.g. response status line in HTTP
 *   - CONNECT header was received during proxying the connection
 *   - 1XX header is part of an intermediate response, e.g. HTTP 1xx code
 *   - TRAILER header is trailing response data, e.g. HTTP trailers
 * BODY, INFO and HEADER should not be mixed, as this would lead to
 * confusion on how to interpret/format/convert the data.
 */
#घोषणा CLIENTWRITE_BODY    (1 << 0) /* non-meta information, BODY */
#घोषणा CLIENTWRITE_INFO    (1 << 1) /* meta information, not a HEADER */
#घोषणा CLIENTWRITE_HEADER  (1 << 2) /* meta information, HEADER */
#घोषणा CLIENTWRITE_STATUS  (1 << 3) /* a special status HEADER */
#घोषणा CLIENTWRITE_CONNECT (1 << 4) /* a CONNECT related HEADER */
#घोषणा CLIENTWRITE_1XX     (1 << 5) /* a 1xx response related HEADER */
#घोषणा CLIENTWRITE_TRAILER (1 << 6) /* a trailer HEADER */
#घोषणा CLIENTWRITE_EOS     (1 << 7) /* End Of transfer download Stream */
#घोषणा CLIENTWRITE_0LEN    (1 << 8) /* write even 0-length buffers */

/* Forward declarations */
काष्ठा Curl_creader;
काष्ठा Curl_cwriter;
काष्ठा Curl_easy;

/**
 * Write `len` bytes at `buf` to the client. `type` indicates what
 * kind of data is being written.
 */
CURLcode Curl_client_write(काष्ठा Curl_easy *तथ्य, पूर्णांक type, स्थिर अक्षर *buf,
                           माप_प्रकार len) WARN_UNUSED_परिणाम;

CURLcode Curl_client_flush(काष्ठा Curl_easy *तथ्य);

/**
 * Free all resources related to client writing.
 */
व्योम Curl_client_cleanup(काष्ठा Curl_easy *तथ्य);

/**
 * Reset readers and writer chains, keep rewind information
 * when necessary.
 */
व्योम Curl_client_reset(काष्ठा Curl_easy *तथ्य);

/**
 * A new request is starting, perform any ops like rewinding
 * previous readers when needed.
 */
CURLcode Curl_client_start(काष्ठा Curl_easy *तथ्य);

/**
 * Client Writers - a chain passing transfer BODY data to the client.
 * Main application: HTTP and related protocols
 * Other uses: monitoring of download progress
 *
 * Writers in the chain are order by their `phase`. First come all
 * writers in CURL_CW_RAW, followed by any in CURL_CW_TRANSFER_DECODE,
 * followed by any in CURL_CW_PROTOCOL, etc.
 *
 * When adding a writer, it is inserted as first in its phase. This means
 * the order of adding writers of the same phase matters, but writers for
 * different phases may be added in any order.
 *
 * Writers which do modify the BODY data written are expected to be of
 * phases TRANSFER_DECODE or CONTENT_DECODE. The other phases are intended
 * for monitoring writers. Which do *not* modify the data but gather
 * statistics or update progress reporting.
 */

/* Phase a writer operates at. */
प्रकार क्रमागत {
  CURL_CW_RAW,  /* raw data written, before any decoding */
  CURL_CW_TRANSFER_DECODE, /* remove transfer-encodings */
  CURL_CW_PROTOCOL, /* after transfer, but before content decoding */
  CURL_CW_BEFORE_DECODE, /* after protocol, but before content decoding */
  CURL_CW_CONTENT_DECODE, /* remove content-encodings */
  CURL_CW_CLIENT  /* data written to client */
} Curl_cwriter_phase;

/* writer may blow up size of write data, e.g. zip bombs */
#घोषणा CURL_CW_FLAG_BLOWUP     (1U << 0)

/* Client Writer Type, provides the implementation */
काष्ठा Curl_cwtype {
  स्थिर अक्षर *name;        /* writer name. */
  स्थिर अक्षर *alias;       /* writer name alias, maybe NULL. */
  uint8_t flags;           /* flags for writer behaviour */
  CURLcode (*do_init)(काष्ठा Curl_easy *तथ्य,
                      काष्ठा Curl_cwriter *writer);
  CURLcode (*do_write)(काष्ठा Curl_easy *तथ्य,
                       काष्ठा Curl_cwriter *writer, पूर्णांक type,
                       स्थिर अक्षर *buf, माप_प्रकार nbytes);
  CURLcode (*do_flush)(काष्ठा Curl_easy *तथ्य,
                       काष्ठा Curl_cwriter *writer);
  व्योम (*do_close)(काष्ठा Curl_easy *तथ्य,
                   काष्ठा Curl_cwriter *writer);
  माप_प्रकार cwriter_आकार;  /* sizeof() allocated struct Curl_cwriter */
};

/* Client writer instance, allocated on creation.
 * `void *ctx` is the pointer from the allocation of
 * the `struct Curl_cwriter` itself. This is suitable for "downcasting"
 * by the writers implementation. See https://github.com/curl/curl/pull/13054
 * for the alignment problems that arise otherwise.
 */
काष्ठा Curl_cwriter {
  स्थिर काष्ठा Curl_cwtype *cwt;  /* type implementation */
  काष्ठा Curl_cwriter *अग्र;  /* Downstream writer. */
  व्योम *ctx;                  /* allocated instance pointer */
  Curl_cwriter_phase phase; /* phase at which it operates */
};

/**
 * Create a new cwriter instance with given type and phase. Is not
 * inserted into the writer chain by this call.
 * Invokes `writer->do_init()`.
 */
CURLcode Curl_cwriter_create(काष्ठा Curl_cwriter **pwriter,
                             काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा Curl_cwtype *cwt,
                             Curl_cwriter_phase phase);

/**
 * Free a cwriter instance.
 * Invokes `writer->do_close()`.
 */
व्योम Curl_cwriter_free(काष्ठा Curl_easy *तथ्य,
                       काष्ठा Curl_cwriter *writer);

/**
 * Count the number of writers installed of the given phase.
 */
माप_प्रकार Curl_cwriter_गिनती(काष्ठा Curl_easy *तथ्य, Curl_cwriter_phase phase);

/**
 * Adds a writer to the transfer's writer chain.
 * The writers `phase` determines where in the chain it is inserted.
 */
CURLcode Curl_cwriter_add(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_cwriter *writer);

/**
 * Look up an installed client writer on `data` by its type.
 * @return first writer with that type or NULL
 */
काष्ठा Curl_cwriter *Curl_cwriter_get_by_type(काष्ठा Curl_easy *तथ्य,
                                              स्थिर काष्ठा Curl_cwtype *cwt);

काष्ठा Curl_cwriter *Curl_cwriter_get_by_name(काष्ठा Curl_easy *तथ्य,
                                              स्थिर अक्षर *name);

/* Convenience method for calling `writer->do_write()` that
 * checks for NULL writer. */
#घोषणा Curl_cwriter_write(d, w, t, b, n) \
  ((w) ? (w)->cwt->do_write((d), (w), (t), (b), (n)) : CURLE_WRITE_ERROR)

#घोषणा Curl_cwriter_flush(d, w) \
  ((w) ? (w)->cwt->do_flush((d), (w)) : CURLE_WRITE_ERROR)

/* TRUE if client writer is paused. */
#घोषणा Curl_cwriter_is_paused(d)     ((bool)(d)->req.writer.paused)

/**
 * Unpause client writer and flush any buffered date to the client.
 */
CURLcode Curl_cwriter_unpause(काष्ठा Curl_easy *तथ्य);

/**
 * Default implementations for do_init, do_write, do_close that
 * do nothing and pass the data through.
 */
CURLcode Curl_cwriter_def_init(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_cwriter *writer);
CURLcode Curl_cwriter_def_write(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *writer, पूर्णांक type,
                                स्थिर अक्षर *buf, माप_प्रकार nbytes);
CURLcode Curl_cwriter_def_flush(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_cwriter *writer);
व्योम Curl_cwriter_def_close(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_cwriter *writer);

प्रकार क्रमागत {
  CURL_CRCNTRL_REWIND,
  CURL_CRCNTRL_UNPAUSE,
  CURL_CRCNTRL_CLEAR_EOS
} Curl_creader_cntrl;

/* Client Reader Type, provides the implementation */
काष्ठा Curl_crtype {
  स्थिर अक्षर *name;        /* writer name. */
  CURLcode (*do_init)(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader);
  CURLcode (*do_read)(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader,
                      अक्षर *buf, माप_प्रकार blen, माप_प्रकार *nread, bool *eos);
  व्योम (*do_close)(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader);
  bool (*needs_rewind)(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader);
  curl_off_t (*योग_लंबाई)(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_creader *reader);
  CURLcode (*resume_from)(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_creader *reader, curl_off_t offset);
  CURLcode (*cntrl)(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader,
                    Curl_creader_cntrl opcode);
  bool (*is_paused)(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader);
  व्योम (*done)(काष्ठा Curl_easy *तथ्य,
               काष्ठा Curl_creader *reader, पूर्णांक premature);
  माप_प्रकार creader_आकार;  /* sizeof() allocated struct Curl_creader */
};

/* Phase a reader operates at. */
प्रकार क्रमागत {
  CURL_CR_NET,  /* data send to the network (connection filters) */
  CURL_CR_TRANSFER_ENCODE, /* add transfer-encodings */
  CURL_CR_PROTOCOL, /* before transfer, but after content decoding */
  CURL_CR_CONTENT_ENCODE, /* add content-encodings */
  CURL_CR_CLIENT  /* data read from client */
} Curl_creader_phase;

/* Client reader instance, allocated on creation.
 * `void *ctx` is the pointer from the allocation of
 * the `struct Curl_cwriter` itself. This is suitable for "downcasting"
 * by the writers implementation. See https://github.com/curl/curl/pull/13054
 * for the alignment problems that arise otherwise.
 */
काष्ठा Curl_creader {
  स्थिर काष्ठा Curl_crtype *crt;  /* type implementation */
  काष्ठा Curl_creader *अग्र;  /* Downstream reader. */
  व्योम *ctx;
  Curl_creader_phase phase; /* phase at which it operates */
};

/**
 * Default implementations for do_init, do_write, do_close that
 * do nothing and pass the data through.
 */
CURLcode Curl_creader_def_init(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creader *reader);
व्योम Curl_creader_def_close(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader);
CURLcode Curl_creader_def_read(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creader *reader,
                               अक्षर *buf, माप_प्रकार blen,
                               माप_प्रकार *nread, bool *eos);
bool Curl_creader_def_needs_rewind(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_creader *reader);
curl_off_t Curl_creader_def_योग_लंबाई(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा Curl_creader *reader);
CURLcode Curl_creader_def_resume_from(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_creader *reader,
                                      curl_off_t offset);
CURLcode Curl_creader_def_cntrl(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_creader *reader,
                                Curl_creader_cntrl opcode);
bool Curl_creader_def_is_paused(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_creader *reader);
व्योम Curl_creader_def_done(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_creader *reader, पूर्णांक premature);

/**
 * Convenience method for calling `reader->do_read()` that
 * checks for NULL reader.
 */
CURLcode Curl_creader_read(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_creader *reader,
                           अक्षर *buf, माप_प्रकार blen, माप_प्रकार *nread, bool *eos);

/* Tell the reader and all below that any EOS state is to be cleared */
व्योम Curl_creader_clear_eos(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_creader *reader);

/**
 * Create a new creader instance with given type and phase. Is not
 * inserted into the writer chain by this call.
 * Invokes `reader->do_init()`.
 */
CURLcode Curl_creader_create(काष्ठा Curl_creader **preader,
                             काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा Curl_crtype *crt,
                             Curl_creader_phase phase);

/**
 * Free a creader instance.
 * Invokes `reader->do_close()`.
 */
व्योम Curl_creader_free(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *reader);

/**
 * Adds a reader to the transfer's reader chain.
 * The readers `phase` determines where in the chain it is inserted.
 */
CURLcode Curl_creader_add(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_creader *reader);

/**
 * Set the given reader, which needs to be of type CURL_CR_CLIENT,
 * as the new first reader. Discard any installed readers and init
 * the reader chain anew.
 * The function takes ownership of `r`.
 */
CURLcode Curl_creader_set(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_creader *r);

/**
 * Read at most `blen` bytes at `buf` from the client.
 * @param data    the transfer to read client bytes for
 * @param buf     the memory location to read to
 * @param blen    the amount of memory at `buf`
 * @param nread   on return the number of bytes read into `buf`
 * @param eos     TRUE iff bytes are the end of data from client
 * @return CURLE_OK on successful read (even 0 length) or error
 */
CURLcode Curl_client_read(काष्ठा Curl_easy *तथ्य, अक्षर *buf, माप_प्रकार blen,
                          माप_प्रकार *nread, bool *eos) WARN_UNUSED_परिणाम;

/**
 * TRUE iff client reader needs rewing before it can be used for
 * a retry request.
 */
bool Curl_creader_needs_rewind(काष्ठा Curl_easy *तथ्य);

/**
 * TRUE iff client reader will rewind at next start
 */
bool Curl_creader_will_rewind(काष्ठा Curl_easy *तथ्य);

/**
 * En-/disable rewind of client reader at next start.
 */
व्योम Curl_creader_set_rewind(काष्ठा Curl_easy *तथ्य, bool enable);

/**
 * Get the total length of bytes provided by the installed readers.
 * This is independent of the amount already delivered and is calculated
 * by all readers in the stack. If a reader like "chunked" or
 * "crlf conversion" is installed, the returned length will be -1.
 * @return -1 if length is indeterminate
 */
curl_off_t Curl_creader_योग_लंबाई(काष्ठा Curl_easy *तथ्य);

/**
 * Get the total length of bytes provided by the reader at phase
 * CURL_CR_CLIENT. This may not match the amount of bytes read
 * for a request, depending if other, encoding readers are also installed.
 * However it allows for rough estimation of the overall length.
 * @return -1 if length is indeterminate
 */
curl_off_t Curl_creader_client_लंबाई(काष्ठा Curl_easy *तथ्य);

/**
 * Unpause all installed readers.
 */
CURLcode Curl_creader_unpause(काष्ठा Curl_easy *तथ्य);

/**
 * Return TRUE iff any of the installed readers is paused.
 */
bool Curl_creader_is_paused(काष्ठा Curl_easy *तथ्य);

/**
 * Tell all client readers that they are done.
 */
व्योम Curl_creader_done(काष्ठा Curl_easy *तथ्य, पूर्णांक premature);

/**
 * Look up an installed client reader on `data` by its type.
 * @return first reader with that type or NULL
 */
काष्ठा Curl_creader *Curl_creader_get_by_type(काष्ठा Curl_easy *तथ्य,
                                              स्थिर काष्ठा Curl_crtype *crt);

/**
 * Set the client reader to provide 0 bytes, immediate EOS.
 */
CURLcode Curl_creader_set_null(काष्ठा Curl_easy *तथ्य);

/**
 * Set the client reader the reads from fread callback.
 */
CURLcode Curl_creader_set_fread(काष्ठा Curl_easy *तथ्य, curl_off_t len);

/**
 * Set the client reader the reads from the supplied buf (NOT COPIED).
 */
CURLcode Curl_creader_set_buf(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *buf, माप_प्रकार blen);

#पूर्ण_अगर /* HEADER_CURL_SENDF_H */

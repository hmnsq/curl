#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SENDF_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SENDF_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "cw-out.h"

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_BODY    (1 << 0) /* non-meta information, BODY */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_INFO    (1 << 1) /* meta information, not a HEADER */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_ਸਿਰਲੇਖ  (1 << 2) /* meta information, HEADER */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_ਸਥਿਤੀ  (1 << 3) /* a special status HEADER */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_CONNECT (1 << 4) /* a CONNECT related HEADER */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_1XX     (1 << 5) /* a 1xx response related HEADER */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_TRAILER (1 << 6) /* a trailer HEADER */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_EOS     (1 << 7) /* End Of transfer download Stream */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIENTWRITE_0LEN    (1 << 8) /* write even 0-length buffers */

/* Forward declarations */
ਬਣਤਰ Curl_creader;
ਬਣਤਰ Curl_cwriter;
ਬਣਤਰ Curl_easy;

/**
 * Write `len` bytes at `buf` to the client. `type` indicates what
 * kind of data is being written.
 */
CURLcode Curl_client_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ *buf,
                           size_t len) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

CURLcode Curl_client_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Free all resources related to client writing.
 */
ਖਾਲੀ Curl_client_cleanup(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Reset readers and writer chains, keep rewind information
 * when necessary.
 */
ਖਾਲੀ Curl_client_reset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * A new request is starting, perform any ops like rewinding
 * previous readers when needed.
 */
CURLcode Curl_client_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_CW_RAW,  /* raw data written, before any decoding */
  CURL_CW_TRANSFER_DECODE, /* remove transfer-encodings */
  CURL_CW_PROTOCOL, /* after transfer, but before content decoding */
  CURL_CW_BEFORE_DECODE, /* after protocol, but before content decoding */
  CURL_CW_CONTENT_DECODE, /* remove content-encodings */
  CURL_CW_CLIENT  /* data written to client */
} Curl_cwriter_phase;

/* writer may blow up size of write data, e.g. zip bombs */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CW_FLAG_BLOWUP     (1U << 0)

/* Client Writer Type, provides the implementation */
ਬਣਤਰ Curl_cwtype {
  ਅਚਲ ਅੱਖਰ *name;        /* writer name. */
  ਅਚਲ ਅੱਖਰ *alias;       /* writer name alias, maybe NULL. */
  uint8_t flags;           /* flags for writer behaviour */
  CURLcode (*do_init)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ Curl_cwriter *writer);
  CURLcode (*do_write)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                       ਅਚਲ ਅੱਖਰ *buf, size_t nbytes);
  CURLcode (*do_flush)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ Curl_cwriter *writer);
  ਖਾਲੀ (*do_close)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                   ਬਣਤਰ Curl_cwriter *writer);
  size_t cwriter_ਮਾਪ;  /* sizeof() allocated struct Curl_cwriter */
};

/* Client writer instance, allocated on creation.
 * `void *ctx` is the pointer from the allocation of
 * the `struct Curl_cwriter` itself. This is suitable for "downcasting"
 * by the writers implementation. See https://github.com/curl/curl/pull/13054
 * for the alignment problems that arise otherwise.
 */
ਬਣਤਰ Curl_cwriter {
  ਅਚਲ ਬਣਤਰ Curl_cwtype *cwt;  /* type implementation */
  ਬਣਤਰ Curl_cwriter *ਅਗਲਾ;  /* Downstream writer. */
  ਖਾਲੀ *ctx;                  /* allocated instance pointer */
  Curl_cwriter_phase phase; /* phase at which it operates */
};

/**
 * Create a new cwriter instance with given type and phase. Is not
 * inserted into the writer chain by this call.
 * Invokes `writer->do_init()`.
 */
CURLcode Curl_cwriter_create(ਬਣਤਰ Curl_cwriter **pwriter,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ Curl_cwtype *cwt,
                             Curl_cwriter_phase phase);

/**
 * Free a cwriter instance.
 * Invokes `writer->do_close()`.
 */
ਖਾਲੀ Curl_cwriter_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ Curl_cwriter *writer);

/**
 * Count the number of writers installed of the given phase.
 */
size_t Curl_cwriter_ਗਿਣਤੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, Curl_cwriter_phase phase);

/**
 * Adds a writer to the transfer's writer chain.
 * The writers `phase` determines where in the chain it is inserted.
 */
CURLcode Curl_cwriter_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_cwriter *writer);

/**
 * Look up an installed client writer on `data` by its type.
 * @return first writer with that type or NULL
 */
ਬਣਤਰ Curl_cwriter *Curl_cwriter_get_by_type(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਅਚਲ ਬਣਤਰ Curl_cwtype *cwt);

ਬਣਤਰ Curl_cwriter *Curl_cwriter_get_by_name(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਅਚਲ ਅੱਖਰ *name);

/* Convenience method for calling `writer->do_write()` that
 * checks for NULL writer. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_cwriter_write(d, w, t, b, n) \
  ((w) ? (w)->cwt->do_write((d), (w), (t), (b), (n)) : CURLE_WRITE_ERROR)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_cwriter_flush(d, w) \
  ((w) ? (w)->cwt->do_flush((d), (w)) : CURLE_WRITE_ERROR)

/* TRUE if client writer is paused. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_cwriter_is_paused(d)     ((bool)(d)->req.writer.paused)

/**
 * Unpause client writer and flush any buffered date to the client.
 */
CURLcode Curl_cwriter_unpause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Default implementations for do_init, do_write, do_close that
 * do nothing and pass the data through.
 */
CURLcode Curl_cwriter_def_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_cwriter *writer);
CURLcode Curl_cwriter_def_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                                ਅਚਲ ਅੱਖਰ *buf, size_t nbytes);
CURLcode Curl_cwriter_def_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_cwriter *writer);
ਖਾਲੀ Curl_cwriter_def_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_cwriter *writer);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_CRCNTRL_REWIND,
  CURL_CRCNTRL_UNPAUSE,
  CURL_CRCNTRL_CLEAR_EOS
} Curl_creader_cntrl;

/* Client Reader Type, provides the implementation */
ਬਣਤਰ Curl_crtype {
  ਅਚਲ ਅੱਖਰ *name;        /* writer name. */
  CURLcode (*do_init)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader);
  CURLcode (*do_read)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader,
                      ਅੱਖਰ *buf, size_t blen, size_t *nread, bool *eos);
  ਖਾਲੀ (*do_close)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader);
  bool (*needs_rewind)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader);
  curl_off_t (*ਕੁੱਲ_ਲੰਮਾਈ)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_creader *reader);
  CURLcode (*resume_from)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_creader *reader, curl_off_t ਵਿੱਥ);
  CURLcode (*cntrl)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader,
                    Curl_creader_cntrl opcode);
  bool (*is_paused)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader);
  ਖਾਲੀ (*ਪੂਰਾ)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
               ਬਣਤਰ Curl_creader *reader, ਪੂਰਨ premature);
  size_t creader_ਮਾਪ;  /* sizeof() allocated struct Curl_creader */
};

/* Phase a reader operates at. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
ਬਣਤਰ Curl_creader {
  ਅਚਲ ਬਣਤਰ Curl_crtype *crt;  /* type implementation */
  ਬਣਤਰ Curl_creader *ਅਗਲਾ;  /* Downstream reader. */
  ਖਾਲੀ *ctx;
  Curl_creader_phase phase; /* phase at which it operates */
};

/**
 * Default implementations for do_init, do_write, do_close that
 * do nothing and pass the data through.
 */
CURLcode Curl_creader_def_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creader *reader);
ਖਾਲੀ Curl_creader_def_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader);
CURLcode Curl_creader_def_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creader *reader,
                               ਅੱਖਰ *buf, size_t blen,
                               size_t *nread, bool *eos);
bool Curl_creader_def_needs_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_creader *reader);
curl_off_t Curl_creader_def_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ Curl_creader *reader);
CURLcode Curl_creader_def_resume_from(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_creader *reader,
                                      curl_off_t ਵਿੱਥ);
CURLcode Curl_creader_def_cntrl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_creader *reader,
                                Curl_creader_cntrl opcode);
bool Curl_creader_def_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_creader *reader);
ਖਾਲੀ Curl_creader_def_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_creader *reader, ਪੂਰਨ premature);

/**
 * Convenience method for calling `reader->do_read()` that
 * checks for NULL reader.
 */
CURLcode Curl_creader_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_creader *reader,
                           ਅੱਖਰ *buf, size_t blen, size_t *nread, bool *eos);

/* Tell the reader and all below that any EOS state is to be cleared */
ਖਾਲੀ Curl_creader_clear_eos(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader);

/**
 * Create a new creader instance with given type and phase. Is not
 * inserted into the writer chain by this call.
 * Invokes `reader->do_init()`.
 */
CURLcode Curl_creader_create(ਬਣਤਰ Curl_creader **preader,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ Curl_crtype *crt,
                             Curl_creader_phase phase);

/**
 * Free a creader instance.
 * Invokes `reader->do_close()`.
 */
ਖਾਲੀ Curl_creader_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader);

/**
 * Adds a reader to the transfer's reader chain.
 * The readers `phase` determines where in the chain it is inserted.
 */
CURLcode Curl_creader_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_creader *reader);

/**
 * Set the given reader, which needs to be of type CURL_CR_CLIENT,
 * as the new first reader. Discard any installed readers and init
 * the reader chain anew.
 * The function takes ownership of `r`.
 */
CURLcode Curl_creader_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *r);

/**
 * Read at most `blen` bytes at `buf` from the client.
 * @param data    the transfer to read client bytes for
 * @param buf     the memory location to read to
 * @param blen    the amount of memory at `buf`
 * @param nread   on return the number of bytes read into `buf`
 * @param eos     TRUE iff bytes are the end of data from client
 * @return CURLE_OK on successful read (even 0 length) or error
 */
CURLcode Curl_client_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *buf, size_t blen,
                          size_t *nread, bool *eos) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

/**
 * TRUE iff client reader needs rewing before it can be used for
 * a retry request.
 */
bool Curl_creader_needs_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * TRUE iff client reader will rewind at next start
 */
bool Curl_creader_will_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * En-/disable rewind of client reader at next start.
 */
ਖਾਲੀ Curl_creader_ਸੈੱਟ_rewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable);

/**
 * Get the total length of bytes provided by the installed readers.
 * This is independent of the amount already delivered and is calculated
 * by all readers in the stack. If a reader like "chunked" or
 * "crlf conversion" is installed, the returned length will be -1.
 * @return -1 if length is indeterminate
 */
curl_off_t Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Get the total length of bytes provided by the reader at phase
 * CURL_CR_CLIENT. This may not match the amount of bytes read
 * for a request, depending if other, encoding readers are also installed.
 * However it allows for rough estimation of the overall length.
 * @return -1 if length is indeterminate
 */
curl_off_t Curl_creader_client_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Unpause all installed readers.
 */
CURLcode Curl_creader_unpause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Return TRUE iff any of the installed readers is paused.
 */
bool Curl_creader_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Tell all client readers that they are done.
 */
ਖਾਲੀ Curl_creader_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ premature);

/**
 * Look up an installed client reader on `data` by its type.
 * @return first reader with that type or NULL
 */
ਬਣਤਰ Curl_creader *Curl_creader_get_by_type(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਅਚਲ ਬਣਤਰ Curl_crtype *crt);

/**
 * Set the client reader to provide 0 bytes, immediate EOS.
 */
CURLcode Curl_creader_ਸੈੱਟ_null(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Set the client reader the reads from fread callback.
 */
CURLcode Curl_creader_ਸੈੱਟ_fread(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t len);

/**
 * Set the client reader the reads from the supplied buf (NOT COPIED).
 */
CURLcode Curl_creader_ਸੈੱਟ_buf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *buf, size_t blen);

#ਜੇ_ਅੰਤ /* HEADER_CURL_SENDF_H */

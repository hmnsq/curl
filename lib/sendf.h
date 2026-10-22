#ifndef HEADER_CURL_SENDF_H
#define HEADER_CURL_SENDF_H
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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "cw-out.h"

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
#define CLIENTWRITE_BODY    (1 << 0) /* non-meta information, BODY */
#define CLIENTWRITE_INFO    (1 << 1) /* meta information, not a HEADER */
#define CLIENTWRITE_HEADER  (1 << 2) /* meta information, HEADER */
#define CLIENTWRITE_STATUS  (1 << 3) /* a special status HEADER */
#define CLIENTWRITE_CONNECT (1 << 4) /* a CONNECT related HEADER */
#define CLIENTWRITE_1XX     (1 << 5) /* a 1xx response related HEADER */
#define CLIENTWRITE_TRAILER (1 << 6) /* a trailer HEADER */
#define CLIENTWRITE_EOS     (1 << 7) /* End Of transfer download Stream */
#define CLIENTWRITE_0LEN    (1 << 8) /* write even 0-length buffers */

/* Forward declarations */
ഘടന Curl_creader;
ഘടന Curl_cwriter;
ഘടന Curl_easy;

/**
 * Write `len` bytes at `buf` to the client. `type` indicates what
 * kind of data is being written.
 */
CURLcode Curl_client_write(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം *buf,
                           size_t len) WARN_UNUSED_RESULT;

CURLcode Curl_client_flush(ഘടന Curl_easy *data);

/**
 * Free all resources related to client writing.
 */
ശൂന്യം Curl_client_cleanup(ഘടന Curl_easy *data);

/**
 * Reset readers and writer chains, keep rewind information
 * when necessary.
 */
ശൂന്യം Curl_client_reset(ഘടന Curl_easy *data);

/**
 * A new request is starting, perform any ops like rewinding
 * previous readers when needed.
 */
CURLcode Curl_client_start(ഘടന Curl_easy *data);

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
തരനിർവചനം എണ്ണൽ {
  CURL_CW_RAW,  /* raw data written, before any decoding */
  CURL_CW_TRANSFER_DECODE, /* remove transfer-encodings */
  CURL_CW_PROTOCOL, /* after transfer, but before content decoding */
  CURL_CW_BEFORE_DECODE, /* after protocol, but before content decoding */
  CURL_CW_CONTENT_DECODE, /* remove content-encodings */
  CURL_CW_CLIENT  /* data written to client */
} Curl_cwriter_phase;

/* writer may blow up size of write data, e.g. zip bombs */
#define CURL_CW_FLAG_BLOWUP     (1U << 0)

/* Client Writer Type, provides the implementation */
ഘടന Curl_cwtype {
  സ്ഥിരം അക്ഷരം *name;        /* writer name. */
  സ്ഥിരം അക്ഷരം *alias;       /* writer name alias, maybe NULL. */
  uint8_t flags;           /* flags for writer behaviour */
  CURLcode (*do_init)(ഘടന Curl_easy *data,
                      ഘടന Curl_cwriter *writer);
  CURLcode (*do_write)(ഘടന Curl_easy *data,
                       ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                       സ്ഥിരം അക്ഷരം *buf, size_t nbytes);
  CURLcode (*do_flush)(ഘടന Curl_easy *data,
                       ഘടന Curl_cwriter *writer);
  ശൂന്യം (*do_close)(ഘടന Curl_easy *data,
                   ഘടന Curl_cwriter *writer);
  size_t cwriter_size;  /* sizeof() allocated struct Curl_cwriter */
};

/* Client writer instance, allocated on creation.
 * `void *ctx` is the pointer from the allocation of
 * the `struct Curl_cwriter` itself. This is suitable for "downcasting"
 * by the writers implementation. See https://github.com/curl/curl/pull/13054
 * for the alignment problems that arise otherwise.
 */
ഘടന Curl_cwriter {
  സ്ഥിരം ഘടന Curl_cwtype *cwt;  /* type implementation */
  ഘടന Curl_cwriter *next;  /* Downstream writer. */
  ശൂന്യം *ctx;                  /* allocated instance pointer */
  Curl_cwriter_phase phase; /* phase at which it operates */
};

/**
 * Create a new cwriter instance with given type and phase. Is not
 * inserted into the writer chain by this call.
 * Invokes `writer->do_init()`.
 */
CURLcode Curl_cwriter_create(ഘടന Curl_cwriter **pwriter,
                             ഘടന Curl_easy *data,
                             സ്ഥിരം ഘടന Curl_cwtype *cwt,
                             Curl_cwriter_phase phase);

/**
 * Free a cwriter instance.
 * Invokes `writer->do_close()`.
 */
ശൂന്യം Curl_cwriter_free(ഘടന Curl_easy *data,
                       ഘടന Curl_cwriter *writer);

/**
 * Count the number of writers installed of the given phase.
 */
size_t Curl_cwriter_എണ്ണം(ഘടന Curl_easy *data, Curl_cwriter_phase phase);

/**
 * Adds a writer to the transfer's writer chain.
 * The writers `phase` determines where in the chain it is inserted.
 */
CURLcode Curl_cwriter_add(ഘടന Curl_easy *data,
                          ഘടന Curl_cwriter *writer);

/**
 * Look up an installed client writer on `data` by its type.
 * @return first writer with that type or NULL
 */
ഘടന Curl_cwriter *Curl_cwriter_get_by_type(ഘടന Curl_easy *data,
                                              സ്ഥിരം ഘടന Curl_cwtype *cwt);

ഘടന Curl_cwriter *Curl_cwriter_get_by_name(ഘടന Curl_easy *data,
                                              സ്ഥിരം അക്ഷരം *name);

/* Convenience method for calling `writer->do_write()` that
 * checks for NULL writer. */
#define Curl_cwriter_write(d, w, t, b, n) \
  ((w) ? (w)->cwt->do_write((d), (w), (t), (b), (n)) : CURLE_WRITE_ERROR)

#define Curl_cwriter_flush(d, w) \
  ((w) ? (w)->cwt->do_flush((d), (w)) : CURLE_WRITE_ERROR)

/* TRUE if client writer is paused. */
#define Curl_cwriter_is_paused(d)     ((bool)(d)->req.writer.paused)

/**
 * Unpause client writer and flush any buffered date to the client.
 */
CURLcode Curl_cwriter_unpause(ഘടന Curl_easy *data);

/**
 * Default implementations for do_init, do_write, do_close that
 * do nothing and pass the data through.
 */
CURLcode Curl_cwriter_def_init(ഘടന Curl_easy *data,
                               ഘടന Curl_cwriter *writer);
CURLcode Curl_cwriter_def_write(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                                സ്ഥിരം അക്ഷരം *buf, size_t nbytes);
CURLcode Curl_cwriter_def_flush(ഘടന Curl_easy *data,
                                ഘടന Curl_cwriter *writer);
ശൂന്യം Curl_cwriter_def_close(ഘടന Curl_easy *data,
                            ഘടന Curl_cwriter *writer);

തരനിർവചനം എണ്ണൽ {
  CURL_CRCNTRL_REWIND,
  CURL_CRCNTRL_UNPAUSE,
  CURL_CRCNTRL_CLEAR_EOS
} Curl_creader_cntrl;

/* Client Reader Type, provides the implementation */
ഘടന Curl_crtype {
  സ്ഥിരം അക്ഷരം *name;        /* writer name. */
  CURLcode (*do_init)(ഘടന Curl_easy *data, ഘടന Curl_creader *reader);
  CURLcode (*do_read)(ഘടന Curl_easy *data, ഘടന Curl_creader *reader,
                      അക്ഷരം *buf, size_t blen, size_t *nread, bool *eos);
  ശൂന്യം (*do_close)(ഘടന Curl_easy *data, ഘടന Curl_creader *reader);
  bool (*needs_rewind)(ഘടന Curl_easy *data, ഘടന Curl_creader *reader);
  curl_off_t (*total_length)(ഘടന Curl_easy *data,
                             ഘടന Curl_creader *reader);
  CURLcode (*resume_from)(ഘടന Curl_easy *data,
                          ഘടന Curl_creader *reader, curl_off_t offset);
  CURLcode (*cntrl)(ഘടന Curl_easy *data, ഘടന Curl_creader *reader,
                    Curl_creader_cntrl opcode);
  bool (*is_paused)(ഘടന Curl_easy *data, ഘടന Curl_creader *reader);
  ശൂന്യം (*done)(ഘടന Curl_easy *data,
               ഘടന Curl_creader *reader, പൂർണ്ണസംഖ്യ premature);
  size_t creader_size;  /* sizeof() allocated struct Curl_creader */
};

/* Phase a reader operates at. */
തരനിർവചനം എണ്ണൽ {
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
ഘടന Curl_creader {
  സ്ഥിരം ഘടന Curl_crtype *crt;  /* type implementation */
  ഘടന Curl_creader *next;  /* Downstream reader. */
  ശൂന്യം *ctx;
  Curl_creader_phase phase; /* phase at which it operates */
};

/**
 * Default implementations for do_init, do_write, do_close that
 * do nothing and pass the data through.
 */
CURLcode Curl_creader_def_init(ഘടന Curl_easy *data,
                               ഘടന Curl_creader *reader);
ശൂന്യം Curl_creader_def_close(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader);
CURLcode Curl_creader_def_read(ഘടന Curl_easy *data,
                               ഘടന Curl_creader *reader,
                               അക്ഷരം *buf, size_t blen,
                               size_t *nread, bool *eos);
bool Curl_creader_def_needs_rewind(ഘടന Curl_easy *data,
                                   ഘടന Curl_creader *reader);
curl_off_t Curl_creader_def_total_length(ഘടന Curl_easy *data,
                                         ഘടന Curl_creader *reader);
CURLcode Curl_creader_def_resume_from(ഘടന Curl_easy *data,
                                      ഘടന Curl_creader *reader,
                                      curl_off_t offset);
CURLcode Curl_creader_def_cntrl(ഘടന Curl_easy *data,
                                ഘടന Curl_creader *reader,
                                Curl_creader_cntrl opcode);
bool Curl_creader_def_is_paused(ഘടന Curl_easy *data,
                                ഘടന Curl_creader *reader);
ശൂന്യം Curl_creader_def_done(ഘടന Curl_easy *data,
                           ഘടന Curl_creader *reader, പൂർണ്ണസംഖ്യ premature);

/**
 * Convenience method for calling `reader->do_read()` that
 * checks for NULL reader.
 */
CURLcode Curl_creader_read(ഘടന Curl_easy *data,
                           ഘടന Curl_creader *reader,
                           അക്ഷരം *buf, size_t blen, size_t *nread, bool *eos);

/* Tell the reader and all below that any EOS state is to be cleared */
ശൂന്യം Curl_creader_clear_eos(ഘടന Curl_easy *data,
                            ഘടന Curl_creader *reader);

/**
 * Create a new creader instance with given type and phase. Is not
 * inserted into the writer chain by this call.
 * Invokes `reader->do_init()`.
 */
CURLcode Curl_creader_create(ഘടന Curl_creader **preader,
                             ഘടന Curl_easy *data,
                             സ്ഥിരം ഘടന Curl_crtype *crt,
                             Curl_creader_phase phase);

/**
 * Free a creader instance.
 * Invokes `reader->do_close()`.
 */
ശൂന്യം Curl_creader_free(ഘടന Curl_easy *data, ഘടന Curl_creader *reader);

/**
 * Adds a reader to the transfer's reader chain.
 * The readers `phase` determines where in the chain it is inserted.
 */
CURLcode Curl_creader_add(ഘടന Curl_easy *data,
                          ഘടന Curl_creader *reader);

/**
 * Set the given reader, which needs to be of type CURL_CR_CLIENT,
 * as the new first reader. Discard any installed readers and init
 * the reader chain anew.
 * The function takes ownership of `r`.
 */
CURLcode Curl_creader_set(ഘടന Curl_easy *data, ഘടന Curl_creader *r);

/**
 * Read at most `blen` bytes at `buf` from the client.
 * @param data    the transfer to read client bytes for
 * @param buf     the memory location to read to
 * @param blen    the amount of memory at `buf`
 * @param nread   on return the number of bytes read into `buf`
 * @param eos     TRUE iff bytes are the end of data from client
 * @return CURLE_OK on successful read (even 0 length) or error
 */
CURLcode Curl_client_read(ഘടന Curl_easy *data, അക്ഷരം *buf, size_t blen,
                          size_t *nread, bool *eos) WARN_UNUSED_RESULT;

/**
 * TRUE iff client reader needs rewing before it can be used for
 * a retry request.
 */
bool Curl_creader_needs_rewind(ഘടന Curl_easy *data);

/**
 * TRUE iff client reader will rewind at next start
 */
bool Curl_creader_will_rewind(ഘടന Curl_easy *data);

/**
 * En-/disable rewind of client reader at next start.
 */
ശൂന്യം Curl_creader_set_rewind(ഘടന Curl_easy *data, bool enable);

/**
 * Get the total length of bytes provided by the installed readers.
 * This is independent of the amount already delivered and is calculated
 * by all readers in the stack. If a reader like "chunked" or
 * "crlf conversion" is installed, the returned length will be -1.
 * @return -1 if length is indeterminate
 */
curl_off_t Curl_creader_total_length(ഘടന Curl_easy *data);

/**
 * Get the total length of bytes provided by the reader at phase
 * CURL_CR_CLIENT. This may not match the amount of bytes read
 * for a request, depending if other, encoding readers are also installed.
 * However it allows for rough estimation of the overall length.
 * @return -1 if length is indeterminate
 */
curl_off_t Curl_creader_client_length(ഘടന Curl_easy *data);

/**
 * Unpause all installed readers.
 */
CURLcode Curl_creader_unpause(ഘടന Curl_easy *data);

/**
 * Return TRUE iff any of the installed readers is paused.
 */
bool Curl_creader_is_paused(ഘടന Curl_easy *data);

/**
 * Tell all client readers that they are done.
 */
ശൂന്യം Curl_creader_done(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ premature);

/**
 * Look up an installed client reader on `data` by its type.
 * @return first reader with that type or NULL
 */
ഘടന Curl_creader *Curl_creader_get_by_type(ഘടന Curl_easy *data,
                                              സ്ഥിരം ഘടന Curl_crtype *crt);

/**
 * Set the client reader to provide 0 bytes, immediate EOS.
 */
CURLcode Curl_creader_set_null(ഘടന Curl_easy *data);

/**
 * Set the client reader the reads from fread callback.
 */
CURLcode Curl_creader_set_fread(ഘടന Curl_easy *data, curl_off_t len);

/**
 * Set the client reader the reads from the supplied buf (NOT COPIED).
 */
CURLcode Curl_creader_set_buf(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *buf, size_t blen);

#endif /* HEADER_CURL_SENDF_H */

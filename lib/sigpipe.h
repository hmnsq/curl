#ifndef HEADER_CURL_SIGPIPE_H
#define HEADER_CURL_SIGPIPE_H
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

#اگر defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#شامل <signal.h>

ساختار Curl_sigpipe_ctx {
  ساختار sigaction old_pipe_act;
  BIT(no_signal);
};

ایستا CURL_INLINE تهی sigpipe_init(ساختار Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, اندازه(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
ایستا CURL_INLINE تهی sigpipe_ignore(ساختار Curl_easy *data,
                                       ساختار Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = data->set.no_signal;
  اگر(!data->set.no_signal) {
    ساختار sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, پوچ, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#ifdef SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#endif
    sigaction(SIGPIPE, &action, پوچ);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
ایستا CURL_INLINE تهی sigpipe_restore(ساختار Curl_sigpipe_ctx *ig)
{
  اگر(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, پوچ);
}

ایستا CURL_INLINE تهی sigpipe_apply(ساختار Curl_easy *data,
                                      ساختار Curl_sigpipe_ctx *ig)
{
  اگر(data && (data->set.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(data, ig);
  }
}

#وگرنه /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#define sigpipe_ignore(x, y) انجامبده { (تهی)(x); (تهی)(y); } وقتی(0)
#define sigpipe_apply(x, y)  انجامبده { (تهی)(x); (تهی)(y); } وقتی(0)
#define sigpipe_init(x)      انجامبده { (تهی)(x); } وقتی(0)
#define sigpipe_restore(x)   انجامبده { (تهی)(x); } وقتی(0)

ساختار Curl_sigpipe_ctx {
  bool dummy;
};

#endif /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#endif /* HEADER_CURL_SIGPIPE_H */

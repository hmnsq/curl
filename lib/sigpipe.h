#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SIGPIPE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SIGPIPE_H
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

#ਜੇ defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

ਬਣਤਰ Curl_sigpipe_ctx {
  ਬਣਤਰ sigaction old_pipe_act;
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_signal);
};

ਸਥਿਰ CURL_INLINE ਖਾਲੀ sigpipe_init(ਬਣਤਰ Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, ਆਕਾਰ(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
ਸਥਿਰ CURL_INLINE ਖਾਲੀ sigpipe_ignore(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = ਅੰਕੜੇ->ਸੈੱਟ.no_signal;
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.no_signal) {
    ਬਣਤਰ sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, ਨਿਰਾ, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#ਜੇ_ਅੰਤ
    sigaction(SIGPIPE, &action, ਨਿਰਾ);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
ਸਥਿਰ CURL_INLINE ਖਾਲੀ sigpipe_restore(ਬਣਤਰ Curl_sigpipe_ctx *ig)
{
  ਜੇ(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, ਨਿਰਾ);
}

ਸਥਿਰ CURL_INLINE ਖਾਲੀ sigpipe_apply(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_sigpipe_ctx *ig)
{
  ਜੇ(ਅੰਕੜੇ && (ਅੰਕੜੇ->ਸੈੱਟ.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(ਅੰਕੜੇ, ig);
  }
}

#ਨਹੀਂਤਾਂ /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sigpipe_ignore(x, y) ਕਰੋ { (ਖਾਲੀ)(x); (ਖਾਲੀ)(y); } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sigpipe_apply(x, y)  ਕਰੋ { (ਖਾਲੀ)(x); (ਖਾਲੀ)(y); } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sigpipe_init(x)      ਕਰੋ { (ਖਾਲੀ)(x); } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sigpipe_restore(x)   ਕਰੋ { (ਖਾਲੀ)(x); } ਜਦੋਂ(0)

ਬਣਤਰ Curl_sigpipe_ctx {
  bool dummy;
};

#ਜੇ_ਅੰਤ /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#ਜੇ_ਅੰਤ /* HEADER_CURL_SIGPIPE_H */

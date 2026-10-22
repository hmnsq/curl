#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SIGPIPE_H
#وصف_ڪريو سرو_CURL_SIGPIPE_H
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

#إذا defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#ضمن <signal.h>

بنية Curl_sigpipe_ctx {
  بنية sigaction old_pipe_act;
  ٻٽو_انگ(no_signal);
};

ساكن CURL_INLINE فراغ sigpipe_init(بنية Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, حجم(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
ساكن CURL_INLINE فراغ sigpipe_ignore(بنية Curl_easy *معلومات,
                                       بنية Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = معلومات->سيٽ.no_signal;
  إذا(!معلومات->سيٽ.no_signal) {
    بنية sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, عدم, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#جيڪڏهن_وصف_ٿيل SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#جيڪڏهن_پورو
    sigaction(SIGPIPE, &action, عدم);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
ساكن CURL_INLINE فراغ sigpipe_restore(بنية Curl_sigpipe_ctx *ig)
{
  إذا(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, عدم);
}

ساكن CURL_INLINE فراغ sigpipe_apply(بنية Curl_easy *معلومات,
                                      بنية Curl_sigpipe_ctx *ig)
{
  إذا(معلومات && (معلومات->سيٽ.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(معلومات, ig);
  }
}

#وإلا /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#وصف_ڪريو sigpipe_ignore(x, y) افعل { (فراغ)(x); (فراغ)(y); } طالما(0)
#وصف_ڪريو sigpipe_apply(x, y)  افعل { (فراغ)(x); (فراغ)(y); } طالما(0)
#وصف_ڪريو sigpipe_init(x)      افعل { (فراغ)(x); } طالما(0)
#وصف_ڪريو sigpipe_restore(x)   افعل { (فراغ)(x); } طالما(0)

بنية Curl_sigpipe_ctx {
  bool dummy;
};

#جيڪڏهن_پورو /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#جيڪڏهن_پورو /* HEADER_CURL_SIGPIPE_H */

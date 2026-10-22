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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#অন্তর্ভাব <signal.h>

কাঠামো Curl_sigpipe_ctx {
  কাঠামো sigaction old_pipe_act;
  BIT(no_signal);
};

স্থির CURL_INLINE শূন্য sigpipe_init(কাঠামো Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, আকার(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
স্থির CURL_INLINE শূন্য sigpipe_ignore(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = তথ্য->set.no_signal;
  যদি(!তথ্য->set.no_signal) {
    কাঠামো sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, ফাঁকা, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#ifdef SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#endif
    sigaction(SIGPIPE, &action, ফাঁকা);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
স্থির CURL_INLINE শূন্য sigpipe_restore(কাঠামো Curl_sigpipe_ctx *ig)
{
  যদি(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, ফাঁকা);
}

স্থির CURL_INLINE শূন্য sigpipe_apply(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো Curl_sigpipe_ctx *ig)
{
  যদি(তথ্য && (তথ্য->set.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(তথ্য, ig);
  }
}

#অন্যথা /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#define sigpipe_ignore(x, y) করো { (শূন্য)(x); (শূন্য)(y); } যখন(0)
#define sigpipe_apply(x, y)  করো { (শূন্য)(x); (শূন্য)(y); } যখন(0)
#define sigpipe_init(x)      করো { (শূন্য)(x); } যখন(0)
#define sigpipe_restore(x)   করো { (শূন্য)(x); } যখন(0)

কাঠামো Curl_sigpipe_ctx {
  bool dummy;
};

#endif /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#endif /* HEADER_CURL_SIGPIPE_H */

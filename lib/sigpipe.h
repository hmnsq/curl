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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#ಅನ್ತರ್ಭಾವ <signal.h>

ಸಂರಚನಾ Curl_sigpipe_ctx {
  ಸಂರಚನಾ sigaction old_pipe_act;
  BIT(no_signal);
};

ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ sigpipe_init(ಸಂರಚನಾ Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, ಪರಿಮಾಣ(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ sigpipe_ignore(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = data->set.no_signal;
  ಯದಿ(!data->set.no_signal) {
    ಸಂರಚನಾ sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, ರಿಕ್ತ, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#ifdef SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#endif
    sigaction(SIGPIPE, &action, ರಿಕ್ತ);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ sigpipe_restore(ಸಂರಚನಾ Curl_sigpipe_ctx *ig)
{
  ಯದಿ(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, ರಿಕ್ತ);
}

ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ sigpipe_apply(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ Curl_sigpipe_ctx *ig)
{
  ಯದಿ(data && (data->set.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(data, ig);
  }
}

#ಅನ್ಯಥಾ /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#define sigpipe_ignore(x, y) ಕುರು { (ಶೂನ್ಯ)(x); (ಶೂನ್ಯ)(y); } ಯಾವತ್(0)
#define sigpipe_apply(x, y)  ಕುರು { (ಶೂನ್ಯ)(x); (ಶೂನ್ಯ)(y); } ಯಾವತ್(0)
#define sigpipe_init(x)      ಕುರು { (ಶೂನ್ಯ)(x); } ಯಾವತ್(0)
#define sigpipe_restore(x)   ಕುರು { (ಶೂನ್ಯ)(x); } ಯಾವತ್(0)

ಸಂರಚನಾ Curl_sigpipe_ctx {
  bool dummy;
};

#endif /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#endif /* HEADER_CURL_SIGPIPE_H */

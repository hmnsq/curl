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
#अन्तर्भाव "curl_setup.h"

#जइ defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#अन्तर्भाव <signal.h>

संरचना Curl_sigpipe_ctx {
  संरचना sigaction old_pipe_act;
  BIT(no_signal);
};

स्थिर CURL_INLINE शून्य sigpipe_init(संरचना Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, परिमाण(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
स्थिर CURL_INLINE शून्य sigpipe_ignore(संरचना Curl_easy *data,
                                       संरचना Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = data->set.no_signal;
  जइ(!data->set.no_signal) {
    संरचना sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, रिक्त, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#ifdef SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#endif
    sigaction(SIGPIPE, &action, रिक्त);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
स्थिर CURL_INLINE शून्य sigpipe_restore(संरचना Curl_sigpipe_ctx *ig)
{
  जइ(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, रिक्त);
}

स्थिर CURL_INLINE शून्य sigpipe_apply(संरचना Curl_easy *data,
                                      संरचना Curl_sigpipe_ctx *ig)
{
  जइ(data && (data->set.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(data, ig);
  }
}

#अण्णहा /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#define sigpipe_ignore(x, y) करेहि { (शून्य)(x); (शून्य)(y); } जाव(0)
#define sigpipe_apply(x, y)  करेहि { (शून्य)(x); (शून्य)(y); } जाव(0)
#define sigpipe_init(x)      करेहि { (शून्य)(x); } जाव(0)
#define sigpipe_restore(x)   करेहि { (शून्य)(x); } जाव(0)

संरचना Curl_sigpipe_ctx {
  bool dummy;
};

#endif /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#endif /* HEADER_CURL_SIGPIPE_H */

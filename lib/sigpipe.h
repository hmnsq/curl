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
#அந்தர்பாவ "curl_setup.h"

#எனில் defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#அந்தர்பாவ <signal.h>

கட்டமைப்பு Curl_sigpipe_ctx {
  கட்டமைப்பு sigaction old_pipe_act;
  BIT(no_signal);
};

நிலையான CURL_INLINE வெறுமை sigpipe_init(கட்டமைப்பு Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, அளவு(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
நிலையான CURL_INLINE வெறுமை sigpipe_ignore(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = data->set.no_signal;
  எனில்(!data->set.no_signal) {
    கட்டமைப்பு sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, வெற்று, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#ifdef SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#endif
    sigaction(SIGPIPE, &action, வெற்று);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
நிலையான CURL_INLINE வெறுமை sigpipe_restore(கட்டமைப்பு Curl_sigpipe_ctx *ig)
{
  எனில்(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, வெற்று);
}

நிலையான CURL_INLINE வெறுமை sigpipe_apply(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு Curl_sigpipe_ctx *ig)
{
  எனில்(data && (data->set.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(data, ig);
  }
}

#இல்லையெனில் /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#define sigpipe_ignore(x, y) செய் { (வெறுமை)(x); (வெறுமை)(y); } போது(0)
#define sigpipe_apply(x, y)  செய் { (வெறுமை)(x); (வெறுமை)(y); } போது(0)
#define sigpipe_init(x)      செய் { (வெறுமை)(x); } போது(0)
#define sigpipe_restore(x)   செய் { (வெறுமை)(x); } போது(0)

கட்டமைப்பு Curl_sigpipe_ctx {
  bool dummy;
};

#endif /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#endif /* HEADER_CURL_SIGPIPE_H */

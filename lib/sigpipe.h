#यदि_अपरिभाषितम् शीर्षकम्_CURL_SIGPIPE_H
#परिभाषय शीर्षकम्_CURL_SIGPIPE_H
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

#यदि defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#अन्तर्भाव <signal.h>

संरचना Curl_sigpipe_ctx {
  संरचना sigaction old_pipe_act;
  BIT(no_signal);
};

स्थिर CURL_INLINE शून्य sigpipe_init(संरचना Curl_sigpipe_ctx *ig)
{
  स्मृतिं_पूरय(ig, 0, परिमाण(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
स्थिर CURL_INLINE शून्य sigpipe_ignore(संरचना Curl_easy *दत्त,
                                       संरचना Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = दत्त->समुच्चयः.no_signal;
  यदि(!दत्त->समुच्चयः.no_signal) {
    संरचना sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, रिक्त, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#यदि_परिभाषितम् SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#यदि_अन्तः
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
  यदि(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, रिक्त);
}

स्थिर CURL_INLINE शून्य sigpipe_apply(संरचना Curl_easy *दत्त,
                                      संरचना Curl_sigpipe_ctx *ig)
{
  यदि(दत्त && (दत्त->समुच्चयः.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(दत्त, ig);
  }
}

#अन्यथा /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#परिभाषय sigpipe_ignore(x, y) कुरु { (शून्य)(x); (शून्य)(y); } यावत्(0)
#परिभाषय sigpipe_apply(x, y)  कुरु { (शून्य)(x); (शून्य)(y); } यावत्(0)
#परिभाषय sigpipe_init(x)      कुरु { (शून्य)(x); } यावत्(0)
#परिभाषय sigpipe_restore(x)   कुरु { (शून्य)(x); } यावत्(0)

संरचना Curl_sigpipe_ctx {
  bool dummy;
};

#यदि_अन्तः /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#यदि_अन्तः /* HEADER_CURL_SIGPIPE_H */

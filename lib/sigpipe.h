#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SIGPIPE_H
#నిర్వచించు శీర్షిక_CURL_SIGPIPE_H
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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#అన్తర్భావ <signal.h>

నిర్మాణం Curl_sigpipe_ctx {
  నిర్మాణం sigaction old_pipe_act;
  బిట్టు(no_signal);
};

స్థిర CURL_INLINE శూన్య sigpipe_init(నిర్మాణం Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, పరిమాణం(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
స్థిర CURL_INLINE శూన్య sigpipe_ignore(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = దత్తాంశం->సమితి.no_signal;
  ఒకవేళ(!దత్తాంశం->సమితి.no_signal) {
    నిర్మాణం sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, ఖాళీ, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#నిర్వచితమైతే SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#అయితే_ముగింపు
    sigaction(SIGPIPE, &action, ఖాళీ);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
స్థిర CURL_INLINE శూన్య sigpipe_restore(నిర్మాణం Curl_sigpipe_ctx *ig)
{
  ఒకవేళ(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, ఖాళీ);
}

స్థిర CURL_INLINE శూన్య sigpipe_apply(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం Curl_sigpipe_ctx *ig)
{
  ఒకవేళ(దత్తాంశం && (దత్తాంశం->సమితి.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(దత్తాంశం, ig);
  }
}

#లేకపోతే /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#నిర్వచించు sigpipe_ignore(x, y) చేయి { (శూన్య)(x); (శూన్య)(y); } అయితే(0)
#నిర్వచించు sigpipe_apply(x, y)  చేయి { (శూన్య)(x); (శూన్య)(y); } అయితే(0)
#నిర్వచించు sigpipe_init(x)      చేయి { (శూన్య)(x); } అయితే(0)
#నిర్వచించు sigpipe_restore(x)   చేయి { (శూన్య)(x); } అయితే(0)

నిర్మాణం Curl_sigpipe_ctx {
  bool dummy;
};

#అయితే_ముగింపు /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#అయితే_ముగింపు /* HEADER_CURL_SIGPIPE_H */

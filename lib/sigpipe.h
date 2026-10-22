#अगर_अघोषित HEADER_CURL_SIGPIPE_H
#घोषणा HEADER_CURL_SIGPIPE_H
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
#समावेश "curl_setup.h"

#अगर defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#समावेश <signal.h>

काष्ठा Curl_sigpipe_ctx {
  काष्ठा sigaction old_pipe_act;
  BIT(no_signal);
};

अटल CURL_INLINE व्योम sigpipe_init(काष्ठा Curl_sigpipe_ctx *ig)
{
  स_रखो(ig, 0, माप(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
अटल CURL_INLINE व्योम sigpipe_ignore(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = तथ्य->set.no_signal;
  अगर(!तथ्य->set.no_signal) {
    काष्ठा sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, शून्य, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = संक_छोड़ो;
#अगर_घोषित SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#पूर्ण_अगर
    sigaction(SIGPIPE, &action, शून्य);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
अटल CURL_INLINE व्योम sigpipe_restore(काष्ठा Curl_sigpipe_ctx *ig)
{
  अगर(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, शून्य);
}

अटल CURL_INLINE व्योम sigpipe_apply(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_sigpipe_ctx *ig)
{
  अगर(तथ्य && (तथ्य->set.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(तथ्य, ig);
  }
}

#अन्यथा /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#घोषणा sigpipe_ignore(x, y) करो { (व्योम)(x); (व्योम)(y); } जबतक(0)
#घोषणा sigpipe_apply(x, y)  करो { (व्योम)(x); (व्योम)(y); } जबतक(0)
#घोषणा sigpipe_init(x)      करो { (व्योम)(x); } जबतक(0)
#घोषणा sigpipe_restore(x)   करो { (व्योम)(x); } जबतक(0)

काष्ठा Curl_sigpipe_ctx {
  bool dummy;
};

#पूर्ण_अगर /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#पूर्ण_अगर /* HEADER_CURL_SIGPIPE_H */

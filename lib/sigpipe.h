#اگر_غیر_تعریف_شدہ سرخی_CURL_SIGPIPE_H
#تعریف_کرو سرخی_CURL_SIGPIPE_H
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

ساخت Curl_sigpipe_ctx {
  ساخت sigaction old_pipe_act;
  ثنائی_ہندسہ(no_signal);
};

ساکن CURL_INLINE خالی sigpipe_init(ساخت Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, حجم(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
ساکن CURL_INLINE خالی sigpipe_ignore(ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = ڈیٹا->سیٹ.no_signal;
  اگر(!ڈیٹا->سیٹ.no_signal) {
    ساخت sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, تہی, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#اگر_تعریف_شدہ SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#اگر_ختم
    sigaction(SIGPIPE, &action, تہی);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
ساکن CURL_INLINE خالی sigpipe_restore(ساخت Curl_sigpipe_ctx *ig)
{
  اگر(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, تہی);
}

ساکن CURL_INLINE خالی sigpipe_apply(ساخت Curl_easy *ڈیٹا,
                                      ساخت Curl_sigpipe_ctx *ig)
{
  اگر(ڈیٹا && (ڈیٹا->سیٹ.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(ڈیٹا, ig);
  }
}

#ورنہ /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#تعریف_کرو sigpipe_ignore(x, y) کرو { (خالی)(x); (خالی)(y); } جب(0)
#تعریف_کرو sigpipe_apply(x, y)  کرو { (خالی)(x); (خالی)(y); } جب(0)
#تعریف_کرو sigpipe_init(x)      کرو { (خالی)(x); } جب(0)
#تعریف_کرو sigpipe_restore(x)   کرو { (خالی)(x); } جب(0)

ساخت Curl_sigpipe_ctx {
  bool dummy;
};

#اگر_ختم /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#اگر_ختم /* HEADER_CURL_SIGPIPE_H */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SIGPIPE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SIGPIPE_H
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

#ಆದರೆ defined(HAVE_SIGACTION) && !defined(USE_SO_NOSIGPIPE)
#ಅನ್ತರ್ಭಾವ <signal.h>

ರಚನೆ Curl_sigpipe_ctx {
  ರಚನೆ sigaction old_pipe_act;
  ಬಿಟ್(no_signal);
};

ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ sigpipe_init(ರಚನೆ Curl_sigpipe_ctx *ig)
{
  memset(ig, 0, ಗಾತ್ರ(*ig));
  ig->no_signal = TRUE;
}

/*
 * sigpipe_ignore() makes sure we ignore SIGPIPE while running libcurl
 * internals, and then sigpipe_restore() will restore the situation when we
 * return from libcurl again.
 */
ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ sigpipe_ignore(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_sigpipe_ctx *ig)
{
  /* get a local copy of no_signal because the Curl_easy might not be
     around when we restore */
  ig->no_signal = ದತ್ತಾಂಶ->ಗಣ.no_signal;
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.no_signal) {
    ರಚನೆ sigaction action;
    /* first, extract the existing situation */
    sigaction(SIGPIPE, ಖಾಲಿ, &ig->old_pipe_act);
    action = ig->old_pipe_act;
    /* ignore this signal */
    action.sa_handler = SIG_IGN;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SA_SIGINFO
    /* clear SA_SIGINFO flag since we are using sa_handler */
    action.sa_flags &= ~SA_SIGINFO;
#ಆದರೆ_ಅಂತ್ಯ
    sigaction(SIGPIPE, &action, ಖಾಲಿ);
  }
}

/*
 * sigpipe_restore() puts back the outside world's opinion of signal handler
 * and SIGPIPE handling. It MUST only be called after a corresponding
 * sigpipe_ignore() was used.
 */
ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ sigpipe_restore(ರಚನೆ Curl_sigpipe_ctx *ig)
{
  ಆದರೆ(!ig->no_signal)
    /* restore the outside state */
    sigaction(SIGPIPE, &ig->old_pipe_act, ಖಾಲಿ);
}

ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ sigpipe_apply(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ Curl_sigpipe_ctx *ig)
{
  ಆದರೆ(ದತ್ತಾಂಶ && (ದತ್ತಾಂಶ->ಗಣ.no_signal != ig->no_signal)) {
    sigpipe_restore(ig);
    sigpipe_ignore(ದತ್ತಾಂಶ, ig);
  }
}

#ಇಲ್ಲದಿದ್ದರೆ /* !HAVE_SIGACTION || USE_SO_NOSIGPIPE */
/* for systems without sigaction or where SO_NOSIGPIPE is used. */
#ವ್ಯಾಖ್ಯಾನಿಸು sigpipe_ignore(x, y) ಮಾಡು { (ಶೂನ್ಯ)(x); (ಶೂನ್ಯ)(y); } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು sigpipe_apply(x, y)  ಮಾಡು { (ಶೂನ್ಯ)(x); (ಶೂನ್ಯ)(y); } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು sigpipe_init(x)      ಮಾಡು { (ಶೂನ್ಯ)(x); } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು sigpipe_restore(x)   ಮಾಡು { (ಶೂನ್ಯ)(x); } ಆಗ(0)

ರಚನೆ Curl_sigpipe_ctx {
  bool dummy;
};

#ಆದರೆ_ಅಂತ್ಯ /* else HAVE_SIGACTION && !USE_SO_NOSIGPIPE */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SIGPIPE_H */

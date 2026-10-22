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

#ಅನ್ತರ್ಭಾವ "fake_addrinfo.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FAKE_GETADDRINFO

#ಅನ್ತರ್ಭಾವ <ares.h>

ಶೂನ್ಯ r_freeaddrinfo(ರಚನೆ addrinfo *cahead)
{
  ರಚನೆ addrinfo *canext;
  ರಚನೆ addrinfo *ca;

  ಗಾಗಿ(ca = cahead; ca; ca = canext) {
    canext = ca->ai_ಮುಂದಿನ;
    curlx_free(ca);
  }
}

ರಚನೆ context {
  ರಚನೆ ares_addrinfo *addr;
  ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;
};

ಸ್ಥಿರ ಶೂನ್ಯ async_addrinfo_cb(ಶೂನ್ಯ *userp, ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ timeouts,
                              ರಚನೆ ares_addrinfo *addr)
{
  ರಚನೆ context *ctx = (ರಚನೆ context *)userp;
  (ಶೂನ್ಯ)timeouts;
  ctx->ಸ್ಥಿತಿ_2 = ಸ್ಥಿತಿ_2;
  ಆದರೆ(ARES_SUCCESS == ಸ್ಥಿತಿ_2) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
ಸ್ಥಿರ ರಚನೆ addrinfo *mk_getaddrinfo(ಸ್ಥಿರಾಂಕ ರಚನೆ ares_addrinfo *aihead)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ ares_addrinfo_ಗಂಟು *ai;
  ರಚನೆ addrinfo *ca;
  ರಚನೆ addrinfo *cafirst = ಖಾಲಿ;
  ರಚನೆ addrinfo *calast = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = aihead->name;

  /* traverse the addrinfo list */
  ಗಾಗಿ(ai = aihead->nodes; ai; ai = ai->ai_ಮುಂದಿನ) {
    size_t ss_ಗಾತ್ರ;
    size_t namelen = name ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ಆದರೆ(ai->ai_family == AF_INET)
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ai->ai_family == AF_INET6)
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
    ಇಲ್ಲದಿದ್ದರೆ
      ಮುಂದುವರಿ;

    /* ignore elements without required address info */
    ಆದರೆ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ಮುಂದುವರಿ;

    /* ignore elements with bogus address size */
    ಆದರೆ((size_t)ai->ai_addrlen < ss_ಗಾತ್ರ)
      ಮುಂದುವರಿ;

    ca = curlx_malloc(ಗಾತ್ರ(ರಚನೆ addrinfo) + ss_ಗಾತ್ರ + namelen);
    ಆದರೆ(!ca) {
      r_freeaddrinfo(cafirst);
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_ಗಾತ್ರ;
    ca->ai_addr      = ಖಾಲಿ;
    ca->ai_canonname = ಖಾಲಿ;
    ca->ai_ಮುಂದಿನ      = ಖಾಲಿ;

    ca->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ca + ಗಾತ್ರ(ರಚನೆ addrinfo));
    ಸ್ಮರಣೆ_ನಕಲು(ca->ai_addr, ai->ai_addr, ss_ಗಾತ್ರ);

    ಆದರೆ(namelen) {
      ca->ai_canonname = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ca->ai_addr + ss_ಗಾತ್ರ);
      ಸ್ಮರಣೆ_ನಕಲು(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = ಖಾಲಿ;
    }

    /* if the return list is empty, this becomes the first element */
    ಆದರೆ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ಆದರೆ(calast)
      calast->ai_ಮುಂದಿನ = ca;
    calast = ca;
  }

  ಹಿಂತಿರುಗಿ cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
ಪೂರ್ಣಾಂಕ r_getaddrinfo(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗಂಟು,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service,
                  ಸ್ಥಿರಾಂಕ ರಚನೆ addrinfo *hints,
                  ರಚನೆ addrinfo **res)
{
  ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;
  ರಚನೆ context ctx;
  ರಚನೆ ares_options options;
  ಪೂರ್ಣಾಂಕ optmask = 0;
  ರಚನೆ ares_addrinfo_hints ahints;
  ares_channel channel;
  ಪೂರ್ಣಾಂಕ rc = 0;

  memset(&options, 0, ಗಾತ್ರ(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, ಗಾತ್ರ(ahints));
  memset(&ctx, 0, ಗಾತ್ರ(ctx));

  ಆದರೆ(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  ಸ್ಥಿತಿ_2 = ares_init_options(&channel, &options, optmask);
  ಆದರೆ(ಸ್ಥಿತಿ_2)
    ಹಿಂತಿರುಗಿ EAI_MEMORY; /* major problem */

  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env = getenv("CURL_DNS_SERVER");
    ಆದರೆ(env) {
      rc = ares_ಗಣ_servers_ports_csv(channel, env);
      ಆದರೆ(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        ಹಿಂತಿರುಗಿ EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, ಗಂಟು, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_ಸರತಿ_wait_ಖಾಲಿಗಳು(channel, -1);

  ಆದರೆ(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ctx.ಸ್ಥಿತಿ_2 == ARES_ENOTFOUND) || (ctx.ಸ್ಥಿತಿ_2 == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  ಇಲ್ಲದಿದ್ದರೆ
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  ಹಿಂತಿರುಗಿ rc;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_FAKE_GETADDRINFO */

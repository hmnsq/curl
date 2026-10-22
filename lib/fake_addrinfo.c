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

#ifdef USE_FAKE_GETADDRINFO

#ಅನ್ತರ್ಭಾವ <ares.h>

ಶೂನ್ಯ r_freeaddrinfo(ಸಂರಚನಾ addrinfo *cahead)
{
  ಸಂರಚನಾ addrinfo *canext;
  ಸಂರಚನಾ addrinfo *ca;

  ಪ್ರತಿ(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

ಸಂರಚನಾ context {
  ಸಂರಚನಾ ares_addrinfo *addr;
  ಪೂರ್ಣಾಙ್ಕ status;
};

ಸ್ಥಿರ ಶೂನ್ಯ async_addrinfo_cb(ಶೂನ್ಯ *userp, ಪೂರ್ಣಾಙ್ಕ status, ಪೂರ್ಣಾಙ್ಕ timeouts,
                              ಸಂರಚನಾ ares_addrinfo *addr)
{
  ಸಂರಚನಾ context *ctx = (ಸಂರಚನಾ context *)userp;
  (ಶೂನ್ಯ)timeouts;
  ctx->status = status;
  ಯದಿ(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
ಸ್ಥಿರ ಸಂರಚನಾ addrinfo *mk_getaddrinfo(ಧ್ರುವ ಸಂರಚನಾ ares_addrinfo *aihead)
{
  ಧ್ರುವ ಸಂರಚನಾ ares_addrinfo_ಗಂಟು *ai;
  ಸಂರಚನಾ addrinfo *ca;
  ಸಂರಚನಾ addrinfo *cafirst = ರಿಕ್ತ;
  ಸಂರಚನಾ addrinfo *calast = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *name = aihead->name;

  /* traverse the addrinfo list */
  ಪ್ರತಿ(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ಯದಿ(ai->ai_family == AF_INET)
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
    ಅನ್ಯಥಾ ಯದಿ(ai->ai_family == AF_INET6)
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
    ಅನ್ಯಥಾ
      ಅನುವರ್ತ;

    /* ignore elements without required address info */
    ಯದಿ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ಅನುವರ್ತ;

    /* ignore elements with bogus address size */
    ಯದಿ((size_t)ai->ai_addrlen < ss_size)
      ಅನುವರ್ತ;

    ca = curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ addrinfo) + ss_size + namelen);
    ಯದಿ(!ca) {
      r_freeaddrinfo(cafirst);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ರಿಕ್ತ;
    ca->ai_canonname = ರಿಕ್ತ;
    ca->ai_next      = ರಿಕ್ತ;

    ca->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ca + ಪರಿಮಾಣ(ಸಂರಚನಾ addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    ಯದಿ(namelen) {
      ca->ai_canonname = (ಶೂನ್ಯ *)((ವರ್ಣ *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = ರಿಕ್ತ;
    }

    /* if the return list is empty, this becomes the first element */
    ಯದಿ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ಯದಿ(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  ಪ್ರತ್ಯಾಗಮ cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
ಪೂರ್ಣಾಙ್ಕ r_getaddrinfo(ಧ್ರುವ ವರ್ಣ *ಗಂಟು,
                  ಧ್ರುವ ವರ್ಣ *service,
                  ಧ್ರುವ ಸಂರಚನಾ addrinfo *hints,
                  ಸಂರಚನಾ addrinfo **res)
{
  ಪೂರ್ಣಾಙ್ಕ status;
  ಸಂರಚನಾ context ctx;
  ಸಂರಚನಾ ares_options options;
  ಪೂರ್ಣಾಙ್ಕ optmask = 0;
  ಸಂರಚನಾ ares_addrinfo_hints ahints;
  ares_channel channel;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;

  memset(&options, 0, ಪರಿಮಾಣ(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, ಪರಿಮಾಣ(ahints));
  memset(&ctx, 0, ಪರಿಮಾಣ(ctx));

  ಯದಿ(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  ಯದಿ(status)
    ಪ್ರತ್ಯಾಗಮ EAI_MEMORY; /* major problem */

  ಅನ್ಯಥಾ {
    ಧ್ರುವ ವರ್ಣ *env = getenv("CURL_DNS_SERVER");
    ಯದಿ(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      ಯದಿ(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        ಪ್ರತ್ಯಾಗಮ EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, ಗಂಟು, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  ಯದಿ(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  ಅನ್ಯಥಾ ಯದಿ((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  ಅನ್ಯಥಾ
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  ಪ್ರತ್ಯಾಗಮ rc;
}

#endif /* USE_FAKE_GETADDRINFO */

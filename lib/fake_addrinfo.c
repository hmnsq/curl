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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "fake_addrinfo.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FAKE_GETADDRINFO

#ਅਨ੍ਤਰ੍ਭਾਵ <ares.h>

ਖਾਲੀ r_freeaddrinfo(ਬਣਤਰ addrinfo *cahead)
{
  ਬਣਤਰ addrinfo *canext;
  ਬਣਤਰ addrinfo *ca;

  ਲਈ(ca = cahead; ca; ca = canext) {
    canext = ca->ai_ਅਗਲਾ;
    curlx_free(ca);
  }
}

ਬਣਤਰ context {
  ਬਣਤਰ ares_addrinfo *addr;
  ਪੂਰਨ ਸਥਿਤੀ;
};

ਸਥਿਰ ਖਾਲੀ async_addrinfo_cb(ਖਾਲੀ *userp, ਪੂਰਨ ਸਥਿਤੀ, ਪੂਰਨ timeouts,
                              ਬਣਤਰ ares_addrinfo *addr)
{
  ਬਣਤਰ context *ctx = (ਬਣਤਰ context *)userp;
  (ਖਾਲੀ)timeouts;
  ctx->ਸਥਿਤੀ = ਸਥਿਤੀ;
  ਜੇ(ARES_SUCCESS == ਸਥਿਤੀ) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
ਸਥਿਰ ਬਣਤਰ addrinfo *mk_getaddrinfo(ਅਚਲ ਬਣਤਰ ares_addrinfo *aihead)
{
  ਅਚਲ ਬਣਤਰ ares_addrinfo_ਗੰਢ *ai;
  ਬਣਤਰ addrinfo *ca;
  ਬਣਤਰ addrinfo *cafirst = ਨਿਰਾ;
  ਬਣਤਰ addrinfo *calast = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *name = aihead->name;

  /* traverse the addrinfo list */
  ਲਈ(ai = aihead->nodes; ai; ai = ai->ai_ਅਗਲਾ) {
    size_t ss_ਮਾਪ;
    size_t namelen = name ? ਲੜੀ_ਲੰਬਾਈ(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ਜੇ(ai->ai_family == AF_INET)
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
    ਨਹੀਂਤਾਂ ਜੇ(ai->ai_family == AF_INET6)
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
    ਨਹੀਂਤਾਂ
      ਜਾਰੀ;

    /* ignore elements without required address info */
    ਜੇ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ਜਾਰੀ;

    /* ignore elements with bogus address size */
    ਜੇ((size_t)ai->ai_addrlen < ss_ਮਾਪ)
      ਜਾਰੀ;

    ca = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ addrinfo) + ss_ਮਾਪ + namelen);
    ਜੇ(!ca) {
      r_freeaddrinfo(cafirst);
      ਵਾਪਸ ਨਿਰਾ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_ਮਾਪ;
    ca->ai_addr      = ਨਿਰਾ;
    ca->ai_canonname = ਨਿਰਾ;
    ca->ai_ਅਗਲਾ      = ਨਿਰਾ;

    ca->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ca + ਆਕਾਰ(ਬਣਤਰ addrinfo));
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ca->ai_addr, ai->ai_addr, ss_ਮਾਪ);

    ਜੇ(namelen) {
      ca->ai_canonname = (ਖਾਲੀ *)((ਅੱਖਰ *)ca->ai_addr + ss_ਮਾਪ);
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = ਨਿਰਾ;
    }

    /* if the return list is empty, this becomes the first element */
    ਜੇ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ਜੇ(calast)
      calast->ai_ਅਗਲਾ = ca;
    calast = ca;
  }

  ਵਾਪਸ cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
ਪੂਰਨ r_getaddrinfo(ਅਚਲ ਅੱਖਰ *ਗੰਢ,
                  ਅਚਲ ਅੱਖਰ *service,
                  ਅਚਲ ਬਣਤਰ addrinfo *hints,
                  ਬਣਤਰ addrinfo **res)
{
  ਪੂਰਨ ਸਥਿਤੀ;
  ਬਣਤਰ context ctx;
  ਬਣਤਰ ares_options options;
  ਪੂਰਨ optmask = 0;
  ਬਣਤਰ ares_addrinfo_hints ahints;
  ares_channel channel;
  ਪੂਰਨ rc = 0;

  memset(&options, 0, ਆਕਾਰ(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, ਆਕਾਰ(ahints));
  memset(&ctx, 0, ਆਕਾਰ(ctx));

  ਜੇ(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  ਸਥਿਤੀ = ares_init_options(&channel, &options, optmask);
  ਜੇ(ਸਥਿਤੀ)
    ਵਾਪਸ EAI_MEMORY; /* major problem */

  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਅੱਖਰ *env = getenv("CURL_DNS_SERVER");
    ਜੇ(env) {
      rc = ares_ਸੈੱਟ_servers_ports_csv(channel, env);
      ਜੇ(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        ਵਾਪਸ EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, ਗੰਢ, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_ਕਤਾਰ_wait_ਖਾਲੀ_ਥਾਵਾਂ(channel, -1);

  ਜੇ(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  ਨਹੀਂਤਾਂ ਜੇ((ctx.ਸਥਿਤੀ == ARES_ENOTFOUND) || (ctx.ਸਥਿਤੀ == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  ਨਹੀਂਤਾਂ
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  ਵਾਪਸ rc;
}

#ਜੇ_ਅੰਤ /* USE_FAKE_GETADDRINFO */

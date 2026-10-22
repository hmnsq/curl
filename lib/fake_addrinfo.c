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
#ضمن "curl_setup.h"

#ضمن "fake_addrinfo.h"

#جے_تعریف_شدہ USE_FAKE_GETADDRINFO

#ضمن <ares.h>

فراغ r_freeaddrinfo(بنية addrinfo *cahead)
{
  بنية addrinfo *canext;
  بنية addrinfo *ca;

  لكل(ca = cahead; ca; ca = canext) {
    canext = ca->ai_اگلا;
    curlx_free(ca);
  }
}

بنية context {
  بنية ares_addrinfo *addr;
  صحيح حالت;
};

ساكن فراغ async_addrinfo_cb(فراغ *userp, صحيح حالت, صحيح timeouts,
                              بنية ares_addrinfo *addr)
{
  بنية context *ctx = (بنية context *)userp;
  (فراغ)timeouts;
  ctx->حالت = حالت;
  إذا(ARES_SUCCESS == حالت) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
ساكن بنية addrinfo *mk_getaddrinfo(ثابت بنية ares_addrinfo *aihead)
{
  ثابت بنية ares_addrinfo_گرہ *ai;
  بنية addrinfo *ca;
  بنية addrinfo *cafirst = عدم;
  بنية addrinfo *calast = عدم;
  ثابت حرف *name = aihead->name;

  /* traverse the addrinfo list */
  لكل(ai = aihead->nodes; ai; ai = ai->ai_اگلا) {
    size_t ss_ناپ;
    size_t namelen = name ? لڑی_لمبائی(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    إذا(ai->ai_family == AF_INET)
      ss_ناپ = حجم(بنية sockaddr_in);
    وإلا إذا(ai->ai_family == AF_INET6)
      ss_ناپ = حجم(بنية sockaddr_in6);
    وإلا
      استمر;

    /* ignore elements without required address info */
    إذا(!ai->ai_addr || !(ai->ai_addrlen > 0))
      استمر;

    /* ignore elements with bogus address size */
    إذا((size_t)ai->ai_addrlen < ss_ناپ)
      استمر;

    ca = curlx_malloc(حجم(بنية addrinfo) + ss_ناپ + namelen);
    إذا(!ca) {
      r_freeaddrinfo(cafirst);
      أرجع عدم;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_ناپ;
    ca->ai_addr      = عدم;
    ca->ai_canonname = عدم;
    ca->ai_اگلا      = عدم;

    ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية addrinfo));
    حافظہ_نقل(ca->ai_addr, ai->ai_addr, ss_ناپ);

    إذا(namelen) {
      ca->ai_canonname = (فراغ *)((حرف *)ca->ai_addr + ss_ناپ);
      حافظہ_نقل(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = عدم;
    }

    /* if the return list is empty, this becomes the first element */
    إذا(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    إذا(calast)
      calast->ai_اگلا = ca;
    calast = ca;
  }

  أرجع cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
صحيح r_getaddrinfo(ثابت حرف *گرہ,
                  ثابت حرف *service,
                  ثابت بنية addrinfo *hints,
                  بنية addrinfo **res)
{
  صحيح حالت;
  بنية context ctx;
  بنية ares_options options;
  صحيح optmask = 0;
  بنية ares_addrinfo_hints ahints;
  ares_channel channel;
  صحيح rc = 0;

  memset(&options, 0, حجم(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, حجم(ahints));
  memset(&ctx, 0, حجم(ctx));

  إذا(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  حالت = ares_init_options(&channel, &options, optmask);
  إذا(حالت)
    أرجع EAI_MEMORY; /* major problem */

  وإلا {
    ثابت حرف *env = getenv("CURL_DNS_SERVER");
    إذا(env) {
      rc = ares_سیٹ_servers_ports_csv(channel, env);
      إذا(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        أرجع EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, گرہ, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_قطار_wait_خالی_تھاواں(channel, -1);

  إذا(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  وإلا إذا((ctx.حالت == ARES_ENOTFOUND) || (ctx.حالت == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  وإلا
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  أرجع rc;
}

#جے_انت /* USE_FAKE_GETADDRINFO */

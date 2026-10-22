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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "fake_addrinfo.h"

#ifdef USE_FAKE_GETADDRINFO

#অন্তর্ভাব <ares.h>

শূন্য r_freeaddrinfo(কাঠামো addrinfo *cahead)
{
  কাঠামো addrinfo *canext;
  কাঠামো addrinfo *ca;

  জন্য(ca = cahead; ca; ca = canext) {
    canext = ca->ai_পরবর্তী;
    curlx_free(ca);
  }
}

কাঠামো context {
  কাঠামো ares_addrinfo *addr;
  পূর্ণসংখ্যা status;
};

স্থির শূন্য async_addrinfo_cb(শূন্য *userp, পূর্ণসংখ্যা status, পূর্ণসংখ্যা timeouts,
                              কাঠামো ares_addrinfo *addr)
{
  কাঠামো context *ctx = (কাঠামো context *)userp;
  (শূন্য)timeouts;
  ctx->status = status;
  যদি(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
স্থির কাঠামো addrinfo *mk_getaddrinfo(ধ্রুবক কাঠামো ares_addrinfo *aihead)
{
  ধ্রুবক কাঠামো ares_addrinfo_গ্রন্থি *ai;
  কাঠামো addrinfo *ca;
  কাঠামো addrinfo *cafirst = ফাঁকা;
  কাঠামো addrinfo *calast = ফাঁকা;
  ধ্রুবক অক্ষর *name = aihead->name;

  /* traverse the addrinfo list */
  জন্য(ai = aihead->nodes; ai; ai = ai->ai_পরবর্তী) {
    size_t ss_আকার;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    যদি(ai->ai_family == AF_INET)
      ss_আকার = আকার(কাঠামো sockaddr_in);
    অন্যথা যদি(ai->ai_family == AF_INET6)
      ss_আকার = আকার(কাঠামো sockaddr_in6);
    অন্যথা
      চালু;

    /* ignore elements without required address info */
    যদি(!ai->ai_addr || !(ai->ai_addrlen > 0))
      চালু;

    /* ignore elements with bogus address size */
    যদি((size_t)ai->ai_addrlen < ss_আকার)
      চালু;

    ca = curlx_malloc(আকার(কাঠামো addrinfo) + ss_আকার + namelen);
    যদি(!ca) {
      r_freeaddrinfo(cafirst);
      ফেরত ফাঁকা;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_আকার;
    ca->ai_addr      = ফাঁকা;
    ca->ai_canonname = ফাঁকা;
    ca->ai_পরবর্তী      = ফাঁকা;

    ca->ai_addr = (শূন্য *)((অক্ষর *)ca + আকার(কাঠামো addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_আকার);

    যদি(namelen) {
      ca->ai_canonname = (শূন্য *)((অক্ষর *)ca->ai_addr + ss_আকার);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = ফাঁকা;
    }

    /* if the return list is empty, this becomes the first element */
    যদি(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    যদি(calast)
      calast->ai_পরবর্তী = ca;
    calast = ca;
  }

  ফেরত cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
পূর্ণসংখ্যা r_getaddrinfo(ধ্রুবক অক্ষর *গ্রন্থি,
                  ধ্রুবক অক্ষর *service,
                  ধ্রুবক কাঠামো addrinfo *hints,
                  কাঠামো addrinfo **res)
{
  পূর্ণসংখ্যা status;
  কাঠামো context ctx;
  কাঠামো ares_options options;
  পূর্ণসংখ্যা optmask = 0;
  কাঠামো ares_addrinfo_hints ahints;
  ares_channel channel;
  পূর্ণসংখ্যা rc = 0;

  memset(&options, 0, আকার(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, আকার(ahints));
  memset(&ctx, 0, আকার(ctx));

  যদি(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  যদি(status)
    ফেরত EAI_MEMORY; /* major problem */

  অন্যথা {
    ধ্রুবক অক্ষর *env = getenv("CURL_DNS_SERVER");
    যদি(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      যদি(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        ফেরত EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, গ্রন্থি, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  যদি(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  অন্যথা যদি((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  অন্যথা
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  ফেরত rc;
}

#endif /* USE_FAKE_GETADDRINFO */

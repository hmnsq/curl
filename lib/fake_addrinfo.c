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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "fake_addrinfo.h"

#ifdef USE_FAKE_GETADDRINFO

#অন্তর্ভাৱ <ares.h>

শূন্য r_freeaddrinfo(সংরচনা addrinfo *cahead)
{
  সংরচনা addrinfo *canext;
  সংরচনা addrinfo *ca;

  বাবে(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

সংরচনা context {
  সংরচনা ares_addrinfo *addr;
  পূর্ণাঙ্ক status;
};

স্থির শূন্য async_addrinfo_cb(শূন্য *userp, পূর্ণাঙ্ক status, পূর্ণাঙ্ক timeouts,
                              সংরচনা ares_addrinfo *addr)
{
  সংরচনা context *ctx = (সংরচনা context *)userp;
  (শূন্য)timeouts;
  ctx->status = status;
  যদি(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
স্থির সংরচনা addrinfo *mk_getaddrinfo(ধ্রুৱ সংরচনা ares_addrinfo *aihead)
{
  ধ্রুৱ সংরচনা ares_addrinfo_গ্ৰন্থি *ai;
  সংরচনা addrinfo *ca;
  সংরচনা addrinfo *cafirst = রিক্ত;
  সংরচনা addrinfo *calast = রিক্ত;
  ধ্রুৱ ৱর্ণ *name = aihead->name;

  /* traverse the addrinfo list */
  বাবে(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    যদি(ai->ai_family == AF_INET)
      ss_size = পরিমাণ(সংরচনা sockaddr_in);
    অন্যথা যদি(ai->ai_family == AF_INET6)
      ss_size = পরিমাণ(সংরচনা sockaddr_in6);
    অন্যথা
      অনুৱর্ত;

    /* ignore elements without required address info */
    যদি(!ai->ai_addr || !(ai->ai_addrlen > 0))
      অনুৱর্ত;

    /* ignore elements with bogus address size */
    যদি((size_t)ai->ai_addrlen < ss_size)
      অনুৱর্ত;

    ca = curlx_malloc(পরিমাণ(সংরচনা addrinfo) + ss_size + namelen);
    যদি(!ca) {
      r_freeaddrinfo(cafirst);
      ঘূৰাই রিক্ত;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = রিক্ত;
    ca->ai_canonname = রিক্ত;
    ca->ai_next      = রিক্ত;

    ca->ai_addr = (শূন্য *)((ৱর্ণ *)ca + পরিমাণ(সংরচনা addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    যদি(namelen) {
      ca->ai_canonname = (শূন্য *)((ৱর্ণ *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = রিক্ত;
    }

    /* if the return list is empty, this becomes the first element */
    যদি(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    যদি(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  ঘূৰাই cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
পূর্ণাঙ্ক r_getaddrinfo(ধ্রুৱ ৱর্ণ *গ্ৰন্থি,
                  ধ্রুৱ ৱর্ণ *service,
                  ধ্রুৱ সংরচনা addrinfo *hints,
                  সংরচনা addrinfo **res)
{
  পূর্ণাঙ্ক status;
  সংরচনা context ctx;
  সংরচনা ares_options options;
  পূর্ণাঙ্ক optmask = 0;
  সংরচনা ares_addrinfo_hints ahints;
  ares_channel channel;
  পূর্ণাঙ্ক rc = 0;

  memset(&options, 0, পরিমাণ(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, পরিমাণ(ahints));
  memset(&ctx, 0, পরিমাণ(ctx));

  যদি(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  যদি(status)
    ঘূৰাই EAI_MEMORY; /* major problem */

  অন্যথা {
    ধ্রুৱ ৱর্ণ *env = getenv("CURL_DNS_SERVER");
    যদি(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      যদি(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        ঘূৰাই EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, গ্ৰন্থি, service, &ahints, async_addrinfo_cb, &ctx);

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

  ঘূৰাই rc;
}

#endif /* USE_FAKE_GETADDRINFO */

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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "fake_addrinfo.h"

#ifdef USE_FAKE_GETADDRINFO

#ଅନ୍ତର୍ଭାଵ <ares.h>

ଶୂନ୍ୟ r_freeaddrinfo(ସଂରଚନା addrinfo *cahead)
{
  ସଂରଚନା addrinfo *canext;
  ସଂରଚନା addrinfo *ca;

  ପାଇଁ(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

ସଂରଚନା context {
  ସଂରଚନା ares_addrinfo *addr;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;
};

ସ୍ଥିର ଶୂନ୍ୟ async_addrinfo_cb(ଶୂନ୍ୟ *userp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeouts,
                              ସଂରଚନା ares_addrinfo *addr)
{
  ସଂରଚନା context *ctx = (ସଂରଚନା context *)userp;
  (ଶୂନ୍ୟ)timeouts;
  ctx->status = status;
  ଯଦି(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
ସ୍ଥିର ସଂରଚନା addrinfo *mk_getaddrinfo(ଅଚଳ ସଂରଚନା ares_addrinfo *aihead)
{
  ଅଚଳ ସଂରଚନା ares_addrinfo_ଗଣ୍ଠି *ai;
  ସଂରଚନା addrinfo *ca;
  ସଂରଚନା addrinfo *cafirst = ଖାଲି;
  ସଂରଚନା addrinfo *calast = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *name = aihead->name;

  /* traverse the addrinfo list */
  ପାଇଁ(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ଯଦି(ai->ai_family == AF_INET)
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in);
    ଅନ୍ୟଥା ଯଦି(ai->ai_family == AF_INET6)
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in6);
    ଅନ୍ୟଥା
      ଜାରି;

    /* ignore elements without required address info */
    ଯଦି(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ଜାରି;

    /* ignore elements with bogus address size */
    ଯଦି((size_t)ai->ai_addrlen < ss_size)
      ଜାରି;

    ca = curlx_malloc(ଆକାର(ସଂରଚନା addrinfo) + ss_size + namelen);
    ଯଦି(!ca) {
      r_freeaddrinfo(cafirst);
      ଫେରନ୍ତୁ ଖାଲି;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ଖାଲି;
    ca->ai_canonname = ଖାଲି;
    ca->ai_next      = ଖାଲି;

    ca->ai_addr = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ca + ଆକାର(ସଂରଚନା addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    ଯଦି(namelen) {
      ca->ai_canonname = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = ଖାଲି;
    }

    /* if the return list is empty, this becomes the first element */
    ଯଦି(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ଯଦି(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  ଫେରନ୍ତୁ cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା r_getaddrinfo(ଅଚଳ ଅକ୍ଷର *ଗଣ୍ଠି,
                  ଅଚଳ ଅକ୍ଷର *service,
                  ଅଚଳ ସଂରଚନା addrinfo *hints,
                  ସଂରଚନା addrinfo **res)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;
  ସଂରଚନା context ctx;
  ସଂରଚନା ares_options options;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା optmask = 0;
  ସଂରଚନା ares_addrinfo_hints ahints;
  ares_channel channel;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

  memset(&options, 0, ଆକାର(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, ଆକାର(ahints));
  memset(&ctx, 0, ଆକାର(ctx));

  ଯଦି(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  ଯଦି(status)
    ଫେରନ୍ତୁ EAI_MEMORY; /* major problem */

  ଅନ୍ୟଥା {
    ଅଚଳ ଅକ୍ଷର *env = getenv("CURL_DNS_SERVER");
    ଯଦି(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      ଯଦି(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        ଫେରନ୍ତୁ EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, ଗଣ୍ଠି, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  ଯଦି(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  ଅନ୍ୟଥା ଯଦି((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  ଅନ୍ୟଥା
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  ଫେରନ୍ତୁ rc;
}

#endif /* USE_FAKE_GETADDRINFO */

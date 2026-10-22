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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "fake_addrinfo.h"

#ifdef USE_FAKE_GETADDRINFO

#அந்தர்பாவ <ares.h>

வெறுமை r_freeaddrinfo(கட்டமைப்பு addrinfo *cahead)
{
  கட்டமைப்பு addrinfo *canext;
  கட்டமைப்பு addrinfo *ca;

  க்கு(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

கட்டமைப்பு context {
  கட்டமைப்பு ares_addrinfo *addr;
  முழு status;
};

நிலையான வெறுமை async_addrinfo_cb(வெறுமை *userp, முழு status, முழு timeouts,
                              கட்டமைப்பு ares_addrinfo *addr)
{
  கட்டமைப்பு context *ctx = (கட்டமைப்பு context *)userp;
  (வெறுமை)timeouts;
  ctx->status = status;
  எனில்(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
நிலையான கட்டமைப்பு addrinfo *mk_getaddrinfo(மாறிலி கட்டமைப்பு ares_addrinfo *aihead)
{
  மாறிலி கட்டமைப்பு ares_addrinfo_முனை *ai;
  கட்டமைப்பு addrinfo *ca;
  கட்டமைப்பு addrinfo *cafirst = வெற்று;
  கட்டமைப்பு addrinfo *calast = வெற்று;
  மாறிலி எழுத்து *name = aihead->name;

  /* traverse the addrinfo list */
  க்கு(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    எனில்(ai->ai_family == AF_INET)
      ss_size = அளவு(கட்டமைப்பு sockaddr_in);
    இல்லையெனில் எனில்(ai->ai_family == AF_INET6)
      ss_size = அளவு(கட்டமைப்பு sockaddr_in6);
    இல்லையெனில்
      தொடர்;

    /* ignore elements without required address info */
    எனில்(!ai->ai_addr || !(ai->ai_addrlen > 0))
      தொடர்;

    /* ignore elements with bogus address size */
    எனில்((size_t)ai->ai_addrlen < ss_size)
      தொடர்;

    ca = curlx_malloc(அளவு(கட்டமைப்பு addrinfo) + ss_size + namelen);
    எனில்(!ca) {
      r_freeaddrinfo(cafirst);
      திரும்பு வெற்று;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = வெற்று;
    ca->ai_canonname = வெற்று;
    ca->ai_next      = வெற்று;

    ca->ai_addr = (வெறுமை *)((எழுத்து *)ca + அளவு(கட்டமைப்பு addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    எனில்(namelen) {
      ca->ai_canonname = (வெறுமை *)((எழுத்து *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = வெற்று;
    }

    /* if the return list is empty, this becomes the first element */
    எனில்(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    எனில்(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  திரும்பு cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
முழு r_getaddrinfo(மாறிலி எழுத்து *முனை,
                  மாறிலி எழுத்து *service,
                  மாறிலி கட்டமைப்பு addrinfo *hints,
                  கட்டமைப்பு addrinfo **res)
{
  முழு status;
  கட்டமைப்பு context ctx;
  கட்டமைப்பு ares_options options;
  முழு optmask = 0;
  கட்டமைப்பு ares_addrinfo_hints ahints;
  ares_channel channel;
  முழு rc = 0;

  memset(&options, 0, அளவு(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, அளவு(ahints));
  memset(&ctx, 0, அளவு(ctx));

  எனில்(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  எனில்(status)
    திரும்பு EAI_MEMORY; /* major problem */

  இல்லையெனில் {
    மாறிலி எழுத்து *env = getenv("CURL_DNS_SERVER");
    எனில்(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      எனில்(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        திரும்பு EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, முனை, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  எனில்(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  இல்லையெனில் எனில்((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  இல்லையெனில்
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  திரும்பு rc;
}

#endif /* USE_FAKE_GETADDRINFO */

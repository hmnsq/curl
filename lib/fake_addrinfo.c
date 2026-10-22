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

#شامل "fake_addrinfo.h"

#اگر_تعریف_شدہ USE_FAKE_GETADDRINFO

#شامل <ares.h>

خالی r_freeaddrinfo(ساخت addrinfo *cahead)
{
  ساخت addrinfo *canext;
  ساخت addrinfo *ca;

  برائے(ca = cahead; ca; ca = canext) {
    canext = ca->ai_اگلا;
    curlx_free(ca);
  }
}

ساخت context {
  ساخت ares_addrinfo *addr;
  صحیح حالت_2;
};

ساکن خالی async_addrinfo_cb(خالی *userp, صحیح حالت_2, صحیح timeouts,
                              ساخت ares_addrinfo *addr)
{
  ساخت context *ctx = (ساخت context *)userp;
  (خالی)timeouts;
  ctx->حالت_2 = حالت_2;
  اگر(ARES_SUCCESS == حالت_2) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
ساکن ساخت addrinfo *mk_getaddrinfo(ثابت ساخت ares_addrinfo *aihead)
{
  ثابت ساخت ares_addrinfo_گرہ *ai;
  ساخت addrinfo *ca;
  ساخت addrinfo *cafirst = تہی;
  ساخت addrinfo *calast = تہی;
  ثابت حرف *name = aihead->name;

  /* traverse the addrinfo list */
  برائے(ai = aihead->nodes; ai; ai = ai->ai_اگلا) {
    size_t ss_سائز;
    size_t namelen = name ? لڑی_لمبائی(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    اگر(ai->ai_family == AF_INET)
      ss_سائز = حجم(ساخت sockaddr_in);
    ورنہ اگر(ai->ai_family == AF_INET6)
      ss_سائز = حجم(ساخت sockaddr_in6);
    ورنہ
      جاری;

    /* ignore elements without required address info */
    اگر(!ai->ai_addr || !(ai->ai_addrlen > 0))
      جاری;

    /* ignore elements with bogus address size */
    اگر((size_t)ai->ai_addrlen < ss_سائز)
      جاری;

    ca = curlx_malloc(حجم(ساخت addrinfo) + ss_سائز + namelen);
    اگر(!ca) {
      r_freeaddrinfo(cafirst);
      واپس تہی;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_سائز;
    ca->ai_addr      = تہی;
    ca->ai_canonname = تہی;
    ca->ai_اگلا      = تہی;

    ca->ai_addr = (خالی *)((حرف *)ca + حجم(ساخت addrinfo));
    حافظہ_نقل(ca->ai_addr, ai->ai_addr, ss_سائز);

    اگر(namelen) {
      ca->ai_canonname = (خالی *)((حرف *)ca->ai_addr + ss_سائز);
      حافظہ_نقل(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = تہی;
    }

    /* if the return list is empty, this becomes the first element */
    اگر(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    اگر(calast)
      calast->ai_اگلا = ca;
    calast = ca;
  }

  واپس cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
صحیح r_getaddrinfo(ثابت حرف *گرہ,
                  ثابت حرف *service,
                  ثابت ساخت addrinfo *hints,
                  ساخت addrinfo **res)
{
  صحیح حالت_2;
  ساخت context ctx;
  ساخت ares_options options;
  صحیح optmask = 0;
  ساخت ares_addrinfo_hints ahints;
  ares_channel channel;
  صحیح rc = 0;

  memset(&options, 0, حجم(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, حجم(ahints));
  memset(&ctx, 0, حجم(ctx));

  اگر(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  حالت_2 = ares_init_options(&channel, &options, optmask);
  اگر(حالت_2)
    واپس EAI_MEMORY; /* major problem */

  ورنہ {
    ثابت حرف *env = getenv("CURL_DNS_SERVER");
    اگر(env) {
      rc = ares_سیٹ_servers_ports_csv(channel, env);
      اگر(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        واپس EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, گرہ, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_قطار_wait_خالی_خانے(channel, -1);

  اگر(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  ورنہ اگر((ctx.حالت_2 == ARES_ENOTFOUND) || (ctx.حالت_2 == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  ورنہ
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  واپس rc;
}

#اگر_ختم /* USE_FAKE_GETADDRINFO */

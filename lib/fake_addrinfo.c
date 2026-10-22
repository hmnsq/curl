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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "fake_addrinfo.h"

#నిర్వచితమైతే USE_FAKE_GETADDRINFO

#అన్తర్భావ <ares.h>

శూన్య r_freeaddrinfo(నిర్మాణం addrinfo *cahead)
{
  నిర్మాణం addrinfo *canext;
  నిర్మాణం addrinfo *ca;

  కొరకు(ca = cahead; ca; ca = canext) {
    canext = ca->ai_తదుపరి;
    curlx_free(ca);
  }
}

నిర్మాణం context {
  నిర్మాణం ares_addrinfo *addr;
  పూర్ణాంకం స్థితి_2;
};

స్థిర శూన్య async_addrinfo_cb(శూన్య *userp, పూర్ణాంకం స్థితి_2, పూర్ణాంకం timeouts,
                              నిర్మాణం ares_addrinfo *addr)
{
  నిర్మాణం context *ctx = (నిర్మాణం context *)userp;
  (శూన్య)timeouts;
  ctx->స్థితి_2 = స్థితి_2;
  ఒకవేళ(ARES_SUCCESS == స్థితి_2) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
స్థిర నిర్మాణం addrinfo *mk_getaddrinfo(స్థిరరాశి నిర్మాణం ares_addrinfo *aihead)
{
  స్థిరరాశి నిర్మాణం ares_addrinfo_కణుపు *ai;
  నిర్మాణం addrinfo *ca;
  నిర్మాణం addrinfo *cafirst = ఖాళీ;
  నిర్మాణం addrinfo *calast = ఖాళీ;
  స్థిరరాశి అక్షరం *name = aihead->name;

  /* traverse the addrinfo list */
  కొరకు(ai = aihead->nodes; ai; ai = ai->ai_తదుపరి) {
    size_t ss_కొలత;
    size_t namelen = name ? అక్షరశ్రేణి_పొడవు(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ఒకవేళ(ai->ai_family == AF_INET)
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in);
    లేకపోతే ఒకవేళ(ai->ai_family == AF_INET6)
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in6);
    లేకపోతే
      కొనసాగు;

    /* ignore elements without required address info */
    ఒకవేళ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      కొనసాగు;

    /* ignore elements with bogus address size */
    ఒకవేళ((size_t)ai->ai_addrlen < ss_కొలత)
      కొనసాగు;

    ca = curlx_malloc(పరిమాణం(నిర్మాణం addrinfo) + ss_కొలత + namelen);
    ఒకవేళ(!ca) {
      r_freeaddrinfo(cafirst);
      తిరిగి ఖాళీ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_కొలత;
    ca->ai_addr      = ఖాళీ;
    ca->ai_canonname = ఖాళీ;
    ca->ai_తదుపరి      = ఖాళీ;

    ca->ai_addr = (శూన్య *)((అక్షరం *)ca + పరిమాణం(నిర్మాణం addrinfo));
    జ్ఞాపకం_నకలు(ca->ai_addr, ai->ai_addr, ss_కొలత);

    ఒకవేళ(namelen) {
      ca->ai_canonname = (శూన్య *)((అక్షరం *)ca->ai_addr + ss_కొలత);
      జ్ఞాపకం_నకలు(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = ఖాళీ;
    }

    /* if the return list is empty, this becomes the first element */
    ఒకవేళ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ఒకవేళ(calast)
      calast->ai_తదుపరి = ca;
    calast = ca;
  }

  తిరిగి cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
పూర్ణాంకం r_getaddrinfo(స్థిరరాశి అక్షరం *కణుపు,
                  స్థిరరాశి అక్షరం *service,
                  స్థిరరాశి నిర్మాణం addrinfo *hints,
                  నిర్మాణం addrinfo **res)
{
  పూర్ణాంకం స్థితి_2;
  నిర్మాణం context ctx;
  నిర్మాణం ares_options options;
  పూర్ణాంకం optmask = 0;
  నిర్మాణం ares_addrinfo_hints ahints;
  ares_channel channel;
  పూర్ణాంకం rc = 0;

  memset(&options, 0, పరిమాణం(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, పరిమాణం(ahints));
  memset(&ctx, 0, పరిమాణం(ctx));

  ఒకవేళ(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  స్థితి_2 = ares_init_options(&channel, &options, optmask);
  ఒకవేళ(స్థితి_2)
    తిరిగి EAI_MEMORY; /* major problem */

  లేకపోతే {
    స్థిరరాశి అక్షరం *env = getenv("CURL_DNS_SERVER");
    ఒకవేళ(env) {
      rc = ares_సమితి_servers_ports_csv(channel, env);
      ఒకవేళ(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        తిరిగి EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, కణుపు, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_వరుస_wait_ఖాళీలు(channel, -1);

  ఒకవేళ(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  లేకపోతే ఒకవేళ((ctx.స్థితి_2 == ARES_ENOTFOUND) || (ctx.స్థితి_2 == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  లేకపోతే
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  తిరిగి rc;
}

#అయితే_ముగింపు /* USE_FAKE_GETADDRINFO */

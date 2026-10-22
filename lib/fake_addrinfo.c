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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "fake_addrinfo.h"

#यदि_परिभाषितम् USE_FAKE_GETADDRINFO

#अन्तर्भाव <ares.h>

शून्य r_freeaddrinfo(संरचना addrinfo *cahead)
{
  संरचना addrinfo *canext;
  संरचना addrinfo *ca;

  प्रति(ca = cahead; ca; ca = canext) {
    canext = ca->ai_अग्र;
    curlx_free(ca);
  }
}

संरचना context {
  संरचना ares_addrinfo *addr;
  पूर्णाङ्क स्थितिः;
};

स्थिर शून्य async_addrinfo_cb(शून्य *userp, पूर्णाङ्क स्थितिः, पूर्णाङ्क timeouts,
                              संरचना ares_addrinfo *addr)
{
  संरचना context *ctx = (संरचना context *)userp;
  (शून्य)timeouts;
  ctx->स्थितिः = स्थितिः;
  यदि(ARES_SUCCESS == स्थितिः) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
स्थिर संरचना addrinfo *mk_getaddrinfo(ध्रुव संरचना ares_addrinfo *aihead)
{
  ध्रुव संरचना ares_addrinfo_ग्रन्थिः *ai;
  संरचना addrinfo *ca;
  संरचना addrinfo *cafirst = रिक्त;
  संरचना addrinfo *calast = रिक्त;
  ध्रुव वर्ण *name = aihead->name;

  /* traverse the addrinfo list */
  प्रति(ai = aihead->nodes; ai; ai = ai->ai_अग्र) {
    size_t ss_आकार;
    size_t namelen = name ? वाक्यदैर्घ्यम्(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    यदि(ai->ai_family == AF_INET)
      ss_आकार = परिमाण(संरचना sockaddr_in);
    अन्यथा यदि(ai->ai_family == AF_INET6)
      ss_आकार = परिमाण(संरचना sockaddr_in6);
    अन्यथा
      अनुवर्त;

    /* ignore elements without required address info */
    यदि(!ai->ai_addr || !(ai->ai_addrlen > 0))
      अनुवर्त;

    /* ignore elements with bogus address size */
    यदि((size_t)ai->ai_addrlen < ss_आकार)
      अनुवर्त;

    ca = curlx_malloc(परिमाण(संरचना addrinfo) + ss_आकार + namelen);
    यदि(!ca) {
      r_freeaddrinfo(cafirst);
      प्रत्यागम रिक्त;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
    ca->ai_addr      = रिक्त;
    ca->ai_canonname = रिक्त;
    ca->ai_अग्र      = रिक्त;

    ca->ai_addr = (शून्य *)((वर्ण *)ca + परिमाण(संरचना addrinfo));
    स्मृतिं_प्रतिलिख(ca->ai_addr, ai->ai_addr, ss_आकार);

    यदि(namelen) {
      ca->ai_canonname = (शून्य *)((वर्ण *)ca->ai_addr + ss_आकार);
      स्मृतिं_प्रतिलिख(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = रिक्त;
    }

    /* if the return list is empty, this becomes the first element */
    यदि(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    यदि(calast)
      calast->ai_अग्र = ca;
    calast = ca;
  }

  प्रत्यागम cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
पूर्णाङ्क r_getaddrinfo(ध्रुव वर्ण *ग्रन्थिः,
                  ध्रुव वर्ण *service,
                  ध्रुव संरचना addrinfo *hints,
                  संरचना addrinfo **res)
{
  पूर्णाङ्क स्थितिः;
  संरचना context ctx;
  संरचना ares_options options;
  पूर्णाङ्क optmask = 0;
  संरचना ares_addrinfo_hints ahints;
  ares_channel channel;
  पूर्णाङ्क rc = 0;

  स्मृतिं_पूरय(&options, 0, परिमाण(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  स्मृतिं_पूरय(&ahints, 0, परिमाण(ahints));
  स्मृतिं_पूरय(&ctx, 0, परिमाण(ctx));

  यदि(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  स्थितिः = ares_init_options(&channel, &options, optmask);
  यदि(स्थितिः)
    प्रत्यागम EAI_MEMORY; /* major problem */

  अन्यथा {
    ध्रुव वर्ण *env = getenv("CURL_DNS_SERVER");
    यदि(env) {
      rc = ares_समुच्चयः_servers_ports_csv(channel, env);
      यदि(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        प्रत्यागम EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, ग्रन्थिः, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_रिक्तानि(channel, -1);

  यदि(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  अन्यथा यदि((ctx.स्थितिः == ARES_ENOTFOUND) || (ctx.स्थितिः == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  अन्यथा
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  प्रत्यागम rc;
}

#यदि_अन्तः /* USE_FAKE_GETADDRINFO */

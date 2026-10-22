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

#ifdef USE_FAKE_GETADDRINFO

#अन्तर्भाव <ares.h>

शून्य r_freeaddrinfo(संरचना addrinfo *cahead)
{
  संरचना addrinfo *canext;
  संरचना addrinfo *ca;

  प्रति(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

संरचना context {
  संरचना ares_addrinfo *addr;
  पूर्णाङ्क status;
};

स्थिर शून्य async_addrinfo_cb(शून्य *userp, पूर्णाङ्क status, पूर्णाङ्क timeouts,
                              संरचना ares_addrinfo *addr)
{
  संरचना context *ctx = (संरचना context *)userp;
  (शून्य)timeouts;
  ctx->status = status;
  यदि(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
स्थिर संरचना addrinfo *mk_getaddrinfo(ध्रुव संरचना ares_addrinfo *aihead)
{
  ध्रुव संरचना ares_addrinfo_गाठ *ai;
  संरचना addrinfo *ca;
  संरचना addrinfo *cafirst = रिक्त;
  संरचना addrinfo *calast = रिक्त;
  ध्रुव वर्ण *name = aihead->name;

  /* traverse the addrinfo list */
  प्रति(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    यदि(ai->ai_family == AF_INET)
      ss_size = परिमाण(संरचना sockaddr_in);
    अन्यथा यदि(ai->ai_family == AF_INET6)
      ss_size = परिमाण(संरचना sockaddr_in6);
    अन्यथा
      अनुवर्त;

    /* ignore elements without required address info */
    यदि(!ai->ai_addr || !(ai->ai_addrlen > 0))
      अनुवर्त;

    /* ignore elements with bogus address size */
    यदि((size_t)ai->ai_addrlen < ss_size)
      अनुवर्त;

    ca = curlx_malloc(परिमाण(संरचना addrinfo) + ss_size + namelen);
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
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = रिक्त;
    ca->ai_canonname = रिक्त;
    ca->ai_next      = रिक्त;

    ca->ai_addr = (शून्य *)((वर्ण *)ca + परिमाण(संरचना addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    यदि(namelen) {
      ca->ai_canonname = (शून्य *)((वर्ण *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = रिक्त;
    }

    /* if the return list is empty, this becomes the first element */
    यदि(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    यदि(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  प्रत्यागम cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
पूर्णाङ्क r_getaddrinfo(ध्रुव वर्ण *गाठ,
                  ध्रुव वर्ण *service,
                  ध्रुव संरचना addrinfo *hints,
                  संरचना addrinfo **res)
{
  पूर्णाङ्क status;
  संरचना context ctx;
  संरचना ares_options options;
  पूर्णाङ्क optmask = 0;
  संरचना ares_addrinfo_hints ahints;
  ares_channel channel;
  पूर्णाङ्क rc = 0;

  memset(&options, 0, परिमाण(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, परिमाण(ahints));
  memset(&ctx, 0, परिमाण(ctx));

  यदि(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  यदि(status)
    प्रत्यागम EAI_MEMORY; /* major problem */

  अन्यथा {
    ध्रुव वर्ण *env = getenv("CURL_DNS_SERVER");
    यदि(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      यदि(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        प्रत्यागम EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, गाठ, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  यदि(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  अन्यथा यदि((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  अन्यथा
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  प्रत्यागम rc;
}

#endif /* USE_FAKE_GETADDRINFO */

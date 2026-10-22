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

  साठी(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

संरचना context {
  संरचना ares_addrinfo *addr;
  पूर्णांक status;
};

स्थिर शून्य async_addrinfo_cb(शून्य *userp, पूर्णांक status, पूर्णांक timeouts,
                              संरचना ares_addrinfo *addr)
{
  संरचना context *ctx = (संरचना context *)userp;
  (शून्य)timeouts;
  ctx->status = status;
  जर(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
स्थिर संरचना addrinfo *mk_getaddrinfo(स्थिरांक संरचना ares_addrinfo *aihead)
{
  स्थिरांक संरचना ares_addrinfo_गाठ *ai;
  संरचना addrinfo *ca;
  संरचना addrinfo *cafirst = रिक्त;
  संरचना addrinfo *calast = रिक्त;
  स्थिरांक अक्षर *name = aihead->name;

  /* traverse the addrinfo list */
  साठी(ai = aihead->nodes; ai; ai = ai->ai_next) {
    माप_प्रकार ss_size;
    माप_प्रकार namelen = name ? म_माप(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    जर(ai->ai_family == AF_INET)
      ss_size = आकार(संरचना sockaddr_in);
    अन्यथा जर(ai->ai_family == AF_INET6)
      ss_size = आकार(संरचना sockaddr_in6);
    अन्यथा
      सुरूठेवा;

    /* ignore elements without required address info */
    जर(!ai->ai_addr || !(ai->ai_addrlen > 0))
      सुरूठेवा;

    /* ignore elements with bogus address size */
    जर((माप_प्रकार)ai->ai_addrlen < ss_size)
      सुरूठेवा;

    ca = curlx_malloc(आकार(संरचना addrinfo) + ss_size + namelen);
    जर(!ca) {
      r_freeaddrinfo(cafirst);
      परत रिक्त;
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

    ca->ai_addr = (शून्य *)((अक्षर *)ca + आकार(संरचना addrinfo));
    स_नकल(ca->ai_addr, ai->ai_addr, ss_size);

    जर(namelen) {
      ca->ai_canonname = (शून्य *)((अक्षर *)ca->ai_addr + ss_size);
      स_नकल(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = रिक्त;
    }

    /* if the return list is empty, this becomes the first element */
    जर(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    जर(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  परत cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
पूर्णांक r_getaddrinfo(स्थिरांक अक्षर *गाठ,
                  स्थिरांक अक्षर *service,
                  स्थिरांक संरचना addrinfo *hints,
                  संरचना addrinfo **res)
{
  पूर्णांक status;
  संरचना context ctx;
  संरचना ares_options options;
  पूर्णांक optmask = 0;
  संरचना ares_addrinfo_hints ahints;
  ares_channel channel;
  पूर्णांक rc = 0;

  स_रखो(&options, 0, आकार(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  स_रखो(&ahints, 0, आकार(ahints));
  स_रखो(&ctx, 0, आकार(ctx));

  जर(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  जर(status)
    परत EAI_MEMORY; /* major problem */

  अन्यथा {
    स्थिरांक अक्षर *env = दो_पर्या("CURL_DNS_SERVER");
    जर(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      जर(rc) {
        curl_mfprintf(मानक_त्रुटि, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        परत EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, गाठ, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  जर(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  अन्यथा जर((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  अन्यथा
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  परत rc;
}

#endif /* USE_FAKE_GETADDRINFO */

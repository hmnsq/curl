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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "fake_addrinfo.h"

#ifdef USE_FAKE_GETADDRINFO

#അന്തര്ഭാവ <ares.h>

ശൂന്യം r_freeaddrinfo(ഘടന addrinfo *cahead)
{
  ഘടന addrinfo *canext;
  ഘടന addrinfo *ca;

  വേണ്ടി(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

ഘടന context {
  ഘടന ares_addrinfo *addr;
  പൂർണ്ണസംഖ്യ status;
};

സ്ഥിതം ശൂന്യം async_addrinfo_cb(ശൂന്യം *userp, പൂർണ്ണസംഖ്യ status, പൂർണ്ണസംഖ്യ timeouts,
                              ഘടന ares_addrinfo *addr)
{
  ഘടന context *ctx = (ഘടന context *)userp;
  (ശൂന്യം)timeouts;
  ctx->status = status;
  എങ്കിൽ(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
സ്ഥിതം ഘടന addrinfo *mk_getaddrinfo(സ്ഥിരം ഘടന ares_addrinfo *aihead)
{
  സ്ഥിരം ഘടന ares_addrinfo_കണ്ണി *ai;
  ഘടന addrinfo *ca;
  ഘടന addrinfo *cafirst = ശൂന്യ;
  ഘടന addrinfo *calast = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *name = aihead->name;

  /* traverse the addrinfo list */
  വേണ്ടി(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    എങ്കിൽ(ai->ai_family == AF_INET)
      ss_size = വലുപ്പം(ഘടന sockaddr_in);
    അല്ലെങ്കിൽ എങ്കിൽ(ai->ai_family == AF_INET6)
      ss_size = വലുപ്പം(ഘടന sockaddr_in6);
    അല്ലെങ്കിൽ
      തുടരുക;

    /* ignore elements without required address info */
    എങ്കിൽ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      തുടരുക;

    /* ignore elements with bogus address size */
    എങ്കിൽ((size_t)ai->ai_addrlen < ss_size)
      തുടരുക;

    ca = curlx_malloc(വലുപ്പം(ഘടന addrinfo) + ss_size + namelen);
    എങ്കിൽ(!ca) {
      r_freeaddrinfo(cafirst);
      തിരികെ ശൂന്യ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ശൂന്യ;
    ca->ai_canonname = ശൂന്യ;
    ca->ai_next      = ശൂന്യ;

    ca->ai_addr = (ശൂന്യം *)((അക്ഷരം *)ca + വലുപ്പം(ഘടന addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    എങ്കിൽ(namelen) {
      ca->ai_canonname = (ശൂന്യം *)((അക്ഷരം *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = ശൂന്യ;
    }

    /* if the return list is empty, this becomes the first element */
    എങ്കിൽ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    എങ്കിൽ(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  തിരികെ cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
പൂർണ്ണസംഖ്യ r_getaddrinfo(സ്ഥിരം അക്ഷരം *കണ്ണി,
                  സ്ഥിരം അക്ഷരം *service,
                  സ്ഥിരം ഘടന addrinfo *hints,
                  ഘടന addrinfo **res)
{
  പൂർണ്ണസംഖ്യ status;
  ഘടന context ctx;
  ഘടന ares_options options;
  പൂർണ്ണസംഖ്യ optmask = 0;
  ഘടന ares_addrinfo_hints ahints;
  ares_channel channel;
  പൂർണ്ണസംഖ്യ rc = 0;

  memset(&options, 0, വലുപ്പം(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, വലുപ്പം(ahints));
  memset(&ctx, 0, വലുപ്പം(ctx));

  എങ്കിൽ(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  എങ്കിൽ(status)
    തിരികെ EAI_MEMORY; /* major problem */

  അല്ലെങ്കിൽ {
    സ്ഥിരം അക്ഷരം *env = getenv("CURL_DNS_SERVER");
    എങ്കിൽ(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      എങ്കിൽ(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        തിരികെ EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, കണ്ണി, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  എങ്കിൽ(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  അല്ലെങ്കിൽ എങ്കിൽ((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  അല്ലെങ്കിൽ
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  തിരികെ rc;
}

#endif /* USE_FAKE_GETADDRINFO */

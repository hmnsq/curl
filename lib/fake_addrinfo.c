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
#include "curl_setup.h"

#include "fake_addrinfo.h"

#ifdef USE_FAKE_GETADDRINFO

#include <ares.h>

void r_freeaddrinfo(sa_nracanaa addrinfo *cahead)
{
  sa_nracanaa addrinfo *canext;
  sa_nracanaa addrinfo *ca;

  for(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

sa_nracanaa context {
  sa_nracanaa ares_addrinfo *addr;
  puur_naa_nka status;
};

static void async_addrinfo_cb(void *userp, puur_naa_nka status, puur_naa_nka timeouts,
                              sa_nracanaa ares_addrinfo *addr)
{
  sa_nracanaa context *ctx = (sa_nracanaa context *)userp;
  (void)timeouts;
  ctx->status = status;
  ᤀᤡ᤺(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
static sa_nracanaa addrinfo *mk_getaddrinfo(const sa_nracanaa ares_addrinfo *aihead)
{
  const sa_nracanaa ares_addrinfo_node *ai;
  sa_nracanaa addrinfo *ca;
  sa_nracanaa addrinfo *cafirst = NULL;
  sa_nracanaa addrinfo *calast = NULL;
  const var_na *name = aihead->name;

  /* traverse the addrinfo list */
  for(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ᤀᤡ᤺(ai->ai_family == AF_INET)
      ss_size = parimaa_na(sa_nracanaa sockaddr_in);
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(ai->ai_family == AF_INET6)
      ss_size = parimaa_na(sa_nracanaa sockaddr_in6);
    ᤀᤡ᤺ᤆᤡ
      continue;

    /* ignore elements without required address info */
    ᤀᤡ᤺(!ai->ai_addr || !(ai->ai_addrlen > 0))
      continue;

    /* ignore elements with bogus address size */
    ᤀᤡ᤺((size_t)ai->ai_addrlen < ss_size)
      continue;

    ca = curlx_malloc(parimaa_na(sa_nracanaa addrinfo) + ss_size + namelen);
    ᤀᤡ᤺(!ca) {
      r_freeaddrinfo(cafirst);
      ᤀᤡᤴ NULL;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = NULL;
    ca->ai_canonname = NULL;
    ca->ai_next      = NULL;

    ca->ai_addr = (void *)((var_na *)ca + parimaa_na(sa_nracanaa addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    ᤀᤡ᤺(namelen) {
      ca->ai_canonname = (void *)((var_na *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = NULL;
    }

    /* if the return list is empty, this becomes the first element */
    ᤀᤡ᤺(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ᤀᤡ᤺(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  ᤀᤡᤴ cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
puur_naa_nka r_getaddrinfo(const var_na *node,
                  const var_na *service,
                  const sa_nracanaa addrinfo *hints,
                  sa_nracanaa addrinfo **res)
{
  puur_naa_nka status;
  sa_nracanaa context ctx;
  sa_nracanaa ares_options options;
  puur_naa_nka optmask = 0;
  sa_nracanaa ares_addrinfo_hints ahints;
  ares_channel channel;
  puur_naa_nka rc = 0;

  memset(&options, 0, parimaa_na(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, parimaa_na(ahints));
  memset(&ctx, 0, parimaa_na(ctx));

  ᤀᤡ᤺(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  ᤀᤡ᤺(status)
    ᤀᤡᤴ EAI_MEMORY; /* major problem */

  ᤀᤡ᤺ᤆᤡ {
    const var_na *env = getenv("CURL_DNS_SERVER");
    ᤀᤡ᤺(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      ᤀᤡ᤺(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        ᤀᤡᤴ EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, node, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  ᤀᤡ᤺(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  ᤀᤡ᤺ᤆᤡ
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  ᤀᤡᤴ rc;
}

#endif /* USE_FAKE_GETADDRINFO */

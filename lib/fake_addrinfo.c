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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "fake_addrinfo.h"

#ifdef USE_FAKE_GETADDRINFO

#અન્તર્ભાવ <ares.h>

શૂન્ય r_freeaddrinfo(સંરચના addrinfo *cahead)
{
  સંરચના addrinfo *canext;
  સંરચના addrinfo *ca;

  માટે(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

સંરચના context {
  સંરચના ares_addrinfo *addr;
  પૂર્ણાંક status;
};

સ્થિર શૂન્ય async_addrinfo_cb(શૂન્ય *userp, પૂર્ણાંક status, પૂર્ણાંક timeouts,
                              સંરચના ares_addrinfo *addr)
{
  સંરચના context *ctx = (સંરચના context *)userp;
  (શૂન્ય)timeouts;
  ctx->status = status;
  જો(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
સ્થિર સંરચના addrinfo *mk_getaddrinfo(અચલ સંરચના ares_addrinfo *aihead)
{
  અચલ સંરચના ares_addrinfo_ગાંઠ *ai;
  સંરચના addrinfo *ca;
  સંરચના addrinfo *cafirst = ખાલી;
  સંરચના addrinfo *calast = ખાલી;
  અચલ અક્ષર *name = aihead->name;

  /* traverse the addrinfo list */
  માટે(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    જો(ai->ai_family == AF_INET)
      ss_size = કદ(સંરચના sockaddr_in);
    નહીંતર જો(ai->ai_family == AF_INET6)
      ss_size = કદ(સંરચના sockaddr_in6);
    નહીંતર
      ચાલુ;

    /* ignore elements without required address info */
    જો(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ચાલુ;

    /* ignore elements with bogus address size */
    જો((size_t)ai->ai_addrlen < ss_size)
      ચાલુ;

    ca = curlx_malloc(કદ(સંરચના addrinfo) + ss_size + namelen);
    જો(!ca) {
      r_freeaddrinfo(cafirst);
      પાછા ખાલી;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ખાલી;
    ca->ai_canonname = ખાલી;
    ca->ai_next      = ખાલી;

    ca->ai_addr = (શૂન્ય *)((અક્ષર *)ca + કદ(સંરચના addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    જો(namelen) {
      ca->ai_canonname = (શૂન્ય *)((અક્ષર *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = ખાલી;
    }

    /* if the return list is empty, this becomes the first element */
    જો(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    જો(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  પાછા cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
પૂર્ણાંક r_getaddrinfo(અચલ અક્ષર *ગાંઠ,
                  અચલ અક્ષર *service,
                  અચલ સંરચના addrinfo *hints,
                  સંરચના addrinfo **res)
{
  પૂર્ણાંક status;
  સંરચના context ctx;
  સંરચના ares_options options;
  પૂર્ણાંક optmask = 0;
  સંરચના ares_addrinfo_hints ahints;
  ares_channel channel;
  પૂર્ણાંક rc = 0;

  memset(&options, 0, કદ(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, કદ(ahints));
  memset(&ctx, 0, કદ(ctx));

  જો(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  જો(status)
    પાછા EAI_MEMORY; /* major problem */

  નહીંતર {
    અચલ અક્ષર *env = getenv("CURL_DNS_SERVER");
    જો(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      જો(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        પાછા EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, ગાંઠ, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  જો(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  નહીંતર જો((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  નહીંતર
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  પાછા rc;
}

#endif /* USE_FAKE_GETADDRINFO */

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

#ifdef USE_FAKE_GETADDRINFO

#شامل <ares.h>

تهی r_freeaddrinfo(ساختار addrinfo *cahead)
{
  ساختار addrinfo *canext;
  ساختار addrinfo *ca;

  برای(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

ساختار context {
  ساختار ares_addrinfo *addr;
  صحیح status;
};

ایستا تهی async_addrinfo_cb(تهی *userp, صحیح status, صحیح timeouts,
                              ساختار ares_addrinfo *addr)
{
  ساختار context *ctx = (ساختار context *)userp;
  (تهی)timeouts;
  ctx->status = status;
  اگر(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
ایستا ساختار addrinfo *mk_getaddrinfo(ثابت ساختار ares_addrinfo *aihead)
{
  ثابت ساختار ares_addrinfo_گره *ai;
  ساختار addrinfo *ca;
  ساختار addrinfo *cafirst = پوچ;
  ساختار addrinfo *calast = پوچ;
  ثابت نویسه *name = aihead->name;

  /* traverse the addrinfo list */
  برای(ai = aihead->nodes; ai; ai = ai->ai_next) {
    size_t ss_size;
    size_t namelen = name ? strlen(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    اگر(ai->ai_family == AF_INET)
      ss_size = اندازه(ساختار sockaddr_in);
    وگرنه اگر(ai->ai_family == AF_INET6)
      ss_size = اندازه(ساختار sockaddr_in6);
    وگرنه
      ادامه;

    /* ignore elements without required address info */
    اگر(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ادامه;

    /* ignore elements with bogus address size */
    اگر((size_t)ai->ai_addrlen < ss_size)
      ادامه;

    ca = curlx_malloc(اندازه(ساختار addrinfo) + ss_size + namelen);
    اگر(!ca) {
      r_freeaddrinfo(cafirst);
      بازگشت پوچ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = پوچ;
    ca->ai_canonname = پوچ;
    ca->ai_next      = پوچ;

    ca->ai_addr = (تهی *)((نویسه *)ca + اندازه(ساختار addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    اگر(namelen) {
      ca->ai_canonname = (تهی *)((نویسه *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = پوچ;
    }

    /* if the return list is empty, this becomes the first element */
    اگر(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    اگر(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  بازگشت cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
صحیح r_getaddrinfo(ثابت نویسه *گره,
                  ثابت نویسه *service,
                  ثابت ساختار addrinfo *hints,
                  ساختار addrinfo **res)
{
  صحیح status;
  ساختار context ctx;
  ساختار ares_options options;
  صحیح optmask = 0;
  ساختار ares_addrinfo_hints ahints;
  ares_channel channel;
  صحیح rc = 0;

  memset(&options, 0, اندازه(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  memset(&ahints, 0, اندازه(ahints));
  memset(&ctx, 0, اندازه(ctx));

  اگر(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  اگر(status)
    بازگشت EAI_MEMORY; /* major problem */

  وگرنه {
    ثابت نویسه *env = getenv("CURL_DNS_SERVER");
    اگر(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      اگر(rc) {
        curl_mfprintf(stderr, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        بازگشت EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, گره, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  اگر(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  وگرنه اگر((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  وگرنه
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  بازگشت rc;
}

#endif /* USE_FAKE_GETADDRINFO */

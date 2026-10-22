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
#समावेश "curl_setup.h"

#समावेश "fake_addrinfo.h"

#अगर_घोषित USE_FAKE_GETADDRINFO

#समावेश <ares.h>

व्योम r_freeaddrinfo(काष्ठा addrinfo *cahead)
{
  काष्ठा addrinfo *canext;
  काष्ठा addrinfo *ca;

  क्रम(ca = cahead; ca; ca = canext) {
    canext = ca->ai_अग्र;
    curlx_free(ca);
  }
}

काष्ठा context {
  काष्ठा ares_addrinfo *addr;
  पूर्णांक status;
};

अटल व्योम async_addrinfo_cb(व्योम *userp, पूर्णांक status, पूर्णांक timeouts,
                              काष्ठा ares_addrinfo *addr)
{
  काष्ठा context *ctx = (काष्ठा context *)userp;
  (व्योम)timeouts;
  ctx->status = status;
  अगर(ARES_SUCCESS == status) {
    ctx->addr = addr;
  }
}

/* convert the c-ares version into the "native" version */
अटल काष्ठा addrinfo *mk_getaddrinfo(स्थिर काष्ठा ares_addrinfo *aihead)
{
  स्थिर काष्ठा ares_addrinfo_गाँठ *ai;
  काष्ठा addrinfo *ca;
  काष्ठा addrinfo *cafirst = शून्य;
  काष्ठा addrinfo *calast = शून्य;
  स्थिर अक्षर *name = aihead->name;

  /* traverse the addrinfo list */
  क्रम(ai = aihead->nodes; ai; ai = ai->ai_अग्र) {
    माप_प्रकार ss_आकार;
    माप_प्रकार namelen = name ? म_माप(name) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    अगर(ai->ai_family == AF_INET)
      ss_आकार = माप(काष्ठा sockaddr_in);
    अन्यथा अगर(ai->ai_family == AF_INET6)
      ss_आकार = माप(काष्ठा sockaddr_in6);
    अन्यथा
      जारी;

    /* ignore elements without required address info */
    अगर(!ai->ai_addr || !(ai->ai_addrlen > 0))
      जारी;

    /* ignore elements with bogus address size */
    अगर((माप_प्रकार)ai->ai_addrlen < ss_आकार)
      जारी;

    ca = curlx_malloc(माप(काष्ठा addrinfo) + ss_आकार + namelen);
    अगर(!ca) {
      r_freeaddrinfo(cafirst);
      वापस शून्य;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
    ca->ai_addr      = शून्य;
    ca->ai_canonname = शून्य;
    ca->ai_अग्र      = शून्य;

    ca->ai_addr = (व्योम *)((अक्षर *)ca + माप(काष्ठा addrinfo));
    स_नकल(ca->ai_addr, ai->ai_addr, ss_आकार);

    अगर(namelen) {
      ca->ai_canonname = (व्योम *)((अक्षर *)ca->ai_addr + ss_आकार);
      स_नकल(ca->ai_canonname, name, namelen);

      /* the name is only pointed to by the first entry in the "real"
         addrinfo chain, so stop now */
      name = शून्य;
    }

    /* if the return list is empty, this becomes the first element */
    अगर(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    अगर(calast)
      calast->ai_अग्र = ca;
    calast = ca;
  }

  वापस cafirst;
}

/* RETURN VALUE
   getaddrinfo() returns 0 if it succeeds, or one of the following nonzero
   error codes:
   ... */
पूर्णांक r_getaddrinfo(स्थिर अक्षर *गाँठ,
                  स्थिर अक्षर *service,
                  स्थिर काष्ठा addrinfo *hints,
                  काष्ठा addrinfo **res)
{
  पूर्णांक status;
  काष्ठा context ctx;
  काष्ठा ares_options options;
  पूर्णांक optmask = 0;
  काष्ठा ares_addrinfo_hints ahints;
  ares_channel channel;
  पूर्णांक rc = 0;

  स_रखो(&options, 0, माप(options));
  optmask      |= ARES_OPT_EVENT_THREAD;
  options.evsys = ARES_EVSYS_DEFAULT;

  स_रखो(&ahints, 0, माप(ahints));
  स_रखो(&ctx, 0, माप(ctx));

  अगर(hints) {
    ahints.ai_flags = hints->ai_flags;
    ahints.ai_family = hints->ai_family;
    ahints.ai_socktype = hints->ai_socktype;
    ahints.ai_protocol = hints->ai_protocol;
  }

  status = ares_init_options(&channel, &options, optmask);
  अगर(status)
    वापस EAI_MEMORY; /* major problem */

  अन्यथा {
    स्थिर अक्षर *env = दो_पर्या("CURL_DNS_SERVER");
    अगर(env) {
      rc = ares_set_servers_ports_csv(channel, env);
      अगर(rc) {
        curl_mfprintf(मानक_त्रुटि, "ares_set_servers_ports_csv failed: %d", rc);
        /* Cleanup */
        ares_destroy(channel);
        वापस EAI_MEMORY; /* we cannot run */
      }
    }
  }

  ares_getaddrinfo(channel, गाँठ, service, &ahints, async_addrinfo_cb, &ctx);

  /* Wait until no more requests are left to be processed */
  ares_queue_wait_empty(channel, -1);

  अगर(ctx.addr) {
    /* convert the c-ares version */
    *res = mk_getaddrinfo(ctx.addr);
    /* free the old */
    ares_freeaddrinfo(ctx.addr);
  }
  अन्यथा अगर((ctx.status == ARES_ENOTFOUND) || (ctx.status == ARES_ENODATA))
    rc = EAI_NONAME; /* no such name */
  अन्यथा
    rc = EAI_AGAIN; /* failed without an authoritative answer */

  /* Cleanup */
  ares_destroy(channel);

  वापस rc;
}

#पूर्ण_अगर /* USE_FAKE_GETADDRINFO */

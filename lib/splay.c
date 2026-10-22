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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "splay.h"

ସ୍ଥିର ସଂରଚନା Curl_ବୃକ୍ଷ *splay(timediff_t ଚାବି,
                               ସଂରଚନା Curl_ବୃକ୍ଷ *ମୂଳ);

#ifdef UNITTESTS
uint32_t Curl_splayget(ସଂରଚନା Curl_ବୃକ୍ଷ *ଗଣ୍ଠି)
{
  DEBUGASSERT(ଗଣ୍ଠି);
  ଫେରନ୍ତୁ ଗଣ୍ଠି->id;
}

#endif

ଶୂନ୍ୟ Curl_timeouts_init(ସଂରଚନା Curl_timeouts *timeouts,
                        ଅଚଳ ସଂରଚନା curltime *ptime_base)
{
  ସଂରଚନା curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->ବୃକ୍ଷ = ଖାଲି;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_ବୃକ୍ଷ *ଗଣ୍ଠି = data ? &data->state.timeouts.splaynode : ଖାଲି;
  ଫେରନ୍ତୁ ଗଣ୍ଠି && ଗଣ୍ଠି->registered;
}

timediff_t Curl_timeouts_offset_us(ସଂରଚନା Curl_timeouts *timeouts,
                                   ଅଚଳ ସଂରଚନା curltime *pts)
{
  ସଂରଚନା curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  ଫେରନ୍ତୁ curlx_ptimediff_us(pts, &time_base);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_timeouts_next_ms(ସଂରଚନା Curl_timeouts *timeouts,
                          ଅଚଳ ସଂରଚନା curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  ଯଦି(timeouts->ବୃକ୍ଷ) { /* splay the lowest key to the root */
    timeouts->ବୃକ୍ଷ = splay(TIMEDIFF_T_MIN, timeouts->ବୃକ୍ଷ);
  }

  ଯଦି(timeouts->ବୃକ୍ଷ) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->ବୃକ୍ଷ->ଚାବି - elapsed_us;
    ଯଦି(pmid)
      *pmid = timeouts->ବୃକ୍ଷ->id;
    ଯଦି(pexpire_offset_us)
      *pexpire_offset_us = timeouts->ବୃକ୍ଷ->ଚାବି;
    ଯଦି(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      ଫେରନ୍ତୁ (ms > INT_MAX) ? INT_MAX : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ms;
    }
    ଅନ୍ୟଥା /* has expired */
      ଫେରନ୍ତୁ 0;
  }
  ଯଦି(pmid)
    *pmid = UINT32_MAX;
  ଯଦି(pexpire_offset_us)
    *pexpire_offset_us = 0;
  ଫେରନ୍ତୁ -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
ସଂରଚନା Curl_ବୃକ୍ଷ *splay(timediff_t ଚାବି,
                             ସଂରଚନା Curl_ବୃକ୍ଷ *ମୂଳ)
{
  ସଂରଚନା Curl_ବୃକ୍ଷ N, *l, *r, *y;

  ଯଦି(!ମୂଳ)
    ଫେରନ୍ତୁ ଖାଲି;
  N.smaller = N.larger = ଖାଲି;
  l = r = &N;

  ପାଇଁ(;;) {
    ଯଦି(ଚାବି < ମୂଳ->ଚାବି) {
      /* key is somewhere in root->smaller branch */
      ଯଦି(!ମୂଳ->smaller)  /* which is empty, done */
        ଭାଙ୍ଗ;
      ଯଦି(ଚାବି < ମୂଳ->smaller->ଚାବି) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = ମୂଳ->smaller;
        ମୂଳ->smaller = y->larger;
        y->larger = ମୂଳ;
        ମୂଳ = y;
        ଯଦି(!ମୂଳ->smaller)
          ଭାଙ୍ଗ;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = ମୂଳ;
      r = ମୂଳ;
      ମୂଳ = ମୂଳ->smaller;
    }
    ଅନ୍ୟଥା ଯଦି(ଚାବି > ମୂଳ->ଚାବି) {
      /* key is somewhere in root->larger branch */
      ଯଦି(!ମୂଳ->larger)  /* which is empty, done */
        ଭାଙ୍ଗ;
      ଯଦି(ଚାବି > ମୂଳ->larger->ଚାବି) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = ମୂଳ->larger;
        ମୂଳ->larger = y->smaller;
        y->smaller = ମୂଳ;
        ମୂଳ = y;
        ଯଦି(!ମୂଳ->larger)
          ଭାଙ୍ଗ;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = ମୂଳ;
      l = ମୂଳ;
      ମୂଳ = ମୂଳ->larger;
    }
    ଅନ୍ୟଥା  /* exact match, root is key, done */
      ଭାଙ୍ଗ;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = ମୂଳ->smaller;
  r->smaller = ମୂଳ->larger;
  ମୂଳ->smaller = N.larger;
  ମୂଳ->larger = N.smaller;

  ଫେରନ୍ତୁ ମୂଳ;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST ସଂରଚନା Curl_ବୃକ୍ଷ *splayinsert(timediff_t ଚାବି,
                                       ସଂରଚନା Curl_ବୃକ୍ଷ *ମୂଳ,
                                       ସଂରଚନା Curl_ବୃକ୍ଷ *ଗଣ୍ଠି,
                                       uint32_t id);
UNITTEST ସଂରଚନା Curl_ବୃକ୍ଷ *splayinsert(timediff_t ଚାବି,
                                       ସଂରଚନା Curl_ବୃକ୍ଷ *ମୂଳ,
                                       ସଂରଚନା Curl_ବୃକ୍ଷ *ଗଣ୍ଠି,
                                       uint32_t id)
{
  DEBUGASSERT(ଗଣ୍ଠି);

  ଗଣ୍ଠି->ଚାବି = ଚାବି;
  ଗଣ୍ଠି->id = id;
  ଗଣ୍ଠି->same = ଖାଲି;
  ଗଣ୍ଠି->registered = TRUE;
  ଯଦି(ମୂଳ) {
    ମୂଳ = splay(ଚାବି, ମୂଳ);
    DEBUGASSERT(ମୂଳ);
    ଯଦି(ଚାବି == ମୂଳ->ଚାବି) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      ସଂରଚନା Curl_ବୃକ୍ଷ **panchor = &ମୂଳ->same;
      ଯେତେବେଳେ(*panchor)
        panchor = &(*panchor)->same;
      *panchor = ଗଣ୍ଠି;
      ଫେରନ୍ତୁ ମୂଳ; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  ଯଦି(!ମୂଳ) {
    ଗଣ୍ଠି->smaller = ଗଣ୍ଠି->larger = ଖାଲି;
  }
  ଅନ୍ୟଥା ଯଦି(ଚାବି < ମୂଳ->ଚାବି) {
    ଗଣ୍ଠି->smaller = ମୂଳ->smaller;
    ଗଣ୍ଠି->larger = ମୂଳ;
    ମୂଳ->smaller = ଖାଲି;
  }
  ଅନ୍ୟଥା {
    ଗଣ୍ଠି->larger = ମୂଳ->larger;
    ଗଣ୍ଠି->smaller = ମୂଳ;
    ମୂଳ->larger = ଖାଲି;
  }

  ଫେରନ୍ତୁ ଗଣ୍ଠି;
}

ଶୂନ୍ୟ Curl_timeouts_add(ସଂରଚନା Curl_timeouts *timeouts,
                       ସଂରଚନା Curl_easy *data,
                       timediff_t offset_us)
{
  ସଂରଚନା Curl_ବୃକ୍ଷ *ଗଣ୍ଠି = &data->state.timeouts.splaynode;
  DEBUGASSERT(!ଗଣ୍ଠି->registered);
  timeouts->ବୃକ୍ଷ = splayinsert(offset_us, timeouts->ବୃକ୍ଷ,
                               ଗଣ୍ଠି, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST ସଂରଚନା Curl_ବୃକ୍ଷ *splaygetbest(timediff_t ଚାବି,
                                        ସଂରଚନା Curl_ବୃକ୍ଷ *ମୂଳ,
                                        ସଂରଚନା Curl_ବୃକ୍ଷ **removed);
UNITTEST ସଂରଚନା Curl_ବୃକ୍ଷ *splaygetbest(timediff_t ଚାବି,
                                        ସଂରଚନା Curl_ବୃକ୍ଷ *ମୂଳ,
                                        ସଂରଚନା Curl_ବୃକ୍ଷ **removed)
{
  ସଂରଚନା Curl_ବୃକ୍ଷ *x;

  ଯଦି(!ମୂଳ) {
    *removed = ଖାଲି; /* none removed since there was no root */
    ଫେରନ୍ତୁ ଖାଲି;
  }

  /* find smallest */
  ମୂଳ = splay(TIMEDIFF_T_MIN, ମୂଳ);
  DEBUGASSERT(ମୂଳ);
  ଯଦି(ଚାବି < ମୂଳ->ଚାବି) {
    /* even the smallest is too big */
    *removed = ଖାଲି;
    ଫେରନ୍ତୁ ମୂଳ;
  }

  /* FIRST! Check if there is a list with identical keys */
  ଯଦି(ମୂଳ->same) {
    x = ମୂଳ->same;
    DEBUGASSERT(x->ଚାବି == ମୂଳ->ଚାବି);
    /* 'x' becomes the new root node */
    x->larger = ମୂଳ->larger;
    x->smaller = ମୂଳ->smaller;
    ମୂଳ->same = ଖାଲି;
    ମୂଳ->registered = FALSE;
    *removed = ମୂଳ;
    ଫେରନ୍ତୁ x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = ମୂଳ->larger;
  ମୂଳ->registered = FALSE;
  *removed = ମୂଳ;

  ଫେରନ୍ତୁ x;
}

bool Curl_timeouts_remove_expired(ସଂରଚନା Curl_timeouts *timeouts,
                                  ଅଚଳ ସଂରଚନା curltime *ts,
                                  uint32_t *pmid)
{
  ଯଦି(timeouts->ବୃକ୍ଷ) {
    ସଂରଚନା Curl_ବୃକ୍ଷ *t = ଖାଲି;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->ବୃକ୍ଷ = splaygetbest(elapsed_us, timeouts->ବୃକ୍ଷ, &t);
    ଯଦି(t) {
      *pmid = t->id;
      ଫେରନ୍ତୁ TRUE;
    }
  }
  *pmid = UINT32_MAX;
  ଫେରନ୍ତୁ FALSE;
}

/* Deletes the node we point out from the tree if it is there. Stores a
 * pointer to the new resulting tree in 'newroot'.
 *
 * Returns zero on success and non-zero on errors!
 * When returning error, it does not touch the 'newroot' pointer.
 *
 * NOTE: when the last node of the tree is removed, there is no tree left so
 * 'newroot' will be made to point to NULL.
 *
 * @unittest 1309
 */
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା splayremove(ସଂରଚନା Curl_ବୃକ୍ଷ *ମୂଳ,
                         ସଂରଚନା Curl_ବୃକ୍ଷ *removenode,
                         ସଂରଚନା Curl_ବୃକ୍ଷ **newroot);
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା splayremove(ସଂରଚନା Curl_ବୃକ୍ଷ *ମୂଳ,
                         ସଂରଚନା Curl_ବୃକ୍ଷ *removenode,
                         ସଂରଚନା Curl_ବୃକ୍ଷ **newroot)
{
  ସଂରଚନା Curl_ବୃକ୍ଷ *x;

  ଯଦି(!ମୂଳ)
    ଫେରନ୍ତୁ 1;

  DEBUGASSERT(removenode);
  ଯଦି(!removenode->registered)
    ଫେରନ୍ତୁ 2;

  ମୂଳ = splay(removenode->ଚାବି, ମୂଳ);
  DEBUGASSERT(ମୂଳ);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  ଯଦି(ମୂଳ->ଚାବି != removenode->ଚାବି) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ 2;
  }

  ଯଦି(ମୂଳ != removenode) {
    /* Should be in the root->same list then */
    ସଂରଚନା Curl_ବୃକ୍ଷ **panchor;
    ପାଇଁ(panchor = &ମୂଳ->same; *panchor; panchor = &(*panchor)->same) {
      ଯଦି(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = ଖାଲି;
        removenode->registered = FALSE;
        *newroot = ମୂଳ;
        ଫେରନ୍ତୁ 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ 2;
  }
  /* removing the root node */
  ଯଦି(ମୂଳ->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = ମୂଳ->same;
    x->larger = ମୂଳ->larger;
    x->smaller = ମୂଳ->smaller;
    ମୂଳ->same = ଖାଲି;
  }
  ଅନ୍ୟଥା {
    /* Remove the root node */
    ଯଦି(!ମୂଳ->smaller)
      x = ମୂଳ->larger;
    ଅନ୍ୟଥା {
      x = splay(removenode->ଚାବି, ମୂଳ->smaller);
      DEBUGASSERT(x);
      x->larger = ମୂଳ->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  ଫେରନ୍ତୁ 0;
}

bool Curl_timeouts_remove(ସଂରଚନା Curl_timeouts *timeouts,
                          ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_ବୃକ୍ଷ *ଗଣ୍ଠି = &data->state.timeouts.splaynode;
  ଯଦି(ଗଣ୍ଠି->registered) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = splayremove(timeouts->ବୃକ୍ଷ, ଗଣ୍ଠି, &timeouts->ବୃକ୍ଷ);
#ifdef DEBUGBUILD
    ଯଦି(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#ଅନ୍ୟଥା
    (ଶୂନ୍ୟ)rc;
#endif
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "splay.h"

ਸਥਿਰ ਬਣਤਰ Curl_ਰੁੱਖ *splay(timediff_t ਕੁੰਜੀ,
                               ਬਣਤਰ Curl_ਰੁੱਖ *ਜੜ੍ਹ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
uint32_t Curl_splayget(ਬਣਤਰ Curl_ਰੁੱਖ *ਗੰਢ)
{
  DEBUGASSERT(ਗੰਢ);
  ਵਾਪਸ ਗੰਢ->id;
}

#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_timeouts_init(ਬਣਤਰ Curl_timeouts *timeouts,
                        ਅਚਲ ਬਣਤਰ curltime *ptime_ਅਧਾਰ)
{
  ਬਣਤਰ curltime ਸਮਾਂ_ਅਧਾਰ = ptime_ਅਧਾਰ ? *ptime_ਅਧਾਰ : curlx_now();
  timeouts->ਰੁੱਖ = ਨਿਰਾ;
  timeouts->ਸਮਾਂ_ਅਧਾਰ_sec = ਸਮਾਂ_ਅਧਾਰ.tv_sec;
}

bool Curl_timeouts_has(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_ਰੁੱਖ *ਗੰਢ = ਅੰਕੜੇ ? &ਅੰਕੜੇ->state.timeouts.splaynode : ਨਿਰਾ;
  ਵਾਪਸ ਗੰਢ && ਗੰਢ->registered;
}

timediff_t Curl_timeouts_ਵਿੱਥ_us(ਬਣਤਰ Curl_timeouts *timeouts,
                                   ਅਚਲ ਬਣਤਰ curltime *pts)
{
  ਬਣਤਰ curltime ਸਮਾਂ_ਅਧਾਰ;
  ਸਮਾਂ_ਅਧਾਰ.tv_sec = timeouts->ਸਮਾਂ_ਅਧਾਰ_sec;
  ਸਮਾਂ_ਅਧਾਰ.tv_usec = 0;
  ਵਾਪਸ curlx_ptimediff_us(pts, &ਸਮਾਂ_ਅਧਾਰ);
}

ਪੂਰਨ Curl_timeouts_ਅਗਲਾ_ms(ਬਣਤਰ Curl_timeouts *timeouts,
                          ਅਚਲ ਬਣਤਰ curltime *pnow,
                          timediff_t *pexpire_ਵਿੱਥ_us,
                          uint32_t *pmid)
{
  ਜੇ(timeouts->ਰੁੱਖ) { /* splay the lowest key to the root */
    timeouts->ਰੁੱਖ = splay(TIMEDIFF_T_MIN, timeouts->ਰੁੱਖ);
  }

  ਜੇ(timeouts->ਰੁੱਖ) {
    timediff_t elapsed_us = Curl_timeouts_ਵਿੱਥ_us(timeouts, pnow);
    timediff_t delta_us = timeouts->ਰੁੱਖ->ਕੁੰਜੀ - elapsed_us;
    ਜੇ(pmid)
      *pmid = timeouts->ਰੁੱਖ->id;
    ਜੇ(pexpire_ਵਿੱਥ_us)
      *pexpire_ਵਿੱਥ_us = timeouts->ਰੁੱਖ->ਕੁੰਜੀ;
    ਜੇ(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_ਮੰਜ਼ਿਲ_ceil_ms(delta_us);
      ਵਾਪਸ (ms > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : (ਪੂਰਨ)ms;
    }
    ਨਹੀਂਤਾਂ /* has expired */
      ਵਾਪਸ 0;
  }
  ਜੇ(pmid)
    *pmid = UINT32_MAX;
  ਜੇ(pexpire_ਵਿੱਥ_us)
    *pexpire_ਵਿੱਥ_us = 0;
  ਵਾਪਸ -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
ਬਣਤਰ Curl_ਰੁੱਖ *splay(timediff_t ਕੁੰਜੀ,
                             ਬਣਤਰ Curl_ਰੁੱਖ *ਜੜ੍ਹ)
{
  ਬਣਤਰ Curl_ਰੁੱਖ N, *l, *r, *y;

  ਜੇ(!ਜੜ੍ਹ)
    ਵਾਪਸ ਨਿਰਾ;
  N.smaller = N.larger = ਨਿਰਾ;
  l = r = &N;

  ਲਈ(;;) {
    ਜੇ(ਕੁੰਜੀ < ਜੜ੍ਹ->ਕੁੰਜੀ) {
      /* key is somewhere in root->smaller branch */
      ਜੇ(!ਜੜ੍ਹ->smaller)  /* which is empty, done */
        ਰੋਕੋ;
      ਜੇ(ਕੁੰਜੀ < ਜੜ੍ਹ->smaller->ਕੁੰਜੀ) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = ਜੜ੍ਹ->smaller;
        ਜੜ੍ਹ->smaller = y->larger;
        y->larger = ਜੜ੍ਹ;
        ਜੜ੍ਹ = y;
        ਜੇ(!ਜੜ੍ਹ->smaller)
          ਰੋਕੋ;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = ਜੜ੍ਹ;
      r = ਜੜ੍ਹ;
      ਜੜ੍ਹ = ਜੜ੍ਹ->smaller;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਕੁੰਜੀ > ਜੜ੍ਹ->ਕੁੰਜੀ) {
      /* key is somewhere in root->larger branch */
      ਜੇ(!ਜੜ੍ਹ->larger)  /* which is empty, done */
        ਰੋਕੋ;
      ਜੇ(ਕੁੰਜੀ > ਜੜ੍ਹ->larger->ਕੁੰਜੀ) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = ਜੜ੍ਹ->larger;
        ਜੜ੍ਹ->larger = y->smaller;
        y->smaller = ਜੜ੍ਹ;
        ਜੜ੍ਹ = y;
        ਜੇ(!ਜੜ੍ਹ->larger)
          ਰੋਕੋ;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = ਜੜ੍ਹ;
      l = ਜੜ੍ਹ;
      ਜੜ੍ਹ = ਜੜ੍ਹ->larger;
    }
    ਨਹੀਂਤਾਂ  /* exact match, root is key, done */
      ਰੋਕੋ;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = ਜੜ੍ਹ->smaller;
  r->smaller = ਜੜ੍ਹ->larger;
  ਜੜ੍ਹ->smaller = N.larger;
  ਜੜ੍ਹ->larger = N.smaller;

  ਵਾਪਸ ਜੜ੍ਹ;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST ਬਣਤਰ Curl_ਰੁੱਖ *splayinsert(timediff_t ਕੁੰਜੀ,
                                       ਬਣਤਰ Curl_ਰੁੱਖ *ਜੜ੍ਹ,
                                       ਬਣਤਰ Curl_ਰੁੱਖ *ਗੰਢ,
                                       uint32_t id);
UNITTEST ਬਣਤਰ Curl_ਰੁੱਖ *splayinsert(timediff_t ਕੁੰਜੀ,
                                       ਬਣਤਰ Curl_ਰੁੱਖ *ਜੜ੍ਹ,
                                       ਬਣਤਰ Curl_ਰੁੱਖ *ਗੰਢ,
                                       uint32_t id)
{
  DEBUGASSERT(ਗੰਢ);

  ਗੰਢ->ਕੁੰਜੀ = ਕੁੰਜੀ;
  ਗੰਢ->id = id;
  ਗੰਢ->same = ਨਿਰਾ;
  ਗੰਢ->registered = TRUE;
  ਜੇ(ਜੜ੍ਹ) {
    ਜੜ੍ਹ = splay(ਕੁੰਜੀ, ਜੜ੍ਹ);
    DEBUGASSERT(ਜੜ੍ਹ);
    ਜੇ(ਕੁੰਜੀ == ਜੜ੍ਹ->ਕੁੰਜੀ) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      ਬਣਤਰ Curl_ਰੁੱਖ **panchor = &ਜੜ੍ਹ->same;
      ਜਦੋਂ(*panchor)
        panchor = &(*panchor)->same;
      *panchor = ਗੰਢ;
      ਵਾਪਸ ਜੜ੍ਹ; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  ਜੇ(!ਜੜ੍ਹ) {
    ਗੰਢ->smaller = ਗੰਢ->larger = ਨਿਰਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਕੁੰਜੀ < ਜੜ੍ਹ->ਕੁੰਜੀ) {
    ਗੰਢ->smaller = ਜੜ੍ਹ->smaller;
    ਗੰਢ->larger = ਜੜ੍ਹ;
    ਜੜ੍ਹ->smaller = ਨਿਰਾ;
  }
  ਨਹੀਂਤਾਂ {
    ਗੰਢ->larger = ਜੜ੍ਹ->larger;
    ਗੰਢ->smaller = ਜੜ੍ਹ;
    ਜੜ੍ਹ->larger = ਨਿਰਾ;
  }

  ਵਾਪਸ ਗੰਢ;
}

ਖਾਲੀ Curl_timeouts_ਜੋੜੋ(ਬਣਤਰ Curl_timeouts *timeouts,
                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       timediff_t ਵਿੱਥ_us)
{
  ਬਣਤਰ Curl_ਰੁੱਖ *ਗੰਢ = &ਅੰਕੜੇ->state.timeouts.splaynode;
  DEBUGASSERT(!ਗੰਢ->registered);
  timeouts->ਰੁੱਖ = splayinsert(ਵਿੱਥ_us, timeouts->ਰੁੱਖ,
                               ਗੰਢ, ਅੰਕੜੇ->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST ਬਣਤਰ Curl_ਰੁੱਖ *splaygetbest(timediff_t ਕੁੰਜੀ,
                                        ਬਣਤਰ Curl_ਰੁੱਖ *ਜੜ੍ਹ,
                                        ਬਣਤਰ Curl_ਰੁੱਖ **removed);
UNITTEST ਬਣਤਰ Curl_ਰੁੱਖ *splaygetbest(timediff_t ਕੁੰਜੀ,
                                        ਬਣਤਰ Curl_ਰੁੱਖ *ਜੜ੍ਹ,
                                        ਬਣਤਰ Curl_ਰੁੱਖ **removed)
{
  ਬਣਤਰ Curl_ਰੁੱਖ *x;

  ਜੇ(!ਜੜ੍ਹ) {
    *removed = ਨਿਰਾ; /* none removed since there was no root */
    ਵਾਪਸ ਨਿਰਾ;
  }

  /* find smallest */
  ਜੜ੍ਹ = splay(TIMEDIFF_T_MIN, ਜੜ੍ਹ);
  DEBUGASSERT(ਜੜ੍ਹ);
  ਜੇ(ਕੁੰਜੀ < ਜੜ੍ਹ->ਕੁੰਜੀ) {
    /* even the smallest is too big */
    *removed = ਨਿਰਾ;
    ਵਾਪਸ ਜੜ੍ਹ;
  }

  /* FIRST! Check if there is a list with identical keys */
  ਜੇ(ਜੜ੍ਹ->same) {
    x = ਜੜ੍ਹ->same;
    DEBUGASSERT(x->ਕੁੰਜੀ == ਜੜ੍ਹ->ਕੁੰਜੀ);
    /* 'x' becomes the new root node */
    x->larger = ਜੜ੍ਹ->larger;
    x->smaller = ਜੜ੍ਹ->smaller;
    ਜੜ੍ਹ->same = ਨਿਰਾ;
    ਜੜ੍ਹ->registered = FALSE;
    *removed = ਜੜ੍ਹ;
    ਵਾਪਸ x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = ਜੜ੍ਹ->larger;
  ਜੜ੍ਹ->registered = FALSE;
  *removed = ਜੜ੍ਹ;

  ਵਾਪਸ x;
}

bool Curl_timeouts_remove_expired(ਬਣਤਰ Curl_timeouts *timeouts,
                                  ਅਚਲ ਬਣਤਰ curltime *ts,
                                  uint32_t *pmid)
{
  ਜੇ(timeouts->ਰੁੱਖ) {
    ਬਣਤਰ Curl_ਰੁੱਖ *t = ਨਿਰਾ;
    timediff_t elapsed_us = Curl_timeouts_ਵਿੱਥ_us(timeouts, ts);
    timeouts->ਰੁੱਖ = splaygetbest(elapsed_us, timeouts->ਰੁੱਖ, &t);
    ਜੇ(t) {
      *pmid = t->id;
      ਵਾਪਸ TRUE;
    }
  }
  *pmid = UINT32_MAX;
  ਵਾਪਸ FALSE;
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
UNITTEST ਪੂਰਨ splayremove(ਬਣਤਰ Curl_ਰੁੱਖ *ਜੜ੍ਹ,
                         ਬਣਤਰ Curl_ਰੁੱਖ *removenode,
                         ਬਣਤਰ Curl_ਰੁੱਖ **newroot);
UNITTEST ਪੂਰਨ splayremove(ਬਣਤਰ Curl_ਰੁੱਖ *ਜੜ੍ਹ,
                         ਬਣਤਰ Curl_ਰੁੱਖ *removenode,
                         ਬਣਤਰ Curl_ਰੁੱਖ **newroot)
{
  ਬਣਤਰ Curl_ਰੁੱਖ *x;

  ਜੇ(!ਜੜ੍ਹ)
    ਵਾਪਸ 1;

  DEBUGASSERT(removenode);
  ਜੇ(!removenode->registered)
    ਵਾਪਸ 2;

  ਜੜ੍ਹ = splay(removenode->ਕੁੰਜੀ, ਜੜ੍ਹ);
  DEBUGASSERT(ਜੜ੍ਹ);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  ਜੇ(ਜੜ੍ਹ->ਕੁੰਜੀ != removenode->ਕੁੰਜੀ) {
    DEBUGASSERT(0);
    ਵਾਪਸ 2;
  }

  ਜੇ(ਜੜ੍ਹ != removenode) {
    /* Should be in the root->same list then */
    ਬਣਤਰ Curl_ਰੁੱਖ **panchor;
    ਲਈ(panchor = &ਜੜ੍ਹ->same; *panchor; panchor = &(*panchor)->same) {
      ਜੇ(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = ਨਿਰਾ;
        removenode->registered = FALSE;
        *newroot = ਜੜ੍ਹ;
        ਵਾਪਸ 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    ਵਾਪਸ 2;
  }
  /* removing the root node */
  ਜੇ(ਜੜ੍ਹ->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = ਜੜ੍ਹ->same;
    x->larger = ਜੜ੍ਹ->larger;
    x->smaller = ਜੜ੍ਹ->smaller;
    ਜੜ੍ਹ->same = ਨਿਰਾ;
  }
  ਨਹੀਂਤਾਂ {
    /* Remove the root node */
    ਜੇ(!ਜੜ੍ਹ->smaller)
      x = ਜੜ੍ਹ->larger;
    ਨਹੀਂਤਾਂ {
      x = splay(removenode->ਕੁੰਜੀ, ਜੜ੍ਹ->smaller);
      DEBUGASSERT(x);
      x->larger = ਜੜ੍ਹ->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  ਵਾਪਸ 0;
}

bool Curl_timeouts_remove(ਬਣਤਰ Curl_timeouts *timeouts,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_ਰੁੱਖ *ਗੰਢ = &ਅੰਕੜੇ->state.timeouts.splaynode;
  ਜੇ(ਗੰਢ->registered) {
    ਪੂਰਨ rc = splayremove(timeouts->ਰੁੱਖ, ਗੰਢ, &timeouts->ਰੁੱਖ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    ਜੇ(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)rc;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

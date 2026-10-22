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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "splay.h"

ساكن بنية Curl_ڤوکوق *splay(timediff_t کونچي,
                               بنية Curl_ڤوکوق *اکر);

#ifdef UNITTESTS
uint32_t Curl_splayget(بنية Curl_ڤوکوق *نود)
{
  DEBUGASSERT(نود);
  أرجع نود->id;
}

#endif

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_base)
{
  بنية curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->ڤوکوق = عدم;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *data)
{
  بنية Curl_ڤوکوق *نود = data ? &data->state.timeouts.splaynode : عدم;
  أرجع نود && نود->registered;
}

timediff_t Curl_timeouts_offset_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts)
{
  بنية curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  أرجع curlx_ptimediff_us(pts, &time_base);
}

صحيح Curl_timeouts_next_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  إذا(timeouts->ڤوکوق) { /* splay the lowest key to the root */
    timeouts->ڤوکوق = splay(TIMEDIFF_T_MIN, timeouts->ڤوکوق);
  }

  إذا(timeouts->ڤوکوق) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->ڤوکوق->کونچي - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->ڤوکوق->id;
    إذا(pexpire_offset_us)
      *pexpire_offset_us = timeouts->ڤوکوق->کونچي;
    إذا(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      أرجع (ms > INT_MAX) ? INT_MAX : (صحيح)ms;
    }
    وإلا /* has expired */
      أرجع 0;
  }
  إذا(pmid)
    *pmid = UINT32_MAX;
  إذا(pexpire_offset_us)
    *pexpire_offset_us = 0;
  أرجع -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
بنية Curl_ڤوکوق *splay(timediff_t کونچي,
                             بنية Curl_ڤوکوق *اکر)
{
  بنية Curl_ڤوکوق N, *l, *r, *y;

  إذا(!اکر)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(کونچي < اکر->کونچي) {
      /* key is somewhere in root->smaller branch */
      إذا(!اکر->smaller)  /* which is empty, done */
        اكسر;
      إذا(کونچي < اکر->smaller->کونچي) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = اکر->smaller;
        اکر->smaller = y->larger;
        y->larger = اکر;
        اکر = y;
        إذا(!اکر->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = اکر;
      r = اکر;
      اکر = اکر->smaller;
    }
    وإلا إذا(کونچي > اکر->کونچي) {
      /* key is somewhere in root->larger branch */
      إذا(!اکر->larger)  /* which is empty, done */
        اكسر;
      إذا(کونچي > اکر->larger->کونچي) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = اکر->larger;
        اکر->larger = y->smaller;
        y->smaller = اکر;
        اکر = y;
        إذا(!اکر->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = اکر;
      l = اکر;
      اکر = اکر->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = اکر->smaller;
  r->smaller = اکر->larger;
  اکر->smaller = N.larger;
  اکر->larger = N.smaller;

  أرجع اکر;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_ڤوکوق *splayinsert(timediff_t کونچي,
                                       بنية Curl_ڤوکوق *اکر,
                                       بنية Curl_ڤوکوق *نود,
                                       uint32_t id);
UNITTEST بنية Curl_ڤوکوق *splayinsert(timediff_t کونچي,
                                       بنية Curl_ڤوکوق *اکر,
                                       بنية Curl_ڤوکوق *نود,
                                       uint32_t id)
{
  DEBUGASSERT(نود);

  نود->کونچي = کونچي;
  نود->id = id;
  نود->same = عدم;
  نود->registered = TRUE;
  إذا(اکر) {
    اکر = splay(کونچي, اکر);
    DEBUGASSERT(اکر);
    إذا(کونچي == اکر->کونچي) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_ڤوکوق **panchor = &اکر->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = نود;
      أرجع اکر; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!اکر) {
    نود->smaller = نود->larger = عدم;
  }
  وإلا إذا(کونچي < اکر->کونچي) {
    نود->smaller = اکر->smaller;
    نود->larger = اکر;
    اکر->smaller = عدم;
  }
  وإلا {
    نود->larger = اکر->larger;
    نود->smaller = اکر;
    اکر->larger = عدم;
  }

  أرجع نود;
}

فراغ Curl_timeouts_add(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *data,
                       timediff_t offset_us)
{
  بنية Curl_ڤوکوق *نود = &data->state.timeouts.splaynode;
  DEBUGASSERT(!نود->registered);
  timeouts->ڤوکوق = splayinsert(offset_us, timeouts->ڤوکوق,
                               نود, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_ڤوکوق *splaygetbest(timediff_t کونچي,
                                        بنية Curl_ڤوکوق *اکر,
                                        بنية Curl_ڤوکوق **removed);
UNITTEST بنية Curl_ڤوکوق *splaygetbest(timediff_t کونچي,
                                        بنية Curl_ڤوکوق *اکر,
                                        بنية Curl_ڤوکوق **removed)
{
  بنية Curl_ڤوکوق *x;

  إذا(!اکر) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  اکر = splay(TIMEDIFF_T_MIN, اکر);
  DEBUGASSERT(اکر);
  إذا(کونچي < اکر->کونچي) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع اکر;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(اکر->same) {
    x = اکر->same;
    DEBUGASSERT(x->کونچي == اکر->کونچي);
    /* 'x' becomes the new root node */
    x->larger = اکر->larger;
    x->smaller = اکر->smaller;
    اکر->same = عدم;
    اکر->registered = FALSE;
    *removed = اکر;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = اکر->larger;
  اکر->registered = FALSE;
  *removed = اکر;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->ڤوکوق) {
    بنية Curl_ڤوکوق *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->ڤوکوق = splaygetbest(elapsed_us, timeouts->ڤوکوق, &t);
    إذا(t) {
      *pmid = t->id;
      أرجع TRUE;
    }
  }
  *pmid = UINT32_MAX;
  أرجع FALSE;
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
UNITTEST صحيح splayremove(بنية Curl_ڤوکوق *اکر,
                         بنية Curl_ڤوکوق *removenode,
                         بنية Curl_ڤوکوق **newroot);
UNITTEST صحيح splayremove(بنية Curl_ڤوکوق *اکر,
                         بنية Curl_ڤوکوق *removenode,
                         بنية Curl_ڤوکوق **newroot)
{
  بنية Curl_ڤوکوق *x;

  إذا(!اکر)
    أرجع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    أرجع 2;

  اکر = splay(removenode->کونچي, اکر);
  DEBUGASSERT(اکر);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(اکر->کونچي != removenode->کونچي) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  إذا(اکر != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_ڤوکوق **panchor;
    لكل(panchor = &اکر->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = اکر;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  إذا(اکر->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = اکر->same;
    x->larger = اکر->larger;
    x->smaller = اکر->smaller;
    اکر->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!اکر->smaller)
      x = اکر->larger;
    وإلا {
      x = splay(removenode->کونچي, اکر->smaller);
      DEBUGASSERT(x);
      x->larger = اکر->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *data)
{
  بنية Curl_ڤوکوق *نود = &data->state.timeouts.splaynode;
  إذا(نود->registered) {
    صحيح rc = splayremove(timeouts->ڤوکوق, نود, &timeouts->ڤوکوق);
#ifdef DEBUGBUILD
    إذا(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#وإلا
    (فراغ)rc;
#endif
    أرجع TRUE;
  }
  أرجع FALSE;
}

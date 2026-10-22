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

ساكن بنية Curl_درەخت *splay(timediff_t کلیل,
                               بنية Curl_درەخت *ڕەگ);

#ifdef UNITTESTS
uint32_t Curl_splayget(بنية Curl_درەخت *گرێ)
{
  DEBUGASSERT(گرێ);
  أرجع گرێ->id;
}

#endif

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_base)
{
  بنية curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->درەخت = عدم;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *data)
{
  بنية Curl_درەخت *گرێ = data ? &data->state.timeouts.splaynode : عدم;
  أرجع گرێ && گرێ->registered;
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
  إذا(timeouts->درەخت) { /* splay the lowest key to the root */
    timeouts->درەخت = splay(TIMEDIFF_T_MIN, timeouts->درەخت);
  }

  إذا(timeouts->درەخت) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->درەخت->کلیل - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->درەخت->id;
    إذا(pexpire_offset_us)
      *pexpire_offset_us = timeouts->درەخت->کلیل;
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
بنية Curl_درەخت *splay(timediff_t کلیل,
                             بنية Curl_درەخت *ڕەگ)
{
  بنية Curl_درەخت N, *l, *r, *y;

  إذا(!ڕەگ)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(کلیل < ڕەگ->کلیل) {
      /* key is somewhere in root->smaller branch */
      إذا(!ڕەگ->smaller)  /* which is empty, done */
        اكسر;
      إذا(کلیل < ڕەگ->smaller->کلیل) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = ڕەگ->smaller;
        ڕەگ->smaller = y->larger;
        y->larger = ڕەگ;
        ڕەگ = y;
        إذا(!ڕەگ->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = ڕەگ;
      r = ڕەگ;
      ڕەگ = ڕەگ->smaller;
    }
    وإلا إذا(کلیل > ڕەگ->کلیل) {
      /* key is somewhere in root->larger branch */
      إذا(!ڕەگ->larger)  /* which is empty, done */
        اكسر;
      إذا(کلیل > ڕەگ->larger->کلیل) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = ڕەگ->larger;
        ڕەگ->larger = y->smaller;
        y->smaller = ڕەگ;
        ڕەگ = y;
        إذا(!ڕەگ->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = ڕەگ;
      l = ڕەگ;
      ڕەگ = ڕەگ->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = ڕەگ->smaller;
  r->smaller = ڕەگ->larger;
  ڕەگ->smaller = N.larger;
  ڕەگ->larger = N.smaller;

  أرجع ڕەگ;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_درەخت *splayinsert(timediff_t کلیل,
                                       بنية Curl_درەخت *ڕەگ,
                                       بنية Curl_درەخت *گرێ,
                                       uint32_t id);
UNITTEST بنية Curl_درەخت *splayinsert(timediff_t کلیل,
                                       بنية Curl_درەخت *ڕەگ,
                                       بنية Curl_درەخت *گرێ,
                                       uint32_t id)
{
  DEBUGASSERT(گرێ);

  گرێ->کلیل = کلیل;
  گرێ->id = id;
  گرێ->same = عدم;
  گرێ->registered = TRUE;
  إذا(ڕەگ) {
    ڕەگ = splay(کلیل, ڕەگ);
    DEBUGASSERT(ڕەگ);
    إذا(کلیل == ڕەگ->کلیل) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_درەخت **panchor = &ڕەگ->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = گرێ;
      أرجع ڕەگ; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!ڕەگ) {
    گرێ->smaller = گرێ->larger = عدم;
  }
  وإلا إذا(کلیل < ڕەگ->کلیل) {
    گرێ->smaller = ڕەگ->smaller;
    گرێ->larger = ڕەگ;
    ڕەگ->smaller = عدم;
  }
  وإلا {
    گرێ->larger = ڕەگ->larger;
    گرێ->smaller = ڕەگ;
    ڕەگ->larger = عدم;
  }

  أرجع گرێ;
}

فراغ Curl_timeouts_add(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *data,
                       timediff_t offset_us)
{
  بنية Curl_درەخت *گرێ = &data->state.timeouts.splaynode;
  DEBUGASSERT(!گرێ->registered);
  timeouts->درەخت = splayinsert(offset_us, timeouts->درەخت,
                               گرێ, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_درەخت *splaygetbest(timediff_t کلیل,
                                        بنية Curl_درەخت *ڕەگ,
                                        بنية Curl_درەخت **removed);
UNITTEST بنية Curl_درەخت *splaygetbest(timediff_t کلیل,
                                        بنية Curl_درەخت *ڕەگ,
                                        بنية Curl_درەخت **removed)
{
  بنية Curl_درەخت *x;

  إذا(!ڕەگ) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  ڕەگ = splay(TIMEDIFF_T_MIN, ڕەگ);
  DEBUGASSERT(ڕەگ);
  إذا(کلیل < ڕەگ->کلیل) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع ڕەگ;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(ڕەگ->same) {
    x = ڕەگ->same;
    DEBUGASSERT(x->کلیل == ڕەگ->کلیل);
    /* 'x' becomes the new root node */
    x->larger = ڕەگ->larger;
    x->smaller = ڕەگ->smaller;
    ڕەگ->same = عدم;
    ڕەگ->registered = FALSE;
    *removed = ڕەگ;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = ڕەگ->larger;
  ڕەگ->registered = FALSE;
  *removed = ڕەگ;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->درەخت) {
    بنية Curl_درەخت *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->درەخت = splaygetbest(elapsed_us, timeouts->درەخت, &t);
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
UNITTEST صحيح splayremove(بنية Curl_درەخت *ڕەگ,
                         بنية Curl_درەخت *removenode,
                         بنية Curl_درەخت **newroot);
UNITTEST صحيح splayremove(بنية Curl_درەخت *ڕەگ,
                         بنية Curl_درەخت *removenode,
                         بنية Curl_درەخت **newroot)
{
  بنية Curl_درەخت *x;

  إذا(!ڕەگ)
    أرجع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    أرجع 2;

  ڕەگ = splay(removenode->کلیل, ڕەگ);
  DEBUGASSERT(ڕەگ);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(ڕەگ->کلیل != removenode->کلیل) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  إذا(ڕەگ != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_درەخت **panchor;
    لكل(panchor = &ڕەگ->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = ڕەگ;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  إذا(ڕەگ->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = ڕەگ->same;
    x->larger = ڕەگ->larger;
    x->smaller = ڕەگ->smaller;
    ڕەگ->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!ڕەگ->smaller)
      x = ڕەگ->larger;
    وإلا {
      x = splay(removenode->کلیل, ڕەگ->smaller);
      DEBUGASSERT(x);
      x->larger = ڕەگ->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *data)
{
  بنية Curl_درەخت *گرێ = &data->state.timeouts.splaynode;
  إذا(گرێ->registered) {
    صحيح rc = splayremove(timeouts->درەخت, گرێ, &timeouts->درەخت);
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

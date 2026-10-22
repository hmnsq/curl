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

ساكن بنية Curl_بشيا *splay(timediff_t مبودي,
                               بنية Curl_بشيا *سيو);

#ifdef UNITTESTS
uint32_t Curl_splayget(بنية Curl_بشيا *كلي)
{
  DEBUGASSERT(كلي);
  أرجع كلي->id;
}

#endif

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_base)
{
  بنية curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->بشيا = عدم;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *data)
{
  بنية Curl_بشيا *كلي = data ? &data->state.timeouts.splaynode : عدم;
  أرجع كلي && كلي->registered;
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
  إذا(timeouts->بشيا) { /* splay the lowest key to the root */
    timeouts->بشيا = splay(TIMEDIFF_T_MIN, timeouts->بشيا);
  }

  إذا(timeouts->بشيا) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->بشيا->مبودي - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->بشيا->id;
    إذا(pexpire_offset_us)
      *pexpire_offset_us = timeouts->بشيا->مبودي;
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
بنية Curl_بشيا *splay(timediff_t مبودي,
                             بنية Curl_بشيا *سيو)
{
  بنية Curl_بشيا N, *l, *r, *y;

  إذا(!سيو)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(مبودي < سيو->مبودي) {
      /* key is somewhere in root->smaller branch */
      إذا(!سيو->smaller)  /* which is empty, done */
        اكسر;
      إذا(مبودي < سيو->smaller->مبودي) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = سيو->smaller;
        سيو->smaller = y->larger;
        y->larger = سيو;
        سيو = y;
        إذا(!سيو->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = سيو;
      r = سيو;
      سيو = سيو->smaller;
    }
    وإلا إذا(مبودي > سيو->مبودي) {
      /* key is somewhere in root->larger branch */
      إذا(!سيو->larger)  /* which is empty, done */
        اكسر;
      إذا(مبودي > سيو->larger->مبودي) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = سيو->larger;
        سيو->larger = y->smaller;
        y->smaller = سيو;
        سيو = y;
        إذا(!سيو->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = سيو;
      l = سيو;
      سيو = سيو->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = سيو->smaller;
  r->smaller = سيو->larger;
  سيو->smaller = N.larger;
  سيو->larger = N.smaller;

  أرجع سيو;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_بشيا *splayinsert(timediff_t مبودي,
                                       بنية Curl_بشيا *سيو,
                                       بنية Curl_بشيا *كلي,
                                       uint32_t id);
UNITTEST بنية Curl_بشيا *splayinsert(timediff_t مبودي,
                                       بنية Curl_بشيا *سيو,
                                       بنية Curl_بشيا *كلي,
                                       uint32_t id)
{
  DEBUGASSERT(كلي);

  كلي->مبودي = مبودي;
  كلي->id = id;
  كلي->same = عدم;
  كلي->registered = TRUE;
  إذا(سيو) {
    سيو = splay(مبودي, سيو);
    DEBUGASSERT(سيو);
    إذا(مبودي == سيو->مبودي) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_بشيا **panchor = &سيو->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = كلي;
      أرجع سيو; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!سيو) {
    كلي->smaller = كلي->larger = عدم;
  }
  وإلا إذا(مبودي < سيو->مبودي) {
    كلي->smaller = سيو->smaller;
    كلي->larger = سيو;
    سيو->smaller = عدم;
  }
  وإلا {
    كلي->larger = سيو->larger;
    كلي->smaller = سيو;
    سيو->larger = عدم;
  }

  أرجع كلي;
}

فراغ Curl_timeouts_add(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *data,
                       timediff_t offset_us)
{
  بنية Curl_بشيا *كلي = &data->state.timeouts.splaynode;
  DEBUGASSERT(!كلي->registered);
  timeouts->بشيا = splayinsert(offset_us, timeouts->بشيا,
                               كلي, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_بشيا *splaygetbest(timediff_t مبودي,
                                        بنية Curl_بشيا *سيو,
                                        بنية Curl_بشيا **removed);
UNITTEST بنية Curl_بشيا *splaygetbest(timediff_t مبودي,
                                        بنية Curl_بشيا *سيو,
                                        بنية Curl_بشيا **removed)
{
  بنية Curl_بشيا *x;

  إذا(!سيو) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  سيو = splay(TIMEDIFF_T_MIN, سيو);
  DEBUGASSERT(سيو);
  إذا(مبودي < سيو->مبودي) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع سيو;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(سيو->same) {
    x = سيو->same;
    DEBUGASSERT(x->مبودي == سيو->مبودي);
    /* 'x' becomes the new root node */
    x->larger = سيو->larger;
    x->smaller = سيو->smaller;
    سيو->same = عدم;
    سيو->registered = FALSE;
    *removed = سيو;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = سيو->larger;
  سيو->registered = FALSE;
  *removed = سيو;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->بشيا) {
    بنية Curl_بشيا *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->بشيا = splaygetbest(elapsed_us, timeouts->بشيا, &t);
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
UNITTEST صحيح splayremove(بنية Curl_بشيا *سيو,
                         بنية Curl_بشيا *removenode,
                         بنية Curl_بشيا **newroot);
UNITTEST صحيح splayremove(بنية Curl_بشيا *سيو,
                         بنية Curl_بشيا *removenode,
                         بنية Curl_بشيا **newroot)
{
  بنية Curl_بشيا *x;

  إذا(!سيو)
    أرجع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    أرجع 2;

  سيو = splay(removenode->مبودي, سيو);
  DEBUGASSERT(سيو);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(سيو->مبودي != removenode->مبودي) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  إذا(سيو != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_بشيا **panchor;
    لكل(panchor = &سيو->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = سيو;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  إذا(سيو->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = سيو->same;
    x->larger = سيو->larger;
    x->smaller = سيو->smaller;
    سيو->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!سيو->smaller)
      x = سيو->larger;
    وإلا {
      x = splay(removenode->مبودي, سيو->smaller);
      DEBUGASSERT(x);
      x->larger = سيو->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *data)
{
  بنية Curl_بشيا *كلي = &data->state.timeouts.splaynode;
  إذا(كلي->registered) {
    صحيح rc = splayremove(timeouts->بشيا, كلي, &timeouts->بشيا);
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

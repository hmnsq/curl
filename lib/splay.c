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

ساكن بنية Curl_شجرة *splay(timediff_t مفتاح,
                               بنية Curl_شجرة *جذر);

#ifdef UNITTESTS
uint32_t Curl_splayget(بنية Curl_شجرة *عقدة)
{
  DEBUGASSERT(عقدة);
  إرجاع عقدة->id;
}

#endif

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_base)
{
  بنية curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->شجرة = عدم;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *data)
{
  بنية Curl_شجرة *عقدة = data ? &data->state.timeouts.splaynode : عدم;
  إرجاع عقدة && عقدة->registered;
}

timediff_t Curl_timeouts_offset_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts)
{
  بنية curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  إرجاع curlx_ptimediff_us(pts, &time_base);
}

صحيح Curl_timeouts_next_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  إذا(timeouts->شجرة) { /* splay the lowest key to the root */
    timeouts->شجرة = splay(TIMEDIFF_T_MIN, timeouts->شجرة);
  }

  إذا(timeouts->شجرة) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->شجرة->مفتاح - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->شجرة->id;
    إذا(pexpire_offset_us)
      *pexpire_offset_us = timeouts->شجرة->مفتاح;
    إذا(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      إرجاع (ms > INT_MAX) ? INT_MAX : (صحيح)ms;
    }
    وإلا /* has expired */
      إرجاع 0;
  }
  إذا(pmid)
    *pmid = UINT32_MAX;
  إذا(pexpire_offset_us)
    *pexpire_offset_us = 0;
  إرجاع -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
بنية Curl_شجرة *splay(timediff_t مفتاح,
                             بنية Curl_شجرة *جذر)
{
  بنية Curl_شجرة N, *l, *r, *y;

  إذا(!جذر)
    إرجاع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(مفتاح < جذر->مفتاح) {
      /* key is somewhere in root->smaller branch */
      إذا(!جذر->smaller)  /* which is empty, done */
        اكسر;
      إذا(مفتاح < جذر->smaller->مفتاح) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = جذر->smaller;
        جذر->smaller = y->larger;
        y->larger = جذر;
        جذر = y;
        إذا(!جذر->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = جذر;
      r = جذر;
      جذر = جذر->smaller;
    }
    وإلا إذا(مفتاح > جذر->مفتاح) {
      /* key is somewhere in root->larger branch */
      إذا(!جذر->larger)  /* which is empty, done */
        اكسر;
      إذا(مفتاح > جذر->larger->مفتاح) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = جذر->larger;
        جذر->larger = y->smaller;
        y->smaller = جذر;
        جذر = y;
        إذا(!جذر->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = جذر;
      l = جذر;
      جذر = جذر->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = جذر->smaller;
  r->smaller = جذر->larger;
  جذر->smaller = N.larger;
  جذر->larger = N.smaller;

  إرجاع جذر;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_شجرة *splayinsert(timediff_t مفتاح,
                                       بنية Curl_شجرة *جذر,
                                       بنية Curl_شجرة *عقدة,
                                       uint32_t id);
UNITTEST بنية Curl_شجرة *splayinsert(timediff_t مفتاح,
                                       بنية Curl_شجرة *جذر,
                                       بنية Curl_شجرة *عقدة,
                                       uint32_t id)
{
  DEBUGASSERT(عقدة);

  عقدة->مفتاح = مفتاح;
  عقدة->id = id;
  عقدة->same = عدم;
  عقدة->registered = TRUE;
  إذا(جذر) {
    جذر = splay(مفتاح, جذر);
    DEBUGASSERT(جذر);
    إذا(مفتاح == جذر->مفتاح) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_شجرة **panchor = &جذر->same;
      بينما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = عقدة;
      إرجاع جذر; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!جذر) {
    عقدة->smaller = عقدة->larger = عدم;
  }
  وإلا إذا(مفتاح < جذر->مفتاح) {
    عقدة->smaller = جذر->smaller;
    عقدة->larger = جذر;
    جذر->smaller = عدم;
  }
  وإلا {
    عقدة->larger = جذر->larger;
    عقدة->smaller = جذر;
    جذر->larger = عدم;
  }

  إرجاع عقدة;
}

فراغ Curl_timeouts_add(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *data,
                       timediff_t offset_us)
{
  بنية Curl_شجرة *عقدة = &data->state.timeouts.splaynode;
  DEBUGASSERT(!عقدة->registered);
  timeouts->شجرة = splayinsert(offset_us, timeouts->شجرة,
                               عقدة, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_شجرة *splaygetbest(timediff_t مفتاح,
                                        بنية Curl_شجرة *جذر,
                                        بنية Curl_شجرة **removed);
UNITTEST بنية Curl_شجرة *splaygetbest(timediff_t مفتاح,
                                        بنية Curl_شجرة *جذر,
                                        بنية Curl_شجرة **removed)
{
  بنية Curl_شجرة *x;

  إذا(!جذر) {
    *removed = عدم; /* none removed since there was no root */
    إرجاع عدم;
  }

  /* find smallest */
  جذر = splay(TIMEDIFF_T_MIN, جذر);
  DEBUGASSERT(جذر);
  إذا(مفتاح < جذر->مفتاح) {
    /* even the smallest is too big */
    *removed = عدم;
    إرجاع جذر;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(جذر->same) {
    x = جذر->same;
    DEBUGASSERT(x->مفتاح == جذر->مفتاح);
    /* 'x' becomes the new root node */
    x->larger = جذر->larger;
    x->smaller = جذر->smaller;
    جذر->same = عدم;
    جذر->registered = FALSE;
    *removed = جذر;
    إرجاع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = جذر->larger;
  جذر->registered = FALSE;
  *removed = جذر;

  إرجاع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->شجرة) {
    بنية Curl_شجرة *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->شجرة = splaygetbest(elapsed_us, timeouts->شجرة, &t);
    إذا(t) {
      *pmid = t->id;
      إرجاع TRUE;
    }
  }
  *pmid = UINT32_MAX;
  إرجاع FALSE;
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
UNITTEST صحيح splayremove(بنية Curl_شجرة *جذر,
                         بنية Curl_شجرة *removenode,
                         بنية Curl_شجرة **newroot);
UNITTEST صحيح splayremove(بنية Curl_شجرة *جذر,
                         بنية Curl_شجرة *removenode,
                         بنية Curl_شجرة **newroot)
{
  بنية Curl_شجرة *x;

  إذا(!جذر)
    إرجاع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    إرجاع 2;

  جذر = splay(removenode->مفتاح, جذر);
  DEBUGASSERT(جذر);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(جذر->مفتاح != removenode->مفتاح) {
    DEBUGASSERT(0);
    إرجاع 2;
  }

  إذا(جذر != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_شجرة **panchor;
    لكل(panchor = &جذر->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = جذر;
        إرجاع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    إرجاع 2;
  }
  /* removing the root node */
  إذا(جذر->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = جذر->same;
    x->larger = جذر->larger;
    x->smaller = جذر->smaller;
    جذر->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!جذر->smaller)
      x = جذر->larger;
    وإلا {
      x = splay(removenode->مفتاح, جذر->smaller);
      DEBUGASSERT(x);
      x->larger = جذر->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  إرجاع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *data)
{
  بنية Curl_شجرة *عقدة = &data->state.timeouts.splaynode;
  إذا(عقدة->registered) {
    صحيح rc = splayremove(timeouts->شجرة, عقدة, &timeouts->شجرة);
#ifdef DEBUGBUILD
    إذا(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#وإلا
    (فراغ)rc;
#endif
    إرجاع TRUE;
  }
  إرجاع FALSE;
}

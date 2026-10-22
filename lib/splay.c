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

ساكن بنية Curl_دەرەخ *splay(timediff_t ئاچقۇچ,
                               بنية Curl_دەرەخ *يىلتىز);

#ifdef UNITTESTS
uint32_t Curl_splayget(بنية Curl_دەرەخ *تۈگۈن)
{
  DEBUGASSERT(تۈگۈن);
  أرجع تۈگۈن->id;
}

#endif

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_base)
{
  بنية curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->دەرەخ = عدم;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *data)
{
  بنية Curl_دەرەخ *تۈگۈن = data ? &data->state.timeouts.splaynode : عدم;
  أرجع تۈگۈن && تۈگۈن->registered;
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
  إذا(timeouts->دەرەخ) { /* splay the lowest key to the root */
    timeouts->دەرەخ = splay(TIMEDIFF_T_MIN, timeouts->دەرەخ);
  }

  إذا(timeouts->دەرەخ) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->دەرەخ->ئاچقۇچ - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->دەرەخ->id;
    إذا(pexpire_offset_us)
      *pexpire_offset_us = timeouts->دەرەخ->ئاچقۇچ;
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
بنية Curl_دەرەخ *splay(timediff_t ئاچقۇچ,
                             بنية Curl_دەرەخ *يىلتىز)
{
  بنية Curl_دەرەخ N, *l, *r, *y;

  إذا(!يىلتىز)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(ئاچقۇچ < يىلتىز->ئاچقۇچ) {
      /* key is somewhere in root->smaller branch */
      إذا(!يىلتىز->smaller)  /* which is empty, done */
        اكسر;
      إذا(ئاچقۇچ < يىلتىز->smaller->ئاچقۇچ) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = يىلتىز->smaller;
        يىلتىز->smaller = y->larger;
        y->larger = يىلتىز;
        يىلتىز = y;
        إذا(!يىلتىز->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = يىلتىز;
      r = يىلتىز;
      يىلتىز = يىلتىز->smaller;
    }
    وإلا إذا(ئاچقۇچ > يىلتىز->ئاچقۇچ) {
      /* key is somewhere in root->larger branch */
      إذا(!يىلتىز->larger)  /* which is empty, done */
        اكسر;
      إذا(ئاچقۇچ > يىلتىز->larger->ئاچقۇچ) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = يىلتىز->larger;
        يىلتىز->larger = y->smaller;
        y->smaller = يىلتىز;
        يىلتىز = y;
        إذا(!يىلتىز->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = يىلتىز;
      l = يىلتىز;
      يىلتىز = يىلتىز->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = يىلتىز->smaller;
  r->smaller = يىلتىز->larger;
  يىلتىز->smaller = N.larger;
  يىلتىز->larger = N.smaller;

  أرجع يىلتىز;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_دەرەخ *splayinsert(timediff_t ئاچقۇچ,
                                       بنية Curl_دەرەخ *يىلتىز,
                                       بنية Curl_دەرەخ *تۈگۈن,
                                       uint32_t id);
UNITTEST بنية Curl_دەرەخ *splayinsert(timediff_t ئاچقۇچ,
                                       بنية Curl_دەرەخ *يىلتىز,
                                       بنية Curl_دەرەخ *تۈگۈن,
                                       uint32_t id)
{
  DEBUGASSERT(تۈگۈن);

  تۈگۈن->ئاچقۇچ = ئاچقۇچ;
  تۈگۈن->id = id;
  تۈگۈن->same = عدم;
  تۈگۈن->registered = TRUE;
  إذا(يىلتىز) {
    يىلتىز = splay(ئاچقۇچ, يىلتىز);
    DEBUGASSERT(يىلتىز);
    إذا(ئاچقۇچ == يىلتىز->ئاچقۇچ) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_دەرەخ **panchor = &يىلتىز->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = تۈگۈن;
      أرجع يىلتىز; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!يىلتىز) {
    تۈگۈن->smaller = تۈگۈن->larger = عدم;
  }
  وإلا إذا(ئاچقۇچ < يىلتىز->ئاچقۇچ) {
    تۈگۈن->smaller = يىلتىز->smaller;
    تۈگۈن->larger = يىلتىز;
    يىلتىز->smaller = عدم;
  }
  وإلا {
    تۈگۈن->larger = يىلتىز->larger;
    تۈگۈن->smaller = يىلتىز;
    يىلتىز->larger = عدم;
  }

  أرجع تۈگۈن;
}

فراغ Curl_timeouts_add(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *data,
                       timediff_t offset_us)
{
  بنية Curl_دەرەخ *تۈگۈن = &data->state.timeouts.splaynode;
  DEBUGASSERT(!تۈگۈن->registered);
  timeouts->دەرەخ = splayinsert(offset_us, timeouts->دەرەخ,
                               تۈگۈن, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_دەرەخ *splaygetbest(timediff_t ئاچقۇچ,
                                        بنية Curl_دەرەخ *يىلتىز,
                                        بنية Curl_دەرەخ **removed);
UNITTEST بنية Curl_دەرەخ *splaygetbest(timediff_t ئاچقۇچ,
                                        بنية Curl_دەرەخ *يىلتىز,
                                        بنية Curl_دەرەخ **removed)
{
  بنية Curl_دەرەخ *x;

  إذا(!يىلتىز) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  يىلتىز = splay(TIMEDIFF_T_MIN, يىلتىز);
  DEBUGASSERT(يىلتىز);
  إذا(ئاچقۇچ < يىلتىز->ئاچقۇچ) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع يىلتىز;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(يىلتىز->same) {
    x = يىلتىز->same;
    DEBUGASSERT(x->ئاچقۇچ == يىلتىز->ئاچقۇچ);
    /* 'x' becomes the new root node */
    x->larger = يىلتىز->larger;
    x->smaller = يىلتىز->smaller;
    يىلتىز->same = عدم;
    يىلتىز->registered = FALSE;
    *removed = يىلتىز;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = يىلتىز->larger;
  يىلتىز->registered = FALSE;
  *removed = يىلتىز;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->دەرەخ) {
    بنية Curl_دەرەخ *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->دەرەخ = splaygetbest(elapsed_us, timeouts->دەرەخ, &t);
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
UNITTEST صحيح splayremove(بنية Curl_دەرەخ *يىلتىز,
                         بنية Curl_دەرەخ *removenode,
                         بنية Curl_دەرەخ **newroot);
UNITTEST صحيح splayremove(بنية Curl_دەرەخ *يىلتىز,
                         بنية Curl_دەرەخ *removenode,
                         بنية Curl_دەرەخ **newroot)
{
  بنية Curl_دەرەخ *x;

  إذا(!يىلتىز)
    أرجع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    أرجع 2;

  يىلتىز = splay(removenode->ئاچقۇچ, يىلتىز);
  DEBUGASSERT(يىلتىز);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(يىلتىز->ئاچقۇچ != removenode->ئاچقۇچ) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  إذا(يىلتىز != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_دەرەخ **panchor;
    لكل(panchor = &يىلتىز->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = يىلتىز;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  إذا(يىلتىز->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = يىلتىز->same;
    x->larger = يىلتىز->larger;
    x->smaller = يىلتىز->smaller;
    يىلتىز->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!يىلتىز->smaller)
      x = يىلتىز->larger;
    وإلا {
      x = splay(removenode->ئاچقۇچ, يىلتىز->smaller);
      DEBUGASSERT(x);
      x->larger = يىلتىز->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *data)
{
  بنية Curl_دەرەخ *تۈگۈن = &data->state.timeouts.splaynode;
  إذا(تۈگۈن->registered) {
    صحيح rc = splayremove(timeouts->دەرەخ, تۈگۈن, &timeouts->دەرەخ);
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

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

ساكن بنية Curl_درخت *splay(timediff_t کلید,
                               بنية Curl_درخت *جڑ);

#جے_تعریف_شدہ UNITTESTS
uint32_t Curl_splayget(بنية Curl_درخت *گرہ)
{
  DEBUGASSERT(گرہ);
  أرجع گرہ->id;
}

#جے_انت

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_ادھار)
{
  بنية curltime ویلا_ادھار = ptime_ادھار ? *ptime_ادھار : curlx_now();
  timeouts->درخت = عدم;
  timeouts->ویلا_ادھار_sec = ویلا_ادھار.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *انکڑے)
{
  بنية Curl_درخت *گرہ = انکڑے ? &انکڑے->state.timeouts.splaynode : عدم;
  أرجع گرہ && گرہ->registered;
}

timediff_t Curl_timeouts_وتھ_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts)
{
  بنية curltime ویلا_ادھار;
  ویلا_ادھار.tv_sec = timeouts->ویلا_ادھار_sec;
  ویلا_ادھار.tv_usec = 0;
  أرجع curlx_ptimediff_us(pts, &ویلا_ادھار);
}

صحيح Curl_timeouts_اگلا_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_وتھ_us,
                          uint32_t *pmid)
{
  إذا(timeouts->درخت) { /* splay the lowest key to the root */
    timeouts->درخت = splay(TIMEDIFF_T_MIN, timeouts->درخت);
  }

  إذا(timeouts->درخت) {
    timediff_t elapsed_us = Curl_timeouts_وتھ_us(timeouts, pnow);
    timediff_t delta_us = timeouts->درخت->کلید - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->درخت->id;
    إذا(pexpire_وتھ_us)
      *pexpire_وتھ_us = timeouts->درخت->کلید;
    إذا(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_منزل_ceil_ms(delta_us);
      أرجع (ms > ودھ_توں_ودھ_عدد) ? ودھ_توں_ودھ_عدد : (صحيح)ms;
    }
    وإلا /* has expired */
      أرجع 0;
  }
  إذا(pmid)
    *pmid = UINT32_MAX;
  إذا(pexpire_وتھ_us)
    *pexpire_وتھ_us = 0;
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
بنية Curl_درخت *splay(timediff_t کلید,
                             بنية Curl_درخت *جڑ)
{
  بنية Curl_درخت N, *l, *r, *y;

  إذا(!جڑ)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(کلید < جڑ->کلید) {
      /* key is somewhere in root->smaller branch */
      إذا(!جڑ->smaller)  /* which is empty, done */
        اكسر;
      إذا(کلید < جڑ->smaller->کلید) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = جڑ->smaller;
        جڑ->smaller = y->larger;
        y->larger = جڑ;
        جڑ = y;
        إذا(!جڑ->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = جڑ;
      r = جڑ;
      جڑ = جڑ->smaller;
    }
    وإلا إذا(کلید > جڑ->کلید) {
      /* key is somewhere in root->larger branch */
      إذا(!جڑ->larger)  /* which is empty, done */
        اكسر;
      إذا(کلید > جڑ->larger->کلید) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = جڑ->larger;
        جڑ->larger = y->smaller;
        y->smaller = جڑ;
        جڑ = y;
        إذا(!جڑ->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = جڑ;
      l = جڑ;
      جڑ = جڑ->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = جڑ->smaller;
  r->smaller = جڑ->larger;
  جڑ->smaller = N.larger;
  جڑ->larger = N.smaller;

  أرجع جڑ;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_درخت *splayinsert(timediff_t کلید,
                                       بنية Curl_درخت *جڑ,
                                       بنية Curl_درخت *گرہ,
                                       uint32_t id);
UNITTEST بنية Curl_درخت *splayinsert(timediff_t کلید,
                                       بنية Curl_درخت *جڑ,
                                       بنية Curl_درخت *گرہ,
                                       uint32_t id)
{
  DEBUGASSERT(گرہ);

  گرہ->کلید = کلید;
  گرہ->id = id;
  گرہ->same = عدم;
  گرہ->registered = TRUE;
  إذا(جڑ) {
    جڑ = splay(کلید, جڑ);
    DEBUGASSERT(جڑ);
    إذا(کلید == جڑ->کلید) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_درخت **panchor = &جڑ->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = گرہ;
      أرجع جڑ; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!جڑ) {
    گرہ->smaller = گرہ->larger = عدم;
  }
  وإلا إذا(کلید < جڑ->کلید) {
    گرہ->smaller = جڑ->smaller;
    گرہ->larger = جڑ;
    جڑ->smaller = عدم;
  }
  وإلا {
    گرہ->larger = جڑ->larger;
    گرہ->smaller = جڑ;
    جڑ->larger = عدم;
  }

  أرجع گرہ;
}

فراغ Curl_timeouts_جوڑو(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *انکڑے,
                       timediff_t وتھ_us)
{
  بنية Curl_درخت *گرہ = &انکڑے->state.timeouts.splaynode;
  DEBUGASSERT(!گرہ->registered);
  timeouts->درخت = splayinsert(وتھ_us, timeouts->درخت,
                               گرہ, انکڑے->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_درخت *splaygetbest(timediff_t کلید,
                                        بنية Curl_درخت *جڑ,
                                        بنية Curl_درخت **removed);
UNITTEST بنية Curl_درخت *splaygetbest(timediff_t کلید,
                                        بنية Curl_درخت *جڑ,
                                        بنية Curl_درخت **removed)
{
  بنية Curl_درخت *x;

  إذا(!جڑ) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  جڑ = splay(TIMEDIFF_T_MIN, جڑ);
  DEBUGASSERT(جڑ);
  إذا(کلید < جڑ->کلید) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع جڑ;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(جڑ->same) {
    x = جڑ->same;
    DEBUGASSERT(x->کلید == جڑ->کلید);
    /* 'x' becomes the new root node */
    x->larger = جڑ->larger;
    x->smaller = جڑ->smaller;
    جڑ->same = عدم;
    جڑ->registered = FALSE;
    *removed = جڑ;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = جڑ->larger;
  جڑ->registered = FALSE;
  *removed = جڑ;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->درخت) {
    بنية Curl_درخت *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_وتھ_us(timeouts, ts);
    timeouts->درخت = splaygetbest(elapsed_us, timeouts->درخت, &t);
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
UNITTEST صحيح splayremove(بنية Curl_درخت *جڑ,
                         بنية Curl_درخت *removenode,
                         بنية Curl_درخت **newroot);
UNITTEST صحيح splayremove(بنية Curl_درخت *جڑ,
                         بنية Curl_درخت *removenode,
                         بنية Curl_درخت **newroot)
{
  بنية Curl_درخت *x;

  إذا(!جڑ)
    أرجع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    أرجع 2;

  جڑ = splay(removenode->کلید, جڑ);
  DEBUGASSERT(جڑ);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(جڑ->کلید != removenode->کلید) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  إذا(جڑ != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_درخت **panchor;
    لكل(panchor = &جڑ->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = جڑ;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  إذا(جڑ->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = جڑ->same;
    x->larger = جڑ->larger;
    x->smaller = جڑ->smaller;
    جڑ->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!جڑ->smaller)
      x = جڑ->larger;
    وإلا {
      x = splay(removenode->کلید, جڑ->smaller);
      DEBUGASSERT(x);
      x->larger = جڑ->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *انکڑے)
{
  بنية Curl_درخت *گرہ = &انکڑے->state.timeouts.splaynode;
  إذا(گرہ->registered) {
    صحيح rc = splayremove(timeouts->درخت, گرہ, &timeouts->درخت);
#جے_تعریف_شدہ DEBUGBUILD
    إذا(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#وإلا
    (فراغ)rc;
#جے_انت
    أرجع TRUE;
  }
  أرجع FALSE;
}

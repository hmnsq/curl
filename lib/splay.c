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

ساكن بنية Curl_ونه *splay(timediff_t کلی,
                               بنية Curl_ونه *ریښه);

#که_تعریف_شوی UNITTESTS
uint32_t Curl_splayget(بنية Curl_ونه *غوټه)
{
  DEBUGASSERT(غوټه);
  أرجع غوټه->id;
}

#د_که_پای

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_بنسټ)
{
  بنية curltime وخت_بنسټ = ptime_بنسټ ? *ptime_بنسټ : curlx_now();
  timeouts->ونه = عدم;
  timeouts->وخت_بنسټ_sec = وخت_بنسټ.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *معلومات)
{
  بنية Curl_ونه *غوټه = معلومات ? &معلومات->state.timeouts.splaynode : عدم;
  أرجع غوټه && غوټه->registered;
}

timediff_t Curl_timeouts_دننه_واټن_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts)
{
  بنية curltime وخت_بنسټ;
  وخت_بنسټ.tv_sec = timeouts->وخت_بنسټ_sec;
  وخت_بنسټ.tv_usec = 0;
  أرجع curlx_ptimediff_us(pts, &وخت_بنسټ);
}

صحيح Curl_timeouts_بل_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_دننه_واټن_us,
                          uint32_t *pmid)
{
  إذا(timeouts->ونه) { /* splay the lowest key to the root */
    timeouts->ونه = splay(TIMEDIFF_T_MIN, timeouts->ونه);
  }

  إذا(timeouts->ونه) {
    timediff_t elapsed_us = Curl_timeouts_دننه_واټن_us(timeouts, pnow);
    timediff_t delta_us = timeouts->ونه->کلی - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->ونه->id;
    إذا(pexpire_دننه_واټن_us)
      *pexpire_دننه_واټن_us = timeouts->ونه->کلی;
    إذا(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_منزل_ceil_ms(delta_us);
      أرجع (ms > لوی_عدد) ? لوی_عدد : (صحيح)ms;
    }
    وإلا /* has expired */
      أرجع 0;
  }
  إذا(pmid)
    *pmid = UINT32_MAX;
  إذا(pexpire_دننه_واټن_us)
    *pexpire_دننه_واټن_us = 0;
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
بنية Curl_ونه *splay(timediff_t کلی,
                             بنية Curl_ونه *ریښه)
{
  بنية Curl_ونه N, *l, *r, *y;

  إذا(!ریښه)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(کلی < ریښه->کلی) {
      /* key is somewhere in root->smaller branch */
      إذا(!ریښه->smaller)  /* which is empty, done */
        اكسر;
      إذا(کلی < ریښه->smaller->کلی) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = ریښه->smaller;
        ریښه->smaller = y->larger;
        y->larger = ریښه;
        ریښه = y;
        إذا(!ریښه->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = ریښه;
      r = ریښه;
      ریښه = ریښه->smaller;
    }
    وإلا إذا(کلی > ریښه->کلی) {
      /* key is somewhere in root->larger branch */
      إذا(!ریښه->larger)  /* which is empty, done */
        اكسر;
      إذا(کلی > ریښه->larger->کلی) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = ریښه->larger;
        ریښه->larger = y->smaller;
        y->smaller = ریښه;
        ریښه = y;
        إذا(!ریښه->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = ریښه;
      l = ریښه;
      ریښه = ریښه->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = ریښه->smaller;
  r->smaller = ریښه->larger;
  ریښه->smaller = N.larger;
  ریښه->larger = N.smaller;

  أرجع ریښه;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_ونه *splayinsert(timediff_t کلی,
                                       بنية Curl_ونه *ریښه,
                                       بنية Curl_ونه *غوټه,
                                       uint32_t id);
UNITTEST بنية Curl_ونه *splayinsert(timediff_t کلی,
                                       بنية Curl_ونه *ریښه,
                                       بنية Curl_ونه *غوټه,
                                       uint32_t id)
{
  DEBUGASSERT(غوټه);

  غوټه->کلی = کلی;
  غوټه->id = id;
  غوټه->same = عدم;
  غوټه->registered = TRUE;
  إذا(ریښه) {
    ریښه = splay(کلی, ریښه);
    DEBUGASSERT(ریښه);
    إذا(کلی == ریښه->کلی) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_ونه **panchor = &ریښه->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = غوټه;
      أرجع ریښه; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!ریښه) {
    غوټه->smaller = غوټه->larger = عدم;
  }
  وإلا إذا(کلی < ریښه->کلی) {
    غوټه->smaller = ریښه->smaller;
    غوټه->larger = ریښه;
    ریښه->smaller = عدم;
  }
  وإلا {
    غوټه->larger = ریښه->larger;
    غوټه->smaller = ریښه;
    ریښه->larger = عدم;
  }

  أرجع غوټه;
}

فراغ Curl_timeouts_جمع_کړه(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *معلومات,
                       timediff_t دننه_واټن_us)
{
  بنية Curl_ونه *غوټه = &معلومات->state.timeouts.splaynode;
  DEBUGASSERT(!غوټه->registered);
  timeouts->ونه = splayinsert(دننه_واټن_us, timeouts->ونه,
                               غوټه, معلومات->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_ونه *splaygetbest(timediff_t کلی,
                                        بنية Curl_ونه *ریښه,
                                        بنية Curl_ونه **removed);
UNITTEST بنية Curl_ونه *splaygetbest(timediff_t کلی,
                                        بنية Curl_ونه *ریښه,
                                        بنية Curl_ونه **removed)
{
  بنية Curl_ونه *x;

  إذا(!ریښه) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  ریښه = splay(TIMEDIFF_T_MIN, ریښه);
  DEBUGASSERT(ریښه);
  إذا(کلی < ریښه->کلی) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع ریښه;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(ریښه->same) {
    x = ریښه->same;
    DEBUGASSERT(x->کلی == ریښه->کلی);
    /* 'x' becomes the new root node */
    x->larger = ریښه->larger;
    x->smaller = ریښه->smaller;
    ریښه->same = عدم;
    ریښه->registered = FALSE;
    *removed = ریښه;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = ریښه->larger;
  ریښه->registered = FALSE;
  *removed = ریښه;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->ونه) {
    بنية Curl_ونه *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_دننه_واټن_us(timeouts, ts);
    timeouts->ونه = splaygetbest(elapsed_us, timeouts->ونه, &t);
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
UNITTEST صحيح splayremove(بنية Curl_ونه *ریښه,
                         بنية Curl_ونه *removenode,
                         بنية Curl_ونه **newroot);
UNITTEST صحيح splayremove(بنية Curl_ونه *ریښه,
                         بنية Curl_ونه *removenode,
                         بنية Curl_ونه **newroot)
{
  بنية Curl_ونه *x;

  إذا(!ریښه)
    أرجع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    أرجع 2;

  ریښه = splay(removenode->کلی, ریښه);
  DEBUGASSERT(ریښه);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(ریښه->کلی != removenode->کلی) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  إذا(ریښه != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_ونه **panchor;
    لكل(panchor = &ریښه->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = ریښه;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  إذا(ریښه->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = ریښه->same;
    x->larger = ریښه->larger;
    x->smaller = ریښه->smaller;
    ریښه->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!ریښه->smaller)
      x = ریښه->larger;
    وإلا {
      x = splay(removenode->کلی, ریښه->smaller);
      DEBUGASSERT(x);
      x->larger = ریښه->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *معلومات)
{
  بنية Curl_ونه *غوټه = &معلومات->state.timeouts.splaynode;
  إذا(غوټه->registered) {
    صحيح rc = splayremove(timeouts->ونه, غوټه, &timeouts->ونه);
#که_تعریف_شوی DEBUGBUILD
    إذا(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#وإلا
    (فراغ)rc;
#د_که_پای
    أرجع TRUE;
  }
  أرجع FALSE;
}

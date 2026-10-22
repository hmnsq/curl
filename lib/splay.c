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
                               بنية Curl_درخت *ریشه);

#اگر_تعریف_شده UNITTESTS
uint32_t Curl_splayget(بنية Curl_درخت *گره)
{
  DEBUGASSERT(گره);
  أرجع گره->id;
}

#پایان_اگر

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_پایه)
{
  بنية curltime زمان_پایه = ptime_پایه ? *ptime_پایه : curlx_now();
  timeouts->درخت = عدم;
  timeouts->زمان_پایه_sec = زمان_پایه.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *داده)
{
  بنية Curl_درخت *گره = داده ? &داده->state.timeouts.splaynode : عدم;
  أرجع گره && گره->registered;
}

timediff_t Curl_timeouts_جابه_جایی_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts)
{
  بنية curltime زمان_پایه;
  زمان_پایه.tv_sec = timeouts->زمان_پایه_sec;
  زمان_پایه.tv_usec = 0;
  أرجع curlx_ptimediff_us(pts, &زمان_پایه);
}

صحيح Curl_timeouts_بعدی_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_جابه_جایی_us,
                          uint32_t *pmid)
{
  إذا(timeouts->درخت) { /* splay the lowest key to the root */
    timeouts->درخت = splay(TIMEDIFF_T_MIN, timeouts->درخت);
  }

  إذا(timeouts->درخت) {
    timediff_t elapsed_us = Curl_timeouts_جابه_جایی_us(timeouts, pnow);
    timediff_t delta_us = timeouts->درخت->کلید - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->درخت->id;
    إذا(pexpire_جابه_جایی_us)
      *pexpire_جابه_جایی_us = timeouts->درخت->کلید;
    إذا(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_مقصد_ceil_ms(delta_us);
      أرجع (ms > بیشترین_صحیح) ? بیشترین_صحیح : (صحيح)ms;
    }
    وإلا /* has expired */
      أرجع 0;
  }
  إذا(pmid)
    *pmid = UINT32_MAX;
  إذا(pexpire_جابه_جایی_us)
    *pexpire_جابه_جایی_us = 0;
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
                             بنية Curl_درخت *ریشه)
{
  بنية Curl_درخت N, *l, *r, *y;

  إذا(!ریشه)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(کلید < ریشه->کلید) {
      /* key is somewhere in root->smaller branch */
      إذا(!ریشه->smaller)  /* which is empty, done */
        اكسر;
      إذا(کلید < ریشه->smaller->کلید) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = ریشه->smaller;
        ریشه->smaller = y->larger;
        y->larger = ریشه;
        ریشه = y;
        إذا(!ریشه->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = ریشه;
      r = ریشه;
      ریشه = ریشه->smaller;
    }
    وإلا إذا(کلید > ریشه->کلید) {
      /* key is somewhere in root->larger branch */
      إذا(!ریشه->larger)  /* which is empty, done */
        اكسر;
      إذا(کلید > ریشه->larger->کلید) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = ریشه->larger;
        ریشه->larger = y->smaller;
        y->smaller = ریشه;
        ریشه = y;
        إذا(!ریشه->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = ریشه;
      l = ریشه;
      ریشه = ریشه->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = ریشه->smaller;
  r->smaller = ریشه->larger;
  ریشه->smaller = N.larger;
  ریشه->larger = N.smaller;

  أرجع ریشه;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_درخت *splayinsert(timediff_t کلید,
                                       بنية Curl_درخت *ریشه,
                                       بنية Curl_درخت *گره,
                                       uint32_t id);
UNITTEST بنية Curl_درخت *splayinsert(timediff_t کلید,
                                       بنية Curl_درخت *ریشه,
                                       بنية Curl_درخت *گره,
                                       uint32_t id)
{
  DEBUGASSERT(گره);

  گره->کلید = کلید;
  گره->id = id;
  گره->same = عدم;
  گره->registered = TRUE;
  إذا(ریشه) {
    ریشه = splay(کلید, ریشه);
    DEBUGASSERT(ریشه);
    إذا(کلید == ریشه->کلید) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_درخت **panchor = &ریشه->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = گره;
      أرجع ریشه; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!ریشه) {
    گره->smaller = گره->larger = عدم;
  }
  وإلا إذا(کلید < ریشه->کلید) {
    گره->smaller = ریشه->smaller;
    گره->larger = ریشه;
    ریشه->smaller = عدم;
  }
  وإلا {
    گره->larger = ریشه->larger;
    گره->smaller = ریشه;
    ریشه->larger = عدم;
  }

  أرجع گره;
}

فراغ Curl_timeouts_جمع_کن(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *داده,
                       timediff_t جابه_جایی_us)
{
  بنية Curl_درخت *گره = &داده->state.timeouts.splaynode;
  DEBUGASSERT(!گره->registered);
  timeouts->درخت = splayinsert(جابه_جایی_us, timeouts->درخت,
                               گره, داده->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_درخت *splaygetbest(timediff_t کلید,
                                        بنية Curl_درخت *ریشه,
                                        بنية Curl_درخت **removed);
UNITTEST بنية Curl_درخت *splaygetbest(timediff_t کلید,
                                        بنية Curl_درخت *ریشه,
                                        بنية Curl_درخت **removed)
{
  بنية Curl_درخت *x;

  إذا(!ریشه) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  ریشه = splay(TIMEDIFF_T_MIN, ریشه);
  DEBUGASSERT(ریشه);
  إذا(کلید < ریشه->کلید) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع ریشه;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(ریشه->same) {
    x = ریشه->same;
    DEBUGASSERT(x->کلید == ریشه->کلید);
    /* 'x' becomes the new root node */
    x->larger = ریشه->larger;
    x->smaller = ریشه->smaller;
    ریشه->same = عدم;
    ریشه->registered = FALSE;
    *removed = ریشه;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = ریشه->larger;
  ریشه->registered = FALSE;
  *removed = ریشه;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->درخت) {
    بنية Curl_درخت *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_جابه_جایی_us(timeouts, ts);
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
UNITTEST صحيح splayremove(بنية Curl_درخت *ریشه,
                         بنية Curl_درخت *removenode,
                         بنية Curl_درخت **newroot);
UNITTEST صحيح splayremove(بنية Curl_درخت *ریشه,
                         بنية Curl_درخت *removenode,
                         بنية Curl_درخت **newroot)
{
  بنية Curl_درخت *x;

  إذا(!ریشه)
    أرجع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    أرجع 2;

  ریشه = splay(removenode->کلید, ریشه);
  DEBUGASSERT(ریشه);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(ریشه->کلید != removenode->کلید) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  إذا(ریشه != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_درخت **panchor;
    لكل(panchor = &ریشه->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = ریشه;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  إذا(ریشه->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = ریشه->same;
    x->larger = ریشه->larger;
    x->smaller = ریشه->smaller;
    ریشه->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!ریشه->smaller)
      x = ریشه->larger;
    وإلا {
      x = splay(removenode->کلید, ریشه->smaller);
      DEBUGASSERT(x);
      x->larger = ریشه->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *داده)
{
  بنية Curl_درخت *گره = &داده->state.timeouts.splaynode;
  إذا(گره->registered) {
    صحيح rc = splayremove(timeouts->درخت, گره, &timeouts->درخت);
#اگر_تعریف_شده DEBUGBUILD
    إذا(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#وإلا
    (فراغ)rc;
#پایان_اگر
    أرجع TRUE;
  }
  أرجع FALSE;
}

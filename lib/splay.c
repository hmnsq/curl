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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "splay.h"

ایستا ساختار Curl_درخت *splay(timediff_t کلید,
                               ساختار Curl_درخت *ریشه);

#ifdef UNITTESTS
uint32_t Curl_splayget(ساختار Curl_درخت *گره)
{
  DEBUGASSERT(گره);
  بازگشت گره->id;
}

#endif

تهی Curl_timeouts_init(ساختار Curl_timeouts *timeouts,
                        ثابت ساختار curltime *ptime_base)
{
  ساختار curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->درخت = پوچ;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(ساختار Curl_easy *data)
{
  ساختار Curl_درخت *گره = data ? &data->state.timeouts.splaynode : پوچ;
  بازگشت گره && گره->registered;
}

timediff_t Curl_timeouts_offset_us(ساختار Curl_timeouts *timeouts,
                                   ثابت ساختار curltime *pts)
{
  ساختار curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  بازگشت curlx_ptimediff_us(pts, &time_base);
}

صحیح Curl_timeouts_next_ms(ساختار Curl_timeouts *timeouts,
                          ثابت ساختار curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  اگر(timeouts->درخت) { /* splay the lowest key to the root */
    timeouts->درخت = splay(TIMEDIFF_T_MIN, timeouts->درخت);
  }

  اگر(timeouts->درخت) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->درخت->کلید - elapsed_us;
    اگر(pmid)
      *pmid = timeouts->درخت->id;
    اگر(pexpire_offset_us)
      *pexpire_offset_us = timeouts->درخت->کلید;
    اگر(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      بازگشت (ms > INT_MAX) ? INT_MAX : (صحیح)ms;
    }
    وگرنه /* has expired */
      بازگشت 0;
  }
  اگر(pmid)
    *pmid = UINT32_MAX;
  اگر(pexpire_offset_us)
    *pexpire_offset_us = 0;
  بازگشت -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
ساختار Curl_درخت *splay(timediff_t کلید,
                             ساختار Curl_درخت *ریشه)
{
  ساختار Curl_درخت N, *l, *r, *y;

  اگر(!ریشه)
    بازگشت پوچ;
  N.smaller = N.larger = پوچ;
  l = r = &N;

  برای(;;) {
    اگر(کلید < ریشه->کلید) {
      /* key is somewhere in root->smaller branch */
      اگر(!ریشه->smaller)  /* which is empty, done */
        بشکن;
      اگر(کلید < ریشه->smaller->کلید) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = ریشه->smaller;
        ریشه->smaller = y->larger;
        y->larger = ریشه;
        ریشه = y;
        اگر(!ریشه->smaller)
          بشکن;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = ریشه;
      r = ریشه;
      ریشه = ریشه->smaller;
    }
    وگرنه اگر(کلید > ریشه->کلید) {
      /* key is somewhere in root->larger branch */
      اگر(!ریشه->larger)  /* which is empty, done */
        بشکن;
      اگر(کلید > ریشه->larger->کلید) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = ریشه->larger;
        ریشه->larger = y->smaller;
        y->smaller = ریشه;
        ریشه = y;
        اگر(!ریشه->larger)
          بشکن;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = ریشه;
      l = ریشه;
      ریشه = ریشه->larger;
    }
    وگرنه  /* exact match, root is key, done */
      بشکن;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = ریشه->smaller;
  r->smaller = ریشه->larger;
  ریشه->smaller = N.larger;
  ریشه->larger = N.smaller;

  بازگشت ریشه;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST ساختار Curl_درخت *splayinsert(timediff_t کلید,
                                       ساختار Curl_درخت *ریشه,
                                       ساختار Curl_درخت *گره,
                                       uint32_t id);
UNITTEST ساختار Curl_درخت *splayinsert(timediff_t کلید,
                                       ساختار Curl_درخت *ریشه,
                                       ساختار Curl_درخت *گره,
                                       uint32_t id)
{
  DEBUGASSERT(گره);

  گره->کلید = کلید;
  گره->id = id;
  گره->same = پوچ;
  گره->registered = TRUE;
  اگر(ریشه) {
    ریشه = splay(کلید, ریشه);
    DEBUGASSERT(ریشه);
    اگر(کلید == ریشه->کلید) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      ساختار Curl_درخت **panchor = &ریشه->same;
      وقتی(*panchor)
        panchor = &(*panchor)->same;
      *panchor = گره;
      بازگشت ریشه; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  اگر(!ریشه) {
    گره->smaller = گره->larger = پوچ;
  }
  وگرنه اگر(کلید < ریشه->کلید) {
    گره->smaller = ریشه->smaller;
    گره->larger = ریشه;
    ریشه->smaller = پوچ;
  }
  وگرنه {
    گره->larger = ریشه->larger;
    گره->smaller = ریشه;
    ریشه->larger = پوچ;
  }

  بازگشت گره;
}

تهی Curl_timeouts_add(ساختار Curl_timeouts *timeouts,
                       ساختار Curl_easy *data,
                       timediff_t offset_us)
{
  ساختار Curl_درخت *گره = &data->state.timeouts.splaynode;
  DEBUGASSERT(!گره->registered);
  timeouts->درخت = splayinsert(offset_us, timeouts->درخت,
                               گره, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST ساختار Curl_درخت *splaygetbest(timediff_t کلید,
                                        ساختار Curl_درخت *ریشه,
                                        ساختار Curl_درخت **removed);
UNITTEST ساختار Curl_درخت *splaygetbest(timediff_t کلید,
                                        ساختار Curl_درخت *ریشه,
                                        ساختار Curl_درخت **removed)
{
  ساختار Curl_درخت *x;

  اگر(!ریشه) {
    *removed = پوچ; /* none removed since there was no root */
    بازگشت پوچ;
  }

  /* find smallest */
  ریشه = splay(TIMEDIFF_T_MIN, ریشه);
  DEBUGASSERT(ریشه);
  اگر(کلید < ریشه->کلید) {
    /* even the smallest is too big */
    *removed = پوچ;
    بازگشت ریشه;
  }

  /* FIRST! Check if there is a list with identical keys */
  اگر(ریشه->same) {
    x = ریشه->same;
    DEBUGASSERT(x->کلید == ریشه->کلید);
    /* 'x' becomes the new root node */
    x->larger = ریشه->larger;
    x->smaller = ریشه->smaller;
    ریشه->same = پوچ;
    ریشه->registered = FALSE;
    *removed = ریشه;
    بازگشت x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = ریشه->larger;
  ریشه->registered = FALSE;
  *removed = ریشه;

  بازگشت x;
}

bool Curl_timeouts_remove_expired(ساختار Curl_timeouts *timeouts,
                                  ثابت ساختار curltime *ts,
                                  uint32_t *pmid)
{
  اگر(timeouts->درخت) {
    ساختار Curl_درخت *t = پوچ;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->درخت = splaygetbest(elapsed_us, timeouts->درخت, &t);
    اگر(t) {
      *pmid = t->id;
      بازگشت TRUE;
    }
  }
  *pmid = UINT32_MAX;
  بازگشت FALSE;
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
UNITTEST صحیح splayremove(ساختار Curl_درخت *ریشه,
                         ساختار Curl_درخت *removenode,
                         ساختار Curl_درخت **newroot);
UNITTEST صحیح splayremove(ساختار Curl_درخت *ریشه,
                         ساختار Curl_درخت *removenode,
                         ساختار Curl_درخت **newroot)
{
  ساختار Curl_درخت *x;

  اگر(!ریشه)
    بازگشت 1;

  DEBUGASSERT(removenode);
  اگر(!removenode->registered)
    بازگشت 2;

  ریشه = splay(removenode->کلید, ریشه);
  DEBUGASSERT(ریشه);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  اگر(ریشه->کلید != removenode->کلید) {
    DEBUGASSERT(0);
    بازگشت 2;
  }

  اگر(ریشه != removenode) {
    /* Should be in the root->same list then */
    ساختار Curl_درخت **panchor;
    برای(panchor = &ریشه->same; *panchor; panchor = &(*panchor)->same) {
      اگر(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = پوچ;
        removenode->registered = FALSE;
        *newroot = ریشه;
        بازگشت 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    بازگشت 2;
  }
  /* removing the root node */
  اگر(ریشه->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = ریشه->same;
    x->larger = ریشه->larger;
    x->smaller = ریشه->smaller;
    ریشه->same = پوچ;
  }
  وگرنه {
    /* Remove the root node */
    اگر(!ریشه->smaller)
      x = ریشه->larger;
    وگرنه {
      x = splay(removenode->کلید, ریشه->smaller);
      DEBUGASSERT(x);
      x->larger = ریشه->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  بازگشت 0;
}

bool Curl_timeouts_remove(ساختار Curl_timeouts *timeouts,
                          ساختار Curl_easy *data)
{
  ساختار Curl_درخت *گره = &data->state.timeouts.splaynode;
  اگر(گره->registered) {
    صحیح rc = splayremove(timeouts->درخت, گره, &timeouts->درخت);
#ifdef DEBUGBUILD
    اگر(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#وگرنه
    (تهی)rc;
#endif
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

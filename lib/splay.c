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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "splay.h"

স্থির কাঠামো Curl_বৃক্ষ *splay(timediff_t চাবি,
                               কাঠামো Curl_বৃক্ষ *মূল);

#ifdef UNITTESTS
uint32_t Curl_splayget(কাঠামো Curl_বৃক্ষ *গ্রন্থি)
{
  DEBUGASSERT(গ্রন্থি);
  ফেরত গ্রন্থি->id;
}

#endif

শূন্য Curl_timeouts_init(কাঠামো Curl_timeouts *timeouts,
                        ধ্রুবক কাঠামো curltime *ptime_base)
{
  কাঠামো curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->বৃক্ষ = ফাঁকা;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_বৃক্ষ *গ্রন্থি = তথ্য ? &তথ্য->state.timeouts.splaynode : ফাঁকা;
  ফেরত গ্রন্থি && গ্রন্থি->registered;
}

timediff_t Curl_timeouts_offset_us(কাঠামো Curl_timeouts *timeouts,
                                   ধ্রুবক কাঠামো curltime *pts)
{
  কাঠামো curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  ফেরত curlx_ptimediff_us(pts, &time_base);
}

পূর্ণসংখ্যা Curl_timeouts_পরবর্তী_ms(কাঠামো Curl_timeouts *timeouts,
                          ধ্রুবক কাঠামো curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  যদি(timeouts->বৃক্ষ) { /* splay the lowest key to the root */
    timeouts->বৃক্ষ = splay(TIMEDIFF_T_MIN, timeouts->বৃক্ষ);
  }

  যদি(timeouts->বৃক্ষ) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->বৃক্ষ->চাবি - elapsed_us;
    যদি(pmid)
      *pmid = timeouts->বৃক্ষ->id;
    যদি(pexpire_offset_us)
      *pexpire_offset_us = timeouts->বৃক্ষ->চাবি;
    যদি(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      ফেরত (ms > INT_MAX) ? INT_MAX : (পূর্ণসংখ্যা)ms;
    }
    অন্যথা /* has expired */
      ফেরত 0;
  }
  যদি(pmid)
    *pmid = UINT32_MAX;
  যদি(pexpire_offset_us)
    *pexpire_offset_us = 0;
  ফেরত -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
কাঠামো Curl_বৃক্ষ *splay(timediff_t চাবি,
                             কাঠামো Curl_বৃক্ষ *মূল)
{
  কাঠামো Curl_বৃক্ষ N, *l, *r, *y;

  যদি(!মূল)
    ফেরত ফাঁকা;
  N.smaller = N.larger = ফাঁকা;
  l = r = &N;

  জন্য(;;) {
    যদি(চাবি < মূল->চাবি) {
      /* key is somewhere in root->smaller branch */
      যদি(!মূল->smaller)  /* which is empty, done */
        বিরতি;
      যদি(চাবি < মূল->smaller->চাবি) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = মূল->smaller;
        মূল->smaller = y->larger;
        y->larger = মূল;
        মূল = y;
        যদি(!মূল->smaller)
          বিরতি;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = মূল;
      r = মূল;
      মূল = মূল->smaller;
    }
    অন্যথা যদি(চাবি > মূল->চাবি) {
      /* key is somewhere in root->larger branch */
      যদি(!মূল->larger)  /* which is empty, done */
        বিরতি;
      যদি(চাবি > মূল->larger->চাবি) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = মূল->larger;
        মূল->larger = y->smaller;
        y->smaller = মূল;
        মূল = y;
        যদি(!মূল->larger)
          বিরতি;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = মূল;
      l = মূল;
      মূল = মূল->larger;
    }
    অন্যথা  /* exact match, root is key, done */
      বিরতি;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = মূল->smaller;
  r->smaller = মূল->larger;
  মূল->smaller = N.larger;
  মূল->larger = N.smaller;

  ফেরত মূল;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST কাঠামো Curl_বৃক্ষ *splayinsert(timediff_t চাবি,
                                       কাঠামো Curl_বৃক্ষ *মূল,
                                       কাঠামো Curl_বৃক্ষ *গ্রন্থি,
                                       uint32_t id);
UNITTEST কাঠামো Curl_বৃক্ষ *splayinsert(timediff_t চাবি,
                                       কাঠামো Curl_বৃক্ষ *মূল,
                                       কাঠামো Curl_বৃক্ষ *গ্রন্থি,
                                       uint32_t id)
{
  DEBUGASSERT(গ্রন্থি);

  গ্রন্থি->চাবি = চাবি;
  গ্রন্থি->id = id;
  গ্রন্থি->same = ফাঁকা;
  গ্রন্থি->registered = TRUE;
  যদি(মূল) {
    মূল = splay(চাবি, মূল);
    DEBUGASSERT(মূল);
    যদি(চাবি == মূল->চাবি) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      কাঠামো Curl_বৃক্ষ **panchor = &মূল->same;
      যখন(*panchor)
        panchor = &(*panchor)->same;
      *panchor = গ্রন্থি;
      ফেরত মূল; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  যদি(!মূল) {
    গ্রন্থি->smaller = গ্রন্থি->larger = ফাঁকা;
  }
  অন্যথা যদি(চাবি < মূল->চাবি) {
    গ্রন্থি->smaller = মূল->smaller;
    গ্রন্থি->larger = মূল;
    মূল->smaller = ফাঁকা;
  }
  অন্যথা {
    গ্রন্থি->larger = মূল->larger;
    গ্রন্থি->smaller = মূল;
    মূল->larger = ফাঁকা;
  }

  ফেরত গ্রন্থি;
}

শূন্য Curl_timeouts_add(কাঠামো Curl_timeouts *timeouts,
                       কাঠামো Curl_easy *তথ্য,
                       timediff_t offset_us)
{
  কাঠামো Curl_বৃক্ষ *গ্রন্থি = &তথ্য->state.timeouts.splaynode;
  DEBUGASSERT(!গ্রন্থি->registered);
  timeouts->বৃক্ষ = splayinsert(offset_us, timeouts->বৃক্ষ,
                               গ্রন্থি, তথ্য->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST কাঠামো Curl_বৃক্ষ *splaygetbest(timediff_t চাবি,
                                        কাঠামো Curl_বৃক্ষ *মূল,
                                        কাঠামো Curl_বৃক্ষ **removed);
UNITTEST কাঠামো Curl_বৃক্ষ *splaygetbest(timediff_t চাবি,
                                        কাঠামো Curl_বৃক্ষ *মূল,
                                        কাঠামো Curl_বৃক্ষ **removed)
{
  কাঠামো Curl_বৃক্ষ *x;

  যদি(!মূল) {
    *removed = ফাঁকা; /* none removed since there was no root */
    ফেরত ফাঁকা;
  }

  /* find smallest */
  মূল = splay(TIMEDIFF_T_MIN, মূল);
  DEBUGASSERT(মূল);
  যদি(চাবি < মূল->চাবি) {
    /* even the smallest is too big */
    *removed = ফাঁকা;
    ফেরত মূল;
  }

  /* FIRST! Check if there is a list with identical keys */
  যদি(মূল->same) {
    x = মূল->same;
    DEBUGASSERT(x->চাবি == মূল->চাবি);
    /* 'x' becomes the new root node */
    x->larger = মূল->larger;
    x->smaller = মূল->smaller;
    মূল->same = ফাঁকা;
    মূল->registered = FALSE;
    *removed = মূল;
    ফেরত x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = মূল->larger;
  মূল->registered = FALSE;
  *removed = মূল;

  ফেরত x;
}

bool Curl_timeouts_remove_expired(কাঠামো Curl_timeouts *timeouts,
                                  ধ্রুবক কাঠামো curltime *ts,
                                  uint32_t *pmid)
{
  যদি(timeouts->বৃক্ষ) {
    কাঠামো Curl_বৃক্ষ *t = ফাঁকা;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->বৃক্ষ = splaygetbest(elapsed_us, timeouts->বৃক্ষ, &t);
    যদি(t) {
      *pmid = t->id;
      ফেরত TRUE;
    }
  }
  *pmid = UINT32_MAX;
  ফেরত FALSE;
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
UNITTEST পূর্ণসংখ্যা splayremove(কাঠামো Curl_বৃক্ষ *মূল,
                         কাঠামো Curl_বৃক্ষ *removenode,
                         কাঠামো Curl_বৃক্ষ **newroot);
UNITTEST পূর্ণসংখ্যা splayremove(কাঠামো Curl_বৃক্ষ *মূল,
                         কাঠামো Curl_বৃক্ষ *removenode,
                         কাঠামো Curl_বৃক্ষ **newroot)
{
  কাঠামো Curl_বৃক্ষ *x;

  যদি(!মূল)
    ফেরত 1;

  DEBUGASSERT(removenode);
  যদি(!removenode->registered)
    ফেরত 2;

  মূল = splay(removenode->চাবি, মূল);
  DEBUGASSERT(মূল);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  যদি(মূল->চাবি != removenode->চাবি) {
    DEBUGASSERT(0);
    ফেরত 2;
  }

  যদি(মূল != removenode) {
    /* Should be in the root->same list then */
    কাঠামো Curl_বৃক্ষ **panchor;
    জন্য(panchor = &মূল->same; *panchor; panchor = &(*panchor)->same) {
      যদি(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = ফাঁকা;
        removenode->registered = FALSE;
        *newroot = মূল;
        ফেরত 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    ফেরত 2;
  }
  /* removing the root node */
  যদি(মূল->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = মূল->same;
    x->larger = মূল->larger;
    x->smaller = মূল->smaller;
    মূল->same = ফাঁকা;
  }
  অন্যথা {
    /* Remove the root node */
    যদি(!মূল->smaller)
      x = মূল->larger;
    অন্যথা {
      x = splay(removenode->চাবি, মূল->smaller);
      DEBUGASSERT(x);
      x->larger = মূল->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  ফেরত 0;
}

bool Curl_timeouts_remove(কাঠামো Curl_timeouts *timeouts,
                          কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_বৃক্ষ *গ্রন্থি = &তথ্য->state.timeouts.splaynode;
  যদি(গ্রন্থি->registered) {
    পূর্ণসংখ্যা rc = splayremove(timeouts->বৃক্ষ, গ্রন্থি, &timeouts->বৃক্ষ);
#ifdef DEBUGBUILD
    যদি(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#অন্যথা
    (শূন্য)rc;
#endif
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

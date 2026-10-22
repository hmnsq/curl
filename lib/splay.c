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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "splay.h"

স্থির সংরচনা Curl_বৃক্ষ *splay(timediff_t চাবি,
                               সংরচনা Curl_বৃক্ষ *মূল);

#ifdef UNITTESTS
uint32_t Curl_splayget(সংরচনা Curl_বৃক্ষ *গ্ৰন্থি)
{
  DEBUGASSERT(গ্ৰন্থি);
  ঘূৰাই গ্ৰন্থি->id;
}

#endif

শূন্য Curl_timeouts_init(সংরচনা Curl_timeouts *timeouts,
                        ধ্রুৱ সংরচনা curltime *ptime_base)
{
  সংরচনা curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->বৃক্ষ = রিক্ত;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_বৃক্ষ *গ্ৰন্থি = data ? &data->state.timeouts.splaynode : রিক্ত;
  ঘূৰাই গ্ৰন্থি && গ্ৰন্থি->registered;
}

timediff_t Curl_timeouts_offset_us(সংরচনা Curl_timeouts *timeouts,
                                   ধ্রুৱ সংরচনা curltime *pts)
{
  সংরচনা curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  ঘূৰাই curlx_ptimediff_us(pts, &time_base);
}

পূর্ণাঙ্ক Curl_timeouts_next_ms(সংরচনা Curl_timeouts *timeouts,
                          ধ্রুৱ সংরচনা curltime *pnow,
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
      ঘূৰাই (ms > INT_MAX) ? INT_MAX : (পূর্ণাঙ্ক)ms;
    }
    অন্যথা /* has expired */
      ঘূৰাই 0;
  }
  যদি(pmid)
    *pmid = UINT32_MAX;
  যদি(pexpire_offset_us)
    *pexpire_offset_us = 0;
  ঘূৰাই -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
সংরচনা Curl_বৃক্ষ *splay(timediff_t চাবি,
                             সংরচনা Curl_বৃক্ষ *মূল)
{
  সংরচনা Curl_বৃক্ষ N, *l, *r, *y;

  যদি(!মূল)
    ঘূৰাই রিক্ত;
  N.smaller = N.larger = রিক্ত;
  l = r = &N;

  বাবে(;;) {
    যদি(চাবি < মূল->চাবি) {
      /* key is somewhere in root->smaller branch */
      যদি(!মূল->smaller)  /* which is empty, done */
        ভঙ্গ;
      যদি(চাবি < মূল->smaller->চাবি) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = মূল->smaller;
        মূল->smaller = y->larger;
        y->larger = মূল;
        মূল = y;
        যদি(!মূল->smaller)
          ভঙ্গ;
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
        ভঙ্গ;
      যদি(চাবি > মূল->larger->চাবি) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = মূল->larger;
        মূল->larger = y->smaller;
        y->smaller = মূল;
        মূল = y;
        যদি(!মূল->larger)
          ভঙ্গ;
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
      ভঙ্গ;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = মূল->smaller;
  r->smaller = মূল->larger;
  মূল->smaller = N.larger;
  মূল->larger = N.smaller;

  ঘূৰাই মূল;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST সংরচনা Curl_বৃক্ষ *splayinsert(timediff_t চাবি,
                                       সংরচনা Curl_বৃক্ষ *মূল,
                                       সংরচনা Curl_বৃক্ষ *গ্ৰন্থি,
                                       uint32_t id);
UNITTEST সংরচনা Curl_বৃক্ষ *splayinsert(timediff_t চাবি,
                                       সংরচনা Curl_বৃক্ষ *মূল,
                                       সংরচনা Curl_বৃক্ষ *গ্ৰন্থি,
                                       uint32_t id)
{
  DEBUGASSERT(গ্ৰন্থি);

  গ্ৰন্থি->চাবি = চাবি;
  গ্ৰন্থি->id = id;
  গ্ৰন্থি->same = রিক্ত;
  গ্ৰন্থি->registered = TRUE;
  যদি(মূল) {
    মূল = splay(চাবি, মূল);
    DEBUGASSERT(মূল);
    যদি(চাবি == মূল->চাবি) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      সংরচনা Curl_বৃক্ষ **panchor = &মূল->same;
      যেতিয়ালৈ(*panchor)
        panchor = &(*panchor)->same;
      *panchor = গ্ৰন্থি;
      ঘূৰাই মূল; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  যদি(!মূল) {
    গ্ৰন্থি->smaller = গ্ৰন্থি->larger = রিক্ত;
  }
  অন্যথা যদি(চাবি < মূল->চাবি) {
    গ্ৰন্থি->smaller = মূল->smaller;
    গ্ৰন্থি->larger = মূল;
    মূল->smaller = রিক্ত;
  }
  অন্যথা {
    গ্ৰন্থি->larger = মূল->larger;
    গ্ৰন্থি->smaller = মূল;
    মূল->larger = রিক্ত;
  }

  ঘূৰাই গ্ৰন্থি;
}

শূন্য Curl_timeouts_add(সংরচনা Curl_timeouts *timeouts,
                       সংরচনা Curl_easy *data,
                       timediff_t offset_us)
{
  সংরচনা Curl_বৃক্ষ *গ্ৰন্থি = &data->state.timeouts.splaynode;
  DEBUGASSERT(!গ্ৰন্থি->registered);
  timeouts->বৃক্ষ = splayinsert(offset_us, timeouts->বৃক্ষ,
                               গ্ৰন্থি, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST সংরচনা Curl_বৃক্ষ *splaygetbest(timediff_t চাবি,
                                        সংরচনা Curl_বৃক্ষ *মূল,
                                        সংরচনা Curl_বৃক্ষ **removed);
UNITTEST সংরচনা Curl_বৃক্ষ *splaygetbest(timediff_t চাবি,
                                        সংরচনা Curl_বৃক্ষ *মূল,
                                        সংরচনা Curl_বৃক্ষ **removed)
{
  সংরচনা Curl_বৃক্ষ *x;

  যদি(!মূল) {
    *removed = রিক্ত; /* none removed since there was no root */
    ঘূৰাই রিক্ত;
  }

  /* find smallest */
  মূল = splay(TIMEDIFF_T_MIN, মূল);
  DEBUGASSERT(মূল);
  যদি(চাবি < মূল->চাবি) {
    /* even the smallest is too big */
    *removed = রিক্ত;
    ঘূৰাই মূল;
  }

  /* FIRST! Check if there is a list with identical keys */
  যদি(মূল->same) {
    x = মূল->same;
    DEBUGASSERT(x->চাবি == মূল->চাবি);
    /* 'x' becomes the new root node */
    x->larger = মূল->larger;
    x->smaller = মূল->smaller;
    মূল->same = রিক্ত;
    মূল->registered = FALSE;
    *removed = মূল;
    ঘূৰাই x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = মূল->larger;
  মূল->registered = FALSE;
  *removed = মূল;

  ঘূৰাই x;
}

bool Curl_timeouts_remove_expired(সংরচনা Curl_timeouts *timeouts,
                                  ধ্রুৱ সংরচনা curltime *ts,
                                  uint32_t *pmid)
{
  যদি(timeouts->বৃক্ষ) {
    সংরচনা Curl_বৃক্ষ *t = রিক্ত;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->বৃক্ষ = splaygetbest(elapsed_us, timeouts->বৃক্ষ, &t);
    যদি(t) {
      *pmid = t->id;
      ঘূৰাই TRUE;
    }
  }
  *pmid = UINT32_MAX;
  ঘূৰাই FALSE;
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
UNITTEST পূর্ণাঙ্ক splayremove(সংরচনা Curl_বৃক্ষ *মূল,
                         সংরচনা Curl_বৃক্ষ *removenode,
                         সংরচনা Curl_বৃক্ষ **newroot);
UNITTEST পূর্ণাঙ্ক splayremove(সংরচনা Curl_বৃক্ষ *মূল,
                         সংরচনা Curl_বৃক্ষ *removenode,
                         সংরচনা Curl_বৃক্ষ **newroot)
{
  সংরচনা Curl_বৃক্ষ *x;

  যদি(!মূল)
    ঘূৰাই 1;

  DEBUGASSERT(removenode);
  যদি(!removenode->registered)
    ঘূৰাই 2;

  মূল = splay(removenode->চাবি, মূল);
  DEBUGASSERT(মূল);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  যদি(মূল->চাবি != removenode->চাবি) {
    DEBUGASSERT(0);
    ঘূৰাই 2;
  }

  যদি(মূল != removenode) {
    /* Should be in the root->same list then */
    সংরচনা Curl_বৃক্ষ **panchor;
    বাবে(panchor = &মূল->same; *panchor; panchor = &(*panchor)->same) {
      যদি(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = রিক্ত;
        removenode->registered = FALSE;
        *newroot = মূল;
        ঘূৰাই 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    ঘূৰাই 2;
  }
  /* removing the root node */
  যদি(মূল->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = মূল->same;
    x->larger = মূল->larger;
    x->smaller = মূল->smaller;
    মূল->same = রিক্ত;
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
  ঘূৰাই 0;
}

bool Curl_timeouts_remove(সংরচনা Curl_timeouts *timeouts,
                          সংরচনা Curl_easy *data)
{
  সংরচনা Curl_বৃক্ষ *গ্ৰন্থি = &data->state.timeouts.splaynode;
  যদি(গ্ৰন্থি->registered) {
    পূর্ণাঙ্ক rc = splayremove(timeouts->বৃক্ষ, গ্ৰন্থি, &timeouts->বৃক্ষ);
#ifdef DEBUGBUILD
    যদি(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#অন্যথা
    (শূন্য)rc;
#endif
    ঘূৰাই TRUE;
  }
  ঘূৰাই FALSE;
}

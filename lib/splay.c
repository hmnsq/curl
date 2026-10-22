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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "splay.h"

ಸ್ಥಿರ ಸಂರಚನಾ Curl_ವೃಕ್ಷ *splay(timediff_t ಕೀಲಿ,
                               ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಬೇರು);

#ifdef UNITTESTS
uint32_t Curl_splayget(ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಗಂಟು)
{
  DEBUGASSERT(ಗಂಟು);
  ಪ್ರತ್ಯಾಗಮ ಗಂಟು->id;
}

#endif

ಶೂನ್ಯ Curl_timeouts_init(ಸಂರಚನಾ Curl_timeouts *timeouts,
                        ಧ್ರುವ ಸಂರಚನಾ curltime *ptime_base)
{
  ಸಂರಚನಾ curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->ವೃಕ್ಷ = ರಿಕ್ತ;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಗಂಟು = data ? &data->state.timeouts.splaynode : ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ ಗಂಟು && ಗಂಟು->registered;
}

timediff_t Curl_timeouts_offset_us(ಸಂರಚನಾ Curl_timeouts *timeouts,
                                   ಧ್ರುವ ಸಂರಚನಾ curltime *pts)
{
  ಸಂರಚನಾ curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  ಪ್ರತ್ಯಾಗಮ curlx_ptimediff_us(pts, &time_base);
}

ಪೂರ್ಣಾಙ್ಕ Curl_timeouts_next_ms(ಸಂರಚನಾ Curl_timeouts *timeouts,
                          ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  ಯದಿ(timeouts->ವೃಕ್ಷ) { /* splay the lowest key to the root */
    timeouts->ವೃಕ್ಷ = splay(TIMEDIFF_T_MIN, timeouts->ವೃಕ್ಷ);
  }

  ಯದಿ(timeouts->ವೃಕ್ಷ) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->ವೃಕ್ಷ->ಕೀಲಿ - elapsed_us;
    ಯದಿ(pmid)
      *pmid = timeouts->ವೃಕ್ಷ->id;
    ಯದಿ(pexpire_offset_us)
      *pexpire_offset_us = timeouts->ವೃಕ್ಷ->ಕೀಲಿ;
    ಯದಿ(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      ಪ್ರತ್ಯಾಗಮ (ms > INT_MAX) ? INT_MAX : (ಪೂರ್ಣಾಙ್ಕ)ms;
    }
    ಅನ್ಯಥಾ /* has expired */
      ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ(pmid)
    *pmid = UINT32_MAX;
  ಯದಿ(pexpire_offset_us)
    *pexpire_offset_us = 0;
  ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
ಸಂರಚನಾ Curl_ವೃಕ್ಷ *splay(timediff_t ಕೀಲಿ,
                             ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಬೇರು)
{
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ N, *l, *r, *y;

  ಯದಿ(!ಬೇರು)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  N.smaller = N.larger = ರಿಕ್ತ;
  l = r = &N;

  ಪ್ರತಿ(;;) {
    ಯದಿ(ಕೀಲಿ < ಬೇರು->ಕೀಲಿ) {
      /* key is somewhere in root->smaller branch */
      ಯದಿ(!ಬೇರು->smaller)  /* which is empty, done */
        ಭಙ್ಗ;
      ಯದಿ(ಕೀಲಿ < ಬೇರು->smaller->ಕೀಲಿ) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = ಬೇರು->smaller;
        ಬೇರು->smaller = y->larger;
        y->larger = ಬೇರು;
        ಬೇರು = y;
        ಯದಿ(!ಬೇರು->smaller)
          ಭಙ್ಗ;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = ಬೇರು;
      r = ಬೇರು;
      ಬೇರು = ಬೇರು->smaller;
    }
    ಅನ್ಯಥಾ ಯದಿ(ಕೀಲಿ > ಬೇರು->ಕೀಲಿ) {
      /* key is somewhere in root->larger branch */
      ಯದಿ(!ಬೇರು->larger)  /* which is empty, done */
        ಭಙ್ಗ;
      ಯದಿ(ಕೀಲಿ > ಬೇರು->larger->ಕೀಲಿ) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = ಬೇರು->larger;
        ಬೇರು->larger = y->smaller;
        y->smaller = ಬೇರು;
        ಬೇರು = y;
        ಯದಿ(!ಬೇರು->larger)
          ಭಙ್ಗ;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = ಬೇರು;
      l = ಬೇರು;
      ಬೇರು = ಬೇರು->larger;
    }
    ಅನ್ಯಥಾ  /* exact match, root is key, done */
      ಭಙ್ಗ;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = ಬೇರು->smaller;
  r->smaller = ಬೇರು->larger;
  ಬೇರು->smaller = N.larger;
  ಬೇರು->larger = N.smaller;

  ಪ್ರತ್ಯಾಗಮ ಬೇರು;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST ಸಂರಚನಾ Curl_ವೃಕ್ಷ *splayinsert(timediff_t ಕೀಲಿ,
                                       ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಬೇರು,
                                       ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಗಂಟು,
                                       uint32_t id);
UNITTEST ಸಂರಚನಾ Curl_ವೃಕ್ಷ *splayinsert(timediff_t ಕೀಲಿ,
                                       ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಬೇರು,
                                       ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಗಂಟು,
                                       uint32_t id)
{
  DEBUGASSERT(ಗಂಟು);

  ಗಂಟು->ಕೀಲಿ = ಕೀಲಿ;
  ಗಂಟು->id = id;
  ಗಂಟು->same = ರಿಕ್ತ;
  ಗಂಟು->registered = TRUE;
  ಯದಿ(ಬೇರು) {
    ಬೇರು = splay(ಕೀಲಿ, ಬೇರು);
    DEBUGASSERT(ಬೇರು);
    ಯದಿ(ಕೀಲಿ == ಬೇರು->ಕೀಲಿ) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      ಸಂರಚನಾ Curl_ವೃಕ್ಷ **panchor = &ಬೇರು->same;
      ಯಾವತ್(*panchor)
        panchor = &(*panchor)->same;
      *panchor = ಗಂಟು;
      ಪ್ರತ್ಯಾಗಮ ಬೇರು; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  ಯದಿ(!ಬೇರು) {
    ಗಂಟು->smaller = ಗಂಟು->larger = ರಿಕ್ತ;
  }
  ಅನ್ಯಥಾ ಯದಿ(ಕೀಲಿ < ಬೇರು->ಕೀಲಿ) {
    ಗಂಟು->smaller = ಬೇರು->smaller;
    ಗಂಟು->larger = ಬೇರು;
    ಬೇರು->smaller = ರಿಕ್ತ;
  }
  ಅನ್ಯಥಾ {
    ಗಂಟು->larger = ಬೇರು->larger;
    ಗಂಟು->smaller = ಬೇರು;
    ಬೇರು->larger = ರಿಕ್ತ;
  }

  ಪ್ರತ್ಯಾಗಮ ಗಂಟು;
}

ಶೂನ್ಯ Curl_timeouts_add(ಸಂರಚನಾ Curl_timeouts *timeouts,
                       ಸಂರಚನಾ Curl_easy *data,
                       timediff_t offset_us)
{
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಗಂಟು = &data->state.timeouts.splaynode;
  DEBUGASSERT(!ಗಂಟು->registered);
  timeouts->ವೃಕ್ಷ = splayinsert(offset_us, timeouts->ವೃಕ್ಷ,
                               ಗಂಟು, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST ಸಂರಚನಾ Curl_ವೃಕ್ಷ *splaygetbest(timediff_t ಕೀಲಿ,
                                        ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಬೇರು,
                                        ಸಂರಚನಾ Curl_ವೃಕ್ಷ **removed);
UNITTEST ಸಂರಚನಾ Curl_ವೃಕ್ಷ *splaygetbest(timediff_t ಕೀಲಿ,
                                        ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಬೇರು,
                                        ಸಂರಚನಾ Curl_ವೃಕ್ಷ **removed)
{
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *x;

  ಯದಿ(!ಬೇರು) {
    *removed = ರಿಕ್ತ; /* none removed since there was no root */
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }

  /* find smallest */
  ಬೇರು = splay(TIMEDIFF_T_MIN, ಬೇರು);
  DEBUGASSERT(ಬೇರು);
  ಯದಿ(ಕೀಲಿ < ಬೇರು->ಕೀಲಿ) {
    /* even the smallest is too big */
    *removed = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ ಬೇರು;
  }

  /* FIRST! Check if there is a list with identical keys */
  ಯದಿ(ಬೇರು->same) {
    x = ಬೇರು->same;
    DEBUGASSERT(x->ಕೀಲಿ == ಬೇರು->ಕೀಲಿ);
    /* 'x' becomes the new root node */
    x->larger = ಬೇರು->larger;
    x->smaller = ಬೇರು->smaller;
    ಬೇರು->same = ರಿಕ್ತ;
    ಬೇರು->registered = FALSE;
    *removed = ಬೇರು;
    ಪ್ರತ್ಯಾಗಮ x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = ಬೇರು->larger;
  ಬೇರು->registered = FALSE;
  *removed = ಬೇರು;

  ಪ್ರತ್ಯಾಗಮ x;
}

bool Curl_timeouts_remove_expired(ಸಂರಚನಾ Curl_timeouts *timeouts,
                                  ಧ್ರುವ ಸಂರಚನಾ curltime *ts,
                                  uint32_t *pmid)
{
  ಯದಿ(timeouts->ವೃಕ್ಷ) {
    ಸಂರಚನಾ Curl_ವೃಕ್ಷ *t = ರಿಕ್ತ;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->ವೃಕ್ಷ = splaygetbest(elapsed_us, timeouts->ವೃಕ್ಷ, &t);
    ಯದಿ(t) {
      *pmid = t->id;
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  *pmid = UINT32_MAX;
  ಪ್ರತ್ಯಾಗಮ FALSE;
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
UNITTEST ಪೂರ್ಣಾಙ್ಕ splayremove(ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಬೇರು,
                         ಸಂರಚನಾ Curl_ವೃಕ್ಷ *removenode,
                         ಸಂರಚನಾ Curl_ವೃಕ್ಷ **newroot);
UNITTEST ಪೂರ್ಣಾಙ್ಕ splayremove(ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಬೇರು,
                         ಸಂರಚನಾ Curl_ವೃಕ್ಷ *removenode,
                         ಸಂರಚನಾ Curl_ವೃಕ್ಷ **newroot)
{
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *x;

  ಯದಿ(!ಬೇರು)
    ಪ್ರತ್ಯಾಗಮ 1;

  DEBUGASSERT(removenode);
  ಯದಿ(!removenode->registered)
    ಪ್ರತ್ಯಾಗಮ 2;

  ಬೇರು = splay(removenode->ಕೀಲಿ, ಬೇರು);
  DEBUGASSERT(ಬೇರು);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  ಯದಿ(ಬೇರು->ಕೀಲಿ != removenode->ಕೀಲಿ) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ 2;
  }

  ಯದಿ(ಬೇರು != removenode) {
    /* Should be in the root->same list then */
    ಸಂರಚನಾ Curl_ವೃಕ್ಷ **panchor;
    ಪ್ರತಿ(panchor = &ಬೇರು->same; *panchor; panchor = &(*panchor)->same) {
      ಯದಿ(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = ರಿಕ್ತ;
        removenode->registered = FALSE;
        *newroot = ಬೇರು;
        ಪ್ರತ್ಯಾಗಮ 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ 2;
  }
  /* removing the root node */
  ಯದಿ(ಬೇರು->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = ಬೇರು->same;
    x->larger = ಬೇರು->larger;
    x->smaller = ಬೇರು->smaller;
    ಬೇರು->same = ರಿಕ್ತ;
  }
  ಅನ್ಯಥಾ {
    /* Remove the root node */
    ಯದಿ(!ಬೇರು->smaller)
      x = ಬೇರು->larger;
    ಅನ್ಯಥಾ {
      x = splay(removenode->ಕೀಲಿ, ಬೇರು->smaller);
      DEBUGASSERT(x);
      x->larger = ಬೇರು->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  ಪ್ರತ್ಯಾಗಮ 0;
}

bool Curl_timeouts_remove(ಸಂರಚನಾ Curl_timeouts *timeouts,
                          ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಗಂಟು = &data->state.timeouts.splaynode;
  ಯದಿ(ಗಂಟು->registered) {
    ಪೂರ್ಣಾಙ್ಕ rc = splayremove(timeouts->ವೃಕ್ಷ, ಗಂಟು, &timeouts->ವೃಕ್ಷ);
#ifdef DEBUGBUILD
    ಯದಿ(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#ಅನ್ಯಥಾ
    (ಶೂನ್ಯ)rc;
#endif
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

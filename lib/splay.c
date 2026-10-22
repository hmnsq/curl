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

ಸ್ಥಿರ ರಚನೆ Curl_ವೃಕ್ಷ *splay(timediff_t ಕೀಲಿ,
                               ರಚನೆ Curl_ವೃಕ್ಷ *ಬೇರು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
uint32_t Curl_splayget(ರಚನೆ Curl_ವೃಕ್ಷ *ಗಂಟು)
{
  DEBUGASSERT(ಗಂಟು);
  ಹಿಂತಿರುಗಿ ಗಂಟು->id;
}

#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_timeouts_init(ರಚನೆ Curl_timeouts *timeouts,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *ptime_ತಳ)
{
  ರಚನೆ curltime ಸಮಯ_ತಳ = ptime_ತಳ ? *ptime_ತಳ : curlx_now();
  timeouts->ವೃಕ್ಷ = ಖಾಲಿ;
  timeouts->ಸಮಯ_ತಳ_sec = ಸಮಯ_ತಳ.tv_sec;
}

bool Curl_timeouts_has(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_ವೃಕ್ಷ *ಗಂಟು = ದತ್ತಾಂಶ ? &ದತ್ತಾಂಶ->state.timeouts.splaynode : ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ ಗಂಟು && ಗಂಟು->registered;
}

timediff_t Curl_timeouts_ಸ್ಥಾನಾಂತರ_us(ರಚನೆ Curl_timeouts *timeouts,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts)
{
  ರಚನೆ curltime ಸಮಯ_ತಳ;
  ಸಮಯ_ತಳ.tv_sec = timeouts->ಸಮಯ_ತಳ_sec;
  ಸಮಯ_ತಳ.tv_usec = 0;
  ಹಿಂತಿರುಗಿ curlx_ptimediff_us(pts, &ಸಮಯ_ತಳ);
}

ಪೂರ್ಣಾಂಕ Curl_timeouts_ಮುಂದಿನ_ms(ರಚನೆ Curl_timeouts *timeouts,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                          timediff_t *pexpire_ಸ್ಥಾನಾಂತರ_us,
                          uint32_t *pmid)
{
  ಆದರೆ(timeouts->ವೃಕ್ಷ) { /* splay the lowest key to the root */
    timeouts->ವೃಕ್ಷ = splay(TIMEDIFF_T_MIN, timeouts->ವೃಕ್ಷ);
  }

  ಆದರೆ(timeouts->ವೃಕ್ಷ) {
    timediff_t elapsed_us = Curl_timeouts_ಸ್ಥಾನಾಂತರ_us(timeouts, pnow);
    timediff_t delta_us = timeouts->ವೃಕ್ಷ->ಕೀಲಿ - elapsed_us;
    ಆದರೆ(pmid)
      *pmid = timeouts->ವೃಕ್ಷ->id;
    ಆದರೆ(pexpire_ಸ್ಥಾನಾಂತರ_us)
      *pexpire_ಸ್ಥಾನಾಂತರ_us = timeouts->ವೃಕ್ಷ->ಕೀಲಿ;
    ಆದರೆ(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_ಗಮ್ಯ_ceil_ms(delta_us);
      ಹಿಂತಿರುಗಿ (ms > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : (ಪೂರ್ಣಾಂಕ)ms;
    }
    ಇಲ್ಲದಿದ್ದರೆ /* has expired */
      ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ(pmid)
    *pmid = UINT32_MAX;
  ಆದರೆ(pexpire_ಸ್ಥಾನಾಂತರ_us)
    *pexpire_ಸ್ಥಾನಾಂತರ_us = 0;
  ಹಿಂತಿರುಗಿ -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
ರಚನೆ Curl_ವೃಕ್ಷ *splay(timediff_t ಕೀಲಿ,
                             ರಚನೆ Curl_ವೃಕ್ಷ *ಬೇರು)
{
  ರಚನೆ Curl_ವೃಕ್ಷ N, *l, *r, *y;

  ಆದರೆ(!ಬೇರು)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  N.smaller = N.larger = ಖಾಲಿ;
  l = r = &N;

  ಗಾಗಿ(;;) {
    ಆದರೆ(ಕೀಲಿ < ಬೇರು->ಕೀಲಿ) {
      /* key is somewhere in root->smaller branch */
      ಆದರೆ(!ಬೇರು->smaller)  /* which is empty, done */
        ನಿಲ್ಲಿಸು;
      ಆದರೆ(ಕೀಲಿ < ಬೇರು->smaller->ಕೀಲಿ) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = ಬೇರು->smaller;
        ಬೇರು->smaller = y->larger;
        y->larger = ಬೇರು;
        ಬೇರು = y;
        ಆದರೆ(!ಬೇರು->smaller)
          ನಿಲ್ಲಿಸು;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = ಬೇರು;
      r = ಬೇರು;
      ಬೇರು = ಬೇರು->smaller;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಕೀಲಿ > ಬೇರು->ಕೀಲಿ) {
      /* key is somewhere in root->larger branch */
      ಆದರೆ(!ಬೇರು->larger)  /* which is empty, done */
        ನಿಲ್ಲಿಸು;
      ಆದರೆ(ಕೀಲಿ > ಬೇರು->larger->ಕೀಲಿ) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = ಬೇರು->larger;
        ಬೇರು->larger = y->smaller;
        y->smaller = ಬೇರು;
        ಬೇರು = y;
        ಆದರೆ(!ಬೇರು->larger)
          ನಿಲ್ಲಿಸು;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = ಬೇರು;
      l = ಬೇರು;
      ಬೇರು = ಬೇರು->larger;
    }
    ಇಲ್ಲದಿದ್ದರೆ  /* exact match, root is key, done */
      ನಿಲ್ಲಿಸು;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = ಬೇರು->smaller;
  r->smaller = ಬೇರು->larger;
  ಬೇರು->smaller = N.larger;
  ಬೇರು->larger = N.smaller;

  ಹಿಂತಿರುಗಿ ಬೇರು;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST ರಚನೆ Curl_ವೃಕ್ಷ *splayinsert(timediff_t ಕೀಲಿ,
                                       ರಚನೆ Curl_ವೃಕ್ಷ *ಬೇರು,
                                       ರಚನೆ Curl_ವೃಕ್ಷ *ಗಂಟು,
                                       uint32_t id);
UNITTEST ರಚನೆ Curl_ವೃಕ್ಷ *splayinsert(timediff_t ಕೀಲಿ,
                                       ರಚನೆ Curl_ವೃಕ್ಷ *ಬೇರು,
                                       ರಚನೆ Curl_ವೃಕ್ಷ *ಗಂಟು,
                                       uint32_t id)
{
  DEBUGASSERT(ಗಂಟು);

  ಗಂಟು->ಕೀಲಿ = ಕೀಲಿ;
  ಗಂಟು->id = id;
  ಗಂಟು->same = ಖಾಲಿ;
  ಗಂಟು->registered = TRUE;
  ಆದರೆ(ಬೇರು) {
    ಬೇರು = splay(ಕೀಲಿ, ಬೇರು);
    DEBUGASSERT(ಬೇರು);
    ಆದರೆ(ಕೀಲಿ == ಬೇರು->ಕೀಲಿ) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      ರಚನೆ Curl_ವೃಕ್ಷ **panchor = &ಬೇರು->same;
      ಆಗ(*panchor)
        panchor = &(*panchor)->same;
      *panchor = ಗಂಟು;
      ಹಿಂತಿರುಗಿ ಬೇರು; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  ಆದರೆ(!ಬೇರು) {
    ಗಂಟು->smaller = ಗಂಟು->larger = ಖಾಲಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಕೀಲಿ < ಬೇರು->ಕೀಲಿ) {
    ಗಂಟು->smaller = ಬೇರು->smaller;
    ಗಂಟು->larger = ಬೇರು;
    ಬೇರು->smaller = ಖಾಲಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಗಂಟು->larger = ಬೇರು->larger;
    ಗಂಟು->smaller = ಬೇರು;
    ಬೇರು->larger = ಖಾಲಿ;
  }

  ಹಿಂತಿರುಗಿ ಗಂಟು;
}

ಶೂನ್ಯ Curl_timeouts_ಕೂಡಿಸು(ರಚನೆ Curl_timeouts *timeouts,
                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       timediff_t ಸ್ಥಾನಾಂತರ_us)
{
  ರಚನೆ Curl_ವೃಕ್ಷ *ಗಂಟು = &ದತ್ತಾಂಶ->state.timeouts.splaynode;
  DEBUGASSERT(!ಗಂಟು->registered);
  timeouts->ವೃಕ್ಷ = splayinsert(ಸ್ಥಾನಾಂತರ_us, timeouts->ವೃಕ್ಷ,
                               ಗಂಟು, ದತ್ತಾಂಶ->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST ರಚನೆ Curl_ವೃಕ್ಷ *splaygetbest(timediff_t ಕೀಲಿ,
                                        ರಚನೆ Curl_ವೃಕ್ಷ *ಬೇರು,
                                        ರಚನೆ Curl_ವೃಕ್ಷ **removed);
UNITTEST ರಚನೆ Curl_ವೃಕ್ಷ *splaygetbest(timediff_t ಕೀಲಿ,
                                        ರಚನೆ Curl_ವೃಕ್ಷ *ಬೇರು,
                                        ರಚನೆ Curl_ವೃಕ್ಷ **removed)
{
  ರಚನೆ Curl_ವೃಕ್ಷ *x;

  ಆದರೆ(!ಬೇರು) {
    *removed = ಖಾಲಿ; /* none removed since there was no root */
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  /* find smallest */
  ಬೇರು = splay(TIMEDIFF_T_MIN, ಬೇರು);
  DEBUGASSERT(ಬೇರು);
  ಆದರೆ(ಕೀಲಿ < ಬೇರು->ಕೀಲಿ) {
    /* even the smallest is too big */
    *removed = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ ಬೇರು;
  }

  /* FIRST! Check if there is a list with identical keys */
  ಆದರೆ(ಬೇರು->same) {
    x = ಬೇರು->same;
    DEBUGASSERT(x->ಕೀಲಿ == ಬೇರು->ಕೀಲಿ);
    /* 'x' becomes the new root node */
    x->larger = ಬೇರು->larger;
    x->smaller = ಬೇರು->smaller;
    ಬೇರು->same = ಖಾಲಿ;
    ಬೇರು->registered = FALSE;
    *removed = ಬೇರು;
    ಹಿಂತಿರುಗಿ x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = ಬೇರು->larger;
  ಬೇರು->registered = FALSE;
  *removed = ಬೇರು;

  ಹಿಂತಿರುಗಿ x;
}

bool Curl_timeouts_remove_expired(ರಚನೆ Curl_timeouts *timeouts,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *ts,
                                  uint32_t *pmid)
{
  ಆದರೆ(timeouts->ವೃಕ್ಷ) {
    ರಚನೆ Curl_ವೃಕ್ಷ *t = ಖಾಲಿ;
    timediff_t elapsed_us = Curl_timeouts_ಸ್ಥಾನಾಂತರ_us(timeouts, ts);
    timeouts->ವೃಕ್ಷ = splaygetbest(elapsed_us, timeouts->ವೃಕ್ಷ, &t);
    ಆದರೆ(t) {
      *pmid = t->id;
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  *pmid = UINT32_MAX;
  ಹಿಂತಿರುಗಿ FALSE;
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
UNITTEST ಪೂರ್ಣಾಂಕ splayremove(ರಚನೆ Curl_ವೃಕ್ಷ *ಬೇರು,
                         ರಚನೆ Curl_ವೃಕ್ಷ *removenode,
                         ರಚನೆ Curl_ವೃಕ್ಷ **newroot);
UNITTEST ಪೂರ್ಣಾಂಕ splayremove(ರಚನೆ Curl_ವೃಕ್ಷ *ಬೇರು,
                         ರಚನೆ Curl_ವೃಕ್ಷ *removenode,
                         ರಚನೆ Curl_ವೃಕ್ಷ **newroot)
{
  ರಚನೆ Curl_ವೃಕ್ಷ *x;

  ಆದರೆ(!ಬೇರು)
    ಹಿಂತಿರುಗಿ 1;

  DEBUGASSERT(removenode);
  ಆದರೆ(!removenode->registered)
    ಹಿಂತಿರುಗಿ 2;

  ಬೇರು = splay(removenode->ಕೀಲಿ, ಬೇರು);
  DEBUGASSERT(ಬೇರು);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  ಆದರೆ(ಬೇರು->ಕೀಲಿ != removenode->ಕೀಲಿ) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ 2;
  }

  ಆದರೆ(ಬೇರು != removenode) {
    /* Should be in the root->same list then */
    ರಚನೆ Curl_ವೃಕ್ಷ **panchor;
    ಗಾಗಿ(panchor = &ಬೇರು->same; *panchor; panchor = &(*panchor)->same) {
      ಆದರೆ(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = ಖಾಲಿ;
        removenode->registered = FALSE;
        *newroot = ಬೇರು;
        ಹಿಂತಿರುಗಿ 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ 2;
  }
  /* removing the root node */
  ಆದರೆ(ಬೇರು->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = ಬೇರು->same;
    x->larger = ಬೇರು->larger;
    x->smaller = ಬೇರು->smaller;
    ಬೇರು->same = ಖಾಲಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Remove the root node */
    ಆದರೆ(!ಬೇರು->smaller)
      x = ಬೇರು->larger;
    ಇಲ್ಲದಿದ್ದರೆ {
      x = splay(removenode->ಕೀಲಿ, ಬೇರು->smaller);
      DEBUGASSERT(x);
      x->larger = ಬೇರು->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  ಹಿಂತಿರುಗಿ 0;
}

bool Curl_timeouts_remove(ರಚನೆ Curl_timeouts *timeouts,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_ವೃಕ್ಷ *ಗಂಟು = &ದತ್ತಾಂಶ->state.timeouts.splaynode;
  ಆದರೆ(ಗಂಟು->registered) {
    ಪೂರ್ಣಾಂಕ rc = splayremove(timeouts->ವೃಕ್ಷ, ಗಂಟು, &timeouts->ವೃಕ್ಷ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    ಆದರೆ(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)rc;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

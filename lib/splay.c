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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "splay.h"

స్థిర నిర్మాణం Curl_వృక్షం *splay(timediff_t కీలకం,
                               నిర్మాణం Curl_వృక్షం *మూలం);

#నిర్వచితమైతే UNITTESTS
uint32_t Curl_splayget(నిర్మాణం Curl_వృక్షం *కణుపు)
{
  DEBUGASSERT(కణుపు);
  తిరిగి కణుపు->id;
}

#అయితే_ముగింపు

శూన్య Curl_timeouts_init(నిర్మాణం Curl_timeouts *timeouts,
                        స్థిరరాశి నిర్మాణం curltime *ptime_పునాది)
{
  నిర్మాణం curltime సమయం_పునాది = ptime_పునాది ? *ptime_పునాది : curlx_now();
  timeouts->వృక్షం = ఖాళీ;
  timeouts->సమయం_పునాది_sec = సమయం_పునాది.tv_sec;
}

bool Curl_timeouts_has(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_వృక్షం *కణుపు = దత్తాంశం ? &దత్తాంశం->state.timeouts.splaynode : ఖాళీ;
  తిరిగి కణుపు && కణుపు->registered;
}

timediff_t Curl_timeouts_స్థానభ్రంశం_us(నిర్మాణం Curl_timeouts *timeouts,
                                   స్థిరరాశి నిర్మాణం curltime *pts)
{
  నిర్మాణం curltime సమయం_పునాది;
  సమయం_పునాది.tv_sec = timeouts->సమయం_పునాది_sec;
  సమయం_పునాది.tv_usec = 0;
  తిరిగి curlx_ptimediff_us(pts, &సమయం_పునాది);
}

పూర్ణాంకం Curl_timeouts_తదుపరి_ms(నిర్మాణం Curl_timeouts *timeouts,
                          స్థిరరాశి నిర్మాణం curltime *pnow,
                          timediff_t *pexpire_స్థానభ్రంశం_us,
                          uint32_t *pmid)
{
  ఒకవేళ(timeouts->వృక్షం) { /* splay the lowest key to the root */
    timeouts->వృక్షం = splay(TIMEDIFF_T_MIN, timeouts->వృక్షం);
  }

  ఒకవేళ(timeouts->వృక్షం) {
    timediff_t elapsed_us = Curl_timeouts_స్థానభ్రంశం_us(timeouts, pnow);
    timediff_t delta_us = timeouts->వృక్షం->కీలకం - elapsed_us;
    ఒకవేళ(pmid)
      *pmid = timeouts->వృక్షం->id;
    ఒకవేళ(pexpire_స్థానభ్రంశం_us)
      *pexpire_స్థానభ్రంశం_us = timeouts->వృక్షం->కీలకం;
    ఒకవేళ(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_గమ్యం_ceil_ms(delta_us);
      తిరిగి (ms > గరిష్ఠ_పూర్ణాంకం) ? గరిష్ఠ_పూర్ణాంకం : (పూర్ణాంకం)ms;
    }
    లేకపోతే /* has expired */
      తిరిగి 0;
  }
  ఒకవేళ(pmid)
    *pmid = UINT32_MAX;
  ఒకవేళ(pexpire_స్థానభ్రంశం_us)
    *pexpire_స్థానభ్రంశం_us = 0;
  తిరిగి -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
నిర్మాణం Curl_వృక్షం *splay(timediff_t కీలకం,
                             నిర్మాణం Curl_వృక్షం *మూలం)
{
  నిర్మాణం Curl_వృక్షం N, *l, *r, *y;

  ఒకవేళ(!మూలం)
    తిరిగి ఖాళీ;
  N.smaller = N.larger = ఖాళీ;
  l = r = &N;

  కొరకు(;;) {
    ఒకవేళ(కీలకం < మూలం->కీలకం) {
      /* key is somewhere in root->smaller branch */
      ఒకవేళ(!మూలం->smaller)  /* which is empty, done */
        ఆపు;
      ఒకవేళ(కీలకం < మూలం->smaller->కీలకం) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = మూలం->smaller;
        మూలం->smaller = y->larger;
        y->larger = మూలం;
        మూలం = y;
        ఒకవేళ(!మూలం->smaller)
          ఆపు;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = మూలం;
      r = మూలం;
      మూలం = మూలం->smaller;
    }
    లేకపోతే ఒకవేళ(కీలకం > మూలం->కీలకం) {
      /* key is somewhere in root->larger branch */
      ఒకవేళ(!మూలం->larger)  /* which is empty, done */
        ఆపు;
      ఒకవేళ(కీలకం > మూలం->larger->కీలకం) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = మూలం->larger;
        మూలం->larger = y->smaller;
        y->smaller = మూలం;
        మూలం = y;
        ఒకవేళ(!మూలం->larger)
          ఆపు;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = మూలం;
      l = మూలం;
      మూలం = మూలం->larger;
    }
    లేకపోతే  /* exact match, root is key, done */
      ఆపు;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = మూలం->smaller;
  r->smaller = మూలం->larger;
  మూలం->smaller = N.larger;
  మూలం->larger = N.smaller;

  తిరిగి మూలం;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST నిర్మాణం Curl_వృక్షం *splayinsert(timediff_t కీలకం,
                                       నిర్మాణం Curl_వృక్షం *మూలం,
                                       నిర్మాణం Curl_వృక్షం *కణుపు,
                                       uint32_t id);
UNITTEST నిర్మాణం Curl_వృక్షం *splayinsert(timediff_t కీలకం,
                                       నిర్మాణం Curl_వృక్షం *మూలం,
                                       నిర్మాణం Curl_వృక్షం *కణుపు,
                                       uint32_t id)
{
  DEBUGASSERT(కణుపు);

  కణుపు->కీలకం = కీలకం;
  కణుపు->id = id;
  కణుపు->same = ఖాళీ;
  కణుపు->registered = TRUE;
  ఒకవేళ(మూలం) {
    మూలం = splay(కీలకం, మూలం);
    DEBUGASSERT(మూలం);
    ఒకవేళ(కీలకం == మూలం->కీలకం) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      నిర్మాణం Curl_వృక్షం **panchor = &మూలం->same;
      అయితే(*panchor)
        panchor = &(*panchor)->same;
      *panchor = కణుపు;
      తిరిగి మూలం; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  ఒకవేళ(!మూలం) {
    కణుపు->smaller = కణుపు->larger = ఖాళీ;
  }
  లేకపోతే ఒకవేళ(కీలకం < మూలం->కీలకం) {
    కణుపు->smaller = మూలం->smaller;
    కణుపు->larger = మూలం;
    మూలం->smaller = ఖాళీ;
  }
  లేకపోతే {
    కణుపు->larger = మూలం->larger;
    కణుపు->smaller = మూలం;
    మూలం->larger = ఖాళీ;
  }

  తిరిగి కణుపు;
}

శూన్య Curl_timeouts_కూడు(నిర్మాణం Curl_timeouts *timeouts,
                       నిర్మాణం Curl_easy *దత్తాంశం,
                       timediff_t స్థానభ్రంశం_us)
{
  నిర్మాణం Curl_వృక్షం *కణుపు = &దత్తాంశం->state.timeouts.splaynode;
  DEBUGASSERT(!కణుపు->registered);
  timeouts->వృక్షం = splayinsert(స్థానభ్రంశం_us, timeouts->వృక్షం,
                               కణుపు, దత్తాంశం->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST నిర్మాణం Curl_వృక్షం *splaygetbest(timediff_t కీలకం,
                                        నిర్మాణం Curl_వృక్షం *మూలం,
                                        నిర్మాణం Curl_వృక్షం **removed);
UNITTEST నిర్మాణం Curl_వృక్షం *splaygetbest(timediff_t కీలకం,
                                        నిర్మాణం Curl_వృక్షం *మూలం,
                                        నిర్మాణం Curl_వృక్షం **removed)
{
  నిర్మాణం Curl_వృక్షం *x;

  ఒకవేళ(!మూలం) {
    *removed = ఖాళీ; /* none removed since there was no root */
    తిరిగి ఖాళీ;
  }

  /* find smallest */
  మూలం = splay(TIMEDIFF_T_MIN, మూలం);
  DEBUGASSERT(మూలం);
  ఒకవేళ(కీలకం < మూలం->కీలకం) {
    /* even the smallest is too big */
    *removed = ఖాళీ;
    తిరిగి మూలం;
  }

  /* FIRST! Check if there is a list with identical keys */
  ఒకవేళ(మూలం->same) {
    x = మూలం->same;
    DEBUGASSERT(x->కీలకం == మూలం->కీలకం);
    /* 'x' becomes the new root node */
    x->larger = మూలం->larger;
    x->smaller = మూలం->smaller;
    మూలం->same = ఖాళీ;
    మూలం->registered = FALSE;
    *removed = మూలం;
    తిరిగి x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = మూలం->larger;
  మూలం->registered = FALSE;
  *removed = మూలం;

  తిరిగి x;
}

bool Curl_timeouts_remove_expired(నిర్మాణం Curl_timeouts *timeouts,
                                  స్థిరరాశి నిర్మాణం curltime *ts,
                                  uint32_t *pmid)
{
  ఒకవేళ(timeouts->వృక్షం) {
    నిర్మాణం Curl_వృక్షం *t = ఖాళీ;
    timediff_t elapsed_us = Curl_timeouts_స్థానభ్రంశం_us(timeouts, ts);
    timeouts->వృక్షం = splaygetbest(elapsed_us, timeouts->వృక్షం, &t);
    ఒకవేళ(t) {
      *pmid = t->id;
      తిరిగి TRUE;
    }
  }
  *pmid = UINT32_MAX;
  తిరిగి FALSE;
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
UNITTEST పూర్ణాంకం splayremove(నిర్మాణం Curl_వృక్షం *మూలం,
                         నిర్మాణం Curl_వృక్షం *removenode,
                         నిర్మాణం Curl_వృక్షం **newroot);
UNITTEST పూర్ణాంకం splayremove(నిర్మాణం Curl_వృక్షం *మూలం,
                         నిర్మాణం Curl_వృక్షం *removenode,
                         నిర్మాణం Curl_వృక్షం **newroot)
{
  నిర్మాణం Curl_వృక్షం *x;

  ఒకవేళ(!మూలం)
    తిరిగి 1;

  DEBUGASSERT(removenode);
  ఒకవేళ(!removenode->registered)
    తిరిగి 2;

  మూలం = splay(removenode->కీలకం, మూలం);
  DEBUGASSERT(మూలం);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  ఒకవేళ(మూలం->కీలకం != removenode->కీలకం) {
    DEBUGASSERT(0);
    తిరిగి 2;
  }

  ఒకవేళ(మూలం != removenode) {
    /* Should be in the root->same list then */
    నిర్మాణం Curl_వృక్షం **panchor;
    కొరకు(panchor = &మూలం->same; *panchor; panchor = &(*panchor)->same) {
      ఒకవేళ(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = ఖాళీ;
        removenode->registered = FALSE;
        *newroot = మూలం;
        తిరిగి 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    తిరిగి 2;
  }
  /* removing the root node */
  ఒకవేళ(మూలం->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = మూలం->same;
    x->larger = మూలం->larger;
    x->smaller = మూలం->smaller;
    మూలం->same = ఖాళీ;
  }
  లేకపోతే {
    /* Remove the root node */
    ఒకవేళ(!మూలం->smaller)
      x = మూలం->larger;
    లేకపోతే {
      x = splay(removenode->కీలకం, మూలం->smaller);
      DEBUGASSERT(x);
      x->larger = మూలం->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  తిరిగి 0;
}

bool Curl_timeouts_remove(నిర్మాణం Curl_timeouts *timeouts,
                          నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_వృక్షం *కణుపు = &దత్తాంశం->state.timeouts.splaynode;
  ఒకవేళ(కణుపు->registered) {
    పూర్ణాంకం rc = splayremove(timeouts->వృక్షం, కణుపు, &timeouts->వృక్షం);
#నిర్వచితమైతే DEBUGBUILD
    ఒకవేళ(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#లేకపోతే
    (శూన్య)rc;
#అయితే_ముగింపు
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "splay.h"

സ്ഥിതം ഘടന Curl_വൃക്ഷം *splay(timediff_t താക്കോൽ,
                               ഘടന Curl_വൃക്ഷം *വേര്);

#ifdef UNITTESTS
uint32_t Curl_splayget(ഘടന Curl_വൃക്ഷം *കണ്ണി)
{
  DEBUGASSERT(കണ്ണി);
  തിരികെ കണ്ണി->id;
}

#endif

ശൂന്യം Curl_timeouts_init(ഘടന Curl_timeouts *timeouts,
                        സ്ഥിരം ഘടന curltime *ptime_base)
{
  ഘടന curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->വൃക്ഷം = ശൂന്യ;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(ഘടന Curl_easy *data)
{
  ഘടന Curl_വൃക്ഷം *കണ്ണി = data ? &data->state.timeouts.splaynode : ശൂന്യ;
  തിരികെ കണ്ണി && കണ്ണി->registered;
}

timediff_t Curl_timeouts_offset_us(ഘടന Curl_timeouts *timeouts,
                                   സ്ഥിരം ഘടന curltime *pts)
{
  ഘടന curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  തിരികെ curlx_ptimediff_us(pts, &time_base);
}

പൂർണ്ണസംഖ്യ Curl_timeouts_next_ms(ഘടന Curl_timeouts *timeouts,
                          സ്ഥിരം ഘടന curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  എങ്കിൽ(timeouts->വൃക്ഷം) { /* splay the lowest key to the root */
    timeouts->വൃക്ഷം = splay(TIMEDIFF_T_MIN, timeouts->വൃക്ഷം);
  }

  എങ്കിൽ(timeouts->വൃക്ഷം) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->വൃക്ഷം->താക്കോൽ - elapsed_us;
    എങ്കിൽ(pmid)
      *pmid = timeouts->വൃക്ഷം->id;
    എങ്കിൽ(pexpire_offset_us)
      *pexpire_offset_us = timeouts->വൃക്ഷം->താക്കോൽ;
    എങ്കിൽ(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      തിരികെ (ms > INT_MAX) ? INT_MAX : (പൂർണ്ണസംഖ്യ)ms;
    }
    അല്ലെങ്കിൽ /* has expired */
      തിരികെ 0;
  }
  എങ്കിൽ(pmid)
    *pmid = UINT32_MAX;
  എങ്കിൽ(pexpire_offset_us)
    *pexpire_offset_us = 0;
  തിരികെ -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
ഘടന Curl_വൃക്ഷം *splay(timediff_t താക്കോൽ,
                             ഘടന Curl_വൃക്ഷം *വേര്)
{
  ഘടന Curl_വൃക്ഷം N, *l, *r, *y;

  എങ്കിൽ(!വേര്)
    തിരികെ ശൂന്യ;
  N.smaller = N.larger = ശൂന്യ;
  l = r = &N;

  വേണ്ടി(;;) {
    എങ്കിൽ(താക്കോൽ < വേര്->താക്കോൽ) {
      /* key is somewhere in root->smaller branch */
      എങ്കിൽ(!വേര്->smaller)  /* which is empty, done */
        നിർത്തുക;
      എങ്കിൽ(താക്കോൽ < വേര്->smaller->താക്കോൽ) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = വേര്->smaller;
        വേര്->smaller = y->larger;
        y->larger = വേര്;
        വേര് = y;
        എങ്കിൽ(!വേര്->smaller)
          നിർത്തുക;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = വേര്;
      r = വേര്;
      വേര് = വേര്->smaller;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(താക്കോൽ > വേര്->താക്കോൽ) {
      /* key is somewhere in root->larger branch */
      എങ്കിൽ(!വേര്->larger)  /* which is empty, done */
        നിർത്തുക;
      എങ്കിൽ(താക്കോൽ > വേര്->larger->താക്കോൽ) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = വേര്->larger;
        വേര്->larger = y->smaller;
        y->smaller = വേര്;
        വേര് = y;
        എങ്കിൽ(!വേര്->larger)
          നിർത്തുക;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = വേര്;
      l = വേര്;
      വേര് = വേര്->larger;
    }
    അല്ലെങ്കിൽ  /* exact match, root is key, done */
      നിർത്തുക;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = വേര്->smaller;
  r->smaller = വേര്->larger;
  വേര്->smaller = N.larger;
  വേര്->larger = N.smaller;

  തിരികെ വേര്;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST ഘടന Curl_വൃക്ഷം *splayinsert(timediff_t താക്കോൽ,
                                       ഘടന Curl_വൃക്ഷം *വേര്,
                                       ഘടന Curl_വൃക്ഷം *കണ്ണി,
                                       uint32_t id);
UNITTEST ഘടന Curl_വൃക്ഷം *splayinsert(timediff_t താക്കോൽ,
                                       ഘടന Curl_വൃക്ഷം *വേര്,
                                       ഘടന Curl_വൃക്ഷം *കണ്ണി,
                                       uint32_t id)
{
  DEBUGASSERT(കണ്ണി);

  കണ്ണി->താക്കോൽ = താക്കോൽ;
  കണ്ണി->id = id;
  കണ്ണി->same = ശൂന്യ;
  കണ്ണി->registered = TRUE;
  എങ്കിൽ(വേര്) {
    വേര് = splay(താക്കോൽ, വേര്);
    DEBUGASSERT(വേര്);
    എങ്കിൽ(താക്കോൽ == വേര്->താക്കോൽ) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      ഘടന Curl_വൃക്ഷം **panchor = &വേര്->same;
      ആകുമ്പോൾ(*panchor)
        panchor = &(*panchor)->same;
      *panchor = കണ്ണി;
      തിരികെ വേര്; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  എങ്കിൽ(!വേര്) {
    കണ്ണി->smaller = കണ്ണി->larger = ശൂന്യ;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(താക്കോൽ < വേര്->താക്കോൽ) {
    കണ്ണി->smaller = വേര്->smaller;
    കണ്ണി->larger = വേര്;
    വേര്->smaller = ശൂന്യ;
  }
  അല്ലെങ്കിൽ {
    കണ്ണി->larger = വേര്->larger;
    കണ്ണി->smaller = വേര്;
    വേര്->larger = ശൂന്യ;
  }

  തിരികെ കണ്ണി;
}

ശൂന്യം Curl_timeouts_add(ഘടന Curl_timeouts *timeouts,
                       ഘടന Curl_easy *data,
                       timediff_t offset_us)
{
  ഘടന Curl_വൃക്ഷം *കണ്ണി = &data->state.timeouts.splaynode;
  DEBUGASSERT(!കണ്ണി->registered);
  timeouts->വൃക്ഷം = splayinsert(offset_us, timeouts->വൃക്ഷം,
                               കണ്ണി, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST ഘടന Curl_വൃക്ഷം *splaygetbest(timediff_t താക്കോൽ,
                                        ഘടന Curl_വൃക്ഷം *വേര്,
                                        ഘടന Curl_വൃക്ഷം **removed);
UNITTEST ഘടന Curl_വൃക്ഷം *splaygetbest(timediff_t താക്കോൽ,
                                        ഘടന Curl_വൃക്ഷം *വേര്,
                                        ഘടന Curl_വൃക്ഷം **removed)
{
  ഘടന Curl_വൃക്ഷം *x;

  എങ്കിൽ(!വേര്) {
    *removed = ശൂന്യ; /* none removed since there was no root */
    തിരികെ ശൂന്യ;
  }

  /* find smallest */
  വേര് = splay(TIMEDIFF_T_MIN, വേര്);
  DEBUGASSERT(വേര്);
  എങ്കിൽ(താക്കോൽ < വേര്->താക്കോൽ) {
    /* even the smallest is too big */
    *removed = ശൂന്യ;
    തിരികെ വേര്;
  }

  /* FIRST! Check if there is a list with identical keys */
  എങ്കിൽ(വേര്->same) {
    x = വേര്->same;
    DEBUGASSERT(x->താക്കോൽ == വേര്->താക്കോൽ);
    /* 'x' becomes the new root node */
    x->larger = വേര്->larger;
    x->smaller = വേര്->smaller;
    വേര്->same = ശൂന്യ;
    വേര്->registered = FALSE;
    *removed = വേര്;
    തിരികെ x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = വേര്->larger;
  വേര്->registered = FALSE;
  *removed = വേര്;

  തിരികെ x;
}

bool Curl_timeouts_remove_expired(ഘടന Curl_timeouts *timeouts,
                                  സ്ഥിരം ഘടന curltime *ts,
                                  uint32_t *pmid)
{
  എങ്കിൽ(timeouts->വൃക്ഷം) {
    ഘടന Curl_വൃക്ഷം *t = ശൂന്യ;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->വൃക്ഷം = splaygetbest(elapsed_us, timeouts->വൃക്ഷം, &t);
    എങ്കിൽ(t) {
      *pmid = t->id;
      തിരികെ TRUE;
    }
  }
  *pmid = UINT32_MAX;
  തിരികെ FALSE;
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
UNITTEST പൂർണ്ണസംഖ്യ splayremove(ഘടന Curl_വൃക്ഷം *വേര്,
                         ഘടന Curl_വൃക്ഷം *removenode,
                         ഘടന Curl_വൃക്ഷം **newroot);
UNITTEST പൂർണ്ണസംഖ്യ splayremove(ഘടന Curl_വൃക്ഷം *വേര്,
                         ഘടന Curl_വൃക്ഷം *removenode,
                         ഘടന Curl_വൃക്ഷം **newroot)
{
  ഘടന Curl_വൃക്ഷം *x;

  എങ്കിൽ(!വേര്)
    തിരികെ 1;

  DEBUGASSERT(removenode);
  എങ്കിൽ(!removenode->registered)
    തിരികെ 2;

  വേര് = splay(removenode->താക്കോൽ, വേര്);
  DEBUGASSERT(വേര്);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  എങ്കിൽ(വേര്->താക്കോൽ != removenode->താക്കോൽ) {
    DEBUGASSERT(0);
    തിരികെ 2;
  }

  എങ്കിൽ(വേര് != removenode) {
    /* Should be in the root->same list then */
    ഘടന Curl_വൃക്ഷം **panchor;
    വേണ്ടി(panchor = &വേര്->same; *panchor; panchor = &(*panchor)->same) {
      എങ്കിൽ(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = ശൂന്യ;
        removenode->registered = FALSE;
        *newroot = വേര്;
        തിരികെ 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    തിരികെ 2;
  }
  /* removing the root node */
  എങ്കിൽ(വേര്->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = വേര്->same;
    x->larger = വേര്->larger;
    x->smaller = വേര്->smaller;
    വേര്->same = ശൂന്യ;
  }
  അല്ലെങ്കിൽ {
    /* Remove the root node */
    എങ്കിൽ(!വേര്->smaller)
      x = വേര്->larger;
    അല്ലെങ്കിൽ {
      x = splay(removenode->താക്കോൽ, വേര്->smaller);
      DEBUGASSERT(x);
      x->larger = വേര്->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  തിരികെ 0;
}

bool Curl_timeouts_remove(ഘടന Curl_timeouts *timeouts,
                          ഘടന Curl_easy *data)
{
  ഘടന Curl_വൃക്ഷം *കണ്ണി = &data->state.timeouts.splaynode;
  എങ്കിൽ(കണ്ണി->registered) {
    പൂർണ്ണസംഖ്യ rc = splayremove(timeouts->വൃക്ഷം, കണ്ണി, &timeouts->വൃക്ഷം);
#ifdef DEBUGBUILD
    എങ്കിൽ(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#അല്ലെങ്കിൽ
    (ശൂന്യം)rc;
#endif
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

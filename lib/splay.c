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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "splay.h"

સ્થિર સંરચના Curl_વૃક્ષ *splay(timediff_t ચાવી,
                               સંરચના Curl_વૃક્ષ *મૂળ);

#ifdef UNITTESTS
uint32_t Curl_splayget(સંરચના Curl_વૃક્ષ *ગાંઠ)
{
  DEBUGASSERT(ગાંઠ);
  પાછા ગાંઠ->id;
}

#endif

શૂન્ય Curl_timeouts_init(સંરચના Curl_timeouts *timeouts,
                        અચલ સંરચના curltime *ptime_base)
{
  સંરચના curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->વૃક્ષ = ખાલી;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(સંરચના Curl_easy *data)
{
  સંરચના Curl_વૃક્ષ *ગાંઠ = data ? &data->state.timeouts.splaynode : ખાલી;
  પાછા ગાંઠ && ગાંઠ->registered;
}

timediff_t Curl_timeouts_offset_us(સંરચના Curl_timeouts *timeouts,
                                   અચલ સંરચના curltime *pts)
{
  સંરચના curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  પાછા curlx_ptimediff_us(pts, &time_base);
}

પૂર્ણાંક Curl_timeouts_next_ms(સંરચના Curl_timeouts *timeouts,
                          અચલ સંરચના curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  જો(timeouts->વૃક્ષ) { /* splay the lowest key to the root */
    timeouts->વૃક્ષ = splay(TIMEDIFF_T_MIN, timeouts->વૃક્ષ);
  }

  જો(timeouts->વૃક્ષ) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->વૃક્ષ->ચાવી - elapsed_us;
    જો(pmid)
      *pmid = timeouts->વૃક્ષ->id;
    જો(pexpire_offset_us)
      *pexpire_offset_us = timeouts->વૃક્ષ->ચાવી;
    જો(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      પાછા (ms > INT_MAX) ? INT_MAX : (પૂર્ણાંક)ms;
    }
    નહીંતર /* has expired */
      પાછા 0;
  }
  જો(pmid)
    *pmid = UINT32_MAX;
  જો(pexpire_offset_us)
    *pexpire_offset_us = 0;
  પાછા -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
સંરચના Curl_વૃક્ષ *splay(timediff_t ચાવી,
                             સંરચના Curl_વૃક્ષ *મૂળ)
{
  સંરચના Curl_વૃક્ષ N, *l, *r, *y;

  જો(!મૂળ)
    પાછા ખાલી;
  N.smaller = N.larger = ખાલી;
  l = r = &N;

  માટે(;;) {
    જો(ચાવી < મૂળ->ચાવી) {
      /* key is somewhere in root->smaller branch */
      જો(!મૂળ->smaller)  /* which is empty, done */
        તોડો;
      જો(ચાવી < મૂળ->smaller->ચાવી) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = મૂળ->smaller;
        મૂળ->smaller = y->larger;
        y->larger = મૂળ;
        મૂળ = y;
        જો(!મૂળ->smaller)
          તોડો;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = મૂળ;
      r = મૂળ;
      મૂળ = મૂળ->smaller;
    }
    નહીંતર જો(ચાવી > મૂળ->ચાવી) {
      /* key is somewhere in root->larger branch */
      જો(!મૂળ->larger)  /* which is empty, done */
        તોડો;
      જો(ચાવી > મૂળ->larger->ચાવી) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = મૂળ->larger;
        મૂળ->larger = y->smaller;
        y->smaller = મૂળ;
        મૂળ = y;
        જો(!મૂળ->larger)
          તોડો;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = મૂળ;
      l = મૂળ;
      મૂળ = મૂળ->larger;
    }
    નહીંતર  /* exact match, root is key, done */
      તોડો;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = મૂળ->smaller;
  r->smaller = મૂળ->larger;
  મૂળ->smaller = N.larger;
  મૂળ->larger = N.smaller;

  પાછા મૂળ;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST સંરચના Curl_વૃક્ષ *splayinsert(timediff_t ચાવી,
                                       સંરચના Curl_વૃક્ષ *મૂળ,
                                       સંરચના Curl_વૃક્ષ *ગાંઠ,
                                       uint32_t id);
UNITTEST સંરચના Curl_વૃક્ષ *splayinsert(timediff_t ચાવી,
                                       સંરચના Curl_વૃક્ષ *મૂળ,
                                       સંરચના Curl_વૃક્ષ *ગાંઠ,
                                       uint32_t id)
{
  DEBUGASSERT(ગાંઠ);

  ગાંઠ->ચાવી = ચાવી;
  ગાંઠ->id = id;
  ગાંઠ->same = ખાલી;
  ગાંઠ->registered = TRUE;
  જો(મૂળ) {
    મૂળ = splay(ચાવી, મૂળ);
    DEBUGASSERT(મૂળ);
    જો(ચાવી == મૂળ->ચાવી) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      સંરચના Curl_વૃક્ષ **panchor = &મૂળ->same;
      જ્યારે(*panchor)
        panchor = &(*panchor)->same;
      *panchor = ગાંઠ;
      પાછા મૂળ; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  જો(!મૂળ) {
    ગાંઠ->smaller = ગાંઠ->larger = ખાલી;
  }
  નહીંતર જો(ચાવી < મૂળ->ચાવી) {
    ગાંઠ->smaller = મૂળ->smaller;
    ગાંઠ->larger = મૂળ;
    મૂળ->smaller = ખાલી;
  }
  નહીંતર {
    ગાંઠ->larger = મૂળ->larger;
    ગાંઠ->smaller = મૂળ;
    મૂળ->larger = ખાલી;
  }

  પાછા ગાંઠ;
}

શૂન્ય Curl_timeouts_add(સંરચના Curl_timeouts *timeouts,
                       સંરચના Curl_easy *data,
                       timediff_t offset_us)
{
  સંરચના Curl_વૃક્ષ *ગાંઠ = &data->state.timeouts.splaynode;
  DEBUGASSERT(!ગાંઠ->registered);
  timeouts->વૃક્ષ = splayinsert(offset_us, timeouts->વૃક્ષ,
                               ગાંઠ, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST સંરચના Curl_વૃક્ષ *splaygetbest(timediff_t ચાવી,
                                        સંરચના Curl_વૃક્ષ *મૂળ,
                                        સંરચના Curl_વૃક્ષ **removed);
UNITTEST સંરચના Curl_વૃક્ષ *splaygetbest(timediff_t ચાવી,
                                        સંરચના Curl_વૃક્ષ *મૂળ,
                                        સંરચના Curl_વૃક્ષ **removed)
{
  સંરચના Curl_વૃક્ષ *x;

  જો(!મૂળ) {
    *removed = ખાલી; /* none removed since there was no root */
    પાછા ખાલી;
  }

  /* find smallest */
  મૂળ = splay(TIMEDIFF_T_MIN, મૂળ);
  DEBUGASSERT(મૂળ);
  જો(ચાવી < મૂળ->ચાવી) {
    /* even the smallest is too big */
    *removed = ખાલી;
    પાછા મૂળ;
  }

  /* FIRST! Check if there is a list with identical keys */
  જો(મૂળ->same) {
    x = મૂળ->same;
    DEBUGASSERT(x->ચાવી == મૂળ->ચાવી);
    /* 'x' becomes the new root node */
    x->larger = મૂળ->larger;
    x->smaller = મૂળ->smaller;
    મૂળ->same = ખાલી;
    મૂળ->registered = FALSE;
    *removed = મૂળ;
    પાછા x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = મૂળ->larger;
  મૂળ->registered = FALSE;
  *removed = મૂળ;

  પાછા x;
}

bool Curl_timeouts_remove_expired(સંરચના Curl_timeouts *timeouts,
                                  અચલ સંરચના curltime *ts,
                                  uint32_t *pmid)
{
  જો(timeouts->વૃક્ષ) {
    સંરચના Curl_વૃક્ષ *t = ખાલી;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->વૃક્ષ = splaygetbest(elapsed_us, timeouts->વૃક્ષ, &t);
    જો(t) {
      *pmid = t->id;
      પાછા TRUE;
    }
  }
  *pmid = UINT32_MAX;
  પાછા FALSE;
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
UNITTEST પૂર્ણાંક splayremove(સંરચના Curl_વૃક્ષ *મૂળ,
                         સંરચના Curl_વૃક્ષ *removenode,
                         સંરચના Curl_વૃક્ષ **newroot);
UNITTEST પૂર્ણાંક splayremove(સંરચના Curl_વૃક્ષ *મૂળ,
                         સંરચના Curl_વૃક્ષ *removenode,
                         સંરચના Curl_વૃક્ષ **newroot)
{
  સંરચના Curl_વૃક્ષ *x;

  જો(!મૂળ)
    પાછા 1;

  DEBUGASSERT(removenode);
  જો(!removenode->registered)
    પાછા 2;

  મૂળ = splay(removenode->ચાવી, મૂળ);
  DEBUGASSERT(મૂળ);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  જો(મૂળ->ચાવી != removenode->ચાવી) {
    DEBUGASSERT(0);
    પાછા 2;
  }

  જો(મૂળ != removenode) {
    /* Should be in the root->same list then */
    સંરચના Curl_વૃક્ષ **panchor;
    માટે(panchor = &મૂળ->same; *panchor; panchor = &(*panchor)->same) {
      જો(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = ખાલી;
        removenode->registered = FALSE;
        *newroot = મૂળ;
        પાછા 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    પાછા 2;
  }
  /* removing the root node */
  જો(મૂળ->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = મૂળ->same;
    x->larger = મૂળ->larger;
    x->smaller = મૂળ->smaller;
    મૂળ->same = ખાલી;
  }
  નહીંતર {
    /* Remove the root node */
    જો(!મૂળ->smaller)
      x = મૂળ->larger;
    નહીંતર {
      x = splay(removenode->ચાવી, મૂળ->smaller);
      DEBUGASSERT(x);
      x->larger = મૂળ->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  પાછા 0;
}

bool Curl_timeouts_remove(સંરચના Curl_timeouts *timeouts,
                          સંરચના Curl_easy *data)
{
  સંરચના Curl_વૃક્ષ *ગાંઠ = &data->state.timeouts.splaynode;
  જો(ગાંઠ->registered) {
    પૂર્ણાંક rc = splayremove(timeouts->વૃક્ષ, ગાંઠ, &timeouts->વૃક્ષ);
#ifdef DEBUGBUILD
    જો(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#નહીંતર
    (શૂન્ય)rc;
#endif
    પાછા TRUE;
  }
  પાછા FALSE;
}

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
#include "curl_setup.h"

#include "urldata.h"
#include "splay.h"

static sa_nracanaa Curl_tree *splay(timediff_t key,
                               sa_nracanaa Curl_tree *root);

#ifdef UNITTESTS
uint32_t Curl_splayget(sa_nracanaa Curl_tree *node)
{
  DEBUGASSERT(node);
  ආපසු node->id;
}

#endif

void Curl_timeouts_init(sa_nracanaa Curl_timeouts *timeouts,
                        const sa_nracanaa curltime *ptime_base)
{
  sa_nracanaa curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->tree = NULL;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_tree *node = data ? &data->state.timeouts.splaynode : NULL;
  ආපසු node && node->registered;
}

timediff_t Curl_timeouts_offset_us(sa_nracanaa Curl_timeouts *timeouts,
                                   const sa_nracanaa curltime *pts)
{
  sa_nracanaa curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  ආපසු curlx_ptimediff_us(pts, &time_base);
}

පූර්ණ_සංඛ්‍යා Curl_timeouts_next_ms(sa_nracanaa Curl_timeouts *timeouts,
                          const sa_nracanaa curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  නම්(timeouts->tree) { /* splay the lowest key to the root */
    timeouts->tree = splay(TIMEDIFF_T_MIN, timeouts->tree);
  }

  නම්(timeouts->tree) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->tree->key - elapsed_us;
    නම්(pmid)
      *pmid = timeouts->tree->id;
    නම්(pexpire_offset_us)
      *pexpire_offset_us = timeouts->tree->key;
    නම්(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      ආපසු (ms > INT_MAX) ? INT_MAX : (පූර්ණ_සංඛ්‍යා)ms;
    }
    නැතහොත් /* has expired */
      ආපසු 0;
  }
  නම්(pmid)
    *pmid = UINT32_MAX;
  නම්(pexpire_offset_us)
    *pexpire_offset_us = 0;
  ආපසු -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
sa_nracanaa Curl_tree *splay(timediff_t key,
                             sa_nracanaa Curl_tree *root)
{
  sa_nracanaa Curl_tree N, *l, *r, *y;

  නම්(!root)
    ආපසු NULL;
  N.smaller = N.larger = NULL;
  l = r = &N;

  සඳහා(;;) {
    නම්(key < root->key) {
      /* key is somewhere in root->smaller branch */
      නම්(!root->smaller)  /* which is empty, done */
        bha_nga;
      නම්(key < root->smaller->key) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = root->smaller;
        root->smaller = y->larger;
        y->larger = root;
        root = y;
        නම්(!root->smaller)
          bha_nga;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = root;
      r = root;
      root = root->smaller;
    }
    නැතහොත් නම්(key > root->key) {
      /* key is somewhere in root->larger branch */
      නම්(!root->larger)  /* which is empty, done */
        bha_nga;
      නම්(key > root->larger->key) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = root->larger;
        root->larger = y->smaller;
        y->smaller = root;
        root = y;
        නම්(!root->larger)
          bha_nga;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = root;
      l = root;
      root = root->larger;
    }
    නැතහොත්  /* exact match, root is key, done */
      bha_nga;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = root->smaller;
  r->smaller = root->larger;
  root->smaller = N.larger;
  root->larger = N.smaller;

  ආපසු root;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST sa_nracanaa Curl_tree *splayinsert(timediff_t key,
                                       sa_nracanaa Curl_tree *root,
                                       sa_nracanaa Curl_tree *node,
                                       uint32_t id);
UNITTEST sa_nracanaa Curl_tree *splayinsert(timediff_t key,
                                       sa_nracanaa Curl_tree *root,
                                       sa_nracanaa Curl_tree *node,
                                       uint32_t id)
{
  DEBUGASSERT(node);

  node->key = key;
  node->id = id;
  node->same = NULL;
  node->registered = TRUE;
  නම්(root) {
    root = splay(key, root);
    DEBUGASSERT(root);
    නම්(key == root->key) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      sa_nracanaa Curl_tree **panchor = &root->same;
      අතරතුර(*panchor)
        panchor = &(*panchor)->same;
      *panchor = node;
      ආපසු root; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  නම්(!root) {
    node->smaller = node->larger = NULL;
  }
  නැතහොත් නම්(key < root->key) {
    node->smaller = root->smaller;
    node->larger = root;
    root->smaller = NULL;
  }
  නැතහොත් {
    node->larger = root->larger;
    node->smaller = root;
    root->larger = NULL;
  }

  ආපසු node;
}

void Curl_timeouts_add(sa_nracanaa Curl_timeouts *timeouts,
                       sa_nracanaa Curl_easy *data,
                       timediff_t offset_us)
{
  sa_nracanaa Curl_tree *node = &data->state.timeouts.splaynode;
  DEBUGASSERT(!node->registered);
  timeouts->tree = splayinsert(offset_us, timeouts->tree,
                               node, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST sa_nracanaa Curl_tree *splaygetbest(timediff_t key,
                                        sa_nracanaa Curl_tree *root,
                                        sa_nracanaa Curl_tree **removed);
UNITTEST sa_nracanaa Curl_tree *splaygetbest(timediff_t key,
                                        sa_nracanaa Curl_tree *root,
                                        sa_nracanaa Curl_tree **removed)
{
  sa_nracanaa Curl_tree *x;

  නම්(!root) {
    *removed = NULL; /* none removed since there was no root */
    ආපසු NULL;
  }

  /* find smallest */
  root = splay(TIMEDIFF_T_MIN, root);
  DEBUGASSERT(root);
  නම්(key < root->key) {
    /* even the smallest is too big */
    *removed = NULL;
    ආපසු root;
  }

  /* FIRST! Check if there is a list with identical keys */
  නම්(root->same) {
    x = root->same;
    DEBUGASSERT(x->key == root->key);
    /* 'x' becomes the new root node */
    x->larger = root->larger;
    x->smaller = root->smaller;
    root->same = NULL;
    root->registered = FALSE;
    *removed = root;
    ආපසු x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = root->larger;
  root->registered = FALSE;
  *removed = root;

  ආපසු x;
}

bool Curl_timeouts_remove_expired(sa_nracanaa Curl_timeouts *timeouts,
                                  const sa_nracanaa curltime *ts,
                                  uint32_t *pmid)
{
  නම්(timeouts->tree) {
    sa_nracanaa Curl_tree *t = NULL;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->tree = splaygetbest(elapsed_us, timeouts->tree, &t);
    නම්(t) {
      *pmid = t->id;
      ආපසු TRUE;
    }
  }
  *pmid = UINT32_MAX;
  ආපසු FALSE;
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
UNITTEST පූර්ණ_සංඛ්‍යා splayremove(sa_nracanaa Curl_tree *root,
                         sa_nracanaa Curl_tree *removenode,
                         sa_nracanaa Curl_tree **newroot);
UNITTEST පූර්ණ_සංඛ්‍යා splayremove(sa_nracanaa Curl_tree *root,
                         sa_nracanaa Curl_tree *removenode,
                         sa_nracanaa Curl_tree **newroot)
{
  sa_nracanaa Curl_tree *x;

  නම්(!root)
    ආපසු 1;

  DEBUGASSERT(removenode);
  නම්(!removenode->registered)
    ආපසු 2;

  root = splay(removenode->key, root);
  DEBUGASSERT(root);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  නම්(root->key != removenode->key) {
    DEBUGASSERT(0);
    ආපසු 2;
  }

  නම්(root != removenode) {
    /* Should be in the root->same list then */
    sa_nracanaa Curl_tree **panchor;
    සඳහා(panchor = &root->same; *panchor; panchor = &(*panchor)->same) {
      නම්(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = NULL;
        removenode->registered = FALSE;
        *newroot = root;
        ආපසු 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    ආපසු 2;
  }
  /* removing the root node */
  නම්(root->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = root->same;
    x->larger = root->larger;
    x->smaller = root->smaller;
    root->same = NULL;
  }
  නැතහොත් {
    /* Remove the root node */
    නම්(!root->smaller)
      x = root->larger;
    නැතහොත් {
      x = splay(removenode->key, root->smaller);
      DEBUGASSERT(x);
      x->larger = root->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  ආපසු 0;
}

bool Curl_timeouts_remove(sa_nracanaa Curl_timeouts *timeouts,
                          sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_tree *node = &data->state.timeouts.splaynode;
  නම්(node->registered) {
    පූර්ණ_සංඛ්‍යා rc = splayremove(timeouts->tree, node, &timeouts->tree);
#ifdef DEBUGBUILD
    නම්(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#නැතහොත්
    (void)rc;
#endif
    ආපසු TRUE;
  }
  ආපසු FALSE;
}

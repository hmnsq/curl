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

static struct Curl_tree *splay(timediff_t key,
                               struct Curl_tree *root);

#ifdef UNITTESTS
uint32_t Curl_splayget(struct Curl_tree *node)
{
  DEBUGASSERT(node);
  반환 node->id;
}

#endif

void Curl_timeouts_init(struct Curl_timeouts *timeouts,
                        const struct curltime *ptime_base)
{
  struct curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->tree = NULL;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(struct Curl_easy *data)
{
  struct Curl_tree *node = data ? &data->state.timeouts.splaynode : NULL;
  반환 node && node->registered;
}

timediff_t Curl_timeouts_offset_us(struct Curl_timeouts *timeouts,
                                   const struct curltime *pts)
{
  struct curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  반환 curlx_ptimediff_us(pts, &time_base);
}

정수 Curl_timeouts_next_ms(struct Curl_timeouts *timeouts,
                          const struct curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  만약(timeouts->tree) { /* splay the lowest key to the root */
    timeouts->tree = splay(TIMEDIFF_T_MIN, timeouts->tree);
  }

  만약(timeouts->tree) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->tree->key - elapsed_us;
    만약(pmid)
      *pmid = timeouts->tree->id;
    만약(pexpire_offset_us)
      *pexpire_offset_us = timeouts->tree->key;
    만약(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      반환 (ms > INT_MAX) ? INT_MAX : (정수)ms;
    }
    아니면 /* has expired */
      반환 0;
  }
  만약(pmid)
    *pmid = UINT32_MAX;
  만약(pexpire_offset_us)
    *pexpire_offset_us = 0;
  반환 -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
struct Curl_tree *splay(timediff_t key,
                             struct Curl_tree *root)
{
  struct Curl_tree N, *l, *r, *y;

  만약(!root)
    반환 NULL;
  N.smaller = N.larger = NULL;
  l = r = &N;

  위해(;;) {
    만약(key < root->key) {
      /* key is somewhere in root->smaller branch */
      만약(!root->smaller)  /* which is empty, done */
        break;
      만약(key < root->smaller->key) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = root->smaller;
        root->smaller = y->larger;
        y->larger = root;
        root = y;
        만약(!root->smaller)
          break;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = root;
      r = root;
      root = root->smaller;
    }
    아니면 만약(key > root->key) {
      /* key is somewhere in root->larger branch */
      만약(!root->larger)  /* which is empty, done */
        break;
      만약(key > root->larger->key) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = root->larger;
        root->larger = y->smaller;
        y->smaller = root;
        root = y;
        만약(!root->larger)
          break;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = root;
      l = root;
      root = root->larger;
    }
    아니면  /* exact match, root is key, done */
      break;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = root->smaller;
  r->smaller = root->larger;
  root->smaller = N.larger;
  root->larger = N.smaller;

  반환 root;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST struct Curl_tree *splayinsert(timediff_t key,
                                       struct Curl_tree *root,
                                       struct Curl_tree *node,
                                       uint32_t id);
UNITTEST struct Curl_tree *splayinsert(timediff_t key,
                                       struct Curl_tree *root,
                                       struct Curl_tree *node,
                                       uint32_t id)
{
  DEBUGASSERT(node);

  node->key = key;
  node->id = id;
  node->same = NULL;
  node->registered = TRUE;
  만약(root) {
    root = splay(key, root);
    DEBUGASSERT(root);
    만약(key == root->key) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      struct Curl_tree **panchor = &root->same;
      동안(*panchor)
        panchor = &(*panchor)->same;
      *panchor = node;
      반환 root; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  만약(!root) {
    node->smaller = node->larger = NULL;
  }
  아니면 만약(key < root->key) {
    node->smaller = root->smaller;
    node->larger = root;
    root->smaller = NULL;
  }
  아니면 {
    node->larger = root->larger;
    node->smaller = root;
    root->larger = NULL;
  }

  반환 node;
}

void Curl_timeouts_add(struct Curl_timeouts *timeouts,
                       struct Curl_easy *data,
                       timediff_t offset_us)
{
  struct Curl_tree *node = &data->state.timeouts.splaynode;
  DEBUGASSERT(!node->registered);
  timeouts->tree = splayinsert(offset_us, timeouts->tree,
                               node, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST struct Curl_tree *splaygetbest(timediff_t key,
                                        struct Curl_tree *root,
                                        struct Curl_tree **removed);
UNITTEST struct Curl_tree *splaygetbest(timediff_t key,
                                        struct Curl_tree *root,
                                        struct Curl_tree **removed)
{
  struct Curl_tree *x;

  만약(!root) {
    *removed = NULL; /* none removed since there was no root */
    반환 NULL;
  }

  /* find smallest */
  root = splay(TIMEDIFF_T_MIN, root);
  DEBUGASSERT(root);
  만약(key < root->key) {
    /* even the smallest is too big */
    *removed = NULL;
    반환 root;
  }

  /* FIRST! Check if there is a list with identical keys */
  만약(root->same) {
    x = root->same;
    DEBUGASSERT(x->key == root->key);
    /* 'x' becomes the new root node */
    x->larger = root->larger;
    x->smaller = root->smaller;
    root->same = NULL;
    root->registered = FALSE;
    *removed = root;
    반환 x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = root->larger;
  root->registered = FALSE;
  *removed = root;

  반환 x;
}

bool Curl_timeouts_remove_expired(struct Curl_timeouts *timeouts,
                                  const struct curltime *ts,
                                  uint32_t *pmid)
{
  만약(timeouts->tree) {
    struct Curl_tree *t = NULL;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->tree = splaygetbest(elapsed_us, timeouts->tree, &t);
    만약(t) {
      *pmid = t->id;
      반환 TRUE;
    }
  }
  *pmid = UINT32_MAX;
  반환 FALSE;
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
UNITTEST 정수 splayremove(struct Curl_tree *root,
                         struct Curl_tree *removenode,
                         struct Curl_tree **newroot);
UNITTEST 정수 splayremove(struct Curl_tree *root,
                         struct Curl_tree *removenode,
                         struct Curl_tree **newroot)
{
  struct Curl_tree *x;

  만약(!root)
    반환 1;

  DEBUGASSERT(removenode);
  만약(!removenode->registered)
    반환 2;

  root = splay(removenode->key, root);
  DEBUGASSERT(root);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  만약(root->key != removenode->key) {
    DEBUGASSERT(0);
    반환 2;
  }

  만약(root != removenode) {
    /* Should be in the root->same list then */
    struct Curl_tree **panchor;
    위해(panchor = &root->same; *panchor; panchor = &(*panchor)->same) {
      만약(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = NULL;
        removenode->registered = FALSE;
        *newroot = root;
        반환 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    반환 2;
  }
  /* removing the root node */
  만약(root->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = root->same;
    x->larger = root->larger;
    x->smaller = root->smaller;
    root->same = NULL;
  }
  아니면 {
    /* Remove the root node */
    만약(!root->smaller)
      x = root->larger;
    아니면 {
      x = splay(removenode->key, root->smaller);
      DEBUGASSERT(x);
      x->larger = root->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  반환 0;
}

bool Curl_timeouts_remove(struct Curl_timeouts *timeouts,
                          struct Curl_easy *data)
{
  struct Curl_tree *node = &data->state.timeouts.splaynode;
  만약(node->registered) {
    정수 rc = splayremove(timeouts->tree, node, &timeouts->tree);
#ifdef DEBUGBUILD
    만약(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#아니면
    (void)rc;
#endif
    반환 TRUE;
  }
  반환 FALSE;
}

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

static struct Curl_पुनो *splay(timediff_t सुसि,
                               struct Curl_पुनो *उगात);

#ifdef UNITTESTS
uint32_t Curl_splayget(struct Curl_पुनो *बुहोल)
{
  DEBUGASSERT(बुहोल);
  return बुहोल->id;
}

#endif

void Curl_timeouts_init(struct Curl_timeouts *timeouts,
                        const struct curltime *ptime_base)
{
  struct curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->पुनो = NULL;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(struct Curl_easy *data)
{
  struct Curl_पुनो *बुहोल = data ? &data->state.timeouts.splaynode : NULL;
  return बुहोल && बुहोल->registered;
}

timediff_t Curl_timeouts_offset_us(struct Curl_timeouts *timeouts,
                                   const struct curltime *pts)
{
  struct curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  return curlx_ptimediff_us(pts, &time_base);
}

int Curl_timeouts_next_ms(struct Curl_timeouts *timeouts,
                          const struct curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  if(timeouts->पुनो) { /* splay the lowest key to the root */
    timeouts->पुनो = splay(TIMEDIFF_T_MIN, timeouts->पुनो);
  }

  if(timeouts->पुनो) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->पुनो->सुसि - elapsed_us;
    if(pmid)
      *pmid = timeouts->पुनो->id;
    if(pexpire_offset_us)
      *pexpire_offset_us = timeouts->पुनो->सुसि;
    if(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      return (ms > INT_MAX) ? INT_MAX : (int)ms;
    }
    else /* has expired */
      return 0;
  }
  if(pmid)
    *pmid = UINT32_MAX;
  if(pexpire_offset_us)
    *pexpire_offset_us = 0;
  return -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
struct Curl_पुनो *splay(timediff_t सुसि,
                             struct Curl_पुनो *उगात)
{
  struct Curl_पुनो N, *l, *r, *y;

  if(!उगात)
    return NULL;
  N.smaller = N.larger = NULL;
  l = r = &N;

  for(;;) {
    if(सुसि < उगात->सुसि) {
      /* key is somewhere in root->smaller branch */
      if(!उगात->smaller)  /* which is empty, done */
        break;
      if(सुसि < उगात->smaller->सुसि) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = उगात->smaller;
        उगात->smaller = y->larger;
        y->larger = उगात;
        उगात = y;
        if(!उगात->smaller)
          break;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = उगात;
      r = उगात;
      उगात = उगात->smaller;
    }
    else if(सुसि > उगात->सुसि) {
      /* key is somewhere in root->larger branch */
      if(!उगात->larger)  /* which is empty, done */
        break;
      if(सुसि > उगात->larger->सुसि) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = उगात->larger;
        उगात->larger = y->smaller;
        y->smaller = उगात;
        उगात = y;
        if(!उगात->larger)
          break;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = उगात;
      l = उगात;
      उगात = उगात->larger;
    }
    else  /* exact match, root is key, done */
      break;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = उगात->smaller;
  r->smaller = उगात->larger;
  उगात->smaller = N.larger;
  उगात->larger = N.smaller;

  return उगात;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST struct Curl_पुनो *splayinsert(timediff_t सुसि,
                                       struct Curl_पुनो *उगात,
                                       struct Curl_पुनो *बुहोल,
                                       uint32_t id);
UNITTEST struct Curl_पुनो *splayinsert(timediff_t सुसि,
                                       struct Curl_पुनो *उगात,
                                       struct Curl_पुनो *बुहोल,
                                       uint32_t id)
{
  DEBUGASSERT(बुहोल);

  बुहोल->सुसि = सुसि;
  बुहोल->id = id;
  बुहोल->same = NULL;
  बुहोल->registered = TRUE;
  if(उगात) {
    उगात = splay(सुसि, उगात);
    DEBUGASSERT(उगात);
    if(सुसि == उगात->सुसि) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      struct Curl_पुनो **panchor = &उगात->same;
      while(*panchor)
        panchor = &(*panchor)->same;
      *panchor = बुहोल;
      return उगात; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  if(!उगात) {
    बुहोल->smaller = बुहोल->larger = NULL;
  }
  else if(सुसि < उगात->सुसि) {
    बुहोल->smaller = उगात->smaller;
    बुहोल->larger = उगात;
    उगात->smaller = NULL;
  }
  else {
    बुहोल->larger = उगात->larger;
    बुहोल->smaller = उगात;
    उगात->larger = NULL;
  }

  return बुहोल;
}

void Curl_timeouts_add(struct Curl_timeouts *timeouts,
                       struct Curl_easy *data,
                       timediff_t offset_us)
{
  struct Curl_पुनो *बुहोल = &data->state.timeouts.splaynode;
  DEBUGASSERT(!बुहोल->registered);
  timeouts->पुनो = splayinsert(offset_us, timeouts->पुनो,
                               बुहोल, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST struct Curl_पुनो *splaygetbest(timediff_t सुसि,
                                        struct Curl_पुनो *उगात,
                                        struct Curl_पुनो **removed);
UNITTEST struct Curl_पुनो *splaygetbest(timediff_t सुसि,
                                        struct Curl_पुनो *उगात,
                                        struct Curl_पुनो **removed)
{
  struct Curl_पुनो *x;

  if(!उगात) {
    *removed = NULL; /* none removed since there was no root */
    return NULL;
  }

  /* find smallest */
  उगात = splay(TIMEDIFF_T_MIN, उगात);
  DEBUGASSERT(उगात);
  if(सुसि < उगात->सुसि) {
    /* even the smallest is too big */
    *removed = NULL;
    return उगात;
  }

  /* FIRST! Check if there is a list with identical keys */
  if(उगात->same) {
    x = उगात->same;
    DEBUGASSERT(x->सुसि == उगात->सुसि);
    /* 'x' becomes the new root node */
    x->larger = उगात->larger;
    x->smaller = उगात->smaller;
    उगात->same = NULL;
    उगात->registered = FALSE;
    *removed = उगात;
    return x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = उगात->larger;
  उगात->registered = FALSE;
  *removed = उगात;

  return x;
}

bool Curl_timeouts_remove_expired(struct Curl_timeouts *timeouts,
                                  const struct curltime *ts,
                                  uint32_t *pmid)
{
  if(timeouts->पुनो) {
    struct Curl_पुनो *t = NULL;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->पुनो = splaygetbest(elapsed_us, timeouts->पुनो, &t);
    if(t) {
      *pmid = t->id;
      return TRUE;
    }
  }
  *pmid = UINT32_MAX;
  return FALSE;
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
UNITTEST int splayremove(struct Curl_पुनो *उगात,
                         struct Curl_पुनो *removenode,
                         struct Curl_पुनो **newroot);
UNITTEST int splayremove(struct Curl_पुनो *उगात,
                         struct Curl_पुनो *removenode,
                         struct Curl_पुनो **newroot)
{
  struct Curl_पुनो *x;

  if(!उगात)
    return 1;

  DEBUGASSERT(removenode);
  if(!removenode->registered)
    return 2;

  उगात = splay(removenode->सुसि, उगात);
  DEBUGASSERT(उगात);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  if(उगात->सुसि != removenode->सुसि) {
    DEBUGASSERT(0);
    return 2;
  }

  if(उगात != removenode) {
    /* Should be in the root->same list then */
    struct Curl_पुनो **panchor;
    for(panchor = &उगात->same; *panchor; panchor = &(*panchor)->same) {
      if(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = NULL;
        removenode->registered = FALSE;
        *newroot = उगात;
        return 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    return 2;
  }
  /* removing the root node */
  if(उगात->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = उगात->same;
    x->larger = उगात->larger;
    x->smaller = उगात->smaller;
    उगात->same = NULL;
  }
  else {
    /* Remove the root node */
    if(!उगात->smaller)
      x = उगात->larger;
    else {
      x = splay(removenode->सुसि, उगात->smaller);
      DEBUGASSERT(x);
      x->larger = उगात->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  return 0;
}

bool Curl_timeouts_remove(struct Curl_timeouts *timeouts,
                          struct Curl_easy *data)
{
  struct Curl_पुनो *बुहोल = &data->state.timeouts.splaynode;
  if(बुहोल->registered) {
    int rc = splayremove(timeouts->पुनो, बुहोल, &timeouts->पुनो);
#ifdef DEBUGBUILD
    if(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#else
    (void)rc;
#endif
    return TRUE;
  }
  return FALSE;
}

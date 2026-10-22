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

struct Curl_easy;

#include "formdata.h"

#만약 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#include "urldata.h" /* for struct Curl_easy */
#include "mime.h"
#include "curlx/strdup.h"
#include "bufref.h"
#include "curlx/win32-fopen.h"


#define HTTPPOST_PTRNAME     CURL_HTTPPOST_PTRNAME
#define HTTPPOST_FILENAME    CURL_HTTPPOST_FILENAME
#define HTTPPOST_PTRCONTENTS CURL_HTTPPOST_PTRCONTENTS
#define HTTPPOST_READFILE    CURL_HTTPPOST_READFILE
#define HTTPPOST_PTRBUFFER   CURL_HTTPPOST_PTRBUFFER
#define HTTPPOST_CALLBACK    CURL_HTTPPOST_CALLBACK
#define HTTPPOST_BUFFER      CURL_HTTPPOST_BUFFER

/***************************************************************************
 *
 * httppost_add()
 *
 * Adds an HttpPost structure to the list, if parent_post is given becomes
 * a subpost of parent_post instead of a direct list element.
 *
 * Returns newly allocated HttpPost on success and NULL if malloc failed.
 *
 ***************************************************************************/
static struct curl_httppost *httppost_add(struct FormInfo *src,
                                          struct curl_httppost *parent_post,
                                          struct curl_httppost **httppost,
                                          struct curl_httppost **last_post)
{
  struct curl_httppost *post;
  size_t namelength = src->namelength;
  만약(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  만약((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    반환 NULL;
  post = curlx_calloc(1, sizeof(struct curl_httppost));
  만약(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->value));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  아니면
    반환 NULL;

  만약(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  아니면 {
    /* make the previous point to this */
    만약(*last_post)
      (*last_post)->next = post;
    아니면
      (*httppost) = post;

    (*last_post) = post;
  }
  반환 post;
}

/* Allocate and initialize a new FormInfo structure. */
static struct FormInfo *forminfo_new(void)
{
  struct FormInfo *form_info = curlx_calloc(1, sizeof(struct FormInfo));

  만약(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->value);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  반환 form_info;
}

/* Replace the target field data by a dynamic copy of it. */
static CURLcode forminfo_copyfield(struct bufref *field, size_t len)
{
  const char *value = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  만약(value) {
    만약(!len)
      len = strlen(value);
    result = Curl_bufref_memdup0(field, value, len);
  }

  반환 result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
static void forminfo_add(struct FormInfo *form_info, struct FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  만약(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

static void free_formlist(struct FormInfo *ptr)
{
  위해(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->value);
    Curl_bufref_free(&ptr->contenttype);
    Curl_bufref_free(&ptr->showfilename);
  }
}

/***************************************************************************
 *
 * formadd_check()
 *
 * Stores a formpost parameter and builds the appropriate linked list.
 *
 * Has two principal functionalities: using files and byte arrays as post
 * parts. Byte arrays are either copied or the pointer is stored (as the user
 * requests) while for files only the filename and not the content is stored.
 *
 * While you may have only one byte array for each name, multiple filenames
 * are allowed (and because of this feature CURLFORM_END is needed after
 * using CURLFORM_FILE).
 *
 * Examples:
 *
 * Simple name/value pair with copied contents:
 * curl_formadd(&post, &last, CURLFORM_COPYNAME, "name",
 *              CURLFORM_COPYCONTENTS, "value", CURLFORM_END);
 *
 * name/value pair where only the content pointer is remembered:
 * curl_formadd(&post, &last, CURLFORM_COPYNAME, "name",
 *              CURLFORM_PTRCONTENTS, ptr, CURLFORM_CONTENTSLENGTH, 10L,
 *              CURLFORM_END);
 * (if CURLFORM_CONTENTSLENGTH is missing strlen() is used)
 *
 * storing a filename (CONTENTTYPE is optional!):
 * curl_formadd(&post, &last, CURLFORM_COPYNAME, "name",
 *              CURLFORM_FILE, "filename1", CURLFORM_CONTENTTYPE, "plain/text",
 *              CURLFORM_END);
 *
 * storing multiple filenames:
 * curl_formadd(&post, &last, CURLFORM_COPYNAME, "name",
 *              CURLFORM_FILE, "filename1", CURLFORM_FILE, "filename2",
 *              CURLFORM_END);
 *
 * Returns:
 * CURL_FORMADD_OK             on success
 * CURL_FORMADD_MEMORY         if the FormInfo allocation fails
 * CURL_FORMADD_OPTION_TWICE   if one option is given twice for one Form
 * CURL_FORMADD_NULL           if a null pointer was given for a char
 * CURL_FORMADD_MEMORY         if the allocation of a FormInfo struct failed
 * CURL_FORMADD_UNKNOWN_OPTION if an unknown option was used
 * CURL_FORMADD_INCOMPLETE     if the some FormInfo is not complete (or error)
 * CURL_FORMADD_MEMORY         if an HttpPost struct cannot be allocated
 * CURL_FORMADD_MEMORY         if some allocation for string copying failed.
 * CURL_FORMADD_ILLEGAL_ARRAY  if an illegal option is used in an array
 *
 ***************************************************************************/

static CURLFORMcode formadd_check(struct FormInfo *first_form,
                                  struct curl_httppost **httppost,
                                  struct curl_httppost **last_post)
{
  const char *prevtype = NULL;
  struct FormInfo *form = NULL;
  struct curl_httppost *post = NULL;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  위해(form = first_form; form; form = form->more) {
    const char *name = Curl_bufref_ptr(&form->name);

    만약(((!name || !Curl_bufref_ptr(&form->value)) && !post) ||
       (form->contentslength &&
        (form->flags & HTTPPOST_FILENAME)) ||
       ((form->flags & HTTPPOST_FILENAME) &&
        (form->flags & HTTPPOST_PTRCONTENTS)) ||

       (!form->buffer &&
        (form->flags & HTTPPOST_BUFFER) &&
        (form->flags & HTTPPOST_PTRBUFFER)) ||

       ((form->flags & HTTPPOST_READFILE) &&
        (form->flags & HTTPPOST_PTRCONTENTS))
      ) {
      반환 CURL_FORMADD_INCOMPLETE;
    }
    만약(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      const char *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->value);
      const char *type = Curl_mime_contenttype(f);
      만약(!type)
        type = prevtype;
      만약(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      만약(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        반환 CURL_FORMADD_MEMORY;
    }
    만약(name && form->namelength && memchr(name, 0, form->namelength))
      반환 CURL_FORMADD_NULL;
    만약(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      반환 CURL_FORMADD_MEMORY;
    만약(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->value, (size_t)form->contentslength))
      반환 CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    만약(!post)
      반환 CURL_FORMADD_MEMORY;

    만약(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  반환 CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
static void free_chain(struct curl_httppost *c)
{
  동안(c) {
    struct curl_httppost *next = c->next;
    만약(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

static CURLFORMcode formadd(struct curl_httppost **httppost,
                            struct curl_httppost **last_post, va_list params)
{
  struct FormInfo *first_form, *curr, *form = NULL;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  const struct curl_forms *forms = NULL;
  char *avalue = NULL;
  struct curl_httppost *newchain = NULL;
  struct curl_httppost *lastnode = NULL;

#define form_ptr_arg(t) (forms ? (t)(void *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#아니면
#define form_int_arg(t) (forms ? (t)(void *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  만약(!first_form)
    반환 CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  동안(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    만약(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (char *)CURL_UNCONST(forms->value);

      forms++; /* advance this to next entry */
      만약(CURLFORM_END == option) {
        /* end of array state */
        forms = NULL;
        continue;
      }
    }
    아니면 {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, 정수);
      만약(CURLFORM_END == option)
        break;
    }

    switch(option) {
    case CURLFORM_ARRAY:
      만약(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      아니면 {
        forms = va_arg(params, struct curl_forms *);
        만약(!forms)
          retval = CURL_FORMADD_NULL;
      }
      break;

      /*
       * Set the Name property.
       */
    case CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    case CURLFORM_COPYNAME:
      만약(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면 {
        avalue = form_ptr_arg(char *);
        만약(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, NULL); /* No copy yet. */
        아니면
          retval = CURL_FORMADD_NULL;
      }
      break;
    case CURLFORM_NAMELENGTH:
      만약(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면
        curr->namelength = (size_t)form_int_arg(long);
      break;

      /*
       * Set the contents property.
       */
    case CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    case CURLFORM_COPYCONTENTS:
      만약(Curl_bufref_ptr(&curr->value))
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면 {
        avalue = form_ptr_arg(char *);
        만약(avalue)
          Curl_bufref_set(&curr->value, avalue, 0, NULL); /* No copy yet. */
        아니면
          retval = CURL_FORMADD_NULL;
      }
      break;
    case CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      만약(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      아니면
        curr->contentslength = (curl_off_t)clen;
      break;
    }

    case CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      만약(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      아니면 {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      break;
    }

      /* Get contents from a given filename */
    case CURLFORM_FILECONTENT:
      만약(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면 {
        avalue = form_ptr_arg(char *);
        만약(avalue) {
          만약(Curl_bufref_memdup0(&curr->value, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          아니면
            curr->flags |= HTTPPOST_READFILE;
        }
        아니면
          retval = CURL_FORMADD_NULL;
      }
      break;

      /* We upload a file */
    case CURLFORM_FILE:
      avalue = form_ptr_arg(char *);
      만약(Curl_bufref_ptr(&curr->value)) {
        만약(curr->flags & HTTPPOST_FILENAME) {
          만약(avalue) {
            form = forminfo_new();
            만약(!form ||
               Curl_bufref_memdup0(&form->value, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            아니면 {
              forminfo_add(form, curr);
              curr = form;
              form = NULL;
            }
          }
          아니면
            retval = CURL_FORMADD_NULL;
        }
        아니면
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      아니면 {
        만약(avalue) {
          만약(Curl_bufref_memdup0(&curr->value, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          아니면
            curr->flags |= HTTPPOST_FILENAME;
        }
        아니면
          retval = CURL_FORMADD_NULL;
      }
      break;

    case CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      만약(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면 {
        avalue = form_ptr_arg(char *);
        만약(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->value, avalue, 0, NULL);
        }
        아니면
          retval = CURL_FORMADD_NULL;
      }
      break;

    case CURLFORM_BUFFERLENGTH:
      만약(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면
        curr->bufferlength = (size_t)form_int_arg(long);
      break;

    case CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      만약(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면 {
        avalue = form_ptr_arg(char *);
        만약(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->value, avalue, 0, NULL);
        }
        아니면
          retval = CURL_FORMADD_NULL;
      }
      break;

    case CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(char *);
      만약(Curl_bufref_ptr(&curr->contenttype)) {
        만약(curr->flags & HTTPPOST_FILENAME) {
          만약(avalue) {
            form = forminfo_new();
            만약(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            아니면 {
              forminfo_add(form, curr);
              curr = form;
              form = NULL;
            }
          }
          아니면
            retval = CURL_FORMADD_NULL;
        }
        아니면
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      아니면 만약(avalue) {
        만약(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      아니면
        retval = CURL_FORMADD_NULL;
      break;

    case CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      struct curl_slist *list = form_ptr_arg(struct curl_slist *);

      만약(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면
        curr->contentheader = list;

      break;
    }
    case CURLFORM_FILENAME:
    case CURLFORM_BUFFER:
      avalue = form_ptr_arg(char *);
      만약(!avalue)
        retval = CURL_FORMADD_NULL;
      아니면 만약(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      아니면 만약(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      break;

    default:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      break;
    }
  }

  만약(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  만약(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  동안(first_form) {
    struct FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  만약(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    만약(*last_post)
      (*last_post)->next = newchain;
    아니면
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  아니면
    free_chain(newchain);

  반환 retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(struct curl_httppost **httppost,
                          struct curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  반환 form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
정수 curl_formget(struct curl_httppost *form, void *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  만약(!append)
    반환 (정수)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(NULL, &toppart, form, NULL);
  만약(!result)
    result = Curl_mime_prepare_headers(NULL, &toppart, "multipart/form-data",
                                       NULL, MIMESTRATEGY_FORM);

  동안(!result) {
    char buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, sizeof(buffer), &toppart);

    만약(!nread)
      break;

    만약(nread > sizeof(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      만약(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  반환 (정수)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
void curl_formfree(struct curl_httppost *form)
{
  struct curl_httppost *next;

  만약(!form)
    /* no form to free, get out of this */
    반환;

  do {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    만약(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    만약(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } 동안(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
static CURLcode setname(curl_mimepart *part, const char *name, size_t len)
{
  char *zname;
  CURLcode result;

  만약(!name || !len)
    반환 curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  만약(!zname)
    반환 CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  반환 result;
}

/*
 * Curl_getformdata() converts a linked list of "meta data" into a mime
 * structure. The input list is in 'post', while the output is stored in
 * mime part at '*finalform'.
 *
 * This function will not do a failf() for the potential memory failures but
 * should for all other errors it spots. Note that this function MAY get a
 * NULL pointer in the 'data' argument.
 */

CURLcode Curl_getformdata(CURL *data,
                          curl_mimepart *finalform,
                          struct curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = NULL;
  curl_mimepart *part;
  struct curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  만약(!post)
    반환 result; /* no input => no output! */

  form = curl_mime_init(data);
  만약(!form)
    result = CURLE_OUT_OF_MEMORY;

  만약(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  위해(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    만약(post->more) {
      part = curl_mime_addpart(form);
      만약(!part)
        result = CURLE_OUT_OF_MEMORY;
      만약(!result)
        result = setname(part, post->name, post->namelength);
      만약(!result) {
        multipart = curl_mime_init(data);
        만약(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      만약(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    위해(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      만약(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      만약(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      만약(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      만약(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      만약(!result) {
        curl_off_t clen = post->contentslength;

        만약(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        만약(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          만약(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#만약 defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       NULL, (void *)stdin);
#만약 defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
          }
          아니면
            result = curl_mime_filedata(part, file->contents);
          만약(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, NULL);
        }
        아니면 만약(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        아니면 만약(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          만약(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, NULL, NULL, post->userp);
        }
        아니면 {
          size_t uclen;
          만약(!clen)
            uclen = CURL_ZERO_TERMINATED;
          아니면
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      만약(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  만약(result)
    Curl_mime_cleanpart(finalform);

  반환 result;
}

#아니면 /* if disabled */
CURLFORMcode curl_formadd(struct curl_httppost **httppost,
                          struct curl_httppost **last_post, ...)
{
  (void)httppost;
  (void)last_post;
  반환 CURL_FORMADD_DISABLED;
}

정수 curl_formget(struct curl_httppost *form, void *arg,
                 curl_formget_callback append)
{
  (void)form;
  (void)arg;
  (void)append;
  반환 CURL_FORMADD_DISABLED;
}

void curl_formfree(struct curl_httppost *form)
{
  (void)form;
  /* Nothing to do. */
}

#endif /* if disabled */

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

સંરચના Curl_easy;

#અન્તર્ભાવ "formdata.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#અન્તર્ભાવ "urldata.h" /* for struct Curl_easy */
#અન્તર્ભાવ "mime.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"


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
સ્થિર સંરચના curl_httppost *httppost_add(સંરચના FormInfo *src,
                                          સંરચના curl_httppost *parent_post,
                                          સંરચના curl_httppost **httppost,
                                          સંરચના curl_httppost **last_post)
{
  સંરચના curl_httppost *post;
  size_t namelength = src->namelength;
  જો(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  જો((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    પાછા ખાલી;
  post = curlx_calloc(1, કદ(સંરચના curl_httppost));
  જો(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->કિંમત));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  નહીંતર
    પાછા ખાલી;

  જો(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  નહીંતર {
    /* make the previous point to this */
    જો(*last_post)
      (*last_post)->next = post;
    નહીંતર
      (*httppost) = post;

    (*last_post) = post;
  }
  પાછા post;
}

/* Allocate and initialize a new FormInfo structure. */
સ્થિર સંરચના FormInfo *forminfo_new(શૂન્ય)
{
  સંરચના FormInfo *form_info = curlx_calloc(1, કદ(સંરચના FormInfo));

  જો(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->કિંમત);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  પાછા form_info;
}

/* Replace the target field data by a dynamic copy of it. */
સ્થિર CURLcode forminfo_copyfield(સંરચના bufref *field, size_t len)
{
  અચલ અક્ષર *કિંમત = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  જો(કિંમત) {
    જો(!len)
      len = strlen(કિંમત);
    result = Curl_bufref_memdup0(field, કિંમત, len);
  }

  પાછા result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
સ્થિર શૂન્ય forminfo_add(સંરચના FormInfo *form_info, સંરચના FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  જો(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

સ્થિર શૂન્ય free_formlist(સંરચના FormInfo *ptr)
{
  માટે(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->કિંમત);
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

સ્થિર CURLFORMcode formadd_check(સંરચના FormInfo *first_form,
                                  સંરચના curl_httppost **httppost,
                                  સંરચના curl_httppost **last_post)
{
  અચલ અક્ષર *prevtype = ખાલી;
  સંરચના FormInfo *form = ખાલી;
  સંરચના curl_httppost *post = ખાલી;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  માટે(form = first_form; form; form = form->more) {
    અચલ અક્ષર *name = Curl_bufref_ptr(&form->name);

    જો(((!name || !Curl_bufref_ptr(&form->કિંમત)) && !post) ||
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
      પાછા CURL_FORMADD_INCOMPLETE;
    }
    જો(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      અચલ અક્ષર *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->કિંમત);
      અચલ અક્ષર *type = Curl_mime_contenttype(f);
      જો(!type)
        type = prevtype;
      જો(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      જો(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        પાછા CURL_FORMADD_MEMORY;
    }
    જો(name && form->namelength && memchr(name, 0, form->namelength))
      પાછા CURL_FORMADD_NULL;
    જો(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      પાછા CURL_FORMADD_MEMORY;
    જો(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->કિંમત, (size_t)form->contentslength))
      પાછા CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    જો(!post)
      પાછા CURL_FORMADD_MEMORY;

    જો(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  પાછા CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
સ્થિર શૂન્ય free_chain(સંરચના curl_httppost *c)
{
  જ્યારે(c) {
    સંરચના curl_httppost *next = c->next;
    જો(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

સ્થિર CURLFORMcode formadd(સંરચના curl_httppost **httppost,
                            સંરચના curl_httppost **last_post, va_list params)
{
  સંરચના FormInfo *first_form, *curr, *form = ખાલી;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  અચલ સંરચના curl_forms *forms = ખાલી;
  અક્ષર *avalue = ખાલી;
  સંરચના curl_httppost *newchain = ખાલી;
  સંરચના curl_httppost *lastnode = ખાલી;

#define form_ptr_arg(t) (forms ? (t)(શૂન્ય *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#નહીંતર
#define form_int_arg(t) (forms ? (t)(શૂન્ય *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  જો(!first_form)
    પાછા CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  જ્યારે(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    જો(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (અક્ષર *)CURL_UNCONST(forms->કિંમત);

      forms++; /* advance this to next entry */
      જો(CURLFORM_END == option) {
        /* end of array state */
        forms = ખાલી;
        ચાલુ;
      }
    }
    નહીંતર {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, પૂર્ણાંક);
      જો(CURLFORM_END == option)
        તોડો;
    }

    પસંદગી(option) {
    સ્થિતિ CURLFORM_ARRAY:
      જો(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      નહીંતર {
        forms = va_arg(params, સંરચના curl_forms *);
        જો(!forms)
          retval = CURL_FORMADD_NULL;
      }
      તોડો;

      /*
       * Set the Name property.
       */
    સ્થિતિ CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    સ્થિતિ CURLFORM_COPYNAME:
      જો(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર {
        avalue = form_ptr_arg(અક્ષર *);
        જો(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, ખાલી); /* No copy yet. */
        નહીંતર
          retval = CURL_FORMADD_NULL;
      }
      તોડો;
    સ્થિતિ CURLFORM_NAMELENGTH:
      જો(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર
        curr->namelength = (size_t)form_int_arg(long);
      તોડો;

      /*
       * Set the contents property.
       */
    સ્થિતિ CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    સ્થિતિ CURLFORM_COPYCONTENTS:
      જો(Curl_bufref_ptr(&curr->કિંમત))
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર {
        avalue = form_ptr_arg(અક્ષર *);
        જો(avalue)
          Curl_bufref_set(&curr->કિંમત, avalue, 0, ખાલી); /* No copy yet. */
        નહીંતર
          retval = CURL_FORMADD_NULL;
      }
      તોડો;
    સ્થિતિ CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      જો(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      નહીંતર
        curr->contentslength = (curl_off_t)clen;
      તોડો;
    }

    સ્થિતિ CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      જો(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      નહીંતર {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      તોડો;
    }

      /* Get contents from a given filename */
    સ્થિતિ CURLFORM_FILECONTENT:
      જો(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર {
        avalue = form_ptr_arg(અક્ષર *);
        જો(avalue) {
          જો(Curl_bufref_memdup0(&curr->કિંમત, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          નહીંતર
            curr->flags |= HTTPPOST_READFILE;
        }
        નહીંતર
          retval = CURL_FORMADD_NULL;
      }
      તોડો;

      /* We upload a file */
    સ્થિતિ CURLFORM_FILE:
      avalue = form_ptr_arg(અક્ષર *);
      જો(Curl_bufref_ptr(&curr->કિંમત)) {
        જો(curr->flags & HTTPPOST_FILENAME) {
          જો(avalue) {
            form = forminfo_new();
            જો(!form ||
               Curl_bufref_memdup0(&form->કિંમત, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            નહીંતર {
              forminfo_add(form, curr);
              curr = form;
              form = ખાલી;
            }
          }
          નહીંતર
            retval = CURL_FORMADD_NULL;
        }
        નહીંતર
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      નહીંતર {
        જો(avalue) {
          જો(Curl_bufref_memdup0(&curr->કિંમત, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          નહીંતર
            curr->flags |= HTTPPOST_FILENAME;
        }
        નહીંતર
          retval = CURL_FORMADD_NULL;
      }
      તોડો;

    સ્થિતિ CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      જો(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર {
        avalue = form_ptr_arg(અક્ષર *);
        જો(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->કિંમત, avalue, 0, ખાલી);
        }
        નહીંતર
          retval = CURL_FORMADD_NULL;
      }
      તોડો;

    સ્થિતિ CURLFORM_BUFFERLENGTH:
      જો(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર
        curr->bufferlength = (size_t)form_int_arg(long);
      તોડો;

    સ્થિતિ CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      જો(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર {
        avalue = form_ptr_arg(અક્ષર *);
        જો(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->કિંમત, avalue, 0, ખાલી);
        }
        નહીંતર
          retval = CURL_FORMADD_NULL;
      }
      તોડો;

    સ્થિતિ CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(અક્ષર *);
      જો(Curl_bufref_ptr(&curr->contenttype)) {
        જો(curr->flags & HTTPPOST_FILENAME) {
          જો(avalue) {
            form = forminfo_new();
            જો(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            નહીંતર {
              forminfo_add(form, curr);
              curr = form;
              form = ખાલી;
            }
          }
          નહીંતર
            retval = CURL_FORMADD_NULL;
        }
        નહીંતર
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      નહીંતર જો(avalue) {
        જો(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      નહીંતર
        retval = CURL_FORMADD_NULL;
      તોડો;

    સ્થિતિ CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      સંરચના curl_slist *list = form_ptr_arg(સંરચના curl_slist *);

      જો(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર
        curr->contentheader = list;

      તોડો;
    }
    સ્થિતિ CURLFORM_FILENAME:
    સ્થિતિ CURLFORM_BUFFER:
      avalue = form_ptr_arg(અક્ષર *);
      જો(!avalue)
        retval = CURL_FORMADD_NULL;
      નહીંતર જો(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      નહીંતર જો(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      તોડો;

    મૂળભૂત:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      તોડો;
    }
  }

  જો(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  જો(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  જ્યારે(first_form) {
    સંરચના FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  જો(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    જો(*last_post)
      (*last_post)->next = newchain;
    નહીંતર
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  નહીંતર
    free_chain(newchain);

  પાછા retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(સંરચના curl_httppost **httppost,
                          સંરચના curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  પાછા form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
પૂર્ણાંક curl_formget(સંરચના curl_httppost *form, શૂન્ય *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  જો(!append)
    પાછા (પૂર્ણાંક)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(ખાલી, &toppart, form, ખાલી);
  જો(!result)
    result = Curl_mime_prepare_headers(ખાલી, &toppart, "multipart/form-data",
                                       ખાલી, MIMESTRATEGY_FORM);

  જ્યારે(!result) {
    અક્ષર buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, કદ(buffer), &toppart);

    જો(!nread)
      તોડો;

    જો(nread > કદ(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      જો(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  પાછા (પૂર્ણાંક)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
શૂન્ય curl_formfree(સંરચના curl_httppost *form)
{
  સંરચના curl_httppost *next;

  જો(!form)
    /* no form to free, get out of this */
    પાછા;

  કરો {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    જો(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    જો(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } જ્યારે(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
સ્થિર CURLcode setname(curl_mimepart *part, અચલ અક્ષર *name, size_t len)
{
  અક્ષર *zname;
  CURLcode result;

  જો(!name || !len)
    પાછા curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  જો(!zname)
    પાછા CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  પાછા result;
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
                          સંરચના curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = ખાલી;
  curl_mimepart *part;
  સંરચના curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  જો(!post)
    પાછા result; /* no input => no output! */

  form = curl_mime_init(data);
  જો(!form)
    result = CURLE_OUT_OF_MEMORY;

  જો(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  માટે(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    જો(post->more) {
      part = curl_mime_addpart(form);
      જો(!part)
        result = CURLE_OUT_OF_MEMORY;
      જો(!result)
        result = setname(part, post->name, post->namelength);
      જો(!result) {
        multipart = curl_mime_init(data);
        જો(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      જો(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    માટે(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      જો(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      જો(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      જો(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      જો(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      જો(!result) {
        curl_off_t clen = post->contentslength;

        જો(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        જો(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          જો(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       ખાલી, (શૂન્ય *)stdin);
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic કાઢો
#endif
          }
          નહીંતર
            result = curl_mime_filedata(part, file->contents);
          જો(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, ખાલી);
        }
        નહીંતર જો(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        નહીંતર જો(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          જો(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, ખાલી, ખાલી, post->userp);
        }
        નહીંતર {
          size_t uclen;
          જો(!clen)
            uclen = CURL_ZERO_TERMINATED;
          નહીંતર
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      જો(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  જો(result)
    Curl_mime_cleanpart(finalform);

  પાછા result;
}

#નહીંતર /* if disabled */
CURLFORMcode curl_formadd(સંરચના curl_httppost **httppost,
                          સંરચના curl_httppost **last_post, ...)
{
  (શૂન્ય)httppost;
  (શૂન્ય)last_post;
  પાછા CURL_FORMADD_DISABLED;
}

પૂર્ણાંક curl_formget(સંરચના curl_httppost *form, શૂન્ય *arg,
                 curl_formget_callback append)
{
  (શૂન્ય)form;
  (શૂન્ય)arg;
  (શૂન્ય)append;
  પાછા CURL_FORMADD_DISABLED;
}

શૂન્ય curl_formfree(સંરચના curl_httppost *form)
{
  (શૂન્ય)form;
  /* Nothing to do. */
}

#endif /* if disabled */

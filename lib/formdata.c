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

ಸಂರಚನಾ Curl_easy;

#ಅನ್ತರ್ಭಾವ "formdata.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#ಅನ್ತರ್ಭಾವ "urldata.h" /* for struct Curl_easy */
#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"


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
ಸ್ಥಿರ ಸಂರಚನಾ curl_httppost *httppost_add(ಸಂರಚನಾ FormInfo *src,
                                          ಸಂರಚನಾ curl_httppost *parent_post,
                                          ಸಂರಚನಾ curl_httppost **httppost,
                                          ಸಂರಚನಾ curl_httppost **last_post)
{
  ಸಂರಚನಾ curl_httppost *post;
  size_t namelength = src->namelength;
  ಯದಿ(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  ಯದಿ((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  post = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ curl_httppost));
  ಯದಿ(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->ಮೌಲ್ಯ));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಯದಿ(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  ಅನ್ಯಥಾ {
    /* make the previous point to this */
    ಯದಿ(*last_post)
      (*last_post)->next = post;
    ಅನ್ಯಥಾ
      (*httppost) = post;

    (*last_post) = post;
  }
  ಪ್ರತ್ಯಾಗಮ post;
}

/* Allocate and initialize a new FormInfo structure. */
ಸ್ಥಿರ ಸಂರಚನಾ FormInfo *forminfo_new(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ FormInfo *form_info = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ FormInfo));

  ಯದಿ(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->ಮೌಲ್ಯ);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  ಪ್ರತ್ಯಾಗಮ form_info;
}

/* Replace the target field data by a dynamic copy of it. */
ಸ್ಥಿರ CURLcode forminfo_copyfield(ಸಂರಚನಾ bufref *field, size_t len)
{
  ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  ಯದಿ(ಮೌಲ್ಯ) {
    ಯದಿ(!len)
      len = strlen(ಮೌಲ್ಯ);
    result = Curl_bufref_memdup0(field, ಮೌಲ್ಯ, len);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
ಸ್ಥಿರ ಶೂನ್ಯ forminfo_add(ಸಂರಚನಾ FormInfo *form_info, ಸಂರಚನಾ FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  ಯದಿ(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ free_formlist(ಸಂರಚನಾ FormInfo *ptr)
{
  ಪ್ರತಿ(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->ಮೌಲ್ಯ);
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

ಸ್ಥಿರ CURLFORMcode formadd_check(ಸಂರಚನಾ FormInfo *first_form,
                                  ಸಂರಚನಾ curl_httppost **httppost,
                                  ಸಂರಚನಾ curl_httppost **last_post)
{
  ಧ್ರುವ ವರ್ಣ *prevtype = ರಿಕ್ತ;
  ಸಂರಚನಾ FormInfo *form = ರಿಕ್ತ;
  ಸಂರಚನಾ curl_httppost *post = ರಿಕ್ತ;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  ಪ್ರತಿ(form = first_form; form; form = form->more) {
    ಧ್ರುವ ವರ್ಣ *name = Curl_bufref_ptr(&form->name);

    ಯದಿ(((!name || !Curl_bufref_ptr(&form->ಮೌಲ್ಯ)) && !post) ||
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
      ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_INCOMPLETE;
    }
    ಯದಿ(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ಧ್ರುವ ವರ್ಣ *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->ಮೌಲ್ಯ);
      ಧ್ರುವ ವರ್ಣ *type = Curl_mime_contenttype(f);
      ಯದಿ(!type)
        type = prevtype;
      ಯದಿ(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      ಯದಿ(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_MEMORY;
    }
    ಯದಿ(name && form->namelength && memchr(name, 0, form->namelength))
      ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_NULL;
    ಯದಿ(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_MEMORY;
    ಯದಿ(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->ಮೌಲ್ಯ, (size_t)form->contentslength))
      ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    ಯದಿ(!post)
      ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_MEMORY;

    ಯದಿ(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
ಸ್ಥಿರ ಶೂನ್ಯ free_chain(ಸಂರಚನಾ curl_httppost *c)
{
  ಯಾವತ್(c) {
    ಸಂರಚನಾ curl_httppost *next = c->next;
    ಯದಿ(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

ಸ್ಥಿರ CURLFORMcode formadd(ಸಂರಚನಾ curl_httppost **httppost,
                            ಸಂರಚನಾ curl_httppost **last_post, va_list params)
{
  ಸಂರಚನಾ FormInfo *first_form, *curr, *form = ರಿಕ್ತ;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ಧ್ರುವ ಸಂರಚನಾ curl_forms *forms = ರಿಕ್ತ;
  ವರ್ಣ *avalue = ರಿಕ್ತ;
  ಸಂರಚನಾ curl_httppost *newchain = ರಿಕ್ತ;
  ಸಂರಚನಾ curl_httppost *lastnode = ರಿಕ್ತ;

#define form_ptr_arg(t) (forms ? (t)(ಶೂನ್ಯ *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#ಅನ್ಯಥಾ
#define form_int_arg(t) (forms ? (t)(ಶೂನ್ಯ *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  ಯದಿ(!first_form)
    ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  ಯಾವತ್(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    ಯದಿ(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (ವರ್ಣ *)CURL_UNCONST(forms->ಮೌಲ್ಯ);

      forms++; /* advance this to next entry */
      ಯದಿ(CURLFORM_END == option) {
        /* end of array state */
        forms = ರಿಕ್ತ;
        ಅನುವರ್ತ;
      }
    }
    ಅನ್ಯಥಾ {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, ಪೂರ್ಣಾಙ್ಕ);
      ಯದಿ(CURLFORM_END == option)
        ಭಙ್ಗ;
    }

    ವಿಕಲ್ಪ(option) {
    ಪಕ್ಷ CURLFORM_ARRAY:
      ಯದಿ(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      ಅನ್ಯಥಾ {
        forms = va_arg(params, ಸಂರಚನಾ curl_forms *);
        ಯದಿ(!forms)
          retval = CURL_FORMADD_NULL;
      }
      ಭಙ್ಗ;

      /*
       * Set the Name property.
       */
    ಪಕ್ಷ CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    ಪಕ್ಷ CURLFORM_COPYNAME:
      ಯದಿ(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ {
        avalue = form_ptr_arg(ವರ್ಣ *);
        ಯದಿ(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, ರಿಕ್ತ); /* No copy yet. */
        ಅನ್ಯಥಾ
          retval = CURL_FORMADD_NULL;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLFORM_NAMELENGTH:
      ಯದಿ(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ
        curr->namelength = (size_t)form_int_arg(long);
      ಭಙ್ಗ;

      /*
       * Set the contents property.
       */
    ಪಕ್ಷ CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    ಪಕ್ಷ CURLFORM_COPYCONTENTS:
      ಯದಿ(Curl_bufref_ptr(&curr->ಮೌಲ್ಯ))
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ {
        avalue = form_ptr_arg(ವರ್ಣ *);
        ಯದಿ(avalue)
          Curl_bufref_set(&curr->ಮೌಲ್ಯ, avalue, 0, ರಿಕ್ತ); /* No copy yet. */
        ಅನ್ಯಥಾ
          retval = CURL_FORMADD_NULL;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      ಯದಿ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ಅನ್ಯಥಾ
        curr->contentslength = (curl_off_t)clen;
      ಭಙ್ಗ;
    }

    ಪಕ್ಷ CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      ಯದಿ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ಅನ್ಯಥಾ {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      ಭಙ್ಗ;
    }

      /* Get contents from a given filename */
    ಪಕ್ಷ CURLFORM_FILECONTENT:
      ಯದಿ(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ {
        avalue = form_ptr_arg(ವರ್ಣ *);
        ಯದಿ(avalue) {
          ಯದಿ(Curl_bufref_memdup0(&curr->ಮೌಲ್ಯ, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          ಅನ್ಯಥಾ
            curr->flags |= HTTPPOST_READFILE;
        }
        ಅನ್ಯಥಾ
          retval = CURL_FORMADD_NULL;
      }
      ಭಙ್ಗ;

      /* We upload a file */
    ಪಕ್ಷ CURLFORM_FILE:
      avalue = form_ptr_arg(ವರ್ಣ *);
      ಯದಿ(Curl_bufref_ptr(&curr->ಮೌಲ್ಯ)) {
        ಯದಿ(curr->flags & HTTPPOST_FILENAME) {
          ಯದಿ(avalue) {
            form = forminfo_new();
            ಯದಿ(!form ||
               Curl_bufref_memdup0(&form->ಮೌಲ್ಯ, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            ಅನ್ಯಥಾ {
              forminfo_add(form, curr);
              curr = form;
              form = ರಿಕ್ತ;
            }
          }
          ಅನ್ಯಥಾ
            retval = CURL_FORMADD_NULL;
        }
        ಅನ್ಯಥಾ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(avalue) {
          ಯದಿ(Curl_bufref_memdup0(&curr->ಮೌಲ್ಯ, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          ಅನ್ಯಥಾ
            curr->flags |= HTTPPOST_FILENAME;
        }
        ಅನ್ಯಥಾ
          retval = CURL_FORMADD_NULL;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      ಯದಿ(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ {
        avalue = form_ptr_arg(ವರ್ಣ *);
        ಯದಿ(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->ಮೌಲ್ಯ, avalue, 0, ರಿಕ್ತ);
        }
        ಅನ್ಯಥಾ
          retval = CURL_FORMADD_NULL;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURLFORM_BUFFERLENGTH:
      ಯದಿ(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ
        curr->bufferlength = (size_t)form_int_arg(long);
      ಭಙ್ಗ;

    ಪಕ್ಷ CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      ಯದಿ(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ {
        avalue = form_ptr_arg(ವರ್ಣ *);
        ಯದಿ(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->ಮೌಲ್ಯ, avalue, 0, ರಿಕ್ತ);
        }
        ಅನ್ಯಥಾ
          retval = CURL_FORMADD_NULL;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(ವರ್ಣ *);
      ಯದಿ(Curl_bufref_ptr(&curr->contenttype)) {
        ಯದಿ(curr->flags & HTTPPOST_FILENAME) {
          ಯದಿ(avalue) {
            form = forminfo_new();
            ಯದಿ(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            ಅನ್ಯಥಾ {
              forminfo_add(form, curr);
              curr = form;
              form = ರಿಕ್ತ;
            }
          }
          ಅನ್ಯಥಾ
            retval = CURL_FORMADD_NULL;
        }
        ಅನ್ಯಥಾ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ಅನ್ಯಥಾ ಯದಿ(avalue) {
        ಯದಿ(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      ಅನ್ಯಥಾ
        retval = CURL_FORMADD_NULL;
      ಭಙ್ಗ;

    ಪಕ್ಷ CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      ಸಂರಚನಾ curl_slist *list = form_ptr_arg(ಸಂರಚನಾ curl_slist *);

      ಯದಿ(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ
        curr->contentheader = list;

      ಭಙ್ಗ;
    }
    ಪಕ್ಷ CURLFORM_FILENAME:
    ಪಕ್ಷ CURLFORM_BUFFER:
      avalue = form_ptr_arg(ವರ್ಣ *);
      ಯದಿ(!avalue)
        retval = CURL_FORMADD_NULL;
      ಅನ್ಯಥಾ ಯದಿ(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      ಅನ್ಯಥಾ ಯದಿ(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      ಭಙ್ಗ;
    }
  }

  ಯದಿ(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  ಯದಿ(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  ಯಾವತ್(first_form) {
    ಸಂರಚನಾ FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  ಯದಿ(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    ಯದಿ(*last_post)
      (*last_post)->next = newchain;
    ಅನ್ಯಥಾ
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  ಅನ್ಯಥಾ
    free_chain(newchain);

  ಪ್ರತ್ಯಾಗಮ retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(ಸಂರಚನಾ curl_httppost **httppost,
                          ಸಂರಚನಾ curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  ಪ್ರತ್ಯಾಗಮ form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
ಪೂರ್ಣಾಙ್ಕ curl_formget(ಸಂರಚನಾ curl_httppost *form, ಶೂನ್ಯ *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  ಯದಿ(!append)
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(ರಿಕ್ತ, &toppart, form, ರಿಕ್ತ);
  ಯದಿ(!result)
    result = Curl_mime_prepare_headers(ರಿಕ್ತ, &toppart, "multipart/form-data",
                                       ರಿಕ್ತ, MIMESTRATEGY_FORM);

  ಯಾವತ್(!result) {
    ವರ್ಣ buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, ಪರಿಮಾಣ(buffer), &toppart);

    ಯದಿ(!nread)
      ಭಙ್ಗ;

    ಯದಿ(nread > ಪರಿಮಾಣ(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      ಯದಿ(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
ಶೂನ್ಯ curl_formfree(ಸಂರಚನಾ curl_httppost *form)
{
  ಸಂರಚನಾ curl_httppost *next;

  ಯದಿ(!form)
    /* no form to free, get out of this */
    ಪ್ರತ್ಯಾಗಮ;

  ಕುರು {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    ಯದಿ(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    ಯದಿ(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } ಯಾವತ್(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
ಸ್ಥಿರ CURLcode setname(curl_mimepart *part, ಧ್ರುವ ವರ್ಣ *name, size_t len)
{
  ವರ್ಣ *zname;
  CURLcode result;

  ಯದಿ(!name || !len)
    ಪ್ರತ್ಯಾಗಮ curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  ಯದಿ(!zname)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  ಪ್ರತ್ಯಾಗಮ result;
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
                          ಸಂರಚನಾ curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = ರಿಕ್ತ;
  curl_mimepart *part;
  ಸಂರಚನಾ curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  ಯದಿ(!post)
    ಪ್ರತ್ಯಾಗಮ result; /* no input => no output! */

  form = curl_mime_init(data);
  ಯದಿ(!form)
    result = CURLE_OUT_OF_MEMORY;

  ಯದಿ(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  ಪ್ರತಿ(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    ಯದಿ(post->more) {
      part = curl_mime_addpart(form);
      ಯದಿ(!part)
        result = CURLE_OUT_OF_MEMORY;
      ಯದಿ(!result)
        result = setname(part, post->name, post->namelength);
      ಯದಿ(!result) {
        multipart = curl_mime_init(data);
        ಯದಿ(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      ಯದಿ(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    ಪ್ರತಿ(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      ಯದಿ(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      ಯದಿ(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      ಯದಿ(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      ಯದಿ(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      ಯದಿ(!result) {
        curl_off_t clen = post->contentslength;

        ಯದಿ(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        ಯದಿ(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          ಯದಿ(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       ರಿಕ್ತ, (ಶೂನ್ಯ *)stdin);
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತೆಗೆ
#endif
          }
          ಅನ್ಯಥಾ
            result = curl_mime_filedata(part, file->contents);
          ಯದಿ(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, ರಿಕ್ತ);
        }
        ಅನ್ಯಥಾ ಯದಿ(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        ಅನ್ಯಥಾ ಯದಿ(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          ಯದಿ(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, ರಿಕ್ತ, ರಿಕ್ತ, post->userp);
        }
        ಅನ್ಯಥಾ {
          size_t uclen;
          ಯದಿ(!clen)
            uclen = CURL_ZERO_TERMINATED;
          ಅನ್ಯಥಾ
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      ಯದಿ(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  ಯದಿ(result)
    Curl_mime_cleanpart(finalform);

  ಪ್ರತ್ಯಾಗಮ result;
}

#ಅನ್ಯಥಾ /* if disabled */
CURLFORMcode curl_formadd(ಸಂರಚನಾ curl_httppost **httppost,
                          ಸಂರಚನಾ curl_httppost **last_post, ...)
{
  (ಶೂನ್ಯ)httppost;
  (ಶೂನ್ಯ)last_post;
  ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_DISABLED;
}

ಪೂರ್ಣಾಙ್ಕ curl_formget(ಸಂರಚನಾ curl_httppost *form, ಶೂನ್ಯ *arg,
                 curl_formget_callback append)
{
  (ಶೂನ್ಯ)form;
  (ಶೂನ್ಯ)arg;
  (ಶೂನ್ಯ)append;
  ಪ್ರತ್ಯಾಗಮ CURL_FORMADD_DISABLED;
}

ಶೂನ್ಯ curl_formfree(ಸಂರಚನಾ curl_httppost *form)
{
  (ಶೂನ್ಯ)form;
  /* Nothing to do. */
}

#endif /* if disabled */

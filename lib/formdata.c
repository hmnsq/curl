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
#ضمن "curl_setup.h"

بنية Curl_easy;

#ضمن "formdata.h"

#𐤀𐤌 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#ضمن "urldata.h" /* for struct Curl_easy */
#ضمن "mime.h"
#ضمن "curlx/strdup.h"
#ضمن "bufref.h"
#ضمن "curlx/win32-fopen.h"


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
ساكن بنية curl_httppost *httppost_add(بنية FormInfo *src,
                                          بنية curl_httppost *parent_post,
                                          بنية curl_httppost **httppost,
                                          بنية curl_httppost **last_post)
{
  بنية curl_httppost *post;
  size_t namelength = src->namelength;
  𐤀𐤌(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  𐤀𐤌((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    أرجع عدم;
  post = curlx_calloc(1, حجم(بنية curl_httppost));
  𐤀𐤌(post) {
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
  وإلا
    أرجع عدم;

  𐤀𐤌(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  وإلا {
    /* make the previous point to this */
    𐤀𐤌(*last_post)
      (*last_post)->next = post;
    وإلا
      (*httppost) = post;

    (*last_post) = post;
  }
  أرجع post;
}

/* Allocate and initialize a new FormInfo structure. */
ساكن بنية FormInfo *forminfo_new(فراغ)
{
  بنية FormInfo *form_info = curlx_calloc(1, حجم(بنية FormInfo));

  𐤀𐤌(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->value);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  أرجع form_info;
}

/* Replace the target field data by a dynamic copy of it. */
ساكن CURLcode forminfo_copyfield(بنية bufref *field, size_t len)
{
  ثابت حرف *value = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  𐤀𐤌(value) {
    𐤀𐤌(!len)
      len = strlen(value);
    result = Curl_bufref_memdup0(field, value, len);
  }

  أرجع result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
ساكن فراغ forminfo_add(بنية FormInfo *form_info, بنية FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  𐤀𐤌(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

ساكن فراغ free_formlist(بنية FormInfo *ptr)
{
  لكل(; ptr; ptr = ptr->more) {
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

ساكن CURLFORMcode formadd_check(بنية FormInfo *first_form,
                                  بنية curl_httppost **httppost,
                                  بنية curl_httppost **last_post)
{
  ثابت حرف *prevtype = عدم;
  بنية FormInfo *form = عدم;
  بنية curl_httppost *post = عدم;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  لكل(form = first_form; form; form = form->more) {
    ثابت حرف *name = Curl_bufref_ptr(&form->name);

    𐤀𐤌(((!name || !Curl_bufref_ptr(&form->value)) && !post) ||
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
      أرجع CURL_FORMADD_INCOMPLETE;
    }
    𐤀𐤌(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ثابت حرف *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->value);
      ثابت حرف *type = Curl_mime_contenttype(f);
      𐤀𐤌(!type)
        type = prevtype;
      𐤀𐤌(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      𐤀𐤌(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        أرجع CURL_FORMADD_MEMORY;
    }
    𐤀𐤌(name && form->namelength && memchr(name, 0, form->namelength))
      أرجع CURL_FORMADD_NULL;
    𐤀𐤌(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      أرجع CURL_FORMADD_MEMORY;
    𐤀𐤌(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->value, (size_t)form->contentslength))
      أرجع CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    𐤀𐤌(!post)
      أرجع CURL_FORMADD_MEMORY;

    𐤀𐤌(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  أرجع CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
ساكن فراغ free_chain(بنية curl_httppost *c)
{
  طالما(c) {
    بنية curl_httppost *next = c->next;
    𐤀𐤌(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

ساكن CURLFORMcode formadd(بنية curl_httppost **httppost,
                            بنية curl_httppost **last_post, va_list params)
{
  بنية FormInfo *first_form, *curr, *form = عدم;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ثابت بنية curl_forms *forms = عدم;
  حرف *avalue = عدم;
  بنية curl_httppost *newchain = عدم;
  بنية curl_httppost *lastnode = عدم;

#define form_ptr_arg(t) (forms ? (t)(فراغ *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#وإلا
#define form_int_arg(t) (forms ? (t)(فراغ *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  𐤀𐤌(!first_form)
    أرجع CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  طالما(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    𐤀𐤌(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (حرف *)CURL_UNCONST(forms->value);

      forms++; /* advance this to next entry */
      𐤀𐤌(CURLFORM_END == option) {
        /* end of array state */
        forms = عدم;
        استمر;
      }
    }
    وإلا {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, صحيح);
      𐤀𐤌(CURLFORM_END == option)
        اكسر;
    }

    بدل(option) {
    حالة CURLFORM_ARRAY:
      𐤀𐤌(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      وإلا {
        forms = va_arg(params, بنية curl_forms *);
        𐤀𐤌(!forms)
          retval = CURL_FORMADD_NULL;
      }
      اكسر;

      /*
       * Set the Name property.
       */
    حالة CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    حالة CURLFORM_COPYNAME:
      𐤀𐤌(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا {
        avalue = form_ptr_arg(حرف *);
        𐤀𐤌(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, عدم); /* No copy yet. */
        وإلا
          retval = CURL_FORMADD_NULL;
      }
      اكسر;
    حالة CURLFORM_NAMELENGTH:
      𐤀𐤌(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا
        curr->namelength = (size_t)form_int_arg(long);
      اكسر;

      /*
       * Set the contents property.
       */
    حالة CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    حالة CURLFORM_COPYCONTENTS:
      𐤀𐤌(Curl_bufref_ptr(&curr->value))
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا {
        avalue = form_ptr_arg(حرف *);
        𐤀𐤌(avalue)
          Curl_bufref_set(&curr->value, avalue, 0, عدم); /* No copy yet. */
        وإلا
          retval = CURL_FORMADD_NULL;
      }
      اكسر;
    حالة CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      𐤀𐤌(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      وإلا
        curr->contentslength = (curl_off_t)clen;
      اكسر;
    }

    حالة CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      𐤀𐤌(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      وإلا {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      اكسر;
    }

      /* Get contents from a given filename */
    حالة CURLFORM_FILECONTENT:
      𐤀𐤌(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا {
        avalue = form_ptr_arg(حرف *);
        𐤀𐤌(avalue) {
          𐤀𐤌(Curl_bufref_memdup0(&curr->value, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          وإلا
            curr->flags |= HTTPPOST_READFILE;
        }
        وإلا
          retval = CURL_FORMADD_NULL;
      }
      اكسر;

      /* We upload a file */
    حالة CURLFORM_FILE:
      avalue = form_ptr_arg(حرف *);
      𐤀𐤌(Curl_bufref_ptr(&curr->value)) {
        𐤀𐤌(curr->flags & HTTPPOST_FILENAME) {
          𐤀𐤌(avalue) {
            form = forminfo_new();
            𐤀𐤌(!form ||
               Curl_bufref_memdup0(&form->value, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            وإلا {
              forminfo_add(form, curr);
              curr = form;
              form = عدم;
            }
          }
          وإلا
            retval = CURL_FORMADD_NULL;
        }
        وإلا
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      وإلا {
        𐤀𐤌(avalue) {
          𐤀𐤌(Curl_bufref_memdup0(&curr->value, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          وإلا
            curr->flags |= HTTPPOST_FILENAME;
        }
        وإلا
          retval = CURL_FORMADD_NULL;
      }
      اكسر;

    حالة CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      𐤀𐤌(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا {
        avalue = form_ptr_arg(حرف *);
        𐤀𐤌(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->value, avalue, 0, عدم);
        }
        وإلا
          retval = CURL_FORMADD_NULL;
      }
      اكسر;

    حالة CURLFORM_BUFFERLENGTH:
      𐤀𐤌(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا
        curr->bufferlength = (size_t)form_int_arg(long);
      اكسر;

    حالة CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      𐤀𐤌(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا {
        avalue = form_ptr_arg(حرف *);
        𐤀𐤌(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->value, avalue, 0, عدم);
        }
        وإلا
          retval = CURL_FORMADD_NULL;
      }
      اكسر;

    حالة CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(حرف *);
      𐤀𐤌(Curl_bufref_ptr(&curr->contenttype)) {
        𐤀𐤌(curr->flags & HTTPPOST_FILENAME) {
          𐤀𐤌(avalue) {
            form = forminfo_new();
            𐤀𐤌(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            وإلا {
              forminfo_add(form, curr);
              curr = form;
              form = عدم;
            }
          }
          وإلا
            retval = CURL_FORMADD_NULL;
        }
        وإلا
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      وإلا 𐤀𐤌(avalue) {
        𐤀𐤌(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      وإلا
        retval = CURL_FORMADD_NULL;
      اكسر;

    حالة CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      بنية curl_slist *list = form_ptr_arg(بنية curl_slist *);

      𐤀𐤌(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا
        curr->contentheader = list;

      اكسر;
    }
    حالة CURLFORM_FILENAME:
    حالة CURLFORM_BUFFER:
      avalue = form_ptr_arg(حرف *);
      𐤀𐤌(!avalue)
        retval = CURL_FORMADD_NULL;
      وإلا 𐤀𐤌(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      وإلا 𐤀𐤌(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      اكسر;

    افتراضي:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      اكسر;
    }
  }

  𐤀𐤌(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  𐤀𐤌(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  طالما(first_form) {
    بنية FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  𐤀𐤌(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    𐤀𐤌(*last_post)
      (*last_post)->next = newchain;
    وإلا
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  وإلا
    free_chain(newchain);

  أرجع retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(بنية curl_httppost **httppost,
                          بنية curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  أرجع form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
صحيح curl_formget(بنية curl_httppost *form, فراغ *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  𐤀𐤌(!append)
    أرجع (صحيح)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(عدم, &toppart, form, عدم);
  𐤀𐤌(!result)
    result = Curl_mime_prepare_headers(عدم, &toppart, "multipart/form-data",
                                       عدم, MIMESTRATEGY_FORM);

  طالما(!result) {
    حرف buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, حجم(buffer), &toppart);

    𐤀𐤌(!nread)
      اكسر;

    𐤀𐤌(nread > حجم(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      𐤀𐤌(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  أرجع (صحيح)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
فراغ curl_formfree(بنية curl_httppost *form)
{
  بنية curl_httppost *next;

  𐤀𐤌(!form)
    /* no form to free, get out of this */
    أرجع;

  𐤐𐤏𐤋 {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    𐤀𐤌(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    𐤀𐤌(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } طالما(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
ساكن CURLcode setname(curl_mimepart *part, ثابت حرف *name, size_t len)
{
  حرف *zname;
  CURLcode result;

  𐤀𐤌(!name || !len)
    أرجع curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  𐤀𐤌(!zname)
    أرجع CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  أرجع result;
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
                          بنية curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = عدم;
  curl_mimepart *part;
  بنية curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  𐤀𐤌(!post)
    أرجع result; /* no input => no output! */

  form = curl_mime_init(data);
  𐤀𐤌(!form)
    result = CURLE_OUT_OF_MEMORY;

  𐤀𐤌(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  لكل(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    𐤀𐤌(post->more) {
      part = curl_mime_addpart(form);
      𐤀𐤌(!part)
        result = CURLE_OUT_OF_MEMORY;
      𐤀𐤌(!result)
        result = setname(part, post->name, post->namelength);
      𐤀𐤌(!result) {
        multipart = curl_mime_init(data);
        𐤀𐤌(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      𐤀𐤌(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    لكل(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      𐤀𐤌(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      𐤀𐤌(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      𐤀𐤌(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      𐤀𐤌(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      𐤀𐤌(!result) {
        curl_off_t clen = post->contentslength;

        𐤀𐤌(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        𐤀𐤌(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          𐤀𐤌(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#𐤀𐤌 defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       عدم, (فراغ *)stdin);
#𐤀𐤌 defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
          }
          وإلا
            result = curl_mime_filedata(part, file->contents);
          𐤀𐤌(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, عدم);
        }
        وإلا 𐤀𐤌(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        وإلا 𐤀𐤌(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          𐤀𐤌(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, عدم, عدم, post->userp);
        }
        وإلا {
          size_t uclen;
          𐤀𐤌(!clen)
            uclen = CURL_ZERO_TERMINATED;
          وإلا
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      𐤀𐤌(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  𐤀𐤌(result)
    Curl_mime_cleanpart(finalform);

  أرجع result;
}

#وإلا /* if disabled */
CURLFORMcode curl_formadd(بنية curl_httppost **httppost,
                          بنية curl_httppost **last_post, ...)
{
  (فراغ)httppost;
  (فراغ)last_post;
  أرجع CURL_FORMADD_DISABLED;
}

صحيح curl_formget(بنية curl_httppost *form, فراغ *arg,
                 curl_formget_callback append)
{
  (فراغ)form;
  (فراغ)arg;
  (فراغ)append;
  أرجع CURL_FORMADD_DISABLED;
}

فراغ curl_formfree(بنية curl_httppost *form)
{
  (فراغ)form;
  /* Nothing to do. */
}

#endif /* if disabled */

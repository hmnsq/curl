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
#अन्तर्भाव "curl_setup.h"

संरचना Curl_easy;

#अन्तर्भाव "formdata.h"

#सचे !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#अन्तर्भाव "urldata.h" /* for struct Curl_easy */
#अन्तर्भाव "mime.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/win32-fopen.h"


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
स्थिर संरचना curl_httppost *httppost_add(संरचना FormInfo *src,
                                          संरचना curl_httppost *parent_post,
                                          संरचना curl_httppost **httppost,
                                          संरचना curl_httppost **last_post)
{
  संरचना curl_httppost *post;
  size_t namelength = src->namelength;
  सचे(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  सचे((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    पटिनिवत्त रिक्त;
  post = curlx_calloc(1, परिमाण(संरचना curl_httppost));
  सचे(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->मानम्));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  अञ्ञथा
    पटिनिवत्त रिक्त;

  सचे(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  अञ्ञथा {
    /* make the previous point to this */
    सचे(*last_post)
      (*last_post)->next = post;
    अञ्ञथा
      (*httppost) = post;

    (*last_post) = post;
  }
  पटिनिवत्त post;
}

/* Allocate and initialize a new FormInfo structure. */
स्थिर संरचना FormInfo *forminfo_new(शून्य)
{
  संरचना FormInfo *form_info = curlx_calloc(1, परिमाण(संरचना FormInfo));

  सचे(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->मानम्);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  पटिनिवत्त form_info;
}

/* Replace the target field data by a dynamic copy of it. */
स्थिर CURLcode forminfo_copyfield(संरचना bufref *field, size_t len)
{
  ध्रुव वर्ण *मानम् = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  सचे(मानम्) {
    सचे(!len)
      len = strlen(मानम्);
    result = Curl_bufref_memdup0(field, मानम्, len);
  }

  पटिनिवत्त result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
स्थिर शून्य forminfo_add(संरचना FormInfo *form_info, संरचना FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  सचे(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

स्थिर शून्य free_formlist(संरचना FormInfo *ptr)
{
  अत्थाय(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->मानम्);
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

स्थिर CURLFORMcode formadd_check(संरचना FormInfo *first_form,
                                  संरचना curl_httppost **httppost,
                                  संरचना curl_httppost **last_post)
{
  ध्रुव वर्ण *prevtype = रिक्त;
  संरचना FormInfo *form = रिक्त;
  संरचना curl_httppost *post = रिक्त;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  अत्थाय(form = first_form; form; form = form->more) {
    ध्रुव वर्ण *name = Curl_bufref_ptr(&form->name);

    सचे(((!name || !Curl_bufref_ptr(&form->मानम्)) && !post) ||
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
      पटिनिवत्त CURL_FORMADD_INCOMPLETE;
    }
    सचे(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ध्रुव वर्ण *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->मानम्);
      ध्रुव वर्ण *type = Curl_mime_contenttype(f);
      सचे(!type)
        type = prevtype;
      सचे(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      सचे(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        पटिनिवत्त CURL_FORMADD_MEMORY;
    }
    सचे(name && form->namelength && memchr(name, 0, form->namelength))
      पटिनिवत्त CURL_FORMADD_NULL;
    सचे(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      पटिनिवत्त CURL_FORMADD_MEMORY;
    सचे(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->मानम्, (size_t)form->contentslength))
      पटिनिवत्त CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    सचे(!post)
      पटिनिवत्त CURL_FORMADD_MEMORY;

    सचे(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  पटिनिवत्त CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
स्थिर शून्य free_chain(संरचना curl_httppost *c)
{
  याव(c) {
    संरचना curl_httppost *next = c->next;
    सचे(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

स्थिर CURLFORMcode formadd(संरचना curl_httppost **httppost,
                            संरचना curl_httppost **last_post, va_list params)
{
  संरचना FormInfo *first_form, *curr, *form = रिक्त;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ध्रुव संरचना curl_forms *forms = रिक्त;
  वर्ण *avalue = रिक्त;
  संरचना curl_httppost *newchain = रिक्त;
  संरचना curl_httppost *lastnode = रिक्त;

#define form_ptr_arg(t) (forms ? (t)(शून्य *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#अञ्ञथा
#define form_int_arg(t) (forms ? (t)(शून्य *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  सचे(!first_form)
    पटिनिवत्त CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  याव(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    सचे(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (वर्ण *)CURL_UNCONST(forms->मानम्);

      forms++; /* advance this to next entry */
      सचे(CURLFORM_END == option) {
        /* end of array state */
        forms = रिक्त;
        अनुवर्त;
      }
    }
    अञ्ञथा {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, पूर्णाङ्क);
      सचे(CURLFORM_END == option)
        भङ्ग;
    }

    विकल्प(option) {
    पक्ष CURLFORM_ARRAY:
      सचे(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      अञ्ञथा {
        forms = va_arg(params, संरचना curl_forms *);
        सचे(!forms)
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

      /*
       * Set the Name property.
       */
    पक्ष CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    पक्ष CURLFORM_COPYNAME:
      सचे(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा {
        avalue = form_ptr_arg(वर्ण *);
        सचे(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, रिक्त); /* No copy yet. */
        अञ्ञथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;
    पक्ष CURLFORM_NAMELENGTH:
      सचे(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा
        curr->namelength = (size_t)form_int_arg(long);
      भङ्ग;

      /*
       * Set the contents property.
       */
    पक्ष CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    पक्ष CURLFORM_COPYCONTENTS:
      सचे(Curl_bufref_ptr(&curr->मानम्))
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा {
        avalue = form_ptr_arg(वर्ण *);
        सचे(avalue)
          Curl_bufref_set(&curr->मानम्, avalue, 0, रिक्त); /* No copy yet. */
        अञ्ञथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;
    पक्ष CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      सचे(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      अञ्ञथा
        curr->contentslength = (curl_off_t)clen;
      भङ्ग;
    }

    पक्ष CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      सचे(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      अञ्ञथा {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      भङ्ग;
    }

      /* Get contents from a given filename */
    पक्ष CURLFORM_FILECONTENT:
      सचे(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा {
        avalue = form_ptr_arg(वर्ण *);
        सचे(avalue) {
          सचे(Curl_bufref_memdup0(&curr->मानम्, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          अञ्ञथा
            curr->flags |= HTTPPOST_READFILE;
        }
        अञ्ञथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

      /* We upload a file */
    पक्ष CURLFORM_FILE:
      avalue = form_ptr_arg(वर्ण *);
      सचे(Curl_bufref_ptr(&curr->मानम्)) {
        सचे(curr->flags & HTTPPOST_FILENAME) {
          सचे(avalue) {
            form = forminfo_new();
            सचे(!form ||
               Curl_bufref_memdup0(&form->मानम्, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            अञ्ञथा {
              forminfo_add(form, curr);
              curr = form;
              form = रिक्त;
            }
          }
          अञ्ञथा
            retval = CURL_FORMADD_NULL;
        }
        अञ्ञथा
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      अञ्ञथा {
        सचे(avalue) {
          सचे(Curl_bufref_memdup0(&curr->मानम्, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          अञ्ञथा
            curr->flags |= HTTPPOST_FILENAME;
        }
        अञ्ञथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

    पक्ष CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      सचे(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा {
        avalue = form_ptr_arg(वर्ण *);
        सचे(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->मानम्, avalue, 0, रिक्त);
        }
        अञ्ञथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

    पक्ष CURLFORM_BUFFERLENGTH:
      सचे(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा
        curr->bufferlength = (size_t)form_int_arg(long);
      भङ्ग;

    पक्ष CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      सचे(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा {
        avalue = form_ptr_arg(वर्ण *);
        सचे(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->मानम्, avalue, 0, रिक्त);
        }
        अञ्ञथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

    पक्ष CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(वर्ण *);
      सचे(Curl_bufref_ptr(&curr->contenttype)) {
        सचे(curr->flags & HTTPPOST_FILENAME) {
          सचे(avalue) {
            form = forminfo_new();
            सचे(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            अञ्ञथा {
              forminfo_add(form, curr);
              curr = form;
              form = रिक्त;
            }
          }
          अञ्ञथा
            retval = CURL_FORMADD_NULL;
        }
        अञ्ञथा
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      अञ्ञथा सचे(avalue) {
        सचे(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      अञ्ञथा
        retval = CURL_FORMADD_NULL;
      भङ्ग;

    पक्ष CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      संरचना curl_slist *list = form_ptr_arg(संरचना curl_slist *);

      सचे(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा
        curr->contentheader = list;

      भङ्ग;
    }
    पक्ष CURLFORM_FILENAME:
    पक्ष CURLFORM_BUFFER:
      avalue = form_ptr_arg(वर्ण *);
      सचे(!avalue)
        retval = CURL_FORMADD_NULL;
      अञ्ञथा सचे(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      अञ्ञथा सचे(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      भङ्ग;

    स्वभाव:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      भङ्ग;
    }
  }

  सचे(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  सचे(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  याव(first_form) {
    संरचना FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  सचे(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    सचे(*last_post)
      (*last_post)->next = newchain;
    अञ्ञथा
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  अञ्ञथा
    free_chain(newchain);

  पटिनिवत्त retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(संरचना curl_httppost **httppost,
                          संरचना curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  पटिनिवत्त form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
पूर्णाङ्क curl_formget(संरचना curl_httppost *form, शून्य *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  सचे(!append)
    पटिनिवत्त (पूर्णाङ्क)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(रिक्त, &toppart, form, रिक्त);
  सचे(!result)
    result = Curl_mime_prepare_headers(रिक्त, &toppart, "multipart/form-data",
                                       रिक्त, MIMESTRATEGY_FORM);

  याव(!result) {
    वर्ण buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, परिमाण(buffer), &toppart);

    सचे(!nread)
      भङ्ग;

    सचे(nread > परिमाण(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      सचे(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  पटिनिवत्त (पूर्णाङ्क)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
शून्य curl_formfree(संरचना curl_httppost *form)
{
  संरचना curl_httppost *next;

  सचे(!form)
    /* no form to free, get out of this */
    पटिनिवत्त;

  करोहि {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    सचे(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    सचे(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } याव(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
स्थिर CURLcode setname(curl_mimepart *part, ध्रुव वर्ण *name, size_t len)
{
  वर्ण *zname;
  CURLcode result;

  सचे(!name || !len)
    पटिनिवत्त curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  सचे(!zname)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  पटिनिवत्त result;
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
                          संरचना curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = रिक्त;
  curl_mimepart *part;
  संरचना curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  सचे(!post)
    पटिनिवत्त result; /* no input => no output! */

  form = curl_mime_init(data);
  सचे(!form)
    result = CURLE_OUT_OF_MEMORY;

  सचे(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  अत्थाय(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    सचे(post->more) {
      part = curl_mime_addpart(form);
      सचे(!part)
        result = CURLE_OUT_OF_MEMORY;
      सचे(!result)
        result = setname(part, post->name, post->namelength);
      सचे(!result) {
        multipart = curl_mime_init(data);
        सचे(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      सचे(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    अत्थाय(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      सचे(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      सचे(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      सचे(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      सचे(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      सचे(!result) {
        curl_off_t clen = post->contentslength;

        सचे(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        सचे(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          सचे(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#सचे defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic प्रक्षिप
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       रिक्त, (शून्य *)stdin);
#सचे defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic निष्कासय
#endif
          }
          अञ्ञथा
            result = curl_mime_filedata(part, file->contents);
          सचे(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, रिक्त);
        }
        अञ्ञथा सचे(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        अञ्ञथा सचे(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          सचे(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, रिक्त, रिक्त, post->userp);
        }
        अञ्ञथा {
          size_t uclen;
          सचे(!clen)
            uclen = CURL_ZERO_TERMINATED;
          अञ्ञथा
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      सचे(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  सचे(result)
    Curl_mime_cleanpart(finalform);

  पटिनिवत्त result;
}

#अञ्ञथा /* if disabled */
CURLFORMcode curl_formadd(संरचना curl_httppost **httppost,
                          संरचना curl_httppost **last_post, ...)
{
  (शून्य)httppost;
  (शून्य)last_post;
  पटिनिवत्त CURL_FORMADD_DISABLED;
}

पूर्णाङ्क curl_formget(संरचना curl_httppost *form, शून्य *arg,
                 curl_formget_callback append)
{
  (शून्य)form;
  (शून्य)arg;
  (शून्य)append;
  पटिनिवत्त CURL_FORMADD_DISABLED;
}

शून्य curl_formfree(संरचना curl_httppost *form)
{
  (शून्य)form;
  /* Nothing to do. */
}

#endif /* if disabled */

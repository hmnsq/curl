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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

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
  माप_प्रकार namelength = src->namelength;
  यदि(!namelength && Curl_bufref_ptr(&src->name))
    namelength = म_माप(Curl_bufref_ptr(&src->name));
  यदि((src->bufferlength > दीर्घ_उच्च) || (namelength > दीर्घ_उच्च))
    /* avoid overflow in typecasts below */
    प्रत्यागम रिक्त;
  post = curlx_calloc(1, परिमाण(संरचना curl_httppost));
  यदि(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (दीर्घ)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->मूल्य));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (दीर्घ)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  अन्यथा
    प्रत्यागम रिक्त;

  यदि(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  अन्यथा {
    /* make the previous point to this */
    यदि(*last_post)
      (*last_post)->next = post;
    अन्यथा
      (*httppost) = post;

    (*last_post) = post;
  }
  प्रत्यागम post;
}

/* Allocate and initialize a new FormInfo structure. */
स्थिर संरचना FormInfo *forminfo_new(शून्य)
{
  संरचना FormInfo *form_info = curlx_calloc(1, परिमाण(संरचना FormInfo));

  यदि(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->मूल्य);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  प्रत्यागम form_info;
}

/* Replace the target field data by a dynamic copy of it. */
स्थिर CURLcode forminfo_copyfield(संरचना bufref *field, माप_प्रकार len)
{
  ध्रुव वर्ण *मूल्य = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  यदि(मूल्य) {
    यदि(!len)
      len = म_माप(मूल्य);
    result = Curl_bufref_memdup0(field, मूल्य, len);
  }

  प्रत्यागम result;
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

  यदि(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

स्थिर शून्य free_formlist(संरचना FormInfo *ptr)
{
  प्रति(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->मूल्य);
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

  प्रति(form = first_form; form; form = form->more) {
    ध्रुव वर्ण *name = Curl_bufref_ptr(&form->name);

    यदि(((!name || !Curl_bufref_ptr(&form->मूल्य)) && !post) ||
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
      प्रत्यागम CURL_FORMADD_INCOMPLETE;
    }
    यदि(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ध्रुव वर्ण *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->मूल्य);
      ध्रुव वर्ण *type = Curl_mime_contenttype(f);
      यदि(!type)
        type = prevtype;
      यदि(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      यदि(Curl_bufref_memdup0(&form->contenttype, type, म_माप(type)))
        प्रत्यागम CURL_FORMADD_MEMORY;
    }
    यदि(name && form->namelength && स_प्रथम(name, 0, form->namelength))
      प्रत्यागम CURL_FORMADD_NULL;
    यदि(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      प्रत्यागम CURL_FORMADD_MEMORY;
    यदि(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->मूल्य, (माप_प्रकार)form->contentslength))
      प्रत्यागम CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    यदि(!post)
      प्रत्यागम CURL_FORMADD_MEMORY;

    यदि(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  प्रत्यागम CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
स्थिर शून्य free_chain(संरचना curl_httppost *c)
{
  यावत्(c) {
    संरचना curl_httppost *next = c->next;
    यदि(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

स्थिर CURLFORMcode formadd(संरचना curl_httppost **httppost,
                            संरचना curl_httppost **last_post, बहु_सूची params)
{
  संरचना FormInfo *first_form, *curr, *form = रिक्त;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ध्रुव संरचना curl_forms *forms = रिक्त;
  वर्ण *avalue = रिक्त;
  संरचना curl_httppost *newchain = रिक्त;
  संरचना curl_httppost *lastnode = रिक्त;

#define form_ptr_arg(t) (forms ? (t)(शून्य *)avalue : बहु_तर्क(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : बहु_तर्क(params, t))
#अन्यथा
#define form_int_arg(t) (forms ? (t)(शून्य *)avalue : बहु_तर्क(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  यदि(!first_form)
    प्रत्यागम CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  यावत्(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    यदि(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (वर्ण *)CURL_UNCONST(forms->मूल्य);

      forms++; /* advance this to next entry */
      यदि(CURLFORM_END == option) {
        /* end of array state */
        forms = रिक्त;
        अनुवर्त;
      }
    }
    अन्यथा {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)बहु_तर्क(params, पूर्णाङ्क);
      यदि(CURLFORM_END == option)
        भङ्ग;
    }

    विकल्प(option) {
    पक्ष CURLFORM_ARRAY:
      यदि(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      अन्यथा {
        forms = बहु_तर्क(params, संरचना curl_forms *);
        यदि(!forms)
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
      यदि(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(वर्ण *);
        यदि(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, रिक्त); /* No copy yet. */
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;
    पक्ष CURLFORM_NAMELENGTH:
      यदि(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->namelength = (माप_प्रकार)form_int_arg(दीर्घ);
      भङ्ग;

      /*
       * Set the contents property.
       */
    पक्ष CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    पक्ष CURLFORM_COPYCONTENTS:
      यदि(Curl_bufref_ptr(&curr->मूल्य))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(वर्ण *);
        यदि(avalue)
          Curl_bufref_set(&curr->मूल्य, avalue, 0, रिक्त); /* No copy yet. */
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;
    पक्ष CURLFORM_CONTENTSLENGTH: {
      दीर्घ clen = form_int_arg(दीर्घ);
      यदि(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      अन्यथा
        curr->contentslength = (curl_off_t)clen;
      भङ्ग;
    }

    पक्ष CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      यदि(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      अन्यथा {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      भङ्ग;
    }

      /* Get contents from a given filename */
    पक्ष CURLFORM_FILECONTENT:
      यदि(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(वर्ण *);
        यदि(avalue) {
          यदि(Curl_bufref_memdup0(&curr->मूल्य, avalue, म_माप(avalue)))
            retval = CURL_FORMADD_MEMORY;
          अन्यथा
            curr->flags |= HTTPPOST_READFILE;
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

      /* We upload a file */
    पक्ष CURLFORM_FILE:
      avalue = form_ptr_arg(वर्ण *);
      यदि(Curl_bufref_ptr(&curr->मूल्य)) {
        यदि(curr->flags & HTTPPOST_FILENAME) {
          यदि(avalue) {
            form = forminfo_new();
            यदि(!form ||
               Curl_bufref_memdup0(&form->मूल्य, avalue, म_माप(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            अन्यथा {
              forminfo_add(form, curr);
              curr = form;
              form = रिक्त;
            }
          }
          अन्यथा
            retval = CURL_FORMADD_NULL;
        }
        अन्यथा
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      अन्यथा {
        यदि(avalue) {
          यदि(Curl_bufref_memdup0(&curr->मूल्य, avalue, म_माप(avalue)))
            retval = CURL_FORMADD_MEMORY;
          अन्यथा
            curr->flags |= HTTPPOST_FILENAME;
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

    पक्ष CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      यदि(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(वर्ण *);
        यदि(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->मूल्य, avalue, 0, रिक्त);
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

    पक्ष CURLFORM_BUFFERLENGTH:
      यदि(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->bufferlength = (माप_प्रकार)form_int_arg(दीर्घ);
      भङ्ग;

    पक्ष CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      यदि(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(वर्ण *);
        यदि(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->मूल्य, avalue, 0, रिक्त);
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      भङ्ग;

    पक्ष CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(वर्ण *);
      यदि(Curl_bufref_ptr(&curr->contenttype)) {
        यदि(curr->flags & HTTPPOST_FILENAME) {
          यदि(avalue) {
            form = forminfo_new();
            यदि(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            म_माप(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            अन्यथा {
              forminfo_add(form, curr);
              curr = form;
              form = रिक्त;
            }
          }
          अन्यथा
            retval = CURL_FORMADD_NULL;
        }
        अन्यथा
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      अन्यथा यदि(avalue) {
        यदि(Curl_bufref_memdup0(&curr->contenttype, avalue, म_माप(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      अन्यथा
        retval = CURL_FORMADD_NULL;
      भङ्ग;

    पक्ष CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      संरचना curl_slist *list = form_ptr_arg(संरचना curl_slist *);

      यदि(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->contentheader = list;

      भङ्ग;
    }
    पक्ष CURLFORM_FILENAME:
    पक्ष CURLFORM_BUFFER:
      avalue = form_ptr_arg(वर्ण *);
      यदि(!avalue)
        retval = CURL_FORMADD_NULL;
      अन्यथा यदि(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा यदि(Curl_bufref_memdup0(&curr->showfilename, avalue, म_माप(avalue)))
        retval = CURL_FORMADD_MEMORY;
      भङ्ग;

    स्वभाव:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      भङ्ग;
    }
  }

  यदि(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  यदि(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  यावत्(first_form) {
    संरचना FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  यदि(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    यदि(*last_post)
      (*last_post)->next = newchain;
    अन्यथा
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  अन्यथा
    free_chain(newchain);

  प्रत्यागम retval;
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
  बहु_सूची arg;
  CURLFORMcode form;
  बहु_शुरू(arg, last_post);
  form = formadd(httppost, last_post, arg);
  बहु_पूर्ण(arg);
  प्रत्यागम form;
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
  यदि(!append)
    प्रत्यागम (पूर्णाङ्क)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(रिक्त, &toppart, form, रिक्त);
  यदि(!result)
    result = Curl_mime_prepare_headers(रिक्त, &toppart, "multipart/form-data",
                                       रिक्त, MIMESTRATEGY_FORM);

  यावत्(!result) {
    वर्ण buffer[8192];
    माप_प्रकार nread = Curl_mime_read(buffer, 1, परिमाण(buffer), &toppart);

    यदि(!nread)
      भङ्ग;

    यदि(nread > परिमाण(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      यदि(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  प्रत्यागम (पूर्णाङ्क)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
शून्य curl_formfree(संरचना curl_httppost *form)
{
  संरचना curl_httppost *next;

  यदि(!form)
    /* no form to free, get out of this */
    प्रत्यागम;

  कुरु {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    यदि(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    यदि(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } यावत्(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
स्थिर CURLcode setname(curl_mimepart *part, ध्रुव वर्ण *name, माप_प्रकार len)
{
  वर्ण *zname;
  CURLcode result;

  यदि(!name || !len)
    प्रत्यागम curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  यदि(!zname)
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  प्रत्यागम result;
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

  यदि(!post)
    प्रत्यागम result; /* no input => no output! */

  form = curl_mime_init(data);
  यदि(!form)
    result = CURLE_OUT_OF_MEMORY;

  यदि(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  प्रति(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    यदि(post->more) {
      part = curl_mime_addpart(form);
      यदि(!part)
        result = CURLE_OUT_OF_MEMORY;
      यदि(!result)
        result = setname(part, post->name, post->namelength);
      यदि(!result) {
        multipart = curl_mime_init(data);
        यदि(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      यदि(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    प्रति(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      यदि(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      यदि(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      यदि(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      यदि(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      यदि(!result) {
        curl_off_t clen = post->contentslength;

        यदि(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        यदि(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          यदि(!म_भेद(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#यदि defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic धकेलो
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)ख_पढ़ो,
                                       curlx_fseek,
                                       रिक्त, (शून्य *)मानक_निवेश);
#यदि defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic निकालो
#endif
          }
          अन्यथा
            result = curl_mime_filedata(part, file->contents);
          यदि(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, रिक्त);
        }
        अन्यथा यदि(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        अन्यथा यदि(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          यदि(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, रिक्त, रिक्त, post->userp);
        }
        अन्यथा {
          माप_प्रकार uclen;
          यदि(!clen)
            uclen = CURL_ZERO_TERMINATED;
          अन्यथा
            uclen = (माप_प्रकार)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      यदि(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  यदि(result)
    Curl_mime_cleanpart(finalform);

  प्रत्यागम result;
}

#अन्यथा /* if disabled */
CURLFORMcode curl_formadd(संरचना curl_httppost **httppost,
                          संरचना curl_httppost **last_post, ...)
{
  (शून्य)httppost;
  (शून्य)last_post;
  प्रत्यागम CURL_FORMADD_DISABLED;
}

पूर्णाङ्क curl_formget(संरचना curl_httppost *form, शून्य *arg,
                 curl_formget_callback append)
{
  (शून्य)form;
  (शून्य)arg;
  (शून्य)append;
  प्रत्यागम CURL_FORMADD_DISABLED;
}

शून्य curl_formfree(संरचना curl_httppost *form)
{
  (शून्य)form;
  /* Nothing to do. */
}

#endif /* if disabled */

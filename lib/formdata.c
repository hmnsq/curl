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

#जर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

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
  जर(!namelength && Curl_bufref_ptr(&src->name))
    namelength = म_माप(Curl_bufref_ptr(&src->name));
  जर((src->bufferlength > दीर्घ_उच्च) || (namelength > दीर्घ_उच्च))
    /* avoid overflow in typecasts below */
    परत रिक्त;
  post = curlx_calloc(1, आकार(संरचना curl_httppost));
  जर(post) {
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
    परत रिक्त;

  जर(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  अन्यथा {
    /* make the previous point to this */
    जर(*last_post)
      (*last_post)->next = post;
    अन्यथा
      (*httppost) = post;

    (*last_post) = post;
  }
  परत post;
}

/* Allocate and initialize a new FormInfo structure. */
स्थिर संरचना FormInfo *forminfo_new(शून्य)
{
  संरचना FormInfo *form_info = curlx_calloc(1, आकार(संरचना FormInfo));

  जर(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->मूल्य);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  परत form_info;
}

/* Replace the target field data by a dynamic copy of it. */
स्थिर CURLcode forminfo_copyfield(संरचना bufref *field, माप_प्रकार len)
{
  स्थिरांक अक्षर *मूल्य = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  जर(मूल्य) {
    जर(!len)
      len = म_माप(मूल्य);
    result = Curl_bufref_memdup0(field, मूल्य, len);
  }

  परत result;
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

  जर(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

स्थिर शून्य free_formlist(संरचना FormInfo *ptr)
{
  साठी(; ptr; ptr = ptr->more) {
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
  स्थिरांक अक्षर *prevtype = रिक्त;
  संरचना FormInfo *form = रिक्त;
  संरचना curl_httppost *post = रिक्त;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  साठी(form = first_form; form; form = form->more) {
    स्थिरांक अक्षर *name = Curl_bufref_ptr(&form->name);

    जर(((!name || !Curl_bufref_ptr(&form->मूल्य)) && !post) ||
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
      परत CURL_FORMADD_INCOMPLETE;
    }
    जर(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      स्थिरांक अक्षर *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->मूल्य);
      स्थिरांक अक्षर *type = Curl_mime_contenttype(f);
      जर(!type)
        type = prevtype;
      जर(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      जर(Curl_bufref_memdup0(&form->contenttype, type, म_माप(type)))
        परत CURL_FORMADD_MEMORY;
    }
    जर(name && form->namelength && स_प्रथम(name, 0, form->namelength))
      परत CURL_FORMADD_NULL;
    जर(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      परत CURL_FORMADD_MEMORY;
    जर(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->मूल्य, (माप_प्रकार)form->contentslength))
      परत CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    जर(!post)
      परत CURL_FORMADD_MEMORY;

    जर(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  परत CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
स्थिर शून्य free_chain(संरचना curl_httppost *c)
{
  पर्यंत(c) {
    संरचना curl_httppost *next = c->next;
    जर(c->more)
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
  स्थिरांक संरचना curl_forms *forms = रिक्त;
  अक्षर *avalue = रिक्त;
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
  जर(!first_form)
    परत CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  पर्यंत(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    जर(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (अक्षर *)CURL_UNCONST(forms->मूल्य);

      forms++; /* advance this to next entry */
      जर(CURLFORM_END == option) {
        /* end of array state */
        forms = रिक्त;
        सुरूठेवा;
      }
    }
    अन्यथा {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)बहु_तर्क(params, पूर्णांक);
      जर(CURLFORM_END == option)
        थांबा;
    }

    निवड(option) {
    स्थिती CURLFORM_ARRAY:
      जर(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      अन्यथा {
        forms = बहु_तर्क(params, संरचना curl_forms *);
        जर(!forms)
          retval = CURL_FORMADD_NULL;
      }
      थांबा;

      /*
       * Set the Name property.
       */
    स्थिती CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    स्थिती CURLFORM_COPYNAME:
      जर(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        जर(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, रिक्त); /* No copy yet. */
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      थांबा;
    स्थिती CURLFORM_NAMELENGTH:
      जर(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->namelength = (माप_प्रकार)form_int_arg(दीर्घ);
      थांबा;

      /*
       * Set the contents property.
       */
    स्थिती CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    स्थिती CURLFORM_COPYCONTENTS:
      जर(Curl_bufref_ptr(&curr->मूल्य))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        जर(avalue)
          Curl_bufref_set(&curr->मूल्य, avalue, 0, रिक्त); /* No copy yet. */
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      थांबा;
    स्थिती CURLFORM_CONTENTSLENGTH: {
      दीर्घ clen = form_int_arg(दीर्घ);
      जर(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      अन्यथा
        curr->contentslength = (curl_off_t)clen;
      थांबा;
    }

    स्थिती CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      जर(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      अन्यथा {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      थांबा;
    }

      /* Get contents from a given filename */
    स्थिती CURLFORM_FILECONTENT:
      जर(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        जर(avalue) {
          जर(Curl_bufref_memdup0(&curr->मूल्य, avalue, म_माप(avalue)))
            retval = CURL_FORMADD_MEMORY;
          अन्यथा
            curr->flags |= HTTPPOST_READFILE;
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      थांबा;

      /* We upload a file */
    स्थिती CURLFORM_FILE:
      avalue = form_ptr_arg(अक्षर *);
      जर(Curl_bufref_ptr(&curr->मूल्य)) {
        जर(curr->flags & HTTPPOST_FILENAME) {
          जर(avalue) {
            form = forminfo_new();
            जर(!form ||
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
        जर(avalue) {
          जर(Curl_bufref_memdup0(&curr->मूल्य, avalue, म_माप(avalue)))
            retval = CURL_FORMADD_MEMORY;
          अन्यथा
            curr->flags |= HTTPPOST_FILENAME;
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      थांबा;

    स्थिती CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      जर(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        जर(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->मूल्य, avalue, 0, रिक्त);
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      थांबा;

    स्थिती CURLFORM_BUFFERLENGTH:
      जर(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->bufferlength = (माप_प्रकार)form_int_arg(दीर्घ);
      थांबा;

    स्थिती CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      जर(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        जर(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->मूल्य, avalue, 0, रिक्त);
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      थांबा;

    स्थिती CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(अक्षर *);
      जर(Curl_bufref_ptr(&curr->contenttype)) {
        जर(curr->flags & HTTPPOST_FILENAME) {
          जर(avalue) {
            form = forminfo_new();
            जर(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
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
      अन्यथा जर(avalue) {
        जर(Curl_bufref_memdup0(&curr->contenttype, avalue, म_माप(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      अन्यथा
        retval = CURL_FORMADD_NULL;
      थांबा;

    स्थिती CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      संरचना curl_slist *list = form_ptr_arg(संरचना curl_slist *);

      जर(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->contentheader = list;

      थांबा;
    }
    स्थिती CURLFORM_FILENAME:
    स्थिती CURLFORM_BUFFER:
      avalue = form_ptr_arg(अक्षर *);
      जर(!avalue)
        retval = CURL_FORMADD_NULL;
      अन्यथा जर(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा जर(Curl_bufref_memdup0(&curr->showfilename, avalue, म_माप(avalue)))
        retval = CURL_FORMADD_MEMORY;
      थांबा;

    पूर्वनिर्धारित:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      थांबा;
    }
  }

  जर(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  जर(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  पर्यंत(first_form) {
    संरचना FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  जर(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    जर(*last_post)
      (*last_post)->next = newchain;
    अन्यथा
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  अन्यथा
    free_chain(newchain);

  परत retval;
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
  परत form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
पूर्णांक curl_formget(संरचना curl_httppost *form, शून्य *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  जर(!append)
    परत (पूर्णांक)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(रिक्त, &toppart, form, रिक्त);
  जर(!result)
    result = Curl_mime_prepare_headers(रिक्त, &toppart, "multipart/form-data",
                                       रिक्त, MIMESTRATEGY_FORM);

  पर्यंत(!result) {
    अक्षर buffer[8192];
    माप_प्रकार nread = Curl_mime_read(buffer, 1, आकार(buffer), &toppart);

    जर(!nread)
      थांबा;

    जर(nread > आकार(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      जर(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  परत (पूर्णांक)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
शून्य curl_formfree(संरचना curl_httppost *form)
{
  संरचना curl_httppost *next;

  जर(!form)
    /* no form to free, get out of this */
    परत;

  करा {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    जर(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    जर(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } पर्यंत(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
स्थिर CURLcode setname(curl_mimepart *part, स्थिरांक अक्षर *name, माप_प्रकार len)
{
  अक्षर *zname;
  CURLcode result;

  जर(!name || !len)
    परत curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  जर(!zname)
    परत CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  परत result;
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

  जर(!post)
    परत result; /* no input => no output! */

  form = curl_mime_init(data);
  जर(!form)
    result = CURLE_OUT_OF_MEMORY;

  जर(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  साठी(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    जर(post->more) {
      part = curl_mime_addpart(form);
      जर(!part)
        result = CURLE_OUT_OF_MEMORY;
      जर(!result)
        result = setname(part, post->name, post->namelength);
      जर(!result) {
        multipart = curl_mime_init(data);
        जर(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      जर(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    साठी(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      जर(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      जर(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      जर(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      जर(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      जर(!result) {
        curl_off_t clen = post->contentslength;

        जर(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        जर(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          जर(!म_भेद(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ढकला
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)ख_पढ़ो,
                                       curlx_fseek,
                                       रिक्त, (शून्य *)मानक_निवेश);
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic काढा
#endif
          }
          अन्यथा
            result = curl_mime_filedata(part, file->contents);
          जर(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, रिक्त);
        }
        अन्यथा जर(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        अन्यथा जर(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          जर(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, रिक्त, रिक्त, post->userp);
        }
        अन्यथा {
          माप_प्रकार uclen;
          जर(!clen)
            uclen = CURL_ZERO_TERMINATED;
          अन्यथा
            uclen = (माप_प्रकार)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      जर(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  जर(result)
    Curl_mime_cleanpart(finalform);

  परत result;
}

#अन्यथा /* if disabled */
CURLFORMcode curl_formadd(संरचना curl_httppost **httppost,
                          संरचना curl_httppost **last_post, ...)
{
  (शून्य)httppost;
  (शून्य)last_post;
  परत CURL_FORMADD_DISABLED;
}

पूर्णांक curl_formget(संरचना curl_httppost *form, शून्य *arg,
                 curl_formget_callback append)
{
  (शून्य)form;
  (शून्य)arg;
  (शून्य)append;
  परत CURL_FORMADD_DISABLED;
}

शून्य curl_formfree(संरचना curl_httppost *form)
{
  (शून्य)form;
  /* Nothing to do. */
}

#endif /* if disabled */

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
#অন্তর্ভাব "curl_setup.h"

কাঠামো Curl_easy;

#অন্তর্ভাব "formdata.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#অন্তর্ভাব "urldata.h" /* for struct Curl_easy */
#অন্তর্ভাব "mime.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curlx/win32-fopen.h"


#define HTTPPOST_PTRNAME     CURL_HTTPPOST_PTRNAME
#define HTTPPOST_FILENAME    CURL_HTTPPOST_FILENAME
#define HTTPPOST_PTRCONTENTS CURL_HTTPPOST_PTRCONTENTS
#define HTTPPOST_READFILE    CURL_HTTPPOST_READFILE
#define HTTPPOST_PTRBUFFER   CURL_HTTPPOST_PTRBUFFER
#define HTTPPOST_CALLBACK    CURL_HTTPPOST_CALLBACK
#define HTTPPOST_বাফার      CURL_HTTPPOST_বাফার

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
স্থির কাঠামো curl_httppost *httppost_add(কাঠামো FormInfo *src,
                                          কাঠামো curl_httppost *parent_post,
                                          কাঠামো curl_httppost **httppost,
                                          কাঠামো curl_httppost **last_post)
{
  কাঠামো curl_httppost *post;
  size_t namelength = src->namelength;
  যদি(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  যদি((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    ফেরত ফাঁকা;
  post = curlx_calloc(1, আকার(কাঠামো curl_httppost));
  যদি(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->মান));
    post->contentlen = src->contentslength;
    post->বাফার = src->বাফার;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  অন্যথা
    ফেরত ফাঁকা;

  যদি(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  অন্যথা {
    /* make the previous point to this */
    যদি(*last_post)
      (*last_post)->পরবর্তী = post;
    অন্যথা
      (*httppost) = post;

    (*last_post) = post;
  }
  ফেরত post;
}

/* Allocate and initialize a new FormInfo structure. */
স্থির কাঠামো FormInfo *forminfo_new(শূন্য)
{
  কাঠামো FormInfo *form_info = curlx_calloc(1, আকার(কাঠামো FormInfo));

  যদি(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->মান);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  ফেরত form_info;
}

/* Replace the target field data by a dynamic copy of it. */
স্থির CURLcode forminfo_copyfield(কাঠামো bufref *field, size_t len)
{
  ধ্রুবক অক্ষর *মান = Curl_bufref_ptr(field);
  CURLcode ফলাফল = CURLE_OK;

  যদি(মান) {
    যদি(!len)
      len = strlen(মান);
    ফলাফল = Curl_bufref_memdup0(field, মান, len);
  }

  ফেরত ফলাফল;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
স্থির শূন্য forminfo_add(কাঠামো FormInfo *form_info, কাঠামো FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  যদি(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

স্থির শূন্য free_formlist(কাঠামো FormInfo *ptr)
{
  জন্য(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->মান);
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

স্থির CURLFORMcode formadd_check(কাঠামো FormInfo *first_form,
                                  কাঠামো curl_httppost **httppost,
                                  কাঠামো curl_httppost **last_post)
{
  ধ্রুবক অক্ষর *prevtype = ফাঁকা;
  কাঠামো FormInfo *form = ফাঁকা;
  কাঠামো curl_httppost *post = ফাঁকা;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  জন্য(form = first_form; form; form = form->more) {
    ধ্রুবক অক্ষর *name = Curl_bufref_ptr(&form->name);

    যদি(((!name || !Curl_bufref_ptr(&form->মান)) && !post) ||
       (form->contentslength &&
        (form->flags & HTTPPOST_FILENAME)) ||
       ((form->flags & HTTPPOST_FILENAME) &&
        (form->flags & HTTPPOST_PTRCONTENTS)) ||

       (!form->বাফার &&
        (form->flags & HTTPPOST_বাফার) &&
        (form->flags & HTTPPOST_PTRBUFFER)) ||

       ((form->flags & HTTPPOST_READFILE) &&
        (form->flags & HTTPPOST_PTRCONTENTS))
      ) {
      ফেরত CURL_FORMADD_INCOMPLETE;
    }
    যদি(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_বাফার)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ধ্রুবক অক্ষর *f = Curl_bufref_ptr((form->flags & HTTPPOST_বাফার) ?
                                      &form->showfilename : &form->মান);
      ধ্রুবক অক্ষর *type = Curl_mime_contenttype(f);
      যদি(!type)
        type = prevtype;
      যদি(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      যদি(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        ফেরত CURL_FORMADD_MEMORY;
    }
    যদি(name && form->namelength && memchr(name, 0, form->namelength))
      ফেরত CURL_FORMADD_NULL;
    যদি(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      ফেরত CURL_FORMADD_MEMORY;
    যদি(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->মান, (size_t)form->contentslength))
      ফেরত CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    যদি(!post)
      ফেরত CURL_FORMADD_MEMORY;

    যদি(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  ফেরত CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
স্থির শূন্য free_chain(কাঠামো curl_httppost *c)
{
  যখন(c) {
    কাঠামো curl_httppost *পরবর্তী = c->পরবর্তী;
    যদি(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = পরবর্তী;
  }
}

স্থির CURLFORMcode formadd(কাঠামো curl_httppost **httppost,
                            কাঠামো curl_httppost **last_post, va_list params)
{
  কাঠামো FormInfo *first_form, *curr, *form = ফাঁকা;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ধ্রুবক কাঠামো curl_forms *forms = ফাঁকা;
  অক্ষর *avalue = ফাঁকা;
  কাঠামো curl_httppost *newchain = ফাঁকা;
  কাঠামো curl_httppost *lastnode = ফাঁকা;

#define form_ptr_arg(t) (forms ? (t)(শূন্য *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#অন্যথা
#define form_int_arg(t) (forms ? (t)(শূন্য *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  যদি(!first_form)
    ফেরত CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  যখন(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    যদি(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (অক্ষর *)CURL_UNCONST(forms->মান);

      forms++; /* advance this to next entry */
      যদি(CURLFORM_END == option) {
        /* end of array state */
        forms = ফাঁকা;
        চালু;
      }
    }
    অন্যথা {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, পূর্ণসংখ্যা);
      যদি(CURLFORM_END == option)
        বিরতি;
    }

    নির্বাচন(option) {
    ক্ষেত্র CURLFORM_ARRAY:
      যদি(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      অন্যথা {
        forms = va_arg(params, কাঠামো curl_forms *);
        যদি(!forms)
          retval = CURL_FORMADD_NULL;
      }
      বিরতি;

      /*
       * Set the Name property.
       */
    ক্ষেত্র CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    ক্ষেত্র CURLFORM_COPYNAME:
      যদি(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(অক্ষর *);
        যদি(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, ফাঁকা); /* No copy yet. */
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      বিরতি;
    ক্ষেত্র CURLFORM_NAMELENGTH:
      যদি(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা
        curr->namelength = (size_t)form_int_arg(long);
      বিরতি;

      /*
       * Set the contents property.
       */
    ক্ষেত্র CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    ক্ষেত্র CURLFORM_COPYCONTENTS:
      যদি(Curl_bufref_ptr(&curr->মান))
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(অক্ষর *);
        যদি(avalue)
          Curl_bufref_set(&curr->মান, avalue, 0, ফাঁকা); /* No copy yet. */
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      বিরতি;
    ক্ষেত্র CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      যদি(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      অন্যথা
        curr->contentslength = (curl_off_t)clen;
      বিরতি;
    }

    ক্ষেত্র CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      যদি(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      অন্যথা {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      বিরতি;
    }

      /* Get contents from a given filename */
    ক্ষেত্র CURLFORM_FILECONTENT:
      যদি(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(অক্ষর *);
        যদি(avalue) {
          যদি(Curl_bufref_memdup0(&curr->মান, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          অন্যথা
            curr->flags |= HTTPPOST_READFILE;
        }
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      বিরতি;

      /* We upload a file */
    ক্ষেত্র CURLFORM_FILE:
      avalue = form_ptr_arg(অক্ষর *);
      যদি(Curl_bufref_ptr(&curr->মান)) {
        যদি(curr->flags & HTTPPOST_FILENAME) {
          যদি(avalue) {
            form = forminfo_new();
            যদি(!form ||
               Curl_bufref_memdup0(&form->মান, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            অন্যথা {
              forminfo_add(form, curr);
              curr = form;
              form = ফাঁকা;
            }
          }
          অন্যথা
            retval = CURL_FORMADD_NULL;
        }
        অন্যথা
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      অন্যথা {
        যদি(avalue) {
          যদি(Curl_bufref_memdup0(&curr->মান, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          অন্যথা
            curr->flags |= HTTPPOST_FILENAME;
        }
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      বিরতি;

    ক্ষেত্র CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_বাফার;
      যদি(curr->বাফার)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(অক্ষর *);
        যদি(avalue) {
          curr->বাফার = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->মান, avalue, 0, ফাঁকা);
        }
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      বিরতি;

    ক্ষেত্র CURLFORM_BUFFERLENGTH:
      যদি(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা
        curr->bufferlength = (size_t)form_int_arg(long);
      বিরতি;

    ক্ষেত্র CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      যদি(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(অক্ষর *);
        যদি(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->মান, avalue, 0, ফাঁকা);
        }
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      বিরতি;

    ক্ষেত্র CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(অক্ষর *);
      যদি(Curl_bufref_ptr(&curr->contenttype)) {
        যদি(curr->flags & HTTPPOST_FILENAME) {
          যদি(avalue) {
            form = forminfo_new();
            যদি(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            অন্যথা {
              forminfo_add(form, curr);
              curr = form;
              form = ফাঁকা;
            }
          }
          অন্যথা
            retval = CURL_FORMADD_NULL;
        }
        অন্যথা
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      অন্যথা যদি(avalue) {
        যদি(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      অন্যথা
        retval = CURL_FORMADD_NULL;
      বিরতি;

    ক্ষেত্র CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      কাঠামো curl_slist *list = form_ptr_arg(কাঠামো curl_slist *);

      যদি(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা
        curr->contentheader = list;

      বিরতি;
    }
    ক্ষেত্র CURLFORM_FILENAME:
    ক্ষেত্র CURLFORM_বাফার:
      avalue = form_ptr_arg(অক্ষর *);
      যদি(!avalue)
        retval = CURL_FORMADD_NULL;
      অন্যথা যদি(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা যদি(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      বিরতি;

    পূর্বনির্ধারিত:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      বিরতি;
    }
  }

  যদি(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  যদি(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  যখন(first_form) {
    কাঠামো FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  যদি(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    যদি(*last_post)
      (*last_post)->পরবর্তী = newchain;
    অন্যথা
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  অন্যথা
    free_chain(newchain);

  ফেরত retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(কাঠামো curl_httppost **httppost,
                          কাঠামো curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  ফেরত form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
পূর্ণসংখ্যা curl_formget(কাঠামো curl_httppost *form, শূন্য *arg,
                 curl_formget_callback append)
{
  CURLcode ফলাফল;
  curl_mimepart toppart;

  /* Validate callback is provided */
  যদি(!append)
    ফেরত (পূর্ণসংখ্যা)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  ফলাফল = Curl_getformdata(ফাঁকা, &toppart, form, ফাঁকা);
  যদি(!ফলাফল)
    ফলাফল = Curl_mime_prepare_headers(ফাঁকা, &toppart, "multipart/form-data",
                                       ফাঁকা, MIMESTRATEGY_FORM);

  যখন(!ফলাফল) {
    অক্ষর বাফার[8192];
    size_t nread = Curl_mime_read(বাফার, 1, আকার(বাফার), &toppart);

    যদি(!nread)
      বিরতি;

    যদি(nread > আকার(বাফার) || append(arg, বাফার, nread) != nread) {
      ফলাফল = CURLE_READ_ERROR;
      যদি(nread == CURL_READFUNC_ABORT)
        ফলাফল = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  ফেরত (পূর্ণসংখ্যা)ফলাফল;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
শূন্য curl_formfree(কাঠামো curl_httppost *form)
{
  কাঠামো curl_httppost *পরবর্তী;

  যদি(!form)
    /* no form to free, get out of this */
    ফেরত;

  করো {
    পরবর্তী = form->পরবর্তী;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    যদি(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    যদি(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_বাফার | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = পরবর্তী;
  } যখন(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
স্থির CURLcode setname(curl_mimepart *part, ধ্রুবক অক্ষর *name, size_t len)
{
  অক্ষর *zname;
  CURLcode ফলাফল;

  যদি(!name || !len)
    ফেরত curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  যদি(!zname)
    ফেরত CURLE_OUT_OF_MEMORY;
  ফলাফল = curl_mime_name(part, zname);
  curlx_free(zname);
  ফেরত ফলাফল;
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

CURLcode Curl_getformdata(CURL *তথ্য,
                          curl_mimepart *finalform,
                          কাঠামো curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode ফলাফল = CURLE_OK;
  curl_mime *form = ফাঁকা;
  curl_mimepart *part;
  কাঠামো curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  যদি(!post)
    ফেরত ফলাফল; /* no input => no output! */

  form = curl_mime_init(তথ্য);
  যদি(!form)
    ফলাফল = CURLE_OUT_OF_MEMORY;

  যদি(!ফলাফল)
    ফলাফল = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  জন্য(; !ফলাফল && post; post = post->পরবর্তী) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    যদি(post->more) {
      part = curl_mime_addpart(form);
      যদি(!part)
        ফলাফল = CURLE_OUT_OF_MEMORY;
      যদি(!ফলাফল)
        ফলাফল = setname(part, post->name, post->namelength);
      যদি(!ফলাফল) {
        multipart = curl_mime_init(তথ্য);
        যদি(!multipart)
          ফলাফল = CURLE_OUT_OF_MEMORY;
      }
      যদি(!ফলাফল)
        ফলাফল = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    জন্য(file = post; !ফলাফল && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      যদি(!part)
        ফলাফল = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      যদি(!ফলাফল)
        ফলাফল = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      যদি(!ফলাফল && file->contenttype)
        ফলাফল = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      যদি(!ফলাফল && !post->more)
        ফলাফল = setname(part, post->name, post->namelength);

      /* Process contents. */
      যদি(!ফলাফল) {
        curl_off_t clen = post->contentslength;

        যদি(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        যদি(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          যদি(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            ফলাফল = curl_mime_তথ্য_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       ফাঁকা, (শূন্য *)stdin);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
          }
          অন্যথা
            ফলাফল = curl_mime_filedata(part, file->contents);
          যদি(!ফলাফল && (post->flags & HTTPPOST_READFILE))
            ফলাফল = curl_mime_filename(part, ফাঁকা);
        }
        অন্যথা যদি(post->flags & HTTPPOST_বাফার)
          ফলাফল = curl_mime_তথ্য(part, post->বাফার,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        অন্যথা যদি(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          যদি(!clen)
            clen = -1;
          ফলাফল = curl_mime_তথ্য_cb(part, clen,
                                     fread_func, ফাঁকা, ফাঁকা, post->userp);
        }
        অন্যথা {
          size_t uclen;
          যদি(!clen)
            uclen = CURL_ZERO_TERMINATED;
          অন্যথা
            uclen = (size_t)clen;
          ফলাফল = curl_mime_তথ্য(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      যদি(!ফলাফল && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_বাফার |
                                        HTTPPOST_CALLBACK))))
        ফলাফল = curl_mime_filename(part, post->showfilename);
    }
  }

  যদি(ফলাফল)
    Curl_mime_cleanpart(finalform);

  ফেরত ফলাফল;
}

#অন্যথা /* if disabled */
CURLFORMcode curl_formadd(কাঠামো curl_httppost **httppost,
                          কাঠামো curl_httppost **last_post, ...)
{
  (শূন্য)httppost;
  (শূন্য)last_post;
  ফেরত CURL_FORMADD_DISABLED;
}

পূর্ণসংখ্যা curl_formget(কাঠামো curl_httppost *form, শূন্য *arg,
                 curl_formget_callback append)
{
  (শূন্য)form;
  (শূন্য)arg;
  (শূন্য)append;
  ফেরত CURL_FORMADD_DISABLED;
}

শূন্য curl_formfree(কাঠামো curl_httppost *form)
{
  (শূন্য)form;
  /* Nothing to do. */
}

#endif /* if disabled */

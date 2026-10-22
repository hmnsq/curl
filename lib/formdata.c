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
#شامل "curl_setup.h"

ساختار Curl_easy;

#شامل "formdata.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#شامل "urldata.h" /* for struct Curl_easy */
#شامل "mime.h"
#شامل "curlx/strdup.h"
#شامل "bufref.h"
#شامل "curlx/win32-fopen.h"


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
ایستا ساختار curl_httppost *httppost_add(ساختار FormInfo *src,
                                          ساختار curl_httppost *parent_post,
                                          ساختار curl_httppost **httppost,
                                          ساختار curl_httppost **last_post)
{
  ساختار curl_httppost *post;
  size_t namelength = src->namelength;
  اگر(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  اگر((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    بازگشت پوچ;
  post = curlx_calloc(1, اندازه(ساختار curl_httppost));
  اگر(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->مقدار));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  وگرنه
    بازگشت پوچ;

  اگر(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  وگرنه {
    /* make the previous point to this */
    اگر(*last_post)
      (*last_post)->next = post;
    وگرنه
      (*httppost) = post;

    (*last_post) = post;
  }
  بازگشت post;
}

/* Allocate and initialize a new FormInfo structure. */
ایستا ساختار FormInfo *forminfo_new(تهی)
{
  ساختار FormInfo *form_info = curlx_calloc(1, اندازه(ساختار FormInfo));

  اگر(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->مقدار);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  بازگشت form_info;
}

/* Replace the target field data by a dynamic copy of it. */
ایستا CURLcode forminfo_copyfield(ساختار bufref *field, size_t len)
{
  ثابت نویسه *مقدار = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  اگر(مقدار) {
    اگر(!len)
      len = strlen(مقدار);
    result = Curl_bufref_memdup0(field, مقدار, len);
  }

  بازگشت result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
ایستا تهی forminfo_add(ساختار FormInfo *form_info, ساختار FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  اگر(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

ایستا تهی free_formlist(ساختار FormInfo *ptr)
{
  برای(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->مقدار);
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

ایستا CURLFORMcode formadd_check(ساختار FormInfo *first_form,
                                  ساختار curl_httppost **httppost,
                                  ساختار curl_httppost **last_post)
{
  ثابت نویسه *prevtype = پوچ;
  ساختار FormInfo *form = پوچ;
  ساختار curl_httppost *post = پوچ;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  برای(form = first_form; form; form = form->more) {
    ثابت نویسه *name = Curl_bufref_ptr(&form->name);

    اگر(((!name || !Curl_bufref_ptr(&form->مقدار)) && !post) ||
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
      بازگشت CURL_FORMADD_INCOMPLETE;
    }
    اگر(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ثابت نویسه *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->مقدار);
      ثابت نویسه *type = Curl_mime_contenttype(f);
      اگر(!type)
        type = prevtype;
      اگر(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      اگر(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        بازگشت CURL_FORMADD_MEMORY;
    }
    اگر(name && form->namelength && memchr(name, 0, form->namelength))
      بازگشت CURL_FORMADD_NULL;
    اگر(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      بازگشت CURL_FORMADD_MEMORY;
    اگر(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->مقدار, (size_t)form->contentslength))
      بازگشت CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    اگر(!post)
      بازگشت CURL_FORMADD_MEMORY;

    اگر(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  بازگشت CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
ایستا تهی free_chain(ساختار curl_httppost *c)
{
  وقتی(c) {
    ساختار curl_httppost *next = c->next;
    اگر(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

ایستا CURLFORMcode formadd(ساختار curl_httppost **httppost,
                            ساختار curl_httppost **last_post, va_list params)
{
  ساختار FormInfo *first_form, *curr, *form = پوچ;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ثابت ساختار curl_forms *forms = پوچ;
  نویسه *avalue = پوچ;
  ساختار curl_httppost *newchain = پوچ;
  ساختار curl_httppost *lastnode = پوچ;

#define form_ptr_arg(t) (forms ? (t)(تهی *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#وگرنه
#define form_int_arg(t) (forms ? (t)(تهی *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  اگر(!first_form)
    بازگشت CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  وقتی(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    اگر(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (نویسه *)CURL_UNCONST(forms->مقدار);

      forms++; /* advance this to next entry */
      اگر(CURLFORM_END == option) {
        /* end of array state */
        forms = پوچ;
        ادامه;
      }
    }
    وگرنه {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, صحیح);
      اگر(CURLFORM_END == option)
        بشکن;
    }

    انتخاب(option) {
    حالت CURLFORM_ARRAY:
      اگر(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      وگرنه {
        forms = va_arg(params, ساختار curl_forms *);
        اگر(!forms)
          retval = CURL_FORMADD_NULL;
      }
      بشکن;

      /*
       * Set the Name property.
       */
    حالت CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    حالت CURLFORM_COPYNAME:
      اگر(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه {
        avalue = form_ptr_arg(نویسه *);
        اگر(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, پوچ); /* No copy yet. */
        وگرنه
          retval = CURL_FORMADD_NULL;
      }
      بشکن;
    حالت CURLFORM_NAMELENGTH:
      اگر(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه
        curr->namelength = (size_t)form_int_arg(long);
      بشکن;

      /*
       * Set the contents property.
       */
    حالت CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    حالت CURLFORM_COPYCONTENTS:
      اگر(Curl_bufref_ptr(&curr->مقدار))
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه {
        avalue = form_ptr_arg(نویسه *);
        اگر(avalue)
          Curl_bufref_set(&curr->مقدار, avalue, 0, پوچ); /* No copy yet. */
        وگرنه
          retval = CURL_FORMADD_NULL;
      }
      بشکن;
    حالت CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      اگر(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      وگرنه
        curr->contentslength = (curl_off_t)clen;
      بشکن;
    }

    حالت CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      اگر(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      وگرنه {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      بشکن;
    }

      /* Get contents from a given filename */
    حالت CURLFORM_FILECONTENT:
      اگر(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه {
        avalue = form_ptr_arg(نویسه *);
        اگر(avalue) {
          اگر(Curl_bufref_memdup0(&curr->مقدار, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          وگرنه
            curr->flags |= HTTPPOST_READFILE;
        }
        وگرنه
          retval = CURL_FORMADD_NULL;
      }
      بشکن;

      /* We upload a file */
    حالت CURLFORM_FILE:
      avalue = form_ptr_arg(نویسه *);
      اگر(Curl_bufref_ptr(&curr->مقدار)) {
        اگر(curr->flags & HTTPPOST_FILENAME) {
          اگر(avalue) {
            form = forminfo_new();
            اگر(!form ||
               Curl_bufref_memdup0(&form->مقدار, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            وگرنه {
              forminfo_add(form, curr);
              curr = form;
              form = پوچ;
            }
          }
          وگرنه
            retval = CURL_FORMADD_NULL;
        }
        وگرنه
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      وگرنه {
        اگر(avalue) {
          اگر(Curl_bufref_memdup0(&curr->مقدار, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          وگرنه
            curr->flags |= HTTPPOST_FILENAME;
        }
        وگرنه
          retval = CURL_FORMADD_NULL;
      }
      بشکن;

    حالت CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      اگر(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه {
        avalue = form_ptr_arg(نویسه *);
        اگر(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->مقدار, avalue, 0, پوچ);
        }
        وگرنه
          retval = CURL_FORMADD_NULL;
      }
      بشکن;

    حالت CURLFORM_BUFFERLENGTH:
      اگر(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه
        curr->bufferlength = (size_t)form_int_arg(long);
      بشکن;

    حالت CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      اگر(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه {
        avalue = form_ptr_arg(نویسه *);
        اگر(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->مقدار, avalue, 0, پوچ);
        }
        وگرنه
          retval = CURL_FORMADD_NULL;
      }
      بشکن;

    حالت CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(نویسه *);
      اگر(Curl_bufref_ptr(&curr->contenttype)) {
        اگر(curr->flags & HTTPPOST_FILENAME) {
          اگر(avalue) {
            form = forminfo_new();
            اگر(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            وگرنه {
              forminfo_add(form, curr);
              curr = form;
              form = پوچ;
            }
          }
          وگرنه
            retval = CURL_FORMADD_NULL;
        }
        وگرنه
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      وگرنه اگر(avalue) {
        اگر(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      وگرنه
        retval = CURL_FORMADD_NULL;
      بشکن;

    حالت CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      ساختار curl_slist *list = form_ptr_arg(ساختار curl_slist *);

      اگر(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه
        curr->contentheader = list;

      بشکن;
    }
    حالت CURLFORM_FILENAME:
    حالت CURLFORM_BUFFER:
      avalue = form_ptr_arg(نویسه *);
      اگر(!avalue)
        retval = CURL_FORMADD_NULL;
      وگرنه اگر(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      وگرنه اگر(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      بشکن;

    پیشفرض:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      بشکن;
    }
  }

  اگر(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  اگر(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  وقتی(first_form) {
    ساختار FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  اگر(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    اگر(*last_post)
      (*last_post)->next = newchain;
    وگرنه
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  وگرنه
    free_chain(newchain);

  بازگشت retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(ساختار curl_httppost **httppost,
                          ساختار curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  بازگشت form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
صحیح curl_formget(ساختار curl_httppost *form, تهی *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  اگر(!append)
    بازگشت (صحیح)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(پوچ, &toppart, form, پوچ);
  اگر(!result)
    result = Curl_mime_prepare_headers(پوچ, &toppart, "multipart/form-data",
                                       پوچ, MIMESTRATEGY_FORM);

  وقتی(!result) {
    نویسه buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, اندازه(buffer), &toppart);

    اگر(!nread)
      بشکن;

    اگر(nread > اندازه(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      اگر(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  بازگشت (صحیح)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
تهی curl_formfree(ساختار curl_httppost *form)
{
  ساختار curl_httppost *next;

  اگر(!form)
    /* no form to free, get out of this */
    بازگشت;

  انجامبده {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    اگر(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    اگر(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } وقتی(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
ایستا CURLcode setname(curl_mimepart *part, ثابت نویسه *name, size_t len)
{
  نویسه *zname;
  CURLcode result;

  اگر(!name || !len)
    بازگشت curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  اگر(!zname)
    بازگشت CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  بازگشت result;
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
                          ساختار curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = پوچ;
  curl_mimepart *part;
  ساختار curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  اگر(!post)
    بازگشت result; /* no input => no output! */

  form = curl_mime_init(data);
  اگر(!form)
    result = CURLE_OUT_OF_MEMORY;

  اگر(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  برای(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    اگر(post->more) {
      part = curl_mime_addpart(form);
      اگر(!part)
        result = CURLE_OUT_OF_MEMORY;
      اگر(!result)
        result = setname(part, post->name, post->namelength);
      اگر(!result) {
        multipart = curl_mime_init(data);
        اگر(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      اگر(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    برای(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      اگر(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      اگر(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      اگر(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      اگر(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      اگر(!result) {
        curl_off_t clen = post->contentslength;

        اگر(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        اگر(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          اگر(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       پوچ, (تهی *)stdin);
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic برداشت
#endif
          }
          وگرنه
            result = curl_mime_filedata(part, file->contents);
          اگر(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, پوچ);
        }
        وگرنه اگر(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        وگرنه اگر(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          اگر(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, پوچ, پوچ, post->userp);
        }
        وگرنه {
          size_t uclen;
          اگر(!clen)
            uclen = CURL_ZERO_TERMINATED;
          وگرنه
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      اگر(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  اگر(result)
    Curl_mime_cleanpart(finalform);

  بازگشت result;
}

#وگرنه /* if disabled */
CURLFORMcode curl_formadd(ساختار curl_httppost **httppost,
                          ساختار curl_httppost **last_post, ...)
{
  (تهی)httppost;
  (تهی)last_post;
  بازگشت CURL_FORMADD_DISABLED;
}

صحیح curl_formget(ساختار curl_httppost *form, تهی *arg,
                 curl_formget_callback append)
{
  (تهی)form;
  (تهی)arg;
  (تهی)append;
  بازگشت CURL_FORMADD_DISABLED;
}

تهی curl_formfree(ساختار curl_httppost *form)
{
  (تهی)form;
  /* Nothing to do. */
}

#endif /* if disabled */

/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "vauth/vauth.h"
#অন্তর্ভাৱ "creds.h"
#অন্তর্ভাৱ "curlx/multibyte.h"
#অন্তর্ভাৱ "url.h"

/*
 * Curl_auth_build_spn()
 *
 * This is used to build an SPN string in the following formats:
 *
 * service/host@realm (Not currently used)
 * service/host       (Not used by GSS-API)
 * service@realm      (Not used by Windows SSPI)
 *
 * Parameters:
 *
 * service  [in] - The service type such as http, smtp, pop or imap.
 * host     [in] - The hostname.
 * realm    [in] - The realm.
 *
 * Returns a pointer to the newly allocated SPN.
 */
#ifndef USE_WINDOWS_SSPI
ৱর্ণ *Curl_auth_build_spn(ধ্রুৱ ৱর্ণ *service, ধ্রুৱ ৱর্ণ *host,
                          ধ্রুৱ ৱর্ণ *realm)
{
  ৱর্ণ *spn = রিক্ত;

  /* Generate our SPN */
  যদি(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  অন্যথা যদি(host)
    spn = curl_maprintf("%s/%s", service, host);
  অন্যথা যদি(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  ঘূৰাই spn;
}
#অন্যথা
TCHAR *Curl_auth_build_spn(ধ্রুৱ ৱর্ণ *service, ধ্রুৱ ৱর্ণ *host,
                           ধ্রুৱ ৱর্ণ *realm)
{
  ৱর্ণ *utf8_spn = রিক্ত;
  TCHAR *tchar_spn = রিক্ত;

  (শূন্য)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  যদি(!utf8_spn)
    ঘূৰাই রিক্ত;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  ঘূৰাই tchar_spn;
}
#endif /* USE_WINDOWS_SSPI */

/*
 * Curl_auth_user_contains_domain()
 *
 * This is used to test if the specified user contains a Windows domain name as
 * follows:
 *
 * Domain\User (Down-level Logon Name)
 * Domain/User (curl Down-level format - for compatibility with existing code)
 * User@Domain (User Principal Name)
 *
 * Note: The username may be empty when using a GSS-API library or Windows
 * SSPI as the user and domain are either obtained from the credentials cache
 * when using GSS-API or via the currently logged in user's credentials when
 * using Windows SSPI.
 *
 * Parameters:
 *
 * user  [in] - The username.
 *
 * Returns TRUE on success; otherwise FALSE.
 */
bool Curl_auth_user_contains_domain(সংরচনা Curl_creds *creds)
{
  bool valid = FALSE;

  যদি(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    ধ্রুৱ ৱর্ণ *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  অন্যথা
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  ঘূৰাই valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(সংরচনা Curl_easy *data)
{
  ঘূৰাই Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(সংরচনা Curl_easy *data,
                                 সংরচনা Curl_peer *origin)
{
  ঘূৰাই data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
স্থির শূন্য ntlm_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা ntlmdata *ntlm = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

সংরচনা ntlmdata *Curl_auth_ntlm_get(সংরচনা connectdata *conn, bool proxy)
{
  ধ্রুৱ ৱর্ণ *চাবি = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  সংরচনা ntlmdata *ntlm = Curl_conn_meta_get(conn, চাবি);
  যদি(!ntlm) {
    ntlm = curlx_calloc(1, পরিমাণ(*ntlm));
    যদি(!ntlm || Curl_conn_meta_set(conn, চাবি, ntlm, ntlm_conn_dtor))
      ঘূৰাই রিক্ত;
  }
  ঘূৰাই ntlm;
}

শূন্য Curl_auth_ntlm_remove(সংরচনা connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
স্থির শূন্য krb5_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা kerberos5data *krb5 = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

সংরচনা kerberos5data *Curl_auth_krb5_get(সংরচনা connectdata *conn)
{
  সংরচনা kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  যদি(!krb5) {
    krb5 = curlx_calloc(1, পরিমাণ(*krb5));
    যদি(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      ঘূৰাই রিক্ত;
  }
  ঘূৰাই krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
স্থির শূন্য gsasl_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা gsasldata *gsasl = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

সংরচনা gsasldata *Curl_auth_gsasl_get(সংরচনা connectdata *conn)
{
  সংরচনা gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  যদি(!gsasl) {
    gsasl = curlx_calloc(1, পরিমাণ(*gsasl));
    যদি(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      ঘূৰাই রিক্ত;
  }
  ঘূৰাই gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
স্থির শূন্য nego_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা negotiatedata *nego = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

সংরচনা negotiatedata *Curl_auth_nego_get(সংরচনা connectdata *conn, bool proxy)
{
  ধ্রুৱ ৱর্ণ *চাবি = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  সংরচনা negotiatedata *nego = Curl_conn_meta_get(conn, চাবি);
  যদি(!nego) {
    nego = curlx_calloc(1, পরিমাণ(*nego));
    যদি(!nego || Curl_conn_meta_set(conn, চাবি, nego, nego_conn_dtor))
      ঘূৰাই রিক্ত;
  }
  ঘূৰাই nego;
}
#endif /* USE_SPNEGO */

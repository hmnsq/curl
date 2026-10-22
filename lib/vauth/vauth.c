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
#شامل "curl_setup.h"

#شامل "vauth/vauth.h"
#شامل "creds.h"
#شامل "curlx/multibyte.h"
#شامل "url.h"

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
نویسه *Curl_auth_build_spn(ثابت نویسه *service, ثابت نویسه *host,
                          ثابت نویسه *realm)
{
  نویسه *spn = پوچ;

  /* Generate our SPN */
  اگر(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  وگرنه اگر(host)
    spn = curl_maprintf("%s/%s", service, host);
  وگرنه اگر(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  بازگشت spn;
}
#وگرنه
TCHAR *Curl_auth_build_spn(ثابت نویسه *service, ثابت نویسه *host,
                           ثابت نویسه *realm)
{
  نویسه *utf8_spn = پوچ;
  TCHAR *tchar_spn = پوچ;

  (تهی)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  اگر(!utf8_spn)
    بازگشت پوچ;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  بازگشت tchar_spn;
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
bool Curl_auth_user_contains_domain(ساختار Curl_creds *creds)
{
  bool valid = FALSE;

  اگر(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    ثابت نویسه *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  وگرنه
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  بازگشت valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(ساختار Curl_easy *data)
{
  بازگشت Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(ساختار Curl_easy *data,
                                 ساختار Curl_peer *origin)
{
  بازگشت data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
ایستا تهی ntlm_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار ntlmdata *ntlm = entry;
  (تهی)کلید;
  (تهی)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

ساختار ntlmdata *Curl_auth_ntlm_get(ساختار connectdata *conn, bool proxy)
{
  ثابت نویسه *کلید = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  ساختار ntlmdata *ntlm = Curl_conn_meta_get(conn, کلید);
  اگر(!ntlm) {
    ntlm = curlx_calloc(1, اندازه(*ntlm));
    اگر(!ntlm || Curl_conn_meta_set(conn, کلید, ntlm, ntlm_conn_dtor))
      بازگشت پوچ;
  }
  بازگشت ntlm;
}

تهی Curl_auth_ntlm_remove(ساختار connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
ایستا تهی krb5_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار kerberos5data *krb5 = entry;
  (تهی)کلید;
  (تهی)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

ساختار kerberos5data *Curl_auth_krb5_get(ساختار connectdata *conn)
{
  ساختار kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  اگر(!krb5) {
    krb5 = curlx_calloc(1, اندازه(*krb5));
    اگر(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      بازگشت پوچ;
  }
  بازگشت krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
ایستا تهی gsasl_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار gsasldata *gsasl = entry;
  (تهی)کلید;
  (تهی)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

ساختار gsasldata *Curl_auth_gsasl_get(ساختار connectdata *conn)
{
  ساختار gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  اگر(!gsasl) {
    gsasl = curlx_calloc(1, اندازه(*gsasl));
    اگر(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      بازگشت پوچ;
  }
  بازگشت gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
ایستا تهی nego_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار negotiatedata *nego = entry;
  (تهی)کلید;
  (تهی)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

ساختار negotiatedata *Curl_auth_nego_get(ساختار connectdata *conn, bool proxy)
{
  ثابت نویسه *کلید = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  ساختار negotiatedata *nego = Curl_conn_meta_get(conn, کلید);
  اگر(!nego) {
    nego = curlx_calloc(1, اندازه(*nego));
    اگر(!nego || Curl_conn_meta_set(conn, کلید, nego, nego_conn_dtor))
      بازگشت پوچ;
  }
  بازگشت nego;
}
#endif /* USE_SPNEGO */

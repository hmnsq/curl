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
#ضمن "curl_setup.h"

#ضمن "vauth/vauth.h"
#ضمن "creds.h"
#ضمن "curlx/multibyte.h"
#ضمن "url.h"

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
حرف *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                          ثابت حرف *realm)
{
  حرف *spn = عدم;

  /* Generate our SPN */
  إذا(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  وإلا إذا(host)
    spn = curl_maprintf("%s/%s", service, host);
  وإلا إذا(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  أرجع spn;
}
#وإلا
TCHAR *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                           ثابت حرف *realm)
{
  حرف *utf8_spn = عدم;
  TCHAR *tchar_spn = عدم;

  (فراغ)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  إذا(!utf8_spn)
    أرجع عدم;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  أرجع tchar_spn;
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
bool Curl_auth_user_contains_domain(بنية Curl_creds *creds)
{
  bool valid = FALSE;

  إذا(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    ثابت حرف *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  وإلا
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  أرجع valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(بنية Curl_easy *data)
{
  أرجع Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(بنية Curl_easy *data,
                                 بنية Curl_peer *origin)
{
  أرجع data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
ساكن فراغ ntlm_conn_dtor(ثابت فراغ *ئاچقۇچ, size_t klen, فراغ *entry)
{
  بنية ntlmdata *ntlm = entry;
  (فراغ)ئاچقۇچ;
  (فراغ)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

بنية ntlmdata *Curl_auth_ntlm_get(بنية connectdata *conn, bool proxy)
{
  ثابت حرف *ئاچقۇچ = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  بنية ntlmdata *ntlm = Curl_conn_meta_get(conn, ئاچقۇچ);
  إذا(!ntlm) {
    ntlm = curlx_calloc(1, حجم(*ntlm));
    إذا(!ntlm || Curl_conn_meta_set(conn, ئاچقۇچ, ntlm, ntlm_conn_dtor))
      أرجع عدم;
  }
  أرجع ntlm;
}

فراغ Curl_auth_ntlm_remove(بنية connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
ساكن فراغ krb5_conn_dtor(ثابت فراغ *ئاچقۇچ, size_t klen, فراغ *entry)
{
  بنية kerberos5data *krb5 = entry;
  (فراغ)ئاچقۇچ;
  (فراغ)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

بنية kerberos5data *Curl_auth_krb5_get(بنية connectdata *conn)
{
  بنية kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  إذا(!krb5) {
    krb5 = curlx_calloc(1, حجم(*krb5));
    إذا(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      أرجع عدم;
  }
  أرجع krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
ساكن فراغ gsasl_conn_dtor(ثابت فراغ *ئاچقۇچ, size_t klen, فراغ *entry)
{
  بنية gsasldata *gsasl = entry;
  (فراغ)ئاچقۇچ;
  (فراغ)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

بنية gsasldata *Curl_auth_gsasl_get(بنية connectdata *conn)
{
  بنية gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  إذا(!gsasl) {
    gsasl = curlx_calloc(1, حجم(*gsasl));
    إذا(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      أرجع عدم;
  }
  أرجع gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
ساكن فراغ nego_conn_dtor(ثابت فراغ *ئاچقۇچ, size_t klen, فراغ *entry)
{
  بنية negotiatedata *nego = entry;
  (فراغ)ئاچقۇچ;
  (فراغ)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

بنية negotiatedata *Curl_auth_nego_get(بنية connectdata *conn, bool proxy)
{
  ثابت حرف *ئاچقۇچ = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  بنية negotiatedata *nego = Curl_conn_meta_get(conn, ئاچقۇچ);
  إذا(!nego) {
    nego = curlx_calloc(1, حجم(*nego));
    إذا(!nego || Curl_conn_meta_set(conn, ئاچقۇچ, nego, nego_conn_dtor))
      أرجع عدم;
  }
  أرجع nego;
}
#endif /* USE_SPNEGO */

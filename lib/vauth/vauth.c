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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "creds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"

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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
ਅੱਖਰ *Curl_auth_build_spn(ਅਚਲ ਅੱਖਰ *service, ਅਚਲ ਅੱਖਰ *host,
                          ਅਚਲ ਅੱਖਰ *realm)
{
  ਅੱਖਰ *spn = ਨਿਰਾ;

  /* Generate our SPN */
  ਜੇ(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  ਨਹੀਂਤਾਂ ਜੇ(host)
    spn = curl_maprintf("%s/%s", service, host);
  ਨਹੀਂਤਾਂ ਜੇ(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  ਵਾਪਸ spn;
}
#ਨਹੀਂਤਾਂ
TCHAR *Curl_auth_build_spn(ਅਚਲ ਅੱਖਰ *service, ਅਚਲ ਅੱਖਰ *host,
                           ਅਚਲ ਅੱਖਰ *realm)
{
  ਅੱਖਰ *utf8_spn = ਨਿਰਾ;
  TCHAR *tchar_spn = ਨਿਰਾ;

  (ਖਾਲੀ)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  ਜੇ(!utf8_spn)
    ਵਾਪਸ ਨਿਰਾ;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(utf8_spn);
  curlx_free(utf8_spn);

  ਵਾਪਸ tchar_spn;
}
#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI */

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
bool Curl_auth_user_ਸ਼ਾਮਲ_ਹੈ_domain(ਬਣਤਰ Curl_creds *creds)
{
  bool valid = FALSE;

  ਜੇ(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    ਅਚਲ ਅੱਖਰ *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + ਲੜੀ_ਲੰਬਾਈ(creds->user) - 1));
  }
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ਨਹੀਂਤਾਂ
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#ਜੇ_ਅੰਤ

  ਵਾਪਸ valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_auth_allowed_ਮੰਜ਼ਿਲ_origin(ਅੰਕੜੇ, ਅੰਕੜੇ->state.origin);
}

bool Curl_auth_allowed_ਮੰਜ਼ਿਲ_origin(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ Curl_peer *origin)
{
  ਵਾਪਸ ਅੰਕੜੇ->ਸੈੱਟ.allow_auth_ਮੰਜ਼ਿਲ_other_hosts ||
         Curl_peer_equal(ਅੰਕੜੇ->state.initial_origin, origin);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
ਸਥਿਰ ਖਾਲੀ ntlm_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ ntlmdata *ntlm = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

ਬਣਤਰ ntlmdata *Curl_auth_ntlm_get(ਬਣਤਰ connectdata *conn, bool proxy)
{
  ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  ਬਣਤਰ ntlmdata *ntlm = Curl_conn_meta_get(conn, ਕੁੰਜੀ);
  ਜੇ(!ntlm) {
    ntlm = curlx_calloc(1, ਆਕਾਰ(*ntlm));
    ਜੇ(!ntlm || Curl_conn_meta_ਸੈੱਟ(conn, ਕੁੰਜੀ, ntlm, ntlm_conn_dtor))
      ਵਾਪਸ ਨਿਰਾ;
  }
  ਵਾਪਸ ntlm;
}

ਖਾਲੀ Curl_auth_ntlm_remove(ਬਣਤਰ connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#ਜੇ_ਅੰਤ /* USE_NTLM */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_KERBEROS5
ਸਥਿਰ ਖਾਲੀ krb5_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ kerberos5data *krb5 = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

ਬਣਤਰ kerberos5data *Curl_auth_krb5_get(ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  ਜੇ(!krb5) {
    krb5 = curlx_calloc(1, ਆਕਾਰ(*krb5));
    ਜੇ(!krb5 ||
       Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      ਵਾਪਸ ਨਿਰਾ;
  }
  ਵਾਪਸ krb5;
}
#ਜੇ_ਅੰਤ /* USE_KERBEROS5 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
ਸਥਿਰ ਖਾਲੀ gsasl_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ gsasldata *gsasl = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

ਬਣਤਰ gsasldata *Curl_auth_gsasl_get(ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  ਜੇ(!gsasl) {
    gsasl = curlx_calloc(1, ਆਕਾਰ(*gsasl));
    ਜੇ(!gsasl ||
       Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      ਵਾਪਸ ਨਿਰਾ;
  }
  ਵਾਪਸ gsasl;
}
#ਜੇ_ਅੰਤ /* USE_GSASL */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
ਸਥਿਰ ਖਾਲੀ nego_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ negotiatedata *nego = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

ਬਣਤਰ negotiatedata *Curl_auth_nego_get(ਬਣਤਰ connectdata *conn, bool proxy)
{
  ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  ਬਣਤਰ negotiatedata *nego = Curl_conn_meta_get(conn, ਕੁੰਜੀ);
  ਜੇ(!nego) {
    nego = curlx_calloc(1, ਆਕਾਰ(*nego));
    ਜੇ(!nego || Curl_conn_meta_ਸੈੱਟ(conn, ਕੁੰਜੀ, nego, nego_conn_dtor))
      ਵਾਪਸ ਨਿਰਾ;
  }
  ਵਾਪਸ nego;
}
#ਜੇ_ਅੰਤ /* USE_SPNEGO */

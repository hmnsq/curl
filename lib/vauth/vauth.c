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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "creds.h"
#ଅନ୍ତର୍ଭାଵ "curlx/multibyte.h"
#ଅନ୍ତର୍ଭାଵ "url.h"

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
ଅକ୍ଷର *Curl_auth_build_spn(ଅଚଳ ଅକ୍ଷର *service, ଅଚଳ ଅକ୍ଷର *host,
                          ଅଚଳ ଅକ୍ଷର *realm)
{
  ଅକ୍ଷର *spn = ଖାଲି;

  /* Generate our SPN */
  ଯଦି(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  ଅନ୍ୟଥା ଯଦି(host)
    spn = curl_maprintf("%s/%s", service, host);
  ଅନ୍ୟଥା ଯଦି(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  ଫେରନ୍ତୁ spn;
}
#ଅନ୍ୟଥା
TCHAR *Curl_auth_build_spn(ଅଚଳ ଅକ୍ଷର *service, ଅଚଳ ଅକ୍ଷର *host,
                           ଅଚଳ ଅକ୍ଷର *realm)
{
  ଅକ୍ଷର *utf8_spn = ଖାଲି;
  TCHAR *tchar_spn = ଖାଲି;

  (ଶୂନ୍ୟ)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  ଯଦି(!utf8_spn)
    ଫେରନ୍ତୁ ଖାଲି;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  ଫେରନ୍ତୁ tchar_spn;
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
bool Curl_auth_user_contains_domain(ସଂରଚନା Curl_creds *creds)
{
  bool valid = FALSE;

  ଯଦି(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    ଅଚଳ ଅକ୍ଷର *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#ଯଦି defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ଅନ୍ୟଥା
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  ଫେରନ୍ତୁ valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା Curl_peer *origin)
{
  ଫେରନ୍ତୁ data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
ସ୍ଥିର ଶୂନ୍ୟ ntlm_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା ntlmdata *ntlm = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

ସଂରଚନା ntlmdata *Curl_auth_ntlm_get(ସଂରଚନା connectdata *conn, bool proxy)
{
  ଅଚଳ ଅକ୍ଷର *ଚାବି = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  ସଂରଚନା ntlmdata *ntlm = Curl_conn_meta_get(conn, ଚାବି);
  ଯଦି(!ntlm) {
    ntlm = curlx_calloc(1, ଆକାର(*ntlm));
    ଯଦି(!ntlm || Curl_conn_meta_set(conn, ଚାବି, ntlm, ntlm_conn_dtor))
      ଫେରନ୍ତୁ ଖାଲି;
  }
  ଫେରନ୍ତୁ ntlm;
}

ଶୂନ୍ୟ Curl_auth_ntlm_remove(ସଂରଚନା connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
ସ୍ଥିର ଶୂନ୍ୟ krb5_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା kerberos5data *krb5 = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

ସଂରଚନା kerberos5data *Curl_auth_krb5_get(ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  ଯଦି(!krb5) {
    krb5 = curlx_calloc(1, ଆକାର(*krb5));
    ଯଦି(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      ଫେରନ୍ତୁ ଖାଲି;
  }
  ଫେରନ୍ତୁ krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
ସ୍ଥିର ଶୂନ୍ୟ gsasl_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା gsasldata *gsasl = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

ସଂରଚନା gsasldata *Curl_auth_gsasl_get(ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  ଯଦି(!gsasl) {
    gsasl = curlx_calloc(1, ଆକାର(*gsasl));
    ଯଦି(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      ଫେରନ୍ତୁ ଖାଲି;
  }
  ଫେରନ୍ତୁ gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
ସ୍ଥିର ଶୂନ୍ୟ nego_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା negotiatedata *nego = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

ସଂରଚନା negotiatedata *Curl_auth_nego_get(ସଂରଚନା connectdata *conn, bool proxy)
{
  ଅଚଳ ଅକ୍ଷର *ଚାବି = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  ସଂରଚନା negotiatedata *nego = Curl_conn_meta_get(conn, ଚାବି);
  ଯଦି(!nego) {
    nego = curlx_calloc(1, ଆକାର(*nego));
    ଯଦି(!nego || Curl_conn_meta_set(conn, ଚାବି, nego, nego_conn_dtor))
      ଫେରନ୍ତୁ ଖାଲି;
  }
  ଫେରନ୍ତୁ nego;
}
#endif /* USE_SPNEGO */

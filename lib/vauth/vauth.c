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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "creds.h"
#అన్తర్భావ "curlx/multibyte.h"
#అన్తర్భావ "url.h"

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
#నిర్వచితం_కాకపోతే USE_WINDOWS_SSPI
అక్షరం *Curl_auth_build_spn(స్థిరరాశి అక్షరం *service, స్థిరరాశి అక్షరం *host,
                          స్థిరరాశి అక్షరం *realm)
{
  అక్షరం *spn = ఖాళీ;

  /* Generate our SPN */
  ఒకవేళ(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  లేకపోతే ఒకవేళ(host)
    spn = curl_maprintf("%s/%s", service, host);
  లేకపోతే ఒకవేళ(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  తిరిగి spn;
}
#లేకపోతే
TCHAR *Curl_auth_build_spn(స్థిరరాశి అక్షరం *service, స్థిరరాశి అక్షరం *host,
                           స్థిరరాశి అక్షరం *realm)
{
  అక్షరం *utf8_spn = ఖాళీ;
  TCHAR *tchar_spn = ఖాళీ;

  (శూన్య)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  ఒకవేళ(!utf8_spn)
    తిరిగి ఖాళీ;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_గమ్యం_tchar(utf8_spn);
  curlx_free(utf8_spn);

  తిరిగి tchar_spn;
}
#అయితే_ముగింపు /* USE_WINDOWS_SSPI */

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
bool Curl_auth_user_కలిగియుంది_domain(నిర్మాణం Curl_creds *creds)
{
  bool valid = FALSE;

  ఒకవేళ(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    స్థిరరాశి అక్షరం *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + అక్షరశ్రేణి_పొడవు(creds->user) - 1));
  }
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  లేకపోతే
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#అయితే_ముగింపు

  తిరిగి valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_గమ్యం_host(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_auth_allowed_గమ్యం_origin(దత్తాంశం, దత్తాంశం->state.origin);
}

bool Curl_auth_allowed_గమ్యం_origin(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_peer *origin)
{
  తిరిగి దత్తాంశం->సమితి.allow_auth_గమ్యం_other_hosts ||
         Curl_peer_equal(దత్తాంశం->state.initial_origin, origin);
}

#నిర్వచితమైతే USE_NTLM
స్థిర శూన్య ntlm_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం ntlmdata *ntlm = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

నిర్మాణం ntlmdata *Curl_auth_ntlm_get(నిర్మాణం connectdata *conn, bool proxy)
{
  స్థిరరాశి అక్షరం *కీలకం = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  నిర్మాణం ntlmdata *ntlm = Curl_conn_meta_get(conn, కీలకం);
  ఒకవేళ(!ntlm) {
    ntlm = curlx_calloc(1, పరిమాణం(*ntlm));
    ఒకవేళ(!ntlm || Curl_conn_meta_సమితి(conn, కీలకం, ntlm, ntlm_conn_dtor))
      తిరిగి ఖాళీ;
  }
  తిరిగి ntlm;
}

శూన్య Curl_auth_ntlm_remove(నిర్మాణం connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#అయితే_ముగింపు /* USE_NTLM */

#నిర్వచితమైతే USE_KERBEROS5
స్థిర శూన్య krb5_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం kerberos5data *krb5 = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

నిర్మాణం kerberos5data *Curl_auth_krb5_get(నిర్మాణం connectdata *conn)
{
  నిర్మాణం kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  ఒకవేళ(!krb5) {
    krb5 = curlx_calloc(1, పరిమాణం(*krb5));
    ఒకవేళ(!krb5 ||
       Curl_conn_meta_సమితి(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      తిరిగి ఖాళీ;
  }
  తిరిగి krb5;
}
#అయితే_ముగింపు /* USE_KERBEROS5 */

#నిర్వచితమైతే USE_GSASL
స్థిర శూన్య gsasl_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం gsasldata *gsasl = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

నిర్మాణం gsasldata *Curl_auth_gsasl_get(నిర్మాణం connectdata *conn)
{
  నిర్మాణం gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  ఒకవేళ(!gsasl) {
    gsasl = curlx_calloc(1, పరిమాణం(*gsasl));
    ఒకవేళ(!gsasl ||
       Curl_conn_meta_సమితి(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      తిరిగి ఖాళీ;
  }
  తిరిగి gsasl;
}
#అయితే_ముగింపు /* USE_GSASL */

#నిర్వచితమైతే USE_SPNEGO
స్థిర శూన్య nego_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం negotiatedata *nego = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

నిర్మాణం negotiatedata *Curl_auth_nego_get(నిర్మాణం connectdata *conn, bool proxy)
{
  స్థిరరాశి అక్షరం *కీలకం = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  నిర్మాణం negotiatedata *nego = Curl_conn_meta_get(conn, కీలకం);
  ఒకవేళ(!nego) {
    nego = curlx_calloc(1, పరిమాణం(*nego));
    ఒకవేళ(!nego || Curl_conn_meta_సమితి(conn, కీలకం, nego, nego_conn_dtor))
      తిరిగి ఖాళీ;
  }
  తిరిగి nego;
}
#అయితే_ముగింపు /* USE_SPNEGO */

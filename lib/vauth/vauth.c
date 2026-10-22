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
#اگر_غیر_تعریف_شدہ USE_WINDOWS_SSPI
حرف *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                          ثابت حرف *realm)
{
  حرف *spn = تہی;

  /* Generate our SPN */
  اگر(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  ورنہ اگر(host)
    spn = curl_maprintf("%s/%s", service, host);
  ورنہ اگر(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  واپس spn;
}
#ورنہ
TCHAR *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                           ثابت حرف *realm)
{
  حرف *utf8_spn = تہی;
  TCHAR *tchar_spn = تہی;

  (خالی)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  اگر(!utf8_spn)
    واپس تہی;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_مقصد_tchar(utf8_spn);
  curlx_free(utf8_spn);

  واپس tchar_spn;
}
#اگر_ختم /* USE_WINDOWS_SSPI */

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
bool Curl_auth_user_شامل_ہے_domain(ساخت Curl_creds *creds)
{
  bool valid = FALSE;

  اگر(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    ثابت حرف *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + لڑی_لمبائی(creds->user) - 1));
  }
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ورنہ
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#اگر_ختم

  واپس valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_مقصد_host(ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_auth_allowed_مقصد_origin(ڈیٹا, ڈیٹا->state.origin);
}

bool Curl_auth_allowed_مقصد_origin(ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_peer *origin)
{
  واپس ڈیٹا->سیٹ.allow_auth_مقصد_other_hosts ||
         Curl_peer_equal(ڈیٹا->state.initial_origin, origin);
}

#اگر_تعریف_شدہ USE_NTLM
ساکن خالی ntlm_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت ntlmdata *ntlm = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

ساخت ntlmdata *Curl_auth_ntlm_get(ساخت connectdata *conn, bool proxy)
{
  ثابت حرف *کلید = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  ساخت ntlmdata *ntlm = Curl_conn_meta_get(conn, کلید);
  اگر(!ntlm) {
    ntlm = curlx_calloc(1, حجم(*ntlm));
    اگر(!ntlm || Curl_conn_meta_سیٹ(conn, کلید, ntlm, ntlm_conn_dtor))
      واپس تہی;
  }
  واپس ntlm;
}

خالی Curl_auth_ntlm_remove(ساخت connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#اگر_ختم /* USE_NTLM */

#اگر_تعریف_شدہ USE_KERBEROS5
ساکن خالی krb5_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت kerberos5data *krb5 = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

ساخت kerberos5data *Curl_auth_krb5_get(ساخت connectdata *conn)
{
  ساخت kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  اگر(!krb5) {
    krb5 = curlx_calloc(1, حجم(*krb5));
    اگر(!krb5 ||
       Curl_conn_meta_سیٹ(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      واپس تہی;
  }
  واپس krb5;
}
#اگر_ختم /* USE_KERBEROS5 */

#اگر_تعریف_شدہ USE_GSASL
ساکن خالی gsasl_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت gsasldata *gsasl = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

ساخت gsasldata *Curl_auth_gsasl_get(ساخت connectdata *conn)
{
  ساخت gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  اگر(!gsasl) {
    gsasl = curlx_calloc(1, حجم(*gsasl));
    اگر(!gsasl ||
       Curl_conn_meta_سیٹ(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      واپس تہی;
  }
  واپس gsasl;
}
#اگر_ختم /* USE_GSASL */

#اگر_تعریف_شدہ USE_SPNEGO
ساکن خالی nego_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت negotiatedata *nego = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

ساخت negotiatedata *Curl_auth_nego_get(ساخت connectdata *conn, bool proxy)
{
  ثابت حرف *کلید = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  ساخت negotiatedata *nego = Curl_conn_meta_get(conn, کلید);
  اگر(!nego) {
    nego = curlx_calloc(1, حجم(*nego));
    اگر(!nego || Curl_conn_meta_سیٹ(conn, کلید, nego, nego_conn_dtor))
      واپس تہی;
  }
  واپس nego;
}
#اگر_ختم /* USE_SPNEGO */

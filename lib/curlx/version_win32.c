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

#నిర్వచితమైతే _WIN32

#అన్తర్భావ "curlx/version_win32.h"

#నిర్వచితం_కాకపోతే CURL_WINDOWS_UWP
/* This Unicode version struct works for VerifyVersionInfoW (OSVERSIONINFOEXW)
   and RtlVerifyVersionInfo (RTLOSVERSIONINFOEXW) */
నిర్మాణం OUR_OSVERSIONINFOEXW {
  ULONG  dw_OSVersion_Info_కొలత;
  ULONG  dw_Major_సంస్కరణ;
  ULONG  dw_Minor_సంస్కరణ;
  ULONG  dw_Build_సంఖ్య;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT w_Suite_ముసుగు;
  UCHAR  w_లబ్ధం_Type;
  UCHAR  wReserved;
};

రకనిర్వచనం LONG (APIENTRY *RTLVERIFYVERSIONINFO_FN)
  (నిర్మాణం OUR_OSVERSIONINFOEXW *, ULONG, ULONGLONG);
స్థిర RTLVERIFYVERSIONINFO_FN s_p_Rtl_Verify_సంస్కరణ_Info;

శూన్య curlx_verify_windows_init(శూన్య)
{
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
  s_p_Rtl_Verify_సంస్కరణ_Info = CURLX_FUNCTION_CAST(RTLVERIFYVERSIONINFO_FN,
    Get_Proc_చిరునామా(GetModuleHandle(పాఠ్యం_2("ntdll")), "RtlVerifyVersionInfo"));
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
}
#అయితే_ముగింపు /* !CURL_WINDOWS_UWP */

/*
 * curlx_verify_windows_version()
 *
 * This is used to verify if we are running on a specific Windows version.
 *
 * Parameters:
 *
 * majorVersion [in] - The major version number.
 * minorVersion [in] - The minor version number.
 * buildVersion [in] - The build version number. If 0, this parameter is
 *                     ignored.
 * platform     [in] - The optional platform identifier.
 * condition    [in] - The test condition used to specifier whether we are
 *                     checking a version less than, equal to or greater than
 *                     what is specified in the major and minor version
 *                     numbers.
 *
 * Returns TRUE if matched; otherwise FALSE.
 */
bool curlx_verify_windows_సంస్కరణ(స్థిరరాశి గుర్తులేని పూర్ణాంకం major_సంస్కరణ,
                                  స్థిరరాశి గుర్తులేని పూర్ణాంకం minor_సంస్కరణ,
                                  స్థిరరాశి గుర్తులేని పూర్ణాంకం build_సంస్కరణ,
                                  స్థిరరాశి PlatformIdentifier platform,
                                  స్థిరరాశి సంస్కరణ_Condition condition)
{
  bool matched = FALSE;

#నిర్వచితమైతే CURL_WINDOWS_UWP
  /* We have no way to determine the Windows version from Windows apps,
     so let's assume we are running on the target Windows version. */
  స్థిరరాశి మాట_2 నిండిన_సంస్కరణ = MAKEWORD(minor_సంస్కరణ, major_సంస్కరణ);
  స్థిరరాశి మాట_2 లక్ష్యం_సంస్కరణ = (మాట_2)_WIN32_WINNT;

  (శూన్య)build_సంస్కరణ;

  ఎంపిక(condition) {
  స్థితి సంస్కరణ_LESS_THAN:
    matched = లక్ష్యం_సంస్కరణ < నిండిన_సంస్కరణ;
    ఆపు;

  స్థితి సంస్కరణ_LESS_THAN_EQUAL:
    matched = లక్ష్యం_సంస్కరణ <= నిండిన_సంస్కరణ;
    ఆపు;

  స్థితి సంస్కరణ_EQUAL:
    matched = లక్ష్యం_సంస్కరణ == నిండిన_సంస్కరణ;
    ఆపు;

  స్థితి సంస్కరణ_GREATER_THAN_EQUAL:
    matched = లక్ష్యం_సంస్కరణ >= నిండిన_సంస్కరణ;
    ఆపు;

  స్థితి సంస్కరణ_GREATER_THAN:
    matched = లక్ష్యం_సంస్కరణ > నిండిన_సంస్కరణ;
    ఆపు;
  }

  ఒకవేళ(matched && (platform == PLATFORM_WINDOWS)) {
    /* we are always running on PLATFORM_WINNT */
    matched = FALSE;
  }
#లేకపోతే
  ULONGLONG cm = 0;
  నిర్మాణం OUR_OSVERSIONINFOEXW osver;
  BYTE majorCondition;
  BYTE minorCondition;
  BYTE buildCondition;
  BYTE spMajorCondition;
  BYTE spMinorCondition;
  DWORD dw_Type_ముసుగు = VER_MAJORVERSION | VER_MINORVERSION |
                     VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR;

  ఎంపిక(condition) {
  స్థితి సంస్కరణ_LESS_THAN:
    majorCondition = VER_LESS;
    minorCondition = VER_LESS;
    buildCondition = VER_LESS;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    ఆపు;

  స్థితి సంస్కరణ_LESS_THAN_EQUAL:
    majorCondition = VER_LESS_EQUAL;
    minorCondition = VER_LESS_EQUAL;
    buildCondition = VER_LESS_EQUAL;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    ఆపు;

  స్థితి సంస్కరణ_EQUAL:
    majorCondition = VER_EQUAL;
    minorCondition = VER_EQUAL;
    buildCondition = VER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ఆపు;

  స్థితి సంస్కరణ_GREATER_THAN_EQUAL:
    majorCondition = VER_GREATER_EQUAL;
    minorCondition = VER_GREATER_EQUAL;
    buildCondition = VER_GREATER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ఆపు;

  స్థితి సంస్కరణ_GREATER_THAN:
    majorCondition = VER_GREATER;
    minorCondition = VER_GREATER;
    buildCondition = VER_GREATER;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ఆపు;

  అప్రమేయ:
    తిరిగి FALSE;
  }

  memset(&osver, 0, పరిమాణం(osver));
  osver.dw_OSVersion_Info_కొలత = పరిమాణం(osver);
  osver.dw_Major_సంస్కరణ = major_సంస్కరణ;
  osver.dw_Minor_సంస్కరణ = minor_సంస్కరణ;
  osver.dw_Build_సంఖ్య = build_సంస్కరణ;
  ఒకవేళ(platform == PLATFORM_WINDOWS)
    osver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  లేకపోతే ఒకవేళ(platform == PLATFORM_WINNT)
    osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

  cm = Ver_సమితి_Condition_ముసుగు(cm, VER_MAJORVERSION, majorCondition);
  cm = Ver_సమితి_Condition_ముసుగు(cm, VER_MINORVERSION, minorCondition);
  cm = Ver_సమితి_Condition_ముసుగు(cm, VER_SERVICEPACKMAJOR, spMajorCondition);
  cm = Ver_సమితి_Condition_ముసుగు(cm, VER_SERVICEPACKMINOR, spMinorCondition);

  ఒకవేళ(platform != PLATFORM_DONT_CARE) {
    cm = Ver_సమితి_Condition_ముసుగు(cm, VER_PLATFORMID, VER_EQUAL);
    dw_Type_ముసుగు |= VER_PLATFORMID;
  }

  /* Later versions of Windows have version functions that may not return the
     real version of Windows unless the application is so manifested. We prefer
     the real version always, so we use the Rtl variant of the function when
     possible. Note though the function signatures have underlying fundamental
     types that are the same, the return values are different. */
  ఒకవేళ(s_p_Rtl_Verify_సంస్కరణ_Info)
    matched = !s_p_Rtl_Verify_సంస్కరణ_Info(&osver, dw_Type_ముసుగు, cm);
  లేకపోతే
    matched = !!Verify_సంస్కరణ_Info_W((OSVERSIONINFOEXW *)&osver, dw_Type_ముసుగు, cm);

  /* Compare the build number separately. VerifyVersionInfo normally compares
     major.minor in hierarchical order (eg 1.9 is less than 2.0) but does not
     do the same for build (eg 1.9 build 222 is not less than 2.0 build 111).
     Build comparison is only needed when build numbers are equal (eg 1.9 is
     always less than 2.0 so build comparison is not needed). */
  ఒకవేళ(matched && build_సంస్కరణ &&
     (condition == సంస్కరణ_EQUAL ||
      ((condition == సంస్కరణ_GREATER_THAN_EQUAL ||
        condition == సంస్కరణ_LESS_THAN_EQUAL) &&
        curlx_verify_windows_సంస్కరణ(major_సంస్కరణ, minor_సంస్కరణ, 0,
                                     platform, సంస్కరణ_EQUAL)))) {

    cm = Ver_సమితి_Condition_ముసుగు(0, VER_BUILDNUMBER, buildCondition);
    dw_Type_ముసుగు = VER_BUILDNUMBER;
    ఒకవేళ(s_p_Rtl_Verify_సంస్కరణ_Info)
      matched = !s_p_Rtl_Verify_సంస్కరణ_Info(&osver, dw_Type_ముసుగు, cm);
    లేకపోతే
      matched = !!Verify_సంస్కరణ_Info_W((OSVERSIONINFOEXW *)&osver,
                                     dw_Type_ముసుగు, cm);
  }

#అయితే_ముగింపు

  తిరిగి matched;
}

#అయితే_ముగింపు /* _WIN32 */

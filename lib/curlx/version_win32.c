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

#که_تعریف_شوی _WIN32

#ضمن "curlx/version_win32.h"

#که_نه_تعریف_شوی CURL_WINDOWS_UWP
/* This Unicode version struct works for VerifyVersionInfoW (OSVERSIONINFOEXW)
   and RtlVerifyVersionInfo (RTLOSVERSIONINFOEXW) */
بنية OUR_OSVERSIONINFOEXW {
  ULONG  dw_OSVersion_Info_اندازه;
  ULONG  dw_Major_نسخه;
  ULONG  dw_Minor_نسخه;
  ULONG  dw_Build_شمېره;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT w_Suite_ماسک;
  UCHAR  w_حاصل_ضرب_Type;
  UCHAR  wReserved;
};

تعريفنوع LONG (APIENTRY *RTLVERIFYVERSIONINFO_FN)
  (بنية OUR_OSVERSIONINFOEXW *, ULONG, ULONGLONG);
ساكن RTLVERIFYVERSIONINFO_FN s_p_Rtl_Verify_نسخه_Info;

فراغ curlx_verify_windows_init(فراغ)
{
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wcast-function-type-strict"
#د_که_پای
  s_p_Rtl_Verify_نسخه_Info = CURLX_FUNCTION_CAST(RTLVERIFYVERSIONINFO_FN,
    Get_Proc_پته(GetModuleHandle(متن_2("ntdll")), "RtlVerifyVersionInfo"));
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic راوباسه
#د_که_پای
}
#د_که_پای /* !CURL_WINDOWS_UWP */

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
bool curlx_verify_windows_نسخه(ثابت بینښه صحيح major_نسخه,
                                  ثابت بینښه صحيح minor_نسخه,
                                  ثابت بینښه صحيح build_نسخه,
                                  ثابت PlatformIdentifier platform,
                                  ثابت نسخه_Condition condition)
{
  bool matched = FALSE;

#که_تعریف_شوی CURL_WINDOWS_UWP
  /* We have no way to determine the Windows version from Windows apps,
     so let's assume we are running on the target Windows version. */
  ثابت کلمه_2 ډک_نسخه = MAKEWORD(minor_نسخه, major_نسخه);
  ثابت کلمه_2 موخه_نسخه = (کلمه_2)_WIN32_WINNT;

  (فراغ)build_نسخه;

  بدل(condition) {
  حالة نسخه_LESS_THAN:
    matched = موخه_نسخه < ډک_نسخه;
    اكسر;

  حالة نسخه_LESS_THAN_EQUAL:
    matched = موخه_نسخه <= ډک_نسخه;
    اكسر;

  حالة نسخه_EQUAL:
    matched = موخه_نسخه == ډک_نسخه;
    اكسر;

  حالة نسخه_GREATER_THAN_EQUAL:
    matched = موخه_نسخه >= ډک_نسخه;
    اكسر;

  حالة نسخه_GREATER_THAN:
    matched = موخه_نسخه > ډک_نسخه;
    اكسر;
  }

  إذا(matched && (platform == PLATFORM_WINDOWS)) {
    /* we are always running on PLATFORM_WINNT */
    matched = FALSE;
  }
#وإلا
  ULONGLONG cm = 0;
  بنية OUR_OSVERSIONINFOEXW osver;
  BYTE majorCondition;
  BYTE minorCondition;
  BYTE buildCondition;
  BYTE spMajorCondition;
  BYTE spMinorCondition;
  DWORD dw_Type_ماسک = VER_MAJORVERSION | VER_MINORVERSION |
                     VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR;

  بدل(condition) {
  حالة نسخه_LESS_THAN:
    majorCondition = VER_LESS;
    minorCondition = VER_LESS;
    buildCondition = VER_LESS;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    اكسر;

  حالة نسخه_LESS_THAN_EQUAL:
    majorCondition = VER_LESS_EQUAL;
    minorCondition = VER_LESS_EQUAL;
    buildCondition = VER_LESS_EQUAL;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    اكسر;

  حالة نسخه_EQUAL:
    majorCondition = VER_EQUAL;
    minorCondition = VER_EQUAL;
    buildCondition = VER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    اكسر;

  حالة نسخه_GREATER_THAN_EQUAL:
    majorCondition = VER_GREATER_EQUAL;
    minorCondition = VER_GREATER_EQUAL;
    buildCondition = VER_GREATER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    اكسر;

  حالة نسخه_GREATER_THAN:
    majorCondition = VER_GREATER;
    minorCondition = VER_GREATER;
    buildCondition = VER_GREATER;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    اكسر;

  افتراضي:
    أرجع FALSE;
  }

  memset(&osver, 0, حجم(osver));
  osver.dw_OSVersion_Info_اندازه = حجم(osver);
  osver.dw_Major_نسخه = major_نسخه;
  osver.dw_Minor_نسخه = minor_نسخه;
  osver.dw_Build_شمېره = build_نسخه;
  إذا(platform == PLATFORM_WINDOWS)
    osver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  وإلا إذا(platform == PLATFORM_WINNT)
    osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

  cm = Ver_سېټ_Condition_ماسک(cm, VER_MAJORVERSION, majorCondition);
  cm = Ver_سېټ_Condition_ماسک(cm, VER_MINORVERSION, minorCondition);
  cm = Ver_سېټ_Condition_ماسک(cm, VER_SERVICEPACKMAJOR, spMajorCondition);
  cm = Ver_سېټ_Condition_ماسک(cm, VER_SERVICEPACKMINOR, spMinorCondition);

  إذا(platform != PLATFORM_DONT_CARE) {
    cm = Ver_سېټ_Condition_ماسک(cm, VER_PLATFORMID, VER_EQUAL);
    dw_Type_ماسک |= VER_PLATFORMID;
  }

  /* Later versions of Windows have version functions that may not return the
     real version of Windows unless the application is so manifested. We prefer
     the real version always, so we use the Rtl variant of the function when
     possible. Note though the function signatures have underlying fundamental
     types that are the same, the return values are different. */
  إذا(s_p_Rtl_Verify_نسخه_Info)
    matched = !s_p_Rtl_Verify_نسخه_Info(&osver, dw_Type_ماسک, cm);
  وإلا
    matched = !!Verify_نسخه_Info_W((OSVERSIONINFOEXW *)&osver, dw_Type_ماسک, cm);

  /* Compare the build number separately. VerifyVersionInfo normally compares
     major.minor in hierarchical order (eg 1.9 is less than 2.0) but does not
     do the same for build (eg 1.9 build 222 is not less than 2.0 build 111).
     Build comparison is only needed when build numbers are equal (eg 1.9 is
     always less than 2.0 so build comparison is not needed). */
  إذا(matched && build_نسخه &&
     (condition == نسخه_EQUAL ||
      ((condition == نسخه_GREATER_THAN_EQUAL ||
        condition == نسخه_LESS_THAN_EQUAL) &&
        curlx_verify_windows_نسخه(major_نسخه, minor_نسخه, 0,
                                     platform, نسخه_EQUAL)))) {

    cm = Ver_سېټ_Condition_ماسک(0, VER_BUILDNUMBER, buildCondition);
    dw_Type_ماسک = VER_BUILDNUMBER;
    إذا(s_p_Rtl_Verify_نسخه_Info)
      matched = !s_p_Rtl_Verify_نسخه_Info(&osver, dw_Type_ماسک, cm);
    وإلا
      matched = !!Verify_نسخه_Info_W((OSVERSIONINFOEXW *)&osver,
                                     dw_Type_ماسک, cm);
  }

#د_که_پای

  أرجع matched;
}

#د_که_پای /* _WIN32 */

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

#اگر_تعریف_شدہ _WIN32

#شامل "curlx/version_win32.h"

#اگر_غیر_تعریف_شدہ CURL_WINDOWS_UWP
/* This Unicode version struct works for VerifyVersionInfoW (OSVERSIONINFOEXW)
   and RtlVerifyVersionInfo (RTLOSVERSIONINFOEXW) */
ساخت OUR_OSVERSIONINFOEXW {
  ULONG  dw_OSVersion_Info_سائز;
  ULONG  dw_Major_نسخہ;
  ULONG  dw_Minor_نسخہ;
  ULONG  dw_Build_نمبر;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT w_Suite_نقاب;
  UCHAR  w_حاصل_ضرب_Type;
  UCHAR  wReserved;
};

قسمتعریف LONG (APIENTRY *RTLVERIFYVERSIONINFO_FN)
  (ساخت OUR_OSVERSIONINFOEXW *, ULONG, ULONGLONG);
ساکن RTLVERIFYVERSIONINFO_FN s_p_Rtl_Verify_نسخہ_Info;

خالی curlx_verify_windows_init(خالی)
{
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
  s_p_Rtl_Verify_نسخہ_Info = CURLX_FUNCTION_CAST(RTLVERIFYVERSIONINFO_FN,
    Get_Proc_پتہ(GetModuleHandle(متن_2("ntdll")), "RtlVerifyVersionInfo"));
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
}
#اگر_ختم /* !CURL_WINDOWS_UWP */

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
bool curlx_verify_windows_نسخہ(ثابت غیرعلامتی صحیح major_نسخہ,
                                  ثابت غیرعلامتی صحیح minor_نسخہ,
                                  ثابت غیرعلامتی صحیح build_نسخہ,
                                  ثابت PlatformIdentifier platform,
                                  ثابت نسخہ_Condition condition)
{
  bool matched = FALSE;

#اگر_تعریف_شدہ CURL_WINDOWS_UWP
  /* We have no way to determine the Windows version from Windows apps,
     so let's assume we are running on the target Windows version. */
  ثابت لفظ_2 بھرا_نسخہ = MAKEWORD(minor_نسخہ, major_نسخہ);
  ثابت لفظ_2 ہدف_نسخہ = (لفظ_2)_WIN32_WINNT;

  (خالی)build_نسخہ;

  انتخاب(condition) {
  حالت نسخہ_LESS_THAN:
    matched = ہدف_نسخہ < بھرا_نسخہ;
    توڑو;

  حالت نسخہ_LESS_THAN_EQUAL:
    matched = ہدف_نسخہ <= بھرا_نسخہ;
    توڑو;

  حالت نسخہ_EQUAL:
    matched = ہدف_نسخہ == بھرا_نسخہ;
    توڑو;

  حالت نسخہ_GREATER_THAN_EQUAL:
    matched = ہدف_نسخہ >= بھرا_نسخہ;
    توڑو;

  حالت نسخہ_GREATER_THAN:
    matched = ہدف_نسخہ > بھرا_نسخہ;
    توڑو;
  }

  اگر(matched && (platform == PLATFORM_WINDOWS)) {
    /* we are always running on PLATFORM_WINNT */
    matched = FALSE;
  }
#ورنہ
  ULONGLONG cm = 0;
  ساخت OUR_OSVERSIONINFOEXW osver;
  BYTE majorCondition;
  BYTE minorCondition;
  BYTE buildCondition;
  BYTE spMajorCondition;
  BYTE spMinorCondition;
  DWORD dw_Type_نقاب = VER_MAJORVERSION | VER_MINORVERSION |
                     VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR;

  انتخاب(condition) {
  حالت نسخہ_LESS_THAN:
    majorCondition = VER_LESS;
    minorCondition = VER_LESS;
    buildCondition = VER_LESS;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    توڑو;

  حالت نسخہ_LESS_THAN_EQUAL:
    majorCondition = VER_LESS_EQUAL;
    minorCondition = VER_LESS_EQUAL;
    buildCondition = VER_LESS_EQUAL;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    توڑو;

  حالت نسخہ_EQUAL:
    majorCondition = VER_EQUAL;
    minorCondition = VER_EQUAL;
    buildCondition = VER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    توڑو;

  حالت نسخہ_GREATER_THAN_EQUAL:
    majorCondition = VER_GREATER_EQUAL;
    minorCondition = VER_GREATER_EQUAL;
    buildCondition = VER_GREATER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    توڑو;

  حالت نسخہ_GREATER_THAN:
    majorCondition = VER_GREATER;
    minorCondition = VER_GREATER;
    buildCondition = VER_GREATER;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    توڑو;

  طےشدہ:
    واپس FALSE;
  }

  memset(&osver, 0, حجم(osver));
  osver.dw_OSVersion_Info_سائز = حجم(osver);
  osver.dw_Major_نسخہ = major_نسخہ;
  osver.dw_Minor_نسخہ = minor_نسخہ;
  osver.dw_Build_نمبر = build_نسخہ;
  اگر(platform == PLATFORM_WINDOWS)
    osver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  ورنہ اگر(platform == PLATFORM_WINNT)
    osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

  cm = Ver_سیٹ_Condition_نقاب(cm, VER_MAJORVERSION, majorCondition);
  cm = Ver_سیٹ_Condition_نقاب(cm, VER_MINORVERSION, minorCondition);
  cm = Ver_سیٹ_Condition_نقاب(cm, VER_SERVICEPACKMAJOR, spMajorCondition);
  cm = Ver_سیٹ_Condition_نقاب(cm, VER_SERVICEPACKMINOR, spMinorCondition);

  اگر(platform != PLATFORM_DONT_CARE) {
    cm = Ver_سیٹ_Condition_نقاب(cm, VER_PLATFORMID, VER_EQUAL);
    dw_Type_نقاب |= VER_PLATFORMID;
  }

  /* Later versions of Windows have version functions that may not return the
     real version of Windows unless the application is so manifested. We prefer
     the real version always, so we use the Rtl variant of the function when
     possible. Note though the function signatures have underlying fundamental
     types that are the same, the return values are different. */
  اگر(s_p_Rtl_Verify_نسخہ_Info)
    matched = !s_p_Rtl_Verify_نسخہ_Info(&osver, dw_Type_نقاب, cm);
  ورنہ
    matched = !!Verify_نسخہ_Info_W((OSVERSIONINFOEXW *)&osver, dw_Type_نقاب, cm);

  /* Compare the build number separately. VerifyVersionInfo normally compares
     major.minor in hierarchical order (eg 1.9 is less than 2.0) but does not
     do the same for build (eg 1.9 build 222 is not less than 2.0 build 111).
     Build comparison is only needed when build numbers are equal (eg 1.9 is
     always less than 2.0 so build comparison is not needed). */
  اگر(matched && build_نسخہ &&
     (condition == نسخہ_EQUAL ||
      ((condition == نسخہ_GREATER_THAN_EQUAL ||
        condition == نسخہ_LESS_THAN_EQUAL) &&
        curlx_verify_windows_نسخہ(major_نسخہ, minor_نسخہ, 0,
                                     platform, نسخہ_EQUAL)))) {

    cm = Ver_سیٹ_Condition_نقاب(0, VER_BUILDNUMBER, buildCondition);
    dw_Type_نقاب = VER_BUILDNUMBER;
    اگر(s_p_Rtl_Verify_نسخہ_Info)
      matched = !s_p_Rtl_Verify_نسخہ_Info(&osver, dw_Type_نقاب, cm);
    ورنہ
      matched = !!Verify_نسخہ_Info_W((OSVERSIONINFOEXW *)&osver,
                                     dw_Type_نقاب, cm);
  }

#اگر_ختم

  واپس matched;
}

#اگر_ختم /* _WIN32 */

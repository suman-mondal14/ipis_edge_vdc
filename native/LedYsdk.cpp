#include "iostream"
#include "time.h"
#include "windows.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#include <iostream>
#include <string>
#include <tchar.h>
using namespace Gdiplus;

#include "atlenc.h"
#include <atlstr.h>
#include <sstream>
#include <vector>

using namespace std;
#include "base64.h"

#define _TEXT_CHAR wchar_t
#define _TEXT_T(x) L##x

typedef struct card {
  char ip[20];      // 控制器地址
  char barcode[17]; // 控制器条形码
} card_unit;
typedef struct _CARD_SERVER {
  char barcode[17]; // 控制器条形
} server_card;

// 初始化动态链接库
typedef int(__stdcall *PInitSdk)();

// 释放动态链接库
typedef int(__stdcall *PReleaseSdk)();

typedef unsigned long(__stdcall *Pcreate_playlist)(int w, int h,
                                                   int device_type);
typedef unsigned long(__stdcall *Pcreate_program)(LPCWSTR name,
                                                  LPCWSTR bg_color);
typedef unsigned long(__stdcall *Pcreate_dynamic)();
typedef void(__stdcall *Pdelete_dynamic)(unsigned long dynamic_area);
typedef int(__stdcall *Pclear_dynamic)(char *ip, unsigned short port,
                                       LPCWSTR user_name, LPCWSTR user_pwd);
typedef int(__stdcall *Padd_dynamic_unit)(
    unsigned long dynamic_area, int dynamic_type, int display_effects,
    int display_speed, int stay_time, LPCWSTR file_path, int gif_flag,
    LPCWSTR bg_color, int font_size, LPCWSTR font_name, LPCWSTR font_color,
    LPCWSTR font_attributes, LPCWSTR align_h, LPCWSTR align_v, int volumn,
    int scale_mode, int rolation_mode, LPCWSTR key_list,
    _TEXT_CHAR *proxyService);

typedef int(__stdcall *Padd_dynamic)(unsigned long program,
                                     unsigned long dynamic_area, int dynamic_id,
                                     int x, int y, int w, int h,
                                     LPCWSTR relative_program, int run_mode,
                                     LPCWSTR update_frequency,
                                     int transparency);

typedef int(__stdcall *Preboot)(char *ip, unsigned short port,
                                LPCWSTR user_name, LPCWSTR user_pwd);

typedef int(__stdcall *Padd_program_in_playlist)(
    unsigned long playlist, unsigned long program, int play_mode, int play_time,
    LPCWSTR aging_start_time, LPCWSTR aging_end_time, LPCWSTR period_ontime,
    LPCWSTR period_offtime, int play_week);

typedef int(__stdcall *Pupdate_dynamic)(char *ip, unsigned short port,
                                        LPCWSTR user_name, LPCWSTR user_pwd,
                                        unsigned long playlist,
                                        _TEXT_CHAR *immediately_play,
                                        int conver, int onlyUpdate);

typedef int(__stdcall *Pupdate_dynamic_small)(char *ip, unsigned short port,
                                              LPCWSTR user_name,
                                              LPCWSTR user_pwd,
                                              unsigned long playlist,
                                              LPCWSTR immediately_play,
                                              int conver, int onlyUpdate);

typedef int(__stdcall *Pupdate_dynamic_unit_small)(char *ip,
                                                   unsigned short port,
                                                   LPCWSTR user_name,
                                                   LPCWSTR user_pwd,
                                                   unsigned long playlist);

typedef int(__stdcall *Pcancel_send_program)(unsigned long playlist);
typedef int(__stdcall *Pdelete_playlist)(unsigned long playlist);
typedef int(__stdcall *Pcheck_time)(char *ip, unsigned short port,
                                    LPCWSTR user_name, LPCWSTR user_pwd);
typedef int(__stdcall *Pcreate_pic)();
typedef int(__stdcall *Padd_pic_unit)(unsigned long pic_area, int pic_type,
                                      int display_effects, int display_speed,
                                      int stay_time, LPCWSTR src_path,
                                      LPCWSTR align);

typedef int(__stdcall *Padd_pic)(unsigned long program, unsigned long pic_area,
                                 int x, int y, int w, int h, int volume_mode,
                                 int pic_type, int rotation_mode,
                                 LPCWSTR clone_str, LPCWSTR crop_type);

typedef int(__stdcall *Psend_program)(char *ip, unsigned short port,
                                      LPCWSTR user_name, LPCWSTR user_pwd,
                                      LPCWSTR tmp_path, unsigned long playlist,
                                      int send_style, long long *free_size,
                                      long long *total_size);

typedef int(__stdcall *Pupdate_dynamic_unit)(char *ip, unsigned short port,
                                             LPCWSTR user_name,
                                             LPCWSTR user_pwd,
                                             unsigned long playlist);

typedef unsigned long(__stdcall *Pcreate_time)();
typedef int(__stdcall *Padd_time_unit)(unsigned long time_area, LPCWSTR content,
                                       LPCWSTR font_color, LPCWSTR font_name,
                                       int font_size, int x, int y,
                                       LPCWSTR font_attributes);
typedef int(__stdcall *Padd_time)(unsigned long program,
                                  unsigned long time_area, int x, int y, int w,
                                  int h, int transparency, LPCWSTR bg_color,
                                  LPCWSTR time_equation, LPCWSTR positive_te,
                                  LPCWSTR adjustment);

typedef unsigned long(__stdcall *Pcreate_video)();
typedef int(__stdcall *Padd_video_unit)(unsigned long video_area, int volume,
                                        int scale_mode, int source,
                                        int play_time, LPCWSTR src_path,
                                        LPCWSTR crop_type);
typedef int(__stdcall *Padd_video)(unsigned long program,
                                   unsigned long video_area, int x, int y,
                                   int w, int h, int volume_mode,
                                   int video_type, int ratation_mode,
                                   LPCWSTR clone_str, LPCWSTR crop_type);

typedef int(__stdcall *Plock_screen)(char *ip, unsigned short port,
                                     LPCWSTR user_name, LPCWSTR user_pwd,
                                     int lock);
typedef int(__stdcall *Pset_screen_volumn)(char *ip, unsigned short port,
                                           LPCWSTR user_name, LPCWSTR user_pwd,
                                           int volumn);
typedef int(__stdcall *Pset_screen_brightness)(char *ip, unsigned short port,
                                               LPCWSTR user_name,
                                               LPCWSTR user_pwd,
                                               int brightness);
typedef int(__stdcall *Pset_screen_auto_brightness)(
    char *ip, unsigned short port, LPCWSTR user_name, LPCWSTR user_pwd,
    unsigned short *brightness, int data_count,
    unsigned short *sensor_brightness, int sensor_data_count,
    LPCWSTR sensor_addr);
typedef int(__stdcall *Pset_screen_cus_brightness)(
    char *ip, unsigned short port, LPCWSTR user_name, LPCWSTR user_pwd,
    unsigned short *brightness, int data_count);

typedef int(__stdcall *Pset_screen_turnonoff)(char *ip, unsigned short port,
                                              LPCWSTR user_name,
                                              LPCWSTR user_pwd,
                                              int turnonoff_status);
typedef unsigned long(__stdcall *Pcreate_turnonoff)();
typedef int(__stdcall *Padd_turnonoff)(unsigned long turnonoff, int action,
                                       LPCWSTR time);
typedef int(__stdcall *Pdelete_turnonoff)(unsigned long turnonoff);
typedef int(__stdcall *Pset_screen_cus_turnonoff)(char *ip, unsigned short port,
                                                  LPCWSTR user_name,
                                                  LPCWSTR user_pwd,
                                                  unsigned long turnonoff);
typedef int(__stdcall *Pcancel_screen_cus_turnonoff)(char *ip,
                                                     unsigned short port,
                                                     LPCWSTR user_name,
                                                     LPCWSTR user_pwd);

typedef unsigned long(__stdcall *Pcreate_text)();
typedef int(__stdcall *Padd_text_unit_text)(unsigned long area_tree,
                                            int stay_time, int display_speed,
                                            LPCWSTR font_name, int font_size,
                                            LPCWSTR font_attributes,
                                            LPCWSTR font_alignment,
                                            LPCWSTR font_color,
                                            LPCWSTR bg_color, LPCWSTR content);
typedef int(__stdcall *Padd_text)(unsigned long tree, unsigned long area_tree,
                                  int x, int y, int w, int h, int transparency,
                                  int display_effects, int unit_type);
typedef int(__stdcall *Pget_screen_parameters)(char *ip, unsigned short port,
                                               LPCWSTR user_name,
                                               LPCWSTR user_pwd,
                                               unsigned char *cards);

PInitSdk init_sdk;
PReleaseSdk release_sdk;
Pcreate_playlist create_playlist;
Pcreate_program create_program;
Padd_program_in_playlist add_program_in_playlist;
Psend_program send_program;
Pcancel_send_program cancel_send_program;
Pdelete_playlist delete_playlist;
Pcreate_dynamic create_dynamic;
Pdelete_dynamic delete_dynamic;
Padd_dynamic_unit add_dynamic_unit;
Padd_dynamic add_dynamic;
Pupdate_dynamic update_dynamic;
Pupdate_dynamic_unit update_dynamic_unit;
Pupdate_dynamic_small update_dynamic_small;
Pupdate_dynamic_unit_small update_dynamic_unit_small;
Pclear_dynamic clear_dynamic;
Pcheck_time check_time;
Preboot reboot;
Pcreate_pic create_pic;
Padd_pic_unit add_pic_unit;
Padd_pic add_pic;
Pcreate_time create_time;
Padd_time_unit add_time_unit;
Padd_time add_time;
Pcreate_video create_video;
Padd_video_unit add_video_unit;
Padd_video add_video;
Plock_screen lock_screen;
Pset_screen_volumn set_screen_volumn;
Pset_screen_brightness set_screen_brightness;
Pset_screen_auto_brightness set_screen_auto_brightness;
Pset_screen_cus_brightness set_screen_cus_brightness;
Pset_screen_turnonoff set_screen_turnonoff;
Pcreate_turnonoff create_turnonoff;
Padd_turnonoff add_turnonoff;
Pdelete_turnonoff delete_turnonoff;
Pset_screen_cus_turnonoff set_screen_cus_turnonoff;
Pcancel_screen_cus_turnonoff cancel_screen_cus_turnonoff;
Pcreate_text create_text;
Padd_text_unit_text add_text_unit_text;
Padd_text add_text;
Pget_screen_parameters get_screen_parameters;

// =========================================================
// NEW SDK INITIALIZATION LOGIC
// =========================================================
void InitializeBXSDK() {
  HINSTANCE hdll = LoadLibrary(L"YQNetCom.dll");
  if (hdll == NULL) {
    cout << "ERROR: YQNetCom.dll NOT FOUND in current directory!" << endl;
    return;
  }
  init_sdk = (PInitSdk)GetProcAddress(hdll, "init_sdk");
  release_sdk = (PReleaseSdk)GetProcAddress(hdll, "release_sdk");
  create_playlist = (Pcreate_playlist)GetProcAddress(hdll, "create_playlist");
  create_program = (Pcreate_program)GetProcAddress(hdll, "create_program");
  add_program_in_playlist =
      (Padd_program_in_playlist)GetProcAddress(hdll, "add_program_in_playlist");
  cancel_send_program =
      (Pcancel_send_program)GetProcAddress(hdll, "cancel_send_program");
  delete_playlist = (Pdelete_playlist)GetProcAddress(hdll, "delete_playlist");
  send_program = (Psend_program)GetProcAddress(hdll, "send_program");
  create_dynamic = (Pcreate_dynamic)GetProcAddress(hdll, "create_dynamic");
  delete_dynamic = (Pdelete_dynamic)GetProcAddress(hdll, "delete_dynamic");
  add_dynamic_unit =
      (Padd_dynamic_unit)GetProcAddress(hdll, "add_dynamic_unit");
  add_dynamic = (Padd_dynamic)GetProcAddress(hdll, "add_dynamic");
  update_dynamic = (Pupdate_dynamic)GetProcAddress(hdll, "update_dynamic");
  update_dynamic_unit =
      (Pupdate_dynamic_unit)GetProcAddress(hdll, "update_dynamic_unit");
  update_dynamic_small =
      (Pupdate_dynamic_small)GetProcAddress(hdll, "update_dynamic_small");
  update_dynamic_unit_small = (Pupdate_dynamic_unit_small)GetProcAddress(
      hdll, "update_dynamic_unit_small");
  clear_dynamic = (Pclear_dynamic)GetProcAddress(hdll, "clear_dynamic");
  check_time = (Pcheck_time)GetProcAddress(hdll, "check_time");
  reboot = (Preboot)GetProcAddress(hdll, "reboot");
  create_pic = (Pcreate_pic)GetProcAddress(hdll, "create_pic");
  add_pic_unit = (Padd_pic_unit)GetProcAddress(hdll, "add_pic_unit");
  add_pic = (Padd_pic)GetProcAddress(hdll, "add_pic");
  create_time = (Pcreate_time)GetProcAddress(hdll, "create_time");
  add_time_unit = (Padd_time_unit)GetProcAddress(hdll, "add_time_unit");
  add_time = (Padd_time)GetProcAddress(hdll, "add_time");
  create_video = (Pcreate_video)GetProcAddress(hdll, "create_video");
  add_video_unit = (Padd_video_unit)GetProcAddress(hdll, "add_video_unit");
  add_video = (Padd_video)GetProcAddress(hdll, "add_video");
  lock_screen = (Plock_screen)GetProcAddress(hdll, "lock_screen");
  set_screen_volumn =
      (Pset_screen_volumn)GetProcAddress(hdll, "set_screen_volumn");
  set_screen_brightness =
      (Pset_screen_brightness)GetProcAddress(hdll, "set_screen_brightness");
  set_screen_auto_brightness = (Pset_screen_auto_brightness)GetProcAddress(
      hdll, "set_screen_auto_brightness");
  set_screen_cus_brightness = (Pset_screen_cus_brightness)GetProcAddress(
      hdll, "set_screen_cus_brightness");
  set_screen_turnonoff =
      (Pset_screen_turnonoff)GetProcAddress(hdll, "set_screen_turnonoff");
  create_turnonoff =
      (Pcreate_turnonoff)GetProcAddress(hdll, "create_turnonoff");
  add_turnonoff = (Padd_turnonoff)GetProcAddress(hdll, "add_turnonoff");
  delete_turnonoff =
      (Pdelete_turnonoff)GetProcAddress(hdll, "delete_turnonoff");
  set_screen_cus_turnonoff = (Pset_screen_cus_turnonoff)GetProcAddress(
      hdll, "set_screen_cus_turnonoff");
  cancel_screen_cus_turnonoff = (Pcancel_screen_cus_turnonoff)GetProcAddress(
      hdll, "cancel_screen_cus_turnonoff");
  create_text = (Pcreate_text)GetProcAddress(hdll, "create_text");
  add_text_unit_text =
      (Padd_text_unit_text)GetProcAddress(hdll, "add_text_unit_text");
  add_text = (Padd_text)GetProcAddress(hdll, "add_text");
  get_screen_parameters =
      (Pget_screen_parameters)GetProcAddress(hdll, "get_screen_parameters");

  if (init_sdk)
    init_sdk();
}

void ReleaseBXSDK() {
  if (release_sdk)
    release_sdk();
}

LPCWSTR stringToLPCWSTR_Safe(const std::string &orig) {
  int len = MultiByteToWideChar(CP_ACP, 0, orig.c_str(), -1, NULL, 0);
  wchar_t *wcstring = new wchar_t[len];
  MultiByteToWideChar(CP_ACP, 0, orig.c_str(), -1, wcstring, len);
  return wcstring;
}

std::string string_To_UTF8(const std::string &str) {
  int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
  wchar_t *pwBuf = new wchar_t[nwLen + 1];
  ZeroMemory(pwBuf, nwLen * 2 + 2);
  ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
  int nLen =
      ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
  char *pBuf = new char[nLen + 1];
  ZeroMemory(pBuf, nLen + 1);
  ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
  std::string retStr(pBuf);
  delete[] pwBuf;
  delete[] pBuf;
  return retStr;
}

LPCWSTR stringToLPCWSTR(std::string orig) {
  size_t origsize = orig.length() + 1;
  const size_t newsize = 100;
  size_t convertedChars = 0;
  wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t) * (orig.length() - 1));
  mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
  return wcstring;
}

// =========================================================
// FIXED: Sends custom text to display safely (OLD COMMENTED BLOCK)
// =========================================================
// void Program_dynamic_small(char *ip, int port, LPCWSTR messageText) {
//   unsigned long playlist = create_playlist(64, 32, 8536);
//   unsigned long program = create_program(L"program_1",
//   _TEXT_T("0xff000000"));

//   int dynamic_type = 1;
//   int display_effects = 52;
//   int display_speed = 10;
//   int stay_time = 0;
//   int gif_flag = 0;
//   LPCWSTR bg_color = L"0xff000000";
//   LPCWSTR color = L"0xffff0000";
//   LPCWSTR font_attributes = L"normal";
//   LPCWSTR font = L"SimSun";
//   LPCWSTR align_h = L"0";
//   LPCWSTR align_v = L"0";

//   // Convert wide string to UTF-8 then Base64 encode it
//   int utf8Len =
//       WideCharToMultiByte(CP_UTF8, 0, messageText, -1, NULL, 0, NULL, NULL);
//   char *utf8Str = new char[utf8Len];
//   WideCharToMultiByte(CP_UTF8, 0, messageText, -1, utf8Str, utf8Len, NULL,
//                       NULL);

//   std::string encoded = base64_encode(
//       reinterpret_cast<const unsigned char *>(utf8Str), utf8Len - 1);
//   delete[] utf8Str;

//   LPCWSTR wcstring = stringToLPCWSTR_Safe(encoded);

//   unsigned long dynamic_area = create_dynamic();
//   int err = add_dynamic_unit(
//       dynamic_area, dynamic_type, display_effects, display_speed, stay_time,
//       wcstring, gif_flag, bg_color, 12, font, color, font_attributes,
//       align_h, align_v, 0, 0, 0, _TEXT_T(""), _TEXT_T(""));

//   delete[] wcstring;

//   err = add_dynamic(program, dynamic_area, 0, 0, 0, 32, 32, _T(""), 0,
//   _T(""),
//                     100);
//   delete_dynamic(dynamic_area);

//   err = add_program_in_playlist(playlist, program, 1, 10, _T(""), _T(""),
//                                 _T(""), _T(""), 127);

//   // Hardcoded the board's default username and password "guest"
//   err = update_dynamic_small(ip, port, L"guest", L"guest", playlist, _T(""),
//   1,
//                              0);

//   cancel_send_program(playlist);
//   delete_playlist(playlist);
// }

// void Program_dynamic_small(char *ip, int port, LPCWSTR messageText,
//                            LPCWSTR customColor, int customEffect) {
//   std::cout << "[SDK] Creating playlist (Width: 48, Height: 16, Type:
//   8536)..."
//             << std::endl;

//   unsigned long playlist = create_playlist(48, 16, 8536);
//   unsigned long program = create_program(L"program_1",
//   _TEXT_T("0xff000000"));

//   int dynamic_type = 1;
//   int display_effects = customEffect; // Uses dynamic effect from user
//   int display_speed = 10;
//   int stay_time = 0;
//   int gif_flag = 0;
//   LPCWSTR bg_color = L"0xff000000";
//   LPCWSTR font_attributes = L"normal";
//   LPCWSTR font = L"SimSun";
//   LPCWSTR align_h = L"0";
//   LPCWSTR align_v = L"0";

//   int utf8Len =
//       WideCharToMultiByte(CP_UTF8, 0, messageText, -1, NULL, 0, NULL, NULL);
//   char *utf8Str = new char[utf8Len];
//   WideCharToMultiByte(CP_UTF8, 0, messageText, -1, utf8Str, utf8Len, NULL,
//                       NULL);

//   std::string encoded = base64_encode(
//       reinterpret_cast<const unsigned char *>(utf8Str), utf8Len - 1);
//   delete[] utf8Str;

//   LPCWSTR wcstring = stringToLPCWSTR_Safe(encoded);

//   unsigned long dynamic_area = create_dynamic();
//   int err1 = add_dynamic_unit(
//       dynamic_area, dynamic_type, display_effects, display_speed, stay_time,
//       wcstring, gif_flag, bg_color, 12, font, customColor, font_attributes,
//       align_h, align_v, 0, 0, 0, _TEXT_T(""), _TEXT_T(""));

//   delete[] wcstring;

//   int err2 = add_dynamic(program, dynamic_area, 0, 0, 0, 48, 16, _T(""), 0,
//                          _T(""), 100);

//   delete_dynamic(dynamic_area);

//   int err3 = add_program_in_playlist(playlist, program, 1, 10, _T(""),
//   _T(""),
//                                      _T(""), _T(""), 127);

//   std::cout << "[SDK] Sending to board..." << std::endl;
//   int err4 = update_dynamic_small(ip, port, L"guest", L"guest", playlist,
//                                   _T(""), 1, 0);

//   if (err4 == 0) {
//     std::cout << "[SDK] update_dynamic_small SUCCESS (Returned 0)" <<
//     std::endl;
//   } else {
//     std::cout << "[SDK] update_dynamic_small FAILED with Error Code: " <<
//     err4
//               << std::endl;
//   }

//   cancel_send_program(playlist);
//   delete_playlist(playlist);
// }

// 144x32 TOPGRIP FALLBACK BITMAP GENERATOR
// =========================================================
// =========================================================
// RDSO SPN 108 Table 1.4 Dynamic Font Selection Helper Engine
// =========================================================
#include <algorithm>
#include <cwctype>

std::wstring getFontForLanguageW(const std::wstring &langCodeOrText) {
  if (!langCodeOrText.empty()) {
    std::wstring lowerInput = langCodeOrText;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::towlower);

    // 1. Explicit Language Code / Name Matching (RDSO SPN 108 Table 1.4 Rules)
    if (lowerInput == L"en" || lowerInput == L"english" || lowerInput == L"eng") {
      return L"Arial";
    }
    if (lowerInput == L"hi" || lowerInput == L"hindi" || lowerInput == L"hin" ||
        lowerInput == L"bodo" || lowerInput == L"dogri" || lowerInput == L"konkani" ||
        lowerInput == L"maithili" || lowerInput == L"marathi" || lowerInput == L"mr") {
      return L"Kokila";
    }
    if (lowerInput == L"bn" || lowerInput == L"bengali" || lowerInput == L"ben" ||
        lowerInput == L"regional" || lowerInput == L"assamese" || lowerInput == L"as" ||
        lowerInput == L"manipuri" || lowerInput == L"mni" || lowerInput == L"ori" ||
        lowerInput == L"oriya" || lowerInput == L"or" || lowerInput == L"od" || lowerInput == L"odia") {
      return L"Shonar Bangla";
    }
    if (lowerInput == L"gu" || lowerInput == L"gujarati" || lowerInput == L"kn" ||
        lowerInput == L"kannada" || lowerInput == L"ks" || lowerInput == L"kashmiri" ||
        lowerInput == L"ml" || lowerInput == L"malayalam" || lowerInput == L"ta" ||
        lowerInput == L"tamil" || lowerInput == L"urdu") {
      return L"Nirmala UI";
    }
    if (lowerInput == L"pa" || lowerInput == L"punjabi" || lowerInput == L"pun" ||
        lowerInput == L"gurmukhi") {
      return L"Gurmukhi";
    }
    if (lowerInput == L"te" || lowerInput == L"telugu" || lowerInput == L"tel") {
      return L"Vani";
    }

    // 2. Unicode Character Range Auto-Detection
    for (wchar_t ch : langCodeOrText) {
      // Devanagari (Hindi, Bodo, Dogri, Konkani, Maithili, Marathi): U+0900..U+097F
      if (ch >= 0x0900 && ch <= 0x097F) {
        return L"Kokila";
      }
      // Bengali / Assamese / Manipuri (U+0980..U+09FF) & Oriya (U+0B00..U+0B7F)
      if ((ch >= 0x0980 && ch <= 0x09FF) || (ch >= 0x0B00 && ch <= 0x0B7F)) {
        return L"Shonar Bangla";
      }
      // Gurmukhi (Punjabi): U+0A00..U+0A7F
      if (ch >= 0x0A00 && ch <= 0x0A7F) {
        return L"Gurmukhi";
      }
      // Telugu: U+0C00..U+0C7F
      if (ch >= 0x0C00 && ch <= 0x0C7F) {
        return L"Vani";
      }
      // Gujarati (U+0A80..U+0AFF), Tamil (U+0B80..U+0BFF), Kannada (U+0C80..U+0CFF), Malayalam (U+0D00..U+0D7F)
      if ((ch >= 0x0A80 && ch <= 0x0AFF) || (ch >= 0x0B80 && ch <= 0x0BFF) ||
          (ch >= 0x0C80 && ch <= 0x0CFF) || (ch >= 0x0D00 && ch <= 0x0D7F)) {
        return L"Nirmala UI";
      }
    }
  }

  // Default font for English / Latin script
  return L"Arial";
}

std::string getFontForLanguage(const std::string &langCode) {
  std::wstring wCode(langCode.begin(), langCode.end());
  std::wstring wFont = getFontForLanguageW(wCode);
  if (wFont.empty()) return "Arial";
  int len = WideCharToMultiByte(CP_UTF8, 0, wFont.c_str(), -1, NULL, 0, NULL, NULL);
  if (len <= 0) return "Arial";
  std::vector<char> buf(len);
  WideCharToMultiByte(CP_UTF8, 0, wFont.c_str(), -1, buf.data(), len, NULL, NULL);
  return std::string(buf.data());
}

// 144x32 DYNAMIC DEFAULT MESSAGE BITMAP GENERATOR (GDI)
bool GenerateDefaultMessageBMP(LPCWSTR msgText, const wchar_t *outFilePath, LPCWSTR langCode = L"en") {
  int width = 192;
  int height = 112;

  HDC hdcMem = CreateCompatibleDC(NULL);
  if (!hdcMem)
    return false;

  BITMAPINFO bmi = {0};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = width;
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 24;
  bmi.bmiHeader.biCompression = BI_RGB;

  void *pBits = nullptr;
  HBITMAP hbmp =
      CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
  if (!hbmp) {
    DeleteDC(hdcMem);
    return false;
  }

  HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hbmp);

  // Background Black
  RECT bgRect = {0, 0, width, height};
  HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
  FillRect(hdcMem, &bgRect, hBlackBrush);
  DeleteObject(hBlackBrush);

  // Draw Yellow Border Box
  HPEN hYellowPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
  HPEN hOldPen = (HPEN)SelectObject(hdcMem, hYellowPen);
  MoveToEx(hdcMem, 0, 0, NULL);
  LineTo(hdcMem, width - 1, 0);
  LineTo(hdcMem, width - 1, height - 1);
  LineTo(hdcMem, 0, height - 1);
  LineTo(hdcMem, 0, 0);
  SelectObject(hdcMem, hOldPen);
  DeleteObject(hYellowPen);

  SetTextColor(hdcMem, RGB(255, 255, 0));
  SetBkMode(hdcMem, TRANSPARENT);

  LPCWSTR displayStr = (msgText && wcslen(msgText) > 0) ? msgText : L"TOPGRIP";
  int textLen = (int)wcslen(displayStr);
  int cellW = width - 4;
  UINT drawFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;

  // Resolve dynamic font per RDSO Table 1.4 rules
  std::wstring mappedFont = getFontForLanguageW(langCode && wcslen(langCode) > 0 ? langCode : displayStr);

  // Auto-scale default message font height so long text fits inside 192px box
  for (int fSize = 28; fSize >= 10; fSize--) {
    HFONT hFont =
        CreateFontW(fSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, mappedFont.c_str());
    HFONT hOldF = (HFONT)SelectObject(hdcMem, hFont);
    SIZE textSZ;
    GetTextExtentPoint32W(hdcMem, displayStr, textLen, &textSZ);
    if (textSZ.cx <= cellW - 2 || fSize == 10) {
      RECT textRect = {2, 2, width - 2, height - 2};
      DrawTextW(hdcMem, displayStr, -1, &textRect, drawFlags);
      SelectObject(hdcMem, hOldF);
      DeleteObject(hFont);
      break;
    }
    SelectObject(hdcMem, hOldF);
    DeleteObject(hFont);
  }

  BITMAPFILEHEADER bfh = {0};
  bfh.bfType = 0x4D42; // "BM"
  bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  bfh.bfSize = bfh.bfOffBits + (width * height * 3);

  HANDLE hFile = CreateFileW(outFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hbmp);
    DeleteDC(hdcMem);
    return false;
  }

  DWORD written = 0;
  WriteFile(hFile, &bfh, sizeof(bfh), &written, NULL);
  WriteFile(hFile, &bmi.bmiHeader, sizeof(bmi.bmiHeader), &written, NULL);
  WriteFile(hFile, pBits, width * height * 3, &written, NULL);
  CloseHandle(hFile);

  SelectObject(hdcMem, hOldBmp);
  DeleteObject(hbmp);
  DeleteDC(hdcMem);
  return true;
}

// Helper to parse hex string like "FF0000", "00FFFF", "0000FF", "#00FF00",
// "green", "yellow", "cyan", "red", "white", "magenta", "blue"
COLORREF ParseHexColorStr(const wchar_t *hexStr, COLORREF fallback) {
  if (!hexStr || wcslen(hexStr) == 0)
    return fallback;
  if (_wcsicmp(hexStr, L"green") == 0)
    return RGB(0, 255, 0);
  if (_wcsicmp(hexStr, L"yellow") == 0)
    return RGB(255, 255, 0);
  if (_wcsicmp(hexStr, L"cyan") == 0)
    return RGB(0, 255, 255);
  if (_wcsicmp(hexStr, L"red") == 0)
    return RGB(255, 0, 0);
  if (_wcsicmp(hexStr, L"white") == 0)
    return RGB(255, 255, 255);
  if (_wcsicmp(hexStr, L"magenta") == 0)
    return RGB(255, 0, 255);
  if (_wcsicmp(hexStr, L"blue") == 0)
    return RGB(0, 128, 255);

  const wchar_t *p = hexStr;
  while (*p == L' ' || *p == L'#')
    p++;
  if (wcsncmp(p, L"0x", 2) == 0 || wcsncmp(p, L"0X", 2) == 0)
    p += 2;
  if (wcslen(p) >= 8)
    p += 2; // skip alpha byte if 0xFF00FF00

  unsigned int r = 255, g = 255, b = 0;
  if (swscanf_s(p, L"%02x%02x%02x", &r, &g, &b) == 3) {
    if (r < 50 && g < 50 && b > 150) {
      // High-visibility bright blue for LED matrix display hardware
      return RGB(0, 128, 255);
    }
    return RGB(r, g, b);
  }
  return fallback;
}

void ParseRowColors(const std::wstring &colorStr, COLORREF &col1,
                    COLORREF &col2, COLORREF &col3, COLORREF &col4,
                    COLORREF &col5, COLORREF &colStatus) {
  std::vector<std::wstring> tokens;
  std::wstringstream wss(colorStr);
  std::wstring item;
  while (std::getline(wss, item, L',')) {
    tokens.push_back(item);
  }
  if (tokens.size() >= 7) {
    // 7-field database/API format: [train_no, train_name, ad, sch_arr, sch_dep, pf_no, status]
    col1 = ParseHexColorStr(tokens[0].c_str(), col1); // Train No
    col2 = ParseHexColorStr(tokens[1].c_str(), col2); // Train Name
    col3 = ParseHexColorStr(tokens[3].c_str(), col3); // Exp Arr Time
    col4 = ParseHexColorStr(tokens[4].c_str(), col4); // Exp Dep Time
    col5 = ParseHexColorStr(tokens[5].c_str(), col5); // PF No
    colStatus = ParseHexColorStr(tokens[6].c_str(), colStatus); // Status Color from Settings/API
  } else if (tokens.size() >= 6) {
    col1 = ParseHexColorStr(tokens[0].c_str(), col1);
    col2 = ParseHexColorStr(tokens[1].c_str(), col2);
    col3 = ParseHexColorStr(tokens[2].c_str(), col3);
    col4 = ParseHexColorStr(tokens[3].c_str(), col4);
    col5 = ParseHexColorStr(tokens[4].c_str(), col5);
    colStatus = ParseHexColorStr(tokens[5].c_str(), colStatus);
  } else if (tokens.size() >= 5) {
    col1 = ParseHexColorStr(tokens[0].c_str(), col1);
    col2 = ParseHexColorStr(tokens[1].c_str(), col2);
    col3 = ParseHexColorStr(tokens[2].c_str(), col3);
    col4 = ParseHexColorStr(tokens[3].c_str(), col4);
    col5 = ParseHexColorStr(tokens[4].c_str(), col5);
  } else if (tokens.size() == 1) {
    COLORREF singleCol = ParseHexColorStr(tokens[0].c_str(), RGB(255, 255, 0));
    col1 = col2 = col3 = col4 = col5 = colStatus = singleCol;
  }
}

int MapChrToFontHeight(const wchar_t* chrHexStr) {
  if (!chrHexStr) return 12;
  std::wstring hexWStr(chrHexStr);
  if (hexWStr == L"0x00") return 12;
  if (hexWStr == L"0x01") return 16;
  if (hexWStr == L"0x02") return 20;
  if (hexWStr == L"0x03") return 24;
  if (hexWStr == L"0x04") return 28;
  if (hexWStr == L"0x05") return 32;
  return 12;
}

LPCWSTR GetCleanStatusText(LPCWSTR statusStr) {
  if (!statusStr) return L"";
  const wchar_t* hashPos = wcschr(statusStr, L'#');
  if (hashPos) {
    return hashPos + 1;
  }
  return statusStr;
}

bool IsCancelledStatus(LPCWSTR statusStr) {
  if (!statusStr || wcslen(statusStr) == 0) return false;
  std::wstring s(statusStr);
  return (s.find(L"0x0B") != std::wstring::npos ||
          s.find(L"0X0B") != std::wstring::npos ||
          s == L"11" ||
          s.find(L"CANCEL") != std::wstring::npos ||
          s.find(L"cancel") != std::wstring::npos ||
          s.find(L"Cancel") != std::wstring::npos ||
          s.find(L"निरस्त") != std::wstring::npos ||
          s.find(L"रद्द") != std::wstring::npos ||
          s.find(L"বাতিল") != std::wstring::npos);
}

// =========================================================
// 192x112 7-ROW TABLE BITMAP GENERATOR (GDI - UP TO 7 TRAINS SIMULTANEOUSLY)
// =========================================================
bool GenerateTableBMP7Rows(
    LPCWSTR t1No, LPCWSTR t1Name, LPCWSTR t1Eat, LPCWSTR t1Edt, LPCWSTR t1Pf, LPCWSTR t1Sta,
    LPCWSTR t2No, LPCWSTR t2Name, LPCWSTR t2Eat, LPCWSTR t2Edt, LPCWSTR t2Pf, LPCWSTR t2Sta,
    LPCWSTR t3No, LPCWSTR t3Name, LPCWSTR t3Eat, LPCWSTR t3Edt, LPCWSTR t3Pf, LPCWSTR t3Sta,
    LPCWSTR t4No, LPCWSTR t4Name, LPCWSTR t4Eat, LPCWSTR t4Edt, LPCWSTR t4Pf, LPCWSTR t4Sta,
    LPCWSTR t5No, LPCWSTR t5Name, LPCWSTR t5Eat, LPCWSTR t5Edt, LPCWSTR t5Pf, LPCWSTR t5Sta,
    LPCWSTR t6No, LPCWSTR t6Name, LPCWSTR t6Eat, LPCWSTR t6Edt, LPCWSTR t6Pf, LPCWSTR t6Sta,
    LPCWSTR t7No, LPCWSTR t7Name, LPCWSTR t7Eat, LPCWSTR t7Edt, LPCWSTR t7Pf, LPCWSTR t7Sta,
    LPCWSTR customColorStr, LPCWSTR borderColorHex, LPCWSTR chrHexStr,
    const wchar_t *outFilePath, LPCWSTR langCode = L"en") {
  int width = 192;
  int height = 112;

  HDC hdcMem = CreateCompatibleDC(NULL);
  if (!hdcMem)
    return false;

  BITMAPINFO bmi = {0};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = width;
  bmi.bmiHeader.biHeight = -height; // Top-down DIB
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 24;
  bmi.bmiHeader.biCompression = BI_RGB;

  void *pBits = nullptr;
  HBITMAP hbmp =
      CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
  if (!hbmp) {
    DeleteDC(hdcMem);
    return false;
  }

  HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hbmp);

  // 1. Background Black
  RECT bgRect = {0, 0, width, height};
  HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
  FillRect(hdcMem, &bgRect, hBlackBrush);
  DeleteObject(hBlackBrush);

  // 2. Draw Table Borders with dynamic Line/Border Color (BRC)
  COLORREF borderPenColor = RGB(255, 255, 0); // Yellow default
  if (borderColorHex && wcslen(borderColorHex) >= 6) {
    unsigned int r = 255, g = 255, b = 0;
    const wchar_t *hexPtr = borderColorHex;
    if (wcsncmp(hexPtr, L"0x", 2) == 0 || wcsncmp(hexPtr, L"0X", 2) == 0)
      hexPtr += 2;
    if (hexPtr[0] == L'#')
      hexPtr++;
    if (swscanf_s(hexPtr, L"%02x%02x%02x", &r, &g, &b) == 3) {
      borderPenColor = RGB(r, g, b);
    }
  }

  HPEN hYellowPen = CreatePen(PS_SOLID, 1, borderPenColor);
  HPEN hOldPen = (HPEN)SelectObject(hdcMem, hYellowPen);

  // Outer Box Border
  MoveToEx(hdcMem, 0, 0, NULL);
  LineTo(hdcMem, width - 1, 0);
  LineTo(hdcMem, width - 1, height - 1);
  LineTo(hdcMem, 0, height - 1);
  LineTo(hdcMem, 0, 0);

  // Horizontal Divider Lines for 7 rows (each 16px high: 16*7 = 112): Y=15, 31, 47, 63, 79, 95
  for (int yLine = 15; yLine < 112 - 1; yLine += 16) {
    MoveToEx(hdcMem, 0, yLine, NULL);
    LineTo(hdcMem, width - 1, yLine);
  }

  // Vertical Column Dividers across full 112px height:
  MoveToEx(hdcMem, 28, 0, NULL);
  LineTo(hdcMem, 28, height - 1);

  MoveToEx(hdcMem, 124, 0, NULL);
  LineTo(hdcMem, 124, height - 1);

  struct TrainRowData {
    LPCWSTR no, name, eat, edt, pf, sta;
  } trainRows[7] = {
      {t1No, t1Name, t1Eat, t1Edt, t1Pf, t1Sta}, {t2No, t2Name, t2Eat, t2Edt, t2Pf, t2Sta},
      {t3No, t3Name, t3Eat, t3Edt, t3Pf, t3Sta}, {t4No, t4Name, t4Eat, t4Edt, t4Pf, t4Sta},
      {t5No, t5Name, t5Eat, t5Edt, t5Pf, t5Sta}, {t6No, t6Name, t6Eat, t6Edt, t6Pf, t6Sta},
      {t7No, t7Name, t7Eat, t7Edt, t7Pf, t7Sta}};

  // Render vertical column dividers at X=150 and X=176 for normal (non-cancelled) rows
  for (int r = 0; r < 7; r++) {
    if (!IsCancelledStatus(trainRows[r].sta)) {
      int yT = r * 16;
      int yB = (r + 1) * 16 - 1;
      MoveToEx(hdcMem, 150, yT, NULL);
      LineTo(hdcMem, 150, yB);
      MoveToEx(hdcMem, 176, yT, NULL);
      LineTo(hdcMem, 176, yB);
    }
  }

  SelectObject(hdcMem, hOldPen);
  DeleteObject(hYellowPen);

  // Default Column Colors
  COLORREF defC1 = RGB(0, 255, 0);   // Train No (Green)
  COLORREF defC2 = RGB(0, 255, 255); // Train Name (Cyan)
  COLORREF defC3 = RGB(0, 128, 255); // Exp Arr Time (Bright Blue)
  COLORREF defC4 = RGB(0, 128, 255); // Exp Dep Time (Bright Blue)
  COLORREF defC5 = RGB(255, 0, 0);   // PF No (Red)
  COLORREF defC6 = RGB(255, 0, 0);   // Status Color (Red default, or from settings)

  COLORREF rCols[7][6];
  for (int r = 0; r < 7; r++) {
    rCols[r][0] = defC1;
    rCols[r][1] = defC2;
    rCols[r][2] = defC3;
    rCols[r][3] = defC4;
    rCols[r][4] = defC5;
    rCols[r][5] = defC6;
  }

  if (customColorStr && wcslen(customColorStr) > 0) {
    std::wstring cStr = customColorStr;
    std::vector<std::wstring> rowColorTokens;
    std::wstringstream wss(cStr);
    std::wstring token;
    while (std::getline(wss, token, L'~')) {
      rowColorTokens.push_back(token);
    }
    for (size_t r = 0; r < 7; r++) {
      std::wstring rowColorStr =
          (r < rowColorTokens.size())
              ? rowColorTokens[r]
              : (rowColorTokens.empty() ? L"" : rowColorTokens[0]);
      if (!rowColorStr.empty()) {
        ParseRowColors(rowColorStr, rCols[r][0], rCols[r][1], rCols[r][2],
                       rCols[r][3], rCols[r][4], rCols[r][5]);
      }
    }
  }

  SetBkMode(hdcMem, TRANSPARENT);
  UINT drawFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;

  // Dynamic font height resolution based on CHR parameter
  int baseFontHeight = MapChrToFontHeight(chrHexStr);
  int tableFontHeight = (baseFontHeight > 14) ? 14 : baseFontHeight;

  // Auto-scale font lambda for 16px row height with strict GDI clipping
  auto drawScaledText = [&](RECT cellRect, LPCWSTR text, int maxFontHeight,
                            COLORREF textColor) {
    if (!text || wcslen(text) == 0)
      return;
    SetTextColor(hdcMem, textColor);
    int cellW = cellRect.right - cellRect.left;
    HRGN hRgn = CreateRectRgn(cellRect.left, cellRect.top, cellRect.right,
                              cellRect.bottom);
    SelectClipRgn(hdcMem, hRgn);

    // Resolve font per RDSO Table 1.4 rules (Language Code or Unicode Text Auto-Detection)
    std::wstring fontNameStr = getFontForLanguageW(langCode && wcslen(langCode) > 0 ? langCode : text);

    int startFont = (maxFontHeight > 14) ? 14 : maxFontHeight;
    for (int fSize = startFont; fSize >= 6; fSize--) {
      HFONT hFont = CreateFontW(fSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, fontNameStr.c_str());
      HFONT hOldF = (HFONT)SelectObject(hdcMem, hFont);
      SIZE textSZ;
      GetTextExtentPoint32W(hdcMem, text, (int)wcslen(text), &textSZ);
      if (textSZ.cx <= cellW - 1 || fSize == 6) {
        DrawTextW(hdcMem, text, -1, &cellRect, drawFlags);
        SelectObject(hdcMem, hOldF);
        DeleteObject(hFont);
        break;
      }
      SelectObject(hdcMem, hOldF);
      DeleteObject(hFont);
    }
    SelectClipRgn(hdcMem, NULL);
    DeleteObject(hRgn);
  };

  for (int r = 0; r < 7; r++) {
    int yTop = r * 16 + 1;
    int yBottom = (r + 1) * 16 - 2;

    RECT rCol1 = {1, yTop, 27, yBottom};
    RECT rCol2 = {29, yTop, 123, yBottom};

    drawScaledText(rCol1, trainRows[r].no ? trainRows[r].no : L"", tableFontHeight,
                   rCols[r][0]);
    drawScaledText(rCol2, trainRows[r].name ? trainRows[r].name : L"", tableFontHeight,
                   rCols[r][1]);

    if (IsCancelledStatus(trainRows[r].sta)) {
      RECT rMergedStatus = {125, yTop, 191, yBottom};
      COLORREF statusColor = rCols[r][5]; // Configured status color from VDB/AVDB settings screen!
      LPCWSTR dispStatus = GetCleanStatusText(trainRows[r].sta);
      drawScaledText(rMergedStatus, (dispStatus && wcslen(dispStatus) > 0) ? dispStatus : L"CANCELLED", tableFontHeight, statusColor);
    } else {
      RECT rCol3 = {125, yTop, 149, yBottom};
      RECT rCol4 = {151, yTop, 175, yBottom};
      RECT rCol5 = {177, yTop, 191, yBottom};

      drawScaledText(rCol3, trainRows[r].eat ? trainRows[r].eat : L"", tableFontHeight,
                     rCols[r][2]);
      drawScaledText(rCol4, trainRows[r].edt ? trainRows[r].edt : L"", tableFontHeight,
                     rCols[r][3]);
      drawScaledText(rCol5, trainRows[r].pf ? trainRows[r].pf : L"", tableFontHeight,
                     rCols[r][4]);
    }
  }

  // Save BMP
  BITMAPFILEHEADER bfh = {0};
  bfh.bfType = 0x4D42;
  bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  bfh.bfSize = bfh.bfOffBits + (width * height * 3);

  HANDLE hFile = CreateFileW(outFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hbmp);
    DeleteDC(hdcMem);
    return false;
  }

  DWORD written = 0;
  WriteFile(hFile, &bfh, sizeof(bfh), &written, NULL);
  WriteFile(hFile, &bmi.bmiHeader, sizeof(bmi.bmiHeader), &written, NULL);
  WriteFile(hFile, pBits, width * height * 3, &written, NULL);
  CloseHandle(hFile);

  SelectObject(hdcMem, hOldBmp);
  DeleteObject(hbmp);
  DeleteDC(hdcMem);
  return true;
}

bool GenerateTableBMP2Rows(LPCWSTR t1No, LPCWSTR t1Name, LPCWSTR t1Eat,
                           LPCWSTR t1Edt, LPCWSTR t1Pf, LPCWSTR t2No,
                           LPCWSTR t2Name, LPCWSTR t2Eat, LPCWSTR t2Edt,
                           LPCWSTR t2Pf, LPCWSTR customColorStr,
                           LPCWSTR borderColorHex, const wchar_t *outFilePath,
                           LPCWSTR langCode = L"en") {
  return GenerateTableBMP7Rows(t1No, t1Name, t1Eat, t1Edt, t1Pf, L"",
                               t2No, t2Name, t2Eat, t2Edt, t2Pf, L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               customColorStr, borderColorHex, L"0x00", outFilePath, langCode);
}

// Single row fallback wrapper
bool GenerateTableBMP(LPCWSTR trainNo, LPCWSTR trainName, LPCWSTR eat,
                      LPCWSTR edt, LPCWSTR pfNo, const wchar_t *outFilePath,
                      LPCWSTR langCode = L"en") {
  return GenerateTableBMP7Rows(trainNo, trainName, eat, edt, pfNo, L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               L"", L"", L"", L"", L"", L"",
                               L"yellow", L"FFFFFF", L"0x00", outFilePath, langCode);
}

static inline int MapToBxEffect(int effect) {
  BYTE bxEffect = 41; // Default fallback

  switch (effect) {
  case 0:
    bxEffect = 39;
    break; // 0x00 Reserved -> 39
  case 1:
    bxEffect = 18;
    break; // 0x01 Curtain Left to Right -> 18
  case 2:
    bxEffect = 16;
    break; // 0x02 Curtain Top to Bottom -> 16
  case 3:
    bxEffect = 15;
    break; // 0x03 Curtain Bottom to Top -> 15
  case 4:
    bxEffect = 38;
    break; // 0x04 Typing Left to Right -> 38
  case 5:
    bxEffect = 4;
    break; // 0x05 Running Right to Left -> 4
  case 6:
    bxEffect = 6;
    break; // 0x06 Running Top to Bottom -> 6
  case 7:
    bxEffect = 50;
    break; // 0x07 Running Bottom to Top -> 50
  case 8:
    bxEffect = 41;
    break; // 0x08 Flashing -> 41
  case 9:
    bxEffect = 36;
    break; // 0x09 Stable / Static -> 36
  default:
    bxEffect = static_cast<BYTE>(effect);
    break;
  }

  return bxEffect;
}

// Track last set intensity to avoid resetting brightness chip on every push
static int lastSetIntensity = -1;

// =========================================================
// TRANSMIT 192x112 TABLE DISPLAY TO BX-Y BOARD (7-ROW SUPPORT & DYNAMIC
// BRC/INT/EFF/SPD)
// =========================================================
void Program_dynamic_table_7rows(
    char *ip, int port,
    LPCWSTR t1No, LPCWSTR t1Name, LPCWSTR t1Eat, LPCWSTR t1Edt, LPCWSTR t1Pf, LPCWSTR t1Sta,
    LPCWSTR t2No, LPCWSTR t2Name, LPCWSTR t2Eat, LPCWSTR t2Edt, LPCWSTR t2Pf, LPCWSTR t2Sta,
    LPCWSTR t3No, LPCWSTR t3Name, LPCWSTR t3Eat, LPCWSTR t3Edt, LPCWSTR t3Pf, LPCWSTR t3Sta,
    LPCWSTR t4No, LPCWSTR t4Name, LPCWSTR t4Eat, LPCWSTR t4Edt, LPCWSTR t4Pf, LPCWSTR t4Sta,
    LPCWSTR t5No, LPCWSTR t5Name, LPCWSTR t5Eat, LPCWSTR t5Edt, LPCWSTR t5Pf, LPCWSTR t5Sta,
    LPCWSTR t6No, LPCWSTR t6Name, LPCWSTR t6Eat, LPCWSTR t6Edt, LPCWSTR t6Pf, LPCWSTR t6Sta,
    LPCWSTR t7No, LPCWSTR t7Name, LPCWSTR t7Eat, LPCWSTR t7Edt, LPCWSTR t7Pf, LPCWSTR t7Sta,
    LPCWSTR customColor, int customEffect,
    LPCWSTR borderColorHex, int intensity, int customSpeed,
    LPCWSTR chrHexStr, LPCWSTR langCode = L"en") {
  std::cout
      << "[SDK] Creating playlist (Width: 192, Height: 112, Type: 8536)..."
      << std::endl;

  // Set hardware screen intensity/brightness level only if changed
  if (set_screen_brightness && intensity > 0 && intensity <= 100 &&
      intensity != lastSetIntensity) {
    std::cout << "[SDK] Setting board intensity level: " << intensity << "%"
              << std::endl;
    set_screen_brightness(ip, port, L"guest", L"guest", intensity);
    lastSetIntensity = intensity;
  }

  wchar_t tempPath[MAX_PATH];
  GetTempPathW(MAX_PATH, tempPath);
  std::wstring bmpPath = std::wstring(tempPath) + L"led_table_192x112.bmp";

  bool bmpOk = false;
  if (t1No && wcsncmp(t1No, L"DEFAULT_MSG:", 12) == 0) {
    LPCWSTR msgText = t1No + 12;
    bmpOk = GenerateDefaultMessageBMP(msgText, bmpPath.c_str(), langCode);
  } else if (t1No && wcscmp(t1No, L"TOPGRIP") == 0) {
    bmpOk = GenerateDefaultMessageBMP(L"TOPGRIP", bmpPath.c_str(), langCode);
  } else {
    bmpOk = GenerateTableBMP7Rows(
        t1No, t1Name, t1Eat, t1Edt, t1Pf, t1Sta,
        t2No, t2Name, t2Eat, t2Edt, t2Pf, t2Sta,
        t3No, t3Name, t3Eat, t3Edt, t3Pf, t3Sta,
        t4No, t4Name, t4Eat, t4Edt, t4Pf, t4Sta,
        t5No, t5Name, t5Eat, t5Edt, t5Pf, t5Sta,
        t6No, t6Name, t6Eat, t6Edt, t6Pf, t6Sta,
        t7No, t7Name, t7Eat, t7Edt, t7Pf, t7Sta,
        customColor, borderColorHex, chrHexStr ? chrHexStr : L"0x00",
        bmpPath.c_str(), langCode);
  }

  if (!bmpOk) {
    std::cerr << "ERROR: Failed generating 192x112 table BMP image."
              << std::endl;
  }

  unsigned long playlist = create_playlist(192, 112, 8536);
  unsigned long program = create_program(L"program_1", _TEXT_T("0xff000000"));

  int display_effects = MapToBxEffect(customEffect);

  int display_speed = 10;
  if (customSpeed == 0x00 || customSpeed == 0) {
    display_speed = 30;
  } else if (customSpeed == 0x01 || customSpeed == 1) {
    display_speed = 20;
  } else if (customSpeed == 0x02 || customSpeed == 2) {
    display_speed = 10;
  } else if (customSpeed == 0x03 || customSpeed == 3) {
    display_speed = 5;
  } else if (customSpeed == 0x04 || customSpeed == 4) {
    display_speed = 1;
  } else if (customSpeed > 0 && customSpeed <= 30) {
    display_speed = customSpeed;
  }

  int stay_time = 65535; // Hold static frame indefinitely
  int gif_flag = 0;
  LPCWSTR bg_color = L"0xff000000";
  LPCWSTR font_attributes = L"normal";
  std::wstring activeFont = getFontForLanguageW(langCode ? langCode : L"en");
  LPCWSTR font = activeFont.c_str();
  LPCWSTR align_h = L"0";
  LPCWSTR align_v = L"0";

  // --- Picture Dynamic Unit (BMP Image) ---
  unsigned long dynamic_area = create_dynamic();
  int err1 = add_dynamic_unit(dynamic_area, 0, display_effects, display_speed,
                              stay_time, bmpPath.c_str(), gif_flag, bg_color,
                              12, font, customColor, font_attributes, align_h,
                              align_v, 0, 0, 0, _TEXT_T(""), _TEXT_T(""));

  int err2 = add_dynamic(program, dynamic_area, 0, 0, 0, 192, 112, _T(""), 0,
                         _T(""), 100);

  delete_dynamic(dynamic_area);

  int err3 = add_program_in_playlist(playlist, program, 0, 0, _T(""), _T(""),
                                     _T(""), _T(""), 127);

  std::cout << "[SDK] Pushing 192x112 7-Row Table to Board -> IP: " << ip
            << " | Port: " << port << " | EFF: " << customEffect
            << " (BX: " << display_effects << ") | SPD: " << customSpeed
            << " (BX: " << display_speed << ")" << std::endl;

  // Use conver=1 (in-place smooth dynamic update without deleting/blanking
  // screen to black)
  int err4 = update_dynamic_small(ip, port, L"guest", L"guest", playlist,
                                  _T(""), 1, 0);
  if (err4 != 0) {
    err4 = update_dynamic(ip, port, (wchar_t *)L"guest", (wchar_t *)L"guest",
                          playlist, (wchar_t *)L"", 1, 0);
  }

  delete_playlist(playlist);
}

void Program_dynamic_table_2rows(char *ip, int port, LPCWSTR t1No,
                                 LPCWSTR t1Name, LPCWSTR t1Eat, LPCWSTR t1Edt,
                                 LPCWSTR t1Pf, LPCWSTR t2No, LPCWSTR t2Name,
                                 LPCWSTR t2Eat, LPCWSTR t2Edt, LPCWSTR t2Pf,
                                 LPCWSTR customColor, int customEffect,
                                 LPCWSTR borderColorHex, int intensity,
                                 int customSpeed, LPCWSTR langCode = L"en") {
  Program_dynamic_table_7rows(
      ip, port, t1No, t1Name, t1Eat, t1Edt, t1Pf, L"",
      t2No, t2Name, t2Eat, t2Edt, t2Pf, L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      customColor, customEffect, borderColorHex, intensity, customSpeed, L"0x00", langCode);
}

void Program_dynamic_table(char *ip, int port, LPCWSTR trainNo,
                           LPCWSTR trainName, LPCWSTR eat, LPCWSTR edt,
                           LPCWSTR pfNo, LPCWSTR customColor,
                           int customEffect, LPCWSTR langCode = L"en") {
  Program_dynamic_table_7rows(ip, port, trainNo, trainName, eat, edt, pfNo, L"",
                              L"", L"", L"", L"", L"", L"",
                              L"", L"", L"", L"", L"", L"",
                              L"", L"", L"", L"", L"", L"",
                              L"", L"", L"", L"", L"", L"",
                              L"", L"", L"", L"", L"", L"",
                              L"", L"", L"", L"", L"", L"",
                              customColor, customEffect, L"FFFFFF", 100, 2, L"0x00", langCode);
}

void Program_dynamic_small(char *ip, int port, LPCWSTR messageText,
                           LPCWSTR customColor, int customEffect, LPCWSTR langCode = L"en") {
  std::cout
      << "[SDK] Creating playlist (Width: 192, Height: 112, Type: 8536)..."
      << std::endl;

  unsigned long playlist = create_playlist(192, 112, 8536);
  unsigned long program = create_program(L"program_1", _TEXT_T("0xff000000"));

  int dynamic_type = 1;
  int display_effects = MapToBxEffect(customEffect);
  int display_speed = 10;
  int stay_time = 65535;
  int gif_flag = 0;
  LPCWSTR bg_color = L"0xff000000";
  LPCWSTR font_attributes = L"normal";
  std::wstring activeFont = getFontForLanguageW(langCode && wcslen(langCode) > 0 ? langCode : messageText);
  LPCWSTR font = activeFont.c_str();
  LPCWSTR align_h = L"0";
  LPCWSTR align_v = L"0";

  int utf8Len =
      WideCharToMultiByte(CP_UTF8, 0, messageText, -1, NULL, 0, NULL, NULL);
  char *utf8Str = new char[utf8Len];
  WideCharToMultiByte(CP_UTF8, 0, messageText, -1, utf8Str, utf8Len, NULL,
                      NULL);

  std::string encoded = base64_encode(
      reinterpret_cast<const unsigned char *>(utf8Str), utf8Len - 1);
  delete[] utf8Str;

  LPCWSTR wcstring = stringToLPCWSTR_Safe(encoded);

  unsigned long dynamic_area = create_dynamic();
  int err1 = add_dynamic_unit(
      dynamic_area, dynamic_type, display_effects, display_speed, stay_time,
      wcstring, gif_flag, bg_color, 16, font, customColor, font_attributes,
      align_h, align_v, 0, 0, 0, _TEXT_T(""), _TEXT_T(""));

  delete[] wcstring;

  int err2 = add_dynamic(program, dynamic_area, 0, 0, 0, 192, 112, _T(""), 0,
                         _T(""), 100);

  delete_dynamic(dynamic_area);

  int err3 = add_program_in_playlist(playlist, program, 1, 10, _T(""), _T(""),
                                     _T(""), _T(""), 127);

  std::cout << "[SDK] Sending to board..." << std::endl;
  int err4 = update_dynamic_small(ip, port, L"guest", L"guest", playlist,
                                  _T(""), 1, 0);

  if (err4 == 0) {
    std::cout << "[SDK] update_dynamic_small SUCCESS (Returned 0)" << std::endl;
  } else {
    std::cout << "[SDK] update_dynamic_small FAILED with Error Code: " << err4
              << std::endl;
  }

  cancel_send_program(playlist);
  delete_playlist(playlist);
}

// Helper to scale high-res image (JPG/PNG/BMP) to 192x112 24-bit BMP using GDI+
bool ScaleImageTo192x112BMP(LPCWSTR srcImagePath, LPCWSTR outBmpPath) {
  ULONG_PTR gdiplusToken;
  GdiplusStartupInput gdiplusStartupInput;
  if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Ok) {
    return false;
  }

  bool success = false;
  {
    Bitmap srcBitmap(srcImagePath);
    if (srcBitmap.GetLastStatus() == Ok) {
      int dstWidth = 192;
      int dstHeight = 112;

      Bitmap dstBitmap(dstWidth, dstHeight, PixelFormat24bppRGB);
      Graphics graphics(&dstBitmap);
      graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
      graphics.DrawImage(&srcBitmap, 0, 0, dstWidth, dstHeight);

      CLSID bmpClsid;
      UINT num = 0, size = 0;
      GetImageEncodersSize(&num, &size);
      if (size > 0) {
        ImageCodecInfo *pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
        if (pImageCodecInfo) {
          GetImageEncoders(num, size, pImageCodecInfo);
          for (UINT j = 0; j < num; ++j) {
            if (wcscmp(pImageCodecInfo[j].MimeType, L"image/bmp") == 0) {
              bmpClsid = pImageCodecInfo[j].Clsid;
              success = (dstBitmap.Save(outBmpPath, &bmpClsid, NULL) == Ok);
              break;
            }
          }
          free(pImageCodecInfo);
        }
      }
    }
  }

  GdiplusShutdown(gdiplusToken);
  return success;
}

void Program_image(char *ip, int port, LPCWSTR imageFilePath) {
  DWORD dwAttrib = GetFileAttributesW(imageFilePath);
  if (dwAttrib == INVALID_FILE_ATTRIBUTES ||
      (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
    std::cerr << "ERROR: Image file does not exist or is invalid." << std::endl;
    return;
  }

  wchar_t tempPath[MAX_PATH];
  GetTempPathW(MAX_PATH, tempPath);
  std::wstring scaledBmpPath =
      std::wstring(tempPath) + L"scaled_temp_image.bmp";

  std::wcout << L"[SDK] Scaling High-Res Image to 192x112 BMP -> "
             << imageFilePath << std::endl;
  bool scaleOk = ScaleImageTo192x112BMP(imageFilePath, scaledBmpPath.c_str());
  if (!scaleOk) {
    std::cerr << "ERROR: Failed scaling image to 192x112 BMP." << std::endl;
    return;
  }

  std::wcout << L"[SDK] Saved scaled 192x112 bitmap -> " << scaledBmpPath
             << std::endl;

  if (clear_dynamic) {
    clear_dynamic(ip, (unsigned short)port, L"guest", L"guest");
    Sleep(100);
  }

  std::cout << "[SDK] Creating dynamic playlist for image (Width: 192, Height: "
               "112, Type: 8536)..."
            << std::endl;
  unsigned long playlist = create_playlist(192, 112, 8536);
  unsigned long program = create_program(L"program_img", _TEXT_T("0xff000000"));

  unsigned long dynamic_area = create_dynamic();
  int err1 =
      add_dynamic_unit(dynamic_area, 0, 52, 10, 65535, scaledBmpPath.c_str(), 0,
                       L"0xff000000", 12, L"Arial", L"yellow", L"normal", L"0",
                       L"0", 0, 0, 0, _TEXT_T(""), _TEXT_T(""));

  int err2 = add_dynamic(program, dynamic_area, 0, 0, 0, 192, 112, _T(""), 0,
                         _T(""), 100);

  delete_dynamic(dynamic_area);

  int err3 = add_program_in_playlist(playlist, program, 0, 0, _T(""), _T(""),
                                     _T(""), _T(""), 127);

  std::cout << "[SDK] Streaming 192x112 Image Dynamic Area to Board -> IP: "
            << ip << " | Port: " << port << std::endl;

  int err = update_dynamic_small(ip, (unsigned short)port, L"guest", L"guest",
                                 playlist, _T(""), 1, 0);
  if (err != 0 && update_dynamic != NULL) {
    err = update_dynamic(ip, (unsigned short)port, (wchar_t *)L"guest",
                         (wchar_t *)L"guest", playlist, (wchar_t *)L"", 1, 0);
  }

  std::cout << "[SDK] update_dynamic_small result code: " << err << std::endl;

  delete_playlist(playlist);
}

void Program_video(char *ip, int port, LPCWSTR videoFilePath) {
  DWORD dwAttrib = GetFileAttributesW(videoFilePath);
  if (dwAttrib == INVALID_FILE_ATTRIBUTES ||
      (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
    std::cerr << "ERROR: Video file does not exist or is invalid." << std::endl;
    return;
  }

  // Clear dynamic RAM area so old dynamic text/bitmaps do not overlay on the video program
  if (clear_dynamic) {
    clear_dynamic(ip, (unsigned short)port, L"guest", L"guest");
    Sleep(100);
  }

  std::wcout << L"[SDK] Dispatching Program Media Video -> IP: " << ip
             << L":" << port << L" | File: " << videoFilePath << std::endl;

  // 1. Create standard program playlist for 192x112 (BX-Y08 controller type 8536)
  unsigned long playlist = create_playlist(192, 112, 8536);
  unsigned long program = create_program(L"video_prog", _TEXT_T("0xff000000"));

  // 2. Create video area and add video unit covering 192x112
  unsigned long video_area = create_video();
  int err1 = add_video_unit(video_area, 100, 1, 0, 0, videoFilePath, L"");
  int err2 = add_video(program, video_area, 0, 0, 192, 112, 0, 0, 0, L"", L"");

  // 3. Add program to playlist with loop playback
  int err3 = add_program_in_playlist(
      playlist, program, 0, 65535,
      L"2020-01-01", L"2035-12-31", L"00:00:00", L"23:59:59", 127);

  // 4. Dispatch via send_program (file playlist upload)
  wchar_t tempPath[MAX_PATH];
  GetTempPathW(MAX_PATH, tempPath);
  long long free_size = 0, total_size = 0;

  if (send_program) {
    try {
      int err = send_program(ip, (unsigned short)port, L"guest", L"guest",
                             tempPath, playlist, 0, &free_size, &total_size);
      std::cout << "[SDK] send_program (Video Program) result code: " << err
                << " | Free: " << free_size << " | Total: " << total_size << std::endl;
    } catch (...) {
      std::cerr << "ERROR: Exception occurred while sending video program to LED board."
                << std::endl;
    }
  } else {
    std::cerr << "ERROR: send_program function pointer is null." << std::endl;
  }

  // 5. Clean up handles
  if (cancel_send_program)
    cancel_send_program(playlist);
  if (delete_playlist)
    delete_playlist(playlist);
}

// ---------------------------------------------------------
// Original remaining SDK functions (Left untouched)
// ---------------------------------------------------------

void Program_dynamic_uint_small(char *ip, int port, LPCWSTR str) {
  unsigned long playlist = create_playlist(128, 96, 8536);
  unsigned long program = create_program(L"program_1", _TEXT_T("0xff000000"));

  int dynamic_type = 1;
  int display_effects = 52;
  int display_speed = 10;
  int stay_time = 0;
  int gif_flag = 0;
  LPCWSTR bg_color = _T("0xff000000");
  LPCWSTR color = _T("0xffff0000");
  LPCWSTR font_attributes = _T("normal");
  LPCWSTR font = _T("SimSun");
  LPCWSTR align_h = _T("0");
  LPCWSTR align_v = _T("0");

  std::string file = string_To_UTF8("动态区更新789");
  std::string encoded = base64_encode(
      reinterpret_cast<const unsigned char *>(file.c_str()), file.length());
  LPCWSTR File = stringToLPCWSTR(encoded);
  unsigned long dynamic_area = create_dynamic();
  int err = add_dynamic_unit(dynamic_area, dynamic_type, display_effects,
                             display_speed, stay_time, File, gif_flag, bg_color,
                             12, font, color, font_attributes, align_h, align_v,
                             0, 0, 0, _TEXT_T(""), _TEXT_T(""));
  err = add_dynamic(program, dynamic_area, 0, 0, 0, 32, 32, _T(""), 0, _T(""),
                    100);
  delete_dynamic(dynamic_area);

  err = add_program_in_playlist(playlist, program, 1, 10, _T(""), _T(""),
                                _T(""), _T(""), 127);
  err = update_dynamic_unit_small(ip, port, str, str, playlist);

  cancel_send_program(playlist);
  delete_playlist(playlist);
}

void Program_dynamic(char *ip, int port, LPCWSTR str) {
  unsigned long playlist = create_playlist(64, 32, 8536);
  unsigned long program = create_program(L"program_1", _TEXT_T("0xff000000"));

  int dynamic_type = 1;
  int display_effects = 52;
  int display_speed = 10;
  int stay_time = 0;
  int gif_flag = 0;
  LPCWSTR bg_color = L"0xff000000";
  LPCWSTR color = L"0xffff0000";
  LPCWSTR font_attributes = L"normal";
  LPCWSTR font = L"SimSun";
  LPCWSTR align_h = L"0";
  LPCWSTR align_v = L"0";

  LPCWSTR ff = L"1.txt";
  TCHAR szFilePath[MAX_PATH + 1] = {0};
  GetModuleFileName(NULL, szFilePath, MAX_PATH);
  (_tcsrchr(szFilePath, L'\\'))[1] = 0;
  LPCWSTR f = wcscat(szFilePath, ff);
  unsigned long dynamic_area = create_dynamic();
  int err = add_dynamic_unit(dynamic_area, dynamic_type, display_effects,
                             display_speed, stay_time, f, gif_flag, bg_color,
                             12, font, color, font_attributes, align_h, align_v,
                             0, 0, 0, _TEXT_T(""), _TEXT_T(""));
  err =
      add_dynamic(program, dynamic_area, 0, 0, 0, 64, 32, _T(""), 0, L"", 100);
  delete_dynamic(dynamic_area);

  LPCWSTR m_aging_start_time = _T("2018-12-01");
  LPCWSTR m_aging_stop_time = _T("2018-12-30");
  LPCWSTR m_period_ontime = _T("15:14:00");
  LPCWSTR m_period_offtime = _T("15:15:00");
  err = add_program_in_playlist(playlist, program, 0, 10, m_aging_start_time,
                                m_aging_stop_time, m_period_ontime,
                                m_period_offtime, 127);
  err = update_dynamic(ip, port, str, str, playlist, _T(""), 1, 0);

  cancel_send_program(playlist);
  delete_playlist(playlist);
}

void Program_dynamic_uint(char *ip, int port, LPCWSTR str) {
  unsigned long playlist = create_playlist(128, 96, 8536);
  unsigned long program = create_program(L"program_1", _TEXT_T("0xff000000"));

  int dynamic_type = 1;
  int display_effects = 52;
  int display_speed = 10;
  int stay_time = 0;
  int gif_flag = 0;
  LPCWSTR bg_color = L"0xff000000";
  LPCWSTR color = L"0xffff0000";
  LPCWSTR font_attributes = L"normal";
  LPCWSTR font = L"SimSun";
  LPCWSTR align_h = L"0";
  LPCWSTR align_v = L"0";

  LPCWSTR ff = L"2.txt";
  TCHAR szFilePath[MAX_PATH + 1] = {0};
  GetModuleFileName(NULL, szFilePath, MAX_PATH);
  (_tcsrchr(szFilePath, L'\\'))[1] = 0;
  LPCWSTR f = wcscat(szFilePath, ff);
  unsigned long dynamic_area = create_dynamic();
  int err = add_dynamic_unit(dynamic_area, dynamic_type, display_effects,
                             display_speed, stay_time, f, gif_flag, bg_color,
                             12, font, color, font_attributes, align_h, align_v,
                             0, 0, 0, _TEXT_T(""), _TEXT_T(""));
  err =
      add_dynamic(program, dynamic_area, 0, 0, 0, 64, 32, _T(""), 0, L"", 100);
  delete_dynamic(dynamic_area);

  err = add_program_in_playlist(playlist, program, 1, 10, _T(""), _T(""),
                                _T(""), _T(""), 127);
  err = update_dynamic_unit(ip, port, str, str, playlist);

  cancel_send_program(playlist);
  delete_playlist(playlist);
}

void Program_cleardynamic(char *ip, int port, LPCWSTR str) {
  int err = clear_dynamic(ip, port, str, str);
}

void Program_bmp(char *ip, int port, LPCWSTR str) {
  unsigned long playlist = create_playlist(128, 96, 8536);
  LPCWSTR name = L"program_1";
  unsigned long program = create_program(name, _TEXT_T("0xff000000"));
  LPCWSTR ff = L"男.bmp";
  TCHAR szFilePath[MAX_PATH + 1] = {0};
  GetModuleFileName(NULL, szFilePath, MAX_PATH);
  (_tcsrchr(szFilePath, L'\\'))[1] = 0;
  LPCWSTR file = wcscat(szFilePath, ff);

  unsigned long pic_area = create_pic();
  int err = add_pic_unit(pic_area, 0, 5, 1, 1, file, L"");
  err = add_pic(program, pic_area, 0, 0, 128, 96, 0, 0, 0, L"", L"");

  LPCWSTR m_aging_start_time = _T("2018-12-01");
  LPCWSTR m_aging_stop_time = _T("2018-12-30");
  LPCWSTR m_period_ontime = _T("15:20:00");
  LPCWSTR m_period_offtime = _T("15:21:00");
  err = add_program_in_playlist(playlist, program, 0, 10, m_aging_start_time,
                                m_aging_stop_time, m_period_ontime,
                                m_period_offtime, 127);
  int send_style = 0;

  LPCWSTR tmp_path = L"F:\\Temp\\";
  long long free_size = 0, total_size = 0;
  err = send_program(ip, port, str, str, tmp_path, playlist, send_style,
                     &free_size, &total_size);

  cancel_send_program(playlist);
  delete_playlist(playlist);
}

void Program_time(char *ip, int port, LPCWSTR str) {
  unsigned long playlist = create_playlist(128, 96, 8280);
  LPCWSTR name = L"program_1";
  unsigned long program = create_program(name, _TEXT_T("0xff000000"));

  unsigned long time_area = create_time();
  LPCWSTR content1 = L"%Y年%m月%d日";
  LPCWSTR font = L"宋体";
  LPCWSTR color = L"0xffff0000";
  LPCWSTR font_attributes = L"bold";
  LPCWSTR bg_color = L"0xff00f000";
  LPCWSTR time_equation = L"1:0:00";
  LPCWSTR positive_te = L"true";
  LPCWSTR adjustment = L"+00:00:00:00";
  int err;

  err = add_time_unit(time_area, content1, color, font, 12, 0, 32,
                      font_attributes);
  err = add_time(program, time_area, 0, 0, 128, 96, 255, bg_color,
                 time_equation, positive_te, adjustment);

  err = add_program_in_playlist(playlist, program, 0, 10, L"", L"", L"", L"",
                                127);
  int send_style = 0;
  LPCWSTR tmp_path = L"F:\\Temp\\";
  long long free_size = 0, total_size = 0;
  err = send_program(ip, port, str, str, tmp_path, playlist, send_style,
                     &free_size, &total_size);

  cancel_send_program(playlist);
  delete_playlist(playlist);
}

void Program_lock_screen(char *ip, int port, LPCWSTR str, int nlock) {
  int err = lock_screen(ip, port, str, str, nlock);
}
void Program_set_screen_volumn(char *ip, int port, LPCWSTR str, int volumn) {
  int err = set_screen_volumn(ip, port, str, str, volumn);
}
void Program_set_screen_brightness(char *ip, int port, LPCWSTR str,
                                   int brightness) {
  int err = set_screen_brightness(ip, port, str, str, brightness);
}
void Program_set_screen_cus_brightness(char *ip, int port, LPCWSTR str) {
  unsigned short brightness[48] = {0};
  for (int i = 0; i < 48; i++) {
    brightness[i] = 255;
  }
  int err = set_screen_cus_brightness(ip, port, str, str, brightness, 48);
}
void Program_set_screen_turnonoff(char *ip, int port, LPCWSTR str,
                                  int turnonoff) {
  int err = set_screen_turnonoff(ip, port, str, str, turnonoff);
}
void Program_set_screen_cus_turnonoff(char *ip, int port, LPCWSTR str) {
  unsigned long trunonoff = create_turnonoff();
  add_turnonoff(trunonoff, 1, L"22:15:00");
  add_turnonoff(trunonoff, 0, L"22:16:00");
  add_turnonoff(trunonoff, 1, L"22:24:00");
  add_turnonoff(trunonoff, 0, L"22:26:00");
  int err = set_screen_cus_turnonoff(ip, port, str, str, trunonoff);
  delete_turnonoff(trunonoff);
}
void Program_cancel_screen_cus_turnonoff(char *ip, int port, LPCWSTR str) {
  int err = cancel_screen_cus_turnonoff(ip, port, str, str);
}

void Program_str(char *ip, int port, LPCWSTR str) {
  int err = 0;

  unsigned long playlist = create_playlist(64, 32, 8536);
  LPCWSTR name = L"默认字幕显示";
  unsigned long program = create_program(name, _TEXT_T("0xff000000"));
  unsigned long area_tree = create_text();
  err = add_text_unit_text(area_tree, 5, 5, L"SimSun", 12, L"normal", L"0",
                           L"0xffff0000", L"0xff000000", L"111111");
  err = add_text(program, area_tree, 0, 0, 64, 32, 100, 4, 1);

  err = add_program_in_playlist(playlist, program, 0, 10, _T(""), _T(""),
                                _T(""), _T(""), 127);
  int send_style = 0;

  LPCWSTR tmp_path = L"F:\\Temp\\";
  long long free_size = 0, total_size = 0;
  err = send_program(ip, port, str, str, tmp_path, playlist, send_style,
                     &free_size, &total_size);

  cancel_send_program(playlist);
  delete_playlist(playlist);
}

void Program_str1(char *ip, int port, LPCWSTR user_name, LPCWSTR user_pwd) {
  int w = 256, h = 120, device_type = 8536;
  unsigned long play_list = create_playlist(w, h, device_type);

  _TEXT_CHAR *name = L"project-1";
  unsigned long program = create_program(name, _TEXT_T("0xff000000"));

  unsigned long text_area = create_text();
  unsigned long text_area1 = create_text();
  unsigned long text_area2 = create_text();
  unsigned long text_area3 = create_text();
  unsigned long text_area4 = create_text();
  unsigned long text_area5 = create_text();

  int x = 470, y = 0, layout = 0, transparency = 100, font_size = 10;
  int display_effects = 52, display_speed = 16, stay_time = 0, content_x = 60;
  int content_y = 20, unit_type = 1;

  _TEXT_CHAR *font_name = L"Arial", *bg_color = L"0xFF000000";
  _TEXT_CHAR *font_attributes = L"normal", *font_alignment = L"10";

  int ate = add_text_unit_text(text_area, stay_time, display_speed, font_name,
                               font_size, font_attributes, font_alignment,
                               L"0xFFFFAB00", bg_color, L"CHINESE");

  add_text_unit_text(text_area1, 0, 16, font_name, 10, L"normal", L"10",
                     L"0xFFFF0000", bg_color, L"501C");
  add_text_unit_text(text_area2, 0, 16, font_name, 10, L"normal", L"10",
                     L"0xFFFF0000", bg_color, L"AURO");
  add_text_unit_text(text_area3, 0, 16, font_name, 10, L"normal", L"10",
                     L"0xFFFF0000", bg_color, L"LED");
  add_text_unit_text(text_area4, 0, 16, font_name, 10, L"normal", L"10",
                     L"0xFFFF0000", bg_color, L"DISPLAY");
  add_text_unit_text(text_area5, 0, 16, font_name, 10, L"normal", L"10",
                     L"0xFFFF0000", bg_color, L"BUSES");

  ate = add_text(program, text_area, 201, 0, 55, 20, transparency,
                 display_effects, unit_type);
  add_text(program, text_area1, 201, 20, 55, 20, transparency, 1, unit_type);
  add_text(program, text_area2, 160, 0, 40, 20, transparency, 1, unit_type);
  add_text(program, text_area3, 160, 20, 40, 20, transparency, 1, unit_type);
  add_text(program, text_area4, 120, 0, 40, 20, transparency, 1, unit_type);
  add_text(program, text_area5, 120, 20, 40, 20, transparency, 1, unit_type);

  int pap = add_program_in_playlist(play_list, program, 1, 1, L"", L"", L"",
                                    L"", 127);
  _TEXT_CHAR *tmp_path = L"F:\\Temp\\";
  int send_style = 0;
  long long free_size, total_size;
  int sam = send_program(ip, port, user_name, user_pwd, tmp_path, play_list,
                         send_style, &free_size, &total_size);
}

void Program_dynamic1(char *ip, int port, LPCWSTR str) {
  unsigned long playlist = create_playlist(64, 32, 8536);
  unsigned long program = create_program(L"program_1", _TEXT_T("0xff000000"));

  int dynamic_type = 1;
  int display_effects = 52;
  int display_speed = 10;
  int stay_time = 0;
  int gif_flag = 0;
  LPCWSTR bg_color = L"0xff000000";
  LPCWSTR color = L"0xffff0000";
  LPCWSTR font_attributes = L"normal";
  LPCWSTR font = L"SimSun";
  LPCWSTR align_h = L"0";
  LPCWSTR align_v = L"0";

  unsigned long dynamic_area = create_dynamic();
  int err = add_dynamic_unit(
      dynamic_area, dynamic_type, display_effects, display_speed, stay_time,
      _TEXT_T("1.txt"), gif_flag, bg_color, 12, font, color, font_attributes,
      align_h, align_v, 0, 0, 0, _TEXT_T(""), _TEXT_T(""));
  err = add_dynamic_unit(dynamic_area, 0, display_effects, display_speed,
                         stay_time, _TEXT_T("1.bmp"), gif_flag, bg_color, 12,
                         font, color, font_attributes, _TEXT_T("0"),
                         _TEXT_T("0"), 0, 0, 0, _TEXT_T(""), _TEXT_T(""));

  err = add_dynamic(program, dynamic_area, 0, 0, 0, 64, 32, L"", 0, L"", 100);
  delete_dynamic(dynamic_area);

  err = add_program_in_playlist(playlist, program, 0, 10, _T("2018-12-01"),
                                _T("2018-12-30"), _T("15:14:00"),
                                _T("15:15:00"), 127);
  err = update_dynamic(ip, port, _TEXT_T("guest"), _TEXT_T("guest"), playlist,
                       _T(""), 1, 0);

  cancel_send_program(playlist);
  delete_playlist(playlist);
}

// =========================================================
// UNIFIED ENTRY POINT: SendToLedBoard
// =========================================================
static std::wstring Utf8ToWideString(const char* str) {
  if (!str || !*str) return L"";
  int wlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
  if (wlen <= 0) return L"";
  std::wstring wstr(wlen - 1, 0);
  MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], wlen);
  return wstr;
}

int SendToLedBoard(const char* ip, unsigned short port, const char* message,
                   int effect, const char* border, int intensity, int speed,
                   int chr, const char* lang) {
  if (!ip || !message) {
    std::cerr << "[SDK ERROR] Missing IP or Message argument." << std::endl;
    return -1;
  }

  InitializeBXSDK();

  std::wstring wMsg = Utf8ToWideString(message);
  std::wstring wBorder = Utf8ToWideString(border ? border : "FFFFFF");
  std::wstring wLang = Utf8ToWideString(lang ? lang : "en");
  if (wBorder.empty()) wBorder = L"FFFFFF";
  if (wLang.empty()) wLang = L"en";

  wchar_t chrHex[16];
  swprintf_s(chrHex, 16, L"0x%02X", chr & 0xFF);

  // 1. Check if the message is a file on disk (or MEDIA/BGCOLOR keyword)
  DWORD dwAttrib = wMsg.empty() ? INVALID_FILE_ATTRIBUTES : GetFileAttributesW(wMsg.c_str());
  bool isFile = (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

  if (!isFile && (_wcsicmp(wMsg.c_str(), L"MEDIA") == 0 || _wcsicmp(wMsg.c_str(), L"BGCOLOR") == 0)) {
    std::wstring defaultBg = L"D:\\IPIS_Edge_VDC\\media\\bgcolor.png";
    if (GetFileAttributesW(defaultBg.c_str()) != INVALID_FILE_ATTRIBUTES) {
      wMsg = defaultBg;
      isFile = true;
    } else {
      defaultBg = L"media\\bgcolor.png";
      if (GetFileAttributesW(defaultBg.c_str()) != INVALID_FILE_ATTRIBUTES) {
        wMsg = defaultBg;
        isFile = true;
      }
    }
  } else if (!isFile && (_wcsicmp(wMsg.c_str(), L"VIDEO") == 0)) {
    std::wstring defaultVid = L"D:\\IPIS_Edge_VDC\\media\\video.mp4";
    if (GetFileAttributesW(defaultVid.c_str()) != INVALID_FILE_ATTRIBUTES) {
      wMsg = defaultVid;
      isFile = true;
    } else {
      defaultVid = L"media\\video.mp4";
      if (GetFileAttributesW(defaultVid.c_str()) != INVALID_FILE_ATTRIBUTES) {
        wMsg = defaultVid;
        isFile = true;
      }
    }
  }

  if (isFile) {
    size_t dotPos = wMsg.rfind(L'.');
    std::wstring ext = (dotPos != std::wstring::npos) ? wMsg.substr(dotPos) : L"";
    for (auto &c : ext) c = towlower(c);

    if (ext == L".mp4" || ext == L".avi" || ext == L".mkv" || ext == L".mov" || ext == L".wmv") {
      std::wcout << L"[SDK] Transmitting Video File -> " << wMsg << std::endl;
      Program_video((char*)ip, (int)port, wMsg.c_str());
      return 0;
    } else {
      std::wcout << L"[SDK] Transmitting Image File -> " << wMsg << std::endl;
      Program_image((char*)ip, (int)port, wMsg.c_str());
      return 0;
    }
  }

  // 2. Check if the message contains multi-row table format (delimiters '|', '~', '\n')
  bool isTable = (wMsg.find(L'|') != std::wstring::npos ||
                  wMsg.find(L'~') != std::wstring::npos ||
                  wMsg.find(L'\n') != std::wstring::npos);

  if (isTable) {
    std::vector<std::wstring> rows;
    std::wstringstream wss(wMsg);
    std::wstring line;
    wchar_t rowDelim = (wMsg.find(L'~') != std::wstring::npos) ? L'~' : L'\n';
    while (std::getline(wss, line, rowDelim)) {
      if (!line.empty()) {
        rows.push_back(line);
      }
    }

    std::wstring rNo[7], rName[7], rEat[7], rEdt[7], rPf[7], rSta[7];
    for (size_t i = 0; i < 7 && i < rows.size(); ++i) {
      std::wstringstream rowStream(rows[i]);
      std::getline(rowStream, rNo[i], L'|');
      std::getline(rowStream, rName[i], L'|');
      std::getline(rowStream, rEat[i], L'|');
      std::getline(rowStream, rEdt[i], L'|');
      std::getline(rowStream, rPf[i], L'|');
      std::getline(rowStream, rSta[i], L'|');
    }

    Program_dynamic_table_7rows(
        (char*)ip, (int)port,
        rNo[0].c_str(), rName[0].c_str(), rEat[0].c_str(), rEdt[0].c_str(), rPf[0].c_str(), rSta[0].c_str(),
        rNo[1].c_str(), rName[1].c_str(), rEat[1].c_str(), rEdt[1].c_str(), rPf[1].c_str(), rSta[1].c_str(),
        rNo[2].c_str(), rName[2].c_str(), rEat[2].c_str(), rEdt[2].c_str(), rPf[2].c_str(), rSta[2].c_str(),
        rNo[3].c_str(), rName[3].c_str(), rEat[3].c_str(), rEdt[3].c_str(), rPf[3].c_str(), rSta[3].c_str(),
        rNo[4].c_str(), rName[4].c_str(), rEat[4].c_str(), rEdt[4].c_str(), rPf[4].c_str(), rSta[4].c_str(),
        rNo[5].c_str(), rName[5].c_str(), rEat[5].c_str(), rEdt[5].c_str(), rPf[5].c_str(), rSta[5].c_str(),
        rNo[6].c_str(), rName[6].c_str(), rEat[6].c_str(), rEdt[6].c_str(), rPf[6].c_str(), rSta[6].c_str(),
        L"yellow", effect, wBorder.c_str(), intensity, speed, chrHex, wLang.c_str()
    );
    return 0;
  }

  // 3. Default plain text message
  std::wstring t1Param;
  if (wMsg.rfind(L"DEFAULT_MSG:", 0) == 0 || wMsg == L"TOPGRIP") {
    t1Param = wMsg;
  } else {
    t1Param = L"DEFAULT_MSG:" + wMsg;
  }

  Program_dynamic_table_7rows(
      (char*)ip, (int)port,
      t1Param.c_str(), L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"", L"", L"", L"", L"", L"",
      L"yellow", effect, wBorder.c_str(), intensity, speed, chrHex, wLang.c_str()
  );

  return 0;
}
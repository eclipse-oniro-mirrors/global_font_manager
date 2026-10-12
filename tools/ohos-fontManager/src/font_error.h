/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_FONT_MANAGER_CLI_FONT_ERROR_H
#define OHOS_FONT_MANAGER_CLI_FONT_ERROR_H

#include <cstdint>
#include <string>

namespace OhosFontManager {

// Font manager error codes (mirrors font_define.h FontErrorCode enum)
// Only includes codes that can actually be returned by InstallFont/UninstallFont
constexpr int32_t FM_ERR_OK = 0;
constexpr int32_t FM_ERR_NO_PERMISSION = 201;
constexpr int32_t FM_ERR_FILE_NOT_EXISTS = 31100101;
constexpr int32_t FM_ERR_FILE_VERIFY_FAIL = 31100102;
constexpr int32_t FM_ERR_COPY_FAIL = 31100103;
constexpr int32_t FM_ERR_INSTALLED_ALREADY = 31100104;
constexpr int32_t FM_ERR_MAX_FILE_COUNT = 31100105;
constexpr int32_t FM_ERR_INSTALL_FAIL = 31100106;
constexpr int32_t FM_ERR_UNINSTALL_FILE_NOT_EXISTS = 31100107;
constexpr int32_t FM_ERR_UNINSTALL_REMOVE_FAIL = 31100108;
constexpr int32_t FM_ERR_UNINSTALL_FAIL = 31100109;

struct ErrorInfo {
    const char* errCode;
    std::string errMsg;
    std::string suggestion;
};

// Map a font_manager business error code to a CLI-standard error response.
ErrorInfo MapFontError(int32_t code);

}  // namespace OhosFontManager

#endif  // OHOS_FONT_MANAGER_CLI_FONT_ERROR_H

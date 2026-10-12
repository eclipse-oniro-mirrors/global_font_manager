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

#include "font_error.h"

#include <cstring>

namespace OhosFontManager {

struct ErrorEntry {
    int32_t code;
    const char* errCode;
    const char* errMsg;
    const char* suggestion;
};

const ErrorEntry gErrorTable[] = {
    {FM_ERR_NO_PERMISSION, "ERR_PERMISSION_DENIED",
     "Permission verification failed. The application does not "
     "have the permission required to call the API.",
     "Add ohos.permission.UPDATE_FONT in module.json5 and ensure granted"},
    {FM_ERR_FILE_NOT_EXISTS, "ERR_RESOURCE_NOT_FOUND",
     "The font does not exist.",
     "Verify the font file path is correct and the file is readable"},
    {FM_ERR_FILE_VERIFY_FAIL, "ERR_FM_FILE_VERIFY_FAIL",
     "The font is not supported.",
     "Use a valid font file format such as .ttf or .otf"},
    {FM_ERR_COPY_FAIL, "ERR_FM_COPY_FAIL",
     "Failed to copy the font file.",
     "Check storage space and write permissions on font directory"},
    {FM_ERR_INSTALLED_ALREADY, "ERR_RESOURCE_EXISTS",
     "The font file is installed.",
     "Use 'ohos-fontManager uninstall --font-name <name>' to remove first"},
    {FM_ERR_MAX_FILE_COUNT, "ERR_FM_MAX_COUNT",
     "Exceeded the maximum number of installed files.",
     "Uninstall unused fonts before installing new ones"},
    {FM_ERR_INSTALL_FAIL, "ERR_FM_INSTALL_FAIL",
     "The system ability works abnormally.",
     "Check if font_manager service (SA ID 66262) is running and retry"},
    {FM_ERR_UNINSTALL_FILE_NOT_EXISTS, "ERR_RESOURCE_NOT_FOUND",
     "The font file does not exist.",
     "Verify the font name is correct and the font is installed"},
    {FM_ERR_UNINSTALL_REMOVE_FAIL, "ERR_FM_REMOVE_FAIL",
     "Failed to delete the font file.",
     "Check file permissions on the font directory and retry"},
    {FM_ERR_UNINSTALL_FAIL, "ERR_FM_UNINSTALL_FAIL",
     "The system ability works abnormally.",
     "Check if font_manager service (SA ID 66262) is running and retry"},
};

ErrorInfo MapFontError(int32_t code)
{
    for (const auto& entry : gErrorTable) {
        if (entry.code == code) {
            return {entry.errCode, entry.errMsg, entry.suggestion};
        }
    }
    return {"ERR_INTERNAL_ERROR",
        "Unknown error code: " + std::to_string(code),
        "Contact system administrator for support"};
}

}  // namespace OhosFontManager

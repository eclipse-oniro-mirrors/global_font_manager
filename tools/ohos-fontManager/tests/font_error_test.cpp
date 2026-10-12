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

#include <gtest/gtest.h>
#include <string>
#include "font_error.h"

using namespace testing::ext;
using namespace OhosFontManager;

class FontErrorTest : public testing::Test {};

HWTEST_F(FontErrorTest, MapFontError_NoPermission, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_NO_PERMISSION);
    EXPECT_STREQ(info.errCode, "ERR_PERMISSION_DENIED");
    EXPECT_NE(info.errMsg.find("Permission verification failed"),
        std::string::npos);
}

HWTEST_F(FontErrorTest, MapFontError_FileNotExists, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_FILE_NOT_EXISTS);
    EXPECT_STREQ(info.errCode, "ERR_RESOURCE_NOT_FOUND");
    EXPECT_EQ(info.errMsg, "The font does not exist.");
}

HWTEST_F(FontErrorTest, MapFontError_FileVerifyFail, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_FILE_VERIFY_FAIL);
    EXPECT_STREQ(info.errCode, "ERR_FM_FILE_VERIFY_FAIL");
    EXPECT_EQ(info.errMsg, "The font is not supported.");
}

HWTEST_F(FontErrorTest, MapFontError_CopyFail, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_COPY_FAIL);
    EXPECT_STREQ(info.errCode, "ERR_FM_COPY_FAIL");
    EXPECT_EQ(info.errMsg, "Failed to copy the font file.");
}

HWTEST_F(FontErrorTest, MapFontError_InstalledAlready, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_INSTALLED_ALREADY);
    EXPECT_STREQ(info.errCode, "ERR_RESOURCE_EXISTS");
    EXPECT_EQ(info.errMsg, "The font file is installed.");
}

HWTEST_F(FontErrorTest, MapFontError_MaxFileCount, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_MAX_FILE_COUNT);
    EXPECT_STREQ(info.errCode, "ERR_FM_MAX_COUNT");
    EXPECT_EQ(info.errMsg, "Exceeded the maximum number of installed files.");
}

HWTEST_F(FontErrorTest, MapFontError_InstallFail, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_INSTALL_FAIL);
    EXPECT_STREQ(info.errCode, "ERR_FM_INSTALL_FAIL");
    EXPECT_EQ(info.errMsg, "The system ability works abnormally.");
}

HWTEST_F(FontErrorTest, MapFontError_UninstallFileNotExists, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_UNINSTALL_FILE_NOT_EXISTS);
    EXPECT_STREQ(info.errCode, "ERR_RESOURCE_NOT_FOUND");
    EXPECT_EQ(info.errMsg, "The font file does not exist.");
}

HWTEST_F(FontErrorTest, MapFontError_UninstallRemoveFail, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_UNINSTALL_REMOVE_FAIL);
    EXPECT_STREQ(info.errCode, "ERR_FM_REMOVE_FAIL");
    EXPECT_EQ(info.errMsg, "Failed to delete the font file.");
}

HWTEST_F(FontErrorTest, MapFontError_UninstallFail, TestSize.Level1)
{
    auto info = MapFontError(FM_ERR_UNINSTALL_FAIL);
    EXPECT_STREQ(info.errCode, "ERR_FM_UNINSTALL_FAIL");
    EXPECT_EQ(info.errMsg, "The system ability works abnormally.");
}

HWTEST_F(FontErrorTest, MapFontError_UnknownCode, TestSize.Level1)
{
    auto info = MapFontError(99999);
    EXPECT_STREQ(info.errCode, "ERR_INTERNAL_ERROR");
    EXPECT_NE(info.errMsg.find("Unknown error code"), std::string::npos);
    EXPECT_NE(info.errMsg.find("99999"), std::string::npos);
}

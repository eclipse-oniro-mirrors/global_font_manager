/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <fcntl.h>
#include <string>

#include "storage_manager_adapter.h"
#include "font_manager_utils.h"
#include "font_define.h"
#include "parameters.h"

namespace {
const std::string INSTALL_PATH_TEST = "/data/service/el1/100/for-all-app/fonts/";
constexpr int32_t TEST_USERID = 100;
const std::string FONT_PATH = "/data/test/TestFont_Sans.ttf";
}

using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace FontManager {

class StorageManagerAdapterTest : public testing::Test {
public:
    StorageManagerAdapterTest(){};
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<StorageManagerAdapter> adapter_;
};

void StorageManagerAdapterTest::SetUpTestCase(void)
{
    FontManagerUtils::DeleteDir(INSTALL_PATH_TEST, false);
}

void StorageManagerAdapterTest::TearDownTestCase(void)
{
}

void StorageManagerAdapterTest::SetUp(void)
{
    adapter_ = StorageManagerAdapter::GetInstance();
}

void StorageManagerAdapterTest::TearDown(void)
{
    FontManagerUtils::DeleteDir(INSTALL_PATH_TEST, false);
}

/**
 * @tc.name: StorageManagerAdapterFuncTest001
 * @tc.desc: Test ReportFontBundleStats proxy is null
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest001, TestSize.Level1)
{
    int32_t ret = adapter_->ReportFontBundleStats(TEST_USERID, INSTALL_PATH_TEST);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.name: StorageManagerAdapterFuncTest002
 * @tc.desc: Test ReportFontBundleStats businessName is empty
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest002, TestSize.Level1)
{
    std::string businessName = adapter_->GetFontInstallerBusinessName();
    if (businessName.empty()) {
        int32_t ret = adapter_->ReportFontBundleStats(TEST_USERID, INSTALL_PATH_TEST);
        EXPECT_EQ(ret, ERR_INVALID_PARAM);
    }
}

/**
 * @tc.name: StorageManagerAdapterFuncTest003
 * @tc.desc: Test GetFontFolderSize with existing directory
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest003, TestSize.Level1)
{
    ASSERT_EQ(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST), true);
    int fd = open(FONT_PATH.c_str(), O_RDONLY);
    ASSERT_GE(fd, 0);
    std::string destPath = INSTALL_PATH_TEST + "TestFont_Sans.ttf";
    ASSERT_TRUE(FontManagerUtils::CopyFile(fd, destPath));
    if (fd >= 0) {
        close(fd);
    }
    uint64_t size = adapter_->GetFontFolderSize(INSTALL_PATH_TEST);
    EXPECT_GT(size, 0);
}

/**
 * @tc.name: StorageManagerAdapterFuncTest004
 * @tc.desc: Test GetFontFolderSize with empty directory
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest004, TestSize.Level1)
{
    ASSERT_EQ(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST), true);
    uint64_t size = adapter_->GetFontFolderSize(INSTALL_PATH_TEST);
    EXPECT_GT(size, 0);
}

/**
 * @tc.name: StorageManagerAdapterFuncTest005
 * @tc.desc: Test GetFontFolderSize with non-existent directory
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest005, TestSize.Level1)
{
    uint64_t size = adapter_->GetFontFolderSize("/data/service/el1/999/for-all-app/fonts/");
    EXPECT_EQ(size, 0);
    uint64_t size2 = adapter_->GetFontFolderSize("/data/test/nonexistent_dir_12345/");
    EXPECT_EQ(size2, 0);
}

/**
 * @tc.name: StorageManagerAdapterFuncTest006
 * @tc.desc: Test ReportFontBundleStats with valid path after install, proxy unavailable
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest006, TestSize.Level1)
{
    ASSERT_EQ(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST), true);
    int fd = open(FONT_PATH.c_str(), O_RDONLY);
    ASSERT_GE(fd, 0);
    std::string destPath = INSTALL_PATH_TEST + "TestFont_Sans.ttf";
    ASSERT_TRUE(FontManagerUtils::CopyFile(fd, destPath));
    if (fd >= 0) {
        close(fd);
    }
    uint32_t size = adapter_->GetFontFolderSize(INSTALL_PATH_TEST);
    EXPECT_GT(size, 0);
    int32_t ret = adapter_->ReportFontBundleStats(TEST_USERID, INSTALL_PATH_TEST);
    std::string businessName = OHOS::system::GetParameter(EXT_STORAGE_BUNDLE_PARAM_KEY, "");
    if (businessName.empty()) {
        EXPECT_EQ(ret, ERR_INVALID_PARAM);
    } else {
        EXPECT_EQ(ret, 13600001);
    }
}

/**
 * @tc.name: StorageManagerAdapterFuncTest007
 * @tc.desc: Test GetFontInstallerBusinessName consistency with ReportFontBundleStats
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest007, TestSize.Level1)
{
    std::string businessName = adapter_->GetFontInstallerBusinessName();
    // businessName comes from system parameter const.fontmanager.extstoragebundle.
    // Value varies by device configuration. Verify consistency: when businessName is empty,
    // ReportFontBundleStats must return ERR_INVALID_PARAM; when non-empty, it must not.
    int32_t ret = adapter_->ReportFontBundleStats(TEST_USERID, INSTALL_PATH_TEST);
    if (businessName.empty()) {
        EXPECT_EQ(ret, ERR_INVALID_PARAM);
    } else {
        EXPECT_NE(ret, ERR_INVALID_PARAM);
    }
}

/**
 * @tc.name: StorageManagerAdapterFuncTest008
 * @tc.desc: Test GetStorageManagerProxy consistency with ReportFontBundleStats
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest008, TestSize.Level1)
{
    auto proxy = adapter_->GetStorageManagerProxy();
    std::string businessName = adapter_->GetFontInstallerBusinessName();
    int32_t ret = adapter_->ReportFontBundleStats(TEST_USERID, INSTALL_PATH_TEST);
    // Verify consistency between proxy availability, businessName, and result
    if (businessName.empty()) {
        EXPECT_EQ(ret, ERR_INVALID_PARAM);
    } else if (proxy == nullptr) {
        EXPECT_EQ(ret, ERR_SYSTEM_ERROR);
    } else {
        EXPECT_NE(ret, ERR_INVALID_PARAM);
        EXPECT_NE(ret, ERR_SYSTEM_ERROR);
    }
}

/**
 * @tc.name: StorageManagerAdapterFuncTest009
 * @tc.desc: Test ReportFontBundleStats with empty install path
 * @tc.type: FUNC
 */
HWTEST_F(StorageManagerAdapterTest, StorageManagerAdapterFuncTest009, TestSize.Level1)
{
    std::string businessName = adapter_->GetFontInstallerBusinessName();
    int32_t ret = adapter_->ReportFontBundleStats(TEST_USERID, "");
    // businessName varies by device. When empty, ReportFontBundleStats returns ERR_INVALID_PARAM.
    // When non-empty, it proceeds to proxy/IPC path (ret depends on SA availability).
    if (businessName.empty()) {
        EXPECT_EQ(ret, ERR_INVALID_PARAM);
    } else {
        EXPECT_NE(ret, ERR_INVALID_PARAM);
    }
}

} // namespace FontManager
} // namespace Global
} // namespace OHOS
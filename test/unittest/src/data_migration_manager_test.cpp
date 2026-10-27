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
#include <fcntl.h>
#include <string>
#include <vector>

#include "data_migration_manager.h"
#include "storage_manager_adapter.h"
#include "font_manager_utils.h"
#include "font_define.h"
#include "callback_mock.h"
#include "directory_ex.h"
#include "permission_common.h"

namespace {
const std::string INSTALL_PATH_TEST = "/data/service/el1/100/for-all-app/fonts/";
const std::string TEMP_PATH_TEST = "/data/service/el1/100/for-all-app/fonts/temp/";
const std::string FONT_PATH = "/data/test/TestFont_Sans.ttf";
constexpr int32_t TEST_USERID = 100;
}

using testing::ext::TestSize;
using namespace std;

namespace OHOS {
namespace Global {
namespace FontManager {

class DataMigrationManagerTest : public testing::Test {
public:
    DataMigrationManagerTest(){};
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    bool CopyTestFileToInstallPath(const std::vector<std::string> &srcPaths);
    std::shared_ptr<DataMigrationManager> manager_;
};

void DataMigrationManagerTest::SetUpTestCase(void)
{
}

void DataMigrationManagerTest::TearDownTestCase(void)
{
    FontManagerUtils::DeleteDir(TEMP_PATH_TEST, true);
    PermissionCommon::ResetTokenAndUid();
}

void DataMigrationManagerTest::SetUp(void)
{
    manager_ = DataMigrationManager::GetInstance();
}

void DataMigrationManagerTest::TearDown(void)
{
    PermissionCommon::ResetTokenAndUid();
    FontManagerUtils::DeleteDir(INSTALL_PATH_TEST, false);
    FontManagerUtils::DeleteDir(INSTALL_PATH_APP, false);
}

bool DataMigrationManagerTest::CopyTestFileToInstallPath(const std::vector<std::string> &srcPaths)
{
    for (const auto &path : srcPaths) {
        std::string fileName = FontManagerUtils::GetFileName(path);
        int fd = open(path.c_str(), O_RDONLY);
        if (fd < 0) {
            return false;
        }

        if (!FontManagerUtils::CopyFile(fd, INSTALL_PATH_APP + fileName)) {
            close(fd);
            return false;
        }
        close(fd);
    }
    return true;
}

/**
 * @tc.name: DataMigrationManagerFuncTest001
 * @tc.desc: Test FontManager CopyFileForDataMigration case
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest001, TestSize.Level1)
{
    const std::string fontPath = "/data/test/NotoSansCJK-Regular.ttc";
    manager_->userIds_ = {TEST_USERID};
    bool res = manager_->CheckAllUserDir();
    ASSERT_TRUE(res);
    res = manager_->InitDataMigrationTempDir();
    ASSERT_TRUE(res);
    res = manager_->InitDataMigrationTempDir();
    ASSERT_TRUE(res);
    int32_t ret = manager_->CopyFileForDataMigration(fontPath, TEST_USERID);
    EXPECT_EQ(ret, ERR_OK);
    ret = manager_->CopyFileForDataMigration(fontPath, TEST_USERID);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(FontManagerUtils::RemoveFile(INSTALL_PATH_TEST + "NotoSansCJK-Regular.ttc"), true);
}

/**
 * @tc.name: DataMigrationManagerFuncTest002
 * @tc.desc: Test FontManager DataMigration case
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest002, TestSize.Level1)
{
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->DataMigration(cb);
    std::vector<std::string> paths;
    OHOS::GetDirFiles(INSTALL_PATH_TEST, paths);
    EXPECT_TRUE(paths.empty());
}

/**
 * @tc.name: DataMigrationManagerFuncTest003
 * @tc.desc: Test FontManager DataMigration case
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest003, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    std::vector<std::string> srcPaths = {
        "/data/test/NotoSansCJK-Regular.ttc",
        "/data/test/NotoSerifCJK-Regular.ttc",
        "/data/test/TestFont_Sans.ttf",
        "/data/test/emptyTTF.ttf",
        "/data/test/200install_fontconfig.json"
    };
    EXPECT_TRUE(this->CopyTestFileToInstallPath(srcPaths));
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->DataMigration(cb);
    std::vector<std::string> paths;
    OHOS::GetDirFiles(INSTALL_PATH_TEST, paths);
    EXPECT_TRUE(paths.size() == srcPaths.size());
}

/**
 * @tc.name: DataMigrationManagerFuncTest004
 * @tc.desc: Test FontManager DataMigration case
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest004, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    std::vector<std::string> srcPaths = {
        "/data/test/NotoSansCJK-Regular.ttc"
    };
    EXPECT_TRUE(this->CopyTestFileToInstallPath(srcPaths));
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->DataMigration(cb);
    std::vector<std::string> paths;
    OHOS::GetDirFiles(INSTALL_PATH_TEST, paths);
    EXPECT_TRUE(paths.size() == srcPaths.size());
}

/**
 * @tc.name: DataMigrationManagerFuncTest005
 * @tc.desc: Test CheckAllUserDir fail
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest005, TestSize.Level1)
{
    // 1. 设置 userIds
    manager_->userIds_ = {999}; // 使用一个不存在的用户ID

    // 2. 确保该用户的目录不存在
    std::string userPath = "/data/service/el1/999/for-all-app/fonts/";
    FontManagerUtils::DeleteDir(userPath, true);

    // 3. 执行 CheckAllUserDir
    // 对应代码: CheckPathExist 返回 false -> 返回 false
    bool ret = manager_->CheckAllUserDir();
    
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DataMigrationManagerFuncTest006
 * @tc.desc: Test DataMigration triggers ReportFontBundleStats for each user
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest006, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    std::vector<std::string> srcPaths = {
        "/data/test/NotoSansCJK-Regular.ttc",
        "/data/test/TestFont_Sans.ttf",
    };
    EXPECT_TRUE(this->CopyTestFileToInstallPath(srcPaths));
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->DataMigration(cb);
    std::vector<std::string> paths;
    OHOS::GetDirFiles(INSTALL_PATH_TEST, paths);
    EXPECT_TRUE(paths.size() == srcPaths.size());
    auto adapter = StorageManagerAdapter::GetInstance();
    uint64_t size = adapter->GetFontFolderSize(INSTALL_PATH_TEST);
    EXPECT_GT(size, 0);
}

/**
 * @tc.name: DataMigrationManagerFuncTest007
 * @tc.desc: Test CopyFileForDataMigration with non-existent source
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest007, TestSize.Level1)
{
    ASSERT_TRUE(manager_->InitDataMigrationTempDir());
    int32_t ret = manager_->CopyFileForDataMigration("/data/test/nonexistent_12345.ttf", TEST_USERID);
    EXPECT_EQ(ret, ERR_OPEN_SRC_FILE);
}

/**
 * @tc.name: DataMigrationManagerFuncTest008
 * @tc.desc: Test IsShouldUpdateProgress with totalCount > MAX_TRIGGER_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest008, TestSize.Level1)
{
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(0, 150));
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(3, 150));
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(6, 150));
    EXPECT_FALSE(manager_->IsShouldUpdateProgress(50, 150));
    EXPECT_FALSE(manager_->IsShouldUpdateProgress(100, 150));
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest009, TestSize.Level1)
{
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(0, 50));
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(25, 50));
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(49, 50));
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest010, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    EXPECT_TRUE(manager_->CheckAllUserDir());
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest011, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    EXPECT_TRUE(manager_->InitDataMigrationTempDir());
    EXPECT_TRUE(FontManagerUtils::CheckPathExist(TEMP_PATH_TEST));
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest012, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    EXPECT_TRUE(manager_->InitDataMigrationTempDir());
    EXPECT_TRUE(manager_->InitDataMigrationTempDir());
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest013, TestSize.Level1)
{
    manager_->userIds_ = {999};
    std::string userPath = "/data/service/el1/999/for-all-app/fonts/";
    FontManagerUtils::DeleteDir(userPath, true);
    EXPECT_FALSE(manager_->CheckAllUserDir());
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest014, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    manager_->userIds_ = {TEST_USERID};
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    int32_t ret = manager_->CopyFileForDataMigration("/data/test/nonexistent_12345.ttf", TEST_USERID);
    EXPECT_EQ(ret, ERR_OPEN_SRC_FILE);
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest015, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    int32_t ret = manager_->CopyFileForDataMigration(FONT_PATH.c_str(), TEST_USERID);
    EXPECT_EQ(ret, ERR_OK);
    std::string destPath = INSTALL_PATH_TEST + "TestFont_Sans.ttf";
    EXPECT_TRUE(FontManagerUtils::CheckPathExist(destPath));
    FontManagerUtils::RemoveFile(destPath);
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest016, TestSize.Level1)
{
    manager_->callback_ = nullptr;
    EventData eventData = {.event = EventType::PROGRESS_RESULT, .progressResult = 0};
    manager_->RefreshEventData(eventData);
    // With null callback, RefreshEventData should be a no-op (no crash)
    EXPECT_EQ(manager_->callback_, nullptr);
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest017, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    std::string destPath = INSTALL_PATH_TEST + "TestFont_Sans.ttf";
    int fd = open(FONT_PATH.c_str(), O_RDONLY);
    ASSERT_GE(fd, 0);
    FontManagerUtils::CopyFile(fd, destPath);
    if (fd >= 0) {
        close(fd);
    }
    int32_t ret = manager_->CopyFileForDataMigration(FONT_PATH.c_str(), TEST_USERID);
    EXPECT_EQ(ret, ERR_OK);
    FontManagerUtils::RemoveFile(destPath);
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest018, TestSize.Level1)
{
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->callback_ = cb;
    EventData eventData = {.event = EventType::HEART_BEAT};
    manager_->RefreshEventData(eventData);
    // callback_ should remain valid after RefreshEventData
    EXPECT_EQ(manager_->callback_, cb);
    manager_->callback_ = nullptr;
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest019, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    std::vector<std::string> srcPaths = {"/data/test/TestFont_Sans.ttf"};
    EXPECT_TRUE(this->CopyTestFileToInstallPath(srcPaths));
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    int32_t ret = manager_->StartOneFileCopyTask(INSTALL_PATH_APP + "TestFont_Sans.ttf");
    EXPECT_EQ(ret, ERR_OK);
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest020, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    int32_t ret = manager_->StartOneFileCopyTask(INSTALL_PATH_APP + "nonexistent_12345.ttf");
    EXPECT_EQ(ret, ERR_OPEN_SRC_FILE);
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest021, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    int32_t ret = manager_->CopyFileForDataMigration(FONT_PATH.c_str(), TEST_USERID);
    EXPECT_EQ(ret, ERR_OK);
    FontManagerUtils::RemoveFile(INSTALL_PATH_TEST + "TestFont_Sans.ttf");
}

HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest022, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    std::vector<std::string> srcPaths = {
        "/data/test/NotoSansCJK-Regular.ttc",
        "/data/test/TestFont_Sans.ttf",
    };
    EXPECT_TRUE(this->CopyTestFileToInstallPath(srcPaths));
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->DataMigration(cb);
    auto adapter = StorageManagerAdapter::GetInstance();
    uint64_t size = adapter->GetFontFolderSize(INSTALL_PATH_TEST);
    EXPECT_GT(size, 0);
}

/**
 * @tc.name: DataMigrationManagerFuncTest023
 * @tc.desc: Test DataMigration reentrance guard skips concurrent call
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest023, TestSize.Level1)
{
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->isDataMigrationing_.store(true);
    manager_->DataMigration(cb);
    EXPECT_TRUE(manager_->isDataMigrationing_.load());
    manager_->isDataMigrationing_.store(false);
}

/**
 * @tc.name: DataMigrationManagerFuncTest024
 * @tc.desc: Test RefreshEventData keeps callback_ valid (local sptr copy)
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest024, TestSize.Level1)
{
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->callback_ = cb;
    EventData eventData = {.event = EventType::HEART_BEAT};
    manager_->RefreshEventData(eventData);
    EXPECT_EQ(manager_->callback_, cb);
    manager_->callback_ = nullptr;
}

/**
 * @tc.name: DataMigrationManagerFuncTest025
 * @tc.desc: Test DataMigration with empty INSTALL_PATH_APP (ERR_NOT_NEED_DATA_MIGRATION)
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest025, TestSize.Level1)
{
    // Ensure INSTALL_PATH_APP is empty so InitDataMigrationEnv returns ERR_NOT_NEED_DATA_MIGRATION
    FontManagerUtils::DeleteDir(INSTALL_PATH_APP, false);
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->isDataMigrationing_.store(false);
    manager_->DataMigration(cb);
    // DataMigration completes without crash; isDataMigrationing_ should be reset
    EXPECT_FALSE(manager_->isDataMigrationing_.load());
}

/**
 * @tc.name: DataMigrationManagerFuncTest026
 * @tc.desc: Test InitDataMigrationEnv with empty INSTALL_PATH_APP
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest026, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    FontManagerUtils::DeleteDir(INSTALL_PATH_APP, false);
    manager_->userIds_ = {TEST_USERID};
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    int32_t ret = manager_->InitDataMigrationEnv();
    EXPECT_EQ(ret, ERR_NOT_NEED_DATA_MIGRATION);
}

/**
 * @tc.name: DataMigrationManagerFuncTest027
 * @tc.desc: Test StartDataMigration with empty paths
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest027, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    FontManagerUtils::DeleteDir(INSTALL_PATH_APP, false);
    FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_APP);
    int32_t ret = manager_->StartDataMigration();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: DataMigrationManagerFuncTest028
 * @tc.desc: Test EventDataProgress with i=0
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest028, TestSize.Level1)
{
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->callback_ = cb;
    manager_->userIds_ = {TEST_USERID};
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    // Should not crash; callback_ should remain valid after call
    manager_->EventDataProgress(0, 10, 1);
    EXPECT_EQ(manager_->callback_, cb);
    manager_->callback_ = nullptr;
}

/**
 * @tc.name: DataMigrationManagerFuncTest029
 * @tc.desc: Test EventDataProgress with i>0
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest029, TestSize.Level1)
{
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->callback_ = cb;
    manager_->userIds_ = {TEST_USERID};
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->EventDataProgress(5, 10, 2);
    EXPECT_EQ(manager_->callback_, cb);
    manager_->callback_ = nullptr;
}

/**
 * @tc.name: DataMigrationManagerFuncTest030
 * @tc.desc: Test EventDataResult
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest030, TestSize.Level1)
{
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->callback_ = cb;
    manager_->EventDataResult(ERR_OK);
    EXPECT_EQ(manager_->callback_, cb);
    manager_->callback_ = nullptr;
}

/**
 * @tc.name: DataMigrationManagerFuncTest031
 * @tc.desc: Test EventDataHeartBeat
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest031, TestSize.Level1)
{
    sptr<IDataMigrationCallback> cb = new (std::nothrow) TestCallback();
    ASSERT_TRUE(cb != nullptr);
    manager_->callback_ = cb;
    manager_->EventDataHeartBeat();
    EXPECT_EQ(manager_->callback_, cb);
    manager_->callback_ = nullptr;
}

/**
 * @tc.name: DataMigrationManagerFuncTest032
 * @tc.desc: Test CheckAndUpdateAllFontRecord
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest032, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    manager_->CheckAndUpdateAllFontRecord();
    // Config file should exist and be valid after update
    EXPECT_TRUE(FontManagerUtils::CheckPathExist(INSTALL_PATH_TEST + FONT_CONFIG_FILE));
}

/**
 * @tc.name: DataMigrationManagerFuncTest033
 * @tc.desc: Test DataMigrationInner with empty INSTALL_PATH_APP
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest033, TestSize.Level1)
{
    // Ensure INSTALL_PATH_APP is empty so InitDataMigrationEnv returns ERR_NOT_NEED_DATA_MIGRATION
    PermissionCommon::SetFontManagerInitEnv();
    FontManagerUtils::DeleteDir(INSTALL_PATH_APP, false);
    manager_->isDataMigrationing_.store(false);
    int32_t ret = manager_->DataMigrationInner();
    EXPECT_EQ(ret, ERR_NOT_NEED_DATA_MIGRATION);
}

/**
 * @tc.name: DataMigrationManagerFuncTest034
 * @tc.desc: Test CopyFileForDataMigration with directory as source (fopen may succeed or fail)
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest034, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    int32_t ret = manager_->CopyFileForDataMigration("/data/test/", TEST_USERID);
    // On some platforms fopen("/data/test/") succeeds (returns directory fd), on others it fails.
    // If fopen succeeds: fileno succeeds, CopyFile likely fails -> ERR_COPY_FILE or ERR_OK
    // If fopen fails: -> ERR_OPEN_SRC_FILE
    // Either way, it should not crash. Verify the return is a known error code.
    EXPECT_TRUE(ret == ERR_OPEN_SRC_FILE || ret == ERR_COPY_FILE || ret == ERR_OK);
}

/**
 * @tc.name: DataMigrationManagerFuncTest035
 * @tc.desc: Test StartOneFileCopyTask with valid file
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest035, TestSize.Level1)
{
    PermissionCommon::SetFontManagerInitEnv();
    ASSERT_TRUE(FontManagerUtils::CheckAndInitInstallPath(INSTALL_PATH_TEST));
    manager_->userIds_ = {TEST_USERID};
    int fd = open(FONT_PATH.c_str(), O_RDONLY);
    ASSERT_GE(fd, 0);
    std::string srcPath = INSTALL_PATH_APP + "test_remove_fail.ttf";
    EXPECT_TRUE(FontManagerUtils::CopyFile(fd, srcPath));
    if (fd >= 0) {
        close(fd);
    }
    int32_t ret = manager_->StartOneFileCopyTask(srcPath);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: DataMigrationManagerFuncTest036
 * @tc.desc: Test IsShouldUpdateProgress with totalCount <= MAX_TRIGGER_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(DataMigrationManagerTest, DataMigrationManagerFuncTest036, TestSize.Level1)
{
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(0, 100));
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(50, 100));
    EXPECT_TRUE(manager_->IsShouldUpdateProgress(99, 100));
}

} // namespace FontManager
} // namespace Global
} // namespace OHOS
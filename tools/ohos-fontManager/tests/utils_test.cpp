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
#include <cJSON.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "utils.h"

using namespace testing::ext;

static std::string CaptureStdout(std::function<void()> func)
{
    int pipefd[2];
    pipe(pipefd);
    int savedStdout = dup(STDOUT_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    func();

    fflush(stdout);
    dup2(savedStdout, STDOUT_FILENO);
    close(savedStdout);

    char buf[4096] = {0};
    read(pipefd[0], buf, sizeof(buf) - 1);
    close(pipefd[0]);
    return std::string(buf);
}

static cJSON* ParseJson(const std::string& str)
{
    cJSON* root = cJSON_Parse(str.c_str());
    return root;
}

class UtilsTest : public testing::Test {};

HWTEST_F(UtilsTest, OutputSuccess_ValidJson, TestSize.Level1)
{
    cJSON* data = cJSON_CreateObject();
    cJSON_AddStringToObject(data, "message", "Font installed successfully");
    cJSON_AddStringToObject(data, "fontPath", "/data/local/tmp/custom.ttf");

    std::string output = CaptureStdout([&]() {
        OutputSuccess(data);
    });

    cJSON* root = ParseJson(output);
    ASSERT_NE(root, nullptr);
    EXPECT_STREQ(cJSON_GetObjectItem(root, "type")->valuestring, "result");
    EXPECT_STREQ(cJSON_GetObjectItem(root, "status")->valuestring, "success");
    cJSON* dataItem = cJSON_GetObjectItem(root, "data");
    ASSERT_NE(dataItem, nullptr);
    EXPECT_STREQ(cJSON_GetObjectItem(dataItem, "message")->valuestring,
        "Font installed successfully");
    EXPECT_STREQ(cJSON_GetObjectItem(dataItem, "fontPath")->valuestring,
        "/data/local/tmp/custom.ttf");
    EXPECT_EQ(cJSON_GetObjectItem(root, "errCode"), nullptr);
    cJSON_Delete(root);
}

HWTEST_F(UtilsTest, OutputError_ValidJson, TestSize.Level1)
{
    std::string output = CaptureStdout([&]() {
        OutputError("ERR_PERMISSION_DENIED",
            "Permission verification failed.",
            "Add ohos.permission.UPDATE_FONT permission");
    });

    cJSON* root = ParseJson(output);
    ASSERT_NE(root, nullptr);
    EXPECT_STREQ(cJSON_GetObjectItem(root, "type")->valuestring, "result");
    EXPECT_STREQ(cJSON_GetObjectItem(root, "status")->valuestring, "failed");
    EXPECT_STREQ(cJSON_GetObjectItem(root, "errCode")->valuestring,
        "ERR_PERMISSION_DENIED");
    EXPECT_STREQ(cJSON_GetObjectItem(root, "errMsg")->valuestring,
        "Permission verification failed.");
    EXPECT_STREQ(cJSON_GetObjectItem(root, "suggestion")->valuestring,
        "Add ohos.permission.UPDATE_FONT permission");
    EXPECT_EQ(cJSON_GetObjectItem(root, "data"), nullptr);
    cJSON_Delete(root);
}

HWTEST_F(UtilsTest, OutputError_EmptyFields, TestSize.Level1)
{
    std::string output = CaptureStdout([&]() {
        OutputError("", "", "");
    });

    cJSON* root = ParseJson(output);
    ASSERT_NE(root, nullptr);
    EXPECT_STREQ(cJSON_GetObjectItem(root, "status")->valuestring, "failed");
    EXPECT_TRUE(cJSON_GetObjectItem(root, "errCode") != nullptr);
    EXPECT_EQ(cJSON_GetObjectItem(root, "data"), nullptr);
    cJSON_Delete(root);
}

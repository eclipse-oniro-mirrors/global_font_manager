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
#include <cstring>
#include <string>
#include "param_parser.h"

using namespace testing::ext;
using namespace OhosFontManager;

class ParamParserTest : public testing::Test {};

HWTEST_F(ParamParserTest, ParseStringOption_SpaceFormat, TestSize.Level1)
{
    char arg0[] = "--font-path";
    char arg1[] = "/data/local/tmp/custom.ttf";
    char* argv[] = {arg0, arg1};
    std::string result = ParseStringOption(2, argv, "font-path");
    EXPECT_EQ(result, "/data/local/tmp/custom.ttf");
}

HWTEST_F(ParamParserTest, ParseStringOption_EqualFormat, TestSize.Level1)
{
    char arg0[] = "--font-path=/data/local/tmp/custom.ttf";
    char* argv[] = {arg0};
    std::string result = ParseStringOption(1, argv, "font-path");
    EXPECT_EQ(result, "/data/local/tmp/custom.ttf");
}

HWTEST_F(ParamParserTest, ParseStringOption_NoValue, TestSize.Level1)
{
    char arg0[] = "--font-path";
    char* argv[] = {arg0};
    std::string result = ParseStringOption(1, argv, "font-path");
    EXPECT_TRUE(result.empty());
}

HWTEST_F(ParamParserTest, ParseStringOption_NotFound, TestSize.Level1)
{
    char arg0[] = "--other-param";
    char arg1[] = "value";
    char* argv[] = {arg0, arg1};
    std::string result = ParseStringOption(2, argv, "font-path");
    EXPECT_TRUE(result.empty());
}

HWTEST_F(ParamParserTest, ParseStringOption_EmptyArgv, TestSize.Level1)
{
    char* argv[] = {nullptr};
    std::string result = ParseStringOption(0, argv, "font-path");
    EXPECT_TRUE(result.empty());
}

HWTEST_F(ParamParserTest, ParseStringOption_MultipleOptions, TestSize.Level1)
{
    char arg0[] = "--verbose";
    char arg1[] = "--font-path";
    char arg2[] = "/data/local/tmp/custom.ttf";
    char arg3[] = "--font-name";
    char arg4[] = "CustomFont";
    char* argv[] = {arg0, arg1, arg2, arg3, arg4};
    std::string fontPath = ParseStringOption(5, argv, "font-path");
    EXPECT_EQ(fontPath, "/data/local/tmp/custom.ttf");
    std::string fontName = ParseStringOption(5, argv, "font-name");
    EXPECT_EQ(fontName, "CustomFont");
}

HWTEST_F(ParamParserTest, ParseStringOption_EqualFormatEmptyValue, TestSize.Level1)
{
    char arg0[] = "--font-path=";
    char* argv[] = {arg0};
    std::string result = ParseStringOption(1, argv, "font-path");
    EXPECT_TRUE(result.empty());
}

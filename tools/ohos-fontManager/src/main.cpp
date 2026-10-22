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

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <functional>

#include <cJSON.h>

#include "utils.h"
#include "font_error.h"
#include "param_parser.h"
#include "font_manager_kits.h"

using namespace OhosFontManager;

namespace {
const char* const K_PROGRAM_NAME = "ohos-fontManager";
const char* const K_TOOL_DESCRIPTION =
    "Font management CLI tool for installing and uninstalling system fonts";
constexpr size_t MAX_FONT_PATH_LEN = 4096;
constexpr size_t MAX_FONT_NAME_LEN = 256;
const int ARG_INDEX_SUB_COMMAND = 1;
const int ARG_COUNT_WITH_SUB_COMMAND = 2;
const int ARG_SKIP_COUNT = 2;
}  // namespace

typedef std::function<int(int, char**)> CommandHandler;

struct Command {
    const char* name;
    const char* description;
    const char* usage;
    const char* parameters;
    const char* examples;
    CommandHandler handler;
};

static std::unordered_map<std::string, Command> gCommands;

void RegisterCommand(const std::string& name, const Command& cmd)
{
    gCommands[name] = cmd;
}

void PrintGlobalHelp()
{
    CLI_LOG("%s - %s", K_PROGRAM_NAME, K_TOOL_DESCRIPTION);
    CLI_LOG("");
    CLI_LOG("Usage:");
    CLI_LOG("  %s <command> [options]", K_PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Parameters:");
    CLI_LOG("  --help                  Display this help message");
    CLI_LOG("");
    CLI_LOG("SubCommands:");
    CLI_LOG("  install                 Install a font from a local file");
    CLI_LOG("  uninstall               Uninstall an installed font by name");
    CLI_LOG("");
    CLI_LOG("Examples:");
    CLI_LOG("  %s install --font-path /data/local/tmp/custom.ttf", K_PROGRAM_NAME);
    CLI_LOG("  %s uninstall --font-name CustomFont", K_PROGRAM_NAME);
    CLI_LOG("  %s install --help", K_PROGRAM_NAME);
}

void PrintCommandHelp(const Command& cmd)
{
    CLI_LOG("%s %s - %s", K_PROGRAM_NAME, cmd.name,
        cmd.description ? cmd.description : "N/A");
    if (cmd.usage != nullptr) {
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s", cmd.usage);
    }
    if (cmd.parameters != nullptr) {
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("%s", cmd.parameters);
    }
    CLI_LOG("    %-18s %s", "--help", "Display this help message");
    if (cmd.examples != nullptr) {
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("%s", cmd.examples);
    }
}

int CmdHelp(int argc, char** argv)
{
    std::string targetCmd;
    for (int i = ARG_INDEX_SUB_COMMAND; i < argc; i++) {
        if (argv[i] != nullptr && argv[i][0] != '-') {
            targetCmd = argv[i];
            break;
        }
    }

    if (targetCmd.empty()) {
        PrintGlobalHelp();
        return 0;
    }

    auto it = gCommands.find(targetCmd);
    if (it == gCommands.end()) {
        return OutputError("ERR_ARG_INVALID",
            "Unknown command: " + targetCmd,
            "Run 'ohos-fontManager --help' to see available commands");
    }
    PrintCommandHelp(it->second);
    return 0;
}

int CmdInstall(int argc, char** argv)
{
    std::string fontPath = ParseStringOption(argc, argv, "font-path");
    if (fontPath.empty()) {
        return OutputError("ERR_ARG_MISSING",
            "Missing required parameter: --font-path. "
            "Context: install command requires a font file path",
            "Provide a valid font file path. "
            "Example: ohos-fontManager install --font-path /data/local/tmp/custom.ttf");
    }
    if (fontPath.size() > MAX_FONT_PATH_LEN) {
        return OutputError("ERR_ARG_INVALID",
            "Font path exceeds maximum length (4096)",
            "Provide a shorter file path");
    }

    CLI_LOG("Installing font from path: %s", fontPath.c_str());

    int outValue = 0;
    auto& fontManager = OHOS::Global::FontManager::FontManagerKits::GetInstance();
    int32_t ret = fontManager.InstallFont(fontPath, outValue);
    if (ret != FM_ERR_OK) {
        return OutputError("ERR_INTERNAL_ERROR",
            "Font service IPC call failed with code " + std::to_string(ret),
            "Check if font_manager service (SA ID 66262) is running and retry");
    }

    if (outValue != FM_ERR_OK) {
        ErrorInfo errInfo = MapFontError(outValue);
        return OutputError(errInfo.errCode, errInfo.errMsg, errInfo.suggestion);
    }

    cJSON *data = cJSON_CreateObject();
    cJSON_AddStringToObject(data, "message", "Font installed successfully");
    cJSON_AddStringToObject(data, "fontPath", fontPath.c_str());
    return OutputSuccess(data);
}

int CmdUninstall(int argc, char** argv)
{
    std::string fontName = ParseStringOption(argc, argv, "font-name");
    if (fontName.empty()) {
        return OutputError("ERR_ARG_MISSING",
            "Missing required parameter: --font-name. "
            "Context: uninstall command requires a font name",
            "Provide a valid font name. "
            "Example: ohos-fontManager uninstall --font-name CustomFont");
    }
    if (fontName.size() > MAX_FONT_NAME_LEN) {
        return OutputError("ERR_ARG_INVALID",
            "Font name exceeds maximum length (256)",
            "Provide a shorter font name");
    }

    CLI_LOG("Uninstalling font: %s", fontName.c_str());

    int outValue = 0;
    auto& fontManager = OHOS::Global::FontManager::FontManagerKits::GetInstance();
    int32_t ret = fontManager.UninstallFont(fontName, outValue);
    if (ret != FM_ERR_OK) {
        return OutputError("ERR_INTERNAL_ERROR",
            "Font service IPC call failed with code " + std::to_string(ret),
            "Check if font_manager service (SA ID 66262) is running and retry");
    }

    if (outValue != FM_ERR_OK) {
        ErrorInfo errInfo = MapFontError(outValue);
        return OutputError(errInfo.errCode, errInfo.errMsg, errInfo.suggestion);
    }

    cJSON *data = cJSON_CreateObject();
    cJSON_AddStringToObject(data, "message", "Font uninstalled successfully");
    cJSON_AddStringToObject(data, "fontName", fontName.c_str());
    return OutputSuccess(data);
}

void InitCommands()
{
    Command helpCmd = {"help", "Show this help message",
        "ohos-fontManager help [command]",
        "    command          Optional. Show detailed help for a specific command.",
        "    ohos-fontManager help\n"
        "    ohos-fontManager help install",
        CmdHelp};
    RegisterCommand("help", helpCmd);

    Command installCmd = {"install",
        "Install a font from a local file into the system font directory",
        "ohos-fontManager install --font-path <path>",
        "    --font-path <path>  Required. Font file path to install "
        "(must exist and be readable, e.g., .ttf or .otf)",
        "    # Install a font from local file\n"
        "    ohos-fontManager install --font-path /data/local/tmp/custom.ttf",
        CmdInstall};
    RegisterCommand("install", installCmd);

    Command uninstallCmd = {"uninstall",
        "Uninstall an installed font by name from the system",
        "ohos-fontManager uninstall --font-name <name>",
        "    --font-name <name>  Required. Font name to uninstall "
        "(the installed font name, not a file path)",
        "    # Uninstall a font by name\n"
        "    ohos-fontManager uninstall --font-name CustomFont",
        CmdUninstall};
    RegisterCommand("uninstall", uninstallCmd);
}

int main(int argc, char** argv)
{
    if (argc >= ARG_COUNT_WITH_SUB_COMMAND && strcmp(argv[ARG_INDEX_SUB_COMMAND], "--help") == 0) {
        InitCommands();
        char* helpArgv[ARG_COUNT_WITH_SUB_COMMAND] = {argv[0], nullptr};
        CmdHelp(ARG_INDEX_SUB_COMMAND, helpArgv);
        return 0;
    }

    if (argc < ARG_COUNT_WITH_SUB_COMMAND) {
        return OutputError("ERR_ARG_MISSING", "No command provided",
            "Run 'ohos-fontManager --help' to see available commands");
    }

    InitCommands();

    std::string cmdName = argv[ARG_INDEX_SUB_COMMAND];

    for (int i = ARG_SKIP_COUNT; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            char* helpArgv[ARG_COUNT_WITH_SUB_COMMAND] = {argv[0], const_cast<char*>(cmdName.c_str())};
            CmdHelp(ARG_COUNT_WITH_SUB_COMMAND, helpArgv);
            return 0;
        }
    }

    auto it = gCommands.find(cmdName);
    if (it == gCommands.end()) {
        return OutputError("ERR_UNKNOWN_COMMAND", "Unknown command: " + cmdName,
            "Run 'ohos-fontManager --help' to see available commands");
    }

    return it->second.handler(argc - ARG_SKIP_COUNT, argv + ARG_SKIP_COUNT);
}

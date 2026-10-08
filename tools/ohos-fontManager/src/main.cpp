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
#include "font_manager_kits.h"

namespace {
// Font manager error codes (mirrors font_define.h FontErrorCode enum)
constexpr int32_t FM_ERR_OK = 0;
constexpr int32_t FM_ERR_NO_PERMISSION = 201;
constexpr int32_t FM_ERR_NOT_SYSTEM_APP = 202;
constexpr int32_t FM_ERR_INVALID_PARAM = 401;
constexpr int32_t FM_ERR_FILE_NOT_EXISTS = 31100101;
constexpr int32_t FM_ERR_FILE_VERIFY_FAIL = 31100102;
constexpr int32_t FM_ERR_COPY_FAIL = 31100103;
constexpr int32_t FM_ERR_INSTALLED_ALREADY = 31100104;
constexpr int32_t FM_ERR_MAX_FILE_COUNT = 31100105;
constexpr int32_t FM_ERR_INSTALL_FAIL = 31100106;
constexpr int32_t FM_ERR_UNINSTALL_FILE_NOT_EXISTS = 31100107;
constexpr int32_t FM_ERR_UNINSTALL_REMOVE_FAIL = 31100108;
constexpr int32_t FM_ERR_UNINSTALL_FAIL = 31100109;
constexpr int32_t FM_ERR_SYSTEM_ERROR = 31100110;
constexpr int32_t FM_ERR_DATA_MIGRATIONING = 31100111;

const char* const kProgramName = "ohos-fontManager";
const char* const kToolDescription =
    "Font management CLI tool for installing and uninstalling system fonts";

// Error mapping: font manager error code -> (errCode, errMsg, suggestion)
struct ErrorInfo {
    const char* errCode;
    std::string errMsg;
    std::string suggestion;
};

ErrorInfo MapFontError(int32_t code)
{
    switch (code) {
        case FM_ERR_NO_PERMISSION:
            return {"ERR_PERMISSION_DENIED",
                "Permission denied: ohos.permission.UPDATE_FONT is required to manage fonts",
                "Add ohos.permission.UPDATE_FONT permission in module.json5 and ensure it is granted"};
        case FM_ERR_NOT_SYSTEM_APP:
            return {"ERR_PERMISSION_DENIED",
                "Permission denied: system app privilege is required to manage fonts",
                "Ensure the caller process has system app privilege"};
        case FM_ERR_INVALID_PARAM:
            return {"ERR_ARG_INVALID",
                "Invalid parameter provided to font manager",
                "Check the parameter format and value"};
        case FM_ERR_FILE_NOT_EXISTS:
            return {"ERR_RESOURCE_NOT_FOUND",
                "Font file does not exist or cannot be opened",
                "Verify the font file path is correct and the file is readable"};
        case FM_ERR_FILE_VERIFY_FAIL:
            return {"ERR_FM_FILE_VERIFY_FAIL",
                "Font file verification failed: the file is not a valid font",
                "Use a valid font file format such as .ttf or .otf"};
        case FM_ERR_COPY_FAIL:
            return {"ERR_FM_COPY_FAIL",
                "Font file copy failed during installation",
                "Check available storage space and write permissions on the font directory"};
        case FM_ERR_INSTALLED_ALREADY:
            return {"ERR_RESOURCE_EXISTS",
                "Font is already installed in the system",
                "Use 'ohos-fontManager uninstall --font-name <name>' to remove it first, or skip installation"};
        case FM_ERR_MAX_FILE_COUNT:
            return {"ERR_FM_MAX_COUNT",
                "Maximum font installation count reached (default limit: 200)",
                "Uninstall unused fonts before installing new ones"};
        case FM_ERR_INSTALL_FAIL:
            return {"ERR_FM_INSTALL_FAIL",
                "Font installation failed due to a service error",
                "Check if the font_manager service (SA ID 66262) is running and retry"};
        case FM_ERR_UNINSTALL_FILE_NOT_EXISTS:
            return {"ERR_RESOURCE_NOT_FOUND",
                "Font not found for uninstall: the specified font name is not installed",
                "Verify the font name is correct and the font is currently installed"};
        case FM_ERR_UNINSTALL_REMOVE_FAIL:
            return {"ERR_FM_REMOVE_FAIL",
                "Font file removal failed during uninstall",
                "Check file permissions on the font directory and retry"};
        case FM_ERR_UNINSTALL_FAIL:
            return {"ERR_FM_UNINSTALL_FAIL",
                "Font uninstall failed due to a service error",
                "Check if the font_manager service (SA ID 66262) is running and retry"};
        case FM_ERR_SYSTEM_ERROR:
            return {"ERR_INTERNAL_ERROR",
                "System error occurred during font operation",
                "Retry the operation; if the problem persists, contact system administrator"};
        case FM_ERR_DATA_MIGRATIONING:
            return {"ERR_FM_MIGRATING",
                "Data migration is in progress, font operation cannot proceed",
                "Wait for data migration to complete and retry the operation"};
        default:
            return {"ERR_INTERNAL_ERROR",
                "Unknown error code: " + std::to_string(code),
                "Contact system administrator for support"};
    }
}
}  // namespace

// ---------------------------------------------------------------------------
// Command table infrastructure
// ---------------------------------------------------------------------------

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
static bool gHasSubCommand = true;

#define REGISTER_CMD(cmdName, desc, use, params, ex, hdl) \
    gCommands[cmdName] = {cmdName, desc, use, params, ex, hdl}

// ---------------------------------------------------------------------------
// Help command
// ---------------------------------------------------------------------------

int CmdHelp(int argc, char** argv)
{
    std::string targetCmd;
    for (int i = 1; i < argc; i++) {
        if (argv[i] != nullptr && argv[i][0] != '-') {
            targetCmd = argv[i];
            break;
        }
    }

    if (targetCmd.empty()) {
        // Global help
        CLI_LOG("%s - %s", kProgramName, kToolDescription);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s <command> [options]", kProgramName);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --help                  Display this help message");
        CLI_LOG("");
        CLI_LOG("SubCommands:");
        CLI_LOG("  install                 Install a font from a local file");
        CLI_LOG("  uninstall               Uninstall an installed font by name");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s install --font-path /data/local/tmp/custom.ttf", kProgramName);
        CLI_LOG("  %s uninstall --font-name CustomFont", kProgramName);
        CLI_LOG("  %s install --help", kProgramName);
        return 0;
    }

    // Subcommand help
    auto it = gCommands.find(targetCmd);
    if (it == gCommands.end()) {
        return OutputError("ERR_ARG_INVALID",
            "Unknown command: " + targetCmd,
            "Run 'ohos-fontManager --help' to see available commands");
    }

    const Command& cmd = it->second;
    CLI_LOG("%s %s - %s", kProgramName, cmd.name, cmd.description ? cmd.description : "N/A");
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
    return 0;
}

// ---------------------------------------------------------------------------
// install command
// ---------------------------------------------------------------------------

int CmdInstall(int argc, char** argv)
{
    std::string fontPath;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--font-path") == 0 && i + 1 < argc) {
            fontPath = argv[i + 1];
            i++;
        } else if (strncmp(argv[i], "--font-path=", 12) == 0) {
            fontPath = argv[i] + 12;
        }
    }

    if (fontPath.empty()) {
        return OutputError("ERR_ARG_MISSING",
            "Missing required parameter: --font-path. Context: install command requires a font file path",
            "Provide a valid font file path. Example: ohos-fontManager install --font-path /data/local/tmp/custom.ttf");
    }

    CLI_LOG("Installing font from path: %s", fontPath.c_str());

    int outValue = 0;
    auto& fontManager = OHOS::Global::FontManager::FontManagerKits::GetInstance();
    int32_t ret = fontManager.InstallFont(fontPath, outValue);

    if (ret != FM_ERR_OK) {
        return OutputError("ERR_INTERNAL_ERROR",
            "Font service IPC call failed with code " + std::to_string(ret),
            "Check if the font_manager service (SA ID 66262) is running and retry");
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

// ---------------------------------------------------------------------------
// uninstall command
// ---------------------------------------------------------------------------

int CmdUninstall(int argc, char** argv)
{
    std::string fontName;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--font-name") == 0 && i + 1 < argc) {
            fontName = argv[i + 1];
            i++;
        } else if (strncmp(argv[i], "--font-name=", 12) == 0) {
            fontName = argv[i] + 12;
        }
    }

    if (fontName.empty()) {
        return OutputError("ERR_ARG_MISSING",
            "Missing required parameter: --font-name. Context: uninstall command requires a font name",
            "Provide a valid font name. Example: ohos-fontManager uninstall --font-name CustomFont");
    }

    CLI_LOG("Uninstalling font: %s", fontName.c_str());

    int outValue = 0;
    auto& fontManager = OHOS::Global::FontManager::FontManagerKits::GetInstance();
    int32_t ret = fontManager.UninstallFont(fontName, outValue);

    if (ret != FM_ERR_OK) {
        return OutputError("ERR_INTERNAL_ERROR",
            "Font service IPC call failed with code " + std::to_string(ret),
            "Check if the font_manager service (SA ID 66262) is running and retry");
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

// ---------------------------------------------------------------------------
// Command table initialization
// ---------------------------------------------------------------------------

void InitCommands()
{
    REGISTER_CMD("help", "Show this help message",
        "ohos-fontManager help [command]",
        "    command          Optional. Show detailed help for a specific command.",
        "    ohos-fontManager help\n"
        "    ohos-fontManager help install",
        CmdHelp);

    REGISTER_CMD("install", "Install a font from a local file into the system font directory",
        "ohos-fontManager install --font-path <path>",
        "    --font-path <path>  Required. Font file path to install (must exist and be readable, e.g., .ttf or .otf)",
        "    # Install a font from local file\n"
        "    ohos-fontManager install --font-path /data/local/tmp/custom.ttf",
        CmdInstall);

    REGISTER_CMD("uninstall", "Uninstall an installed font by name from the system",
        "ohos-fontManager uninstall --font-name <name>",
        "    --font-name <name>  Required. Font name to uninstall (the installed font name, not a file path)",
        "    # Uninstall a font by name\n"
        "    ohos-fontManager uninstall --font-name CustomFont",
        CmdUninstall);
}

// ---------------------------------------------------------------------------
// Main entry
// ---------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // Global --help: ohos-fontManager --help
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        InitCommands();
        char* helpArgv[2] = {argv[0], nullptr};
        CmdHelp(1, helpArgv);
        return 0;
    }

    if (argc < 2) {
        return OutputError("ERR_ARG_MISSING", "No command provided",
            "Run 'ohos-fontManager --help' to see available commands. Example: ohos-fontManager install --font-path /data/local/tmp/custom.ttf");
    }

    InitCommands();

    std::string cmdName = argv[1];

    // Subcommand --help: ohos-fontManager <subcommand> --help
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            char* helpArgv[2] = {argv[0], const_cast<char*>(cmdName.c_str())};
            CmdHelp(2, helpArgv);
            return 0;
        }
    }

    auto it = gCommands.find(cmdName);
    if (it == gCommands.end()) {
        return OutputError("ERR_UNKNOWN_COMMAND", "Unknown command: " + cmdName,
            "Run 'ohos-fontManager --help' to see available commands");
    }

    // Pass remaining arguments starting from argv[2]
    return it->second.handler(argc - 2, argv + 2);
}

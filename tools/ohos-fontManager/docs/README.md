# ohos-fontManager

## 概述

`ohos-fontManager` 是 OpenHarmony 系统字体管理 CLI 工具，用于在设备上安装和卸载系统字体。该工具通过调用 `FontManagerKits` 单例接口（`inner_kits` 层）实现对系统字体服务的操作，封装了 `FontManagerKits::GetInstance().InstallFont()` 和 `FontManagerKits::GetInstance().UninstallFont()` 两个同步接口。

## 功能列表

- **install** — 从本地文件路径安装字体到系统字体目录
- **uninstall** — 按字体名称从系统中卸载已安装的字体

## 依赖

- **系统能力**: `SystemCapability.Global.FontManager`
- **系统服务**: font_manager_server (SA ID: 66262, 按需启动)
- **权限**: `ohos.permission.UPDATE_FONT`
- **链接库**: `//base/global/font_manager/service:font_manager_client` (ohos_shared_library)
- **子系统**: global
- **部件**: font_manager

## 基本用法

```bash
ohos-fontManager <command> [options]
```

## 命令列表

| 命令 | 说明 | 参数 | 权限 | 前置依赖 |
|------|------|------|------|----------|
| install | 从本地文件安装字体到系统字体目录 | `--font-path <path>` (必需) | ohos.permission.UPDATE_FONT | 无 |
| uninstall | 按字体名称卸载已安装的字体 | `--font-name <name>` (必需) | ohos.permission.UPDATE_FONT | 无 |

**前置依赖说明**：
- **无**：该命令可直接执行，无需前置条件。字体安装状态持久化于系统配置文件，跨进程跨会话保留。

## 错误码

| 错误码 | 说明 | 触发场景 |
|--------|------|----------|
| ERR_OK (0) | 成功 | 操作成功完成 |
| ERR_NO_PERMISSION (201) | 权限不足 | 缺少 ohos.permission.UPDATE_FONT 权限 |
| ERR_NOT_SYSTEM_APP (202) | 非系统应用 | 调用方不是系统应用 |
| ERR_INVALID_PARAM (401) | 参数无效 | 参数格式或值不合法 |
| ERR_FILE_NOT_EXISTS (31100101) | 文件不存在 | 字体文件路径不存在或无法打开 |
| ERR_FILE_VERIFY_FAIL (31100102) | 文件验证失败 | 文件不是有效的字体格式 |
| ERR_COPY_FAIL (31100103) | 文件拷贝失败 | 安装过程中字体文件拷贝失败 |
| ERR_INSTALLED_ALRADY (31100104) | 字体已安装 | 该字体已安装，无需重复安装 |
| ERR_MAX_FILE_COUNT (31100105) | 超过最大数量 | 已安装字体数量达到上限 (默认 200) |
| ERR_INSTALL_FAIL (31100106) | 安装失败 | 字体服务不可用或安装操作失败 |
| ERR_UNINSTALL_FILE_NOT_EXISTS (31100107) | 字体不存在 | 指定名称的字体未安装 |
| ERR_UNINSTALL_REMOVE_FAIL (31100108) | 删除失败 | 卸载过程中字体文件删除失败 |
| ERR_UNINSTALL_FAIL (31100109) | 卸载失败 | 字体服务不可用或卸载操作失败 |
| ERR_SYSTEM_ERROR (31100110) | 系统错误 | 系统内部错误 |
| ERR_DATA_MIGRATIONING (31100111) | 数据迁移中 | 数据迁移正在进行，无法执行字体操作 |

## 示例

```bash
# 示例 1：安装字体（无前置依赖）
ohos-fontManager install --font-path /data/local/tmp/custom.ttf

# 示例 2：卸载字体（无前置依赖）
ohos-fontManager uninstall --font-name CustomFont

# 示例 3：查看全局帮助
ohos-fontManager --help

# 示例 4：查看 install 子命令帮助
ohos-fontManager install --help

# 示例 5：查看 uninstall 子命令帮助
ohos-fontManager uninstall --help
```

## 输出格式

所有输出均通过 stdout 输出，采用标准 JSON 格式。

### 成功响应

```json
{
  "type": "result",
  "status": "success",
  "data": {
    "message": "Font installed successfully",
    "fontPath": "/data/local/tmp/custom.ttf"
  }
}
```

### 失败响应

```json
{
  "type": "result",
  "status": "failed",
  "errCode": "ERR_RESOURCE_NOT_FOUND",
  "errMsg": "Font file does not exist or cannot be opened",
  "suggestion": "Verify the font file path is correct and the file is readable"
}
```

## 参数说明

### install 命令参数

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `--font-path` | string | 是 | 字体文件路径，必须存在且可读，支持 .ttf/.otf 等字体格式 |

### uninstall 命令参数

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `--font-name` | string | 是 | 字体名称（非文件路径），需与安装时的字体名称一致 |

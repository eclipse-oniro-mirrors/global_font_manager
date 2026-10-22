# ohos-fontManager

## 项目概述

`ohos-fontManager` 是 OpenHarmony 系统字体管理 CLI 工具，提供字体安装和卸载能力。该工具封装了 `FontManagerKits` 单例接口（`inner_kits` 层），通过同步 IPC 调用与 `font_manager_server` 系统服务（SA ID: 66262）通信，完成字体文件的安装和卸载操作。

## 功能列表

- **install** — 从本地文件路径安装字体到系统字体目录
- **uninstall** — 按字体名称从系统中卸载已安装的字体

## 目录结构

```
tools/ohos-fontManager/
├── BUILD.gn              # GN 构建配置
├── config.json           # CLI 工具描述文件
├── docs/
│   └── README.md         # 使用文档（命令说明、参数、权限、错误码）
├── tests/
│   └── TEST.md           # 测试文档（所有参数组合）
├── README.md             # 项目说明
└── src/
    ├── main.cpp          # 主入口（命令分发、install/uninstall 实现）
    ├── utils.h           # 公共工具函数声明（CLI_LOG、OutputSuccess、OutputError）
    └── utils.cpp         # 公共工具函数实现
```

## 构建说明

### 前置条件

- OpenHarmony 完整源码树
- 构建环境已配置（hb、gn、ninja）

### 构建命令

```bash
# 单独构建 ohos-fontManager
hb build -f ohos-fontManager

# 构建整个 font_manager 部件
hb build -f font_manager
```

### 构建产物

构建成功后，可执行文件安装到：
```
/system/bin/cli_tool/executable/ohos-fontManager
```

## 使用说明

### 基本用法

```bash
ohos-fontManager <command> [options]
```

### install — 安装字体

```bash
ohos-fontManager install --font-path <font-file-path>
```

参数：
- `--font-path <path>` (必需) — 字体文件路径，必须存在且可读

### uninstall — 卸载字体

```bash
ohos-fontManager uninstall --font-name <font-name>
```

参数：
- `--font-name <name>` (必需) — 字体名称（非文件路径）

### 帮助

```bash
# 全局帮助
ohos-fontManager --help

# install 子命令帮助
ohos-fontManager install --help

# uninstall 子命令帮助
ohos-fontManager uninstall --help
```

## 依赖

- **子系统**: global
- **部件**: font_manager
- **系统能力**: SystemCapability.Global.FontManager
- **权限**: `ohos.permission.UPDATE_FONT`
- **系统服务**: font_manager_server (SA ID: 66262, 按需启动)
- **链接库**: `//base/global/font_manager/service:font_manager_client`
- **外部依赖**: cJSON (nlohmann::json), hilog

## 技术特性

- **同步调用**: install 和 uninstall 均为纯同步阻塞操作，无需 EventRunner
- **SA 按需加载**: 初次调用时自动加载 font_manager_server SA（同步阻塞最长 5 秒）
- **全链路同步**: 从 inner_kits 入口 → client 文件操作 → SA 加载 → proxy IPC → server → framework，均为同步执行
- **JSON 输出**: 所有结果通过 stdout 以标准 JSON 格式输出
- **静态命令表**: 使用 `std::unordered_map` 进行命令分发，无 if-else 长链

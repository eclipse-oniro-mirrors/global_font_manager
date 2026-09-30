# ohos-fontManager 测试用例

## 测试覆盖

本文档覆盖 `ohos-fontManager` CLI 工具的所有命令的所有参数组合，包括 install 和 uninstall 两个子命令。

## 测试用例

| 命令示例 | 说明 | 权限 | 前置依赖 |
|------|------|------|----------|
| ohos-fontManager install --font-path /data/local/tmp/custom.ttf | 安装指定路径的字体文件（正常路径） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager install --font-path /data/local/tmp/sample.otf | 安装 OTF 格式字体文件 | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager install --font-path /data/local/tmp/中文.ttf | 安装含中文字符路径的字体文件 | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager install --font-path /data/local/tmp/nonexistent.ttf | 安装不存在的字体文件（预期失败：ERR_RESOURCE_NOT_FOUND） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager install --font-path /data/local/tmp/notfont.txt | 安装非字体文件（预期失败：ERR_FM_FILE_VERIFY_FAIL） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager install | 缺少 --font-path 参数（预期失败：ERR_ARG_MISSING） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager install --font-path | --font-path 后无值（预期失败：ERR_ARG_MISSING） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager install --font-path "" | --font-path 值为空字符串（预期失败：ERR_ARG_MISSING） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager install --font-path=/data/local/tmp/custom.ttf | 使用等号格式传递路径 | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager uninstall --font-name CustomFont | 卸载指定名称的字体（正常名称） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager uninstall --font-name SampleFont | 卸载另一个已安装字体 | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager uninstall --font-name NotInstalledFont | 卸载未安装的字体（预期失败：ERR_RESOURCE_NOT_FOUND） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager uninstall | 缺少 --font-name 参数（预期失败：ERR_ARG_MISSING） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager uninstall --font-name | --font-name 后无值（预期失败：ERR_ARG_MISSING） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager uninstall --font-name "" | --font-name 值为空字符串（预期失败：ERR_ARG_MISSING） | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager uninstall --font-name=CustomFont | 使用等号格式传递名称 | ohos.permission.UPDATE_FONT | 无 |
| ohos-fontManager --help | 查看全局帮助信息 | 无 | 无 |
| ohos-fontManager install --help | 查看 install 子命令帮助 | 无 | 无 |
| ohos-fontManager uninstall --help | 查看 uninstall 子命令帮助 | 无 | 无 |
| ohos-fontManager help | 查看全局帮助信息（help 子命令） | 无 | 无 |
| ohos-fontManager help install | 查看 install 子命令详细帮助 | 无 | 无 |
| ohos-fontManager | 无参数调用（预期失败：显示用法提示） | 无 | 无 |
| ohos-fontManager unknown | 未知命令（预期失败：显示用法提示） | 无 | 无 |

## 参数组合说明

### install 命令参数组合

| 参数组合 | 说明 | 预期结果 |
|---------|------|----------|
| `--font-path <valid_path>` | 有效字体文件路径 | 成功安装 |
| `--font-path <invalid_path>` | 不存在的文件路径 | 失败：ERR_RESOURCE_NOT_FOUND |
| `--font-path <non_font_path>` | 非字体格式文件 | 失败：ERR_FM_FILE_VERIFY_FAIL |
| `--font-path <empty_string>` | 空字符串 | 失败：ERR_ARG_MISSING |
| 缺少 `--font-path` | 无参数 | 失败：ERR_ARG_MISSING |

### uninstall 命令参数组合

| 参数组合 | 说明 | 预期结果 |
|---------|------|----------|
| `--font-name <installed_font>` | 已安装的字体名称 | 成功卸载 |
| `--font-name <not_installed_font>` | 未安装的字体名称 | 失败：ERR_RESOURCE_NOT_FOUND |
| `--font-name <empty_string>` | 空字符串 | 失败：ERR_ARG_MISSING |
| 缺少 `--font-name` | 无参数 | 失败：ERR_ARG_MISSING |

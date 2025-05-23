/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef GLOBAL_FONT_MANAGER_FONT_HILOG_H
#define GLOBAL_FONT_MANAGER_FONT_HILOG_H

#include "hilog/log.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001E00
#undef LOG_TAG
#define LOG_TAG "FONT_MSG"

#define FONT_LOGD(...) HILOG_DEBUG(LOG_CORE, __VA_ARGS__)
#define FONT_LOGI(...) HILOG_INFO(LOG_CORE, __VA_ARGS__)
#define FONT_LOGW(...) HILOG_INFO(LOG_CORE, __VA_ARGS__)
#define FONT_LOGE(...) HILOG_ERROR(LOG_CORE, __VA_ARGS__)
#endif // GLOBAL_FONT_MANAGER_FONT_HILOG_H

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
#ifndef FONT_MANAGER_FUZZ_DATA_H
#define FONT_MANAGER_FUZZ_DATA_H

#include <cstddef>
#include <cstdint>
#include <string>

/**
 * 以下是生成输入参数
 */
int32_t NewInt32(const uint8_t* data, size_t size);

std::string NewString(const uint8_t* data, size_t size);

#endif

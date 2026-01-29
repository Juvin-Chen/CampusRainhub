/*
    为站点枚举提供中文名称映射的工具类
*/
#pragma once

#include <QString>
#include "GlobalEnum.hpp"

class StationUtils {
public:
    //输入站点枚举，返回中文名称
    static QString getChineseName(Station station);
};


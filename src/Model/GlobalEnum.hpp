/*
    定义全局枚举变量，一个是雨具类型枚举，还有一个是站点枚举
*/
#pragma once

#include <QObject> // 需要使用Q_ENUM等Qt特性

//雨具类型枚举,每个数字都对应不同伞的类别,共4种
enum class GearType {
    Unknown = 0,
    StandardPlastic = 1,    // 普通塑料伞
    PremiumWindproof = 2,   // 高质量抗风伞
    Sunshade = 3,           // 专用遮阳伞
    Raincoat = 4            // 雨衣
};

//雨具状态枚举
enum class GearStatus{
    Unknown = 0,
    Available = 1,  // 可用
    Borrowed = 2,  // 借出
    Broken = 3,  // 损坏
};

//站点枚举,使用英文避免乱码
enum class Station{
    Unknown = 0,
    Wende,      // 文德楼
    Mingde,     // 明德楼
    Library,    // 图书馆
    Changwang,  // 长望楼
    Oufang,     // 藕舫楼
    Beichen,    // 北辰楼
    Dorm1,      // 西苑宿舍楼1
    Dorm2,      // 西苑宿舍楼2
    Dorm3,      // 西苑宿舍楼3
    Dorm4,      // 中苑宿舍楼4
    Dorm5,      // 中苑宿舍楼5
    Dorm6,      // 中苑宿舍楼6
    Gym,        // 体育馆
    Admin       // 行政楼
};
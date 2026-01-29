#include <QString>
#include "GlobalEnum.hpp"
#include "StationUtils.h"

QString StationUtils::getChineseName(Station station) {
    switch (station) {
        // 这里使用QStringLiteral（Qt框架提供的编译期优化宏）避免每次都创建新的QString对象
        case Station::Wende:      return QStringLiteral("文德楼");
        case Station::Mingde:     return QStringLiteral("明德楼");
        case Station::Library:    return QStringLiteral("图书馆");
        case Station::Changwang:  return QStringLiteral("长望楼");
        case Station::Oufang:     return QStringLiteral("藕舫楼");
        case Station::Beichen:    return QStringLiteral("北辰楼");
        case Station::Dorm1:      return QStringLiteral("西苑宿舍楼1");
        case Station::Dorm2:      return QStringLiteral("西苑宿舍楼2");
        case Station::Dorm3:      return QStringLiteral("西苑宿舍楼3");
        case Station::Dorm4:      return QStringLiteral("中苑宿舍楼4");
        case Station::Dorm5:      return QStringLiteral("中苑宿舍楼5");
        case Station::Dorm6:      return QStringLiteral("中苑宿舍楼6");
        case Station::Gym:        return QStringLiteral("体育馆");
        case Station::Admin:      return QStringLiteral("行政楼");
        default:                  return QStringLiteral("未知站点");
    }
}

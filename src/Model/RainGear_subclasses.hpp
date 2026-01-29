/*
    将所有继承基类的雨具子类定义都放在这个头文件中
*/
#pragma once

#include <QString>
#include "RainGear.hpp"
#include "GlobalEnum.hpp"

// 1. 普通塑料伞类
class PlasticUmbrella : public RainGear {
public:
    PlasticUmbrella(QString id):RainGear(id,GearType::StandardPlastic) {}
    double get_deposit() const override { return 10.00; } // 普通塑料伞押金10元
    QString get_iconpath() const override { return ":/icons/plastic_unbrella.png"; } // 返回图标路径
};

// 2. 高质量抗风伞类
class HighQualityUmbrella : public RainGear {
public:
    HighQualityUmbrella(QString id):RainGear(id,GearType::PremiumWindproof) {}
    double get_deposit() const override { return 20.00; } // 高质量抗风伞押金20元
    QString get_iconpath() const override { return ":/icons/highquality_unbrella.png"; } 
};

// 3. 专用遮阳伞类
class SunshadeUmbrella : public RainGear {
public:
    SunshadeUmbrella(QString id):RainGear(id,GearType::Sunshade){}
    double get_deposit() const override { return 15.0; } // 专用遮阳伞押金15元
    QString get_iconpath() const override { return ":/icons/sunshade_umbrella.png"; } 
};

// 4. 雨衣类
class Raincoat : public RainGear {
public:
    Raincoat(QString id):RainGear(id,GearType::Raincoat){}
    double get_deposit() const override { return 25.0; } // 雨衣押金25元
    QString get_iconpath() const override { return ":/icons/raincoat.png"; } 
};
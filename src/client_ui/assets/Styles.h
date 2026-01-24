/* QSS
  UI样式表 - 清晨迷雾版 (Morning Mist) 
*/

#pragma once

#include <QString>

namespace Styles {

//配色方案
namespace Colors {
    constexpr const char* Background = "#f5f7fa";    
    constexpr const char* Surface = "#ffffff";       
    constexpr const char* Primary = "#409eff";       
    constexpr const char* Success = "#67c23a";       
    constexpr const char* Warning = "#e6a23c";
    constexpr const char* Danger = "#f56c6c";
    constexpr const char* TextPrimary = "#303133";   
    constexpr const char* TextSecondary = "#606266"; 
    constexpr const char* TextPlaceholder = "#909399"; 
    constexpr const char* Border = "#dcdfe6";        
}

//全局应用样式
inline QString globalStyle() {
    return QStringLiteral(R"(
        * {
            font-family: 'Microsoft YaHei UI', 'Segoe UI', sans-serif;
            outline: none;
        }
        
        QMainWindow {
            background-color: #f5f7fa; 
        }
        
        QWidget#centralWidget {
            background: transparent;
        }
        
        QMessageBox {
            background-color: #ffffff;
        }
        QMessageBox QLabel {
            color: #303133;
            font-size: 14px;
        }
        QMessageBox QPushButton {
            background-color: #409eff;
            color: white;
            border-radius: 4px;
            padding: 6px 20px;
            border: none;
            min-width: 60px;
        }
        
        /* === 下拉框样式修复开始 === */
        QComboBox {
            background-color: #ffffff;
            border: 1px solid #dcdfe6; 
            border-radius: 4px;
            padding: 8px 12px;
            color: #303133; /* 正常状态下的文字颜色：深灰 */
            font-size: 14px;
        }
        QComboBox:hover {
            border-color: #c0c4cc;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
            background: transparent; /* 确保下拉箭头背景透明 */
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #909399; 
            margin-right: 10px;
        }
        
        /* 【关键修复】强制设置下拉弹窗的背景和颜色 */
        QComboBox QAbstractItemView {
            background-color: #ffffff; /* 弹窗背景：纯白 */
            color: #303133;            /* 弹窗文字：深灰 */
            border: 1px solid #e4e7ed;
            selection-background-color: #ecf5ff; /* 选中项背景：浅蓝 */
            selection-color: #409eff;            /* 选中项文字：亮蓝 */
            outline: none;
            min-width: 120px;
        }
        
        /* 针对每一项的额外保险设置 */
        QComboBox QAbstractItemView::item {
            color: #303133;
            background-color: #ffffff;
            min-height: 30px; /* 增加选项高度，更好点 */
        }
        QComboBox QAbstractItemView::item:selected {
            background-color: #ecf5ff;
            color: #409eff;
        }
        /* === 下拉框样式修复结束 === */
        
        QLineEdit {
            background-color: #ffffff;
            border: 1px solid #dcdfe6;
            border-radius: 4px;
            padding: 8px 12px;
            color: #303133; 
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #409eff; 
        }
        QLineEdit::placeholder {
            color: #c0c4cc;
        }
        
        QTextBrowser {
            background-color: #ffffff;
            border: none; 
            border-radius: 8px;
            padding: 15px;
            color: #606266;
            font-size: 14px;
        }
        
        QScrollBar:vertical {
            background: transparent;
            width: 6px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #c0c4cc;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical:hover {
            background: #909399;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QTableWidget {
            background-color: #ffffff;
            border: none; 
            border-radius: 8px;
            gridline-color: transparent; 
            color: #606266;
            font-size: 13px;
            alternate-background-color: #f9fafc; 
        }
        QTableWidget::item {
            padding: 10px;
            border-bottom: 1px solid #ebeef5; 
        }
        QTableWidget::item:selected {
            background-color: #ecf5ff; 
            color: #409eff;
        }
        
        /* 表头修复：浅灰背景，深色文字 */
        QHeaderView::section {
            background-color: #f5f7fa; 
            color: #303133;            
            padding: 8px;              
            border: none;
            border-bottom: 1px solid #ebeef5;
            border-right: 1px solid #ebeef5; 
            font-weight: bold;
            font-size: 13px;
        }
        QTableWidget QTableCornerButton::section {
            background-color: #f5f7fa;
            border: none;
        }
    )");
}

inline QString pageContainer() {
    return QStringLiteral(R"(
        background-color: #ffffff; 
        border-radius: 8px;
    )");
}

//按钮样式
namespace Buttons {
    inline QString primary() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: #409eff;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 10px 24px;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: #66b1ff;
            }
            QPushButton:pressed {
                background-color: #3a8ee6;
            }
        )");
    }

    inline QString accent() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: #67c23a;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 10px 24px;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: #85ce61;
            }
            QPushButton:pressed {
                background-color: #5daf34;
            }
        )");
    }
    
    inline QString link() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: transparent;
                color: #409eff;
                border: none;
                padding: 5px;
                font-size: 14px;
                text-decoration: underline;
                text-align: left;
            }
            QPushButton:hover {
                color: #66b1ff;
            }
        )");
    }
    
    inline QString primaryLarge() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: #409eff;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 16px 40px;
                font-size: 18px;
                font-weight: 600;
            }
            QPushButton:hover {
                background-color: #66b1ff;
            }
        )");
    }
    
    inline QString secondary() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: #ffffff;
                color: #606266;
                border: 1px solid #dcdfe6;
                border-radius: 4px;
                padding: 10px 24px;
                font-size: 14px;
            }
            QPushButton:hover {
                color: #409eff;
                border-color: #c6e2ff;
                background-color: #ecf5ff; 
            }
        )");
    }
    
    // 加大的借还伞按钮
    inline QString feature() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: #ffffff;
                color: #303133;
                border: 1px solid #ebeef5;
                border-radius: 16px;
                padding: 30px;         
                font-size: 26px;       
                font-weight: bold;
                min-width: 280px;      
                min-height: 140px;     
            }
            QPushButton:hover {
                color: #409eff;
                border-color: #409eff;
                background-color: #f0f9eb;
            }
            QPushButton:pressed {
                background-color: #ecf5ff;
            }
        )");
    }
    
    inline QString back() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: transparent;
                color: #909399;
                border: none;
                font-size: 14px;
                padding: 5px 10px;
            }
            QPushButton:hover {
                color: #409eff;
            }
        )");
    }
    
    inline QString logout() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: #fef0f0; 
                color: #f56c6c;
                border: 1px solid #fde2e2;
                border-radius: 4px;
                padding: 6px 16px;
                font-size: 13px;
            }
            QPushButton:hover {
                background-color: #f56c6c;
                color: white;
            }
        )");
    }
    
    inline QString nav() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: #ffffff;
                color: #606266;
                border: 1px solid #ebeef5;
                border-radius: 6px;
                padding: 10px;
            }
            QPushButton:hover {
                color: #409eff;
                border-color: #409eff;
            }
        )");
    }
    
    inline QString sideNav() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: transparent;
                color: #bfcbd9;
                border: none;
                text-align: left;
                padding: 12px 20px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #263445;
                color: #ffffff;
            }
        )");
    }
    
    inline QString sideNavActive() {
        return QStringLiteral(R"(
            QPushButton {
                background-color: #409eff;
                color: #ffffff;
                border: none;
                text-align: left;
                padding: 12px 20px;
                font-size: 14px;
                font-weight: bold;
            }
        )");
    }
}

namespace Labels {
    inline QString title() {
        return QStringLiteral(R"(
            font-size: 32px;
            font-weight: bold;
            color: #303133; 
        )");
    }
    
    inline QString pageTitle() {
        return QStringLiteral(R"(
            font-size: 24px;
            font-weight: 600;
            color: #303133;
        )");
    }
    
    inline QString subtitle() {
        return QStringLiteral(R"(
            font-size: 14px;
            color: #909399;
        )");
    }
    
    inline QString hint() {
        return QStringLiteral(R"(
            font-size: 12px;
            color: #909399;
        )");
    }
    
    inline QString formLabel() {
        return QStringLiteral(R"(
            font-size: 14px;
            font-weight: 600;
            color: #606266;
        )");
    }
    
    inline QString balance() {
        return QStringLiteral(R"(
            font-size: 36px;
            font-weight: bold;
            color: #67c23a; 
        )");
    }
    
    inline QString info() {
        return QStringLiteral(R"(
            font-size: 16px;
            color: #303133;
        )");
    }
    
    inline QString welcomeIcon() {
        return QStringLiteral(R"(
            font-size: 64px;
            color: #409eff;
        )");
    }
    
    inline QString sidebarTitle() {
        return QStringLiteral(R"(
            font-size: 18px;
            font-weight: bold;
            color: #ffffff;
            padding: 10px;
        )");
    }
    
    inline QString statNumber() {
        return QStringLiteral(R"(
            font-size: 28px;
            font-weight: bold;
            color: #409eff;
        )");
    }
    
    inline QString statLabel() {
        return QStringLiteral(R"(
            font-size: 12px;
            color: #909399;
        )");
    }
}

namespace SlotCard {
    inline QString available() {
        return QStringLiteral(R"(
            SlotItem {
                background-color: #f0f9eb; 
                border: 1px solid #e1f3d8;
                border-radius: 12px;
            }
            SlotItem:hover {
                background-color: #67c23a; 
                border: 2px solid #67c23a;
                background-color: #f0f9eb;
            }
        )");
    }
    
    inline QString empty() {
        return QStringLiteral(R"(
            SlotItem {
                background-color: #f4f4f5;
                border: 1px solid #e9e9eb;
                border-radius: 12px;
            }
        )");
    }
    
    inline QString maintenance() {
        return QStringLiteral(R"(
            SlotItem {
                background-color: #fef0f0;
                border: 1px solid #fde2e2;
                border-radius: 12px;
            }
        )");
    }
    
    inline QString selected() {
        return QStringLiteral(R"(
            SlotItem {
                background-color: #ffffff;
                border: 2px solid #409eff;
                border-radius: 12px;
            }
        )");
    }
}

inline QString mapContainer() {
    return QStringLiteral(R"(
        background-color: #ffffff;
        border: 1px solid #ebeef5;
        border-radius: 12px;
    )");
}

inline QString adminSidebar() {
    return QStringLiteral(R"(
        background-color: #304156;
    )");
}

inline QString statCard() {
    return QStringLiteral(R"(
        background-color: #ffffff;
        border: 1px solid #ebeef5;
        border-radius: 8px;
    )");
}

}
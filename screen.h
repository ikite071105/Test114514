#ifndef SCREEN_H
#define SCREEN_H

// 在包含任何其他内容之前，定义这些宏
#define WIN32_LEAN_AND_MEAN  // 减少Windows头文件的大小
#define NOMINMAX             // 防止min/max宏冲突
#include <windows.h>         // 先包含Windows头文件

#include <stdbool.h>
#include <graphics.h>        // 然后包含EasyX


typedef enum screen{
	mainscreen,    //初始主界面
	game,    //游戏界面
	setting, //设置界面
	rule     //规则界面
}SCREEN;

// 菜单选项枚举
enum MenuOption {
	START_GAME,
	GAME_SETTINGS,
	GAME_RULES
};

// 检查点是否在矩形内
bool isPointInRect(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight);
// 绘制带箭头的按钮
void drawArrowButton(int x, int y, int width, int height, int arrowSize, bool hovered);
//点击屏幕以继续 & 点击箭头返回
void WaitForContinueScreen(SCREEN* screen);
//清除点击屏幕提示
void ClearWaitForClick();
// 绘制选择按钮的辅助函数
void drawChoiceButton(int x, int y, int width, int height, const TCHAR* text, bool hovered);
//清除提示文字
void clearTips();
//清除选择按钮
void clearChoiceButton(int x, int y, int width, int height);
//清除底部手牌
void clearBottomCard();
// 下注/加注数值选择器
// 参数：minValue-最小值, maxValue-最大值
int valueSelector(int minValue, int maxValue);
//清除下注区域
void clearvalueSelector();

#endif //SCREEN_H

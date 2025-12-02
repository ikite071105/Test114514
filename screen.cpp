
// 然后包含你的头文件
#include "screen.h"

// 检查点是否在矩形内
bool isPointInRect(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
    return (x >= rectX && x <= rectX + rectWidth &&
        y >= rectY && y <= rectY + rectHeight);
}

void drawArrowButton(int x, int y, int width, int height, int arrowSize, bool hovered) {
    // 绘制按钮背景
    if (hovered) {
        setfillcolor(RGB(255, 255, 200)); // 悬停状态
    }
    else {
        setfillcolor(RGB(245, 245, 220)); // 正常状态
    }
    setlinecolor(RGB(200, 180, 150));
    setlinestyle(PS_SOLID, 2);
    fillrectangle(x, y, x + width, y + height);
    rectangle(x, y, x + width, y + height);

    // 绘制箭头
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID, 3);
    int arrowStartX = x + (width - arrowSize) / 2;
    int arrowStartY = y + (height - arrowSize) / 2;
    line(arrowStartX + arrowSize, arrowStartY,
        arrowStartX, arrowStartY + arrowSize / 2);
    line(arrowStartX, arrowStartY + arrowSize / 2,
        arrowStartX + arrowSize, arrowStartY + arrowSize);
}

// 等待继续屏幕
void WaitForContinueScreen(SCREEN* screen) {
    // 定义箭头按钮区域
    const int arrowButtonX = 20;
    const int arrowButtonY = 20;
    const int arrowButtonWidth = 50;
    const int arrowButtonHeight = 40;
    const int arrowSize = 20;

    // 绘制"点击屏幕以继续"提示
    settextcolor(RGB(255, 255, 255));
    settextstyle(24, 0, _T("黑体"));
    outtextxy(800, 650, _T("点击屏幕以继续"));

    // 绘制初始箭头按钮
    drawArrowButton(arrowButtonX, arrowButtonY, arrowButtonWidth, arrowButtonHeight, arrowSize, false);

    // 刷新整个画面
    FlushBatchDraw();

    // 交互状态变量
    bool continueClicked = false;
    bool arrowHovered = false;
    bool needRedraw = false;
    ExMessage msg;

    // 主交互循环
    while (!continueClicked) {
        // 检查消息
        if (peekmessage(&msg, EM_MOUSE | EM_KEY, true)) {
            switch (msg.message) {
            case WM_MOUSEMOVE: {
                // 检查鼠标是否悬停在箭头按钮上
                bool newHovered = (msg.x >= arrowButtonX && msg.x <= arrowButtonX + arrowButtonWidth &&
                    msg.y >= arrowButtonY && msg.y <= arrowButtonY + arrowButtonHeight);

                if (newHovered != arrowHovered) {
                    arrowHovered = newHovered;
                    needRedraw = true;
                }
                break;
            }

            case WM_LBUTTONDOWN:
                // 检查是否点击了箭头按钮
                if (msg.x >= arrowButtonX && msg.x <= arrowButtonX + arrowButtonWidth &&
                    msg.y >= arrowButtonY && msg.y <= arrowButtonY + arrowButtonHeight) {
                    // 点击箭头按钮：返回主界面
                    *screen = mainscreen;
                    continueClicked = true;
                    break;
                }
                else {
                    // 点击屏幕其他地方表示继续
                    continueClicked = true;
                }
                break;

            case WM_KEYDOWN:
                switch (msg.vkcode) {
                case VK_BACK:    // Backspace键
                case VK_ESCAPE:  // ESC键
                    // 按下退出键：返回主界面
                    *screen = mainscreen;
                    continueClicked = true;
                    break;
                case VK_SPACE:
                case VK_RETURN:
                    // 空格或回车键继续
                    continueClicked = true;
                    break;
                }
                break;
            }
        }

        // 如果需要重绘按钮
        if (needRedraw) {
            drawArrowButton(arrowButtonX, arrowButtonY, arrowButtonWidth, arrowButtonHeight, arrowSize, arrowHovered);
            FlushBatchDraw();
            needRedraw = false;
        }

        // 短暂休眠
        Sleep(10);
    }

    // 正常继续，函数执行完毕
    //清除提示
    setfillcolor(RGB(49, 78, 22));
    solidrectangle(800, 650, 1000, 680);
    FlushBatchDraw();
}

// 下注/加注数值选择器
// 参数：minValue-最小值, maxValue-最大值
int valueSelector(int minBet, int maxBet) {
    //是否确认
    bool isConfirm = false;
    // 按钮和框的位置（左移后）
    const int boxX = 400;           // 数字显示框X坐标
    const int boxY = 400;           // 数字显示框Y坐标
    const int btnY = 420;           // 按钮Y坐标
    const int btnRadius = 20;       // 按钮半径

    // 按钮位置（左移后）
    const int minusBtnX = 350;      // "-"按钮X
    const int plusBtnX = 630;       // "+"按钮X
    const int valueBoxX = 400;      // 数值显示框X
    const int valueBoxWidth = 200;            // 数值框宽度
    const int valueBoxHeight = 50;            // 数值框高度

    // "确定"按钮位置
    const int confirmBtnX = valueBoxX + valueBoxWidth + 100;  // 在数值框右边50像素
    const int confirmBtnY = boxY;                            // 与数值框同一高度
    const int confirmBtnWidth = 80;                          // 确定按钮宽度
    const int confirmBtnHeight = valueBoxHeight;             // 与数值框同高

    // 当前数值（静态变量保持状态）
    static int currentValue = minBet;

    // 悬停状态：0-无悬停, 1-减按钮, 2-加按钮, 3-确定按钮
    static int hoverState = 0;
    static int mouseX = 0, mouseY = 0;

    // 处理鼠标消息
    ExMessage msg;
    while (!isConfirm) {
        while (peekmessage(&msg, EM_MOUSE)) {
            // 处理鼠标移动 - 更新悬停状态
            if (msg.message == WM_MOUSEMOVE) {
                mouseX = msg.x;
                mouseY = msg.y;

                // 检查悬停状态
                hoverState = 0;  // 重置

                // 检查"-"按钮
                if ((mouseX - minusBtnX) * (mouseX - minusBtnX) +
                    (mouseY - btnY) * (mouseY - btnY) <= btnRadius * btnRadius) {
                    hoverState = 1;  // 减按钮
                }
                // 检查"+"按钮
                else if ((mouseX - plusBtnX) * (mouseX - plusBtnX) +
                    (mouseY - btnY) * (mouseY - btnY) <= btnRadius * btnRadius) {
                    hoverState = 2;  // 加按钮
                }
                // 检查"确定"按钮
                else if (mouseX >= confirmBtnX && mouseX <= confirmBtnX + confirmBtnWidth &&
                    mouseY >= confirmBtnY && mouseY <= confirmBtnY + confirmBtnHeight) {
                    hoverState = 3;  // 确定按钮
                }
            }
            // 处理鼠标点击
            else if (msg.message == WM_LBUTTONDOWN) {
                // 减号按钮
                if (hoverState == 1 && currentValue - 5 >= minBet) {
                    currentValue -= 5;
                }
                // 加号按钮
                else if (hoverState == 2 && currentValue + 5 <= maxBet) {
                    currentValue += 5;
                }
                // 确定按钮 - 点击后立即返回
                else if (hoverState == 3) {
                    // 点击确定，直接返回当前值
                    isConfirm = true;
                }
            }
        }

        // 绘制选择器区域

        // 减号按钮 - 根据悬停状态设置颜色
        if (hoverState == 1) {
            setfillcolor(RGB(255, 213, 169)); // 选中色
        }
        else {
            setfillcolor(RGB(251, 234, 208)); // 默认色
        }
        solidcircle(minusBtnX, btnY, btnRadius);

        // 数字显示矩形
        setfillcolor(RGB(251, 234, 208));
        solidrectangle(valueBoxX, boxY, valueBoxX + valueBoxWidth, boxY + valueBoxHeight);

        // 加号按钮 - 根据悬停状态设置颜色
        if (hoverState == 2) {
            setfillcolor(RGB(255, 213, 169)); // 选中色
        }
        else {
            setfillcolor(RGB(251, 234, 208)); // 默认色
        }
        solidcircle(plusBtnX, btnY, btnRadius);

        // "确定"按钮 - 根据悬停状态设置颜色
        if (hoverState == 3) {
            setfillcolor(RGB(255, 213, 169)); // 选中色
        }
        else {
            setfillcolor(RGB(251, 234, 208)); // 默认色
        }
        solidrectangle(confirmBtnX, confirmBtnY,
            confirmBtnX + confirmBtnWidth, confirmBtnY + confirmBtnHeight);

        // 辅助符号绘制
        settextstyle(40, 0, _T("Arial"));
        settextcolor(BLACK);

        // 加减符号
        outtextxy(minusBtnX - 8, btnY - 20, _T("-"));
        outtextxy(plusBtnX - 8, btnY - 20, _T("+"));

        // 显示当前数值
        settextstyle(30, 0, _T("Arial"));
        TCHAR valueText[10];
        _stprintf_s(valueText, sizeof(valueText) / sizeof(TCHAR), _T("%d"), currentValue);
        int textWidth = textwidth(valueText);
        outtextxy(valueBoxX + (valueBoxWidth - textWidth) / 2, boxY + 10, valueText);

        // 显示"确定"文字
        settextstyle(30, 0, _T("黑体"));
        textWidth = textwidth(_T("确定"));
        outtextxy(confirmBtnX + (confirmBtnWidth - textWidth) / 2, confirmBtnY + 12, _T("确定"));

        FlushBatchDraw();
    }
    // 如果没有点击确定，返回0表示还在选择中
    // （或者你可以返回一个特殊值，比如-1）
    return currentValue;  
}

// 使用示例：
// 下注时：int betAmount = valueSelector(10, player->chips);
// 加注时：int raiseAmount = valueSelector(game->currentBet + 10, player->chips);

// 清除点击屏幕提示
void ClearWaitForClick() {
    setfillcolor(RGB(49, 78, 22));
    solidrectangle(400, 650, 600, 680);
    FlushBatchDraw();
}


// 绘制选择按钮的辅助函数
void drawChoiceButton(int x, int y, int width, int height, const TCHAR* text, bool hovered) {
    // 按钮背景颜色
    COLORREF bgColor = hovered ? RGB(100, 150, 200) : RGB(70, 130, 180);
    COLORREF borderColor = hovered ? RGB(150, 200, 250) : RGB(100, 150, 200);
    COLORREF textColor = RGB(255, 255, 255);

    // 绘制按钮背景
    setfillcolor(bgColor);
    setlinecolor(borderColor);
    setlinestyle(PS_SOLID, 2);
    fillrectangle(x, y, x + width, y + height);
    rectangle(x, y, x + width, y + height);

    // 绘制按钮文字
    settextcolor(textColor);
    settextstyle(24, 0, _T("黑体"));
    int textWidth = textwidth(text);
    int textHeight = textheight(text);
    outtextxy(x + (width - textWidth) / 2, y + (height - textHeight) / 2, text);

    FlushBatchDraw();
}

//清除选择按钮
void clearChoiceButton(int x, int y, int width, int height) {
    setfillcolor(RGB(49, 78, 22));
    solidrectangle(x-2, y-2, x + width, y + height);
    FlushBatchDraw();
}

//清除提示文字
void clearTips() {
    setfillcolor(RGB(49, 78, 22));
    solidrectangle(500, 300, 800, 325);
    FlushBatchDraw();
}

//清除下注区域
void clearvalueSelector() {
    setfillcolor(RGB(49, 78, 22));
    solidrectangle(250, 400, 800, 450);
    FlushBatchDraw();
}

//清除底部手牌
void clearBottomCard() {
    setfillcolor(RGB(49, 78, 22));
    solidrectangle(145, 527, 650, 650);
    FlushBatchDraw();
}
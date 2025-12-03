#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "screen.h"

//玩家初始化
void initPlayer(Player* player, const char* name, PlayerType type) {
    strncpy_s(player->name, sizeof(player->name), name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    player->type = type; 
    player->chips = 100;
    player->isActive = true;
    player->hasChecked = false;
    player->finalRank = 0;
    player->isOut = false;
}

//给玩家发牌（5）
void dealCardsToPlayer(Player* player, Deck* deck , int CardNum) {
    for(int i = 0; i < CardNum; i++) {
        if (canDeal(deck)) {
            player->hand[i] = dealCard(deck);
            player->hand[i].visible = false; //发牌时默认反面显示
        }
        else {
            resetDeck(deck);
            player->hand[i] = dealCard(deck);
            player->hand[i].visible = false; //发牌时默认反面显示
        }
    }
}

//玩家下注或跟注
void playerCallorBet(PokerGame* game,Player* player, int amount) {
        player->chips -= amount;
        game->currentBet = amount; //当前下注更新
        game->pot += amount;       //底池+
}

//玩家加注
void playerRaise(PokerGame* game, Player* player, int amount, int raiseAmount) {
     player->chips -= ( amount + raiseAmount);
     game->pot += (amount + raiseAmount);
     game->currentBet = (amount + raiseAmount);
}

//玩家下注回合选项
Action playerBetChoice(PokerGame* game, Player* player, bool* isbetting) {
    Action action;
    // 定义选择区域
    const int choiceBoxY = 450;        // 选择框Y坐标
    const int choiceFirstX = 250;      // 第一个按钮X坐标
    const int choiceStep = 110;        // 按钮x坐标间隔
    const int choiceWidth = 80;        // 选择框宽度
    const int choiceHeight = 40;       // 选择框高度

    bool choiceMade = false;           // 是否已做出选择
    int hoveredChoice = 0;             // 0:无悬停, 1:弃牌, 2:过牌/跟注，3:下注/加注/全押

    // 绘制"请选择操作"
    settextcolor(WHITE);
    settextstyle(30, 0, _T("黑体"));
    outtextxy(200, choiceBoxY - 40, _T("请选择操作"));

    // 绘制选择按钮
    //当前未下注且玩家筹码足够
    if (!*isbetting && player->chips > 10) {
        drawChoiceButton(choiceFirstX, choiceBoxY, choiceWidth, choiceHeight, _T("弃牌"), hoveredChoice == 1);
        drawChoiceButton(choiceFirstX + choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("过牌"), hoveredChoice == 2);
        drawChoiceButton(choiceFirstX + 2*choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("下注"), hoveredChoice == 3);
    }//当前未下注但玩家筹码不足
    else if (!*isbetting && player->chips <= 10) {
        drawChoiceButton(choiceFirstX, choiceBoxY, choiceWidth, choiceHeight, _T("弃牌"), hoveredChoice == 1);
        drawChoiceButton(choiceFirstX + choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("过牌"), hoveredChoice == 2);
        drawChoiceButton(choiceFirstX + 2 * choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("全押"), hoveredChoice == 3);
    }//当前已下注且筹码足够
    else if (*isbetting && player->chips > game->currentBet) {
        drawChoiceButton(choiceFirstX, choiceBoxY, choiceWidth, choiceHeight, _T("弃牌"), hoveredChoice == 1);
        drawChoiceButton(choiceFirstX + choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("跟注"), hoveredChoice == 2);
        drawChoiceButton(choiceFirstX + 2 * choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("加注"), hoveredChoice == 3);
    }//当前已下注但筹码不足以跟注
    else if (*isbetting && player->chips <= game->currentBet) {
        drawChoiceButton(choiceFirstX, choiceBoxY, choiceWidth, choiceHeight, _T("弃牌"), hoveredChoice == 1);
        drawChoiceButton(choiceFirstX + 2 * choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("全押"), hoveredChoice == 3);
    }

    FlushBatchDraw();

    ExMessage msg;
    int lastHoveredChoice = 0; //鼠标上一刻悬停位置

    while (!choiceMade) {

        // 处理所有消息
        while (peekmessage(&msg, EM_MOUSE)) {
            switch (msg.message) {
            case WM_MOUSEMOVE:
                // 检查鼠标是否悬停在选择按钮上
                lastHoveredChoice = hoveredChoice;
                hoveredChoice = 0;

                // 检查第一个按钮
                if (msg.x >= choiceFirstX && msg.x <= choiceFirstX + choiceWidth &&
                    msg.y >= choiceBoxY && msg.y <= choiceBoxY + choiceHeight) {
                    hoveredChoice = 1;
                }
                // 检查第二个按钮
                else if (msg.x >= choiceFirstX+choiceStep && msg.x <= choiceFirstX + choiceStep + choiceWidth &&
                    msg.y >= choiceBoxY && msg.y <= choiceBoxY + choiceHeight) {
                    hoveredChoice = 2;
                }
                // 检查第三个按钮
                else if (msg.x >= choiceFirstX + 2*choiceStep && msg.x <= choiceFirstX + 2*choiceStep + choiceWidth &&
                    msg.y >= choiceBoxY && msg.y <= choiceBoxY + choiceHeight) {
                    hoveredChoice = 3;
                }

                // 如果悬停状态改变，重绘按钮
                if (hoveredChoice != lastHoveredChoice) {
                    //当前未下注且玩家筹码足够
                    if (!*isbetting && player->chips > 10) {
                        drawChoiceButton(choiceFirstX, choiceBoxY, choiceWidth, choiceHeight, _T("弃牌"), hoveredChoice == 1);
                        drawChoiceButton(choiceFirstX + choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("过牌"), hoveredChoice == 2);
                        drawChoiceButton(choiceFirstX + 2 * choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("下注"), hoveredChoice == 3);
                    }//当前未下注但玩家筹码不足
                    else if (!*isbetting && player->chips <= 10) {
                        drawChoiceButton(choiceFirstX, choiceBoxY, choiceWidth, choiceHeight, _T("弃牌"), hoveredChoice == 1);
                        drawChoiceButton(choiceFirstX + choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("过牌"), hoveredChoice == 2);
                        drawChoiceButton(choiceFirstX + 2 * choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("全押"), hoveredChoice == 3);
                    }//当前已下注且筹码足够
                    else if (*isbetting && player->chips > game->currentBet) {
                        drawChoiceButton(choiceFirstX, choiceBoxY, choiceWidth, choiceHeight, _T("弃牌"), hoveredChoice == 1);
                        drawChoiceButton(choiceFirstX + choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("跟注"), hoveredChoice == 2);
                        drawChoiceButton(choiceFirstX + 2 * choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("加注"), hoveredChoice == 3);
                    }//当前已下注但筹码不足以跟注
                    else if (*isbetting && player->chips <= game->currentBet) {
                        drawChoiceButton(choiceFirstX, choiceBoxY, choiceWidth, choiceHeight, _T("弃牌"), hoveredChoice == 1);
                        drawChoiceButton(choiceFirstX + 2 * choiceStep, choiceBoxY, choiceWidth, choiceHeight, _T("全押"), hoveredChoice == 3);
                    }
                }
                break;

            case WM_LBUTTONDOWN:
                // 检查是否点击了选择按钮
                if (hoveredChoice == 1) {  // 点击"弃牌"
                    action = fold;
                    playerFoldCard(player,game);
                    choiceMade = true;
                    player->isActive = false;
                }
                else if (hoveredChoice == 2) {  // 第二个按钮
                    choiceMade = true;
                    //过牌
                    if (!*isbetting && !player->hasChecked) {
                        action = check;
                        player->hasChecked = true;
                    }
                    else if (!*isbetting && player->hasChecked) {
                        choiceMade = false;
                    }
                    //跟注
                    else if (player->chips >= game->currentBet) {
                        action = call;
                    }
                }
                else if (hoveredChoice == 3) {  //第三个按钮
                    choiceMade = true;
                    //下注/全押
                    if (!*isbetting) {
                        if (player->chips <= 10) {//全押
                            action = allin;
                            game->pot += player->chips;
                            player->chips = 0;
                        }
                        else { //下注
                            action = bet;
                            *isbetting = true;
                        }
                    }//加注/全押
                    else {
                        if (player->chips <= game->currentBet) {//全押
                            action = allin;
                            game->pot += player->chips;
                            player->chips = 0;
                        }
                        else { //加注
                            action = raise;
                        }
                    }
                }
                break;
            }
        }
        // 避免CPU占用过高
        Sleep(10);
    }
    //清除选择按钮
    clearChoiceButton(choiceFirstX, choiceBoxY, choiceWidth*4, choiceHeight);
    //清除提示文字
    setfillcolor(RGB(49, 78, 22));
    settextstyle(20, 0, _T("黑体"));
    solidrectangle(200, choiceBoxY - 40, 200 + textwidth(_T("点击手牌选择要更换的牌")), choiceBoxY + 70);
    return action;
}

//玩家换牌
bool playerReplaceCard(Player* player, Deck *deck) {
    static bool isreplace;
    isreplace = false;
    // 定义手牌区域和公共牌区域
    const int playerHandStartX = 150;  // 玩家手牌起始X坐标
    const int playerHandY = 530;       // 玩家手牌Y坐标
    const int cardWidth = 80;          // 牌宽度
    const int cardHeight = 120;        // 牌高度

    // 定义选择区域
    const int choiceBoxY = 450;        // 选择框Y坐标
    const int choiceYesX = 250;        // "是"按钮X坐标
    const int choiceNoX = 350;         // "否"按钮X坐标
    const int choiceWidth = 80;        // 选择框宽度
    const int choiceHeight = 40;       // 选择框高度

    bool selectedCards[5] = { false }; // 存储已选择的牌索引（0-4为手牌）
    bool choiceMade = false;           // 是否已做出选择
    bool hasSelected = false;          // 检查是否有选中的牌
    int hoveredChoice = 0;             // 0:无悬停, 1:是, 2:否

    // 绘制"是否换牌？"
    settextcolor(RGB(255, 255, 255));
    settextstyle(30, 0, _T("黑体"));
    outtextxy(200, choiceBoxY - 40, _T("是否换牌？"));

    // 绘制选择按钮
    drawChoiceButton(choiceYesX, choiceBoxY, choiceWidth, choiceHeight, _T("是"), hoveredChoice == 1);
    drawChoiceButton(choiceNoX, choiceBoxY, choiceWidth, choiceHeight, _T("否"), hoveredChoice == 2);

    // 绘制提示文字
    settextcolor(RGB(255, 255, 255));
    settextstyle(20, 0, _T("黑体"));
    outtextxy(200, choiceBoxY + 50, _T("点击手牌选择要更换的牌"));

    FlushBatchDraw();

    ExMessage msg;
    int lastHoveredChoice = 0; //鼠标上一刻悬停位置
    int clickedCardIndex = -1; //被点击的牌
    while (!choiceMade) {
        
        // 处理所有消息
        while (peekmessage(&msg, EM_MOUSE )) {
            switch (msg.message) {
            case WM_MOUSEMOVE:
                // 检查鼠标是否悬停在选择按钮上
                lastHoveredChoice = hoveredChoice;
                hoveredChoice = 0;

                // 检查"是"按钮
                if (msg.x >= choiceYesX && msg.x <= choiceYesX + choiceWidth &&
                    msg.y >= choiceBoxY && msg.y <= choiceBoxY + choiceHeight) {
                    hoveredChoice = 1;
                }
                // 检查"否"按钮
                else if (msg.x >= choiceNoX && msg.x <= choiceNoX + choiceWidth &&
                    msg.y >= choiceBoxY && msg.y <= choiceBoxY + choiceHeight) {
                    hoveredChoice = 2;
                }

                // 如果悬停状态改变，重绘按钮
                if (hoveredChoice != lastHoveredChoice) {
                    drawChoiceButton(choiceYesX, choiceBoxY, choiceWidth, choiceHeight,
                        _T("是"), hoveredChoice == 1);
                    drawChoiceButton(choiceNoX, choiceBoxY, choiceWidth, choiceHeight,
                        _T("否"), hoveredChoice == 2);
                }
                break;

            case WM_LBUTTONDOWN:
                // 检查是否点击了手牌区域
                if (msg.y >= playerHandY && msg.y <= playerHandY + cardHeight) {
                    // 找出点击的是哪张牌
                    for (int i = 0; i < 5; i++) {
                        int cardX = playerHandStartX + i * 100;
                        if (msg.x >= cardX && msg.x <= cardX + cardWidth) {
                            clickedCardIndex = i;
                            break;
                        }
                    }//有牌被点击
                    if (clickedCardIndex != -1) {
                        // 如果点击的牌已经被选中，则取消选中
                        if (selectedCards[clickedCardIndex]) {
                            selectedCards[clickedCardIndex] = false;
                            hasSelected = false;
                            // 重绘点击的牌（去掉选择框）
                            drawSelectedFrame(clickedCardIndex,
                                playerHandStartX + clickedCardIndex * 100, playerHandY,
                                &player->hand[clickedCardIndex], false);
                            FlushBatchDraw();
                        }
                        // 如果点击的牌未被选中，则先取消所有牌的选择，再选中这张
                        else {
                            // 先取消所有牌的选中状态
                            for (int i = 0; i < 5; i++) {
                                if (selectedCards[i]) {
                                    selectedCards[i] = false;
                                    // 重绘该牌（去掉选择框）
                                    drawSelectedFrame(i, playerHandStartX + i * 100,
                                        playerHandY, &player->hand[i], false);
                                    FlushBatchDraw();
                                }
                            }
                            // 选中点击的牌
                            selectedCards[clickedCardIndex] = true;
                            hasSelected = true;

                            // 只有选中新牌时才绘制选中框
                            drawSelectedFrame(clickedCardIndex,
                                playerHandStartX + clickedCardIndex * 100, playerHandY,
                                &player->hand[clickedCardIndex], true);
                            FlushBatchDraw();
                        }
                    }
                }
                // 检查是否点击了选择按钮
                if (hoveredChoice == 1) {  // 点击"是"
                    // 检查是否有选中的牌
                    for (int i = 0; i < 5; i++) {
                        if (selectedCards[i]) {
                            hasSelected = true;
                            break;
                        }
                    }

                    if (hasSelected) {
                        // 执行换牌逻辑
                        player->hand[clickedCardIndex] = dealCard(deck);
                        player->hand[clickedCardIndex].visible = true;
                        isreplace = true;
                        choiceMade = true;
                    }
                    else {
                        choiceMade = false;
                        // 显示提示：请选择要更换的牌
                        settextcolor(RGB(255, 100, 100));
                        settextstyle(20, 0, _T("黑体"));
                        outtextxy(800, choiceBoxY + 80, _T("请先选择要更换的牌"));
                        Sleep(1500);  // 显示提示1.5秒

                        // 清除提示
                        setfillcolor(RGB(49, 78, 22));
                        solidrectangle(800, choiceBoxY + 80, 1000, choiceBoxY + 110);
                    }
                }
                else if (hoveredChoice == 2) {  // 点击"否"
                    choiceMade = true;
                }
                break;

            }
        }

        // 避免CPU占用过高
        Sleep(10);
    }
    //清除选择按钮
    clearChoiceButton(choiceYesX, choiceBoxY, choiceWidth, choiceHeight);
    clearChoiceButton(choiceNoX, choiceBoxY, choiceWidth, choiceHeight);
    //清除提示文字
    setfillcolor(RGB(49, 78, 22));
    settextstyle(20, 0, _T("黑体"));
    solidrectangle(200, choiceBoxY - 40, 200 + textwidth(_T("点击手牌选择要更换的牌")), choiceBoxY + 70);
    return isreplace;
}

// 摊牌阶段
int Showdown(PokerGame* game,Player* player, Deck* deck) {
    //最终得分
    static int score;
    score = 0;
   //临时牌组（计分用
    Card temp[5] ;  
    temp[0] = game->communityCards[0];
    temp[1] = game->communityCards[1];

    // 定义手牌区域和公共牌区域
    const int playerHandStartX = 150;  // 玩家手牌起始X坐标
    const int playerHandY = 530;       // 玩家手牌Y坐标
    const int cardWidth = 80;          // 牌宽度
    const int cardHeight = 120;        // 牌高度
    // 定义选择区域
    const int choiceBoxY = 450;        // 选择框Y坐标
    const int choiceConfirmX = 350;     // 确认按钮X坐标
    const int choiceWidth = 80;        // 选择框宽度
    const int choiceHeight = 40;       // 选择框高度
    //
    bool selectedCards[5] = { false }; // 存储已选择的牌索引
    bool choiceMade = false;           // 是否已做出选择
    int selectedCount = 0;             // 当前选中的牌数
    int lastIndex = -1;                //上张选中的牌
    int hoveredChoice = 0;             // 0:无悬停, 1:确定

    // 绘制"请选择要展示的牌"
    settextcolor(RGB(255, 255, 255));
    settextstyle(30, 0, _T("黑体"));
    outtextxy(200, choiceBoxY - 40, _T("请选择要展示的牌"));

    // 绘制选择按钮
    drawChoiceButton(choiceConfirmX, choiceBoxY, choiceWidth, choiceHeight, _T("确定"), hoveredChoice == 1);

    // 绘制提示文字
    settextcolor(RGB(255, 255, 255));
    settextstyle(20, 0, _T("黑体"));
    outtextxy(200, choiceBoxY + 50, _T("点击手牌进行选择"));
    FlushBatchDraw();

    ExMessage msg;
    int lastHoveredChoice = 0; //鼠标上一刻悬停位置
    int clickedCardIndex = -1; //被点击的牌

    while (!choiceMade) {

        // 处理所有消息
        while (peekmessage(&msg, EM_MOUSE)) {
            switch (msg.message) {
            case WM_MOUSEMOVE:
                // 检查鼠标是否悬停在选择按钮上
                lastHoveredChoice = hoveredChoice;
                hoveredChoice = 0;

                // 检查"确认"按钮
                if (msg.x >= choiceConfirmX && msg.x <= choiceConfirmX + choiceWidth &&
                    msg.y >= choiceBoxY && msg.y <= choiceBoxY + choiceHeight) {
                    hoveredChoice = 1;
                }

                // 如果悬停状态改变，重绘按钮
                if (hoveredChoice != lastHoveredChoice) {
                    drawChoiceButton(choiceConfirmX, choiceBoxY, choiceWidth, choiceHeight,
                        _T("确定"), hoveredChoice == 1);
                }
                break;

            case WM_LBUTTONDOWN:
                // 检查是否点击了手牌区域
                if (msg.y >= playerHandY && msg.y <= playerHandY + cardHeight) {
                    // 找出点击的是哪张牌
                    for (int i = 0; i < 5; i++) {
                        int cardX = playerHandStartX + i * 100;
                        if (msg.x >= cardX && msg.x <= cardX + cardWidth) {
                            clickedCardIndex = i;
                            break;
                        }
                    }
                    // 有牌被点击
                    if (clickedCardIndex != -1) {
                        // 如果点击的牌已经被选中，则取消选中
                        if (selectedCards[clickedCardIndex]) {
                            selectedCards[clickedCardIndex] = false;
                            selectedCount--;
                            // 重绘点击的牌（去掉选择框）
                            drawSelectedFrame(clickedCardIndex,
                                playerHandStartX + clickedCardIndex * 100, playerHandY,
                                &player->hand[clickedCardIndex], false);
                            FlushBatchDraw();
                        }
                        // 如果点击的牌未被选中
                        else {
                            // 如果已经选中了3张牌，需要取消最早选中的一张
                            if (selectedCount >= 3) {
                                // 找到第一个被选中的牌并取消选中
                                for (int i = 0; i < 5; i++) {
                                    if (selectedCards[i]) {
                                        selectedCards[i] = false;
                                        selectedCount--;
                                        // 重绘被取消选中的牌（去掉选择框）
                                        drawSelectedFrame(i, playerHandStartX + i * 100,
                                            playerHandY, &player->hand[i], false);
                                        break;
                                    }
                                }
                            }
                            // 选中当前点击的牌
                            selectedCards[clickedCardIndex] = true;
                            selectedCount++;
                            // 绘制选中框
                            drawSelectedFrame(clickedCardIndex,
                                playerHandStartX + clickedCardIndex * 100, playerHandY,
                                &player->hand[clickedCardIndex], true);
                            FlushBatchDraw();
                        }
                    }
                }
                // 检查是否点击了选择按钮
                if (hoveredChoice == 1) {  // 点击"是"
                    // 检查是否有选中3张牌
                    if (selectedCount == 3) {
                        int cardIndex = 2;
                        for (int i = 0; i < 5; i++) {
                            if (selectedCards[i]) {//该牌被选中
                                temp[cardIndex] = player->hand[i]; //更新临时牌组
                                cardIndex++;
                                player->hand[i] = dealCard(deck);//先把牌发了
                            }
                        }
                        choiceMade = true;
                            break;
                    }//没选够3张
                    else {
                        choiceMade = false;
                        // 显示提示：请选择要更换的牌
                        settextcolor(RGB(255, 100, 100));
                        settextstyle(20, 0, _T("黑体"));
                        outtextxy(800, choiceBoxY + 80, _T("请先选择要展示的牌"));
                        Sleep(1500);  // 显示提示1.5秒

                        // 清除提示
                        setfillcolor(RGB(49, 78, 22));
                        solidrectangle(800, choiceBoxY + 80, 1000, choiceBoxY + 110);
                    }
                }
                break;
            }
        }
        // 避免CPU占用过高
        Sleep(10);
    }
    //清除被选中的手牌
    for (int i = 0; i < 5; i++) {
        if (selectedCards[i]) {//该牌被选中
            setfillcolor(RGB(49, 78, 22));
            solidrectangle(100 * i + 150, 527, 100 * i + 230,650);
            //清除选中框
            drawSelectedFrame(i, playerHandStartX + i * 100,
                playerHandY, &player->hand[i], false);
        }
    }
    //清除选择按钮
    clearChoiceButton(choiceConfirmX, choiceBoxY, choiceWidth, choiceHeight);
    //清除提示文字
    setfillcolor(RGB(49, 78, 22));
    settextstyle(20, 0, _T("黑体"));
    solidrectangle(200, choiceBoxY - 40, 220 + textwidth(_T("点击手牌选择要更换的牌")), choiceBoxY + 70);
    
    //把选中的牌展示到公共牌旁边
    for (int i = 2; i < 5; i++) {
        drawCardAt(150+i*100, 220, &temp[i]);
    }
    line(330, 200, 330, 340);
    score = GetScore(temp);

    return score;
}

 //玩家弃牌
void playerFoldCard(Player* player, PokerGame* game) {
    player->chips -= 5;
    game->pot += 5;
    player->isActive = false;
    // 手牌数组填充为无效值或特殊标记
    for (int i = 0; i < 5; i++) {
        player->hand[i].suit = NONE;  // 无效花色
        player->hand[i].visible = false;
    }
}

// 更新玩家筹码数显示
void UpdateChipsDisplay(PokerGame* game) {
    //覆盖之前的数据的矩形颜色
    setfillcolor(RGB(49, 78, 22));
    //字体设置
    settextcolor(RGB(252, 251, 159));
    settextstyle(30, 0, _T("黑体"));
    //顶部玩家
    int x = textwidth(_T("玩家2"));
    int x2 = x + 130;
    int x3 = textwidth(_T("筹码:"));
    for (int i = 1; i < game->playerCount; i++) {
        if (!game->players[i].isOut) {//玩家未被淘汰
            //覆盖矩形
            solidrectangle(100 + x2 * (i - 1), 70, 100 + x2 * (i - 1) + x3, 100);
            //玩家筹码数
            TCHAR playerChips[10];
            _stprintf_s(playerChips, 10, _T("%d"), game->players[i].chips);
            outtextxy(100 + x2 * (i - 1), 70, playerChips);
        }
    }

    //底部玩家
    solidrectangle(25, 605, 150, 640);
    settextstyle(35, 0, _T("黑体"));
    TCHAR playerChips[10];
    _stprintf_s(playerChips, 10, _T("%d"), game->players[game->bottomPlayer].chips);
    outtextxy(25, 605, playerChips);

    //底池
    settextstyle(30, 0, _T("黑体"));
    solidrectangle(880, 250, 930, 280);
    TCHAR currentPot[20];
    _stprintf_s(currentPot, 20, _T("%d"), game->pot);
    outtextxy(880, 250, currentPot);

    //下注
    solidrectangle(880, 310, 930, 340);
    TCHAR currentBet[20];
    _stprintf_s(currentBet, 20, _T("%d"), game->currentBet);
    outtextxy(880, 310, currentBet);

    FlushBatchDraw();
}

//初始化游戏
void initGame(PokerGame* game,Deck* deck) {
    //玩家初始化
    for (int i = 0; i < game->playerCount; i++) {
        char name[50];
        if (i < game->humanCount) {
            sprintf_s(name, sizeof(name), "玩家%d", i + 1);
            initPlayer(&game->players[i], name, HUMAN);
        }
        else {
            sprintf_s(name, sizeof(name), "电脑%d", i + 1 - game->humanCount);
            initPlayer(&game->players[i], name, AI);
        }
        //给玩家发牌
        dealCardsToPlayer(&game->players[i], deck, 5);
    }
    //初始公共牌
    for (int i = 0; i < 2; i++) {
        game->communityCards[i] = dealCard(deck);
    }
    game->communityCards[0].visible = true;//第一张公共牌可见
    game->currentPlayer = 0;  //第一名玩家先行动
    game->bottomPlayer = 0;   //底部玩家
    game->currentBet = 0;     //当前最高下注为0 
    game->pot = 0;            //初始底池为0
    game->phase = GAME_START; //初始游戏阶段：开始游戏
    game->allChecked = false; //玩家没有全部选择过牌
    game->currentRank = game->playerCount;
    for (int i = 0; i < 5; i++) {
        game->players[game->bottomPlayer].hand[i].visible = 1;
    }
}

//开始新一轮游戏
void startNewRound(PokerGame* game, Deck* deck) {
    //初始公共牌
    for (int i = 0; i < 2; i++) {
        game->communityCards[i] = dealCard(deck);
    }
    game->communityCards[0].visible = true;//第一张公共牌可见
    game->currentPlayer = 0;  //第一名玩家先行动
    game->bottomPlayer = 0;   //底部玩家
    game->currentBet = 0;     //当前最高下注为0 
    game->pot = 0;            //初始底池为0
    game->phase = GAME_START; //初始游戏阶段：开始游戏
    game->allChecked = false; //玩家没有全部选择过牌
    for (int i = 0; i < 5; i++) {
        game->players[game->bottomPlayer].hand[i].visible = 1;
        if(!game->players[i].isOut){
            game->players[i].hasChecked = false;
            game->players[i].isActive = true;
        }
    }
}

//AI相关
//人机下注回合选项
Action AIBetChoice(PokerGame* game, Player* player, bool* isbetting) {
    int currentCase;
    static Action action;
    //当前行动状态
    //当前未下注且玩家筹码足够
    if (!*isbetting && player->chips > 10) {
        currentCase = 0;
    }//当前未下注但玩家筹码不足
    else if (!*isbetting && player->chips <= 10) {
        currentCase = 1;
    }//当前已下注且筹码足够
    else if (*isbetting && player->chips > game->currentBet) {
        currentCase = 2;
    }//当前已下注但筹码不足以跟注
    else if (*isbetting && player->chips <= game->currentBet) {
        currentCase = 3;
    }
    //根据状态进行行动
    switch (currentCase) {
    case 0 ://自动下注
        action = bet;
        *isbetting = true;
        break;
    case 1://自动全押
        action = allin;
        break;
    case 2://随机选择跟注/加注
        static int choice ;
        choice = 0;
        srand((unsigned int)time(NULL));
        choice = rand() % 2; //跟注0，加注1
        if (!choice)
            action = call;
        else
            action = raise;
        break;
    case 3://自动全押
        action = allin;
        break;
    }
    return action;
}

 //人机选牌(随机选三张
int AIShowdown(PokerGame* game, Player* player, Deck* deck) {
    //最终得分
    static int score;
    score = 0;
    //临时牌组（计分用
    Card temp[5];
    temp[0] = game->communityCards[0];
    temp[1] = game->communityCards[1];

    srand((unsigned int)time(NULL));

    bool selectedCards[5] = { false }; // 存储已选择的牌索引
    int selectedCount = 0;             // 当前选中的牌数
    static int selectedIndex;          //被选中的牌序号
    int cardIndex = 2;
    while (selectedCount != 3) {
        selectedIndex = rand() % 5;
        if (!selectedCards[selectedIndex]) {//该牌未被选中
            selectedCards[selectedIndex] = true;
            player->hand[selectedIndex].visible = true;
            temp[cardIndex] = player->hand[selectedIndex]; //更新临时牌组
            cardIndex++;
            player->hand[selectedIndex] = dealCard(deck);//先把牌发了
            selectedCount++;
        }
    }
    //把选中的牌展示到公共牌旁边
    for (int i = 2; i < 5; i++) {
        drawCardAt(150 + i * 100, 220, &temp[i]);
    }
    line(330, 200, 330, 340);
    score = GetScore(temp);

    return score;
}

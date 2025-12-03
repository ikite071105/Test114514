#include <graphics.h>
#include <stdio.h>
#include "game.h"
#include "card.h"
#include "screen.h"
#pragma execution_character_set("utf-8")



int main() {
    PokerGame pokergame;
    Deck deck;
    int isGameInit = 0;
    initializeDeck(&deck);  //初始化牌堆
    shuffleDeck(&deck);     //洗牌
    pokergame.playerCount = 3;
    pokergame.humanCount = 1;
    int currentHumanPlayer = 0;  //当前人类玩家

    int action = 0;

    SCREEN currentscreen = mainscreen;
    int selectedOption = START_GAME;  // 当前选中的选项
    initgraph(1080, 680);
    BeginBatchDraw();

    // 主菜单选项的矩形区域
    struct OptionRect {
        int x, y, width, height;
    } options[3] = {
        {350, 170, 350, 130},  // 开始游戏
        {350, 340, 350, 130},  // 游戏设置
        {350, 510, 350, 130}   // 游戏规则
    };

    while (1) {

        int x = 0, y = 0;

        // 主菜单界面
        if (currentscreen == mainscreen) {
            setbkcolor(RGB(49, 78, 22));
            cleardevice();

            //绘制标题及组员名字
            settextstyle(130, 0, _T("楷体"));
            settextcolor(WHITE);
            x = textwidth(_T("扑克牌游戏"));
            outtextxy((1080 - x) / 2, 10, _T("扑克牌游戏"));
            settextstyle(20, 0, _T("宋体"));
            settextcolor(WHITE);
            outtextxy(900, 150, _T("by_zyd/yhr/zry"));
            outtextxy(901, 151, _T("by_zyd/yhr/zry"));

            // 绘制选项框 - 根据选中状态设置不同颜色
            for (int i = 0; i < 3; i++) {
                if (i == selectedOption) {
                    setfillcolor(RGB(255, 213, 169));  // 被选中选项颜色
                }
                else {
                    setfillcolor(RGB(251, 234, 208));  // 未被选中
                }
                setlinecolor(BROWN);
                setlinestyle(0, 3);
                fillrectangle(options[i].x, options[i].y,
                    options[i].x + options[i].width,
                    options[i].y + options[i].height);
            }

            // 绘制选项文字
            settextstyle(60, 0, _T("黑体"));
            settextcolor(RGB(136,140,11));
            setbkmode(TRANSPARENT);  // 设置文字背景透明
            outtextxy(400, 200, _T("开始游戏"));
            outtextxy(400, 370, _T("游戏设置"));
            outtextxy(400, 540, _T("游戏规则"));

            // 绘制操作提示
            settextstyle(20, 0, _T("宋体"));
            settextcolor(WHITE);
            outtextxy(400, 650, _T("使用方向键选择，Enter确认 或 鼠标点击选择"));

            // 绘制音乐图标
            setlinecolor(BROWN);
            setfillcolor(RGB(251, 234, 208));
            setlinestyle(0, 3);
            fillcircle(60, 600, 35);
            settextstyle(50, 0, _T("Arial"));
            settextcolor(BLACK);
            outtextxy(45, 580, _T("♫"));  // 连音音符

            FlushBatchDraw();

            // 处理输入
            ExMessage msg;   //鼠标及键盘信息结构体
            while (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
                // 鼠标移动 - 悬停选择
                if (msg.message == WM_MOUSEMOVE) {
                    for (int i = 0; i < 3; i++) {
                        if (isPointInRect(msg.x, msg.y, options[i].x, options[i].y,
                            options[i].width, options[i].height)) {
                            selectedOption = i;
                            break;
                        }
                    }
                }

                // 鼠标点击
                if (msg.message == WM_LBUTTONDOWN) {
                    for (int i = 0; i < 3; i++) {
                        if (isPointInRect(msg.x, msg.y, options[i].x, options[i].y,
                            options[i].width, options[i].height)) {
                            // 根据选择执行相应操作
                            switch (i) {
                            case START_GAME:
                                // 进入游戏界面
                                currentscreen = game;
                                break;
                            case GAME_SETTINGS:
                                // 进入设置界面
                                currentscreen = setting;
                                break;
                            case GAME_RULES:
                                // 进入规则界面
                                currentscreen = rule;
                                break;
                            }
                            break;
                        }
                    }
                }

                // 键盘输入
                if (msg.message == WM_KEYDOWN) {
                    switch (msg.vkcode) {
                    case VK_UP:
                        selectedOption = (selectedOption - 1 + 3) % 3;
                        break;
                    case VK_DOWN:
                        selectedOption = (selectedOption + 1) % 3;
                        break;
                    case VK_RETURN:
                        // 确认选择
                        switch (selectedOption) {
                        case START_GAME:
                            currentscreen = game;
                            break;
                        case GAME_SETTINGS:
                            currentscreen = setting;
                            break;
                        case GAME_RULES:
                            currentscreen = rule;
                            break;
                        }
                        break;
                    case VK_ESCAPE:
                        closegraph();
                        return 0;
                    }
                }
            }
            Sleep(10);  // 减少CPU占用（暂停程序10毫秒
        }

        // 游戏界面
        else if (currentscreen == game) {
            setbkcolor(RGB(49, 78, 22));
            //初始化游戏
            if(!isGameInit){ 
                initGame(&pokergame, &deck); 
                isGameInit = 1;
            }

            switch (pokergame.phase) {
                
            case(GAME_START): {
                cleardevice();
                //牌堆绘制
                drawCardBackAt(970, 220);
                //当前底池
                settextcolor(RGB(252, 251, 159));
                settextstyle(30, 0, _T("黑体"));
                TCHAR currentPot[20];
                _stprintf_s(currentPot, sizeof(currentPot), _T("%d"), pokergame.pot);
                outtextxy(880, 220, _T("底池:"));
                outtextxy(880, 250, currentPot);
                //当前下注
                TCHAR currentBet[20];
                _stprintf_s(currentBet, sizeof(currentBet), _T("%d"), pokergame.currentBet);
                outtextxy(880, 280, _T("下注:"));
                outtextxy(880, 310, currentBet);

                //公共牌绘制
                settextstyle(30, 0, _T("黑体"));
                outtextxy(10, 210, _T("公共牌:"));
                x = textwidth(_T("公共牌:"));
                drawCardAt(x + 15, 220, &pokergame.communityCards[0]);
                drawCardBackAt(x + 130, 220);

                //顶部玩家绘制
                for (int i = 1; i < pokergame.playerCount; i++) {
                    if (!pokergame.players[i].isOut) {
                        settextcolor(RGB(252, 251, 159));
                        settextstyle(30, 0, _T("黑体"));
                        x = textwidth(_T("玩家2"));
                        int x2 = x + 130;
                        //玩家信息
                        TCHAR topplayerText[20];
                        _stprintf_s(topplayerText, sizeof(topplayerText), _T("玩家%d"), i + 1);  // 在%d前加上"玩家"
                        outtextxy(100 + x2 * (i - 1), 10, topplayerText);
                        //玩家筹码数
                        //玩家筹码
                        outtextxy(100 + x2 * (i - 1), 40, _T("筹码:"));
                        TCHAR playerChips[10];
                        _stprintf_s(playerChips, sizeof(playerChips), _T("%d"), pokergame.players[i].chips);
                        outtextxy(100 + x2 * (i - 1), 70, playerChips);
                        //玩家手牌（牌背
                        drawCardBackAt(x + x2 * (i - 1) + 105, 10);
                        drawCardBackAt(x + x2 * (i - 1) + 107, 12);
                        drawCardBackAt(x + x2 * (i - 1) + 109, 14);
                        drawCardBackAt(x + x2 * (i - 1) + 111, 16);
                        drawCardBackAt(x + x2 * (i - 1) + 113, 18);
                    }//玩家已被淘汰
                    else {
                        settextcolor(YELLOW);
                        settextstyle(30, 0, _T("黑体"));
                        x = textwidth(_T("玩家2"));
                        int x2 = x + 130;
                        //玩家信息
                        TCHAR topplayerText[20];
                        _stprintf_s(topplayerText, sizeof(topplayerText), _T("玩家%d已出局"), i + 1); 
                        outtextxy(100 + x2 * (i - 1), 10, topplayerText);
                    }
                }

                //底部玩家选择区域
                //分割线
                setlinecolor(RGB(175, 240, 206));
                setlinestyle(3, 4);
                line(5, 350, 400, 350);


                //底部当前人类玩家
                //玩家信息
                settextcolor(RGB(252, 251, 159));
                settextstyle(40, 0, _T("黑体"));
                TCHAR bottomplayerText[20];
                _stprintf_s(bottomplayerText, sizeof(bottomplayerText), _T("玩家%d"), pokergame.bottomPlayer + 1); 
                outtextxy(25 , 520, bottomplayerText);
                settextstyle(35, 0, _T("黑体"));
                //玩家筹码数
                outtextxy(25, 570, _T("筹码:"));
                TCHAR playerChips[10];
                _stprintf_s(playerChips, sizeof(playerChips), _T("%d"), pokergame.players[pokergame.bottomPlayer].chips);
                outtextxy(25, 605, playerChips);
                //玩家手牌
                for (int i = 0; i < 5; i++) {
                    drawCardAt(100 * i + 150, 530, &pokergame.players[pokergame.bottomPlayer].hand[i]);
                }
                //点击屏幕以继续
                WaitForContinueScreen(&currentscreen);
                if (currentscreen != game) {
                    break;
                }

                //交底注
                for (int i = 0; i < pokergame.playerCount; i++) {
                    pokergame.players[i].chips -= 5;
                    pokergame.pot += 5;
                }
                UpdateChipsDisplay(&pokergame);
                //WaitForContinueScreen();

                FlushBatchDraw();

                // 短暂暂停后继续
                Sleep(100);
                ClearWaitForClick();
                pokergame.phase = REPLACING;
                break;
            }
            case(NEW_ROUND): {
                startNewRound(&pokergame, &deck);
                break;
            }
            case(REPLACING): {
                //第几次换牌
                static int replaceCount = 0;
                printf("replaceCount = %d\n", replaceCount);

                //公共牌绘制
                switch (replaceCount) {
                    //第一次换牌
                case(0):{
                    replaceCount++;
                    break;
                } //第二次换牌
                case(1): {
                    //公共牌绘制2
                    settextstyle(30, 0, _T("黑体"));
                    x = textwidth(_T("公共牌:"));
                    pokergame.communityCards[1].visible = true;
                    drawCardAt(x + 130, 220, &pokergame.communityCards[1]);
                    //重置
                    replaceCount = 0;
                    break;
                }
                }
                FlushBatchDraw();
                //玩家轮流进行换牌操作
                for (int i = 0; i < pokergame.playerCount; i++) {
                    printf("excute player %d\n", i);
                    //人类玩家
                    if (pokergame.players[i].type == HUMAN) {
                        bool isreplace = playerReplaceCard(&pokergame.players[i], &deck);
                        // 先取消所有牌的选中框
                        for (int j = 0; j < 5; j++) {
                            // 去掉选择框
                            drawSelectedFrame(j, 150 + j * 100,
                                530, &(pokergame.players[i].hand[j]), false);
                        }
                        FlushBatchDraw();
                        if (isreplace) {
                            // 绘制提示文字
                            settextcolor(WHITE);
                            settextstyle(25, 0, _T("黑体"));
                            TCHAR playerActionText[50];
                            _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了换牌"), i+1);
                            outtextxy(500, 300, playerActionText);
                            FlushBatchDraw();
                            WaitForContinueScreen(&currentscreen);
                            //清除提示文字
                            clearTips();
                            //玩家手牌绘制
                            for (int i = 0; i < 5; i++) {
                                drawCardAt(100 * i + 150, 530, &pokergame.players[pokergame.bottomPlayer].hand[i]);
                            }
                        }
                    }//人机玩家
                    else if (pokergame.players[i].type == AI) {
                        //暂时还是不会换牌的蠢人机
                        continue;
                    }
                }
                //点击屏幕确认
                WaitForContinueScreen(&currentscreen);
                if (currentscreen != game) {
                    break;
                }
                pokergame.phase = BETTING;
                break;
            }

            //下注回合
            case(BETTING): {
                static bool isbetting;
                //第几次下注//似乎没必要
                static int betCount = 0;
                printf("betCount = %d\n", betCount);
                //当前回合是否有人下注
                switch (betCount) {
                    //第一次下注
                case(0): 
                    isbetting = false;
                    printf("FirstBetting\n");
                    betCount++;
                    break;
                 //第二次下注
                case(1): 
                    //重置
                    betCount++;
                    break;
                }
                
                //玩家轮流进行下注操作
                Action playerAction;
                for (int i = 0; i < pokergame.playerCount; i++) {
                    if (pokergame.players[i].isActive) {//玩家未弃牌
                        printf("excute player %d\n", i);
                        //人类玩家
                        if (pokergame.players[i].type == HUMAN) {
                            //pokergame.bottomPlayer = i; //当前底部玩家
                            playerAction = playerBetChoice(&pokergame, &pokergame.players[i], &isbetting);
                            
                            //执行操作
                            switch (playerAction) {
                            case(fold)://玩家弃牌
                                clearBottomCard();
                                // 绘制提示文字
                                settextcolor(WHITE);
                                settextstyle(25, 0, _T("黑体"));
                                TCHAR playerActionText[50];
                                _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了弃牌"), i + 1);
                                outtextxy(500, 300, playerActionText);
                                FlushBatchDraw();
                                break;
                            case(check)://玩家过牌
                                // 绘制提示文字
                                settextcolor(WHITE);
                                settextstyle(25, 0, _T("黑体"));
                                playerActionText[50];
                                _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了过牌"), i + 1);
                                outtextxy(500, 300, playerActionText);
                                FlushBatchDraw();
                                break;
                            case(bet)://玩家下注
                                pokergame.currentBet = valueSelector(10, pokergame.players[i].chips);
                                WaitForContinueScreen(&currentscreen);
                                //提示文字
                                _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了下注%d"), i + 1, pokergame.currentBet);
                                outtextxy(500, 300, playerActionText);
                                clearvalueSelector();
                                playerCallorBet(&pokergame,&pokergame.players[i], pokergame.currentBet);
                                FlushBatchDraw();
                                break;
                            case(call)://玩家跟注
                                //提示文字
                                _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了跟注%d"), i + 1, pokergame.currentBet);
                                outtextxy(500, 300, playerActionText);
                                playerCallorBet(&pokergame, &pokergame.players[i], pokergame.currentBet);
                                FlushBatchDraw();
                                break;
                            case(raise)://玩家加注
                                static int RaiseAmount;
                                RaiseAmount = valueSelector(5, pokergame.players[i].chips - pokergame.currentBet);
                                WaitForContinueScreen(&currentscreen);
                                //提示文字
                                _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了加注%d"), i + 1, RaiseAmount);
                                outtextxy(500, 300, playerActionText);
                                clearvalueSelector();
                                playerRaise(&pokergame, &pokergame.players[i], pokergame.currentBet, RaiseAmount);
                                FlushBatchDraw();
                                break;
                            case(allin):
                                //提示文字
                                _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了全押"), i + 1);
                                outtextxy(500, 300, playerActionText);
                                break;
                            }
                            UpdateChipsDisplay(&pokergame);
                            WaitForContinueScreen(&currentscreen);
                            //清除提示文字
                            clearTips();
                        }//人机玩家
                        else if (pokergame.players[i].type == AI) {
                            if (!pokergame.players[i].isOut) {
                                //人机行动选择
                                playerAction = AIBetChoice(&pokergame, &pokergame.players[i], &isbetting);
                                //执行操作
                                switch (playerAction) {
                                case(fold)://玩家弃牌
                                    //人机不会弃牌
                                    break;
                                case(check)://玩家过牌
                                    //人机也不会过牌
                                    break;
                                case(bet)://玩家下注
                                    pokergame.currentBet = 10;//人机自动下注10
                                    //提示文字
                                    settextcolor(WHITE);
                                    settextstyle(25, 0, _T("黑体"));
                                    TCHAR playerActionText[50];
                                    _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了下注%d"), i + 1, pokergame.currentBet);
                                    outtextxy(500, 300, playerActionText);
                                    playerCallorBet(&pokergame, &pokergame.players[i], pokergame.currentBet);
                                    FlushBatchDraw();
                                    break;
                                case(call)://玩家跟注
                                    //提示文字
                                    _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了跟注%d"), i + 1, pokergame.currentBet);
                                    outtextxy(500, 300, playerActionText);
                                    playerCallorBet(&pokergame, &pokergame.players[i], pokergame.currentBet);
                                    FlushBatchDraw();
                                    break;
                                case(raise)://玩家加注
                                    static int RaiseAmount;
                                    RaiseAmount = 5;//人机自动加注5
                                    WaitForContinueScreen(&currentscreen);
                                    //提示文字
                                    _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了加注%d"), i + 1, RaiseAmount);
                                    outtextxy(500, 300, playerActionText);
                                    clearvalueSelector();
                                    playerRaise(&pokergame, &pokergame.players[i], pokergame.currentBet, RaiseAmount);
                                    FlushBatchDraw();
                                    break;
                                case(allin):
                                    //提示文字
                                    _stprintf_s(playerActionText, sizeof(playerActionText), _T("玩家%d选择了全押"), i + 1);
                                    outtextxy(500, 300, playerActionText);
                                    pokergame.pot += pokergame.players[i].chips;
                                    pokergame.players[i].chips = 0;
                                    break;
                                }
                                UpdateChipsDisplay(&pokergame);
                                WaitForContinueScreen(&currentscreen);
                                //清除提示文字
                                clearTips();
                            }
                        }
                    }
                }
                //点击屏幕确认
                WaitForContinueScreen(&currentscreen);
                if (currentscreen != game) {
                    break;
                }
                switch (betCount) {
                case 1:
                    pokergame.phase = REPLACING;
                    break;
                case 2:
                    pokergame.phase = SHOWDOWN;
                    betCount = 0;//重置
                    break;
                }
                break;
            }
            //轮流选牌&摊牌/统计分数
            case(SHOWDOWN): {
                //玩家轮流进行换牌操作
                for (int i = 0; i < pokergame.playerCount; i++) {
                    printf("excute player %d\n", i);
                    if (pokergame.players[i].isActive && !pokergame.players[i].isOut) {
                        //人类玩家
                        if (pokergame.players[i].type == HUMAN) {
                            //当前玩家提示
                            TCHAR playerText[50];
                            _stprintf_s(playerText, sizeof(playerText), _T("玩家%d:"), i + 1);
                            outtextxy(500, 170, playerText);
                            pokergame.players[i].score = Showdown(&pokergame, &pokergame.players[i], &deck);
                            // 绘制提示文字
                            settextcolor(WHITE);
                            settextstyle(25, 0, _T("黑体"));
                            TCHAR playerScoreText[50];
                            _stprintf_s(playerScoreText, sizeof(playerScoreText), _T("玩家%d本轮得分%d"), i + 1, pokergame.players[i].score);
                            outtextxy(500, 400, playerScoreText);
                            //玩家分数
                            _stprintf_s(playerScoreText, sizeof(playerScoreText), _T("%d"), pokergame.players[i].score);
                            outtextxy(25, 490, playerScoreText);
                            FlushBatchDraw();
                            WaitForContinueScreen(&currentscreen);
                            //清除提示文字
                            setfillcolor(RGB(49, 78, 22));
                            solidrectangle(500, 165, 700, 450);
                            //清除展示的牌
                            solidrectangle(325, 200, 700, 340);

                        }//人机玩家
                        else if (pokergame.players[i].type == AI) {
                            if(!pokergame.players[i].isOut) {//未出局
                                //当前玩家提示
                                TCHAR playerText[50];
                                _stprintf_s(playerText, sizeof(playerText), _T("玩家%d:"), i + 1);
                                outtextxy(500, 170, playerText);
                                pokergame.players[i].score = AIShowdown(&pokergame, &pokergame.players[i], &deck);
                                // 绘制提示文字
                                settextcolor(WHITE);
                                settextstyle(25, 0, _T("黑体"));
                                TCHAR playerScoreText[50];
                                _stprintf_s(playerScoreText, sizeof(playerScoreText), _T("玩家%d本轮得分%d"), i + 1, pokergame.players[i].score);
                                outtextxy(500, 400, playerScoreText);
                                //玩家分数
                                _stprintf_s(playerScoreText, sizeof(playerScoreText), _T("%d"), pokergame.players[i].score);
                                outtextxy(100 + 210 * (i - 1), 100, playerScoreText);
                                FlushBatchDraw();
                                WaitForContinueScreen(&currentscreen);
                                //清除提示文字
                                setfillcolor(RGB(49, 78, 22));
                                solidrectangle(500, 165, 700, 450);
                                //清除展示的牌
                                solidrectangle(325, 200, 700, 340);
                            }
                        }
                    }
                }
                //底池筹码给到得分最高的玩家
                int maxScore = 0;
                int winnerIndex = 0;
                for (int i = 0; i < pokergame.playerCount; i++) {
                    winnerIndex = (pokergame.players[i].score > maxScore) ? i : winnerIndex;
                    maxScore = (pokergame.players[i].score > maxScore) ? pokergame.players[i].score : maxScore;
                }
                //绘制提示文字
                pokergame.players[winnerIndex].chips += pokergame.pot;
                pokergame.pot = 0;
                TCHAR playerScoreText[50];
                _stprintf_s(playerScoreText, sizeof(playerScoreText), _T("玩家%d本轮得分最高，为%d"), winnerIndex + 1, pokergame.players[winnerIndex].score);
                outtextxy(500, 300, playerScoreText);
                UpdateChipsDisplay(&pokergame);
                
                int remain = 0;  //剩余玩家数
                for (int i = 0; i < pokergame.playerCount; i++) {
                    //剩余筹码为0，出局
                    if (pokergame.players[i].chips == 0){
                        if (!pokergame.players[i].isOut){//标记为出局并统计排名
                            pokergame.players[i].finalRank = pokergame.currentRank;
                            pokergame.currentRank--;
                            pokergame.players[i].isOut = true;
                        }
                    }
                    if (!pokergame.players[i].isOut)
                        remain++;
                }
                WaitForContinueScreen(&currentscreen);
                if (remain <= 2 || pokergame.players[pokergame.bottomPlayer].isOut)
                    pokergame.phase = GAME_OVER;//剩余人数<2或唯一真人被淘汰
                else
                    pokergame.phase = NEW_ROUND;
                break;
            }
            //游戏结束/结果显示
            case(GAME_OVER): {
                cleardevice();
                setbkcolor(RGB(49, 78, 22));
                int maxchips = 0;
                for (int i = 0; i < pokergame.playerCount; i++) {
                    maxchips = (pokergame.players[i].chips > maxchips) ? pokergame.players[i].chips : maxchips;
                }
                if (pokergame.players[0].chips == maxchips) {
                    settextstyle(50, 0, _T("隶书"));
                    settextcolor(RGB(251, 234, 208));
                    //第一行
                    const TCHAR* text1 = _T("恭喜！");
                    int text1_width = textwidth(text1);  // 获取文字宽度
                    int text1_height = textheight(text1); // 获取文字高度
                    int x1 = (1080 - text1_width) / 2;    // 水平居中
                    int y1 = (680 - 2 * text1_height) / 2; // 垂直居中（考虑两行文字）
                    outtextxy(x1, y1, text1);

                    // 第二行文字
                    const TCHAR* text2 = _T("你赢了！");
                    int text2_width = textwidth(text2);
                    int x2 = (1080 - text2_width) / 2;    // 水平居中
                    int y2 = y1 + text1_height + 20;      // 在上一行下面，加20像素间距
                    outtextxy(x2, y2, text2);
                }
                else {
                    settextstyle(50, 0, _T("隶书"));
                    settextcolor(RGB(251, 234, 208));
                    //第一行
                    const TCHAR* text1 = _T("可惜！");
                    int text1_width = textwidth(text1);  // 获取文字宽度
                    int text1_height = textheight(text1); // 获取文字高度
                    int x1 = (1080 - text1_width) / 2;    // 水平居中
                    int y1 = (680 - 2 * text1_height) / 2; // 垂直居中（考虑两行文字）
                    outtextxy(x1, y1, text1);

                    // 第二行文字
                    const TCHAR* text2 = _T("你输了！");
                    int text2_width = textwidth(text2);
                    int x2 = (1080 - text2_width) / 2;    // 水平居中
                    int y2 = y1 + text1_height + 20;      // 在上一行下面，加20像素间距
                    outtextxy(x2, y2, text2);
                }
                WaitForContinueScreen(&currentscreen);
                currentscreen = mainscreen;
                isGameInit = 0;
                break;
            }
            }

            
            FlushBatchDraw();

        }

        // 设置界面
        else if (currentscreen == setting) {
            cleardevice();

            // 键盘鼠标一起检测
            ExMessage msg;                      // 鼠标及键盘信息结构体
            static int mouseX = 0, mouseY = 0;  // 静态变量保持状态
            static int hoverState = 0;  // 0:无悬停, 1:玩家人数减, 2:玩家人数加, 3:人类数量减, 4:人类数量加

            while (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
                if (msg.message == WM_KEYDOWN) {
                    switch (msg.vkcode) {
                    case VK_BACK:
                        currentscreen = mainscreen;
                        break;
                    }
                }
                // 处理鼠标移动 - 更新悬停状态
                else if (msg.message == WM_MOUSEMOVE) {
                    mouseX = msg.x;
                    mouseY = msg.y;

                    // 更新悬停状态
                    if ((mouseX - 450) * (mouseX - 450) + (mouseY - 180) * (mouseY - 180) <= 25 * 25) {
                        hoverState = 1;  // 玩家人数减
                    }
                    else if ((mouseX - 850) * (mouseX - 850) + (mouseY - 180) * (mouseY - 180) <= 25 * 25) {
                        hoverState = 2;  // 玩家人数加
                    }
                    else if ((mouseX - 450) * (mouseX - 450) + (mouseY - 300) * (mouseY - 300) <= 25 * 25) {
                        hoverState = 3;  // 人类数量减
                    }
                    else if ((mouseX - 850) * (mouseX - 850) + (mouseY - 300) * (mouseY - 300) <= 25 * 25) {
                        hoverState = 4;  // 人类数量加
                    }
                    else {
                        hoverState = 0;  // 无悬停
                    }
                }
                // 处理鼠标点击
                else if (msg.message == WM_LBUTTONDOWN) {
                    // 玩家人数减
                    if (hoverState == 1 && pokergame.playerCount > 3) {
                        pokergame.playerCount--;
                        if (pokergame.humanCount > pokergame.playerCount) {
                            pokergame.humanCount = pokergame.playerCount;
                        }
                    }
                    // 玩家人数加
                    else if (hoverState == 2 && pokergame.playerCount < 5) {
                        pokergame.playerCount++;
                    }
                    // 人类数量减
                    else if (hoverState == 3 && pokergame.humanCount > 1) {
                        pokergame.humanCount--;
                    }
                    // 人类数量加
                    else if (hoverState == 4 && pokergame.humanCount < pokergame.playerCount) {
                        pokergame.humanCount++;
                    }
                }
            }

            // 绘制标题
            settextstyle(100, 0, _T("楷体"));
            settextcolor(WHITE);
            x = textwidth(_T("游戏设置"));
            outtextxy((1080 - x) / 2, 10, _T("游戏设置"));

            // 游戏设置提示文字
            settextstyle(80, 0, _T("黑体"));
            settextcolor(RGB(251, 234, 208));
            setfillcolor(RGB(251, 234, 208));

            outtextxy(40, 140, _T("玩家人数："));
            outtextxy(40, 260, _T("真人数  ："));
            outtextxy(40, 380, _T("名字设置："));

            // 绘制玩家人数设置区域
            // 减号按钮 - 根据悬停状态设置颜色
            if (hoverState == 1) {
                setfillcolor(RGB(255, 213, 169)); // 选中色
            }
            else {
                setfillcolor(RGB(251, 234, 208));
            }
            solidcircle(450, 180, 25);

            // 数字显示矩形
            setfillcolor(RGB(251, 234, 208));
            solidrectangle(500, 140, 800, 220);

            // 加号按钮 - 根据悬停状态设置颜色
            if (hoverState == 2) {
                setfillcolor(RGB(255, 213, 169)); // 选中色
            }
            else {
                setfillcolor(RGB(251, 234, 208));
            }
            solidcircle(850, 180, 25);

            // 绘制人类数量设置区域
            // 减号按钮 - 根据悬停状态设置颜色
            if (hoverState == 3) {
                setfillcolor(RGB(255, 213, 169)); // 选中色
            }
            else {
                setfillcolor(RGB(251, 234, 208));
            }
            solidcircle(450, 300, 25);

            // 数字显示矩形
            setfillcolor(RGB(251, 234, 208));
            solidrectangle(500, 260, 800, 340);

            // 加号按钮 - 根据悬停状态设置颜色
            if (hoverState == 4) {
                setfillcolor(RGB(255, 213, 169)); // 选中色
            }
            else {
                setfillcolor(RGB(251, 234, 208));
            }
            solidcircle(850, 300, 25);

            // 名字设置区域（保持不变）
            setfillcolor(RGB(251, 234, 208));
            solidcircle(450, 420, 25);
            solidrectangle(500, 380, 800, 460);
            solidcircle(850, 420, 25);

            // 辅助符号绘制
            settextstyle(50, 0, _T("Arial"));
            settextcolor(BLACK);

            // 玩家人数加减符号
            outtextxy(440, 155, _T("-"));
            outtextxy(840, 155, _T("+"));

            // 人类数量加减符号
            outtextxy(440, 275, _T("-"));
            outtextxy(840, 275, _T("+"));

            // 名字设置加减符号
            outtextxy(440, 395, _T("-"));
            outtextxy(840, 395, _T("+"));
            outtextxy(500, 400, _T("暂无此功能"));

            // 显示数字
            settextstyle(60, 0, _T("Arial"));
            settextcolor(BLACK);

            // 显示玩家人数
            TCHAR playercountText[10];
            _stprintf_s(playercountText, sizeof(playercountText) , _T("%d"), pokergame.playerCount);
            int playerTextWidth = textwidth(playercountText);
            outtextxy(500 + (300 - playerTextWidth) / 2, 150, playercountText);

            // 显示人类数量
            TCHAR humanText[10];
            _stprintf_s(humanText, sizeof(humanText), _T("%d"), pokergame.humanCount);
            int humanTextWidth = textwidth(humanText);
            outtextxy(500 + (300 - humanTextWidth) / 2, 270, humanText);

            FlushBatchDraw();
        }

        // 规则界面
else if (currentscreen == rule) {
    cleardevice();

    // 绘制标题
    settextstyle(100, 0, _T("楷体"));
    settextcolor(WHITE);
    x = textwidth(_T("游戏规则"));
    outtextxy((1080 - x) / 2, 10, _T("游戏规则"));

    // 绘制返回提示
    settextstyle(30, 0, _T("宋体"));
    settextcolor(RGB(200, 200, 200));
    outtextxy(50, 600, _T("按ESC或Backspace返回主菜单"));

    // 设置规则正文样式
    settextstyle(25, 0, _T("宋体"));
    settextcolor(RGB(240, 240, 240));

    // 规则内容数组
    const TCHAR* rules[] = {
        _T("基本规则："),
        _T("• 每名玩家初始筹码为100，每次下注金额不小于10"),
        _T("• 每轮游戏开始后每名玩家上交5筹码作为底注"),
        _T("• 玩家获得5张初始手牌，场上展示1张公共牌，另1张牌面向下"),
        _T("• 此时可选择进行一次换牌（每次只能换一张）"),
        _T("• 换牌结束后进行第一轮下注"),
        _T("• 第一轮下注后展示另1张公共牌，进行第二次换牌和下注"),
        _T("• 下注结束后玩家从5张手牌中选出3张，轮流展示手牌"),
        _T("• 与两张公共牌的组合计算分数，最高者获得所有筹码"),
        _T("• 剩余两张手牌不展示并保留到下一轮游戏"),
        _T("• 若有多名得分最高者，则平分底池"),
        _T(""),
        _T("手牌获得规则："),
        _T("• 第一轮游戏所有玩家获得5张手牌"),
        _T("• 后续轮次："),
        _T("  - 若前一轮未弃牌，玩家获得3张新手牌"),
        _T("  - 若前一轮弃牌，玩家获得5张新手牌"),
        _T(""),
        _T("下注操作："),
        _T("• 弃牌：随时可选，筹码-5并加入底池"),
        _T("• 过牌：无人下注时可选，不下注"),
        _T("• 下注：无人下注时可选，不小于10"),
        _T("• 跟住：有人下注时可选，下注筹码数与上一人相同"),
        _T("• 加注：有人下注时可选，下注筹码数大于上一人"),
        _T("• 全押：筹码不足以跟注或下注时可选"),
        _T(""),
        _T("特殊规则："),
        _T("• 若玩家在一次行动中过牌，下次行动中不得再进行过牌"),
        _T("• 若一次行动中所有玩家均过牌，则下次行动的第一人必须下注"),
        _T("• 当场上只剩两名玩家持有筹码时游戏结束"),
        _T("• 持筹码数多者获胜")
    };

    int ruleCount = sizeof(rules) / sizeof(rules[0]);
    int lineHeight = 40;  // 行间距
    int startY = 130;     // 起始Y坐标
    int pageHeight = 15;  // 每页显示的行数

    static int scrollOffset = 0;  // 滚动偏移量

    ExMessage msg;   // 鼠标及键盘信息结构体

    while (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
        if (msg.message == WM_KEYDOWN) {
            switch (msg.vkcode) {
            case VK_BACK:
            case VK_ESCAPE:
                currentscreen = mainscreen;
                scrollOffset = 0;  // 重置滚动位置
                break;
            case VK_UP:
            case VK_PRIOR:  // Page Up
                if (scrollOffset > 0) scrollOffset--;
                break;
            case VK_DOWN:
            case VK_NEXT:   // Page Down
                if (scrollOffset < ruleCount - pageHeight) scrollOffset++;
                break;
            }
        }
        else if (msg.message == WM_MOUSEWHEEL) {
            // 鼠标滚轮滚动
            if (msg.wheel > 0 && scrollOffset > 0) {
                scrollOffset--;
            }
            else if (msg.wheel < 0 && scrollOffset < ruleCount - pageHeight) {
                scrollOffset++;
            }
        }
    }

    // 绘制规则文本（带滚动效果）
    for (int i = 0; i < pageHeight; i++) {
        int ruleIndex = i + scrollOffset;
        if (ruleIndex < ruleCount) {
            // 标题行特殊处理
            if (rules[ruleIndex][0] == _T('基') ||
                rules[ruleIndex][0] == _T('手') ||
                rules[ruleIndex][0] == _T('下') ||
                rules[ruleIndex][0] == _T('特')) {
                settextcolor(RGB(255, 215, 0));  // 金色标题
                settextstyle(30, 0, _T("楷体"));
            }
            else if (rules[ruleIndex][0] == _T('•') || rules[ruleIndex][0] == _T(' ')) {
                settextcolor(RGB(220, 220, 220));  // 浅灰色正文
                settextstyle(25, 0, _T("宋体"));
            }
            else if (rules[ruleIndex][0] == _T('-')) {
                settextcolor(RGB(200, 230, 255));  // 浅蓝色子项
                settextstyle(22, 0, _T("宋体"));
            }
            else {
                settextcolor(RGB(240, 240, 240));
                settextstyle(25, 0, _T("宋体"));
            }

            // 计算文本位置
            x = textwidth(rules[ruleIndex]);
            outtextxy((1080 - x) / 2, startY + i * lineHeight, rules[ruleIndex]);
        }
    }

    // 绘制滚动条提示
    if (ruleCount > pageHeight) {
        settextstyle(20, 0, _T("宋体"));
        settextcolor(RGB(150, 150, 150));
        outtextxy(950, 620, _T("↑↓ 滚动"));
    }

    FlushBatchDraw();
    }
    }
    
    closegraph();
    return 0;
}

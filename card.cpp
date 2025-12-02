#include "card.h"
#include <stdlib.h>
#include <time.h>
#include <graphics.h>
#include <stdio.h>

// 初始化牌堆
void initializeDeck(Deck* deck) {
    if (!deck) return;
    int index = 0;//第几张牌
    // 创建52张牌
    for (int s = 0; s <= 3; s++) {
        for (int r = 0; r <= 12; r++) {
            deck->cards[index++] = { static_cast<Suit>(s), static_cast<Rank>(r), false };
        }
    }

    deck->currentIndex = 0;
}

// 洗牌
void shuffleDeck(Deck* deck) {

    if (deck == NULL) return;  // 空指针检查
    srand((unsigned int)time(NULL));

    // 从后往前洗牌，避免重复交换
    for (int i = 51; i > 0; i--) {
        int j = rand() % (i+1 );  // 0到i-1之间的随机位置
        // 交换牌
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }

    deck->currentIndex = 0;
}

//当前能否进行发牌
bool canDeal(const Deck* deck) {
    //指针非空，牌堆非空
    return deck != NULL && deck->currentIndex < 52 && deck->currentIndex >= 0;
}

//发一张牌
Card dealCard(Deck* deck) {
    if(canDeal(deck)){
        //从当前牌堆deck中发出一张当前序列的牌
        return deck->cards[deck->currentIndex++];
    }
    else {
        resetDeck(deck);
        return dealCard(deck);
    }
}

//重置牌堆
void resetDeck(Deck* deck) {
    deck->currentIndex = 0;
    shuffleDeck(deck);  // 重新洗牌，确保随机性
}

// 获取花色符号（返回 TCHAR，与绘图 API 的字符类型一致）
const TCHAR* getSuitSymbol(Suit suit) noexcept {
    // 使用 Unicode 转义保证在任何源文件编码下都能得到正确字符（编译器将其展开为 wide char when UNICODE）
    static const TCHAR* symbols[] = {
        _T("\u2665"), // ♥ HEARTS
        _T("\u2666"), // ♦ DIAMONDS
        _T("\u2663"), // ♣ CLUBS
        _T("\u2660")  // ♠ SPADES
    };
    if (suit < HEARTS || suit > SPADES) return _T("?");
    return symbols[static_cast<int>(suit)];
}

// 获取点数符号
const TCHAR* getRankSymbol(Rank rank) noexcept {
    static const TCHAR* symbols[] = {
        _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"), _T("10"),
        _T("J"), _T("Q"), _T("K"), _T("A")
    };
    return symbols[static_cast<int>(rank)];
}

// 绘制牌背面
void drawCardBackAt(int x, int y) {
    int cardWidth = 80;
    int cardHeight = 120;

    // 红色牌背+黑色边框
    setlinecolor(BLACK);
    setlinestyle(0, 2);
    setfillcolor(RGB(173,63,62));  
    fillrectangle(x, y, x + cardWidth, y + cardHeight);

    // 牌背图案 - 中间显示问号
    settextcolor(WHITE);
    settextstyle(40, 0, _T("黑体"));
    outtextxy(x + cardWidth / 2 - 10, y + cardHeight / 2 - 20, _T("?"));

}

// 在指定位置绘制牌
void drawCardAt(int x, int y,  Card* card) {
    if (card == nullptr) return;

    const int cardWidth = 80;
    const int cardHeight = 120;

    if (!card->visible) {
        drawCardBackAt(x, y);
        return;
    }

    // 白色牌面
    setfillcolor(WHITE);
    solidrectangle(x, y, x + cardWidth, y + cardHeight);

    // 黑色边框
    setlinecolor(BLACK);
    setlinestyle(0, 2);
    rectangle(x, y, x + cardWidth, y + cardHeight);

    // 设置花色颜色：红心和方块红色，梅花和黑桃黑色
    if (card->suit == HEARTS || card->suit == DIAMONDS) {
        settextcolor(RED);
    }
    else {
        settextcolor(BLACK);
    }

    // 左上角显示点数和花色
    settextstyle(20, 0, _T("黑体"));
    TCHAR topInfo[32];
    _stprintf_s(topInfo, _T("%s%s"), getRankSymbol(card->rank), getSuitSymbol(card->suit));
    outtextxy(x + 5, y + 5, topInfo);

    // 右下角也显示（镜像）
    TCHAR bottomInfo[32];
    _stprintf_s(bottomInfo, _T("%s%s"), getRankSymbol(card->rank), getSuitSymbol(card->suit));
    outtextxy(x + cardWidth - 35, y + cardHeight - 25, bottomInfo);

    // 中间只显示大花色
    settextstyle(40, 0, _T("宋体"));
    outtextxy(x + cardWidth / 2 - 12, y + cardHeight / 2 - 20, getSuitSymbol(card->suit));

    FlushBatchDraw();
}

// 绘制选择框
void drawSelectedFrame(int cardIndex, int x, int y, Card* card, bool selected) {
    // 如果被选中，绘制黄色选择框
    if (selected) {
        setlinecolor(RGB(255, 255, 0));  // 黄色
        setlinestyle(PS_SOLID, 3);
        rectangle(x - 2, y - 2, x + 82, y + 118);  // 比牌稍大一点的框
    }
    else {
        setlinecolor(RGB(49,78,22));
        setlinestyle(PS_SOLID, 3);
        rectangle(x - 2, y - 2, x + 82, y + 118);  // 比牌稍大一点的框
    }
}

int compareCards(const void* a, const void* b) {
    const Card* cardA = (const Card*)a;
    const Card* cardB = (const Card*)b;

    // 直接比较rank值，因为你的枚举中ACE=12是最大的
    return (int)cardA->rank - (int)cardB->rank;
}

//计算5张牌得分
int GetScore(Card* cards) {
    qsort(cards, 5, sizeof(Card), compareCards);

    int score = 0;
    int cardtype = 0;

    // 1. 先检查同花和顺子
    bool isFlush = true;
    bool isStraight = true;

    for (int i = 0; i < 5; i++) {
        score += (int)(cards[i].rank) + 2;  // 基础分

        if (i > 0) {
            if (cards[i].suit != cards[0].suit) {
                isFlush = false;
            }
            if (cards[i].rank != cards[i - 1].rank + 1) {
                isStraight = false;
            }
        }
    }

    if (isFlush) cardtype |= FLUSH;
    if (isStraight) cardtype |= STRAIGHT;

    // 2. 统计点数频率
    int rankCount[13] = { 0 };
    for (int i = 0; i < 5; i++) {
        rankCount[(int)cards[i].rank]++;
    }

    bool hasPair = false, hasTwoPair = false, hasThree = false, hasFour = false;
    int pairCount = 0;

    for (int i = 0; i < 13; i++) {
        if (rankCount[i] == 2) {
            hasPair = true;
            pairCount++;
        }
        else if (rankCount[i] == 3) {
            hasThree = true;
        }
        else if (rankCount[i] == 4) {
            hasFour = true;
        }
    }

    if (pairCount >= 2) {
        hasTwoPair = true;
    }

    // 3. 设置牌型标志
    if (hasPair) cardtype |= ONE_PAIR;
    if (hasTwoPair) cardtype |= TWO_PAIR;
    if (hasThree) cardtype |= THREE_OF_A_KIND;
    if (hasFour) cardtype |= FOUR_OF_A_KIND;

    if (hasPair && hasThree) {
        cardtype |= FULL_HOUSE;
    }

    // 4. 特殊牌型
    if ((cardtype & FLUSH) && (cardtype & STRAIGHT)) {
        cardtype |= STRAIGHT_FLUSH;
    }
    if ((cardtype & STRAIGHT_FLUSH) && cards[4].rank == ACE) {
        cardtype |= ROYAL_FLUSH;
    }

    // 5. 牌型加分
    if (cardtype & ROYAL_FLUSH)
        score += 50;
    else if (cardtype & STRAIGHT_FLUSH)
        score += 40;
    else if (cardtype & FOUR_OF_A_KIND)
        score += 35;
    else if (cardtype & FULL_HOUSE)
        score += 30;
    else if (cardtype & FLUSH)
        score += 25;
    else if (cardtype & STRAIGHT)
        score += 20;
    else if (cardtype & THREE_OF_A_KIND)
        score += 15;
    else if (cardtype & TWO_PAIR)
        score += 10;
    else if (cardtype & ONE_PAIR)
        score += 5;

    return score;
}
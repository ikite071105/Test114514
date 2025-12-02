#ifndef CARD_H
#define CARD_H

#include <stdbool.h>
#include <tchar.h>

//牌型
#define HIGH_CARD       1<<0   //高牌
#define ONE_PAIR        1<<1   //一对
#define TWO_PAIR        1<<2   //两对
#define THREE_OF_A_KIND 1<<3   //三条
#define STRAIGHT        1<<4   //顺子
#define FLUSH           1<<5   //同花
#define FULL_HOUSE      1<<6   //葫芦
#define FOUR_OF_A_KIND  1<<7   //四条
#define STRAIGHT_FLUSH  1<<8   //同花顺
#define ROYAL_FLUSH     1<<9   //皇家同花顺

// 扑克牌的四种花色
typedef enum {
    HEARTS,      // 红心 ?
    DIAMONDS,    // 方块 ?  
    CLUBS,       // 梅花 ?
    SPADES,      // 黑桃 ?
    NONE         //无效牌
}Suit;

// 扑克牌点数 (2~A)
typedef enum {
    TWO, THREE, FOUR, FIVE, SIX,
    SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE,ZERO//无效牌
}Rank;

//单张牌的信息
typedef struct {
    Suit suit;
    Rank rank;
    bool visible;//是否正面显示
}Card;

// 牌堆结构（包含牌组和状态）
typedef struct {
    Card cards[52];      // 52张牌
    int currentIndex;    // 当前发牌位置
} Deck;

//函数声明
void initializeDeck(Deck* deck);  //初始化牌堆
void shuffleDeck(Deck* deck);     //洗牌
bool canDeal(const Deck* deck);   //判断 是否能发牌
Card dealCard(Deck* deck);        //发一张牌
void resetDeck(Deck* deck);       //重置牌堆并重新洗牌

// 图形绘制函数
void drawCardAt(int x, int y, Card* card);        // 在指定位置绘制牌
void drawCardBackAt(int x, int y);                // 绘制牌背
void drawSelectedFrame(int cardIndex, int x, int y, Card* card, bool selected);// 绘制选择框

// 返回宽字符（与 GUI 绘制函数使用的 TCHAR/UNICODE 保持一致）
const TCHAR* getSuitSymbol(Suit suit) noexcept;   // 获取花色符号（例如 "♥"）
const TCHAR* getRankSymbol(Rank rank) noexcept;   // 获取点数符号（例如 "A"）


//得分计算函数
int GetScore(Card* cards);  //计算5张牌得分


#endif //CARD_H
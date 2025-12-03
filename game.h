#ifndef GAME_H
#define GAME_H

#include "card.h"
#include <graphics.h>
#include <stdbool.h>
#include "screen.h"


#define MAX_PLAYERS 5

//玩家类型
typedef enum { 
	HUMAN, AI 
} PlayerType;

//玩家状态
typedef struct {
	char name[50];             //玩家名称
	PlayerType type;           //玩家类型
	Card hand[5];              //玩家手牌=
	int score;                 //玩家得分
	int chips;                 //玩家筹码数
	bool isActive;             //是否仍参与当局(未弃牌）
	bool hasChecked;           // 记录是否已经过牌
	int finalRank;             // 最终排名 (0表示未排名, 1=第一名, 2=第二名...)
	bool isOut;                //是否淘汰
} Player;

//下注阶段的操作选择
typedef enum {
	fold,  //弃牌
	check, //过牌
	bet,   //下注
	call,  //跟注
	raise, //加注
	allin  //全押
}Action;

// 游戏阶段
typedef enum {
	GAME_START,      //游戏开始/发牌
	NEW_ROUND,       //新一轮开始
	REPLACING,       // 换牌
	BETTING,         // 下注
	SHOWDOWN,        // 轮流摊牌/统计分数
	GAME_OVER        //游戏结束/结果显示
} GamePhase;



// 游戏主结构
typedef struct {
	Player players[MAX_PLAYERS]; //玩家信息
	int playerCount;             //玩家总数
	int humanCount;              //人类玩家数
	Card communityCards[2];      // 2张公共牌
	int pot;                     // 底池
	int currentBet;              // 当前回合最高下注
	int currentPlayer;           // 当前行动玩家索引
	int bottomPlayer;            //底部显示玩家
	GamePhase phase;             //当前游戏阶段
	int roundCount;              //当前轮次
	bool allChecked;             // 记录是否所有玩家都过牌
	int currentRank;             //当前出局的玩家排行
} PokerGame;

//玩家操作函数
void initPlayer(Player* player, const char* name, PlayerType type);          //玩家初始化
void dealCardsToPlayer(Player* player, Deck* deck , int CardNum);            //给玩家发牌（3/5）
Action playerBetChoice(PokerGame* game, Player* player, bool* isbetting);    //玩家下注回合选项
void playerCallorBet(PokerGame* game,Player* player, int amount);            //玩家下注或跟注
void playerRaise(PokerGame* game, Player* player, int amount,int raiseAmount); //玩家加注
bool playerReplaceCard(Player* player,  Deck* deck);                         //玩家换牌
void playerFoldCard(Player* player, PokerGame* game);                        //玩家弃牌


//游戏流程函数
void initGame(PokerGame* game, Deck* deck);                        //初始化游戏
int Showdown(PokerGame* game,Player* player, Deck* deck);          // 摊牌阶段
void endGameRound(PokerGame* game);                                // 结束当前局
void startNewRound(PokerGame* game, Deck* deck);                   //开始新一轮游戏
bool isGameOver(const PokerGame* game);                            //检查游戏是否结束


//更新筹码数
void UpdateChipsDisplay(PokerGame* game);

//AI行动相关函数
Action AIBetChoice(PokerGame* game, Player* player, bool* isbetting); //人机下注回合选项
int AIShowdown(PokerGame* game, Player* player, Deck* deck);          //人机选牌

#endif // GAME_H

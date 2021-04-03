//Git bashed file here
#ifndef GAME_H
#define GAME_H

#include "definition.h"
#include "data.h"
#include "player.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using std::vector;
using std::string;
using std::size_t;
using std::cout;
using std::cin;
using std::endl;

class Game                                                            //��FC18����ǰ��Ϸ������
{
public:
	Game() {}														  //��FC18����ǰ��Ϸ�����๹�캯��
	vector<time_t> roundTime;                                         //��FC18����Ϸÿ�غ�ʱ��   #json
	bool init(string filename, string json_filename,vector<string> players_name, std::ofstream& cmdF,std::ofstream& infoF);  //@@@��FC18�����ļ�������Ϣ����ʼ����Ϸ����д��0�ֵ�JSON
	DATA::Data& getData() { return data; }                            //��FC18����game���ȡ��ǰ��Ϸ��Ϣ   #json
	vector<TPlayerID>& getRank() { return Rank; }                     //��FC18����ȡ��ǰ������ҵ�����
	vector<int>  getScore();                     //��FC18����ȡ��ǰ������ҵ�Score --swm_sxt
	int getTotalPlayerNum() { return totalPlayers; }             //��FC18����ȡ��ǰ�����Ŀ
	int getTotalPlayerAlive() { return playerAlive; }
	TRound getCurrentRound() { return totalRounds; }                  //��FC18����ȡ��ǰ�غ���
	void DebugPhase();                                                //@@@��FC18�����������Ϣ
	Info generatePlayerInfo(TPlayerID id);                            //@@@��FC18���õ������ÿ��ai����dll�Ĳ���info����
	bool isValid();                                                   //��FC18���ж��Ƿ����
	//@@@��FC18�����ݹ���ÿ�غϷֹ��̵�ִ������㺯��
	void commandPhase(vector<CommandList>& command_list);             //@@@��FC18���������ָ��
	bool isAlive(TPlayerID id) { return data.players[id].isAlive(); } //��FC18���ж�����Ƿ����
	void saveJson(ofstream& infoFile);                                                  //��FC18��д����һ�ֵ�Json����
	void printJson();                                                 //��FC18����JSONд���ļ�
	void addRound() { totalRounds++; }               //@@@��FC18���غ�������
	bool goNext();                                                   //��FC18���жϻ��ܷ��������
	void setGameOver() {}

	void setPlayerID(TPlayerID id){ curPlayer = id; }				//��FC18�����õ�ǰ�غ����ID
	void printGameMap();                                             //��FC18�������ǰ��Ϸ��ͼ
	void regeneratePhase();                                          //��FC18�������׶Σ������������ָ�
	void beginPhase();                                               //��FC18����ʼ�׶Σ����������������
	void endPhase(vector<vector<Command>>& cmd2operate, TPlayerID id);             //��FC18��ÿλ���ָ������Ĳ���
	//std::ofstream& cmdFile;                                           //��FC18�����ָ�����ݵ��ļ�
	//std::ofstream& infoFile;                                          //��FC18�������Ϣ���ݵ��ļ�




protected:

	DATA::Data data;                                                  //��FC18�����е����ݴ�Ŵ�
	TRound totalRounds;                                               //��FC18����ǰ�غ���
	size_t totalPlayers;                                              //��FC18���������
	size_t playerAlive;                                               //��FC18���������
	vector<TPlayerID> Rank;                                           //��FC18�����ݹ����������


	string cmd_txt_filename;                                         //��FC18��ָ��TXT�ļ���
	string info_txt_filename;                                        //��FC18����ϢTXT�ļ���
	string mapinfo_txt_filename;                                     //��FC18����ͼ��ϢTXT�ļ���

	TPlayerID curPlayer;	//��FC18�����غ�����ִ�е���� ������
};

#endif // !GAME_H

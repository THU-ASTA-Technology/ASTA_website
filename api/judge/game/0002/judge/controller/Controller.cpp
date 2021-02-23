#include "Controller.h"
#include <ctime>
#include <math.h>
#include <cmath>
#include <algorithm>
#ifdef FC15_DEBUG
#define _COMMAND_OUTPUT_ENABLED_
#endif // FC15_DEBUG
namespace DAGAN
{
	using namespace std;

	TPoint moveDir[4] =    //�ƶ�������enum corpsMoveDirö�����Ͷ�Ӧ
	{// dx, dy
		{0,-1},      //�ϣ�-y
		{0, 1},      //�£�+y
		{-1,0},      //��-x
		{1, 0}       //�ң�+x
	};

	struct rankCmp {  //���ڸ���������Ľṹ��
		TPlayerID ID;
		TScore score;
		int CQTowerNum;//��ռ����
		int ELCorpsNum;//���������
		int CPCorpsNum;//��²������
		int randomCode;//�����
		bool operator < (rankCmp& b) {  //���صıȽ����������������
			if (score > b.score) return true;
			else if (score == b.score) {
				if (CQTowerNum > b.CQTowerNum) return true;
				else if (CQTowerNum == b.CQTowerNum) {
					if (ELCorpsNum > b.ELCorpsNum) return true;
					else if (ELCorpsNum == b.ELCorpsNum) {
						if (CPCorpsNum > b.CPCorpsNum) return true;
						else {
							return (randomCode > b.randomCode);   //��0,1�䷵��������������������
						}
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
	};


	struct configCmdTool { //дJson�ĵ���Cmd����ͳһ�ӿ���Ҫ
		int from_id = -1;
		int cm_type = -1;
		int getcm_id = -1;
		int aim_x = -1;
		int aim_z = -1;
		int result = 0;
		int dT = -1;
		int pT = -1;
		int another_id = -1;
	};

	/***********************************************************************************************
	*������ :��FC18��run����һغ����к���
	*������������ : ���ɵ�ǰ��Ϸ��Ϣ�������ai���������������֮������ִ�У�ִ��ÿ����Ч������޸�
					Data�޸�DATA::Data�����б仯�����ݣ�Ȼ����ú���jsonChange�޸�����Json�����ݣ�
					ִ�����Ҫ�ж�����Ƿ�����Լ���Ϸ�Ƿ�������С���λ��ҵ�����ȫ��ִ�������
					�޸ĳ�����Ϣ��Json���ݡ�
	*�������� : id<TPlayerID>--��ǰ���id��json_filename<char*>--Json�ļ���ǰ׺
	*��������ֵ : ��
	*���� : ������
	***********************************************************************************************/
	void Controller::run(TPlayerID id, ofstream& cmdFile,ofstream& infoFile) {  //@@@��FC18��ÿ���������ִ��
		//�����һ�غϼ�¼��Json����
		/*data->currentRoundCommandJson.clear();
		data->currentRoundPlayerJson.clear();
		data->currentRoundTowerJson.clear();
		data->currentRoundCorpsJson.clear();
		data->currentRoundMapJson.clear();*/

		//�����һ�غ������ӡ�����ʧ���������ӡ�������ʧ������
		data->newTower.clear();
		data->dieTower.clear();
		data->newCorps.clear();
		data->dieCorps.clear();
		data->changeTowers.clear();
		data->changeCorps.clear();

		//data�ڵĻغ����ݸ���+1
		volatile TRound dataRound = data->addRound();
		//Json����data�еĻغ����ݣ���ʱgame�лغ����ݻ���������һ�غ�
		//data->currentRoundCommandJson["round"] = Json::Value(std::to_string(dataRound));
		//data->currentRoundPlayerJson["round"] = Json::Value(std::to_string(dataRound));
		//data->currentRoundTowerJson["round"] = Json::Value(std::to_string(dataRound));
		//data->currentRoundCorpsJson["round"] = Json::Value(std::to_string(dataRound));

		//data->currentRoundMapJson["round"] = Json::Value(std::to_string(dataRound));
		char buffer[64];
		sprintf(buffer, "\nround %d\n", dataRound);
		cmdFile << buffer;
		infoFile << buffer;
		char bufferCmd[64];
		sprintf(bufferCmd, "#command\n");
		cmdFile << bufferCmd;

		int playerSize = game_.getTotalPlayerNum();
		volatile TRound round = game_.getCurrentRound() + 1;
#ifdef OUTPUT_DETAIL
		if (!silent_mode_) cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
#endif
		/*if (debug_mode) {
			game_.DebugPhase();      //@@@���Խ׶Σ����������Ϣ
		}*/
		game_.setPlayerID(id);		//@@@���õ�ǰ��ұ��
		game_.beginPhase();          //@@@�����׶Σ����/��/����/��ͼÿ�غϿ�ʼǰ��׼��������������,���Ѷ�FC15����ע��
		game_.regeneratePhase();     //@@@�ָ��׶Σ����/��/����/��ͼ����Ҫ���еĻָ���������,���Ѷ�FC15����ע��

		//Ϊ��ҵ�AI������������
		Info info2Player = game_.generatePlayerInfo(id);
		CommandList commands; //ѡ������
		Player_Code& player = players_[id - 1];  //ȡ����Ҷ�Ӧ��ai������
		if (player.isValid() && game_.isAlive(id))  //�������г����ai���룬�������г�����ҵ�ai����
		{
			// �������ִ�У��������ai��ȡָ��
#ifdef OUTPUT_DETAIL
			if (!silent_mode_) cout << "Calling Player " << (int)id << "'s run() method" << endl;
#endif
			//run����dll��Ȼ��Ѷ�Ӧ��myCommandList(��dll�޸�)�ش�������
			if (id == 1 || id == 2 || id == 3 || id == 4)
				player.run(info2Player);//��FC18�����䳬ʱ���ж���������������ж�
			else
				testPlayerCommand(info2Player);
			commands = info2Player.myCommandList;
		}
		else
		{
			player.kill();
			commands = CommandList();
		}
		commandRead = 0;
		//����������ı���
#ifdef OUTPUT_DETAIL
#ifdef _COMMAND_OUTPUT_ENABLED_
		if (file_output_enabled_)
			cout << "Player " << id << "'s commands:" << endl;
#endif
#endif
		/*for (Command c : commands.getCommand())
		{
			outPutCommand(id, c);
		}*/
		//ѭ��ִ���������
		set<TTowerID> towerBanned;//������ִ�в�������
		set<TCorpsID> corpsBanned;//������ִ�в����ı���
		if (commands.size() > MAX_CMD_NUM) {    //���������������ǷǷ��ģ�˵��ѡ�ָĶ��˲��ĸĵĶ�����ֱ�ӳ���
			data->players[id - 1].Kill();
#ifdef OUTPUT_DETAIL
			cout << "Player " << id << " break the rules! Out!" << endl;
#endif
		}
		if (id == 1)
			int a = 0;
		//�غ�������1
		game_.addRound();
		//ִ�б��غ�����
		for (Command c : commands.getCommand()) {
			commandRead++;  //���¶�ȡָ��������Ч����Чָ�Ҫ��ȡ
			if (c.cmdType == corpsCommand) {
				if (c.parameters.size() != CorpsOperaNumNeed[c.parameters[0]]) continue;   //�жϲ������Ϸ���
				if (data->newCorps.find(c.parameters[1]) != data->newCorps.end() || corpsBanned.find(c.parameters[1]) != corpsBanned.end()) continue;     //������ű��غϲ���(��)���ܲ��������󲵻�
				if (handleCorpsCommand(id, c) == true) {   //��¼�����ٽ������������ı������
					jsonChange(id, c,cmdFile);   //������Ч��ָ��Json
					//outPutCommand(id, c);  //������ִ�е����δִ�еĲ�����
					switch (c.parameters[0]) {
					//case(CStation):
					case(CStationTower):
					case(CBuild):
					case(CRepair):
					case(CChangeTerrain):
						corpsBanned.insert(c.parameters[1]);//��¼���ܼ��������ı���ID
						break;
					default:;
					}
				}
				else continue; //ָ��ִ��ʧ�ܣ���������ȡ��һ��
			}
			else if (c.cmdType == towerCommand) {
				if (c.parameters.size() != towerOperaNumNeed[c.parameters[0]]) continue;   //�жϲ������������Ƿ�Ϸ�
				if (data->newTower.find(c.parameters[1]) != data->newTower.end() || towerBanned.find(c.parameters[1]) != towerBanned.end()) continue; //�������ǰ�غϲ���(��)���������󲵻�
				if (handleTowerCommand(id, c) == true) {   //��¼�����ٽ������������������
					jsonChange(id, c,cmdFile);   //������Ч��ָ��Json
				    //outPutCommand(id, c);  //������ִ�е����δִ�еĲ�����
					switch (c.parameters[0]) {
					case(TProduct):
					case(TAttackCorps):
						towerBanned.insert(c.parameters[1]);//��¼���ܼ�����������ID
						break;
					default:;
					}
				}
				else continue; //ָ��ִ��ʧ�ܣ���������ȡ��һ��
			}
			else continue;   //ָ������ֱ�Ӷ���
			//ָ������ִ�к�Żᵽ����
			killPlayers();  //�ж�������ҳ���
			if (game_.goNext() == false) {   //���ÿ�������Ч����Ϸ�������˳���ȡ�����ѭ��
				setValid(false);  //�ر�Controller���ж���Ϸ��ֱ��game_over
				break;
			}
			if (moreCommand(id, towerBanned, corpsBanned) == false) break;  //���ղ��˸��������ˣ�ֱ������
		}
		game_.endPhase(Corpslastcmd,id);
		getGameRank();   //��ȡ��Ϸ���������

		isValid_ = game_.isValid();
		if (!isValid())
		{
			if (!silent_mode_)
			{
#ifdef OUTPUT_DETAIL
				cout << "-=-=-=-=-=-=-=-=-=-=-= GAME OVER ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
				cout << "Rank:" << endl;
#endif
				int rank = 0;
				for (TPlayerID r : game_.getRank())
				{
					//
#ifdef OUTPUT_DETAIL
					cout << "Rank " << ++rank << " : player " << players_[r - 1].getName() << endl;
#endif
				}
				for (int playerID = 1; playerID <= 4; playerID++) {
#ifdef OUTPUT_DETAIL
					cout << "ID: " << playerID << " score: " << data->players[playerID - 1].getScore() << " deadround: " << data->players[playerID - 1].getdeadRound() << "\n";
#endif
				}
			}
		}


		//#json save������Json�浵����
		{
			game_.roundTime.push_back(clock());
			//data->currentRoundCommandJson["runDuration"] =
				//Json::Value(std::to_string(int(game_.roundTime[game_.roundTime.size() - 1] - game_.roundTime[game_.roundTime.size() - 2])));
			game_.saveJson(infoFile);//���漰д��Json�ĵ�
		}
	}

	/***********************************************************************************************
	*������ :��FC18��jsonChange����Json�޸ĺ���
	*������������ : ��IDΪid������´������c¼�뵱ǰ�غ�����Json��
	                ע�⣬����ҵ������ǰ����������˳������������
					�ġ����ҵ��ñ�����ǰ��ȷ������ִ�й���Ҳ����Ҫȷ
					��ÿ���������Ϸ��������볡�������Ӧ��
	*�������� : id<TPlayerID>--��ǰ��ҵ�ID��c<Command&>--��ǰָ��
	*��������ֵ : ��
	*���� : ������
	***********************************************************************************************/
	
	
	void Controller::jsonChange(TPlayerID id, Command& c, ofstream& cmdFile) {
		configCmdTool newCmd;
		newCmd.from_id = id;
		newCmd.getcm_id = c.parameters[1];
		if (c.cmdType == corpsCommand) {
			//Json::Value newCmd;
			//Json::Value pos;
			TPoint point = data->myCorps[c.parameters[1]].getPos();
			TPoint point2;
			//pos["x"] = Json::Value(std::to_string(point.m_x));
			//pos["z"] = Json::Value(std::to_string(point.m_y));
			//newCmd["spot"] = pos;
			//newCmd["oId"] = Json::Value(std::to_string(id));
			//newCmd["cT"] = Json::Value(std::to_string(int(corpsCommand)));
			//newCmd["tp"] = Json::Value(std::to_string(int(c.parameters[0])));
			//newCmd["id"] = Json::Value(std::to_string(int(c.parameters[1])));
			switch (c.parameters[0])  //��0������
			{
			case(CMove):
				newCmd.cm_type = JMove;
				newCmd.aim_x = point.m_x /*+ DAGAN::moveDir[c.parameters[2]].m_x*/;
				newCmd.aim_z = point.m_y /*+ DAGAN::moveDir[c.parameters[2]].m_y*/;
				//newCmd["mv"] = Json::Value(std::to_string(int(c.parameters[2])));
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(DAGAN::moveDir[c.parameters[2]].m_y, DAGAN::moveDir[c.parameters[2]].m_x)));
				break;
			/*case(CStation):
				newCmd.cm_type = JStation;
				break;*/
			case(CStationTower):
				newCmd.cm_type = JStationTower;
				break;
			case(CAttackCorps):
				newCmd.cm_type = JAttackCorps;
				//point = data->myCorps[c.parameters[1]].getPos();
				point2 = data->myCorps[c.parameters[2]].getPos();
				newCmd.aim_x = point2.m_x;
				newCmd.aim_z = point2.m_y;
				newCmd.another_id = c.parameters[2];
				//pos["x"] = Json::Value(std::to_string(point2.m_x));
				//pos["z"] = Json::Value(std::to_string(point2.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dEC"] = Json::Value(std::to_string(int(c.parameters[2])));
				//TPoint dirTPoint = point2 - point;
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(dirTPoint.m_y, dirTPoint.m_x)));
				if (data->dieCorps.find(c.parameters[1]) == data->dieCorps.end() && data->dieCorps.find(c.parameters[2]) == data->dieCorps.end()) newCmd.result = 0;     //������
				else if (data->dieCorps.find(c.parameters[1]) == data->dieCorps.end() && data->dieCorps.find(c.parameters[2]) != data->dieCorps.end()) newCmd.result = 1;    //�Լ����ڣ��Է�û��
				else if (data->dieCorps.find(c.parameters[1]) != data->dieCorps.end() && data->dieCorps.find(c.parameters[2]) == data->dieCorps.end()) newCmd.result = 2;    //�Լ�û�ˣ��Է�����
				else  newCmd.result = 3;    //��û��
				break;
			case(CAttackTower):
				newCmd.cm_type = JAttackTower;
				point2 = data->myTowers[c.parameters[2]].getPosition();
				newCmd.aim_x = point2.m_x;
				newCmd.aim_z = point2.m_y;
				newCmd.another_id = c.parameters[2];
				//pos["x"] = Json::Value(std::to_string(point2.m_x));
				//pos["z"] = Json::Value(std::to_string(point2.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dET"] = Json::Value(std::to_string(int(c.parameters[2])));
				//TPoint dirTPoint = point2 - point;
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(dirTPoint.m_y, dirTPoint.m_x)));
				if (data->dieCorps.find(c.parameters[1]) == data->dieCorps.end() && data->dieTower.find(c.parameters[2]) == data->dieTower.end()) newCmd.result = 0;    //������
				else if (data->dieCorps.find(c.parameters[1]) == data->dieCorps.end() && data->dieTower.find(c.parameters[2]) != data->dieTower.end()) newCmd.result = 1;     //�Լ����ڣ��Է�û��
				else if (data->dieCorps.find(c.parameters[1]) != data->dieCorps.end() && data->dieTower.find(c.parameters[2]) == data->dieTower.end()) newCmd.result = 2;     //�Լ�û�ˣ��Է�����
				else  newCmd.result = 3;    //��û��
				break;
			/*case(CRegroup):
				newCmd.cm_type = JRegroup;
				//TPoint point2 = data->myCorps[c.parameters[2]].getPos();
				newCmd.another_id = c.parameters[2];
				//pos["x"] = Json::Value(std::to_string(point2.m_x));
				//pos["z"] = Json::Value(std::to_string(point2.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dFC"] = Json::Value(std::to_string(int(c.parameters[2])));
				//if (data->dieCorps.find(c.parameters[2]) != data->dieCorps.end()) newCmd.result = -1;
				break;*/
			case(CBuild):
				newCmd.cm_type = JBuild;
				newCmd.another_id = data->myTowers.size() - 1;
				break;
			case(CRepair):
				newCmd.cm_type = JRepair;
				break;
			case(CChangeTerrain):
				newCmd.cm_type = JChangeTerrain;
				//newCmd["dT"] = Json::Value(std::to_string(int(c.parameters[2])));
				newCmd.dT = c.parameters[2];
				break;
			default:return;
			}
			//data->currentRoundCommandJson["command"].append(newCmd);
		}
		else if (c.cmdType == towerCommand) {
			TPoint point3;
			//Json::Value newCmd;
			//Json::Value pos;
			//newCmd["oId"] = Json::Value(std::to_string(id));
			//newCmd["cT"] = Json::Value(std::to_string(int(towerCommand)));
			//newCmd["tp"] = Json::Value(std::to_string(int(c.parameters[0])));
			//newCmd["id"] = Json::Value(std::to_string(int(c.parameters[1])));
			switch (c.parameters[0]) 
			{
			case(TProduct):
				newCmd.cm_type = JProduct;
				newCmd.pT = c.parameters[2];
				//newCmd["pT"] = Json::Value(std::to_string(int(c.parameters[2])));
				break;
			case(TAttackCorps):
				newCmd.cm_type = JTowerAttackCorps;
				point3 = data->myCorps[c.parameters[2]].getPos();
				newCmd.aim_x = point3.m_x;
				newCmd.aim_z = point3.m_y;
				newCmd.another_id = c.parameters[2];
				if (data->dieCorps.find(c.parameters[2]) != data->dieCorps.end()) newCmd.result = 1;   //�Է�û��
				//pos["x"] = Json::Value(std::to_string(point.m_x));
				//pos["z"] = Json::Value(std::to_string(point.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dEC"] = Json::Value(std::to_string(int(c.parameters[2])));
				//TPoint dirTPoint = point2 - data->myTowers[c.parameters[1]].getPosition();
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(dirTPoint.m_y, dirTPoint.m_x)));
				break;
			//case(TAttackTower):
				//TPoint point = data->myTowers[c.parameters[2]].getPosition();
				//pos["x"] = Json::Value(std::to_string(point.m_x));
				//pos["z"] = Json::Value(std::to_string(point.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dET"] = Json::Value(std::to_string(int(c.parameters[2])));
				//TPoint dirTPoint = point - data->myTowers[c.parameters[1]].getPosition();
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(dirTPoint.m_y, dirTPoint.m_x)));
				//break;
			default:return;
			}
			//data->currentRoundCommandJson["command"].append(newCmd);
		}
		/*Json::Value newCommand;
		newCommand["oId"] = newCmd.from_id;
		newCommand["tp"] = newCmd.cm_type;
		newCommand["id"] = newCmd.getcm_id;
		newCommand["x"] = newCmd.aim_x;
		newCommand["z"] = newCmd.aim_z;
		newCommand["rst"] = newCmd.result;
		newCommand["dT"] = newCmd.dT;
		newCommand["pT"] = newCmd.pT;
		newCommand["aid"] = newCmd.another_id;
		data->currentRoundCommandJson["command"].append(newCommand);*/
		char bufferCmd[512];
		sprintf(bufferCmd, "%d %d %d %d %d %d %d %d %d\n", newCmd.from_id,
			                                               newCmd.cm_type, 
			                                               newCmd.getcm_id, 
			                                               newCmd.aim_x, 
			                                               newCmd.aim_z,
			                                               newCmd.result,
			                                               newCmd.dT,
			                                               newCmd.pT,
			                                               newCmd.another_id);
		cmdFile << bufferCmd;
	}
	/***********************************************************************************************
	*������ :��FC18��moreCommand�ж���һ��ܷ������һ��ָ��
	*������������ : �������ٲ��������������������һ��|���������ִ����������
	                ���̱��Ų����ٲ������������̱����Ѿ��´﹤������|��פ��
					ս�����Ų����ٲ���������ս��������פ��
					����������������һ�����ߣ�
					�Ѿ�ִ�еĲ������ﵽ�����������Ͳ��ٽ�������
	*�������� : id<TPlayerID>---���id��tBanned<set<TTowerID>&>---����ִ�в�������ID��cBanned
	            <set<TCorpsID>&>---����ִ�в����ı���ID
	*��������ֵ : <bool>---�ܷ����һ��ָ�false---���ܣ�true---��
	*���� : ������
	***********************************************************************************************/
	bool Controller::moreCommand(TPlayerID id, set<TTowerID>& tBanned, set<TCorpsID>& cBanned) {
		bool towerFree = false;  //�ܽ���������
		bool corpsFree = false;  //�ܽ��б��Ų���
		for (TTowerID i : data->players[id - 1].getTower()) {
			if (tBanned.find(i) == tBanned.end())//�û��ִ���������ܽ��в�����
			{
				towerFree = true;
				break;
			}
		}
		if (!towerFree)   //�������ܲ�����
		{
			for (TCorpsID i : data->players[id - 1].getCrops()) {
				if (cBanned.find(i) == cBanned.end())//�û��ִ�ı��Ż����ܽ��в�����
				{
					corpsFree = true;
					break;
				}
			}
		}
		if (commandRead >= MAX_CMD_NUM || (!towerFree && !corpsFree))//���������������������û�пɲ����Ե����������
			return false;
		else return true;
	}


	/***********************************************************************************************
	*������ :��FC18��killPlayers�ж���ҳ��ֺ���
	*������������ : �����ĸ���ҷ�����������0����������
	*�������� : ��
	*��������ֵ : ��
	*���� : ������
	***********************************************************************************************/
	void Controller::killPlayers() {
		for (int i = 0; i < 4; i++) {
			if (data->players[i].isAlive() == true && data->players[i].getTower().size() <= 0)  //û�з����������ֱ�ӳ��֣�����ֻغϵı�ǩ
			{
				data->players[i].Kill();
			}
		}
	}


	/***********************************************************************************************
	*������ :��FC18��outPutCommandָ���������
	*������������ : �����ǰ���ʵ��ִ�е�ָ��
	*�������� : ��
	*��������ֵ : ��
	*���� : ������
	***********************************************************************************************/
	void Controller::outPutCommand(TPlayerID id, Command& c) {
		// ֱ���������ҵĲ���
#ifdef OUTPUT_DETAIL
#ifdef _COMMAND_OUTPUT_ENABLED_
		if (file_output_enabled_ && game_.isAlive(id))
		{
			if (c.cmdType == corpsCommand) {
				cout << "corps " << c.parameters[1] << " " << CorpsCmd[c.parameters[0]];
				switch (c.parameters[0]) {
				case(CMove):
					cout << " " << Direction[c.parameters[2]];
					break;
				//case(CStation):
				case(CBuild):
				case(CRepair):
				case(CStationTower):
					cout << " at (" << data->myCorps[c.parameters[1]].getPos().m_x << "," << data->myCorps[c.parameters[1]].getPos().m_y << ")";
					break;
				case(CAttackCorps):
				case(CAttackTower):
					cout << " " << c.parameters[2];
					break;
				/*case(CRegroup):
					cout << " with corps " << c.parameters[2];
					break;*/
				case(CChangeTerrain):
					cout << " of (" << data->myCorps[c.parameters[1]].getPos().m_x << "," << data->myCorps[c.parameters[1]].getPos().m_y << ")" << " to " << Terrain[c.parameters[2] - 1];
					break;
				default:;
				}
			}
			else if (c.cmdType == towerCommand) {
				cout << "tower " << c.parameters[1] << " " << TowerCmd[c.parameters[0]];
				switch (c.parameters[0]) {
				case(TProduct):
					cout << " " << ProductCmd[c.parameters[2]];
					break;
				case(TAttackCorps):
				case(TAttackTower):
					cout << " " << c.parameters[2];
					break;
				default:;
				}
			}
			cout << ".";
		}
		if (file_output_enabled_) cout  << endl;
#endif
#endif
	}

	/***********************************************************************************************
	*������ :��FC18��handleCorpsCommand����ָ��ִ�к���
	*������������ : ִ�е�ǰ����ָ��������Ƿ�ִ�гɹ�
	*�������� : ��id<TPlayerID>---��ǰ���ID,c<Command&>---��ǰָ
	*��������ֵ : <bool>ָ��ִ�����false---ִ�гɹ���true---û��ִ�гɹ�
	*���� : ������
	***********************************************************************************************/
	bool Controller::handleCorpsCommand(TPlayerID ID, Command& c) {
		//��Ҫreturn��������ִ���Ƿ�ɹ�<bool>
		bool bCmdSucs = false;
		TCorpsID id = c.parameters[1];
		if (c.parameters[0] == CMove && ID == 2)
			int a = 0;
		if (ID == 2 && data->myCorps[id].getType()!=Construct)
			int a = 0;
		//�������idԽ��
		if(id < 0 || id >= data->myCorps.size())
			return false;
		//�������id�����ڸ����
		if(data->myCorps[id].getPlayerID() != ID)
		{
			return false;
		}

		//�������������ж�����ʧ��
		if (data->myCorps[id].bAlive() == false)
		{
			return false;
		}

		//data->myCorps[id].haveCmd();
		switch (c.parameters[0]) {
		case(CMove):
			//�����ƶ��Ĳ���
			{
				int dir = c.parameters[2];
				bCmdSucs = data->myCorps[id].Move(dir);
			}
			break;
			
		/*case(CStation)://����û������ֵ�ı�
			//����ԭ��פ���Ĳ���
			{
				bCmdSucs = data->myCorps[id].GoRest();
			}
			break;
			*/
			/*�ò��������ڣ������Զ�פ��
		case(CStationTower)://����û������ֵ�ı�
			//����פ����ǰ��������Ĳ���
			{
				bCmdSucs = data->myCorps[id].StationInTower();
			}
			break;
			*/
		case(CAttackCorps):
		case(CAttackTower):
			//���Ź����Ĳ���
			{
				int type = c.parameters[0];
				TCorpsID enemyid = c.parameters[2];
				if (type == CAttackCorps && (enemyid < 0 || enemyid >= data->myCorps.size())) return false;   //Խ�磺�������Ƿ�
				if (type == CAttackTower && (enemyid < 0 || enemyid >= data->myTowers.size())) return false;   //Խ�磺�������Ƿ�
				bCmdSucs = data->myCorps[id].Attack(type,enemyid,c);
			}
			break;
		/*case(CRegroup)://��������Ͳ�Ҫ��
			//��������Ĳ���
			{
				TCorpsID target = c.parameters[2];
				if (target < 0 || target >= data->myCorps.size()) return false;     //Խ�磺�������Ƿ�
				bCmdSucs = data->myCorps[id].MergeCrops(target);
			}
			break;*/
		case(CBuild):
			//�����޽������Ĳ���
			{
				if (data->players[ID - 1].towerNumControl() == true) 
					return false;
				bCmdSucs = data->myCorps[id].BuildTower();
			}
			break;
		case(CRepair):
			//��������������Ĳ���
			{
				bCmdSucs = data->myCorps[id].MendTower();
			}
			break;
		case(CChangeTerrain):
			//���Ÿı䷽����εĲ���
			{
				//������β���������Ҫ��
				if(c.parameters[2] < 1 || c.parameters[2] > 5)
				{
					return false;
				}
				//terrainType type = (terrainType)(c.parameters[2]);
				//bCmdSucs = data->myCorps[id].ChangeTerrain(type);
				terrainType type = (terrainType)(c.parameters[2]);
				bCmdSucs = data->myCorps[id].JudgeChangeTerrain(c);
				if (bCmdSucs)
				{
					Corpslastcmd[ID].push_back(c);
				}
			}
			break;
		default:
			return false;
		}
		return bCmdSucs;
	}

	/***********************************************************************************************
	*������ :��FC18��handleTowerCommand������ָ��ִ�к���
	*������������ : ִ�е�ǰ������ָ��������Ƿ�ִ�гɹ�
	*�������� : id<TPlayerID>---��ǰ���ID,c<Command&>---��ǰָ��
	*��������ֵ : <bool>ָ��ִ�����false---ִ�гɹ���true---û��ִ�гɹ�
	*���� : ������
	***********************************************************************************************/
	bool Controller::handleTowerCommand(TPlayerID ID, Command& c) {
		//��Ҫreturn��������ִ���Ƿ�ɹ�<bool>
		bool bCmdSucs = false;
		TCorpsID id = c.parameters[1];
		int pdtType = c.parameters[2];
		//��idԽ��
		if (id < 0 || id >= data->myTowers.size())
			return false;  
		if (data->myTowers[id].getexsit() == false)
			return false;
		switch (c.parameters[0]) {
		case(TProduct):
			if ((pdtType == PWarrior || pdtType == PArcher || pdtType == PCavalry) && data->players[ID - 1].battleNumControl() == true) return false;
			else if ((pdtType == PBuilder || pdtType == PExtender) && data->players[ID - 1].constructNumControl() == true) return false;
			//�������������������಻Խ����ж��ɹ�
			bCmdSucs = data->myTowers[id].set_producttype(productType(pdtType));
			return bCmdSucs;
			break;
		case(TAttackCorps):
			//by lxj�������ڰ������ж�
			//���ù���Ŀ��
			//Խ�磺�������Ƿ�
			//if (c.parameters[2] < 0 || c.parameters[2] >= data->myCorps.size()) 
			//	return false;   
			bCmdSucs = data->myTowers[id].set_attacktarget(c.parameters[2]);
			return bCmdSucs;
			break;
		default:
			return false;
		}
		return bCmdSucs;
	}
	/***********************************************************************************************
	*������ :��FC18��getGameRank��ȡ������Ϸ������������
	*������������ : ���������Ϸ�������õ���������ŵ�Game::Rank����
	                ��һ�ؼ���---��ҵ÷֣��ڶ��ؼ���---���ռ�����������
					�����ؼ���---�������з����Ÿ��������Ĺؼ���---��ҷ�²�з����Ÿ���
					֮���������
	*�������� : ��
	*��������ֵ : ��
	*���� : ������
	***********************************************************************************************/
	void Controller::getGameRank() {
		vector<rankCmp> Ranker;
		for (int i = 0; i < 4; i++) {
			rankCmp playerRanker;
			Player newPlayer = data->players[i];
			playerRanker.ID = newPlayer.getId();
			playerRanker.score = newPlayer.getPlayerScore();
			playerRanker.CQTowerNum = newPlayer.getCqTowerNum();
			playerRanker.ELCorpsNum = newPlayer.getElCorpsNum();
			playerRanker.CPCorpsNum = newPlayer.getCqCorpsNum();
			bool ok = false;
			do {
				ok = false;
				playerRanker.randomCode = generateRanInt(0, 1000);
				for (int cnt = 0; cnt < i; cnt++) {
					if (playerRanker.randomCode == Ranker[cnt].randomCode) {
						ok = true;
						break;
					}
				}
			} while (ok);
			Ranker.push_back(playerRanker);
			data->players[i].setScore(playerRanker.score);
		}
		std::sort(Ranker.begin(), Ranker.end());  //��������򣬰�������������
		for (int i = 1; i <= 4; i++) {
			int Rank = 0;
			for (int j = 0; j < 4; j++) {
				if (Ranker[j].ID == i) {
					Rank = j + 1;
					break;
				}
			}
			data->players[i - 1].setRank(Rank);
			game_.getRank()[Rank - 1] = Ranker[Rank - 1].ID;
		}
	}
	/***********************************************************************************************
	*������ :��FC18��testPlayerCommandģ��AI������������
	*������������ : ģ�����AI��������Ϸָ��������
	*�������� : Info& info
	*��������ֵ : ��
	*���� : ������
	***********************************************************************************************/
	void Controller::testPlayerCommand(Info& info) {
		TPlayerID m_ID = info.myID;
		for (TTowerID t : info.playerInfo[m_ID - 1].tower)
		{
			for (int i = 0; i < info.corpsInfo.size(); i++) {
				if (info.corpsInfo[i].owner == 2 && getDist(info.towerInfo[t].position, info.corpsInfo[i].pos) <= 2) {
					info.myCommandList.addCommand(towerCommand, { TAttackCorps,t,i });
					break;
				}
			}
			//	if(info.totalRounds >= 6&&info.totalRounds <= 9 && (m_ID == 1 || m_ID == 2)) info.myCommandList.addCommand(towerCommand, { TProduct,t,PArcher });   //����ҵ�����������������
			//	if (info.totalRounds >= 6&&info.totalRounds <= 9 && (m_ID == 3 || m_ID == 4)) info.myCommandList.addCommand(towerCommand, { TProduct,t,PExtender });   //����ҵ�����������������
			info.myCommandList.addCommand(towerCommand, { TProduct,t,PCavalry });
		}
		if (info.playerInfo[1].tower.size() == 0)
			return;
		TTowerID target = *(info.playerInfo[1].tower.begin());
		int dir = -1;
		for (TCorpsID t : info.playerInfo[m_ID - 1].corps)
		{
			if (data->myCorps[t].getType() != Battle)
				continue;
			if (data->myCorps[t].getPos().m_x < data->myTowers[target].getPosition().m_x)
				dir = CRight;
			else if (data->myCorps[t].getPos().m_x > data->myTowers[target].getPosition().m_x)
				dir = CLeft;
			else if (data->myCorps[t].getPos().m_y < data->myTowers[target].getPosition().m_y)
				dir = CDown;
			else if (data->myCorps[t].getPos().m_y > data->myTowers[target].getPosition().m_y)
				dir = CUp;
			if(dir!=-1)
				info.myCommandList.addCommand(corpsCommand, { CMove,t,dir });
			for(int i = 0;i<data->myCorps.size(); i++)
				if (data->myCorps[i].bAlive() && data->myCorps[i].getPlayerID() == 2)
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps,t,i });
			for (int i = 0; i < data->myTowers.size(); i++)
				if(data->myTowers[i].getexsit()&& data->myTowers[i].getPlayerID() == 2)
					info.myCommandList.addCommand(corpsCommand, { CAttackTower,t,i });
		}
		if (info.totalRounds >= 150)
			int a = 0;
		//cout << (*info.gameMapInfo)[4][5].type << "\n";
	}
}




//#2021-1-19 <JYP> ��Ϥ���룬���ע��
//#2021-1-28 <JYP> checked�������룬���Ĵ���
#include"map.h"
#include<fstream>
#include<iostream>
#include "data.h"
#include "Crops.h"
#include "tower.h"
#include "player.h"
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
using std::pair;
using std::vector;




/***********************************************************************************************
*������ :��FC18��readMap�����ͼ����
*������������ : ͨ���ļ����뵱ǰ��Ϸ�ĵ�ͼ���ݡ�����������ݣ�����ʼ��������飬д����غ�����
                Json
*�������� : inMap<ifstream&> �����ļ�������enableOutput<bool> �Ƿ����������Ϣ��true--����
            false--������
*��������ֵ : false--�����ͼʧ�ܣ�true--�����ͼ�ɹ�
*���� : ������
***********************************************************************************************/
bool Map::readMap(ifstream& inMap, ofstream& cmdF,ofstream& infoF, bool enableOutput, std::vector<std::string> players_name) {
	data->resetAllCnt();             //��ʼ������Ϊ��0�غ�
	//��ʼ����ͼ�߶ȡ����
#ifdef OUTPUT_DETAIL
	if (enableOutput)
		cout << "init map......" << endl;
#endif
	inMap >> m_height;
	inMap >> m_width;
	//data->mapInfoJsonRoot["head"]["height"] = Json::Value(std::to_string(m_height));
	//data->mapInfoJsonRoot["head"]["width"] = Json::Value(std::to_string(m_width));


	//rankInfo
	//���µ�ǰ�غϵ�������ϢJSON����֪����5�����������ΪʲôҲ��Json�������ʱ����FC15ԭ���޸���
	/*Json::Value rankInfoJson;
	Json::Value rankUnit;
	for (int i = 1; i < 5; i++) {
		rankUnit["rk"] = Json::Value(std::to_string(i));
		rankUnit["scr"] = Json::Value(std::to_string(1 * TOWER_SCORE));
		rankInfoJson.append(rankUnit);
	}
	rankUnit["rk"] = Json::Value(std::to_string(0));
	rankUnit["scr"] = Json::Value(std::to_string(-INF));
	rankInfoJson.append(rankUnit);
	data->currentRoundPlayerJson["rankInfo"] = rankInfoJson;*/


	//��ʼ����Ӫ
#ifdef OUTPUT_DETAIL
	if (enableOutput)
		cout << "init team......" << endl;
#endif
	inMap >> data->totalPlayers;
	//data->commandJsonRoot["head"]["totalPlayers"] = Json::Value(std::to_string(data->totalPlayers)); //#json
	data->players = new Player[data->totalPlayers];




	/*for (int i = 1; i < 5; i++) {
		Json::Value playerJson;
		playerJson["id"] = Json::Value(std::to_string(i));
		playerJson["team"] = Json::Value(std::to_string(i));
		playerJson["name"] = Json::Value(players_name[i]);
		data->commandJsonRoot["head"]["playerInfo"].append(playerJson);
	}*/


	//#json add
	//��ʼ�������ϢJSON
	//��ʼ����Ҳ���JSON
	//Json::Value playerActionJson; 
	for (int i = 0; i < data->totalPlayers; ++i)
	{
		data->players[i].setdata(data);
		data->players[i].setID(i + 1);
		data->players[i].setName(players_name[i]);
		/*Json::Value playerJson;
		playerJson["id"] = Json::Value(std::to_string(i + 1));
		playerJson["nm"] = Json::Value(players_name[i]);
		playerJson["cpN"] = Json::Value(std::to_string(0));
		playerJson["twN"] = Json::Value(std::to_string(1));
		playerJson["scr"] = Json::Value(std::to_string(1 * TOWER_SCORE));
		playerJson["rk"] = Json::Value(std::to_string(i + 1));
		data->currentRoundPlayerJson.append(playerJson);*/

		/*Json::Value paj;
		paj["oId"] = Json::Value(std::to_string(i + 1));
		paj["tp"] = Json::Value(std::to_string(-1));
		paj["id"] = Json::Value(std::to_string(-1));
		paj["x"] = Json::Value(std::to_string(-1));
		paj["z"] = Json::Value(std::to_string(-1));
		paj["rst"] = Json::Value(std::to_string(0));
		paj["dT"] = Json::Value(std::to_string(-1));
		paj["pT"] = Json::Value(std::to_string(-1));
		paj["aid"] = Json::Value(std::to_string(-1));
		data->currentRoundCommandJson["command"].append(paj);*/
	}


	//��ʼ�����ŵı�
	/*data->corps = new CorpsUnit*[m_height];
	for (int i = 0; i < m_height; i++) {
		data->corps[i] = new CorpsUnit[m_width];
	}*/
	char bufferCorps[64];
	sprintf(bufferCorps, "#corps\n");
	infoF << bufferCorps;

	return true;
}

/***********************************************************************************************
*������ :��FC18��randomInitMap�����ͼ������
*������������ : ��ʼ��ȷ����ͼ������֮��Ϊÿ����ͼ���������Σ����ָ������ĳ�ʼ������������
                �ĳ�ʼλ��
*�������� : ��
*��������ֵ : false--�����ͼ��ʼ��ʧ�ܣ�true--�����ͼ��ʼ���ɹ�
*���� : ������
***********************************************************************************************/
bool Map::randomInitMap(ofstream& cmdFile,ofstream& infoFile) {
	//��FC18�����ð��������������������ͼ
	Perlin perlinNoiseGen;
	int terrainArea[TERRAIN_TYPE_NUM] = { 0 };      //��FC18��ͳ��ÿ�ֵ��������˶��ٸ�
	double** perlinNoise = new double* [m_height];  //��FC18������������
	for (int i = 0; i < m_height; i++) {
		perlinNoise[i] = new double[m_width];
	}
	int** typeOfTerrain = new int* [m_height];      //��FC18������Ԥ�����
	for (int i = 0; i < m_height; i++) {
		typeOfTerrain[i] = new int[m_width];
	}

	double minNoise = INF, maxNoise = -INF;
	for (int i = 0; i < m_height; i++) {             //���ɰ�������������¼���е������С����ֵ
		for (int j = 0; j < m_width; j++) {
			typeOfTerrain[i][j] = UNALLOCATED;       //��ʼ�����и���ξ�Ϊδ����
			perlinNoise[i][j] = perlinNoiseGen.PerlinNoise(i, j);            //���ɳ�ʼ�İ�������
			if (perlinNoise[i][j] > maxNoise) maxNoise = perlinNoise[i][j];
			if (perlinNoise[i][j] < minNoise) minNoise = perlinNoise[i][j];
		}
	}
	double interval = (maxNoise - minNoise) / TERRAIN_TYPE_NUM;
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			for (int k = 0; k < TERRAIN_TYPE_NUM; k++) {
				if (((perlinNoise[i][j] - minNoise) >= k * interval) && ((perlinNoise[i][j] - minNoise) < (k + 1) * interval)) {
					typeOfTerrain[i][j] = k;
					break;
				}
			}
			if (typeOfTerrain[i][j] == UNALLOCATED) typeOfTerrain[i][j] = TERRAIN_TYPE_NUM - 1;
		}
	}
	for (int itercnt = 0; itercnt < perlinNoiseGen.iterRound; itercnt++) {
		for (int i = 1; i < m_height - 1; i++) {
			for (int j = 1; j < m_width - 1; j++) {
				int nums[TERRAIN_TYPE_NUM] = { 0 };
				for (int k = 0; k < 8; k++) {
					int newPosX = j + paraOffset[k].m_x;
					int newPosY = i + paraOffset[k].m_y;
					int scaleValue = typeOfTerrain[newPosY][newPosX];
					nums[scaleValue]++;
				}
				for (int cnt = 0; cnt < TERRAIN_TYPE_NUM; cnt++) {
					if (nums[cnt] >= perlinNoiseGen.connectStandard) typeOfTerrain[i][j] = cnt;
				}
				if (itercnt == perlinNoiseGen.iterRound - 1) {
					int currentType = typeOfTerrain[i][j];
					terrainArea[currentType]++;
				}
			}
		}
	}
	vector<pair<int, int>> areaRankPair;
	for (int i = 0; i < TERRAIN_TYPE_NUM; i++) {
		areaRankPair.push_back({i,terrainArea[i]});
	}
	std::sort(areaRankPair.begin(),areaRankPair.end(),
		[](const pair<int, int>& a, pair<int, int>& b) {return a.second > b.second; });
	std::map<int, int> areaRankMap;
	for (int i = 0; i < TERRAIN_TYPE_NUM; i++) {
		areaRankMap.insert(pair<int, int>(areaRankPair[i].first, i));
	}
	for (int i = 0; i < m_height; i++) {
		vector<mapBlock> newVectorMapBlock;
		map.push_back(newVectorMapBlock);
		for (int j = 0; j < m_width; j++) {
			mapBlock newBlock;               //�ؿ��ʼ����������ƽԭ��û�����ˣ�ռ������ֵΪ0
			newBlock.TowerIndex = NOTOWER;
			newBlock.owner = PUBLIC;
			newBlock.type = TRPlain;
			for (int k = 0; k < 4; k++) {
				newBlock.occupyPoint.push_back(0);    //�������ʼռ������ֵ��Ϊ0
			}
			map[i].push_back(newBlock);
		}
	}
	for (int i = 0; i < m_height; i++) {      //���ɵ���
		for (int j = 0; j < m_width; j++) {
			int type = typeOfTerrain[i][j];
			int rank;
			if (areaRankMap.count(type) > 0)
				rank = areaRankMap[type];
			else
				rank = 0;
			map[i][j].type = terrain[rank];
		}
	}
	for (int i = 0; i < m_height; i++) {
		delete[] perlinNoise[i];
		delete[] typeOfTerrain[i];
	}
	delete[] perlinNoise;
	delete[] typeOfTerrain;

	////////////////////////////////////////////////////////
	//��FC18������������ĳ�ʼ������ռ�ݷ��ε�ͼ���ĸ�����//
	//��ͼ������                                          //
	//      X(i=0)--------m_width---------                //
	//Y(j=0)         0     PUB      1                     //
	//  |           PUB    PUB     PUB                    //
	//  |            3     PUB      2                     //
	////////////////////////////////////////////////////////
	int Xinterval, Yinterval;         //X�����Y�����ͼ�Ĺ��������������
	Xinterval = (m_width % 2 == 0) ? 2 : 3;
	Yinterval = (m_height % 2 == 0) ? 2 : 3;
	for (int j = 0; j < m_height; j++) {
		for (int i = 0; i < m_width; i++) {
			if ((i < ((m_width - Xinterval) / 2)) && (j < ((m_height - Yinterval) / 2))) map[j][i].owner = 1;
			else if ((i > ((m_width + Xinterval) / 2) - 1) && (j < ((m_height - Yinterval) / 2))) map[j][i].owner = 2;
			else if ((i > ((m_width + Xinterval) / 2) - 1) && (j > ((m_height + Yinterval) / 2) - 1)) map[j][i].owner = 3;
			else if ((i < ((m_width - Xinterval) / 2)) && (j > ((m_height + Yinterval) / 2 - 1))) map[j][i].owner = 4;
			else map[j][i].owner = PUBLIC;
		}
	}

	////////////////////////////////////////////////////////
	//��FC18��Ϊÿ���������ɷ�����                        //
	//�����������ÿ�������ĳ�ʼ������������һ��������  //
	//ÿ����������Χ��8������Ϊ��·Road                   //
	//��֤��ʼ�����������������������Χ���Ḳ�ǵ��Է�    //
	////////////////////////////////////////////////////////
	data->totalTowers = 0;
	if (((m_width - Xinterval) < 3 * 2) || ((m_height - Yinterval) < 3 * 2)) {      //�ж��Ƿ��пռ����ɷ�����������пռ䣬��ÿ��������������������3 * 3�ķ���
#ifdef OUTPUT_DETAIL
		cout << "map size: (" << m_width << "*" << m_height << ") too small!\n";
#endif
		return false;
	}


	//��ʼ������ָ����Ϣ�浵
	char bufferCmd[64];
	sprintf(bufferCmd, "#command\n");
	cmdFile << bufferCmd;
	vector<pair<TPoint, TPoint>> towerRegion;         //��������������������λ�÷�Χ
	towerRegion.push_back({ {1,1},{(m_width - Xinterval) / 2 - 2,(m_height - Yinterval) / 2 - 2} });
	towerRegion.push_back({ {(m_width + Xinterval) / 2 + 1,1},{m_width - 2,(m_height - Yinterval) / 2 - 2} });
	towerRegion.push_back({ {(m_width + Xinterval) / 2 + 1,(m_height + Yinterval) / 2 + 1},{m_width - 2,m_height - 2} });
	towerRegion.push_back({ {1,(m_height + Yinterval) / 2 + 1},{(m_width - Xinterval) / 2 - 2,m_height - 2} });
	for (int i = 0; i < 4; i++) {
		TPoint towerPoint;
		towerPoint.m_x = generateRanInt(towerRegion[i].first.m_x, towerRegion[i].second.m_x);
		towerPoint.m_y = generateRanInt(towerRegion[i].first.m_y, towerRegion[i].second.m_y);
		map[towerPoint.m_y][towerPoint.m_x].type = TRTower;  //�ظ� tower����   //����data�ĵ�ͼ�ࣺ��ǰ����ĵ����޸�Ϊ������
		map[towerPoint.m_y][towerPoint.m_x].owner = i + 1;//�ظ� tower����
		//map[towerPoint.m_y][towerPoint.m_x].TowerIndex = i;  //�����ϵ������
		//�ظ� tower���� data->totalTowers++;                      //����data�ࣺ���·���������
		//@@@��FC18��[������������Ĺ��캯�����ܻ��]����data�ࣺ���������������������ķ�����
		Tower* newTower = new Tower(data, i + 1, towerPoint);
		data->myTowers.push_back(*newTower);
		//��FC18������player�ࣺ��player�ķ������������������µķ��������
		//�ظ� tower���� data->players[i].getTower().insert(i);
		for (int j = 0; j < 8; j++) {  //���򣺳�ʼ����������8��Ϊ��·
			TPoint p;
			p.m_x = towerPoint.m_x + paraOffset[j].m_x;
			p.m_y = towerPoint.m_y + paraOffset[j].m_y;
			//�ظ� tower���� map[p.m_y][p.m_x].occupyPoint[i] = TowerOccupyPoint[0];
			map[p.m_y][p.m_x].type = TRRoad;
		}
		//д��txt�浵
		char bufferCmd[128];
		sprintf(bufferCmd, "%d %d %d %d %d %d %d %d %d\n", i + 1, JBuild, -1, towerPoint.m_x, towerPoint.m_y, -1, -1, -1, -1);
		cmdFile << bufferCmd;
	}

	//����Ϣ�浵
	char bufferTowerHead[64];
	sprintf(bufferTowerHead, "#tower\n");
	infoFile << bufferTowerHead;
	for (int i = 0; i < data->myTowers.size(); i++) {
		Tower newTower = data->myTowers[i];
		TPoint towerPoint = newTower.getPosition();
		char bufferTower[128];
		sprintf(bufferTower, "%d %d %d %d %d %d %d %d %d %d %d\n", i
			                                                     , towerPoint.m_x
			                                                     , towerPoint.m_y
			                                                     , newTower.getPlayerID()
			                                                     , newTower.getProductPoint()
			                                                     , NOTASK
			                                                     , -1
			                                                     , newTower.getHealthPoint()
			                                                     , newTower.getBattlePoint()
			                                                     , newTower.getLevel()
			                                                     , 1);
		infoFile << bufferTower;
	}

	////////////////////////////////////////////////////////
	//��FC18��ÿ���������������ܱ߷���ʩ�ӳ�ʼռ������ֵ  //
	////////////////////////////////////////////////////////
	/*�ظ���tower����******
	for (int i = 0; i < data->myTowers.size(); i++) {
		TPoint towerPoint = data->myTowers[i].getPosition();
		modifyOccupyPoint(NOTOWER, data->myTowers[i].getOwnerID(), towerPoint);
	}
	***********************/
	/*for (int cnt = 0; cnt < 4; cnt++) {
		TPoint towerPoint = data->myTowers[cnt].getPosition();
		TPlayerID ownerID = data->myTowers[cnt].getOwnerID();
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				TPoint currentPoint;
				currentPoint.m_x = j;
				currentPoint.m_y = i;
				TDist distance = getDist(currentPoint, towerPoint);
				if (distance >= 1 && distance <= 5)
					map[i][j].occupyPoint[ownerID - 1] += TowerOccupyPoint[distance - 1];
				else                     //������ǰ���ڻ�getDist����6���Ͳ�����ռ������ֵ��
					continue;
			}
		}
	}*/
	
	//int countRound = 0, countPlayer = 0;
	char bufferMap[64];
	sprintf(bufferMap, "#map\n");
	infoFile << bufferMap;
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			char bufferBlock[64];
			int terrain = map[i][j].type;
			if (map[i][j].TowerIndex != NOTOWER) terrain = TRTower;
			sprintf(bufferBlock, "%d %d %d %d\n", j, i, terrain, map[i][j].owner);
			infoFile << bufferBlock;
			Json::Value blockJson;
			blockJson["x"] = Json::Value(j);
			blockJson["z"] = Json::Value(i);
			if (map[i][j].TowerIndex != NOTOWER)
				blockJson["tp"] = Json::Value(TRTower);
			else
				blockJson["tp"] = Json::Value(map[i][j].type);
			blockJson["oId"] = Json::Value(map[i][j].owner);
			data->currentRoundMapJson.append(blockJson);
			/*//Json::Value occupyPoint;  //�ݲ��ṩ��Json
			//for (int k = 0; k < 4; k++) {
			//	Json::Value occupyPointUnit;
			//	occupyPointUnit["id"] = Json::Value(k + 1);
			//	occupyPointUnit["point"] = Json::Value(map[i][j].occupyPoint[k]);
			//	occupyPoint.append(occupyPointUnit);
			//}
			if (map[i][j].type == TRTower) {
				Json::Value towerJson;
				towerJson["id"] = Json::Value(std::to_string(map[i][j].TowerIndex));
				//Json::Value towerPos;
				towerJson["x"] = Json::Value(std::to_string(j));
				towerJson["z"] = Json::Value(std::to_string(i));
				//towerJson["pos"] = towerPos;
				towerJson["oId"] = Json::Value(std::to_string(map[i][j].owner));
				towerJson["pP"] = Json::Value(std::to_string(TowerInitConfig[0].initBuildPoint));
				Json::Value production;
				production["pT"] = Json::Value(std::to_string(NOTASK));
				production["pC"] = Json::Value(std::to_string(-1));
				towerJson["pdt"] = production;
				towerJson["hP"] = Json::Value(std::to_string(TowerInitConfig[0].initHealthPoint));
				towerJson["bP"] = Json::Value(std::to_string(TowerInitConfig[0].initProductPoint));
				towerJson["exp"] = Json::Value(std::to_string(0));
				towerJson["sL"] = Json::Value(std::to_string(1));
				towerJson["nT"] = Json::Value(std::to_string(1));
				data->currentRoundTowerJson.append(towerJson);
				Json::Value playerJson;
				playerJson["rk"] = playerJson["tm"] = playerJson["id"] = Json::Value(std::to_string(map[i][j].owner));
				playerJson["scr"] = Json::Value(std::to_string(1 * TOWER_SCORE));
				playerJson["cpN"] = Json::Value(std::to_string(0));
				playerJson["twN"] = Json::Value(std::to_string(1));
				playerJson["tw"].append(towerJson["id"]);
				data->currentRoundPlayerJson["player"].append(playerJson);
			}*/
		}
	}

	//�����ϢTXT�浵
	char bufferInfo[64];
	sprintf(bufferInfo, "#players\n");
	infoFile << bufferInfo;
	for (int i = 0; i < data->totalPlayers; i++)
	{
		char bufferPlayer[128];
		sprintf(bufferPlayer, "%d %s %d %d %d %d\n", i + 1, data->players[i].getName().c_str(), 0, 1, 1 * TOWER_SCORE, i + 1);
		infoFile << bufferPlayer;
	}


	return true;
}

/***********************************************************************************************
*������ :��FC18��saveMapJson�����ͼJson����
*������������ : ����ǰ�غϵĵ�ͼJson���ݱ��浽��Ϸ���лغϵ�Json�����У�ͬʱ��¼�غ���
*�������� : ��
*��������ֵ : ��
*���� : ������
***********************************************************************************************/
void Map::saveMapJson() {
	//TRound round = data->getRound();      //���µ�ͼJsonǰ��¼��ǰ�غ���
	//data->currentRoundMapJson["round"] = Json::Value(std::to_string(round));
	//data->mapInfoJsonRoot["map_logList"].append(data->currentRoundMapJson);
	data->lastRoundMapJson.clear();
	data->lastRoundMapJson = data->currentRoundMapJson;
	data->currentRoundMapJson.clear();
}


/***********************************************************************************************
*������ :��FC18��ShowInfo�����ͼJson����
*������������ : ����ǰ�غϵĵ�ͼJson���ݱ��浽��Ϸ���лغϵ�Json�����У�ͬʱ��¼�غ���
*�������� : x<int>--�����꣬y<int>--������
*��������ֵ : <mapBlockInfo>--��ͼ������Ϣ
*���� : ������
***********************************************************************************************/
mapBlockInfo Map::ShowInfo(int x, int y) {
	mapBlockInfo info;
	info.type = map[y][x].type;
	info.owner = map[y][x].owner;
	info.occupyPoint = map[y][x].occupyPoint;
	info.towerIndex = map[y][x].TowerIndex;
	return info;
}

/***********************************************************************************************
*������ :��FC18��withinMap�ж�ĳ���Ƿ��ڵ�ͼ��Χ��
*������������ : �жϵ�p�Ƿ��ڵ�ǰ��ͼ�ķ�Χ��
*�������� : p<TPoint*>---ָ�����ڵ��ָ��
*��������ֵ : <mapBlockInfo>--��ͼ������Ϣ
*���� : ������
***********************************************************************************************/
bool Map::withinMap(TPoint p) {
	return (p.m_x >= 0) && (p.m_x < m_width) && (p.m_y >= 0) && (p.m_y < m_height);
}

/***********************************************************************************************
*������ :��FC18��modifyOccupyPoint���������߸ı��޸�ռ������ֵ
*������������ : ͨ����֮ǰ��ӵ���ߣ����У�������NOTOWER�����Լ����ڵ�ӵ���ߣ�ͬ�ϣ����޸���Χ
                �����ռ������ֵ
*�������� : oldOwner<TPlayerID>--֮ǰ��ӵ����ID��newOwner<TPlayerID>--���ڵ�ӵ����ID��p<TPoint>
            ---�������ڵ�λ����
*��������ֵ : ��
*���� : ������
***********************************************************************************************/
void Map::modifyOccupyPoint(TPlayerID oldOwner, TPlayerID newOwner, TPoint p) {
	if (!withinMap(p)) return;  //��ǰ�����ڵ�ͼ֮��
	if (oldOwner != NOTOWER && oldOwner >= 1 && oldOwner <= 4) map[p.m_y][p.m_x].occupyPoint[oldOwner - 1] -= INF;
	if (newOwner != NOTOWER && newOwner >= 1 && newOwner <= 4) map[p.m_y][p.m_x].occupyPoint[newOwner - 1] += INF;
	ownerChange(p);
	for (int i = p.m_y - 5; i <= p.m_y + 5; i++) {
		for (int j = p.m_x - 5; j <= p.m_x + 5; j++) {
			TPoint currentPoint = { j,i };
			TDist dist = getDist(currentPoint, p);
			if (!withinMap(currentPoint) || dist < 1 || dist > 5) continue;//�㲻��ͼ�ϣ�����̫����̫Զ
			if (data->getRound() == 0 && map[i][j].owner == PUBLIC) continue;  //���ֲ��ı乫�����̵�ռ������ֵ
			if (oldOwner != NOTOWER) map[i][j].occupyPoint[oldOwner - 1] -= TowerOccupyPoint[dist - 1];
			if (newOwner != NOTOWER) map[i][j].occupyPoint[newOwner - 1] += TowerOccupyPoint[dist - 1];
			ownerChange(currentPoint);
		}
	}
}


/***********************************************************************************************
*������ :��FC18��ownerChang�ı䷽��ӵ���ߺ���
*������������ : �ı�λ��p������ӵ���ߣ��ж���������
*�������� : p<TPoint>---���ڷ����TPoint����
*��������ֵ : <TPlayerID>---����ǰ���ڷ���仯������������꣨PUBLIC=0����������TRANSITION=-1
              ��������OUTOFRANGE=-2�ڵ�ͼ�⣩
*���� : ������
***********************************************************************************************/
TPlayerID Map::ownerChange(TPoint p) {
	if (!withinMap(p)) return OUTOFRANGE;    //��ǰ�����ڵ�ͼ֮��
	int maxOccupyPoint = -1, occupyID = PUBLIC;
	int maxOccupyCnt = 0;
	for (int i = 0; i < 4; i++) {
		if (map[p.m_y][p.m_x].occupyPoint[i] > maxOccupyPoint) {
			maxOccupyPoint = map[p.m_y][p.m_x].occupyPoint[i];
			occupyID = i + 1;
		}
	}
	if (maxOccupyPoint == 0) {
		map[p.m_y][p.m_x].owner = PUBLIC;
		return PUBLIC;
	}
	else {
		for (int i = 0; i < 4; i++) {
			if (map[p.m_y][p.m_x].occupyPoint[i] == maxOccupyPoint && ((i + 1) != occupyID)  && data->getRound() != 0)
			{
				map[p.m_y][p.m_x].owner = TRANSITION;
				return TRANSITION;
			}
		}
	}
	map[p.m_y][p.m_x].owner = occupyID;
	return occupyID;
}

/*****************************s******************************************************************
*������ :��FC18��showOwner�жϷ���ӵ���ߺ���
*������������ : �жϵ�ǰ�����ӵ����
*�������� : p<TPoint>---���ڷ����TPoint����
*��������ֵ :  <TPlayerID>---����ǰ���ڷ�������������꣨PUBLIC=0����������TRANSITION=-1������
               ��OUTOFRANGE=-2�ڵ�ͼ�⣩
*���� : ������
***********************************************************************************************/
TPlayerID Map::showOwner(TPoint p) {
	if (!withinMap(p)) return OUTOFRANGE;  //��ǰ�����ڵ�ͼ֮��
	else return map[p.m_y][p.m_x].owner;
}

/*****************************s******************************************************************
*������ :��FC18��clearAll��������ű��
*������������ : �жϵ�ǰ�����ӵ����
*�������� : p<TPoint>---���ڷ����TPoint����
*��������ֵ :  <TPlayerID>---����ǰ���ڷ�������������꣨PUBLIC=0����������TRANSITION=-1������
			   ��OUTOFRANGE=-2�ڵ�ͼ�⣩
*���� : ������
***********************************************************************************************/
void Map::clearAll(){
	delete[] data->players;
	data->players = nullptr;
}
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "MahjongGB/MahjongGB.h"

#define _BOTZONE_ONLINE
#ifdef _BOTZONE_ONLINE

#include "jsoncpp/json.h"

#else
#include <json/json.h>
#endif

#define SIMPLEIO 0
//由玩家自己定义，0表示JSON交互，1表示简单交互。

using namespace std;

vector<string> request, response;
vector<string> hand;

int main() {
    int turnID;
    string stmp;
#if SIMPLEIO
    cin >> turnID;
    turnID--;
    getline(cin, stmp);
    for(int i = 0; i < turnID; i++) {
        getline(cin, stmp);
        request.push_back(stmp);
        getline(cin, stmp);
        response.push_back(stmp);
    }
    getline(cin, stmp);
    request.push_back(stmp);
#else
    Json::Value inputJSON;
    cin >> inputJSON;
    //获取历史所有的request和response

    //request有四大种类
    //0代表获取门风圈风信息
    //1代表获取手牌信息
    //2表示自己摸了一张牌，必须要进行应对，不能跳过
    //3表示别人打出了牌，别人杠了牌，别人摸了牌等等。可以选择应对（比如碰牌，胡牌），也可以跳过。

    //response的种类有，PASS，PLAY，PENG，CHI，GANG，BUGANG，HU
    //PASS是接收环境信息的response，也是接收其他玩家的信息后，选择不反应的response
    //PLAY是打出一张牌，只有摸牌的response是PLAY
    //PENG,CHI,GANG,BUGANG分别是碰，吃，杠，补杠，在摸牌时可能是杠（即暗杠）和补杠，在其他情况下是吃碰杠别人的牌
    //HU表示胡，可以自摸，荣胡或者抢杠胡
    //抢杠只能抢补杠或者暗杠

    //当前手牌需要通过历史信息进行复原
    turnID = inputJSON["responses"].size();
    //turnID: 当前回合数
    for (int i = 0; i < turnID; i++) {
        request.push_back(inputJSON["requests"][i].asString());
        response.push_back(inputJSON["responses"][i].asString());
    }
    request.push_back(inputJSON["requests"][turnID].asString());
    //解析json，把历史信息存储起来
#endif
    if (turnID < 2) {
        response.emplace_back("PASS");
        //第一回合，仅告知玩家门风和圈风，该信息在一局游戏中不变
        //只用给出“PASS”的response，无需解析
    } else {
        int itmp, myPlayerID, quan;
        //itmp是用作整数IO的暂时存储
        //myPlayerID是玩家编号（门风）
        //quan是圈风
        //风的编号是东南西北->0,1,2,3
        ostringstream sout;
        istringstream sin;
        sin.str(request[0]);
        sin >> itmp >> myPlayerID >> quan;
        //获取门风和圈风信息
        sin.clear();
        sin.str(request[1]);
        for (int j = 0; j < 5; j++) sin >> itmp;
        for (int j = 0; j < 13; j++) {
            sin >> stmp;
            hand.push_back(stmp);
        }
        //获取初始手牌信息
        for (int i = 2; i < turnID; i++) {
            sin.clear();
            sin.str(request[i]);
            sin >> itmp;
            if (itmp == 2) {//如果是摸牌操作
                sin >> stmp;//以前摸的牌
                hand.push_back(stmp);
                sin.clear();
                sin.str(response[i]);
                sin >> stmp >> stmp;//以前打出的牌
                hand.erase(find(hand.begin(), hand.end(), stmp));
            }
        }
        //复原当前手牌


        sin.clear();
        sin.str(request[turnID]);
        sin >> itmp;
        //示例程序，如果摸牌则随机打出一张，否则都PASS
        if (itmp == 2) {
            //此处删掉了shuffle
            sout << "PLAY " << *hand.rbegin();
            hand.pop_back();
        } else {
            sout << "PASS";
        }
        response.push_back(sout.str());
    }

#if SIMPLEIO
    cout << response[turnID] << endl;
#else
    Json::Value outputJSON;
    outputJSON["response"] = response[turnID];
    cout << outputJSON << endl;
#endif
    return 0;
}

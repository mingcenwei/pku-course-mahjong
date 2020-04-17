#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cassert>
#include "MahjongGB/tile.h"

#define LOCAL_DEBUG
//是否为本地debug模式，如果是，开启LOCAL_ASSERT以检测可能的错误
#ifdef LOCAL_DEBUG
#define LOCAL_ASSERT(expr) assert(expr)
#else
#define LOCAL_ASSERT(expr) (expr)
#endif

/* 保存当前对局信息的全局变量

int cur_turn_id                                        当前局数，从1开始算
int my_player_id                                       自己的编号，0~3逆时针编码，同时对应东南西北的自风
int wind                                               当前场风

std::vector<std::string> request, response             所有的request和response，有turnID个request，cur_turn_id-1个response

mahjong::tile_t  global_tiles_table[0x60]              全局用掉的牌，包含自己手牌、补花的花牌、牌河和所有人的副露，用来检测听牌张数的
mahjong::tile_t my_hand_tiles_table[0x60]              手牌牌表，不使用算番库的tile_table_t类型，不涉及递归削减的话就不用紧凑编码了，更方便使用

mahjong::hand_tiles_t my_hand_tiles                    手牌，包含副露
std::vector<mahjong::tile_t> my_standing_tiles_vector  手牌立牌部分的vector形式
std::vector<mahjong::pack_t> my_fixed_packs_vector     手牌副露部分的vector形式

std::vector<mahjong::pack_t> all_player_fixed_packs[4] 记录其他人的副露，不包含自己的副露；除了杠子标注明杠和暗杠，不使用pack_t的offer信息
std::vector<mahjong::tile_t> all_player_flowers[4]     记录所有人的花牌
std::vector<mahjong::tile_t> tiles_river               牌河,不包含花牌
std::vector<mahjong::tile_t> all_player_discard_record[4]   记录所有人曾经的弃章，包括打出、吃打出、碰打出

 */

int cur_turn_id, my_player_id, wind;
std::vector<std::string> request, response;
mahjong::tile_t global_tiles_table[0x60];
mahjong::tile_t my_hand_tiles_table[0x60];
mahjong::hand_tiles_t my_hand_tiles;
std::vector<mahjong::tile_t> my_standing_tiles_vector;
std::vector<mahjong::pack_t> my_fixed_packs_vector;
std::vector<mahjong::pack_t> all_player_fixed_packs[4];
std::vector<mahjong::tile_t> all_player_flowers[4];
std::vector<mahjong::tile_t> tiles_river;
std::vector<mahjong::tile_t> all_player_discard_record[4];

mahjong::tile_t str2tile_t(const std::string &s) {
    mahjong::suit_t suit;
    auto rank = static_cast<mahjong::rank_t>(s[1] - '0');
    switch (s[0]) {
        case 'W':
            suit = TILE_SUIT_CHARACTERS;
            break;
        case 'B':
            suit = TILE_SUIT_DOTS;
            break;
        case 'T':
            suit = TILE_SUIT_BAMBOO;
            break;
        case 'F':
            suit = TILE_SUIT_HONORS;
            break;
        case 'J':
            suit = TILE_SUIT_HONORS;
            rank += 4;
            break;
        case 'H':
            //花牌没有定义宏，这个是库里写漏了，但是是合法的tile
            suit = 5;
            break;
        default:
            suit = TILE_SUIT_NONE;
    }
    return mahjong::make_tile(suit, rank);
}

std::string tile_t2str(const mahjong::tile_t &tile, bool use_chinese = true) {
    std::string type;
    char rank = mahjong::tile_get_rank(tile) + '0';
    switch (mahjong::tile_get_suit(tile)) {
        case TILE_SUIT_CHARACTERS:
            if (use_chinese)type = "万";
            else type = "W";
            break;
        case TILE_SUIT_DOTS:
            if (use_chinese)type = "饼";
            else type = "B";
            break;
        case TILE_SUIT_BAMBOO:
            if (use_chinese)type = "条";
            else type = "T";
            break;
        case TILE_SUIT_HONORS:
            if (rank >= '1' && rank <= '4') {
                if (use_chinese) {
                    switch (rank) {
                        case '1':
                            type = "东";
                            break;
                        case '2':
                            type = "南";
                            break;
                        case '3':
                            type = "西";
                            break;
                        case '4':
                            type = "北";
                            break;
                        default:
                            LOCAL_ASSERT(0);
                    }
                    rank = ' ';
                } else type = "F";
            } else {
                //箭牌
                LOCAL_ASSERT(rank >= '5' && rank <= '7');
                if (use_chinese) {
                    switch (rank) {
                        case '5':
                            type = "中";
                            break;
                        case '6':
                            type = "发";
                            break;
                        case '7':
                            type = "白";
                            break;
                        default:
                            LOCAL_ASSERT(0);
                    }
                    rank = ' ';
                } else {
                    type = "J";
                    rank -= 4;
                }
            }
            break;
        case 5:
            if (use_chinese)type = "花";
            else type = "H";
            break;
        default:
            if (use_chinese)type = "暗";
            else type = "X";
            rank = ' ';
    }
    type.push_back(rank);
    return type;
}

std::string pack_t2str(const mahjong::pack_t &pack) {
    std::string card = tile_t2str(mahjong::pack_get_tile(pack));
    std::string res = "{";
    switch (mahjong::pack_get_type(pack)) {
        case PACK_TYPE_PUNG:
            res += card + card + card;
            break;
        case PACK_TYPE_KONG:
            res += card + card + card + card;
            break;
        case PACK_TYPE_CHOW:
            --card.back();
            res += card;
            ++card.back();
            res += card;
            ++card.back();
            res += card;
            break;
        default:
            LOCAL_ASSERT(0);
    }
    res += "}";
    return res;
}

void init() {
    //该函数用于初始化全局变量
    {
        //获取输入
        std::string s;
        std::cin >> cur_turn_id;
        std::getchar();//取走'\n'
        for (int i = 1; i < cur_turn_id; i++) {
            std::getline(std::cin, s);
            request.push_back(s);
            std::getline(std::cin, s);
            response.push_back(s);
        }
        std::getline(std::cin, s);
        request.push_back(s);
    }
/*
    request有四大种类
    0. 获取门风圈风信息
    1. 代表获取手牌信息
    2. 自己摸了一张牌，必须要进行应对，不能跳过
    3. 别人打出了牌，别人杠了牌，别人摸了牌等等。可以选择应对（比如碰牌，胡牌），也可以跳过。

    四大种类对应格式有十种，其中0,1,2一一对应，3对应3~9
    特别要注意的是，如果一个response会传给所有玩家的话，玩家会收到自己response导致的request，比如自己提出打出一张牌，
    则下回合会收到自己打出了一张牌的request，这时候必须直接PASS

    0."0 player_id wind"               只在第一回合出现，玩家序号、风圈

    1."1 hua0 hua1 hua2 hua3 handCard(1~13) flower0_(1~hua0) flower1_(1~hua1)..."
                                   依次是四个玩家摸到的花牌数，自己的十三张手牌（已经补过花，不存在花牌），所有玩家摸到的花牌

    2."2 card0                    表示自己摸到card0

    3."3 player_id BUHUA card0"        表示打出花牌card0并且摸一张牌

    4."3 player_id DRAW"               表示摸了一张牌

    5."3 player_id PLAY card0"         表示打出card0

    6."3 player_id PENG card0"         表示碰上一张牌后打出card0

    7."3 player_id CHI card0 card1"    表示吃了上家的牌后，生成的顺子中间牌为card0，并且打出card1

    8."3 player_id GANG"               表示杠牌，如果该玩家刚摸完牌则是暗杠，如果其他玩家刚打出牌则是明杠

    9."3 player_id BUGANG card0"       表示补杠

    对应十种request，分别有如下可选的response
    0.  "PASS"                          获取信息后跳过

    1.  "PASS"                          获取信息后跳过

    2:
            "PLAY card0"                摸牌后打出card0
            "GANG card0"                摸牌后进行暗杠，如果成功会再次摸牌（摸牌动作本回合不用处理）
            "BUGANG card0"              摸牌后进行补杠，如果成功会再次摸牌（摸牌动作本回合不用处理）
            "HU"                        自摸

    3.  "PASS"                          表示获取了其他玩家已经补花的信息

    4.  "PASS"                          表示获取了其他玩家已经摸牌的信息

    5,6,7:
            "PASS"                      表示获取了其他人打出了某张牌的信息（通过摸、碰吃的形式），但不做反应
            "PENG card0"                表示碰别人打出的牌
            "CHI card0 card1"           表示吃别人打出的牌后组成顺子的中间牌是card0，打card1
            "GANG"                      表示杠别人打出的牌
            "HU"                        表示荣胡

    8.      "PASS"                      表示知道了别人杠牌的信息

    9:
            "PASS"                      表示知道了别人补杠的信息
            "HU"                        表示抢杠胡
    */

    std::istringstream iss;
    for (int turn = 0; turn != cur_turn_id; ++turn) {
        iss.clear();
        iss.str(request[turn]);
        int request_type, player_id;
        std::string op, card0, card1;
        iss >> request_type;
        switch (request_type) {
            case 0: {
                iss >> my_player_id >> wind;
                break;
            }
            case 1: {
                int flower_cnt[4];
                for (int &x:flower_cnt)iss >> x;
                for (int ii = 0; ii < 13; ++ii) {
                    iss >> card0;
                    my_standing_tiles_vector.push_back(str2tile_t(card0));
                }
                for (int ii = 0; ii < 4; ii++) {
                    for (int jj = 0; jj < flower_cnt[ii]; ++jj) {
                        iss >> card0;
                        all_player_flowers[ii].push_back(str2tile_t(card0));
                    }
                }
                break;
            }
            case 2: {
                iss >> card0;
                my_standing_tiles_vector.push_back(str2tile_t(card0));//此时立牌是14张（无副露情况）
                break;
            }
            case 3: {
                iss >> player_id >> op;
                if (op == "BUHUA") {
                    iss >> card0;
                    all_player_flowers[player_id].push_back(str2tile_t(card0));
                } else if (op == "DRAW") {
                    //其他玩家的摸牌，没有任何信息
                } else if (op == "PLAY") {
                    iss >> card0;
                    auto tile_discard = str2tile_t(card0);
                    if (player_id == my_player_id) {
                        auto p = find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_discard);
                        LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(p);
                    }
                    tiles_river.push_back(tile_discard);
                    all_player_discard_record[player_id].push_back(tile_discard);
                } else if (op == "PENG") {
                    iss >> card0;
                    auto tile_pung = tiles_river.back();
                    tiles_river.pop_back();
                    auto tile_discard = str2tile_t(card0);
                    if (player_id == my_player_id) {
                        auto p = std::find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_pung);
                        LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(p);//删掉第一张

                        p = std::find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_pung);
                        LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(p);//删掉第二张

                        p = std::find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_discard);
                        LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(p);//删掉打出的牌

                        my_fixed_packs_vector.push_back(mahjong::make_pack(0, PACK_TYPE_PUNG, tile_pung));//生成碰
                    } else
                        all_player_fixed_packs[player_id].push_back(mahjong::make_pack(0, PACK_TYPE_PUNG, tile_pung));
                    //不关心谁供的牌，只关心生成了一幅面子，供牌信息应该对局面估值应该是没用的
                    tiles_river.push_back(tile_discard);
                    all_player_discard_record[player_id].push_back(tile_discard);
                } else if (op == "CHI") {
                    iss >> card0 >> card1;
                    //card0是生成顺子的中间牌，card1是打出的
                    auto tile_mid = str2tile_t(card0);
                    auto tile_discard = str2tile_t(card1);
                    auto tile_chow = tiles_river.back();//吃的牌必定来自牌河最后一张
                    tiles_river.pop_back();
                    if (player_id == my_player_id) {
                        std::vector<mahjong::tile_t> chow_in_hand;
                        chow_in_hand.push_back(tile_mid - (unsigned char) 1);
                        chow_in_hand.push_back(tile_mid);
                        chow_in_hand.push_back(tile_mid + (unsigned char) 1);
                        //生成吃的三张牌
                        auto p = find(chow_in_hand.begin(), chow_in_hand.end(), tile_chow);
                        LOCAL_ASSERT(p != chow_in_hand.end());
                        chow_in_hand.erase(p);
                        //去掉从牌河中拿到的牌，剩下两张牌都来自手牌，需要在手牌中删除
                        for (const auto &tile:chow_in_hand) {
                            auto q = find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile);
                            LOCAL_ASSERT(q != my_standing_tiles_vector.end());
                            my_standing_tiles_vector.erase(q);
                        }//在手牌中相关牌删除
                        p = find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_discard);
                        LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(p);
                        //删掉弃章
                        my_fixed_packs_vector.push_back(mahjong::make_pack(0, PACK_TYPE_CHOW, tile_mid));
                    } else all_player_fixed_packs[player_id].push_back(mahjong::make_pack(0, PACK_TYPE_CHOW, tile_mid));
                    tiles_river.push_back(tile_discard);
                    all_player_discard_record[player_id].push_back(tile_discard);
                } else if (op == "GANG") {
                    if (player_id == my_player_id) {
                        iss.clear();
                        iss.str(request[turn - 1]);
                        int last_request_type;
                        iss >> last_request_type;
                        if (last_request_type == 2) {
                            //说明是自己摸牌，即暗杠
                            iss >> card0;
                            auto tile_kong = str2tile_t(card0);
                            for (int tt = 0; tt < 4; ++tt) {
                                auto p = find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(),
                                              tile_kong);
                                LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                                my_standing_tiles_vector.erase(p);
                            }//删除四张手牌
                            my_fixed_packs_vector.push_back(mahjong::make_pack(0, PACK_TYPE_KONG, tile_kong));
                            //生成暗杠，offer为0
                        } else {
                            //说明是杠牌河上一张牌
                            auto tile_kong = tiles_river.back();
                            tiles_river.pop_back();
                            for (int tt = 0; tt < 3; ++tt) {
                                auto p = find(my_standing_tiles_vector.begin(),
                                              my_standing_tiles_vector.end(), tile_kong);
                                LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                                my_standing_tiles_vector.erase(p);
                            }//删除三张手牌
                            my_fixed_packs_vector.push_back(mahjong::make_pack(1, PACK_TYPE_KONG, tile_kong));
                            //生成明杠，统一offer为1
                        }
                    } else {
                        iss.clear();
                        iss.str(request[turn - 1]);
                        //看上一request是否是他摸牌，如果是则暗杠，否则明杠
                        //上一个request只有两种可能，一种是杠牌人摸牌，另一种是非杠牌人打牌
                        //如果是暗杠，则不能使用pack_get_tile，明杠则可以用于计数有效张
                        int last_request_type;
                        int lastPlayerID;
                        std::string last_op;
                        iss >> last_request_type >> lastPlayerID >> last_op;
                        if (player_id == lastPlayerID) {
                            LOCAL_ASSERT(last_request_type == 3);
                            LOCAL_ASSERT(last_op == "DRAW");
                            auto tile_kung = str2tile_t("X1");//不知道暗杠
                            all_player_fixed_packs[player_id].push_back(
                                    mahjong::make_pack(0, PACK_TYPE_KONG, tile_kung));
                        } else {
                            auto tile_kung = tiles_river.back();
                            //从牌河弹出上一张牌
                            tiles_river.pop_back();
                            all_player_fixed_packs[player_id].push_back(
                                    mahjong::make_pack(1, PACK_TYPE_KONG, tile_kung));
                        }
                    }
                } else if (op == "BUGANG") {
                    iss >> card0;
                    auto tile_kong = str2tile_t(card0);
                    if (player_id == my_player_id) {
                        bool flag = true;
                        for (auto &p:my_fixed_packs_vector) {
                            if (mahjong::pack_get_type(p) == PACK_TYPE_PUNG &&
                                mahjong::pack_get_tile(p) == tile_kong) {
                                flag = false;
                                p = mahjong::promote_pung_to_kong(p);
                                break;
                            }
                        }
                        auto q = find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_kong);
                        LOCAL_ASSERT(q != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(q);
                        LOCAL_ASSERT(!flag);
                    } else {
                        bool flag = true;
                        for (auto &p:all_player_fixed_packs[player_id]) {
                            if (mahjong::pack_get_type(p) == PACK_TYPE_PUNG &&
                                mahjong::pack_get_tile(p) == tile_kong) {
                                flag = false;
                                p = mahjong::promote_pung_to_kong(p);
                                break;
                            }
                        }
                        LOCAL_ASSERT(!flag);
                    }
                }
                break;
            }
            default:
                LOCAL_ASSERT(0);
        }
    }

    //生成手牌
    my_hand_tiles.tile_count = 0;
    sort(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end());
    for (const auto &tile:my_standing_tiles_vector)my_hand_tiles.standing_tiles[my_hand_tiles.tile_count++] = tile;
    my_hand_tiles.pack_count = 0;
    for (const auto &pack:my_fixed_packs_vector)my_hand_tiles.fixed_packs[my_hand_tiles.pack_count++] = pack;

    //开始打表
    for (auto &cnt:global_tiles_table)cnt = 0;
    for (auto &cnt:my_hand_tiles_table)cnt = 0;
    //牌河打表
    for (auto const &tile:tiles_river)++global_tiles_table[tile];
    //花牌打表
    for (auto const &v:all_player_flowers)for (auto const &tile_flower:v)++global_tiles_table[tile_flower];
    //手牌立牌打表
    for (auto const &tile:my_standing_tiles_vector)++global_tiles_table[tile], ++my_hand_tiles_table[tile];
    //自己副露打表
    for (auto const &p:my_fixed_packs_vector) {
        auto type = mahjong::pack_get_type(p);
        auto tile = mahjong::pack_get_tile(p);
        switch (type) {
            case PACK_TYPE_CHOW:
                ++global_tiles_table[tile - (unsigned char) 1];
                ++my_hand_tiles_table[tile - (unsigned char) 1];
                ++global_tiles_table[tile];
                ++my_hand_tiles_table[tile];
                ++global_tiles_table[tile + (unsigned char) 1];
                ++my_hand_tiles_table[tile + (unsigned char) 1];
                break;
            case PACK_TYPE_PUNG:
                global_tiles_table[tile] += 3;
                my_hand_tiles_table[tile] += 3;
                break;
            case PACK_TYPE_KONG:
                global_tiles_table[tile] += 4;
                my_hand_tiles_table[tile] += 4;
                break;
            default:
                LOCAL_ASSERT(0);
        }
    }
    //其他人副露打表
    for (auto const &v:all_player_fixed_packs) {
        for (auto const &p:v) {
            auto offer = mahjong::pack_get_offer(p);
            auto type = mahjong::pack_get_type(p);
            auto tile = mahjong::pack_get_tile(p);
            switch (type) {
                case PACK_TYPE_CHOW:
                    ++global_tiles_table[tile - (unsigned char) 1];
                    ++global_tiles_table[tile];
                    ++global_tiles_table[tile + (unsigned char) 1];
                    break;
                case PACK_TYPE_PUNG:
                    global_tiles_table[tile] += 3;
                    break;
                case PACK_TYPE_KONG:
                    if (offer)global_tiles_table[tile] += 4;
                    break;//暗杠不打表
                default:
                    LOCAL_ASSERT(0);
            }
        }
    }
}

void show_current_board() {
    std::cout << "River is:" << std::endl;
    for (const auto &tile:tiles_river) {
        std::cout << '[' << tile_t2str(tile) << ']';
    }
    std::cout << std::endl << std::endl;
    std::cout << "All players' discard recordings are:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << i << ' ';
        for (const auto &tile:all_player_discard_record[i]) {
            std::cout << '[' << tile_t2str(tile) << ']';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
    std::cout << "My hand tiles are:" << std::endl;
    for (const auto &tile:my_standing_tiles_vector) {
        std::cout << '[' << tile_t2str(tile) << ']';
    }
    std::cout << std::endl << std::endl;
    std::cout << "My fixed packs are:" << std::endl;
    for (const auto &pack:my_fixed_packs_vector) {
        std::cout << pack_t2str(pack);
    }
    std::cout << std::endl << std::endl;
    std::cout << "Other players' fixed packs are:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << i << ' ';
        for (const auto &pack:all_player_fixed_packs[i]) {
            std::cout << pack_t2str(pack);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "All players' flowers are:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << i << ' ';
        for (const auto &tile:all_player_flowers[i]) {
            std::cout << '[' << tile_t2str(tile) << ']';
        }
        std::cout << std::endl;
    }
}

void play_response();

void discard_response();

void promote_kong_response(mahjong::tile_t);

int main() {
#ifdef LOCAL_DEBUG
    std::freopen("in.txt", "r", stdin);
    //std::freopen("out.txt", "w", stdout);
#endif
    init();
    //如果调试时想要显示对局信息，去掉show_current_board()的注释
    //show_current_board();
    std::istringstream iss;
    iss.str(request.back());
    int request_type;
    iss >> request_type;
    switch (request_type) {
        case 0:
        case 1:
            response.emplace_back("PASS");
            break;
        case 2:
            play_response();
            break;
        case 3: {
            int player_id;
            std::string op;
            iss >> player_id >> op;
            if (op == "BUHUA" || op == "DRAW" || op == "GANG") response.emplace_back("PASS");
            else if (op == "PLAY" || op == "PENG" || op == "CHI")discard_response();
            else if (op == "BUGANG") {
                std::string card_kong;
                iss >> card_kong;
                auto tile_kong = str2tile_t(card_kong);
                promote_kong_response(tile_kong);
            } else
                LOCAL_ASSERT(0);
            break;
        }
        default:
            LOCAL_ASSERT(0);
    }
    std::cout << response.back();
    return 0;
}


//需要完成的是三个决策函数，play_response，discard_response，kong_response
//三个函数分别是:
//  摸了一张牌后决定打出哪张（此时摸的牌已经在手牌里了）；
//  别人打出一张牌后决定如何反应（吃，碰，杠，胡，PASS），这张牌在tiles_river的最后一张
//  别人加杠的时候如何反应（PASS，抢杠胡），这张牌通过参数传递
//决策的结果按照输出格式push_back到response里面，比如"PASS","CHI card0 card1"等等
void play_response() {
    std::string res;
    //...
    response.emplace_back(res);
}

void discard_response() {
    auto tile_discard = tiles_river.back();//这张就是别人打出的牌
    std::string res;
    //...
    response.emplace_back(res);
}

void promote_kong_response(mahjong::tile_t tile_kong) {
    //tile_kong是别人补杠的牌，其实这个case只用判断能不能胡，不胡是傻逼
    std::string res;
    //...
    response.emplace_back(res);
}

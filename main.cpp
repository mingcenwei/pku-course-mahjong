#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cassert>
#include "MahjongGB/tile.h"

#define LOCAL_DEBUG
//�Ƿ�Ϊ����debugģʽ������ǣ�����LOCAL_ASSERT�Լ����ܵĴ���
#ifdef LOCAL_DEBUG
#define LOCAL_ASSERT(expr) assert(expr)
#else
#define LOCAL_ASSERT(expr) (expr)
#endif

/* ���浱ǰ�Ծ���Ϣ��ȫ�ֱ���

int cur_turn_id                                        ��ǰ��������1��ʼ��
int my_player_id                                       �Լ��ı�ţ�0~3��ʱ����룬ͬʱ��Ӧ�����������Է�
int wind                                               ��ǰ����

std::vector<std::string> request, response             ���е�request��response����turnID��request��cur_turn_id-1��response

mahjong::tile_t  global_tiles_table[0x60]              ȫ���õ����ƣ������Լ����ơ������Ļ��ơ��ƺӺ������˵ĸ�¶�������������������
mahjong::tile_t my_hand_tiles_table[0x60]              �����Ʊ���ʹ���㷬���tile_table_t���ͣ����漰�ݹ������Ļ��Ͳ��ý��ձ����ˣ�������ʹ��

mahjong::hand_tiles_t my_hand_tiles                    ���ƣ�������¶
std::vector<mahjong::tile_t> my_standing_tiles_vector  �������Ʋ��ֵ�vector��ʽ
std::vector<mahjong::pack_t> my_fixed_packs_vector     ���Ƹ�¶���ֵ�vector��ʽ

std::vector<mahjong::pack_t> all_player_fixed_packs[4] ��¼�����˵ĸ�¶���������Լ��ĸ�¶�����˸��ӱ�ע���ܺͰ��ܣ���ʹ��pack_t��offer��Ϣ
std::vector<mahjong::tile_t> all_player_flowers[4]     ��¼�����˵Ļ���
std::vector<mahjong::tile_t> tiles_river               �ƺ�,����������
std::vector<mahjong::tile_t> all_player_discard_record[4]   ��¼���������������£�����������Դ���������

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
            //����û�ж���꣬����ǿ���д©�ˣ������ǺϷ���tile
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
            if (use_chinese)type = "��";
            else type = "W";
            break;
        case TILE_SUIT_DOTS:
            if (use_chinese)type = "��";
            else type = "B";
            break;
        case TILE_SUIT_BAMBOO:
            if (use_chinese)type = "��";
            else type = "T";
            break;
        case TILE_SUIT_HONORS:
            if (rank >= '1' && rank <= '4') {
                if (use_chinese) {
                    switch (rank) {
                        case '1':
                            type = "��";
                            break;
                        case '2':
                            type = "��";
                            break;
                        case '3':
                            type = "��";
                            break;
                        case '4':
                            type = "��";
                            break;
                        default:
                            LOCAL_ASSERT(0);
                    }
                    rank = ' ';
                } else type = "F";
            } else {
                //����
                LOCAL_ASSERT(rank >= '5' && rank <= '7');
                if (use_chinese) {
                    switch (rank) {
                        case '5':
                            type = "��";
                            break;
                        case '6':
                            type = "��";
                            break;
                        case '7':
                            type = "��";
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
            if (use_chinese)type = "��";
            else type = "H";
            break;
        default:
            if (use_chinese)type = "��";
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
    //�ú������ڳ�ʼ��ȫ�ֱ���
    {
        //��ȡ����
        std::string s;
        std::cin >> cur_turn_id;
        std::getchar();//ȡ��'\n'
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
    request���Ĵ�����
    0. ��ȡ�ŷ�Ȧ����Ϣ
    1. �����ȡ������Ϣ
    2. �Լ�����һ���ƣ�����Ҫ����Ӧ�ԣ���������
    3. ���˴�����ƣ����˸����ƣ����������Ƶȵȡ�����ѡ��Ӧ�ԣ��������ƣ����ƣ���Ҳ����������

    �Ĵ������Ӧ��ʽ��ʮ�֣�����0,1,2һһ��Ӧ��3��Ӧ3~9
    �ر�Ҫע����ǣ����һ��response�ᴫ��������ҵĻ�����һ��յ��Լ�response���µ�request�������Լ�������һ���ƣ�
    ���»غϻ��յ��Լ������һ���Ƶ�request����ʱ�����ֱ��PASS

    0."0 player_id wind"               ֻ�ڵ�һ�غϳ��֣������š���Ȧ

    1."1 hua0 hua1 hua2 hua3 handCard(1~13) flower0_(1~hua0) flower1_(1~hua1)..."
                                   �������ĸ���������Ļ��������Լ���ʮ�������ƣ��Ѿ��������������ڻ��ƣ���������������Ļ���

    2."2 card0                    ��ʾ�Լ�����card0

    3."3 player_id BUHUA card0"        ��ʾ�������card0������һ����

    4."3 player_id DRAW"               ��ʾ����һ����

    5."3 player_id PLAY card0"         ��ʾ���card0

    6."3 player_id PENG card0"         ��ʾ����һ���ƺ���card0

    7."3 player_id CHI card0 card1"    ��ʾ�����ϼҵ��ƺ����ɵ�˳���м���Ϊcard0�����Ҵ��card1

    8."3 player_id GANG"               ��ʾ���ƣ��������Ҹ����������ǰ��ܣ����������Ҹմ������������

    9."3 player_id BUGANG card0"       ��ʾ����

    ��Ӧʮ��request���ֱ������¿�ѡ��response
    0.  "PASS"                          ��ȡ��Ϣ������

    1.  "PASS"                          ��ȡ��Ϣ������

    2:
            "PLAY card0"                ���ƺ���card0
            "GANG card0"                ���ƺ���а��ܣ�����ɹ����ٴ����ƣ����ƶ������غϲ��ô���
            "BUGANG card0"              ���ƺ���в��ܣ�����ɹ����ٴ����ƣ����ƶ������غϲ��ô���
            "HU"                        ����

    3.  "PASS"                          ��ʾ��ȡ����������Ѿ���������Ϣ

    4.  "PASS"                          ��ʾ��ȡ����������Ѿ����Ƶ���Ϣ

    5,6,7:
            "PASS"                      ��ʾ��ȡ�������˴����ĳ���Ƶ���Ϣ��ͨ���������Ե���ʽ������������Ӧ
            "PENG card0"                ��ʾ�����˴������
            "CHI card0 card1"           ��ʾ�Ա��˴�����ƺ����˳�ӵ��м�����card0����card1
            "GANG"                      ��ʾ�ܱ��˴������
            "HU"                        ��ʾ�ٺ�

    8.      "PASS"                      ��ʾ֪���˱��˸��Ƶ���Ϣ

    9:
            "PASS"                      ��ʾ֪���˱��˲��ܵ���Ϣ
            "HU"                        ��ʾ���ܺ�
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
                my_standing_tiles_vector.push_back(str2tile_t(card0));//��ʱ������14�ţ��޸�¶�����
                break;
            }
            case 3: {
                iss >> player_id >> op;
                if (op == "BUHUA") {
                    iss >> card0;
                    all_player_flowers[player_id].push_back(str2tile_t(card0));
                } else if (op == "DRAW") {
                    //������ҵ����ƣ�û���κ���Ϣ
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
                        my_standing_tiles_vector.erase(p);//ɾ����һ��

                        p = std::find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_pung);
                        LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(p);//ɾ���ڶ���

                        p = std::find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_discard);
                        LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(p);//ɾ���������

                        my_fixed_packs_vector.push_back(mahjong::make_pack(0, PACK_TYPE_PUNG, tile_pung));//������
                    } else
                        all_player_fixed_packs[player_id].push_back(mahjong::make_pack(0, PACK_TYPE_PUNG, tile_pung));
                    //������˭�����ƣ�ֻ����������һ�����ӣ�������ϢӦ�öԾ����ֵӦ����û�õ�
                    tiles_river.push_back(tile_discard);
                    all_player_discard_record[player_id].push_back(tile_discard);
                } else if (op == "CHI") {
                    iss >> card0 >> card1;
                    //card0������˳�ӵ��м��ƣ�card1�Ǵ����
                    auto tile_mid = str2tile_t(card0);
                    auto tile_discard = str2tile_t(card1);
                    auto tile_chow = tiles_river.back();//�Ե��Ʊض������ƺ����һ��
                    tiles_river.pop_back();
                    if (player_id == my_player_id) {
                        std::vector<mahjong::tile_t> chow_in_hand;
                        chow_in_hand.push_back(tile_mid - (unsigned char) 1);
                        chow_in_hand.push_back(tile_mid);
                        chow_in_hand.push_back(tile_mid + (unsigned char) 1);
                        //���ɳԵ�������
                        auto p = find(chow_in_hand.begin(), chow_in_hand.end(), tile_chow);
                        LOCAL_ASSERT(p != chow_in_hand.end());
                        chow_in_hand.erase(p);
                        //ȥ�����ƺ����õ����ƣ�ʣ�������ƶ��������ƣ���Ҫ��������ɾ��
                        for (const auto &tile:chow_in_hand) {
                            auto q = find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile);
                            LOCAL_ASSERT(q != my_standing_tiles_vector.end());
                            my_standing_tiles_vector.erase(q);
                        }//�������������ɾ��
                        p = find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(), tile_discard);
                        LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                        my_standing_tiles_vector.erase(p);
                        //ɾ������
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
                            //˵�����Լ����ƣ�������
                            iss >> card0;
                            auto tile_kong = str2tile_t(card0);
                            for (int tt = 0; tt < 4; ++tt) {
                                auto p = find(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end(),
                                              tile_kong);
                                LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                                my_standing_tiles_vector.erase(p);
                            }//ɾ����������
                            my_fixed_packs_vector.push_back(mahjong::make_pack(0, PACK_TYPE_KONG, tile_kong));
                            //���ɰ��ܣ�offerΪ0
                        } else {
                            //˵���Ǹ��ƺ���һ����
                            auto tile_kong = tiles_river.back();
                            tiles_river.pop_back();
                            for (int tt = 0; tt < 3; ++tt) {
                                auto p = find(my_standing_tiles_vector.begin(),
                                              my_standing_tiles_vector.end(), tile_kong);
                                LOCAL_ASSERT(p != my_standing_tiles_vector.end());
                                my_standing_tiles_vector.erase(p);
                            }//ɾ����������
                            my_fixed_packs_vector.push_back(mahjong::make_pack(1, PACK_TYPE_KONG, tile_kong));
                            //�������ܣ�ͳһofferΪ1
                        }
                    } else {
                        iss.clear();
                        iss.str(request[turn - 1]);
                        //����һrequest�Ƿ��������ƣ�������򰵸ܣ���������
                        //��һ��requestֻ�����ֿ��ܣ�һ���Ǹ��������ƣ���һ���ǷǸ����˴���
                        //����ǰ��ܣ�����ʹ��pack_get_tile��������������ڼ�����Ч��
                        int last_request_type;
                        int lastPlayerID;
                        std::string last_op;
                        iss >> last_request_type >> lastPlayerID >> last_op;
                        if (player_id == lastPlayerID) {
                            LOCAL_ASSERT(last_request_type == 3);
                            LOCAL_ASSERT(last_op == "DRAW");
                            auto tile_kung = str2tile_t("X1");//��֪������
                            all_player_fixed_packs[player_id].push_back(
                                    mahjong::make_pack(0, PACK_TYPE_KONG, tile_kung));
                        } else {
                            auto tile_kung = tiles_river.back();
                            //���ƺӵ�����һ����
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

    //��������
    my_hand_tiles.tile_count = 0;
    sort(my_standing_tiles_vector.begin(), my_standing_tiles_vector.end());
    for (const auto &tile:my_standing_tiles_vector)my_hand_tiles.standing_tiles[my_hand_tiles.tile_count++] = tile;
    my_hand_tiles.pack_count = 0;
    for (const auto &pack:my_fixed_packs_vector)my_hand_tiles.fixed_packs[my_hand_tiles.pack_count++] = pack;

    //��ʼ���
    for (auto &cnt:global_tiles_table)cnt = 0;
    for (auto &cnt:my_hand_tiles_table)cnt = 0;
    //�ƺӴ��
    for (auto const &tile:tiles_river)++global_tiles_table[tile];
    //���ƴ��
    for (auto const &v:all_player_flowers)for (auto const &tile_flower:v)++global_tiles_table[tile_flower];
    //�������ƴ��
    for (auto const &tile:my_standing_tiles_vector)++global_tiles_table[tile], ++my_hand_tiles_table[tile];
    //�Լ���¶���
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
    //�����˸�¶���
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
                    break;//���ܲ����
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
    //�������ʱ��Ҫ��ʾ�Ծ���Ϣ��ȥ��show_current_board()��ע��
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


//��Ҫ��ɵ����������ߺ�����play_response��discard_response��kong_response
//���������ֱ���:
//  ����һ���ƺ����������ţ���ʱ�������Ѿ����������ˣ���
//  ���˴��һ���ƺ������η�Ӧ���ԣ������ܣ�����PASS������������tiles_river�����һ��
//  ���˼Ӹܵ�ʱ����η�Ӧ��PASS�����ܺ�����������ͨ����������
//���ߵĽ�����������ʽpush_back��response���棬����"PASS","CHI card0 card1"�ȵ�
void play_response() {
    std::string res;
    //...
    response.emplace_back(res);
}

void discard_response() {
    auto tile_discard = tiles_river.back();//���ž��Ǳ��˴������
    std::string res;
    //...
    response.emplace_back(res);
}

void promote_kong_response(mahjong::tile_t tile_kong) {
    //tile_kong�Ǳ��˲��ܵ��ƣ���ʵ���caseֻ���ж��ܲ��ܺ���������ɵ��
    std::string res;
    //...
    response.emplace_back(res);
}

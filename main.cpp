#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include "MahjongGB/shanten.cpp"
#include "MahjongGB/tile.h"

//#define LOCAL_DEBUG

int turnID;
//��ǰ��������1��ʼ�㣬��turnID��request��turnID-1��response
int myPlayerID;
//�Լ��ı�ţ�0~3��ʱ�����
int quan;
//��ǰ����0�ŵ��Է�Ϊ��
std::vector<std::string> request, response;
//���е�request��response
mahjong::tile_t global_tiles_table[0x60];
//ȫ���õ����ƣ������Լ����ơ������Ļ��ơ��ƺӺ������˵ĸ�¶
mahjong::hand_tiles_t my_hand_tiles;
//���ƣ�������¶
mahjong::tile_t my_hand_tiles_table[0x60];
//�����Ʊ���ʹ���㷬���tile_table_t���ͣ����漰�ݹ������Ļ��Ͳ��ý��ձ����ˣ�������ʹ��
std::vector<mahjong::tile_t> my_hand_tiles_vector;
//�ڻ�ԭ�����У���ʹ��hand_tiles_t����Ϊ�������ᳬ��13��
std::vector<mahjong::pack_t> my_fixed_packs_vector;
//ͬ����ԭ��,����hand_tiles_t
std::vector<mahjong::pack_t> all_player_fixed_packs[4];
//��¼�����˵ĸ�¶��ע��Ժ�����ʱ�򲻹���offer��������Ϣ��
//ֻ�и��Ƶ�ʱ����offer��ʾ�����ܣ�0��ʾ���ܣ�1��ʾ���ܣ�����ʱ��֪�����廨ɫ
//�Ҳ������Լ��ĸ�¶��Ϣ�����¼��hand_tiles���棩
std::vector<mahjong::tile_t> all_player_flowers[4];
//�����˵Ļ��Ƽ�¼�������Լ��Ļ���
std::vector<mahjong::tile_t> tiles_river;
//�ƺ�,����������

mahjong::tile_t str2tile_t(const std::string &s) {
    //��ο�tiles.h
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
            throw 1;
    }
    return mahjong::make_tile(suit, rank);
}

std::string tile_t2str(const mahjong::tile_t &tile) {
    std::string type;
    char rank = mahjong::tile_get_rank(tile) + '0';
    switch (mahjong::tile_get_suit(tile)) {
        case TILE_SUIT_CHARACTERS:
            type = "W";
            break;
        case TILE_SUIT_DOTS:
            type = "B";
            break;
        case TILE_SUIT_BAMBOO:
            type = "T";
            break;
        case TILE_SUIT_HONORS:
            if (rank >= '1' && rank <= '4') {
                //����
                type = "F";
            } else {
                //����
                type = "J";
                rank -= 4;
            }
            break;
        case 5:
            type = "H";
            break;
        default:
            throw 1;
    }
    type.push_back(rank);
    return type;
}

std::string pack_t2str(const mahjong::pack_t &pack) {
    std::string card = tile_t2str(mahjong::pack_get_tile(pack));
    std::string res = "[";
    switch (mahjong::pack_get_type(pack)) {
        case PACK_TYPE_PUNG:
            res += card + card + card;
            break;
        case PACK_TYPE_KONG:
            res += card + card + card + card;
            break;
        case PACK_TYPE_CHOW:
            --card[1];
            res += card;
            ++card[1];
            res += card;
            ++card[1];
            res += card;
            break;
        default:
            throw 1;
    }
    res += "]";
    return res;
}

void init() {
    //��ʼ��ȫ�ֱ���


    {
        //��ȡ����
        std::string s;
        std::cin >> turnID;
        std::getchar();//ȡ��'\n'
        for (int i = 1; i < turnID; i++) {
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

    0."0 playerID quan"               ֻ�ڵ�һ�غϳ��֣������š���Ȧ

    1."1 hua0 hua1 hua2 hua3 handCard(1~13) flower0_(1~hua0) flower1_(1~hua1)..."
                                   �������ĸ���������Ļ��������Լ���ʮ�������ƣ��Ѿ��������������ڻ��ƣ���������������Ļ���

    2."2 card0                    ��ʾ�Լ�����card0

    3."3 playerID BUHUA card0"        ��ʾ�������card0������һ����

    4."3 playerID DRAW"               ��ʾ����һ����

    5."3 playerID PLAY card0"         ��ʾ���card0

    6."3 playerID PENG card0"         ��ʾ��card0

    7."3 playerID CHI card0 card1"    ��ʾ�����ϼҵ��ƺ����ɵ�˳���м���Ϊcard0�����Ҵ��card1

    8."3 playerID GANG"               ��ʾ���ƣ��������Ҹ����������ǰ��ܣ����������Ҹմ������������

    9."3 playerID BUGANG card0"       ��ʾ����

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
    for (int turn = 0; turn < turnID - 1; ++turn) {
        std::cout << request[turn] << std::endl;
        iss.str(request[turn]);
        int request_type, playerID;
        std::string op, card0, card1;
        iss >> request_type;
        switch (request_type) {
            case 0: {
                iss >> myPlayerID >> quan;
                break;
            }
            case 1: {
                int flower_cnt[4];
                for (int &x:flower_cnt)iss >> x;
                for (int ii = 0; ii < 13; ++ii) {
                    iss >> card0;
                    my_hand_tiles_vector.push_back(str2tile_t(card0));
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
                my_hand_tiles_vector.push_back(str2tile_t(card0));//��ʱ���ƿ�����14�ţ��޸�¶�����
                break;
            }
            case 3: {
                iss >> playerID >> op;
                if (op == "BUHUA") {
                    iss >> card0;
                    all_player_flowers[playerID].push_back(str2tile_t(card0));
                } else if (op == "DRAW") {
                    //������ҵ����ƣ�û���κ���Ϣ
                } else if (op == "PLAY") {
                    iss >> card0;
                    auto tile_discard = str2tile_t(card0);
                    if (playerID == myPlayerID) {
                        auto p = find(my_hand_tiles_vector.begin(), my_hand_tiles_vector.end(), tile_discard);
                        assert(p != my_hand_tiles_vector.end());
                        my_hand_tiles_vector.erase(p);
                    }
                    tiles_river.push_back(tile_discard);
                } else if (op == "PENG") {
                    iss >> card0;
                    auto tile_pung = tiles_river.back();
                    tiles_river.pop_back();
                    auto tile_discard = str2tile_t(card0);
                    if (playerID == myPlayerID) {
                        auto p = std::find(my_hand_tiles_vector.begin(), my_hand_tiles_vector.end(), tile_pung);
                        assert(p != my_hand_tiles_vector.end());
                        my_hand_tiles_vector.erase(p);//ɾ����һ��

                        p = std::find(my_hand_tiles_vector.begin(), my_hand_tiles_vector.end(), tile_pung);
                        assert(p != my_hand_tiles_vector.end());
                        my_hand_tiles_vector.erase(p);//ɾ���ڶ���

                        p = std::find(my_hand_tiles_vector.begin(), my_hand_tiles_vector.end(), tile_discard);
                        assert(p != my_hand_tiles_vector.end());
                        my_hand_tiles_vector.erase(p);//ɾ���������

                        my_fixed_packs_vector.push_back(mahjong::make_pack(0, PACK_TYPE_PUNG, tile_pung));//������
                    } else all_player_fixed_packs[playerID].push_back(mahjong::make_pack(0, PACK_TYPE_PUNG, tile_pung));
                    //���ﲻ����˭�����ƣ�ֻ����������һ�����ӣ���������Ч����
                    tiles_river.push_back(tile_discard);
                } else if (op == "CHI") {
                    iss >> card0 >> card1;
                    //card0������˳�ӵ��м��ƣ�card1�Ǵ����
                    auto tile_mid = str2tile_t(card0);
                    auto tile_discard = str2tile_t(card1);
                    auto tile_chow = tiles_river.back();//�Ե��Ʊض������ƺ����һ��
                    tiles_river.pop_back();
                    if (playerID == myPlayerID) {
                        std::vector<mahjong::tile_t> chow_in_hand;
                        chow_in_hand.push_back(tile_mid - (unsigned char) 1);
                        chow_in_hand.push_back(tile_mid);
                        chow_in_hand.push_back(tile_mid + (unsigned char) 1);
                        //���ɳԵ�������
                        auto p = find(chow_in_hand.begin(), chow_in_hand.end(), tile_chow);
                        assert(p != chow_in_hand.end());
                        chow_in_hand.erase(p);
                        //ȥ���ƺ������һ�ţ�Ҳ����ʣ��������������
                        for (const auto &tile:chow_in_hand) {
                            auto q = find(my_hand_tiles_vector.begin(), my_hand_tiles_vector.end(), tile);
                            assert(q != my_hand_tiles_vector.end());
                            my_hand_tiles_vector.erase(q);
                        }//�������������ɾ��
                        my_fixed_packs_vector.push_back(mahjong::make_pack(0, PACK_TYPE_CHOW, tile_mid));
                    } else all_player_fixed_packs[playerID].push_back(mahjong::make_pack(0, PACK_TYPE_CHOW, tile_mid));
                    tiles_river.push_back(tile_discard);
                } else if (op == "GANG") {
                    if (playerID == myPlayerID) {
                        //������Լ���һ��֪����ɫ
                        iss.str(request[turn - 1]);
                        int last_request_type;
                        iss >> last_request_type;
                        if (last_request_type == 2) {
                            //˵�����Լ����ƣ���ʹ����
                            iss >> card0;
                            auto tile_kong = str2tile_t(card0);
                            for (int tt = 0; tt < 3; ++tt) {
                                auto p = find(my_hand_tiles_vector.begin(), my_hand_tiles_vector.end(), tile_kong);
                                assert(p != my_hand_tiles_vector.end());
                                my_hand_tiles_vector.erase(p);
                            }//ɾ����������
                            my_fixed_packs_vector.push_back(mahjong::make_pack(0, PACK_TYPE_KONG, tile_kong));
                            //���ɰ��ܣ���offerΪ0
                        } else {
                            //˵���Ǹ��ƺ���һ����
                            auto tile_kong = tiles_river.back();
                            tiles_river.pop_back();
                            for (int tt = 0; tt < 3; ++tt) {
                                auto p = find(my_hand_tiles_vector.begin(), my_hand_tiles_vector.end(), tile_kong);
                                assert(p != my_hand_tiles_vector.end());
                                my_hand_tiles_vector.erase(p);
                            }//ɾ����������
                            my_fixed_packs_vector.push_back(mahjong::make_pack(1, PACK_TYPE_KONG, tile_kong));
                            //�������ܣ�ͳһofferΪ1
                        }
                    } else {
                        iss.str(request[turn - 1]);
                        //����һrequest�Ƿ��������ƣ�������򰵸ܣ���������
                        //��һ��requestֻ�����ֿ��ܣ�һ���Ǹ��������ƣ���һ���ǷǸ����˴���
                        //����ǰ��ܣ���pack_tile����Ч�ģ���������Ч
                        int last_request_type;
                        int lastPlayerID;
                        std::string last_op;
                        iss >> last_request_type >> lastPlayerID >> last_op;
                        if (playerID == lastPlayerID) {
                            assert(last_request_type == 3);
                            assert(last_op == "DRAW");
                            auto tile_kung = str2tile_t("D1");//ͳһ����һ����ʵ���ǲ�֪����ɫ�İ���
                            all_player_fixed_packs[playerID].push_back(
                                    mahjong::make_pack(0, PACK_TYPE_KONG, tile_kung));
                        } else {
                            auto tile_kung = tiles_river.back();
                            //���ƺӵ�����һ����
                            tiles_river.pop_back();
                            all_player_fixed_packs[playerID].push_back(
                                    mahjong::make_pack(1, PACK_TYPE_KONG, tile_kung));
                        }
                    }
                } else if (op == "BUGANG") {
                    iss >> card0;
                    auto tile_kong = str2tile_t(card0);
                    if (playerID == myPlayerID) {
                        bool flag = true;
                        for (auto &p:my_fixed_packs_vector) {
                            if (mahjong::pack_get_type(p) == PACK_TYPE_PUNG &&
                                mahjong::pack_get_tile(p) == tile_kong) {
                                flag = false;
                                p = mahjong::promote_pung_to_kong(p);
                                break;
                            }
                        }
                        assert(!flag);
                    } else {
                        bool flag = true;
                        for (auto &p:all_player_fixed_packs[playerID]) {
                            if (mahjong::pack_get_type(p) == PACK_TYPE_PUNG &&
                                mahjong::pack_get_tile(p) == tile_kong) {
                                flag = false;
                                p = mahjong::promote_pung_to_kong(p);
                                break;
                            }
                        }
                        assert(!flag);
                    }
                }
                break;
            }
            default:
                throw 1;
        }
    }

    //��������
    my_hand_tiles.tile_count = 0;
    for (const auto &tile:my_hand_tiles_vector)my_hand_tiles.standing_tiles[my_hand_tiles.tile_count++] = tile;
    my_hand_tiles.pack_count = 0;
    for (const auto &pack:my_fixed_packs_vector)my_hand_tiles.fixed_packs[my_hand_tiles.pack_count++] = pack;

    //��ʼ���
    for (auto const &tile:tiles_river)++global_tiles_table[tile];
    //�ƺӴ��
    for (auto const &v:all_player_flowers)for (auto const &tile_flower:v)++global_tiles_table[tile_flower];
    //���ƴ��
    for (auto const &tile:my_hand_tiles_vector)++global_tiles_table[tile], ++my_hand_tiles_table[tile];
    //�������ƴ��
    for (auto const &p:my_fixed_packs_vector) {
        //�Լ���¶���
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
                throw 1;
        }
    }
    for (auto const &v:all_player_fixed_packs) {
        //�����˸�¶���
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
                    if (offer == 1)global_tiles_table[tile] += 4;
                    break;//���ܲ����
                default:
                    throw 1;
            }
        }
    }
}

int main() {
#ifdef LOCAL_DEBUG
    std::freopen("in.txt", "r", stdin);
    std::freopen("out.txt", "w", stdout);
#endif


    init();
    std::cout << "River is:" << std::endl;
    for (const auto &tile:tiles_river) {
        std::cout << tile_t2str(tile) << ' ';
    }
    std::cout << std::endl << std::endl;
    std::cout << "My hand tiles are:" << std::endl;
    for (const auto &tile:my_hand_tiles_vector) {
        std::cout << tile_t2str(tile) << ' ';
    }
    std::cout << std::endl << std::endl;
    std::cout << "My fixed packs are:" << std::endl;
    for (const auto &pack:my_fixed_packs_vector) {
        std::cout << pack_t2str(pack) << ' ';
    }
    std::cout << std::endl << std::endl;
    std::cout << "Other players' packs are:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << i << ' ';
        for (const auto &pack:all_player_fixed_packs[i]) {
            std::cout << pack_t2str(pack) << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "All players' flowers are:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << i << ' ';
        for (const auto &tile:all_player_flowers[i]) {
            std::cout << tile_t2str(tile) << ' ';
        }
        std::cout << std::endl;
    }
    return 0;
}

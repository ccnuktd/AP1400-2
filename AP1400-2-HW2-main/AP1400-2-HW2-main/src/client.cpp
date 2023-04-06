#include <random>
#include "client.h"
#include "crypto.h"

Client::Client(std::string id, const Server& server)
: server(&server), id(id), public_key(), private_key() {
    crypto::generate_key(public_key, private_key);
}

std::string Client::get_id() const {
    return id;
}

double Client::get_wallet() const {
    return server->get_wallet(get_id());
}

std::string Client::get_publickey() const {
    return public_key;
}

std::string Client::sign(std::string txt) const{
    return crypto::signMessage(private_key, txt);
}

bool Client::transfer_money(std::string receiver, double value) {
    std::string trx = get_id() + "-" + receiver + "-" + std::to_string(value);
    std::string signature = sign(trx);
    return ((Server* const)server)->add_pending_trx(trx, signature);
}

size_t Client::generate_nonce() {
    size_t min_num = std::numeric_limits<int>::min(), max_num = std::numeric_limits<int>::max();
    std::random_device seed;//硬件生成随机数种子
    std::ranlux48 engine(seed());//利用种子生成随机数引擎
    std::uniform_int_distribution<> distrib(min_num, max_num);//设置随机数范围，并为均匀分布
    return distrib(engine);//随机数
}
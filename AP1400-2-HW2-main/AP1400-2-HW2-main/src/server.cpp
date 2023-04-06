#include <iostream>
#include <memory>
#include "server.h"
#include "client.h"
#include "crypto.h"
#include <random>

std::vector<std::string> pending_trxs {};

std::shared_ptr<Client> Server::add_client(std::string id) {
    // 查找id是否存在
    // 如果存在则随机增加四个数字
    // 将新用户加入到server中，并初始化5枚硬币
    bool id_exist = false;
    std::string search_id = id;

    for (const auto& client: clients) {
        if ((client.first)->get_id() == search_id) {
            id_exist = true;
            break;
        }
    }

    while (id_exist) {
        std::random_device rd{};
        std::uniform_int_distribution<> d{0, 10};
        for (int i = 0; i < 4; i ++) {
            search_id += std::to_string(d(rd));
        }

        id_exist = false;
        for (const auto& client: clients) {
            if ((client.first)->get_id() == search_id) {
                id_exist = true;
                break;
            }
        }
    }

    std::shared_ptr<Client> new_client(new Client(search_id, *this));
    clients.insert({new_client, 5.0});
    return new_client;
}

std::shared_ptr<Client> Server::get_client(std::string id) const {
    for (auto& client: clients) {
        if ((client.first)->get_id() == id) {
            return client.first;
        }
    }
    return nullptr;
}

double Server::get_wallet(std::string id) const{
    auto search = clients.find(get_client(id));
    if (search != clients.end()) {
        return search->second;
    }
    return 0;
}

void show_wallets(const Server& server) {
    std::cout << std::string(20, '*') << std::endl;
    for(const auto& client: server.clients)
        std::cout << client.first->get_id() <<  " : "  << client.second << std::endl;
    std::cout << std::string(20, '*') << std::endl;
}

bool Server::parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value) {
    auto first = find(trx.begin(), trx.end(), '-');
    if (first != trx.end()) {
        auto second = find(first + 1, trx.end(), '-');
        if (second != trx.end()) {
            sender.assign(trx.begin(), first);
            receiver.assign(first + 1, second);
            std::string tmp;
            tmp.assign(second + 1, trx.end());
            try {
                value = stod(tmp);
            }
            catch (std::exception& e) {
                throw std::runtime_error("runtime error!");
            }
            return true;
        }
    }
    throw std::runtime_error("runtime error!");
}

bool Server::add_pending_trx(std::string trx, std::string signature) {
    // 先通过trx查看sender和receiver是否存在，然后找到sender的公钥，然后通过公钥验证signature，最后看一下的sender有没有足够的钱，如果有则将交易加入到pending_trxs中
    std::string sender, receiver;
    double value = 0;
    parse_trx(trx, sender, receiver, value);
    std::shared_ptr<Client> sender_ptr = nullptr, receiver_ptr = nullptr;

    for (auto& client: clients) {
        if ((client.first)->get_id() == sender) {
            sender_ptr = client.first;
            break;
        }
    }
    if (sender_ptr == nullptr) return false;
    for (auto& client: clients) {
        if ((client.first)->get_id() == receiver) {
            receiver_ptr = client.first;
            break;
        }
    }
    if (receiver_ptr == nullptr) return false;

    if (crypto::verifySignature(sender_ptr->get_publickey(), trx, signature)) {
        if (sender_ptr->get_wallet() >= value) {
            pending_trxs.push_back(trx);
            return true;
        }
    }
    return false;
}

size_t Server::mine() {
    std::string mempool{};
    for(const auto& trx : pending_trxs)
        mempool += trx;

    size_t nonce;
    bool flag = true;
    while (flag) {
        for (auto& client: clients) {
            nonce = client.first->generate_nonce();
            std::string hash = crypto::sha256(mempool + std::to_string(nonce));
            if (hash.substr(0, 10).find("000") != std::string::npos) {
                // 完成交易
                std::cout << "success miner: " << client.first->get_id() << std::endl;
                client.second += 6.25;
                flag = false;
                break;
            }
        }
    }
    for(const auto& trx : pending_trxs) {
        std::string sender, receiver;
        double value = 0;
        parse_trx(trx, sender, receiver, value);
        std::shared_ptr<Client> sender_ptr = nullptr, receiver_ptr = nullptr;
        for (auto& client: clients) {
            if ((client.first)->get_id() == sender) {
                client.second -= value;
                break;
            }
        }
        for (auto& client: clients) {
            if ((client.first)->get_id() == receiver) {
                client.second += value;
                break;
            }
        }
    }
    return nonce;
}
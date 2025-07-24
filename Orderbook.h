#pragma once

#include <map>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "Usings.h"
#include "Order.h"
#include "OrderModify.h"
#include "Trade.h"

class Orderbook {
    private:
        struct OrderEntry {
            OrderPointer order_{ nullptr };
            OrderPointers::iterator location_;
        };

        std::map<Price, OrderPointers, std::greater<Price>> bids_;
        std::map<Price, OrderPointers, std::less<Price>> asks_;
        std::unordered_map<OrderId, OrderEntry> orders_;
        mutable std::mutex ordersMutex_;
        std::thread ordersPruneThread_;
        std::condition_variable shutdownConditionVariable_;
        std::atomic<bool> shutdown_{ false };

        void PruneGoodForDayOrders();

        void CancelOrders(OrderIds orderIds);
        void CancelOrderInternal(OrderId orderId);

        bool CanMatch(Side side, Price price) const;
        Trades MatchOrders();

    public:
        Orderbook();
        Orderbook(const Orderbook&) = delete;
        void operator=(const Orderbook&) = delete;
        Orderbook(Orderbook&&) = delete;
        void operator=(Orderbook&&) = delete;
        ~Orderbook();


        Trades AddOrder(OrderPointer order);
        void CancelOrder(OrderId orderId);
        Trades ModifyOrder(OrderModify order);
        std::size_t Size() const;
};
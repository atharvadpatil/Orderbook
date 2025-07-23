#include "Orderbook.h"

bool Orderbook::CanMatch(Side side, Price price) const {
    if(side == Side::Buy) {
        if(asks_.empty()) 
            return false;

        const auto& [bestask, _] = *asks_.begin();
        return price >= bestask;
    }
    else {
        if(bids_.empty()) 
            return false;

        const auto& [bestbid, _] = *bids_.begin();
        return price <= bestbid;
    }
}

Trades Orderbook::MatchOrders() {
    Trades trades;
    trades.reserve(orders_.size());

    while(true) {
        if(bids_.empty() || asks_.empty())
            break;

        auto& [bidPrice, bids] = *bids_.begin();
        auto& [askPrice, asks] = *asks_.begin();

        if(bidPrice < askPrice)
            break;

        while(bids.size() && asks.size()) {
            auto& bid = bids.front();
            auto& ask = asks.front();

            Quantity quantity = std::min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity());

            bid->Fill(quantity);
            ask->Fill(quantity);

            if(bid->IsFilled()){
                bids.pop_front();
                orders_.erase(bid->GetOrderId());
            }

            if(ask->IsFilled()){
                asks.pop_front();
                orders_.erase(ask->GetOrderId());
            }

            if(bids.empty())
                bids_.erase(bidPrice);

            if(asks.empty())
                asks_.erase(askPrice);

            trades.push_back(Trade{
                TradeInfo{ bid->GetOrderId(), bid->GetPrice(), quantity },
                TradeInfo{ ask->GetOrderId(), ask->GetPrice(), quantity }
            });
        }
    }
    if(!bids_.empty()) {
        auto& [_, bids] = *bids_.begin();
        auto& order = bids.front();
        if(order->GetOrderType() == OrderType::FillAndKill)
            CancelOrder(order->GetOrderId());
        
    }

    if(!asks_.empty()) {
        auto& [_, asks] = *asks_.begin();
        auto& order = asks.front();
        if(order->GetOrderType() == OrderType::FillAndKill)
            CancelOrder(order->GetOrderId()); 
    }

    return trades;
}

Trades Orderbook::AddOrder(OrderPointer order) {
    if(orders_.contains(order->GetOrderId()))
        return { };

    if(order->GetOrderType() == OrderType::FillAndKill && !CanMatch(order->GetSide(), order->GetPrice()))
        return { };

    OrderPointers::iterator iterator;

    if(order->GetSide() == Side::Buy) {
        auto& orders = bids_[order->GetPrice()];
        orders.push_back(order);
        iterator = std::next(orders.begin(), orders.size()-1);
    }
    else{
        auto& orders = asks_[order->GetPrice()];
        orders.push_back(order);
        iterator = std::next(orders.begin(), orders.size()-1);
    }

    orders_.insert({ order->GetOrderId(), OrderEntry{order, iterator} });
    return MatchOrders();
}

void Orderbook::CancelOrder(OrderId orderId) {
    if(!orders_.contains(orderId))
        return;
    
    const auto& [order, orderIterator] = orders_.at(orderId);

    if(order->GetSide() == Side::Sell) {
        auto price = order->GetPrice();
        auto& orders = asks_.at(price);
        orders.erase(orderIterator);
        if(orders.empty())
            asks_.erase(price);
    }
    else {
        auto price = order->GetPrice();
        auto& orders = bids_.at(price);
        orders.erase(orderIterator);
        if(orders.empty())
            bids_.erase(price);
    }

}

Trades Orderbook::ModifyOrder(OrderModify order) {
    if(!orders_.contains(order.GetOrderId()))
        return { };

    const auto& [existingOrder, _] = orders_.at(order.GetOrderId());
    CancelOrder(order.GetOrderId());
    return AddOrder(order.ToOrderPointer(existingOrder->GetOrderType()));
}

std::size_t Orderbook::Size() const {
	return orders_.size(); 
}
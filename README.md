# OrderBook

A high-performance, extensible **Orderbook** system implemented in C++ that supports multiple order types.

## Features

- Supported order types:
  - `GoodTillCancel: Remains active until fully filled or explicitly canceled.`
  - `FillAndKill: Executes as much as possible immediately; cancels any unfilled portion.`
  - `FillOrKill: Must be fully filled immediately or it gets canceled entirely.`
  - `GoodForDay: Valid only for the current trading day; expires after 4 pm.`
  - `Market: Executes immediately at the best available price.`
- Efficient order matching engine
- Unit tests are written using the GoogleTest framework.

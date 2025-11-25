# OpenDataSoftAPI
api for daten.sg.ch Willkommen auf dem Open Data Portal von St.Gallen
# main
```cpp
#include "OpenDataSoftAPI.h"
#include <iostream>

int main() {
   OpenDataSoftAPI api;
    auto exports = api.get_catalog_exports().then([](json::value result) {
        std::cout << result<< std::endl;
    });
    exports.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```

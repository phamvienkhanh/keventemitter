
#include "include/event_emitter.hpp"
#include <iostream>

auto main() -> int {

    klib::EventEmitter event;
    uint32_t id = event.on(1, [](klib::argsType args) {
        for(auto& val : args) {
            if(val.has_value()) {
                std::cout << std::any_cast<int>(val) << "\n";
            }
        }
    });

    event.on(2, [](klib::argsType args) {
        int a;
        a = 4;
    });

    event.emit(1, {1});
    event.emit(2, {1 , 2, 3});

    event.removeListener(1, id);

    event.emit(1, {1});
    
    return 1;
}


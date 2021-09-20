
#include "include/event_emitter.hpp"
#include <iostream>

#include <thread>

auto main() -> int {

    std::cout << "main : " << std::this_thread::get_id() << std::endl;

    klib::KEventLoop* eventLoop = new klib::KEventLoop([](klib::KEvent event){
        if(event.eventType == klib::EventEmitterType) {
            auto eData = std::any_cast<klib::EventEmitter::EventData>(event.eventdata);
            eData.listener.call(eData.args);
        }
        
    });

    klib::EventEmitter event(eventLoop);
    uint32_t id = event.on(1, [](klib::argsType args) {
        std::cout << "on 1 : " << std::this_thread::get_id() << std::endl;
        for(auto& val : args) {
            if(val.has_value()) {
                std::cout << "val : " <<std::any_cast<int>(val) << "\n";
            }
        }
    });

    event.on(2, [](klib::argsType args) {
        std::cout << "on 2 : " << std::this_thread::get_id() << std::endl;
    });

    std::thread thread1([&](){
        std::cout << "thread emitter : " << std::this_thread::get_id() << std::endl;
        event.emit(1, {1, 2});
    });

    std::thread thread2([&](){
        std::cout << "thread emitter : " << std::this_thread::get_id() << std::endl;
        event.emit(2, {1 , 2, 3});
    });

    event.emit(1, {1});


    while (1)
    {
        eventLoop->step();
    }
    
    thread1.join();
    thread2.join();
    
    return 1;
}


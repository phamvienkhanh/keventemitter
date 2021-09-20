#ifndef _KLIB_EVENT_EMITTER_
#define _KLIB_EVENT_EMITTER_

#include <map>
#include <any>
#include <mutex>
#include <tuple>
#include <vector>
#include <functional>

#include "event_loop.hpp"


namespace klib {

    using argsType = std::vector<std::any>;
    using callbackType = std::function<void(argsType)>;
    
    std::string EventEmitterType = "klib:emitter";
    class Listener
    {    
    public:
        Listener(uint32_t id, callbackType callback) 
        : m_callback(callback)
        , m_id(id){}

        ~Listener(){};

        void call(argsType args) {
            m_callback(args);         
        }

    private:
        uint32_t m_id;
        callbackType m_callback;
    };

    class EventEmitter
    {
    public:
        struct EventData {
            Listener listener;
            argsType args;
        };
        
    public:
        EventEmitter(KEventLoop* eventLoop = nullptr) 
        : m_lastId(0)
        , m_eventLoop(eventLoop){}
        ~EventEmitter() {};

        uint32_t addListener(uint32_t eventId, callbackType callback) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_lastId++;
            m_listeners.insert(std::make_pair(eventId, Listener(m_lastId, callback)));

            return m_lastId;
        }

        void removeListener(uint32_t eventId, uint32_t listenerId) {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto range = m_listeners.equal_range(eventId);
            for(auto iter = range.first; iter != range.second; ++iter) {
                if(iter->first == listenerId) {
                    m_listeners.erase(iter);
                    break;
                }
            }
        }

        uint32_t on(uint32_t eventId, callbackType callback) {
            return addListener(eventId, callback);
        }

        void emit(uint32_t eventId, argsType args) {
            auto range = m_listeners.equal_range(eventId);

            for(auto iter = range.first; iter != range.second; ++iter) {
                if(m_eventLoop) {
                    klib::KEvent event;
                    event.eventType = klib::EventEmitterType;
                    event.eventdata = EventData {
                        iter->second,
                        args
                    };

                    m_eventLoop->pushEvent(event);
                }
                else {
                    iter->second.call(args);
                }
            }
        }

    private:
        std::multimap<uint32_t, Listener> m_listeners;
        uint32_t m_lastId;
        std::mutex m_mutex;
        KEventLoop* m_eventLoop;     
    };
}

#endif
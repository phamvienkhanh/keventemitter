#ifndef _KLIB_EVENT_EMITTER_
#define _KLIB_EVENT_EMITTER_

#include <map>
#include <any>
#include <mutex>
#include <tuple>
#include <vector>
#include <functional>


namespace klib {

    using argsType = std::vector<std::any>;
    using callbackType = std::function<void(argsType)>;

    class Listener
    {    
    public:
        Listener(uint32_t id, callbackType callback) : m_callback(callback), m_id(id) {}
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
        EventEmitter() : m_lastId(0) {}
        ~EventEmitter() {};

        uint32_t addListener(uint32_t eventId, callbackType callback) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_lastId++;
            m_listeners.insert(std::make_pair(eventId, Listener(m_lastId, callback)));

            return m_lastId;
        }

        void removeListener(uint32_t eventId, uint32_t listenerId) {
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
                iter->second.call(args);
            }
        }

    private:
        std::multimap<uint32_t, Listener> m_listeners;
        uint32_t m_lastId;
        std::mutex m_mutex;       
    };
}

#endif
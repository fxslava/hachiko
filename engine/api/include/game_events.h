#pragma once
#include <list>

template<class ...args_t>
class event_handler_i
{
public:
    virtual void call(args_t... args) = 0;
};

template<class ...args_t>
class game_event_c
{
    using event_handler_c = event_handler_i<args_t...>;
public:
    game_event_c() : event_handlers() {}
    ~game_event_c() {
        for (event_handler_c* handler : event_handlers)
            delete handler;
        event_handlers.clear();
    }

    void operator()(args_t... args) {
        for (event_handler_c* handler : event_handlers)
            handler->call(args...);
    }

    void operator+=(event_handler_c* event_handler) {
        for (event_handler_c* handler : event_handlers)
            if (handler == event_handler)
                return;

        event_handlers.push_back(event_handler);
    }

    void operator-=(event_handler_c* event_handler) {
        event_handlers.push_back(event_handler);
        auto to_remove = event_handler;
    	auto pred = [to_remove](event_handler_c* handler)
    	{
    		return handler == to_remove;
    	};
        event_handlers.remove_if(pred);
    }
private:
    std::list<event_handler_c*> event_handlers;
};
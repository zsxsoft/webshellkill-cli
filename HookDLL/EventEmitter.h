#pragma once
//
// Copyright (c) 2014 Sean Farrell
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//

#ifndef _EVENT_EMITTER_H_
#define _EVENT_EMITTER_H_

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <list>
#include <algorithm>

class EventEmitter
{
public:

	EventEmitter();

	~EventEmitter();

	template <typename... Args>
	unsigned int add_listener(unsigned int event_id, std::function<void(Args...)> cb);

	unsigned int add_listener(unsigned int event_id, std::function<void()> cb);

	template <typename... Args>
	unsigned int on(unsigned int event_id, std::function<void(Args...)> cb);

	unsigned int on(unsigned int event_id, std::function<void()> cb);

	void remove_listener(unsigned int listener_id);

	template <typename... Args>
	void emit(unsigned int event_id, Args... args);

private:
	struct ListenerBase
	{
		ListenerBase() {}

		ListenerBase(unsigned int i)
			: id(i) {}

		virtual ~ListenerBase() {}

		unsigned int id;
	};

	template <typename... Args>
	struct Listener : public ListenerBase
	{
		Listener() {}

		Listener(unsigned int i, std::function<void(Args...)> c)
			: ListenerBase(i), cb(c) {}

		std::function<void(Args...)> cb;
	};

	std::mutex mutex;
	unsigned int last_listener;
	std::multimap<unsigned int, std::shared_ptr<ListenerBase>> listeners;

	EventEmitter(const EventEmitter&) = delete;
	const EventEmitter& operator = (const EventEmitter&) = delete;
};

template <typename... Args>
unsigned int EventEmitter::add_listener(unsigned int event_id, std::function<void(Args...)> cb)
{
	if (!cb)
	{
		throw std::invalid_argument("EventEmitter::add_listener: No callbak provided.");
	}

	std::lock_guard<std::mutex> lock(mutex);

	unsigned int listener_id = ++last_listener;
	listeners.insert(std::make_pair(event_id, std::make_shared<Listener<Args...>>(listener_id, cb)));

	return listener_id;
}

template <typename... Args>
unsigned int EventEmitter::on(unsigned int event_id, std::function<void(Args...)> cb)
{
	return add_listener(event_id, cb);
}

template <typename... Args>
void EventEmitter::emit(unsigned int event_id, Args... args)
{
	std::list<std::shared_ptr<Listener<Args...>>> handlers;

	{
		std::lock_guard<std::mutex> lock(mutex);

		auto range = listeners.equal_range(event_id);
		handlers.resize(std::distance(range.first, range.second));
		std::transform(range.first, range.second, handlers.begin(), [](std::pair<const unsigned int, std::shared_ptr<ListenerBase>> p) {
			auto l = std::dynamic_pointer_cast<Listener<Args...>>(p.second);
			if (l)
			{
				return l;
			}
			else
			{
				throw std::logic_error("EventEmitter::emit: Invalid event signature.");
			}
		});
	}

	for (auto& h : handlers)
	{
		h->cb(args...);
	}
}

#endif
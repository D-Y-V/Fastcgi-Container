// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
// Copyright (C) 2015 Alexander Ponomarenko <contact@propulsion-analysis.com>

// This file is part of Fastcgi Container.
//
// Fastcgi Container is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fastcgi Container is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Fastcgi Container. If not, see <http://www.gnu.org/licenses/>.

#ifndef _FASTCGI_FASTCGI_ENDPOINT_H_
#define _FASTCGI_FASTCGI_ENDPOINT_H_

#include <string>
#include <mutex>

namespace fastcgi
{

class Endpoint {
public:
	class ScopedBusyCounter {
	public:
		ScopedBusyCounter(Endpoint &endpoint);
		~ScopedBusyCounter();
		void increment();
		void decrement();
	private:
		bool incremented_;
		Endpoint &endpoint_;
	};

public:
	Endpoint(const std::string &path, const std::string &port, unsigned int keepConnection, unsigned short threads);
	virtual ~Endpoint();

	int socket() const;

	unsigned short threads() const;

	std::string toString() const;
	unsigned short getBusyCounter() const;

	void openSocket(const int backlog);

	unsigned int getKeepConnection();

	void incrementBusyCounter();
	void decrementBusyCounter();

private:
	int socket_;
	int busy_count_;
	unsigned short threads_;
	mutable std::mutex mutex_;
	std::string socket_path_, socket_port_;
	unsigned int keepConnection_;
};

} // namespace fastcgi

#endif // _FASTCGI_FASTCGI_ENDPOINT_H_

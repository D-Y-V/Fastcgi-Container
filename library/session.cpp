// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2015 Alexander Ponomarenko <contact@propulsion-analysis.com>

// This file is part of Fastcgi Container.
//
// Fastcgi Container is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License (LGPL) as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fastcgi Container is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License (LGPL) for more details.
//
// You should have received a copy of the GNU Lesser General Public License (LGPL)
// along with Fastcgi Container. If not, see <http://www.gnu.org/licenses/>.

#include <chrono>

#include "fastcgi3/session.h"

namespace fastcgi
{

Session::Session(const std::string &id)
: AttributesHolder(true),
  id_(id), created_(std::chrono::steady_clock::now()), last_accessed_(created_), max_inactive_interval_(30) {
	init();
}

Session::~Session() {
	invalidate();
}

void
Session::init() {

}

void
Session::changeId(std::string& newId) {
	id_ = newId;
}

const std::string&
Session::getId() const {
	return id_;
}

void
Session::invalidate() {
	removeAllAttributes();
	last_accessed_ = std::chrono::steady_clock::now() - max_inactive_interval_;
}

const std::chrono::steady_clock::time_point
Session::getCreationTime() const {
	return created_;
}

const std::chrono::steady_clock::time_point
Session::getLastAccessedTime() const {
	return last_accessed_;
}

void
Session::updateLastAccessedTime() {
	last_accessed_ = std::chrono::steady_clock::now();
}

bool
Session::isExpired() const {
	return (std::chrono::steady_clock::now() > (last_accessed_ + max_inactive_interval_));
}


}

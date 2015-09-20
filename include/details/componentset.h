// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
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

#ifndef _FASTCGI_DETAILS_COMPONENTSET_H_
#define _FASTCGI_DETAILS_COMPONENTSET_H_

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace fastcgi
{

class Globals;
class Loader;
class Component;
class ComponentContext;

class ComponentSet {
protected:
	struct ComponentContainer {
		ComponentContainer();

		std::shared_ptr<Component> component;
		std::shared_ptr<ComponentContext> context;
		bool isLoadingStarted;
    };
    using ComponentMap = std::map<std::string, ComponentContainer>;
	
public:
	ComponentSet();
	virtual ~ComponentSet();

	ComponentSet(const ComponentSet&) = delete;
	ComponentSet& operator=(const ComponentSet&) = delete;

	void init(const Globals *globals);
	
	std::shared_ptr<Component> find(const std::string &name) const;

protected:
	void add(const std::string &name, const std::string &type, const std::string &componentXPath);

	void sendOnLoadToComponents();
	void sendOnUnloadToComponents();
	void sendOnLoad(const std::string &componentName, ComponentContainer &cont);
	bool isComponentLoaded(const std::string &componentName) const;

private:
	ComponentMap components_;
	std::vector<std::string> loadingStack_;
	const Globals *globals_;
};

} // namespace fastcgi

#endif // _FASTCGI_DETAILS_COMPONENTSET_H_

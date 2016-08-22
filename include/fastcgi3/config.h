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

#ifndef _FASTCGI_CONFIG_H_
#define _FASTCGI_CONFIG_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iosfwd>

#include <regex>
#include <map>

#include <libxml/tree.h>

#include "fastcgi3/xml.h"
#include "fastcgi3/config.h"
#include "fastcgi3/helpers.h"

namespace fastcgi
{

using HelpFunc = std::ostream& (*)(std::ostream &stream);

class Config {
public:
	Config();
	virtual ~Config();

	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

	virtual int asInt(const std::string &value) const = 0;
	virtual int asInt(const std::string &value, int defval) const = 0;
	
	virtual std::string asString(const std::string &value) const = 0;
	virtual std::string asString(const std::string &value, const std::string &defval) const = 0;
	
	virtual void subKeys(const std::string &value, std::vector<std::string> &v) const = 0;
	
	static std::unique_ptr<Config> create(const char *file);
	static std::unique_ptr<Config> create(int &argc, char *argv[], HelpFunc func = nullptr);

	const std::string& filename() const;

protected:
	void setFilename(const std::string &name);

private:
	std::string filename_;
};

class XmlConfig : public Config
{
public:
	XmlConfig(const char *file);
	XmlConfig(const char *contents, unsigned int length);
	virtual ~XmlConfig();

	XmlConfig(const Config&) = delete;
	XmlConfig& operator=(const XmlConfig&) = delete;

	virtual int asInt(const std::string &value) const;
	virtual int asInt(const std::string &value, int defval) const;

	virtual std::string asString(const std::string &value) const;
	virtual std::string asString(const std::string &value, const std::string &defval) const;

	virtual void subKeys(const std::string &value, std::vector<std::string> &v) const;

private:
	void findVariables(const XmlDocHelper &doc);
	void resolveVariables(std::string &val) const;
	const std::string& findVariable(const std::string &key) const;

private:
	XmlDocHelper doc_;
	std::regex regex_;
	std::map<std::string, std::string> vars_;
};

} // namespace fastcgi

#endif // _FASTCGI_CONFIG_H_

// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (c) 2008 Applied Informatics Software Engineering GmbH and Contributors (POCO Project).
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

#include <string>
#include <cstring>
#include <regex>


#include "page.h"
#include "code_writer.h"
#include "utils.h"

CodeWriter::CodeWriter(const Page& page, const std::string& clazz):
	_page(page), _class(clazz),
	_removeEmptyString("(\\s?responseStream << \"\";|\\s?responseStream << \"\\\\n\";)", std::regex_constants::ECMAScript|std::regex_constants::icase)
{
}

CodeWriter::~CodeWriter() {
}


void
CodeWriter::writeHeader(std::ostream& ostr, const std::string& headerFileName) {
	beginGuard(ostr, headerFileName);
	writeHeaderIncludes(ostr);
	ostr << "\n\n";

	std::string decls(_page.headerDecls().str());
	if (!decls.empty()) {
		ostr << decls << "\n\n";
	}

	beginNamespace(ostr);
	writeHandlerClass(ostr);
	endNamespace(ostr);
	endGuard(ostr, headerFileName);
}


void
CodeWriter::writeImpl(std::ostream& ostr, const std::string& headerFileName) {
	ostr << "#include \"" << headerFileName << "\"\n";
	writeImplIncludes(ostr);
	ostr << "\n";

	std::string decls(_page.implDecls().str());
	if (!decls.empty()) {
		ostr << decls << "\n\n";
	}

	std::string functions(_page.functions().str());
	if (!functions.empty()) {
		ostr << functions << "\n\n";
	}

	beginNamespace(ostr);

	std::string path = _page.getAttribute<std::string>("page.path", "");
	if (!path.empty()) {
		ostr << "\tconst std::string " << _class << "::PATH(\"" << path << "\");\n\n\n";
	}

	writeConstructor(ostr);
	writeHandler(ostr);
	endNamespace(ostr);
}

void
CodeWriter::beginNamespace(std::ostream& ostr) {
	std::string ns = _page.getAttribute<std::string>("page.namespace", "");
	if (!ns.empty()) {
		std::vector<std::string> v;
		split(ns, ':', v);
		for (auto& n : v) {
			ostr << "namespace " << n << " {\n";
		}
		ostr << "\n\n";
	}
}

void
CodeWriter::endNamespace(std::ostream& ostr) {
	std::string ns = _page.getAttribute<std::string>("page.namespace", "");
	if (!ns.empty()) {
		ostr << "\n\n";
		std::vector<std::string> v;
		split(ns, ':', v);
		for (auto& n : v) {
			ostr << "} ";
		}
		ostr << "// namespace " << ns << "\n";
	}
}


void
CodeWriter::beginGuard(std::ostream& ostr, const std::string& headerFileName) {
	std::string guard = replaceAll(basename(headerFileName), ".", "_"); // -
	guard += "_INCLUDED";

	ostr << "#ifndef " << guard << "\n";
	ostr << "#define " << guard << "\n";
	ostr << "\n\n";
}


void
CodeWriter::endGuard(std::ostream& ostr, const std::string& headerFileName) {
	std::string guard = replaceAll(basename(headerFileName), ".", "_"); // -
	guard += "_INCLUDED";
	ostr << "\n\n";
	ostr << "#endif // " << guard << "\n";
}


void
CodeWriter::handlerClass(std::ostream& ostr, const std::string& base, const std::string& ctorArg) {
	std::string exprt(_page.getAttribute<std::string>("page.export", ""));
	if (!exprt.empty()) {
		exprt += ' ';
	}

	ostr << "class " << exprt << _class << ": public " << base << " {\n";
	ostr << "public:\n";
	if (!ctorArg.empty()) {
		ostr << "\t" << _class << "(" << ctorArg << ");\n";
		ostr << "\n";
	}
	ostr << "\tvoid handleRequest(std::shared_ptr<fastcgi::HttpRequest> httpReq, std::shared_ptr<fastcgi::HttpResponse> httpResp) override;\n";

	writeHandlerMembers(ostr);

	std::string path = _page.getAttribute<std::string>("page.path", "");
	if (!path.empty()) {
		ostr << "\n\tstatic const std::string PATH;\n";
	}

	ostr << "};\n";
}

void
CodeWriter::writeHeaderIncludes(std::ostream& ostr) {
	ostr << "#include \"fastcgi3/component.h\"\n";
	ostr << "#include \"fastcgi3/http_request.h\"\n";
	ostr << "#include \"fastcgi3/http_response.h\"\n";
	ostr << "#include \"fastcgi3/http_servlet.h\"\n";
}


void
CodeWriter::writeHandlerClass(std::ostream& ostr) {
	std::string base(_page.getAttribute<std::string>("page.baseClass", "fastcgi::Servlet"));
	std::string ctorArg(_page.getAttribute<std::string>("page.ctorArg", "std::shared_ptr<fastcgi::ComponentContext> context"));
	handlerClass(ostr, base, ctorArg);
}


void
CodeWriter::writeHandlerMembers(std::ostream& ostr) {
	std::string decls(_page.methods().str());
	if (!decls.empty()) {
		ostr << "\n";
		ostr << "private:\n";
		ostr << decls << "\n\n";
	}
}

void
CodeWriter::writeImplIncludes(std::ostream& ostr) {
	ostr << "#include <iostream>\n";
	ostr << "#include <stdexcept>\n";
	ostr << "#include \"fastcgi3/logger.h\"\n";
	ostr << "#include \"fastcgi3/config.h\"\n";
	ostr << "#include \"fastcgi3/stream.h\"\n";
	ostr << "#include \"fastcgi3/handler.h\"\n";
	ostr << "#include \"fastcgi3/request.h\"\n";
	ostr << "#include \"fastcgi3/component.h\"\n";;
	ostr << "#include \"fastcgi3/component_factory.h\"\n";
	ostr << "#include \"fastcgi3/http_request.h\"\n";
	ostr << "#include \"fastcgi3/http_response.h\"\n";
	ostr << "#include \"fastcgi3/http_servlet.h\"\n";
}

void
CodeWriter::writeConstructor(std::ostream& ostr) {
	std::string base(_page.getAttribute<std::string>("page.baseClass", "fastcgi::Servlet"));
	std::string ctorArg(_page.getAttribute<std::string>("page.ctorArg", "std::shared_ptr<fastcgi::ComponentContext> context"));

	ostr << _class << "::" << _class << "(" << ctorArg << "):\n";
	ostr << "\t " << base << "(context) {\n";
	ostr << "}\n";
	ostr << "\n\n";
}

void
CodeWriter::writeHandler(std::ostream& ostr) {
	ostr << "void\n" << _class << "::handleRequest(std::shared_ptr<fastcgi::HttpRequest> request, std::shared_ptr<fastcgi::HttpResponse> response) {\n";
	writeResponse(ostr);
	if (_page.hasAttribute("page.precondition")) {
		ostr << "\tif (!(" << _page.getAttribute<std::string>("page.precondition") << ")) return;\n\n";
	}
	ostr << _page.preHandler().str();
	writeContent(ostr);
	ostr << "}\n";
}

void
CodeWriter::writeFactory(std::ostream& istr, std::ostream& ostr, const std::string& headerFileName) {
	std::string componentName(_page.getAttribute<std::string>("component.name", _class));
	if (!componentName.empty()) {
		istr << "#include \"" << headerFileName << "\"\n";

		beginNamespace(ostr);
		ostr << "FCGIDAEMON_ADD_DEFAULT_FACTORY(\"" << componentName << "\", "<< _class << ")\n";
		endNamespace(ostr);
	}
}

void
CodeWriter::writeResponse(std::ostream& ostr) {
	std::string contentType(_page.getAttribute<std::string>("page.contentType", "text/html"));
	std::string contentLang(_page.getAttribute<std::string>("page.contentLanguage", ""));

	ostr << "\tresponse->setContentType(\"" << contentType << "\");\n";
	if (!contentLang.empty()) {
		ostr << "\tif (request->hasHeader(\"Accept-Language\")) {\n"
			 << "\t\tresponse->setHeader(\"Content-Language\", \"" << contentLang << "\");\n"
			 << "\t}";
	}

	ostr << "\n";
}

void
CodeWriter::writeContent(std::ostream& ostr) {
	ostr << "\tfastcgi::HttpResponseStream responseStream(response);\n";
	ostr << std::regex_replace(_page.handler().str(), _removeEmptyString, "");
}






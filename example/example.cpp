#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "fastcgi3/logger.h"
#include "fastcgi3/config.h"
#include "fastcgi3/stream.h"
#include "fastcgi3/handler.h"
#include "fastcgi3/request.h"
#include "fastcgi3/component.h"
#include "fastcgi3/component_factory.h"

#include "fastcgi3/http_request.h"
#include "fastcgi3/http_response.h"
#include "fastcgi3/http_servlet.h"



#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

namespace example
{

class ExampleHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
public:
	ExampleHandler(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~ExampleHandler();

	virtual void onLoad();
	virtual void onUnload();

	virtual void handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext);

private:
	std::shared_ptr<fastcgi::Logger> logger_;
};

class ExampleHandler2 : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
public:
    ExampleHandler2(std::shared_ptr<fastcgi::ComponentContext> context);
    virtual ~ExampleHandler2();

    virtual void onLoad();
    virtual void onUnload();

    virtual void handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext);
};

class ExampleServlet : virtual public fastcgi::Servlet
{
public:
	ExampleServlet(std::shared_ptr<fastcgi::ComponentContext> context);
	virtual ~ExampleServlet();

	void doGet(fastcgi::HttpRequest* httpReq, fastcgi::HttpResponse* httpResp);
};


class ExampleFilter : virtual public fastcgi::Filter, virtual public fastcgi::Component
{
public:
	ExampleFilter(std::shared_ptr<fastcgi::ComponentContext> context);
    virtual ~ExampleFilter();

    virtual void onLoad();
    virtual void onUnload();

	void doFilter(fastcgi::Request *req, fastcgi::HandlerContext *context, std::function<void(fastcgi::Request *req, fastcgi::HandlerContext *context)> next) override;
};






ExampleHandler::ExampleHandler(std::shared_ptr<fastcgi::ComponentContext> context) : fastcgi::Component(context), logger_(nullptr) {
}

ExampleHandler::~ExampleHandler() {
}

void
ExampleHandler::onLoad() {	
	std::cout << "onLoad handler1 executed" << std::endl;
	const std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
	logger_ = context()->findComponent<fastcgi::Logger>(loggerComponentName);
	if (!logger_) {
		throw std::runtime_error("cannot get component " + loggerComponentName);
	}
}

void 
ExampleHandler::onUnload() {
	std::cout << "onUnload handler1 executed" << std::endl;
}

void
ExampleHandler::handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext) {

printf("1\n");

	std::any param = handlerContext->getParam("testParam");
	if (param.has_value()) {
		std::cout << "testParam not found" << std::endl;
	} else {
		try {
			std::cout << "testParam = " << std::any_cast<std::string>(param) << std::endl;
		} catch (const std::bad_any_cast &) {
			std::cout << "bad_any_cast: testParam is not string" << std::endl;
		}
	}

printf("2\n");

//	req->setContentType("text/plain");
//	req->setContentType("text/html");
	fastcgi::RequestStream stream(req);

printf("3\n");

	std::vector<std::string> names;
	req->argNames(names);
	for (auto& i : names) {
		stream << "arg " << i << " has value " << req->getArg(i) << "\n";
	}
	req->headerNames(names);
	for (auto& i : names) {
		stream << "header " << i << " has value " << req->getHeader(i) << "\n";
	}
	req->cookieNames(names);
	for (auto& i : names) {
		stream << "cookie " << i << " has value " << req->getCookie(i) << "\n";
	}

printf("4\n");

	stream << "Remote User: " << req->getRemoteUser() << "\n";

printf("5\n");

	stream << "ExampleHandler: test ok 222\n";

printf("6\n");

	if (req->getSession()) {
printf("7\n");
		stream << "session_id=" << req->getSession()->getId() << "\n";

printf("8\n");

		if (req->hasArg("a1")) {
			req->getSession()->setAttribute<std::string>("a1", req->getArg("a1"));
		}
printf("9\n");

		if (req->getSession()->hasAttribute("a1")) {
			stream << "session attribute a1=" << req->getSession()->getAttribute<std::string>("a1");
		}
	} else {
		stream << "session is not supported in current configuration\n";
	}

printf("7\n");

//	logger_->info("request processed");

	std::string html_source =
R"html(
<table border="1">
	<tr><td>1</td><td>2</td></tr>
	<tr><td>a</td><td>b</td></tr>
</table>
)html";

	stream << html_source;

printf("6\n");

	req->setStatus(200);

printf("7\n");

	handlerContext->setParam("param1", std::string("hi!"));
}

ExampleHandler2::ExampleHandler2(std::shared_ptr<fastcgi::ComponentContext> context) : fastcgi::Component(context) {
}

ExampleHandler2::~ExampleHandler2() {
}

void
ExampleHandler2::onLoad() {  
	std::cout << "onLoad handler2 executed" << std::endl;
}

void
ExampleHandler2::onUnload() {
	std::cout << "onUnload handler2 executed" << std::endl;
}

void
ExampleHandler2::handleRequest(fastcgi::Request *req, fastcgi::HandlerContext *handlerContext) {
	std::any param1 = handlerContext->getParam("param1");
	std::any param2 = handlerContext->getParam("param2");

	fastcgi::RequestStream stream(req);
	stream << "ExampleHandler2: OK\n";


//	if (param1.empty()) {
//		std::cout << "param1 not found" << std::endl;
//	} else {
//		try {
//			std::cout << "value of param1 = " << core::any_cast<std::string>(param1) << std::endl;
//		} catch (const core::bad_any_cast &) {
//			std::cout << "bad_any_cast: param1 is not string" << std::endl;
//		}
//	}
//
//	if (param2.empty()) {
//		std::cout << "param2 not found" << std::endl;
//	} else {
//		std::cout << "param1 found" << std::endl;
//	}

	if (req->getScriptName() == "/upload" && req->getRequestMethod() == "POST") {
		fastcgi::DataBuffer f = req->remoteFile("file");
		std::string file;
		f.toString(file);
		std::cout << "file=\n" << file << std::endl;
	}
}



ExampleServlet::ExampleServlet(std::shared_ptr<fastcgi::ComponentContext> context) :
	fastcgi::Servlet(context) {
	;
}

ExampleServlet::~ExampleServlet() {
	;
}


void
ExampleServlet::doGet(fastcgi::HttpRequest* httpReq, fastcgi::HttpResponse* httpResp) {

printf("Handle example servlet\n");

	fastcgi::HttpResponseStream stream(httpResp);

	stream << "\ntest ok 33 (servlet)\n";

//	httpResp->includeComponent("example");
	httpResp->includePath("/test");

}



ExampleFilter::ExampleFilter(std::shared_ptr<fastcgi::ComponentContext> context) : fastcgi::Component(context) {

}

ExampleFilter::~ExampleFilter() {
	;
}

void ExampleFilter::onLoad() {
	;
}

void ExampleFilter::onUnload() {
	;
}

void ExampleFilter::doFilter(fastcgi::Request *req, fastcgi::HandlerContext *context, std::function<void(fastcgi::Request *req, fastcgi::HandlerContext *context)> next) {

	fastcgi::RequestStream stream(req);

//	stream << "\nFilter : 1\n";

	printf("Filter: ScriptName=%s | QueryString=%s | PathInfo=%s | Url=%s | URI=%s | UserAgent=%s\n",
			req->getScriptName().c_str(),
			req->getQueryString().c_str(),
			req->getPathInfo().c_str(),
			req->getUrl().c_str(),
			req->getURI().c_str(),
			req->getHeader("USER-AGENT").c_str());


	next(req, context);

try {
	req->setHeader("X-TestHeader", "TestValue");
} catch (const std::exception &e) {
	printf("Exception %s\n", e.what());
} catch (...) {
	printf("Exception\n");
}

	printf("Filter: 2\n");

//	stream << "\nFilter : 2\n";
}




FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("example", ExampleHandler)
FCGIDAEMON_ADD_DEFAULT_FACTORY("example2", ExampleHandler2)
FCGIDAEMON_ADD_DEFAULT_FACTORY("example3", ExampleServlet)
FCGIDAEMON_ADD_DEFAULT_FACTORY("filter1", ExampleFilter)
FCGIDAEMON_REGISTER_FACTORIES_END()


} // namespace example

# Fastcgi Container

Fastcgi Container is a framework for development of high performance web applications in C++.

Fastcgi Container is a branch of Yandex's [Fastcgi Daemon](https://github.com/lmovsesjan/Fastcgi-Daemon).

What's new compared to Fastcgi Daemon:

* The framework is written in C++17 and does not depend on Boost libraries anymore
* Support for request filters
* Support for servlets as an extensions of request handlers
* Support for sessions
* Support for authentication and authorization
* Support for security contexts   
* The framework provides Page Compiler - a command-line C++ server page compiler which generates C++ servlets from [JSP-like](http://en.wikipedia.org/wiki/JavaServer_Pages) source files
* The framework is using CMake as a build system  

All filters (including authentication/authorization filter) are executed under the FastCGI role "RESPONDER" and do not require that the roles "FILTER" and/or "AUTHORIZER" are supported by FastCGI connector (e.g. corresponding modules for Apache HTTPD).

Session manager and security facility (support for authentication and authorization) are implemented as an optional pluggable modules: you may load them to your application and use it, or run the application without these features. 

Note that form-based authentication depends on session management: if you decide to use form-based authentication, you have to activate the session management as well.  

# License 

[LGPL v.3.0](LICENSE), except Page Compiler which is covered by [GPL v.3.0](page-compiler/LICENSE).

# Requirements

* A C++17 compliant compiler with complete support for C++17 regex (e.g., GCC 4.9 meets the minimum feature set required to build the package)
* CMake build system
* Currently the framework can be built on Linux only

# Dependencies (libraries && headers )

* libfcgi
* libfcgi++
* libxml2
* libcrypto
* libuuid
* libdl

# Build


To build the project you will need to have CMake installed appropriately in your system. 

Inside the project directory, execute the following commands to generate Makefiles, build, and install the Fastcgi Container:

* avoid run cmake on project root

    mkdir binout
    cd binout

	cmake ..
	make
	sudo make install 

In default configuration, the package will be installed with prefix `/usr`. If you want to change the prefix, you may specify it as follows:

	cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/local .
	make
	sudo make install 

or

	cmake -DCMAKE_INSTALL_PREFIX:PATH=~/bin/fastcgi-container .
	make
	make install go

For more information about CMake options see the documentation available on official [CMake site](http://www.cmake.org/documentation/).

After installation of the package in system directories `/usr` or `/usr/local`, create the necessary links and cache for use by the run-time linker:

	sudo ldconfig   

# Docs

* [API](docs/API.md) (to be done)
* [C++ Page Compiler](page-compiler/docs/page_compiler.md)

# Examples

The following examples are provided:

* Simple request handler
* Simple request filter
* Simple servlet
* Configuration for form authentication filter
* Simple athorization realm

## Build

All examples except C++ server page are built together with the Fastcgi Container. 
To re-build them separately, execute the following commands:

	cd ./example/pages
	make

## Configuring examples to run with HTTPD server 

Copy built shared libraries from `example/.libs` as well as configuration file `example/fastcgi.conf` into working directory (e.g. `~/tmp/fscgi`). Modify configuration file as appropriate.

Configure available HTTPD server to connect with Fastcgi Container. 

### Apache 2 HTTPD

For example, to run examples via Unix socket with Apache2 HTTPD using modules `mod_poxy` and `mod_proxy_fcgi`, add the following entries into Apache configuration file:

	ProxyPass /myapp/ unix:///tmp/fastcgi3-container-example.sock|fcgi://localhost/
 
To run examples via tcp socket, add the following entries into Apache configuration file:

	ProxyPass /myapp/ fcgi://localhost:8080/

If you want to run examples in clustered environment, enable the Apache module `mod_proxy_balancer` and add the following entries into Apache configuration:

	ProxyPass /myapp/ balancer://myappcluster/ stickysession=JSESSIONID|jsessionid
	<Proxy balancer://myappcluster/>
    	BalancerMember fcgi://localhost:8080
    	BalancerMember fcgi://localhost:8081
	</Proxy>

Note that you will need to configure two or more instances of Fastcgi Container either on different hosts or on the same host with different ports.

For more information, see Apache documentation:
 
* [mod_proxy](http://httpd.apache.org/docs/2.4/mod/mod_proxy.html)
* [mod_proxy_fcgi](http://httpd.apache.org/docs/2.4/mod/mod_proxy_fcgi.html)
* [mod_proxy_balancer](http://httpd.apache.org/docs/2.4/mod/mod_proxy_balancer.html)

### NGINX

To run examples via Unix socket with NGINX server using module `ngx_http_fastcgi_module`, add the following entries into NGINX configuration file:

	location /myapp/ {
		fastcgi_pass unix:/tmp/fastcgi3-container-example.sock;
		include      fastcgi_params;
	}

You may also run examples in clustered environment.

For more information, see NGINX doumentation:

* [fastcgi_pass](http://nginx.org/en/docs/http/ngx_http_fastcgi_module.html?#fastcgi_pass) 
* [load balancer](http://nginx.org/en/docs/http/load_balancing.html)
 
## Running examples

Start HTTPD server as appropriate (e.g. `sudo /etc/init.d/apache start`).

Start Fastcgi Container with exmple configuration (here the container is started under the current user; in production start it as appropriate):

	cd ~/tmp/fscgi
	fastcgi3-domain --config=./fastcgi.conf
 
Open any web browser and type the following address in URL bar:

	http://127.0.0.1/myapp/servlet 

or 
	
	http://127.0.0.1/myapp/myapp/test?a1=22

 
	

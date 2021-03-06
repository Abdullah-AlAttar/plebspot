#include <httplib.h>
#include <filesystem>
#include <fmt/core.h>

#include <string>
#include <sstream>

#include "fs.h"
#include "md.h"
#include "render.h"
#include "config.h"

using namespace std;
using namespace httplib;

extern const char* plain_html_tmpl;

void init(){
	filesystem::create_directory(PAGES_DIR);
	filesystem::create_directory(POSTS_DIR);
	filesystem::create_directory(STATIC_DIR);
	filesystem::create_directory(TEMPLATES_DIR);
	string plain_tmpl_path = string(TEMPLATES_DIR) + "/" + config::html_tmpl;
	if(!filesystem::exists(plain_tmpl_path)){
		ofstream plain_tmpl(plain_tmpl_path);
		plain_tmpl << plain_html_tmpl;
	}
	if(!filesystem::exists(CONFIG_FILE)){
		ofstream config_stream(CONFIG_FILE);
		config_stream <<
			"blog_title: " << config::blog_title << "\n"
			"blog_desc:  " << config::blog_desc << "\n"
			"http_port:  " << config::http_port << "\n"
			"html_tmpl:  " << config::html_tmpl << "\n";
	}
}

void serve(){
	config::load();
	Server svr;
	svr.Get("/", [](const Request& req, Response& res) {
		res.set_content(render::render_home_page(), "text/html");
	});

	svr.Get(R"(/pages/(([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		auto pagePath = req.matches[1];
		res.set_content(render::render_post(string("./pages/") + pagePath.str()), "text/html");
	});

	svr.Get(R"(/posts/(([a-zA-Z0-9_\-\.]+/)*[a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		auto postPath = req.matches[1];
		res.set_content(render::render_post(string("./posts/") + postPath.str()), "text/html");
	});

	svr.set_error_handler([](const Request& req, Response& res) {
		fmt::print("error while serving url {}\n", req.path);
	});

	svr.Get(R"(/static/([a-zA-Z0-9_\-\.]+))", [&](const Request& req, Response& res) {
		string target_path = string("./static/") + req.matches[1].str();
		fmt::print("requesting static uri {}\n", target_path);
		if(filesystem::exists(target_path)){
			res.set_content(fs::get_file_contents(target_path.c_str()), "");
			return res.status = 200;
		} else {
			return res.status = 404;
		}
	});

	const char* ip = "0.0.0.0";
	fmt::print("plebspot is listening on {}:{}\n", ip, config::http_port);
	svr.listen(ip, config::http_port);
}

void help(){
	cout << "usage:\n"
		"\tplebspot init\n"
		"\t\tcreate sample files and folder needed for operation\n"
		"\tplebspot serve\n"
		"\t\tserves plebspot http application on port 1993 or port configured in pleb.yml\n";
}

int main(int argc, char const *argv[])
{
	if(argc == 2 && strcmp(argv[1], "init") == 0){
		init();
	} else if(argc == 2 && strcmp(argv[1], "serve") == 0){
		serve();
	} else {
		help();
	}
}

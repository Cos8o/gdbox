#include "httplib.h"
#include "GeometryDash.hpp"

#include <sstream>
#include <filesystem>

using namespace httplib;
using namespace GeometryDash;
namespace stdfs = std::filesystem;

__declspec(dllexport) void extfunc() {}

std::string getGreetingMessage(int port)
{
	if (port == 42069)
		return std::string("Server running on <cg>port 42069</c> <cy>(lol funi number xdxd)</c>.");

	return std::string("Server running on <cg>port ") + std::to_string(port) + "</c>.";
}

std::vector<std::string> tokenize(std::string const& s)
{
	std::vector<std::string> ret;
	std::stringstream ss(s);
	std::string buf;

	while (std::getline(ss, buf, '/'))
		ret.push_back(buf);

	return ret;
}

void handleGetRequest(Request const& req, Response& res)
{
	auto tokens = tokenize(req.body);

	if (tokens.size() < 2 || tokens.size() > 4)
	{
		res.status = 400;
		res.body = "Invalid request.";
	}
	else
	{
		res.status = 200;
		res.body = "Cos8o";



		switch (tokens.size())
		{
		case 2:
			GeometryDash::showMessageBox(tokens[0], tokens[1]);
			break;
		case 3:
			GeometryDash::showMessageBox(tokens[0], tokens[1], tokens[2]);
			break;
		case 4:
			GeometryDash::showMessageBox(tokens[0], tokens[1], tokens[2], tokens[3]);
			break;
		}
	}
}

void handlePostRequest(Request const& req, Response& res)
{
	auto paramsSize = req.params.size();

	if (paramsSize < 2 || paramsSize > 4)
	{
		res.status = 400;
		res.body = "Invalid request.";
	}
	else
	{
		auto title = req.get_param_value("title");
		auto text = req.get_param_value("text");
		auto button1 = req.get_param_value("button1");
		auto button2 = req.get_param_value("button2");

		GeometryDash::showMessageBox(
			title.empty() ? "Title" : title,
			text.empty() ? "Text" : text,
			button1.empty() ? "Ok" : button1,
			button2);
	}
}

void mainThread()
{
	int port = 42069;
	httplib::Server server;

	//Init server

	server.Get("/msgbox", [](const Request& req, Response& res)
		{
			handleGetRequest(req, res);
		});

	server.Post("/msgbox", [](Request const& req, Response& res)
		{
			handlePostRequest(req, res);
		});

	server.Get("/form", [](const Request& req, Response& res)
		{

		});

	server.Get("/stop", [&](const Request&, Response&)
		{
			server.stop();
			ExitProcess(0);
		});

	if (!stdfs::exists("colonmode.txt"))
	{
		port = server.bind_to_any_port("127.0.0.1");
		GeometryDash::showMessageBox("DLL INJECTED", getGreetingMessage(port));
		server.listen_after_bind();
	}
	else
	{
		GeometryDash::showMessageBox("DLL INJECTED", getGreetingMessage(port));
		server.listen("127.0.0.1", port);
	}
}

void __fastcall menuCallback(void* pthis)
{
	static bool loaded = false;

	GeometryDash::showMenu(pthis);

	if (!loaded)
	{
		CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&mainThread), 0, 0, 0);
		loaded = true;
	}
}

BOOL WINAPI DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
		GeometryDash::init(menuCallback);

	return TRUE;
}
#include <iostream>
#include <windows.h>
#include <string>
#include <regex>

struct SSHInfo {
	std::string host;
	int port;
	std::string user;
	std::string password;
};

SSHInfo parse_ssh_info(const std::string& ssh_param) {
	SSHInfo info;
	std::regex pattern("ssh://([^:]+):([^@]+)@([^:]+):(\\d+)");
	std::smatch matches;

	if (std::regex_match(ssh_param, matches, pattern)) {
		info.user = matches[1];
		info.password = matches[2];
		info.host = matches[3];
		info.port = std::stoi(matches[4]);
	}

	return info;
}

int main(int argc, char* argv[]) {
	if (argc >= 5) {
		std::string ssh_param = argv[4];
		SSHInfo info = parse_ssh_info(ssh_param);

		std::cout << "Host: " << info.host << std::endl;
		std::cout << "Port: " << info.port << std::endl;
		std::cout << "User: " << info.user << std::endl;
		std::cout << "Password: " << info.password << std::endl;

		std::string program_name = argv[0];
		std::string modified_program_name = program_name.insert(program_name.find_last_of("."), "_");
		std::string command_line;
		if (program_name.find("Xshell.exe") != -1)
		{
			command_line = modified_program_name + " -newtab " + std::string(argv[2]) + " -url " + std::string(argv[4]);
		}
		else{
			command_line = "\"" + modified_program_name + "\" -newtab " + "\"" + " ssh -L 18080:localhost:8080 -L 13306:localhost:3306 " + info.user + "@" + info.host + " -p " + std::to_string(info.port) + "\"";
		}

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// Start the child process.
		if (!CreateProcess(NULL,   // No module name (use command line)
			const_cast<char*>(command_line.c_str()),        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)            // Pointer to PROCESS_INFORMATION structure
			) {
			std::cerr << "CreateProcess failed (" << GetLastError() << ")." << std::endl;
			return 1;
		}

		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else {
		std::cerr << "Usage: program -newtab <host> -url <ssh_info>" << std::endl;
		return 1;
	}

	return 0;
}

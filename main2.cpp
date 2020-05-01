#include"control_node.h"
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<Windows.h>

int main(int argc, char* argv[]) {
	ControlNode a(std::atoi(argv[1]), std::atoi(argv[2]), std::atoi(argv[3]), std::atoi(argv[4]), std::atoi(argv[5]));
	bool is_exit = false;
	std::string str;
	std::vector<std::string> str1;
	std::cout << a.GetId() << std::endl;
	while (!a.is_exit) {
		if (a.GetMsgFromPar(str)) {
			str1 = split(str);
			for (int i = 0; i < str1.size(); ++i) {
				std::cout << str << std::endl;
				if (str1[i] == "create") {
					a.Create(std::stoi(str1[i+1]), std::stoi(str1[i+2]), std::stoi(str1[i+3]));
					i += 3;
					continue;
				}
				if (str1[i] == "exit") {
					is_exit = true;
					a.Exit();
					break;
				}
				if (str1[i] == "remove") {
					a.Remove(std::stoi(str1[i+1]));
					i += 1;
				}
				if (str1[i] == "close") {
					a.Close(std::stoi(str1[i+1]), std::stoi(str1[i+2]));
					i += 2;
					is_exit = true;
					break;
				}
				if (str1[i] == "reconnect") {
					a.ReConnectPull(std::stoi(str1[i+1]));
					a.ReConnectPush1(std::stoi(str1[i+2]));
					a.SendToPar("newchild " + std::to_string(a.GetId()));
					i += 2;
				}
				if (str1[i] == "exec") {
					a.Exec(std::stoi(str1[i+1]), str1[i+2]);
					i += 2;
				}
				if (str1[i] == "hearbit") {
					if (str1[i+1] == "stop") {
						a.HearBitStop();
						i += 1;
					}
					else {
						a.HearBit(std::stoll(str1[i+1]));
						i += 1;
					}
				}
			}
			if (is_exit) {
				break;
			}
		}
	}
	return 0;
}
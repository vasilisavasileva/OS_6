#include<zmq.h>
#include<Windows.h>
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<thread>
#include"control_node.h"

int main() {
	ControlNode node;
	bool is_exit = false;
	std::string str;
	std::vector<std::string> str1;
	int port = 4041;
	while (true) {
		std::getline(std::cin, str);
		str1 = split(str);
		for (int i = 0; i < str1.size(); ++i) {
			if (str1[i] == "create") {
				node.Create(std::stoi(str1[i+1]), port + 1, port + 2);
				port += 2;
				i += 1;
				continue;
			}
			if (str1[i] == "exit") {
				node.Exit();
				is_exit = true;
				break;
			}
			if (str1[i] == "remove") {
				node.Remove(std::stoi(str1[i+1]));
				i += 1;
			}
			if (str1[i] == "exec") {
				node.Exec(std::stoi(str1[i+1]), str1[i+2]);
				i += 2;
			}
			if (str1[i] == "hearbit") {
				if (str1[i+1] == "stop") {
					node.HearBitStop();
				}
				else {
					node.HearBit(std::stoll(str1[i+1]));
				}
				std::cin.get();
				std::cout << "Hearbit: Stop\n";
				node.HearBitStop();
				i += 1;
			}
			//считать сообщение
		}
		if (is_exit) {
			break;
		}
	}
	return 0;
}
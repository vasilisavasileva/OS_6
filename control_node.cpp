#include<zmq.h>
#include<Windows.h>
#include<string>
#include<iostream>
#include<algorithm>
#include<vector>
#include"control_node.h"


inline bool space(char c) {
	return std::isspace(c);
}

inline bool notspace(char c) {
	return !std::isspace(c);
}

void DeleteInform(std::string& str) {
	int start = str.find("newchild ");
	int end = start + std::string("newchild ").size();
	for (end; end < str.size(); ++end) {
		if (str[end] == ' ')
			break;
	}	if (start == 0) {
		str = str.substr(end, std::string::npos);
	}
	else {
		str = str.substr(0, start - 1) + str.substr(end, std::string::npos);
	}
}

std::vector<std::string> split(const std::string& s) {
	typedef std::string::const_iterator iter;
	std::vector<std::string> ret;
	iter i = s.begin();
	while (i != s.end()) {
		i = std::find_if(i, s.end(), notspace); // find the beginning of a word
		iter j = std::find_if(i, s.end(), space); // find the end of the same word
		if (i != s.end()) {
			ret.push_back(std::string(i, j)); //insert the word into vector
			i = j; // repeat 1,2,3 on the rest of the line.
		}
	}
	return ret;
}

ControlNode::ControlNode() {
	id = 0;
	port_pull = -1;
	port_push = 4040;
	port_push1 = -1;
	port_pull1 = 4041;
	Context_Push = zmq_ctx_new();
	Context_Pull = zmq_ctx_new();
	Socket_Push = zmq_socket(Context_Push, ZMQ_PUSH);
	Socket_Pull1 = zmq_socket(Context_Pull, ZMQ_PULL);
	//std::cout << "Id : " << id << std::endl;
}

ControlNode::ControlNode(int id1, int port1, int port2, int port3, int port4) {
	id = id1;
	port_pull = port1;
	port_push = port2;
	port_pull1 = port3;
	port_push1 = port4;
	Context_Push = zmq_ctx_new();
	Context_Pull = zmq_ctx_new();
	Socket_Push = zmq_socket(Context_Push, ZMQ_PUSH);
	Socket_Pull = zmq_socket(Context_Push, ZMQ_PULL);
	Socket_Push1 = zmq_socket(Context_Push, ZMQ_PUSH);
	Socket_Pull1 = zmq_socket(Context_Pull, ZMQ_PULL);
	//std::cout << "Id : " << id1 << std::endl;
	Connect(port1, Socket_Pull);
	Connect(port4, Socket_Push1);

}

ControlNode::~ControlNode() {
	//zmq_close(Socket_Pull);
	//zmq_close(Socket_Push1);
}

bool ControlNode::Connect(int port1, void* s) {
	std::string adress("tcp://localhost:");
	adress += std::to_string(port1);
	if (zmq_connect(s, adress.c_str()) == 0) {
		//	std::cout << "Connected: " << adress << "\n";
		return true;
	}
	else {
		//	std::cout << "Not Connected: " << adress << "\n";
		return false;
	}
}

bool ControlNode::Bind(int port, void* s) {
	std::string adress("tcp://*:");
	adress += std::to_string(port);
	if (zmq_bind(s, adress.c_str()) == 0) {
		//	std::cout << "Binded: " << adress << "\n";
		return true;
	}
	else {
		//	std::cout << "Not binded : " << adress << "\n";
		return false;
	}
}

bool ControlNode::SendToSon(const std::string str) {
	zmq_msg_t message;
	zmq_msg_init_size(&message, str.length() + 1);
	memcpy(zmq_msg_data(&message), str.c_str(), str.length() + 1);
	int res = zmq_msg_send(&message, Socket_Push, ZMQ_DONTWAIT);
	zmq_msg_close(&message);
	if (res == -1) {
		return false;
	}
	else {
		return true;
	}
}

bool ControlNode::SendToPar(const std::string str) {
	zmq_msg_t message;
	zmq_msg_init_size(&message, str.length() + 1);
	memcpy(zmq_msg_data(&message), str.c_str(), str.length() + 1);
	int res = zmq_msg_send(&message, Socket_Push1, ZMQ_DONTWAIT);
	zmq_msg_close(&message); 
	if (res == -1) {
		return false;
	}
	else {
		return true;
	}
}

bool ControlNode::GetMsgFromSon(std::string& Buffer) {
	zmq_msg_t reply;
	zmq_msg_init(&reply);
	if (zmq_msg_recv(&reply, Socket_Pull1, ZMQ_DONTWAIT) != -1) {
		char* str = new char[zmq_msg_size(&reply)];
		memcpy(str, zmq_msg_data(&reply), zmq_msg_size(&reply));
		zmq_msg_close(&reply);
		Buffer = std::string(str);
		delete[] str;
		return true;
	}
	return false;
}

bool ControlNode::GetMsgFromPar(std::string& Buffer) {
	zmq_msg_t reply;
	zmq_msg_init(&reply);
	if (zmq_msg_recv(&reply, Socket_Pull, ZMQ_DONTWAIT) != -1) {
		char* str = new char[zmq_msg_size(&reply)];
		memcpy(str, zmq_msg_data(&reply), zmq_msg_size(&reply));
		zmq_msg_close(&reply);
		Buffer = std::string(str);
		delete[] str;
		return true;
	}
	zmq_msg_close(&reply);
	return false;
}

bool ControlNode::Create(int id, int port1, int port2) {
	if (this->id == id) {
		SendToPar("Error: Already exists\n");
		return false;
	}
	if (is_created == false) {
		Bind(port_push, Socket_Push);
		Bind(port_pull1, Socket_Pull1);
		is_created = true;
		child_id = id;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		std::string str(CREATED_PROJECT_NAME + ' ');
		str += std::to_string(id);
		str += ' ';
		str += std::to_string(port_push);
		str += ' ';
		str += std::to_string(port1);
		str += ' ';
		str += std::to_string(port2);
		str += ' ';
		str += std::to_string(port_pull1);

		char* SecondProcess = (char*)str.c_str();

		PROCESS_INFORMATION ProcessInfo;
		ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

		STARTUPINFO StartupInfo;
		ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
		StartupInfo.cb = sizeof(STARTUPINFO);


		BOOL process = CreateProcess(NULL,
			SecondProcess,
			NULL, NULL, TRUE,
			CREATE_NO_WINDOW,
			NULL, NULL,
			&StartupInfo,
			&ProcessInfo);
		if (!process) {
			return false;
		}
		if (port_pull == -1) {
			std::cout << "OK: " << ProcessInfo.dwProcessId << std::endl;
		}
		else {
			SendToPar("OK: " + std::to_string(ProcessInfo.dwProcessId) + '\n');
		}
		if (reciver.joinable()) {
			reciver.join();
		}
		reciver = std::thread(ThreadFunction, this);
		return true;
	}
	else {
		std::string str = "create ";
		str += ' ';
		str += std::to_string(id);
		str += ' ';
		str += std::to_string(port1);
		str += ' ';
		str += std::to_string(port2);
		bool res = SendToSon(str);
		if (!res) {
			if (!SendToPar("Error: " + std::to_string(child_id) + " is unavailable\n")) {
				std::cout << "Error: " + std::to_string(child_id) + " is unavailable\n";
			}
		}
		return res;
	}
}

void ControlNode::Exit() {
	is_exit = true;
	hearbit.StopHearing();
	if (reciver.joinable()) {
		reciver.join();
	}
	if (is_created == true) {
		SendToSon("exit");
	}
}
void ThreadFunction(ControlNode* a) {
	std::string str;
	std::vector<std::string> strings;
	bool is_error_hearbit_sended = false;
	while (!a->is_exit) {
		if (a->hearbit.isHeared()) {
			if (!a->hearbit.CheckTime() && !is_error_hearbit_sended) {
				if (!a->SendToPar("Hearbit: Error: Node " + std::to_string(a->child_id) + " is unavailable\n")) {
					std::cout << "Hearbit: Error: Node " + std::to_string(a->child_id) + " is unavailable\n";
				}
				is_error_hearbit_sended = true;
			}
		}
		if (a->GetMsgFromSon(str)) {
			strings = split(str);
			bool is_exit = false;
			for (int i = 0; i < strings.size(); ++i) {
				if (strings[i] == "close") {
					a->is_created = false;
					is_exit = true;
					break;
				}
				if (strings[i] == "newchild") {
					a->child_id = std::stoi(strings[i + 1]);
					std::cout << "newchild: " << a->child_id << std::endl;
					i += 1;
					DeleteInform(str);
				}
				if (a->hearbit.isHeared()) {
					if (strings[i] == "Hearbit:") {
						if (strings[i+1] == "Ok:") {
							if (a->child_id == std::stoll(strings[i+2])) {
								a->hearbit.SetTime();
								is_error_hearbit_sended = false;
								i += 2;
							}
							else {
								i += 1;
							}
						}
					}
				}
			}
			if (is_exit) break;
			if (!a->SendToPar(str)) {
				std::cout << str;
			}
		}
	}
}

void ControlNode::Remove(int id1) {
	if (!this->is_created) {
		if (!SendToPar("Error: Node is not found!\n")) {
			std::cout << "Error: Node is not found!\n";
		}
		return;
	}
	if (this->child_id == id1) {
		if (!SendToSon("close " + std::to_string(port_push) + ' ' + std::to_string(port_pull1))) {
			if (!SendToPar("Error: Node " + std::to_string(child_id) + " is unavailable!\n")) {
				std::cout << "Error: Node " + std::to_string(child_id) + " is unavailable!\n";
			}
		}
		else {
			if (!SendToPar("Ok\n")) {
				std::cout << "Ok\n";
			}
		}
	}
	else {
		if (SendToSon("remove " + std::to_string(id1))) {			//отправляем remove id1
			return;
		}
		else {
			if (!SendToPar("Error: Node "+ std::to_string(child_id)+" is unavailable!\n")) {
				std::cout << "Error: Node " + std::to_string(child_id) + " is unavailable!\n";
			}
			return;
		}
	}
}

void ControlNode::Close() {
	is_exit = true;
	if (reciver.joinable()) {
		reciver.join();
	}
}

void ControlNode::Close(int p1, int p2) {
	if (isCreated()) {
		SendToSon("reconnect " + std::to_string(p1) + ' ' + std::to_string(p2));
	}
	else {
		SendToPar("close");
	}
	Close();
}

bool ControlNode::ReConnectPull(int port1) {
	std::string adress("tcp://localhost:");
	adress += std::to_string(port_pull);
	zmq_disconnect(Socket_Pull, adress.c_str());
	adress = "tcp://localhost:";
	adress += std::to_string(port1);
	if (zmq_connect(Socket_Pull, adress.c_str()) == 0) {
		return true;
	}
	else {
		return false;
	}
}

bool ControlNode::ReConnectPush1(int port1) {
	std::string adress("tcp://localhost:");
	adress += std::to_string(port_push1);
	zmq_disconnect(Socket_Push1, adress.c_str());
	adress = "tcp://localhost:";
	adress += std::to_string(port1);
	if (zmq_connect(Socket_Push1, adress.c_str()) == 0) {
		//	std::cout << "Connected: " << adress << "\n";
		return true;
	}
	else {
		//	std::cout << "Not Connected: " << adress << "\n";
		return false;
	}
}

int ControlNode::GetId() {
	return id ;
}

bool ControlNode::isCreated() {
	return is_created;
}

void ControlNode::Exec(int id1, std::string command) {
	if (id1 == GetId()) {
		if (command == "time") {
			SendToPar("Ok:" + std::to_string(GetId()) + ": " + std::to_string(timer.Time()) + "\n");
		}
		else if (command == "start") {
			timer.Start();
			SendToPar("Ok:" + std::to_string(GetId()) + '\n');
		}
		else if (command == "stop") {
			timer.Stop();
			SendToPar("Ok:" + std::to_string(GetId()) + '\n');
		}
	}
	else {
		if (is_created) {
			if (!SendToSon("exec " + std::to_string(id1) + ' ' + command)) {
				if (!SendToPar("Error: " + std::to_string(child_id) + " is unavailable\n")) {
					std::cout << "Error: " + std::to_string(child_id) + " is unavailable\n";
				}
			}
		}
		else {
			if (!SendToPar("Error: Node is not found!\n")) {
				std::cout << "Error: Node is not found!\n";
			}
		}
	}
}

void ControlNode::HearBit(int count) {
	hearbit.StartHearing(count, this);
	if (is_created) {
		SendToSon("hearbit " + std::to_string(count));
	}
}

void ControlNode::HearBitStop() {
	hearbit.StopHearing();
	if (is_created) {
		SendToSon("hearbit stop");
	}
}
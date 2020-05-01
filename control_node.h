#pragma once
#include<thread>
#include<string>
#include<vector>
#include<iostream>
#include"timer.h"
const std::string CREATED_PROJECT_NAME = "OS_6_1";
const int MIN_HEAR_TIME = 1000;
class ControlNode {
	class Hearbit {
		bool is_heared = false;
		long long hear_time = 0;
		Timer timer;
		std::thread listner;
		void StartHearing(ControlNode* a);
		std::thread sender;
	public:
		bool isHeared();
		long long GetHearTime();
		void Hear();
		void SetTime();
		void StartHearing(long long time, ControlNode* a);
		void StopHearing();
		void SetHearTime(long long time);
		bool CheckTime();
		static void SenderFunction(ControlNode* a, Hearbit* b);
	};
	int id = -1;
	int pid = -1;
	int port_pull = -1;
	int port_push = -1;
	int port_pull1 = -1;
	int port_push1 = -1;
	int child_id = -1;
	void* Context_Push = nullptr;
	void* Context_Pull = nullptr;
	void* Context_Pull1 = nullptr;
	void* Context_Push1 = nullptr;
	void* Socket_Pull = nullptr;
	void* Socket_Push = nullptr;
	void* Socket_Pull1 = nullptr;
	void* Socket_Push1 = nullptr;
	bool is_created = false;
	std::thread reciver;
	Timer timer;
	Hearbit hearbit;
public:
	bool is_exit = false;
	ControlNode();
	~ControlNode();
	ControlNode(int id, int port1, int port2, int port3, int port4);
	bool SendToSon(const std::string str);//отправляет строку сыну
	bool SendToPar(const std::string str);//отправляет строку родителю
	bool GetMsgFromSon(std::string& Buffer);//получает строку от сына; неблокирующая функция
	bool GetMsgFromPar(std::string& Buffer);//получает стркоу от родителя; неблокирующая функция
	bool Connect(int port, void* s);
	bool Bind(int port, void* s);
	bool Create(int id, int port1, int port2);
	void Exit();
	void Close();
	void Close(int p1, int p2);
	void Remove(int id1);
	bool ReConnectPull(int port);
	bool ReConnectPush1(int port);
	bool isCreated();
	int GetId();
	void Exec(int id, std::string command);
	friend void ThreadFunction(ControlNode* a);
	void HearBit(int count);
	void HearBitStop();
};
std::vector<std::string> split(const std::string& s);
void ThreadFunction(ControlNode* a);


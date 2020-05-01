#include"control_node.h"
#include<chrono>


bool ControlNode::Hearbit::isHeared() {
	return is_heared;
}

void ControlNode::Hearbit::Hear() {
	is_heared = true;
}

void ControlNode::Hearbit::StopHearing() {
	is_heared = false;
	if (sender.joinable()) {
		sender.join();
	}
}

void ControlNode::Hearbit::SetHearTime(long long t) {
	if (t < MIN_HEAR_TIME) {
		hear_time = MIN_HEAR_TIME;
	}
	else {
		hear_time = t;
	}
}

void ControlNode::Hearbit::StartHearing(long long t, ControlNode* a) {
	SetHearTime(t);
	StartHearing(a);
}

void ControlNode::Hearbit::StartHearing(ControlNode* a) {
	StopHearing();
	timer.Start();
	is_heared = true;
	sender = std::thread(SenderFunction, a, this);
}

bool ControlNode::Hearbit::CheckTime() {
	if (timer.Time() < 4 * hear_time) {
		return true;
	}
	else {
		return false;
	}
}

long long ControlNode::Hearbit::GetHearTime() {
	return hear_time;
}

void ControlNode::Hearbit::SenderFunction(ControlNode* a, ControlNode::Hearbit* b) {
	Timer timer;
	timer.Start();
	while (b->isHeared()) {
		if (timer.Time() > b->GetHearTime()) {
			a->SendToPar("Hearbit: Ok: " + std::to_string(a->GetId()) + '\n');
			timer.Start();
		}
	}
}

void ControlNode::Hearbit::SetTime() {
	timer.Start();
}
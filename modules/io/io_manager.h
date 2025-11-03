#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <queue>
#include <deque>
#include <memory>
#include <string>

enum class DeviceType {
    PRINTER,
    DISK_IO,
    NETWORK
};

enum class DeviceState {
    IDLE,
    BUSY
};

struct IORequest {
    int pid;
    DeviceType device;
    int priority;
    int arrival_time;
    int service_time;
    std::string data;
};

struct CompareIORequest {
    bool operator()(const IORequest& a, const IORequest& b) {
        return a.priority < b.priority;
    }
};

class IOManager {
private:
    std::priority_queue<IORequest, std::vector<IORequest>, CompareIORequest> request_queue;
    DeviceState printer_state;
    IORequest* current_request;
    int total_requests_served;
    int total_wait_time;

public:
    IOManager();

    void add_io_request(int pid, DeviceType device, int priority, int service_time, const std::string& data, int current_time);
    void process_io_request(int current_time);
    void print_io_status() const;
    void print_io_stats() const;

    bool is_device_busy() const { return printer_state == DeviceState::BUSY; }
    int get_pending_requests() const { return request_queue.size(); }
};

#endif
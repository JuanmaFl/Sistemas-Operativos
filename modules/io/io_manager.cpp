#include "io_manager.h"
#include <iostream>
#include <iomanip>

IOManager::IOManager()
    : printer_state(DeviceState::IDLE), current_request(nullptr),
    total_requests_served(0), total_wait_time(0) {
}

void IOManager::add_io_request(int pid, DeviceType device, int priority, int service_time, const std::string& data, int current_time) {
    IORequest req;
    req.pid = pid;
    req.device = device;
    req.priority = priority;
    req.arrival_time = current_time;
    req.service_time = service_time;
    req.data = data;

    request_queue.push(req);

    std::string device_name = (device == DeviceType::PRINTER ? "PRINTER" :
        device == DeviceType::DISK_IO ? "DISK_IO" : "NETWORK");

    std::cout << "[IO] Nueva solicitud: PID " << pid << " → " << device_name
        << " (Prioridad: " << priority << ", Cola: " << request_queue.size() << ")" << std::endl;
}

void IOManager::process_io_request(int current_time) {
    if (printer_state == DeviceState::BUSY && current_request) {
        current_request->service_time--;

        if (current_request->service_time <= 0) {
            int wait_time = current_time - current_request->arrival_time;
            total_wait_time += wait_time;
            total_requests_served++;

            std::cout << "[IO] Completado: PID " << current_request->pid
                << " (Espera: " << wait_time << " ticks)" << std::endl;

            delete current_request;
            current_request = nullptr;
            printer_state = DeviceState::IDLE;
        }
    }

    if (printer_state == DeviceState::IDLE && !request_queue.empty()) {
        IORequest next = request_queue.top();
        request_queue.pop();

        current_request = new IORequest(next);
        printer_state = DeviceState::BUSY;

        std::cout << "[IO] Atendiendo: PID " << current_request->pid
            << " - Datos: \"" << current_request->data << "\""
            << " (Tiempo: " << current_request->service_time << " ticks)" << std::endl;
    }
}

void IOManager::print_io_status() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "       ESTADO DEL SISTEMA DE E/S" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string state_str = (printer_state == DeviceState::IDLE ? "IDLE" : "BUSY");
    std::cout << "Estado Dispositivo: " << state_str << std::endl;
    std::cout << "Solicitudes Pendientes: " << request_queue.size() << std::endl;

    if (current_request) {
        std::cout << "\nTrabajo Actual:" << std::endl;
        std::cout << "  PID: " << current_request->pid << std::endl;
        std::cout << "  Tiempo Restante: " << current_request->service_time << " ticks" << std::endl;
        std::cout << "  Datos: \"" << current_request->data << "\"" << std::endl;
    }

    std::cout << "========================================\n" << std::endl;
}

void IOManager::print_io_stats() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "       ESTADÍSTICAS DE E/S" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total Solicitudes Atendidas: " << total_requests_served << std::endl;
    std::cout << "Solicitudes Pendientes: " << request_queue.size() << std::endl;

    if (total_requests_served > 0) {
        double avg_wait = static_cast<double>(total_wait_time) / total_requests_served;
        std::cout << "Tiempo de Espera Promedio: " << std::fixed << std::setprecision(2)
            << avg_wait << " ticks" << std::endl;
    }

    std::cout << "========================================\n" << std::endl;
}
#ifndef HTTPLIB_H
#define HTTPLIB_H
 
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
 
namespace httplib {
 
    struct Request {
        std::string method;
        std::string path;
        std::map<std::string, std::string> params;
 
        bool has_param(const std::string& key) const {
            return params.find(key) != params.end();
        }
        std::string get_param_value(const std::string& key) const {
            return params.at(key);
        }
    };
 
    struct Response {
        int status = 200;
        std::string body;
        void set_content(const std::string& content, const std::string& type) {
            body = content;
        }
    };
 
    class Server {
    public:
        using Handler = std::function<void(const Request&, Response&)>;
 
        void Get(const std::string& path, Handler handler) {
            handlers[path] = handler;
        }
 
        bool listen(const char* host, int port) {
            int server_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (server_fd < 0) return false;
 
            int opt = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 
            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);
 
            if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) return false;
            if (::listen(server_fd, 10) < 0) return false;
 
            std::cout << "Sunucu " << port << " portunda aktif..." << std::endl;
 
            while (true) {
                int new_socket = accept(server_fd, nullptr, nullptr);
                char buffer[2048] = {0};
                read(new_socket, buffer, 2048);
 
                // Çok basit HTTP ayrıştırıcı
                std::string raw_req(buffer);
                Request req;
                Response res;
 
                // Path ve Query string ayıklama
                size_t first_space = raw_req.find(' ');
                size_t second_space = raw_req.find(' ', first_space + 1);
                std::string full_path = raw_req.substr(first_space + 1, second_space - first_space - 1);
                size_t q_mark = full_path.find('?');
                req.path = full_path.substr(0, q_mark);
 
                if (q_mark != std::string::npos) {
                    std::string query = full_path.substr(q_mark + 1);
                    size_t eq = query.find('=');
                    req.params[query.substr(0, eq)] = query.substr(eq + 1);
                }
 
                if (handlers.count(req.path)) {
                    handlers[req.path](req, res);
                }
 
                std::string response = "HTTP/1.1 " + std::to_string(res.status) + " OK\r\n";
                response += "Content-Type: text/plain\r\n";
                response += "Content-Length: " + std::to_string(res.body.length()) + "\r\n\r\n";
                response += res.body;
 
                send(new_socket, response.c_str(), response.length(), 0);
                close(new_socket);
            }
            return true;
        }
 
    private:
        std::map<std::string, Handler> handlers;
    };
}
 
#endif
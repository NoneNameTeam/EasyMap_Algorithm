#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <functional> 

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

std::string t2s(const utility::string_t& t) {
    return utility::conversions::to_utf8string(t);
}

struct bian {
    int loc;      
    double weight;
};

struct point{
    double x, y;
};

class Astar
{
private:
    int n;
    std::vector<std::vector<bian>> graph;
    std::vector<point> points;
    std::vector<int> ans;
    std::vector<int> parents;
    std::priority_queue<std::pair<double,int>,std::vector<std::pair<double,int>>,std::greater<std::pair<double,int>>> pq;
    int start, target;
    std::vector<double> f, g;
    std::vector<bool> visited;
    int out; 

public:
    Astar() : n(0), start(0), target(0), out(0) {}

    void initialize(int node_count)
    {
        n = node_count;
        graph.clear();
        ans.clear();
        f.clear();
        g.clear();
        visited.clear();
        points.clear();
        parents.clear();
        
        std::priority_queue<std::pair<double,int>,std::vector<std::pair<double,int>>,std::greater<std::pair<double,int>>> empty_pq;
        pq.swap(empty_pq);
        
        if (n > 0)
        {
            graph.resize(n);
            f.resize(n, std::numeric_limits<double>::max());
            g.resize(n, std::numeric_limits<double>::max());
            visited.resize(n, false);
            parents.resize(n, -1);
            points.resize(n, {0,0});
            out = 0;
        }
    }

    double safe_get_double(const json::value& v) {
        if (v.is_double()) return v.as_double();
        if (v.is_integer()) return static_cast<double>(v.as_integer());
        throw std::runtime_error("Type mismatch: expected number");
    }

    std::string load_from_json(const json::value& j_data)
    {
        try {
            if(!j_data.has_field(U("n")) || !j_data.has_field(U("start")) || !j_data.has_field(U("target"))) {
                return "Missing required fields: n, start, or target";
            }

            int node_num = j_data.at(U("n")).as_integer();
            if (node_num <= 0) return "Invalid node count (n)";

            initialize(node_num);

            start = j_data.at(U("start")).as_integer() - 1;
            target = j_data.at(U("target")).as_integer() - 1;

            if (start < 0 || start >= n || target < 0 || target >= n) {
                return "Start or Target ID out of bounds (must be between 1 and n)";
            }

            if (j_data.has_field(U("coords"))) {
                const auto& j_coords = j_data.at(U("coords")).as_array();
                for(int i = 0; i < n && i < static_cast<int>(j_coords.size()); ++i) {
                    if(j_coords.at(i).has_field(U("x")) && j_coords.at(i).has_field(U("y"))) {
                        points[i].x = safe_get_double(j_coords.at(i).at(U("x")));
                        points[i].y = safe_get_double(j_coords.at(i).at(U("y")));
                    }
                }
            }

            if (j_data.has_field(U("edges"))) {
                const auto& j_edges = j_data.at(U("edges")).as_array();
                for(const auto& val : j_edges) {
                    if(!val.has_field(U("u")) || !val.has_field(U("v")) || !val.has_field(U("w"))) continue;
                    int u = val.at(U("u")).as_integer() - 1;
                    int v = val.at(U("v")).as_integer() - 1;
                    double w = safe_get_double(val.at(U("w")));
                    
                    if(u >= 0 && u < n && v >= 0 && v < n && w >= 0) {
                        graph[u].push_back({v, w});
                    }
                }
            }
            return ""; 
        } catch (const std::exception& e) {
            return std::string("JSON Parse Exception: ") + e.what();
        }
    }

    double heuristic(int a, int target_idx)
    {
        double dx = points[a].x - points[target_idx].x;
        double dy = points[a].y - points[target_idx].y;
        return std::hypot(dx, dy); 
    }

    void Astarmain()
    {
        if(start < 0 || start >= n || target < 0 || target >= n) return;

        g[start] = 0;
        f[start] = heuristic(start, target);
        pq.push({f[start], start});
        
        parents[start] = -1; 

        while(!pq.empty())
        {
            std::pair<double, int> top = pq.top();
            pq.pop();
            int u = top.second;
            
            if (visited[u]) continue;
            visited[u] = true; 

            if (u == target) {
                out = 1;
                int curr = target;
                while (curr != -1) { 
                    ans.push_back(curr);
                    curr = parents[curr];
                }
                std::reverse(ans.begin(), ans.end());
                return;
            }

            for (const auto& e : graph[u]) {
                int v = e.loc;
                double w = e.weight;

                if (visited[v]) continue;

                if (g[u] + w < g[v]) {
                    g[v] = g[u] + w;              
                    f[v] = g[v] + heuristic(v, target);   
                    parents[v] = u;               
                    pq.push({f[v], v});           
                }
            }
        }
    }

    int get_out() const { return out; }
    double get_min() const { return g[target]; }
    const std::vector<int>& get_ans() const { return ans; }
};

class PathfindingServer
{
public:
    PathfindingServer(utility::string_t url) : m_listener(url)
    {
        m_listener.support(methods::POST, std::bind(&PathfindingServer::handle_post, this, std::placeholders::_1));
    }

    void open() { m_listener.open().wait(); }
    void close() { m_listener.close().wait(); }

private:
    void handle_post(http_request request)
    {
        std::cout << "Received request (1-based ID mode)..." << std::endl;

        request.extract_json()
        .then([=](pplx::task<json::value> task) {
            try {
                json::value request_data = task.get(); 
                
                Astar worker;
                json::value response_data;

                std::string error_msg = worker.load_from_json(request_data);
                if (!error_msg.empty()) {
                    std::cerr << "Bad Request: " << error_msg << std::endl;
                    response_data[U("status")] = json::value::string(U("error"));
                    response_data[U("message")] = json::value::string(utility::conversions::to_string_t(error_msg));
                    request.reply(status_codes::BadRequest, response_data);
                    return;
                }

                worker.Astarmain();

                if(worker.get_out() == 0) {
                    response_data[U("status")] = json::value::string(U("failed"));
                    response_data[U("message")] = json::value::string(U("No path found"));
                    response_data[U("path")] = json::value::array();
                    response_data[U("distance")] = json::value::number(-1);
                } else {
                    response_data[U("status")] = json::value::string(U("success"));
                    
                    std::vector<json::value> json_path_vec;
                    const auto& path_indices = worker.get_ans();
                    for(int node_idx : path_indices) {
                        json_path_vec.push_back(json::value::number(node_idx + 1));
                    }
                    response_data[U("path")] = json::value::array(json_path_vec);
                    response_data[U("distance")] = json::value::number(worker.get_min());
                }

                request.reply(status_codes::OK, response_data);
            } 
            catch (const http_exception& e) {
                std::cerr << "HTTP Exception: " << e.what() << std::endl;
                request.reply(status_codes::BadRequest, json::value::string(U("Invalid JSON body")));
            }
            catch (const std::exception& e) {
                std::cerr << "Internal Error: " << e.what() << std::endl;
                json::value err;
                err[U("error")] = json::value::string(U("Internal Server Error"));
                request.reply(status_codes::InternalError, err);
            }
        }).wait(); 
    }
    http_listener m_listener;
};

int main()
{
    utility::string_t address = U("http://*:8080");
    PathfindingServer server(address);
    try {
        server.open();
        std::cout << "A* Server (1-based ID) listening on " << t2s(address) << "..." << std::endl;
        std::cout << "Press Enter to exit." << std::endl;
        
        std::string line;
        std::getline(std::cin, line);

        server.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
    }
    return 0;
}
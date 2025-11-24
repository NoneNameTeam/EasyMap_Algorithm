#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <queue>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/http_listener.h>

//创建http服务器类

struct bian {
    int loc;//边终点
    double weight;//权重
};

struct point{
    double x,y;//节点坐标
};

class Astar
{
private:
    int n;//节点总数
    std::vector<std::vector<bian>> graph;//邻接表
    std::vector<point> points;//节点坐标数组
    std::vector<int> ans;//返回节点顺序答案的数组
    std::vector<int> parents;//父节点数组
    std::priority_queue<std::pair<double,int>,std::vector<std::pair<double,int>>,std::greater<std::pair<double,int>>> pq;
    int start,target;//起点和终点
    std::vector<double> f,g;//f估计代价f = g + man()，g为实际代价
    std::vector<bool> visited;//已遍历过的节点无需再遍历
    int out;//判断是否找到路径
public:
    void initialize()
    {
        // 清空所有容器
        graph.clear();
        ans.clear();
        f.clear();
        g.clear();
        visited.clear();
        points.clear();
        
        // 清空优先队列
        while (!pq.empty()) pq.pop();
        
        // 根据n的大小重新初始化
        if (n > 0)
        {
            graph.resize(n);
            f.resize(n, std::numeric_limits<double>::max());
            g.resize(n, std::numeric_limits<double>::max());
            visited.resize(n, false);
            parents.resize(n,-1);
            points.resize(n,{0,0});
            out=0;
        }
    }

    void get_size()
    {
        std::cin>>n;//http请求告诉我节点数量n
        initialize();
    }

    void get_graph()
    {
        //http请求输入邻接表
        //http请求输入节点坐标
        //http请求起点终点start,target
    }

    double O(int a,int target)//欧几里得距离启发函数
    {
        double x=points[a].x-points[target].x;
        double y=points[a].y-points[target].y;
        return std::sqrt(x*x+y*y);
    }

    void Astarmain()
    {
        g[start]=0;
        f[start]=g[start]+O(start,target);
        pq.push({f[start],start});

        while(!pq.empty())
        {
            std::pair<double, int> top = pq.top();
            pq.pop();
            
            int u = top.second;
            // 如果该节点已经被处理过（即找到了最短路），跳过
            if (visited[u]) continue;
            visited[u] = true; // 标记为已处理

            //判断是否到达终点
            if (u == target) {
                // 回溯路径
                int curr = target;
                while (curr != -1) { 
                    ans.push_back(curr);
                    curr = parents[curr];
                }
                // 反转数组输出
                std::reverse(ans.begin(), ans.end());
                out=1;
                return;
            }

            //扩展邻居节点
            for (const auto& e : graph[u]) {
                int v = e.loc;
                double w = e.weight;

                // 如果 v 已经在闭集中，无需再处理
                if (visited[v]) continue;

                // 如果经由 u 到达 v 的距离更短
                if (g[u] + w < g[v]) {
                    g[v] = g[u] + w;              // 更新 G 值
                    f[v] = g[v] + O(v, target);   // 更新 F 值 (F = G + H)
                    parents[v] = u;               // 记录父节点
                    pq.push({f[v], v});           // 加入优先队列
                }
            }
        }
    }

    int get_out()
    {
        return out;
    }

    int get_min()
    {
        return g[target];
    }
};

int main()
{
    //打开http服务器持续监听后端请求

    //收到http请求
    Astar worker;
    worker.get_size();
    worker.get_graph();
    worker.Astarmain();

    if(worker.get_out()==0) {
        //返回后端无从start到target的路径的信息
    }else{
        //返回后端需要的最短节点顺序如节点1到5的最短路径为1，3，4，5，即给后端ans数组
        //返回后端需要的最短路径路程,即worker.get_min()的结果
    }
}
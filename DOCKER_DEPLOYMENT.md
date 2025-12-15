# Docker 部署文档 / Docker Deployment Guide

本文档介绍如何使用 Docker 部署和运行 A* 路径规划算法服务。

This document explains how to deploy and run the A* Pathfinding Algorithm service using Docker.

## 目录 / Table of Contents

- [环境要求 / Requirements](#环境要求--requirements)
- [快速开始 / Quick Start](#快速开始--quick-start)
- [详细说明 / Detailed Instructions](#详细说明--detailed-instructions)
- [配置说明 / Configuration](#配置说明--configuration)
- [API 使用 / API Usage](#api-使用--api-usage)
- [故障排除 / Troubleshooting](#故障排除--troubleshooting)

## 环境要求 / Requirements

### 必需软件 / Required Software

- **Docker**: 版本 20.10 或更高 / Version 20.10 or higher
  - [Docker 安装指南](https://docs.docker.com/get-docker/)
  - 验证安装 / Verify installation: `docker --version`

- **Docker Compose**: 版本 2.0 或更高 / Version 2.0 or higher
  - [Docker Compose 安装指南](https://docs.docker.com/compose/install/)
  - 验证安装 / Verify installation: `docker-compose --version`

### 系统要求 / System Requirements

- **CPU**: 至少 2 核 / At least 2 cores
- **内存**: 至少 512 MB 可用内存 / At least 512 MB available RAM
- **磁盘空间**: 至少 1 GB 用于镜像和容器 / At least 1 GB for images and containers
- **操作系统**: Linux, macOS, Windows (with WSL2)

## 快速开始 / Quick Start

### 使用 Docker Compose（推荐）/ Using Docker Compose (Recommended)

```bash
# 1. 克隆仓库 / Clone the repository
git clone https://github.com/NoneNameTeam/EasyMap_Algorithm.git
cd EasyMap_Algorithm

# 2. 构建并启动服务 / Build and start the service
docker-compose up -d

# 3. 查看日志 / View logs
docker-compose logs -f

# 4. 停止服务 / Stop the service
docker-compose down
```

服务将在 `http://localhost:8080` 上运行。

The service will be running at `http://localhost:8080`.

## 详细说明 / Detailed Instructions

### 方法 1: 使用 Docker Compose / Method 1: Using Docker Compose

Docker Compose 提供了最简单的部署方式。

Docker Compose provides the simplest deployment method.

#### 构建镜像 / Build Image

```bash
docker-compose build
```

构建选项 / Build options:
```bash
# 不使用缓存重新构建 / Rebuild without cache
docker-compose build --no-cache

# 显示详细构建输出 / Show detailed build output
docker-compose build --progress=plain
```

#### 启动服务 / Start Service

```bash
# 后台运行 / Run in background
docker-compose up -d

# 前台运行（查看实时日志）/ Run in foreground (view live logs)
docker-compose up

# 强制重新创建容器 / Force recreate containers
docker-compose up -d --force-recreate
```

#### 管理服务 / Manage Service

```bash
# 查看运行状态 / Check running status
docker-compose ps

# 查看日志 / View logs
docker-compose logs
docker-compose logs -f          # 实时查看 / Follow logs
docker-compose logs --tail=100  # 查看最后100行 / Last 100 lines

# 重启服务 / Restart service
docker-compose restart

# 停止服务（保留容器）/ Stop service (keep containers)
docker-compose stop

# 停止并删除容器 / Stop and remove containers
docker-compose down

# 停止并删除容器、网络、镜像 / Stop and remove everything
docker-compose down --rmi all --volumes
```

### 方法 2: 使用 Docker 命令 / Method 2: Using Docker Commands

如果您更喜欢手动控制，可以直接使用 Docker 命令。

If you prefer manual control, you can use Docker commands directly.

#### 构建镜像 / Build Image

```bash
# 构建镜像 / Build the image
docker build -t easymap-astar:latest .

# 查看镜像 / List images
docker images | grep easymap-astar
```

#### 运行容器 / Run Container

```bash
# 基本运行 / Basic run
docker run -d \
  --name astar-pathfinding \
  -p 8080:8080 \
  easymap-astar:latest

# 带有重启策略 / With restart policy
docker run -d \
  --name astar-pathfinding \
  --restart unless-stopped \
  -p 8080:8080 \
  easymap-astar:latest

# 带有资源限制 / With resource limits
docker run -d \
  --name astar-pathfinding \
  --restart unless-stopped \
  -p 8080:8080 \
  --cpus="1.0" \
  --memory="512m" \
  easymap-astar:latest
```

#### 管理容器 / Manage Container

```bash
# 查看运行状态 / Check status
docker ps

# 查看所有容器（包括停止的）/ List all containers
docker ps -a

# 查看日志 / View logs
docker logs astar-pathfinding
docker logs -f astar-pathfinding  # 实时查看 / Follow logs

# 进入容器 / Enter container
docker exec -it astar-pathfinding /bin/bash

# 重启容器 / Restart container
docker restart astar-pathfinding

# 停止容器 / Stop container
docker stop astar-pathfinding

# 删除容器 / Remove container
docker rm astar-pathfinding

# 强制删除运行中的容器 / Force remove running container
docker rm -f astar-pathfinding
```

## 配置说明 / Configuration

### 端口配置 / Port Configuration

默认情况下，服务监听 `8080` 端口。如需更改：

By default, the service listens on port `8080`. To change:

**使用 Docker Compose:**
```yaml
# 编辑 docker-compose.yml / Edit docker-compose.yml
ports:
  - "9090:8080"  # 映射到主机的 9090 端口 / Map to host port 9090
```

**使用 Docker 命令:**
```bash
docker run -d -p 9090:8080 --name astar-pathfinding easymap-astar:latest
```

### 环境变量 / Environment Variables

当前版本不需要额外的环境变量。未来可能添加的配置选项：

No additional environment variables are required currently. Future configuration options may include:

```yaml
environment:
  - PORT=8080              # 服务端口 / Service port
  - LOG_LEVEL=INFO         # 日志级别 / Log level
  - MAX_NODES=10000        # 最大节点数 / Maximum nodes
  - TZ=Asia/Shanghai       # 时区 / Timezone
```

### 资源限制 / Resource Limits

在 `docker-compose.yml` 中调整资源限制：

Adjust resource limits in `docker-compose.yml`:

```yaml
deploy:
  resources:
    limits:
      cpus: '2.0'      # CPU 核心数 / CPU cores
      memory: 1G       # 内存限制 / Memory limit
    reservations:
      cpus: '1.0'      # CPU 保留 / CPU reservation
      memory: 512M     # 内存保留 / Memory reservation
```

## API 使用 / API Usage

### 端点 / Endpoint

```
POST http://localhost:8080/
Content-Type: application/json
```

### 请求格式 / Request Format

```json
{
  "n": 3,
  "start": 1,
  "target": 2,
  "coords": [
    { "x": 1, "y": 1 },
    { "x": 2, "y": 2 },
    { "x": 3, "y": 3 }
  ],
  "edges": [
    { "u": 1, "v": 2, "w": 1.5 },
    { "u": 2, "v": 3, "w": 1.5 }
  ]
}
```

### 响应格式 / Response Format

**成功 / Success:**
```json
{
  "status": "success",
  "path": [1, 2],
  "distance": 1.5
}
```

**失败 / Failure:**
```json
{
  "status": "failed",
  "message": "No path found",
  "path": [],
  "distance": -1
}
```

### 测试示例 / Test Example

```bash
# 使用 curl 测试 / Test with curl
curl -X POST http://localhost:8080/ \
  -H "Content-Type: application/json" \
  -d '{
    "n": 3,
    "start": 1,
    "target": 3,
    "coords": [
      {"x": 0, "y": 0},
      {"x": 1, "y": 1},
      {"x": 2, "y": 2}
    ],
    "edges": [
      {"u": 1, "v": 2, "w": 1.414},
      {"u": 2, "v": 3, "w": 1.414}
    ]
  }'
```

## 故障排除 / Troubleshooting

### 问题：容器无法启动 / Issue: Container won't start

**检查日志 / Check logs:**
```bash
docker-compose logs astar-server
# 或 / or
docker logs astar-pathfinding
```

**常见原因 / Common causes:**
1. 端口被占用 / Port already in use
   ```bash
   # 检查端口占用 / Check port usage
   lsof -i :8080
   netstat -tulpn | grep 8080
   ```

2. 权限问题 / Permission issues
   ```bash
   # 检查 Docker 权限 / Check Docker permissions
   sudo docker ps
   ```

3. 内存不足 / Insufficient memory
   ```bash
   # 检查系统资源 / Check system resources
   docker stats
   free -h
   ```

### 问题：构建失败 / Issue: Build fails

**清除构建缓存 / Clear build cache:**
```bash
docker-compose build --no-cache
# 或 / or
docker build --no-cache -t easymap-astar:latest .
```

**检查磁盘空间 / Check disk space:**
```bash
df -h
docker system df
```

**清理未使用的资源 / Clean unused resources:**
```bash
# 清理停止的容器 / Remove stopped containers
docker container prune

# 清理未使用的镜像 / Remove unused images
docker image prune

# 清理所有未使用的资源 / Remove all unused resources
docker system prune -a
```

### 问题：无法连接到服务 / Issue: Cannot connect to service

**检查容器状态 / Check container status:**
```bash
docker-compose ps
docker ps | grep astar
```

**检查网络 / Check network:**
```bash
docker network ls
docker network inspect easymap_algorithm_astar-network
```

**测试容器内部连接 / Test connection from inside container:**
```bash
docker exec -it astar-pathfinding curl http://localhost:8080/
```

**检查防火墙 / Check firewall:**
```bash
# Linux
sudo iptables -L -n | grep 8080
sudo ufw status

# macOS
sudo pfctl -s rules | grep 8080
```

### 问题：性能问题 / Issue: Performance issues

**检查资源使用 / Check resource usage:**
```bash
docker stats astar-pathfinding
```

**增加资源限制 / Increase resource limits:**
```yaml
# 编辑 docker-compose.yml / Edit docker-compose.yml
deploy:
  resources:
    limits:
      cpus: '2.0'
      memory: 1G
```

**查看慢查询 / Monitor slow requests:**
```bash
docker-compose logs -f | grep -i "slow\|timeout\|error"
```

### 问题：健康检查失败 / Issue: Health check failing

**禁用健康检查进行调试 / Disable health check for debugging:**

编辑 `docker-compose.yml`，注释掉 healthcheck 部分：
```yaml
# healthcheck:
#   test: ["CMD", "curl", "-f", "http://localhost:8080/"]
```

**手动测试健康检查 / Manually test health check:**
```bash
docker exec astar-pathfinding curl -f http://localhost:8080/ || echo "Failed"
```

### 获取帮助 / Getting Help

如果问题仍未解决，请：

If the issue persists:

1. 收集诊断信息 / Collect diagnostic information:
   ```bash
   docker-compose version
   docker version
   docker-compose logs > logs.txt
   docker inspect astar-pathfinding > inspect.txt
   ```

2. 在 GitHub 上提交 Issue / Submit an issue on GitHub:
   https://github.com/NoneNameTeam/EasyMap_Algorithm/issues

## 高级配置 / Advanced Configuration

### 多实例部署 / Multi-Instance Deployment

运行多个实例实现负载均衡：

Run multiple instances for load balancing:

```bash
# 使用不同端口 / Use different ports
docker-compose up -d --scale astar-server=3

# 或手动创建 / Or manually create
docker run -d -p 8081:8080 --name astar-1 easymap-astar:latest
docker run -d -p 8082:8080 --name astar-2 easymap-astar:latest
docker run -d -p 8083:8080 --name astar-3 easymap-astar:latest
```

### 持久化日志 / Persistent Logs

将日志保存到主机：

Save logs to host:

```yaml
# docker-compose.yml
volumes:
  - ./logs:/app/logs
```

### 反向代理集成 / Reverse Proxy Integration

与 Nginx 集成示例 / Example integration with Nginx:

```nginx
upstream astar_backend {
    server localhost:8080;
}

server {
    listen 80;
    server_name astar.example.com;

    location / {
        proxy_pass http://astar_backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

## 安全建议 / Security Recommendations

1. **不要以 root 用户运行** / Don't run as root
   - Dockerfile 已配置使用非 root 用户 / Dockerfile is configured to use non-root user

2. **限制资源使用** / Limit resource usage
   - 使用 CPU 和内存限制防止资源耗尽 / Use CPU and memory limits to prevent resource exhaustion

3. **定期更新镜像** / Regularly update images
   ```bash
   docker-compose pull
   docker-compose up -d
   ```

4. **使用网络隔离** / Use network isolation
   - Docker Compose 自动创建隔离网络 / Docker Compose automatically creates isolated networks

5. **监控日志** / Monitor logs
   ```bash
   docker-compose logs -f | grep -i "error\|warn\|fail"
   ```

## 生产环境部署建议 / Production Deployment Recommendations

1. **使用编排工具** / Use orchestration tools
   - 考虑使用 Kubernetes、Docker Swarm 等 / Consider using Kubernetes, Docker Swarm, etc.

2. **设置监控** / Set up monitoring
   - Prometheus + Grafana
   - ELK Stack for logs

3. **配置自动重启** / Configure auto-restart
   ```yaml
   restart: always
   ```

4. **使用 HTTPS** / Use HTTPS
   - 在反向代理层添加 SSL/TLS / Add SSL/TLS at reverse proxy layer

5. **实施健康检查** / Implement health checks
   - 已在 Dockerfile 和 docker-compose.yml 中配置 / Already configured in Dockerfile and docker-compose.yml

6. **备份和恢复计划** / Backup and recovery plan
   - 定期备份配置文件 / Regularly backup configuration files

## 许可证 / License

本项目遵循原仓库的许可证。

This project follows the license of the original repository.

## 贡献 / Contributing

欢迎提交 Issue 和 Pull Request！

Issues and Pull Requests are welcome!

---

**项目地址 / Project Repository**: https://github.com/NoneNameTeam/EasyMap_Algorithm

#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
ESP32 UDP服务端测试程序
最简单的UDP服务器，用来测试ESP32的UDP通信

使用方法：
1. 确保电脑和ESP32在同一个WIFI网络
2. 修改ESP32代码中的服务器IP为本机IP
3. 运行这个程序: python udp_server_test.py
4. 启动ESP32，观察通信情况
"""

import socket
import time

def main():
    # 服务器配置
    HOST = '192.168.0.102'  # 监听所有网卡
    PORT = 8888       # 端口号，和ESP32保持一致
    
    print("=== ESP32 UDP服务端测试 ===")
    print(f"启动UDP服务器: {HOST}:{PORT}")
    
    # 创建UDP套接字
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind((HOST, PORT))
    
    print("服务器启动成功，等待ESP32连接...")
    print("按 Ctrl+C 退出")
    print("-" * 50)
    
    try:
        while True:
            # 接收ESP32发送的数据
            data, client_address = server_socket.recvfrom(1024)
            
            print(f"收到来自 {client_address} 的数据:")
            print(f"  原始数据: {data}")
            print(f"  十六进制: {data.hex()}")
            print(f"  数据长度: {len(data)} 字节")
            
            # 如果是数字数据，打印出来
            if len(data) <= 20:  # 避免打印太长的数据
                numbers = [int(b) for b in data]
                print(f"  数字格式: {numbers}")
            
            # 回复确认消息给ESP32
            reply_data = b"OK from server"
            server_socket.sendto(reply_data, client_address)
            print(f"  回复: {reply_data}")
            
            # 额外发送一些测试指令给ESP32
            time.sleep(0.1)  # 稍等一下
            
            # 发送控制指令1：飞行参数
            control_cmd1 = bytes([100, 200, 50, 75])  # 模拟飞行控制参数
            server_socket.sendto(control_cmd1, client_address)
            print(f"  发送控制指令1: {list(control_cmd1)}")
            
            # 发送控制指令2：LED控制
            control_cmd2 = bytes([0xFF, 0x00, 0xFF, 0x00, 0xFF])  # 模拟LED颜色
            server_socket.sendto(control_cmd2, client_address)
            print(f"  发送控制指令2: {list(control_cmd2)}")
            
            # 发送文本指令
            text_cmd = b"LAND_NOW"  # 模拟文本指令
            server_socket.sendto(text_cmd, client_address)
            print(f"  发送文本指令: {text_cmd}")
            
            print("-" * 50)
            
    except KeyboardInterrupt:
        print("\n服务器关闭")
    except Exception as e:
        print(f"错误: {e}")
    finally:
        server_socket.close()

if __name__ == "__main__":
    main()

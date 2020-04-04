import json
from tp import ThreadPoolManger
from ctypes import *
# from ctypes import CDLL, string_at, c_uint64

import time
import inspect
import ctypes
import sys
import os
from threading import Thread, Lock
dll = CDLL("../WebSocketTools/x64/Debug/WebSocketTools.dll")


class Server:
    def __init__(self, port_num=8888):
        self.port_num = port_num
        self.clients = {}
        self.thread_pool = ThreadPoolManger(10)

    def rece(self, clientnum):
        result = dll.lgrecv(clientnum)
        if(result == 0):
            print("来自python:数据接收错误\n")
            return {"type": "err"}
        data = string_at(result).decode('utf8')
        data = json.loads(data)
        return data

    def send(self, clientnum, data):
        # print(data)
        data = json.dumps(data)
        dll.lgsend(data.encode("utf8"), clientnum)

    def start_sever(self):

        dll.lginit(8888)
        dll.lgrecv.restype = c_uint64
        print("---服务器启动----\n")

        recv = Thread(target=self.waitThread, args=())
        recv.start()
        while 1:
            com = input("\n请输入命令(如):\nquit 退出\nlist 列出\n")
            if(com == "quit"):
                return 0

            elif(com == "list"):
                print(self.clients)

            else:
                print("没有这条命令")

    def waitThread(self):
        print("登录监听线程已经启动")
        while True:
            clientnum = dll.lgaccept()  # 等待登录
            id = self.rece(clientnum)  # 获得用户id
            id = id["sender"]
            self.clients[id] = clientnum
            print(id, "登入")
            # 发送登录确认
            self.send(
                clientnum,
                {
                    'type': 'sys',
                            'sender': "sys",
                            'recerver': id,
                            'message': {
                                'type': 'logOK'
                            }
                }
            )

            # 为该用户建立单独的监听线程
            self.thread_pool.add_job(self.clientThread, *(clientnum, id,))

    def clientThread(self, clientnum, id):
        print("已建立用户 ", id, " 的监听线程,当前用户数:", len(self.clients))
        while True:
            # 接收数据
            data = self.rece(clientnum)

            if(data['type'] == "err"):
                print("用户"+id+"发送了非法数据或离线,请让其重新登录\n")
                return 0
            if(data['type'] == "user"):
                if(data['receiver'] in self.clients):
                    self.send(self.clients[data['receiver']], data)
                else:
                    print(id, "传来了没有目的用户的数据包")


def main():
    server = Server()
    server.start_sever()
    os._exit(0)
    return 0


if __name__ == "__main__":
    main()


#

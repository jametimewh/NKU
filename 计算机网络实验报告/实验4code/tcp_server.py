import socket
import time
serverSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)    # 生成服务器的TCP连接套接字
serverPort = 6121     # 端口号
serverSocket.bind(("",serverPort))  # 绑定服务器套接字和端口号
serverSocket.listen(10)
connectionSocket,address = serverSocket.accept()
try:
    message = connectionSocket.recv(1024).decode()
    filename = message.split()[1]
    with open(filename[1:], "r") as f:
         content = f.read()
    stateRow = "HTTP/1.1 200 OK\r\n"
    firstRow = "Connection close\r\nDate:"+time.strftime("%Y-%m-%d",time.localtime(time.time()))+"\r\n服务器:Apache/1.3.0 (Windows)\r\nLast-Modified:monday,28 Novermber 2022\r\nContent-Length:"+str(len(content))+"\r\nContent-Type:html\r\n\r\n"
    outputdata = stateRow+firstRow+content
    connectionSocket.send(outputdata.encode())
    connectionSocket.close()
except IOError:
    print("[ERROR]The file being fetched is not existed.")
    with open("error.html","r") as f:
        content = f.read()
        stateRow = "HTTP/1.1 404 Not Found\r\n"  # 状态行
        firstRow = "Connection close\r\nDate:" + time.strftime("%Y-%m-%d",time.localtime(time.time()))+ "\r\n服务器:Apache/1.3.0 (Windows)\r\nLast-Modified:monday,28 Novermber 2022\r\nContent-Length:" + str(len(content)) + "\r\nContent-Type:html\r\n\r\n"
        outputdata = stateRow + firstRow + content
        connectionSocket.send(outputdata.encode())
        connectionSocket.close()



import time
import socket

serverName = "127.0.0.1"  # 服务器主机
serverPort = 6121         # 端口号
clientsocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)      # 创建客户机套接字

clientsocket.connect((serverName,serverPort))

fetch_file = "/myfile.html"
requestRow = "Get "+fetch_file+" HTTP/1.1\r\n"
firstRow = "Host:localhost\r\nUser-agent:Microsoft Edge/100.0.1185.36\r\nConnection:keep-alive\r\nAccept-language:ch\r\n\r\n"
requestMessages = requestRow+firstRow

start = time.perf_counter()
clientsocket.send(requestMessages.encode())
responseMessage = clientsocket.recv(1024)
end = time.perf_counter()
print("RTT:",end-start,"s")

print("响应报文：\n",responseMessage.decode("utf-8"),sep = "")
f = open('localHtml.html','w')
message = responseMessage.decode()
message = message.split("\r\n")
content = message[-1]
f.write(content)
f.close()
clientsocket.close()

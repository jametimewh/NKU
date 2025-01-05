import socket
import time
clientSocket=socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
clientSocket.settimeout(1)#设置最长往返时延为 1s
count=0
start=0
totaltime=0
for i in range(10):
    print("test_"+str(i))
    t1=time.time()
    clientSocket.sendto("ping".encode("utf-8"),("127.0.0.1",10000))
    try:
        message,address=clientSocket.recvfrom(1024)
    except:
        print("out of time!!!")
        count=count+1
        continue
    t2=time.time()

    if (start == 0):
        mintime = (t2 - t1) / 2
        maxtime = (t2 - t1) / 2
        start = 1
    elif ((t2 - t1) / 2 < mintime):
        mintime = (t2 - t1) / 2
    elif ((t2 - t1) / 2 > maxtime):
        maxtime = (t2 - t1) / 2
    totaltime = totaltime + (t2 - t1) / 2
    print('t2-t1%.15f' % ((t2 - t1) / 2))
print('min_RRT:' + str(mintime))
print('max_RRT:' + str(maxtime))
print('average_RRT:' + str(totaltime / (10 - count)))
print('packet loss rate:' + str(count / 10))


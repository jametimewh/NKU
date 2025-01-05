import torch
import numpy as np
from matplotlib import pyplot as plt
from torch.utils.data import DataLoader
from torchvision import transforms
from torchvision import datasets
import torch.nn.functional as F
"""
卷积运算 使用 mnist 数据集，和 10-4，11 类似的，只是这里：1.输出训练轮的 acc 2.模型
上使用 torch.nn.Sequential
"""
batch_size = 64
learning_rate = 0.01
momentum = 0.5
EPOCH = 10
transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize((0.1307,), (0.3081,))])
# softmax 归一化指数函数,其中 0.1307 是 mean 均值和 0.3081 是 std 标准差
train_dataset = datasets.MNIST(root='./data/mnist', train=True, transform=transform,download=True) # 本地没有就加上 download=True
test_dataset = datasets.MNIST(root='./data/mnist', train=False, transform=transform,download=True) # train=True 训练集，=False 测试集
train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False)
fig = plt.figure()
for i in range(12):
 plt.subplot(3, 4, i+1)
 plt.tight_layout()
 plt.imshow(train_dataset.train_data[i], cmap='gray', interpolation='none')
 plt.title("Labels: {}".format(train_dataset.train_labels[i]))
 plt.xticks([])
 plt.yticks([])
plt.show()
# 训练集乱序，测试集有序
class Net(torch.nn.Module):
 def __init__(self):
    super(Net, self).__init__()
    self.conv1 = torch.nn.Sequential(
    torch.nn.Conv2d(1, 10, kernel_size=5),
    torch.nn.ReLU(),
    torch.nn.MaxPool2d(kernel_size=2),
    )
    self.conv2 = torch.nn.Sequential(
    torch.nn.Conv2d(10, 20, kernel_size=5),
    torch.nn.ReLU(),
    torch.nn.MaxPool2d(kernel_size=2),
    )
    self.fc = torch.nn.Sequential(
    torch.nn.Linear(320, 50),
    torch.nn.Linear(50, 10),
    )
 def forward(self, x):
    batch_size = x.size(0)
    x = self.conv1(x) # 一层卷积层,一层池化层,一层激活层(图是先卷积后激活再池
    x = self.conv2(x) # 再来一次
    x = x.view(batch_size, -1) # flatten 变成全连接网络需要的输入 (batch, 20,4,4)
    x = self.fc(x)
    return x # 最后输出的是维度为 10 的，也就是（对应数学符号的 0~9）
model = Net()
# Construct loss and optimizer ----------------------------------------------------------------------------
criterion = torch.nn.CrossEntropyLoss() # 交叉熵损失
optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate, momentum=momentum) # lr 学习率，momentum 冲量
# Train and Test CLASS ------------------------------------------------------------------------------------
# 把单独的一轮一环封装在函数类里
def train(epoch):
 running_loss = 0.0 # 这整个 epoch 的 loss 清零
 running_total = 0
 running_correct = 0
 for batch_idx, data in enumerate(train_loader, 0):
    inputs, target = data
    optimizer.zero_grad()
    # forward + backward + update
    outputs = model(inputs)
    loss = criterion(outputs, target)
    loss.backward()
    optimizer.step()
    # 把运行中的 loss 累加起来，为了下面 300 次一除
    running_loss += loss.item()
    # 把运行中的准确率 acc 算出来
    _, predicted = torch.max(outputs.data, dim=1)
    running_total += inputs.shape[0]
    running_correct += (predicted == target).sum().item()
    if batch_idx % 300 == 299: # 不想要每一次都出 loss，浪费时间，选择每 300
        print('[%d, %5d]: loss: %.3f , acc: %.2f %%'% (epoch + 1, batch_idx + 1, running_loss / 300, 100 * running_correct / running_total))
        running_loss = 0.0 # 这小批 300 的 loss 清零
        running_total = 0
        running_correct = 0 # 这小批 300 的 acc 清零
        # torch.save(model.state_dict(), './model_Mnist.pth')
        # torch.save(optimizer.state_dict(), './optimizer_Mnist.pth')
def test():
 correct = 0
 total = 0
 with torch.no_grad(): # 测试集不用算梯度
    for data in test_loader:
        images, labels = data
        outputs = model(images)
        _, predicted = torch.max(outputs.data, dim=1) # dim = 1 列是第 0 个维
        total += labels.size(0) # 张量之间的比较运算
        correct += (predicted == labels).sum().item()
        acc = correct / total
        print('[%d / %d]: Accuracy on test set: %.1f %% ' % (epoch+1, EPOCH, 100 * acc)) # 求测试的准确率，正确数/总数
 return acc
# Start train and Test --------------------------------------------------------------------------------------
if __name__ == '__main__':
 acc_list_test = []
 for epoch in range(EPOCH):
    train(epoch)
    # if epoch % 10 == 9: #每训练 10 轮 测试 1 次
    acc_test = test()
    acc_list_test.append(acc_test)
 plt.plot(acc_list_test)
 plt.xlabel('Epoch')
 plt.ylabel('Accuracy On TestSet')
 plt.show()
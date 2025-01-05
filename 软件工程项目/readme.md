> _Readme_

部署运行本项目步骤:

1. 已经安装Django,Langchain等相关依赖

2. 在WebQA文件夹的settings.py中,修改DATABASES信息,修改成本地环境

   ```python
   
   DATABASES = {
       "default": {
           "ENGINE": "django.db.backends.mysql",
           "NAME": "test",
           "USER": "root",
           "PASSWORD": "1234",
           "HOST": "127.0.0.1",
           "PORT": "3306",
       }
   }
   
   # 修改为本地数据库
   ```

3. 命令行cd进入项目根目录,运行

   ```powershell
   python manage.py makemigrations
   python manage.py migrate
   ```

   #部署相关表到数据库中

4. 关于API key:

   项目保留了SerpAPI,AssemblyAI以及一些大模型调用相关的key,但是调用B站API时,需要更新B站Cookie中的SESSDATA,BILI_JCT以及BUVID3

   项目最后一次运行时使用的相关数据如下:

   ```python
   SESSDATA = ('93af9fc8%2C1738562403%2C41695%2A81CjA_T62q_lRtd'
               'N9mTkvy8izp3qap7uaIVswTS3veR0MhY96rZDeNA8AX4-GoZ-96o'
               'jUSVlVMc08tbFhOeDd0UWhCMTBuamd5S080WXI3WVdrYVlQaldzZFJiZ'
               'UF2NlRISHBqekl1TzBfMnRZdVJLMTFtT0ZjTE9hRC1rZEl0S3VMNTRSSlJnVE1BIIEC')
   BUVID3 = "AAC2E2F8-68B6-D2FE-ABCC-7F8188588D4029532infoc"
   BILI_JCT = "b2552d96f5b0b61b3fb45223359fd94f"
   ```

   需要登录B站后F12在应用中查找

5. 语音转文字以及文件读取功能,因为国内网络原因以及项目架构等原因可能出现运行较慢的情况

6. 项目初次部署运行时,可能运行较慢,需要等待几分钟甚至十几分钟

7. 关于文件读取:

   因为使用了m3e base作为embedding,直接植入项目文件夹过大,如需测试使用可以从Huggingface下载

   在other中的bilibili.py修改路径即可:

   项目最后一次运行相关数据如下:

   ```python
   EMBEDDING_DEVICE = 'cpu'
   embeddings = HuggingFaceEmbeddings(model_name="C:/Users/86139/Desktop/项目/m3e-base",
                                      model_kwargs={'device': EMBEDDING_DEVICE})
   #该路径为我本地的路径
   ```

   
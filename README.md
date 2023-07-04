# 使用说明
插件仅支持UE5

B站直播开放平台：https://open-live.bilibili.com/

B站弹幕直播互动开发文档：https://open-live.bilibili.com/document/bdb1a8e5-a675-5bfe-41a9-7a7163f75dbf


第一步 下载插件：
![image](https://github.com/sheepandriy/ZMY_BiliBiliLiveLink/assets/72051422/264908a6-62ae-4c94-b4df-1a7825a0e401)

第二步 解压到UE5项目路径下的Plugins文件夹下（没有就新建一个）：

![image](https://github.com/sheepandriy/ZMY_BiliBiliLiveLink/assets/72051422/65c641d0-8201-41d8-94e4-99d70c51b766)
![image](https://github.com/sheepandriy/ZMY_BiliBiliLiveLink/assets/72051422/c8445bfd-bcb7-4598-adce-c5a0150a83ca)
![image](https://github.com/sheepandriy/ZMY_BiliBiliLiveLink/assets/72051422/ac38b3f0-d8c6-4562-87f6-f98ce1172d23)

第三步 运行项目，会提示是否编译插件，选择 “是”：
![image](https://github.com/sheepandriy/ZMY_BiliBiliLiveLink/assets/72051422/98af08d0-87e0-412e-8bcf-ba1ed2b1026e)

第四步 项目打开后就可以开始使用插件了，获取B站弹幕需要在B站申请密钥。获得B站的密钥后，在项目里任意蓝图获取BiliBiliLiveLinkSubsystem，主要通过下图三个节点获取弹幕：
![image](https://github.com/sheepandriy/ZMY_BiliBiliLiveLink/assets/72051422/55f3bea1-b2b8-4f0a-867c-94719b3e5175)

在开始之前，先绑定如下图所述的委托：

![image](https://github.com/sheepandriy/ZMY_BiliBiliLiveLink/assets/72051422/1e587be3-cf3f-4b5b-bbae-a2997d33b23c)

先通过 InitializeAccessInfo 节点输入B站发给你的Key和Secret。然后在项目需要的时机，通过 Start 节点输入主播身份码(IdentityCode)，和项目的AppID开始接收弹幕，并在项目需要结束时调用 End 节点：
![image](https://github.com/sheepandriy/ZMY_BiliBiliLiveLink/assets/72051422/888915c5-e7ab-44b3-8103-d13b7b1e2ae1)






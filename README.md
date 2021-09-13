感谢[paralax/awesome-honeypots](https://github.com/paralax/awesome-honeypots.git) 提供了一个优秀的蜜罐列表，在工作中需要把每个需要用到的开源蜜罐下载下来，比较麻烦，因此创建此库是为了把所有相关的蜜罐都一次性下载下来并希望可以追踪其版本的变化。
[更多优秀的列表请参阅](https://github.com/sindresorhus/awesome.git)

批量更新子模块脚本
```
git submodule foreach git pull
```
# 主要收集github上开源的蜜罐形成一个大集合

1. [dionaea](https://github.com/DinoTools/dionaea.git)
2. [cowire](https://github.com/cowrie/cowrie.git)已取代[kippo](https://github.com/desaster/kippo.git)
3. [p0f](https://github.com/p0f/p0f.git)
4. [conpot](https://github.com/mushorg/conpot.git)
5. snare/tanner
> [Glastopf](https://github.com/mushorg/glastopf)替代者
	1. [phpox](https://github.com/mushorg/phpox.git)
	2. [snare](https://github.com/mushorg/snare.git)
	3. [tanner](https://github.com/mushorg/tanner.git) 
6. [opencanary](https://github.com/thinkst/opencanary.git)
7. [beeswarm](https://github.com/honeynet/beeswarm.git) [freebuf](https://www.freebuf.com/sectool/122214.html)
8. [HoneyPy](https://github.com/foospidy/HoneyPy) 低交互蜜罐，具有更多中等交互蜜罐的能力。
9. [Honeyd](https://github.com/DataSoft/Honeyd) Honeyd是一个小型守护进程，可以在网络上创建虚拟主机。该主机可以配置为运行任意服务及其TCP个性可以调整，使他们看起来运行确定操作系统的版本 [动态蜜罐发生器 - 基于honeyd](https://github.com/ppgirl/InfHoney)
10. [miniprint](https://github.com/sa7mon/miniprint) 打印机蜜罐
11. [Artillery](https://github.com/BinaryDefense/artillery) 炮兵监控工具和警报系统的组合
12. [rdpy](https://github.com/citronneur/rdpy) RDP
13. [MongoDB-HoneyProxyPy](https://github.com/jwxa2015/MongoDB-HoneyProxyPy) 自研的MongoDB蜜罐代理
14. [ciscoasa](https://github.com/cymmetria/ciscoasa_honeypot) Cisco ASA component capable of detecting CVE-2018-0101, a DoS and remote code execution vulnerability. [官网](https://community.cymmetria.com/)
15. [mailoney](https://github.com/awhitehatter/mailoney.git) SMTP蜜罐
16. [honeything](https://github.com/omererdem/honeything) 
17. [elasticpot](https://gitlab.com/bontchev/elasticpot) Elasticsearch 蜜罐
18. [sticky_elephant](https://github.com/betheroot/sticky_elephant.git) PostgreSQL


# 平台
1. [MHN](https://github.com/threatstream/mhn)
2. [tpot](https://github.com/EpistasisLab/tpot.git)
3. [SpiderFoot](https://github.com/smicallef/spiderfoot) 一个开源智能（OSINT）自动化工具。其目标是自动收集有关给定目标的情报过程 [官网](http：//www.spiderfoot.net)
4. [CHN-Server](https://github.com/CommunityHoneyNetwork/CHN-Server)

## 待合并的蜜罐有
* [Nova](https://github.com/DataSoft/Nova.git)
* [thug](https://github.com/buffer/thug.git)
* [Honeysink](http://www.honeynet.org/node/773) - 开源网络陷阱，提供了检测与阻止指定网络上恶意流量的机制

* [D] [honeytrap](https://github.com/armedpot/honeytrap) 用于针对TCP或UDP服务的观察攻击的网络安全工具
* [medpot](https://github.com/schmalle/medpot) 
* Amun - 漏洞模拟蜜罐
* SIREN - 半智能蜜罐网络 - 蜜网只能虚拟环境
* Honeyd
* UDPot Honeypot - 简单 UDP / DNS 蜜罐脚本
* Honeycomb - 使用蜜罐自动创建签名
* [D] hornet - 支持多虚拟主机的中交互 SSH 蜜罐
* [sshd-honeypot](https://github.com/amv42/sshd-honeypot) sshd-honeypot旨在让Cowrie记录暴力攻击以及攻击者执行的shell交互。由于sshd-honeypot使用OpenSSH，因此无法根据协议偏差或错误消息的差异进行指纹识别
* [HoneyBow](https://sourceforge.net/projects/honeybow/)  论文：[HoneyBow：一个基于高交互式蜜罐技术的恶意代码自动捕获器.pdf](./doc/HoneyBow：一个基于高交互式蜜罐技术的恶意代码自动捕获器.pdf)
* [drupot](https://github.com/d1str0/drupot) Drupal Honeypot Drupal是使用PHP语言编写的开源内容管理框架（CMF）
* [LaBrea](http://labrea.sourceforge.net/) - 接管未使用的 IP 地址，创建对蠕虫、黑客有吸引力的虚拟服务 [帮助](https://www.mankier.com/1/labrea#) 2003年后未更新
* KFSensor - 基于 Windows 的入侵检测系统蜜罐(收费)
* [glutton](https://github.com/mushorg/glutton) Glutton提供SSH和TCP代理。SSH代理在攻击者和服务器之间充当MITM，以纯文本形式记录所有内容。TCP代理尚未提供日志记录功能
* [heralding](https://github.com/johnnykv/heralding)收集凭据的简单蜜罐,目前支持以下协议：ftp，telnet，ssh，http，https，pop3，pop3s，imap，imaps，smtp，vnc，postgresql和socks5

网络地址变换:    
* [OFPot](https://github.com/upa/ofpot)
 
高交互蜜罐:

* [HIHAT](https://github.com/honeynet/HIHAT)

其他:

* honeynet [git](https://github.com/honeynet)      [home](https://www.honeynet.org/)

* [SpiderFoot](https://github.com/smicallef/spiderfoot) 一个开源智能（OSINT）自动化工具。其目标是自动收集有关给定目标的情报过程 [官网](http：//www.spiderfoot.net)

hpfriends - real-time social data-sharing - HPFriends 订阅系统的展示

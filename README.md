[![Awesome Intelligent Car Race](https://awesome.re/badge.svg)](https://github.com/ittuann/Awesome-IntelligentCarRace)
## 2022-17th-SmartCar-WirelessCharge
### 17届全国大学生智能汽车竞赛无线充电组开源项目 青岛理工大学

本项目为作者在参加17届全国大学生智能汽车竞赛无线充电组的代码  

本人感觉此种循迹方式对于新人来说十分友好,不用去写繁杂难以理解的补线算法,并且稳定性也有所保证.在省赛中队伍也是苟且完赛,虽然说成绩不好无缘国赛,但是我觉得这种算法的进步空间还是很大的.并且在车辆高速运行下的稳定性不输给补线策略,希望有人可以试试这种,绝对不坑哈。    

摄像头循迹的重要部分参考了上海交通大学AuTop战队开源的循迹策略,即不进行补线处理,在车辆运行的各个阶段采用换循边线的方式处理各种元素.    
在这里简要说明三叉的处理思路,在摄像头识别出三叉元素后,即换边线巡线，使用一个边线拟合出中线,另外陀螺仪记角度值,记录变化值到达设定的目标角度范围后再换回,环岛元素仅仅是比三叉复杂了一点而已,状态机程序的写法复杂了一些而已，大体的思路还是一致的。    
  
#### 上交开源项目地址:https://github.com/SJTU-AuTop/RT1064-Code   
#### 知乎文章地址:https://zhuanlan.zhihu.com/p/391252945   

在此感谢大佬们的开源,在此也将我的代码开源在github供学习以及他人参考.

>注意:此代码对于17届智能车线上赛道做了特别优化,如果想直接拿来用是比较难的,最好是可以借鉴一下思路,其实写出来并不难,难在坚持.我相信如果坚持做的话,每个人是都>可以做出来的.在此非常感谢我的两个队友小崔和宇哥,PCB画的很不错.是好队友!
>作者Email:sunjiyi666@foxmail.com 有问题可以发邮件咨询QQ同号可以搜到







## The 17th National College Students Intelligent Car Competition Wireless Charging Group Open Source Project

This project is the code of the wireless charging group of the author participating in the 17th National College Students Smart Car Competition 


I feel that this way of following is very friendly for newcomers, do not have to write complicated and incomprehensible line filling algorithms, and the stability is also guaranteed. In the provincial competition, the team is also meticulous and finished, although the results are not good and do not win the national competition, but I think there is still a lot of room for improvement in this algorithm. And the stability of the vehicle running at high speed does not lose the supply line strategy, I hope someone can try this, absolutely not pit ha.

An important part of camera tracking refers to the open source tracking strategy of the AuTop team of Shanghai Jiao Tong University, that is, without making up the line, the various elements are processed by changing the sideline at all stages of the vehicle.
Here is a brief description of the three-pronged processing ideas, after the camera recognizes the trifurcated elements, that is, change the sideline to check the line, in addition, the gyroscope remembers the angle value, and then changes back after reaching the angle range, the roundabout element is only a little more complex than the triad, and the idea of answering the question is still the same.
  
#### Turn in the open source project address: https://github.com/SJTU-AuTop/RT1064-Code
#### Zhihu Article Address: https://zhuanlan.zhihu.com/p/391252945

Thanks to the big guys for their open source, I will also open source my code on github for learning and other people's reference.

> note: this code has been specially optimized for the 17th smart car online track, if you want to use it directly, it is more difficult, it is best to learn from the idea, in fact, it is not difficult to write it out, it is difficult to insist. I believe that if you insist on doing it, everyone > be able to do it.Thank you very much to my two teammates Xiao Cui and Yu Ge, the PCB drawing is very good. Be good teammates!

Author Email:sunjiyi666@foxmail.com If you have any questions, you can send an email to consult QQ The same number can be searched


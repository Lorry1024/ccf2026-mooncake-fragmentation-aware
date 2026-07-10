# 展示视频核对清单

## 必须讲清楚

1. 队伍名称为KVCache Forge，成员为刘神舟、罗荣裕、王英颖和杨俊。
2. 作品对应Mooncake赛题2，切入点是Mooncake Store Master侧segment分配策略。
3. 总空闲空间不等于最大连续空闲区域，`free_ratio_first`可能误选无法容纳大对象的segment。
4. `fragmentation_aware`先比较`can_fit`，再比较连续性和总空闲比例，并保留bounded sampling与fallback。
5. 实现已经接入策略枚举、配置、工厂、测试、benchmark和文档，而不是离线原型。
6. PR#2797最终CI为26项成功、1项跳过。
7. 确定性仿真中首选成功由0/6提升到6/6，fallback由11降到0，平均候选数量保持5.00。
8. 当前结果是策略层证据，不宣称真实RDMA或SGLang HiCache端到端吞吐。

## 录制前页面

1. 最终版PPT第1页。
2. PR#2797顶部。
3. PR页面底部“All checks have passed”。
4. PR的`Files changed`页面。
5. 比赛材料GitHub仓库主页。
6. 比赛材料GitLink仓库主页。
7. 已切换到仓库根目录的终端。
8. 技术报告“验证与效果”章节。

## 画面检查

1. 录制分辨率为1920×1080，30fps。
2. 浏览器没有邮箱、令牌、私有仓库或聊天通知。
3. 终端字号不低于20pt。
4. 鼠标移动平稳，关键数据至少停留3秒。
5. 视频不超过5分钟。
6. 最终链接在无登录状态下能够访问。

## 一句话总结

本作品在不改变Mooncake默认行为的前提下，为Store新增请求相关的碎片感知候选排序，让大对象优先尝试真正具有足够连续空间的segment，从而减少错误首选和fallback。

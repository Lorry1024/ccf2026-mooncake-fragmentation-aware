# CCF2026 Mooncake赛题2作品展示视频最终稿

## 一、成片目标

视频建议控制在4分45秒至4分55秒，绝对不要超过5分钟。整段视频只回答五个问题：我们是谁，发现了什么问题，怎样修改Mooncake，效果如何，成果是否已经公开并通过工程验证。

推荐采用“PPT讲解+上游PR与CI证据+终端复现+公开仓库”的结构。摄像头不是必需项，画面清晰、旁白稳定、数据可读比露脸更重要。所有旁白均按正常语速设计，录制时不要额外发挥；如果某一段超过预定时间，优先删减转场和停顿，不删减边界说明。

## 二、录制前准备

### 2.1 基本参数

- 使用OBS Studio录制，分辨率1920×1080，帧率30fps。
- 优先录制为MKV，完成后在OBS中重新封装为MP4。
- 麦克风使用48kHz，提前录制10秒测试音量和底噪。
- 浏览器缩放调整到80%至100%，终端字号不低于20pt。
- 关闭邮件、聊天软件和系统通知，隐藏邮箱、访问令牌和无关标签页。
- 鼠标移动要慢，每个关键结论至少停留2秒。

### 2.2 预先打开的窗口

按以下顺序排列窗口，录制时只使用快捷键切换，不现场输入网址：

1. `slides/Mooncake_FragmentationAware_初赛展示_最终版.pptx`，停在第1页。
2. 上游PR：`https://github.com/kvcache-ai/Mooncake/pull/2797`，停在PR顶部。
3. 同一PR的`Files changed`页面，定位到`mooncake-store/include/allocation_strategy.h`中的新策略。
4. PR底部的合并检查区域，确保画面能看到`All checks have passed`、`26 successful checks`和`1 skipped`。
5. GitHub比赛材料仓库：`https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware`。
6. 终端，工作目录已经切换到比赛材料仓库根目录。

GitLink仓库地址已经写入报告和提交说明，但当前本机凭据没有该仓库写权限。录制前如果GitLink尚未同步到最新提交，不要把GitLink页面作为“最新代码”证据；视频中以GitHub公开仓库、Mooncake fork和上游PR为当前版本依据。

### 2.3 Windows PowerShell终端预演

本机已经安装MinGW `g++`，可以直接在Windows PowerShell中完成演示，不需要WSL。先进入比赛材料仓库：

```powershell
Set-Location 'E:\26ccf\ccf2026-mooncake-fragmentation-aware'
```

录制前运行一次完整命令，完成编译并确认输出正确：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File .\tools\run_powershell_demo.ps1
```

录制时使用`-SkipBuild`直接运行已经编译好的程序，通常几秒内即可显示结果：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File .\tools\run_powershell_demo.ps1 -SkipBuild
```

脚本会把程序的标准输出和标准错误流统一为普通文本，避免PowerShell把汇总信息显示成红色`NativeCommandError`；同时隐藏易受CPU频率和系统负载影响的纳秒计时，只保留0/6、6/6、11到0、平均候选数量5.00、实验边界说明和PASS。录制前清空终端，只保留简短提示符。

## 三、4分50秒逐字分镜稿

### 0:00-0:24 开场与项目定位

**画面：**PPT第1页。开录后先静止1秒，再开始讲话。

**逐字稿：**

大家好，我们是KVCache Forge，成员为刘神舟、罗荣裕、王英颖和杨俊。作品“Mooncake Store碎片感知优化”对应Mooncake赛题2。我们优化Master侧segment分配，让KVCache对象优先选择连续空间充足的位置。

### 0:24-0:53 问题背景

**画面：**切换到PPT第2页。鼠标先指向标题，再停在第二条结论附近。

**逐字稿：**

Mooncake Store长期运行时，不同大小的KVCache对象反复分配和释放，会形成不连续空洞。原有`free_ratio_first`按照总空闲比例排序，但总空闲多不代表连续区域足够大。若选中无法容纳对象的segment，allocator会拒绝申请，系统只能尝试其他候选或进入fallback。

### 0:53-1:27 策略原理

**画面：**切换到PPT第3页，先指向左侧基线，再指向右侧新策略。

**逐字稿：**

我们新增`fragmentation_aware`策略，保留有界候选采样，不做全局扫描。策略读取总空闲和最大连续区域，先用`can_fit`判断请求能否容纳，再计算综合得分。可容纳候选始终优先；preferred、excluded、多副本去重和fallback语义不变，最终申请仍由allocator确认。

### 1:27-1:57 工程实现

**画面：**切换到PPT第4页，按从上到下的顺序扫过五行内容。

**逐字稿：**

这不是脱离主仓库的离线原型。我们在Mooncake正式代码路径的9个文件中完成策略类型、配置、工厂、核心排序、命令行入口、单元测试、benchmark和文档接入。用户通过`--allocation_strategy=fragmentation_aware`显式开启，默认策略不变，因此可以灰度验证和快速回滚。

### 1:57-2:20 核心代码证据

**画面：**切换到PR的`Files changed`页面。画面保留文件路径，并缓慢指向`FragmentationAwareAllocationStrategy`、`can_fit`和`largest_free_region`附近代码。

**逐字稿：**

这里是上游PR核心实现。评分只改变候选顺序，不绕过allocator，也不修改协议。单元测试构造“总空闲较高但连续空间不足”的segment，验证新策略选择可直接容纳对象的候选，并保留preferred语义。

### 2:20-2:47 PR与CI验证

**画面：**先显示PR顶部2秒，再切到`All checks have passed`区域。鼠标依次指向`26 successful checks`和`1 skipped`。

**逐字稿：**

我们已向Mooncake上游提交draft PR，编号2797。最终GitHub Actions为26项成功、1项按条件跳过，Linux构建、格式、文档和相关测试均已通过，说明改动已进入Mooncake真实工程验证流程。

### 2:47-3:27 现场复现实验

**画面：**切换到Windows PowerShell，输入并执行以下命令。命令可以提前放在历史记录中，用方向键调出，避免打错。

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\run_powershell_demo.ps1 -SkipBuild
```

**逐字稿：**

现在运行仓库中的确定性复现实验。两个策略面对相同的5个候选segment和6个大对象请求。基线首选可直接容纳次数是0/6，新策略是6/6；fallback从11次降到0次；平均候选数量都为5.00。收益来自排序信号改进，而不是增加容量或扩大搜索范围。

执行完成后，让结果在屏幕上停留3秒，再切回PPT。

### 3:27-3:58 结果与代价

**画面：**切换到PPT第5页，依次指向0/6、6/6、11到0和5.00四个指标。

**逐字稿：**

两个策略最终都能达到6/6可容纳，所以新策略没有增加物理容量，而是更快找到已有的正确位置。单次决策耗时从121.00纳秒增加到138.93纳秒，新增约17.93纳秒。也就是用可控的纳秒级计算减少错误首选和fallback。这里是策略层验证，不外推为生产集群QPS增益。

### 3:58-4:20 赛题对应关系

**画面：**切换到PPT第7页。依次停在技术完整性、场景适配性和创新性三行。

**逐字稿：**

成果对应赛题2的Store性能和可扩展性方向。优化位于真实Store路径，候选规模有界，副本与fallback语义不变。创新点是把请求大小与最大连续区域直接关联，让分配从“总量感知”升级为“请求相关的碎片感知”。

### 4:20-4:38 开源与提交闭环

**画面：**切换到GitHub比赛材料仓库。画面应同时看到仓库为Public，以及`report/`、`slides/`、`repro/`、`logs/`和`release/`目录。

**逐字稿：**

成果已在GitHub公开托管，包括报告、PPT、PR补丁、复现源码、日志、提交包和SHA-256；Mooncake fork与上游PR保留开发历史和CI结果，便于评审复核。

### 4:38-4:55 总结

**画面：**切换到PPT第8页。讲话结束后保持画面2秒再停止录制。

**逐字稿：**

总结来说，我们实现了可选、可回滚、可复现的碎片感知策略，并通过上游CI和确定性实验完成验证。下一步将补充真实Store、RDMA和SGLang HiCache端到端评测。感谢各位评审。

## 四、字幕建议

不要给所有旁白加满屏字幕，只在关键位置叠加以下短句：

1. `问题：总空闲空间不等于最大连续空闲区域`
2. `核心：can_fit优先，score用于同类候选排序`
3. `工程：9个文件完成正式接入`
4. `CI：26项成功，1项按条件跳过`
5. `首选成功：0/6 → 6/6`
6. `fallback：11 → 0`
7. `平均候选数量：5.00，搜索范围未扩大`
8. `边界：当前结论为策略层证据`

字幕使用白字配半透明深色底，字号保持一致，不要使用飞入、旋转或闪烁动画。

## 五、现场失败兜底稿

如果终端程序没有正常运行，不要在视频中安装依赖或排查环境。立即打开`logs/topic_aligned_store_scalability_sim_20260707.log`，使用下面这段话替换“现场复现实验”的开头：

> 这里展示的是仓库随附的确定性复现日志，对应的C++源码、编译命令和输入口径均已公开。日志显示，两个策略在相同5个候选segment和6个大对象请求下，首选成功由0/6提升到6/6，fallback由11次下降到0次，平均候选数量保持5.00。

随后直接进入PPT第5页，不在视频中解释失败原因。

## 六、录制后检查

1. 确认成片时长不超过5分钟，建议保留5秒安全余量。
2. 检查四名成员姓名、PR编号2797以及所有指标是否口播正确。
3. 检查浏览器画面没有私人邮箱、令牌、聊天通知或无关标签页。
4. 检查终端输出在普通笔记本屏幕和手机横屏下仍可阅读。
5. 检查0/6、6/6、11到0、5.00和17.93纳秒至少各出现一次。
6. 检查旁白明确说明当前结果是策略层证据，没有宣称真实RDMA或HiCache生产吞吐提升。
7. 导出文件建议命名为`CCF2026_Mooncake_KVCacheForge_作品展示.mp4`。
8. 上传后使用无登录或浏览器隐私窗口验证视频链接可以直接访问。

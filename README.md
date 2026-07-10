# Mooncake Store Fragmentation-Aware Allocation

<div align="center">

面向CCF 2026开源创新大赛Mooncake赛题2的KVCache碎片感知分配优化

[![CCF 2026](https://img.shields.io/badge/CCF-2026-blue)](https://gitlink.org.cn/competitions/track2_2026Mooncake)
[![Mooncake](https://img.shields.io/badge/Mooncake-Store-green)](https://github.com/kvcache-ai/Mooncake)
[![PR](https://img.shields.io/badge/Upstream%20PR-%232797-orange)](https://github.com/kvcache-ai/Mooncake/pull/2797)
[![CI](https://img.shields.io/badge/CI-26%20passed%20%7C%201%20skipped-brightgreen)](https://github.com/kvcache-ai/Mooncake/pull/2797)
[![Demo](https://img.shields.io/badge/Demo-video%20ready-brightgreen)](https://pan.baidu.com/s/1v2-jxHSaDi0RlagPGrLAiw?pwd=mxhx)

`KVCache` · `Fragmentation-Aware` · `Bounded Sampling` · `Mooncake Store` · `SGLang HiCache` · `可复现`

**演示视频**：[KVCache Forge Mooncake作品展示](https://pan.baidu.com/s/1v2-jxHSaDi0RlagPGrLAiw?pwd=mxhx) · 提取码：`mxhx`

**队伍**：KVCache Forge · 罗荣裕（队长）· 刘神舟 · 王英颖 · 杨俊

[GitHub材料仓库](https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware) · [GitLink材料仓库](https://gitlink.org.cn/lorry22/ccf2026-mooncake-fragmentation-aware) · [Mooncake上游PR](https://github.com/kvcache-ai/Mooncake/pull/2797)

</div>

---

## 作品简介

本项目面向Mooncake赛题2，聚焦Mooncake Store在混合尺寸KVCache对象和长期运行场景下的分配稳定性。现有`free_ratio_first`策略主要按照segment总空闲比例排序，但“总空闲很多”并不等于“存在足够大的连续空间”；碎片化严重的segment可能看起来更空闲，却无法直接容纳当前大对象，随后触发allocator失败和fallback。

我们在Mooncake Store正式代码路径中新增可选的`fragmentation_aware`策略，将当前请求大小、最大连续空闲区域和空间连续性纳入候选排序。策略保留bounded sampling、preferred/excluded、多副本去重、allocator最终确认和best-effort fallback语义，默认`random`策略保持不变，因此能够灰度启用并快速回滚。

## 赛题对应

| 赛题关注点 | 本项目实现 | 当前证据 |
|---|---|---|
| Mooncake Store吞吐性能 | 减少无法容纳当前对象的错误首选和额外fallback | fallback尝试次数由11降至0 |
| 可扩展性 | 沿用有界候选采样，不扫描全部segment | 平均候选数量保持5.00 |
| 高可用与兼容语义 | 保留preferred、excluded、多副本去重和best-effort fallback | 单元测试、设计说明和上游CI |
| SGLang HiCache+Store | 优化HiCache下游Store对象放置路径 | Store正式代码、benchmark和配置入口 |
| 开源规范性 | 公开材料仓库、Mooncake fork和上游PR | PR#2797通过26项检查 |
| 可复现性 | 提供C++仿真、关键日志、PowerShell演示和技术报告 | [`repro/`](./repro/)、[`logs/`](./logs/)、[`tools/`](./tools/) |

> 当前结论证明的是分配策略层的正确性、排序收益和工程可接入性，不将确定性仿真数据外推为真实RDMA集群或SGLang HiCache生产吞吐增益。

## 核心交付

| 模块 | 内容 | 价值 |
|---|---|---|
| 请求相关碎片感知 | 使用`can_fit`判断最大连续空闲区域能否容纳当前请求 | 避免总空闲比例造成的大对象误选 |
| 连续性综合评分 | `0.70 × contiguity_ratio + 0.30 × free_ratio` | 在可容纳候选中兼顾连续空间和容量均衡 |
| 正式工程接入 | 策略枚举、配置解析、工厂创建、命令行、benchmark和文档 | 不是离线原型，可显式启用和回滚 |
| 兼容性保护 | 保留preferred/excluded、used set和allocator确认 | 不绕过已有空间与副本语义 |
| 测试与CI闭环 | 碎片反例、preferred语义、Linux构建和完整回归测试 | 26项检查成功，90项回归测试通过 |
| 提交与复现 | 报告、PPT、视频、PR补丁、源码、日志和SHA-256 | 降低评审复核与复现成本 |

## 策略流程

```text
请求：slice_length / replica_num / preferred / excluded
                         |
                         v
              按原有语义尝试preferred
                         |
                         v
              bounded sampling候选采样
                         |
                         v
      读取total_free与largest_free_region
                         |
                         v
           can_fit优先 -> score -> tie-break
                         |
                         v
              调用原有allocator实际申请
                         |
               +---------+---------+
               |                   |
             成功               候选不足
               |                   |
          更新used set       原有best-effort fallback
```

评分只决定候选尝试顺序，真正的空间申请仍由原有allocator完成。即使并发状态变化导致评分信息过期，最坏结果也只是该候选申请失败并继续后续候选，不会突破空间安全边界。

## 验证结果

| 指标 | `free_ratio_first` | `fragmentation_aware` |
|---|---:|---:|
| 大对象首选segment可直接容纳 | 0/6 | 6/6 |
| 最终可容纳 | 6/6 | 6/6 |
| fallback尝试次数 | 11 | 0 |
| 平均候选segment数量 | 5.00 | 5.00 |
| 单次决策耗时 | 121.00ns | 138.93ns |
| 新增决策开销 | 不适用 | 17.93ns |

两个策略最终均可容纳6/6个请求，说明新策略没有增加物理容量；收益来自更准确地找到已经存在的连续空间。平均候选数量保持不变，说明结果不是通过扩大搜索范围获得的。

## 快速演示

### Windows PowerShell

首次运行会使用MinGW `g++`编译确定性仿真：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\run_powershell_demo.ps1
```

录屏或重复演示时可以跳过编译：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\run_powershell_demo.ps1 -SkipBuild
```

脚本会输出0/6、6/6、11到0、平均候选数量5.00和`PASS`，并隐藏容易受CPU频率影响的纳秒计时。

### Linux / WSL

```bash
g++ -std=c++17 -O2 repro/topic_aligned_store_scalability_sim.cpp \
  -o topic_aligned_store_scalability_sim
./topic_aligned_store_scalability_sim
```

参考输出位于[`logs/topic_aligned_store_scalability_sim_20260707.log`](./logs/topic_aligned_store_scalability_sim_20260707.log)。

## 在Mooncake中启用

```bash
mooncake_master --allocation_strategy=fragmentation_aware
```

在与PR基线兼容的Mooncake源码树中应用补丁：

```bash
git apply --check mooncake_fragmentation_aware_pr_2797_0123fa1.patch
git apply mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

回滚时可切换回`random`或`free_ratio_first`；也可以反向移除补丁：

```bash
git apply -R --check mooncake_fragmentation_aware_pr_2797_0123fa1.patch
git apply -R mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

## 演示与提交材料

| 材料 | 位置 |
|---|---|
| 演示视频 | [百度网盘](https://pan.baidu.com/s/1v2-jxHSaDi0RlagPGrLAiw?pwd=mxhx)，提取码：`mxhx` |
| 技术报告 | [`report/CCF2026_Mooncake_FragmentationAware_Technical_Report.pdf`](./report/CCF2026_Mooncake_FragmentationAware_Technical_Report.pdf) |
| 展示PPT | [`slides/Mooncake_FragmentationAware_初赛展示_最终版.pptx`](./slides/Mooncake_FragmentationAware_初赛展示_最终版.pptx) |
| 上游PR | [kvcache-ai/Mooncake#2797](https://github.com/kvcache-ai/Mooncake/pull/2797) |
| 当前PR补丁 | [`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`](./mooncake_fragmentation_aware_pr_2797_0123fa1.patch) |
| 设计文档 | [`DESIGN.md`](./DESIGN.md) |
| 评估报告 | [`EVALUATION.md`](./EVALUATION.md) |
| 视频逐字稿 | [`demo_script.md`](./demo_script.md) |
| 平台提交说明 | [`SUBMISSION.md`](./SUBMISSION.md) |
| 最终提交包 | [`release/CCF2026_Mooncake_FragmentationAware_initial_20260710.zip`](./release/CCF2026_Mooncake_FragmentationAware_initial_20260710.zip) |
| SHA-256 | [`release/CCF2026_Mooncake_FragmentationAware_initial_20260710.zip.sha256.txt`](./release/CCF2026_Mooncake_FragmentationAware_initial_20260710.zip.sha256.txt) |

## 项目结构

| 路径 | 说明 |
|---|---|
| [`repro/`](./repro/) | 可独立编译的碎片化反例、指标和专题仿真源码 |
| [`logs/`](./logs/) | 三份稳定、可复核的关键实验日志 |
| [`report/`](./report/) | LaTeX技术报告、最终PDF和GitHub/CI证据截图 |
| [`slides/`](./slides/) | 初赛展示PPT最终版 |
| [`tools/`](./tools/) | Windows PowerShell一键演示脚本 |
| [`release/`](./release/) | 干净提交压缩包与SHA-256校验文件 |

## 上游工程状态

| 项目 | 状态 |
|---|---|
| PR | [kvcache-ai/Mooncake#2797](https://github.com/kvcache-ai/Mooncake/pull/2797) |
| PR状态 | Draft，等待维护者正式评审 |
| PR头提交 | `0123fa1 Fix fragmentation-aware allocation test setup` |
| 改动范围 | 9个文件，覆盖策略、配置、测试、benchmark、文档与CI |
| GitHub Actions | 26项检查成功，1项按条件跳过 |
| 回归测试 | `build (3.10)`环境下90项测试全部通过 |

## 验证边界与后续工作

- 当前已完成策略实现、配置接入、单元测试、benchmark入口、上游PR、完整CI、确定性仿真、技术报告、PPT和演示视频。
- 当前不宣称已经完成真实RDMA硬件benchmark或真实SGLang HiCache端到端吞吐压测。
- 决赛阶段计划补充真实Mooncake Store吞吐、P50/P99、失败分配率、fallback比例、空间利用率和CPU开销。
- 后续将在RDMA集群和SGLang HiCache真实推理负载中验证策略层收益能否转化为系统级收益。

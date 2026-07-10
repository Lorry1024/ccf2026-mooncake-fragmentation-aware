# 初赛提交说明

## 队伍信息

| 项目 | 信息 |
| --- | --- |
| 队伍名称 | KVCache Forge |
| 队长 | 罗荣裕 |
| 队员 | 刘神舟、王英颖、杨俊 |

## 提交模式建议

建议采用“文件+链接”的提交方式。平台提交时同时提供公开GitHub仓库、Mooncake PR链接和最终压缩包。这样既满足比赛“成果必须托管至GitHub平台开源”的要求，也便于评委直接查看代码、文档、PR和CI状态。

## 参赛题目

本项目选择Mooncake赛题2：

```text
优化Mooncake Store吞吐性能、高可用功能和可扩展性等，优化SGLang HiCache+Mooncake Store性能。
```

本项目的具体切入点是Mooncake Store分配策略优化。针对混合大小KVCache对象导致的segment内部碎片问题，新增`fragmentation_aware`策略，让Master优先选择拥有足够连续空闲区域的segment，从而降低失败尝试和fallback压力。

## 提交链接

| 类型 | 链接 |
| --- | --- |
| GitHub比赛材料仓库 | `https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware` |
| GitLink比赛材料仓库 | `https://gitlink.org.cn/lorry22/ccf2026-mooncake-fragmentation-aware` |
| Mooncake fork分支 | `https://github.com/Lorry1024/Mooncake/tree/ccf-fragmentation-aware-allocation` |
| Mooncake draft PR | `https://github.com/kvcache-ai/Mooncake/pull/2797` |

## 当前PR状态

| 项目 | 状态 |
| --- | --- |
| PR编号 | `kvcache-ai/Mooncake#2797` |
| PR头提交 | `0123fa1 Fix fragmentation-aware allocation test setup` |
| GitHub Actions | 26个检查成功，1个检查跳过 |
| PR状态 | Draft，等待后续正式评审 |

## 必交材料对应关系

| 比赛要求 | 本仓库对应材料 | 当前状态 |
| --- | --- | --- |
| 源代码 | Mooncake fork分支、PR#2797、`mooncake_fragmentation_aware_pr_2797_0123fa1.patch` | 已完成 |
| 方案文档或PPT | `report/CCF2026_Mooncake_FragmentationAware_Technical_Report.pdf`、`slides/Mooncake_FragmentationAware_初赛展示_最终版.pptx` | 已完成 |
| 5分钟内作品展示视频 | 待录制，录制后补充公开链接 | 未完成 |
| 测试和benchmark说明 | `EVALUATION.md`、`logs/`、`repro/` | 已完成 |
| 公开GitHub托管 | GitHub材料仓库和Mooncake fork | 已完成 |

## 建议平台提交文本

```text
本作品由KVCache Forge队伍完成，面向CCF2026开源创新大赛Mooncake赛题2，围绕Mooncake Store在混合大小KVCache对象场景下的分配稳定性进行优化。项目新增可选的fragmentation_aware分配策略，在保持默认random策略不变的前提下，将最大连续空闲区域纳入segment排序，使Master优先选择能够直接容纳当前请求的segment，降低碎片化场景下的大对象失败尝试和fallback压力。

源代码已提交至公开GitHub仓库，并以Mooncake draft PR#2797形式提交给上游项目。当前PR头提交0123fa1已通过GitHub Actions，结果为26个检查成功、1个检查跳过。提交包包含技术报告、设计与评估文档、复现实验源码、验证日志、patch文件和展示PPT。当前阶段不宣称完成真实RDMA集群或SGLang HiCache端到端压测，相关内容作为后续工作规划。
```

## 提交前最后检查

- 确认视频录制完成并上传到公开可访问位置。
- 将视频链接补入本文件和`demo_script.md`。
- 重新生成`release/`下的最终压缩包和SHA256文件。
- 确认GitHub仓库为公开状态。
- 确认Mooncake PR链接可访问，CI状态仍为通过。

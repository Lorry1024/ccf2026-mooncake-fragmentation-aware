# CCF2026 Mooncake Store碎片感知分配优化

本仓库是CCF2026开源创新大赛Mooncake赛题的初赛提交材料仓库。项目选择Mooncake赛题2，聚焦Mooncake Store在混合大小KVCache对象场景下的分配稳定性与可扩展性优化。

## 参赛队伍

| 项目 | 信息 |
| --- | --- |
| 队伍名称 | KVCache Forge |
| 队长 | 罗荣裕 |
| 队员 | 刘神舟、王英颖、杨俊 |

## 项目定位

本项目的核心贡献是在Mooncake Store中新增一种可选的`fragmentation_aware`分配策略。该策略不改变默认行为，不重写Mooncake Store协议，不修改SGLang HiCache接口，而是在Master选择segment时增加“最大连续空闲区域”这一信号，避免只按总空闲比例排序时优先选中已经碎片化、无法容纳当前大对象的segment。

| 项目 | 内容 |
| --- | --- |
| 赛题方向 | 赛题2：优化Mooncake Store吞吐性能、高可用功能和可扩展性，优化SGLang HiCache+Mooncake Store性能 |
| 优化对象 | Mooncake Store分配策略层 |
| 新增策略 | `fragmentation_aware` |
| 默认行为 | 保持`random`不变，用户显式开启新策略 |
| 主要收益 | 降低碎片化场景下的大对象失败尝试和fallback压力 |
| 当前证据 | 上游PR通过GitHub Actions，确定性仿真通过，补充性能指标与复现日志 |

## 公开仓库与PR

- GitHub比赛材料仓库：`https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware`
- GitLink比赛材料仓库：`https://gitlink.org.cn/liushenzhou/ccf2026-mooncake-fragmentation-aware`
- Mooncake fork分支：`https://github.com/Lorry1024/Mooncake/tree/ccf-fragmentation-aware-allocation`
- Mooncake draft PR：`https://github.com/kvcache-ai/Mooncake/pull/2797`

当前PR头提交：

```text
0123fa1 Fix fragmentation-aware allocation test setup
```

当前CI状态：

```text
26个检查成功，1个检查跳过
```

## 核心产物

| 产物 | 路径 | 说明 |
| --- | --- | --- |
| 当前PR补丁 | `mooncake_fragmentation_aware_pr_2797_0123fa1.patch` | 与Mooncake PR#2797当前提交对应 |
| 设计文档 | `DESIGN.md` | 问题、目标、算法与兼容性 |
| 评估报告 | `EVALUATION.md` | 仿真、CI、指标和边界 |
| 视频稿 | `demo_script.md` | 5分钟作品展示逐字稿、分镜和兜底方案 |
| 复现源码 | `repro/` | 可独立编译的确定性实验 |
| 关键日志 | `logs/` | 三份稳定仿真结果 |
| PowerShell演示 | `tools/run_powershell_demo.ps1` | Windows下一键编译、运行并展示稳定指标 |
| 技术报告源码 | `report/technical_report.tex` | XeLaTeX报告源码 |
| 技术报告PDF | `report/CCF2026_Mooncake_FragmentationAware_Technical_Report.pdf` | 由LaTeX编译生成 |
| 展示PPT | `slides/Mooncake_FragmentationAware_初赛展示_最终版.pptx` | 初赛展示材料 |
| 提交压缩包 | `release/CCF2026_Mooncake_FragmentationAware_initial_20260710.zip` | 平台提交候选包，视频链接补齐后需重新生成 |

## 已完成的工作

1. 在Mooncake Store中新增`AllocationStrategyType::FRAGMENTATION_AWARE`。
2. 实现`FragmentationAwareAllocationStrategy`。
3. 增加`--allocation_strategy=fragmentation_aware`启动参数支持。
4. 补充分配策略单元测试，覆盖碎片化优先级和preferred segment语义。
5. 将新策略加入allocation strategy benchmark矩阵。
6. 更新Mooncake Store设计文档和部署文档。
7. 为GitHub Actions中的Go store binding集成测试加入磁盘回收步骤，避免runner磁盘不足导致无关失败。
8. 准备确定性仿真和专题对齐仿真，记录可复现实验日志。
9. 创建上游Mooncake draft PR并修复CI中出现的格式、磁盘和测试构造问题。
10. 整理中文提交材料、技术报告和初赛展示材料。

## 主要实验结论

| 指标 | `free_ratio_first` | `fragmentation_aware` |
| --- | ---: | ---: |
| 大对象首选segment可直接容纳 | 0/6 | 6/6 |
| fallback尝试次数 | 11 | 0 |
| 平均候选segment数量 | 5.00 | 5.00 |
| 单次决策耗时 | 121.00ns | 138.93ns |
| 新增决策开销 | 不适用 | 17.93ns |

该结果说明，在混合大小KVCache对象造成内存碎片时，新策略能够以很小的决策开销换取更稳定的首选分配结果。当前结论限定在分配路径仿真、单元测试和上游CI范围内，不宣称已经完成真实RDMA集群或真实SGLang HiCache端到端压测。

## 快速使用

启动Mooncake master时显式指定新策略：

```bash
mooncake_master --allocation_strategy=fragmentation_aware
```

应用当前补丁：

```bash
git apply --check mooncake_fragmentation_aware_pr_2797_0123fa1.patch
git apply mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

回滚补丁：

```bash
git apply -R mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

## 尚未完成

- 5分钟以内作品展示视频尚未录制。
- 视频链接写入`SUBMISSION.md`后，需要重新生成最终提交压缩包。
- 如果进入决赛，建议补充真实Mooncake Store压测、RDMA环境验证和SGLang HiCache端到端对比。

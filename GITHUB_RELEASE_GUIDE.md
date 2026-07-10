# GitHub发布说明

## 目标

比赛要求所有成果托管到GitHub平台开源。因此，本仓库作为公开材料仓库，需要包含源码入口、PR链接、文档、报告、PPT、复现实验和提交压缩包。

## 推荐仓库设置

- 仓库名称：`ccf2026-mooncake-fragmentation-aware`
- 可见性：Public
- 默认分支：`main`
- 描述：`CCF2026 Mooncake Store fragmentation-aware allocation optimization`

## 发布前检查

1. 确认`README.md`为中文入口文档。
2. 确认`SUBMISSION.md`中的链接正确。
3. 确认`report/CCF2026_Mooncake_FragmentationAware_Technical_Report.pdf`可打开。
4. 确认`slides/Mooncake_FragmentationAware_初赛展示_更新版.pptx`存在。
5. 确认`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`存在。
6. 确认`release/`中的压缩包和SHA256已更新。
7. 视频录制完成后，补充视频链接并重新打包。

## 建议Release说明

```text
CCF2026 Mooncake赛题2初赛提交材料。

本作品实现Mooncake Store碎片感知分配策略fragmentation_aware，针对混合大小KVCache对象造成的segment内部碎片问题，在分配策略层引入最大连续空闲区域信号，降低碎片化场景下的大对象失败尝试和fallback压力。

Mooncake PR：https://github.com/kvcache-ai/Mooncake/pull/2797
当前PR头提交：0123fa1
CI状态：26个检查成功，1个检查跳过
```

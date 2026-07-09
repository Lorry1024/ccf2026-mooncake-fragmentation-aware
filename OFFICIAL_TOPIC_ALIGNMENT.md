# 赛题对齐说明

## 官方赛题

本作品对应Mooncake赛题2：

```text
优化Mooncake Store吞吐性能、高可用功能和可扩展性等，优化SGLang HiCache+Mooncake Store性能。
```

## 本项目切入点

本项目不试图同时覆盖赛题2中的所有方向，而是选择其中一个明确、可实现、可验证的切入点：

```text
Mooncake Store在混合大小KVCache对象场景下的碎片感知分配优化。
```

该切入点与赛题2的关系如下：

| 赛题关注点 | 本项目对应方式 |
| --- | --- |
| Store吞吐性能 | 减少碎片化导致的失败尝试和fallback压力 |
| 可扩展性 | 保持有界候选采样，不引入全局扫描 |
| 高可用 | 保留副本best-effort和preferred/excluded语义 |
| SGLang HiCache+Store | 优化其可能使用的Mooncake Store后端分配路径 |

## 不做过度声明

当前作品不宣称以下内容：

- 已完成Mooncake高可用协议重设计。
- 已完成RDMA传输性能优化。
- 已完成真实SGLang HiCache端到端benchmark。

当前主要贡献是Store分配策略层优化，这一点在所有提交材料中保持一致。

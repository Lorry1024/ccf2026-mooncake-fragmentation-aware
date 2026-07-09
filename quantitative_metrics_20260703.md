# 定量指标记录

## 最小复现指标

| segment | 总空闲空间 | 最大连续空闲区域 | 请求大小 | 是否可容纳 |
| --- | ---: | ---: | ---: | --- |
| fragmented | 16MiB | 8MiB | 10MiB | 否 |
| contiguous | 12MiB | 12MiB | 10MiB | 是 |

选择结果：

| 策略 | 首选segment | 首选是否可容纳 |
| --- | --- | --- |
| `free_ratio_first` | fragmented | 否 |
| `fragmentation_aware` | contiguous | 是 |

## 扩展场景

扩展仿真覆盖5个确定性场景，验证普通请求、边界请求、无法容纳请求和零空闲保护逻辑。结果为：

```text
PASS fragmentation_aware_metrics: validated 5 deterministic ranking and boundary scenarios
```

## 专题对齐指标

最终初赛材料建议引用`topic_alignment_metrics_20260706.md`中的指标，因为该文件直接对应Mooncake赛题2。

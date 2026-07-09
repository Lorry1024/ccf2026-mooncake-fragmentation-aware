# 提交检查清单

## 已完成

- [x] 明确选择Mooncake赛题2。
- [x] 将项目定位为Mooncake Store碎片感知分配策略优化。
- [x] 完成`fragmentation_aware`策略实现。
- [x] 增加启动参数`--allocation_strategy=fragmentation_aware`。
- [x] 增加碎片化场景单元测试。
- [x] 将新策略加入benchmark矩阵。
- [x] 更新Mooncake Store设计文档和部署文档。
- [x] 创建Mooncake fork分支。
- [x] 创建上游Mooncake draft PR：`https://github.com/kvcache-ai/Mooncake/pull/2797`。
- [x] 修复PR中的格式、runner磁盘和测试构造问题。
- [x] PR头提交`0123fa1`通过GitHub Actions，结果为26个检查成功、1个检查跳过。
- [x] 生成当前PR补丁：`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`。
- [x] 准备`repro/`复现实验源码。
- [x] 准备`logs/`验证日志。
- [x] 准备展示PPT。
- [x] 准备中文技术报告源码和PDF。
- [x] 准备提交压缩包。

## 提交前待完成

- [ ] 录制5分钟以内展示视频。
- [ ] 上传视频并获得公开可访问链接。
- [ ] 将视频链接补入`SUBMISSION.md`、`README.md`和平台提交说明。
- [ ] 重新生成最终压缩包和SHA256。
- [ ] 最后确认GitHub材料仓库为公开状态。

## 风险说明

- 当前证据已覆盖代码实现、上游CI、单元测试和分配路径仿真。
- 当前不宣称真实RDMA环境或SGLang HiCache端到端benchmark。
- 如果平台要求必须上传视频文件而不是链接，需要将视频文件放入最终压缩包或按平台规则上传。

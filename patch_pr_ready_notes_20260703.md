# PR补丁说明

## 当前推荐补丁

正式评审建议查看：

```text
mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

该补丁对应Mooncake draft PR#2797当前头提交`0123fa1`。

## 历史补丁

仓库中保留以下历史补丁用于追溯：

```text
mooncake_fragmentation_aware.patch
mooncake_fragmentation_aware_pr_ready_20260703.patch
```

这些文件不是当前首选评审材料。

## 当前补丁内容

当前补丁包含：

- 分配策略实现。
- enum和工厂函数接入。
- master配置解析。
- 单元测试。
- benchmark矩阵。
- Mooncake文档更新。
- GitHub Actions磁盘清理步骤。

## 应用方式

```bash
git apply --check mooncake_fragmentation_aware_pr_2797_0123fa1.patch
git apply mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

回滚：

```bash
git apply -R mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

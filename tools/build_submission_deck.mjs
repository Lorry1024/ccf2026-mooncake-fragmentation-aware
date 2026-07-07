import fs from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { Presentation, PresentationFile } from "@oai/artifact-tool";

const __filename = fileURLToPath(import.meta.url);
const repoRoot = process.env.SUBMISSION_REPO_ROOT
  ? path.resolve(process.env.SUBMISSION_REPO_ROOT)
  : path.resolve(path.dirname(__filename), "..");
const outDir = path.join(repoRoot, "slides");
const qaDir = path.join(outDir, "qa");
const finalPptx = path.join(outDir, "Mooncake_FragmentationAware_初赛展示.pptx");

const W = 1280;
const H = 720;
const ink = "#000000";
const muted = "#555555";
const panel = "#EDEDED";
const rule = "#B8BCC4";
const accent = "#FF6B35";
const white = "#FFFFFF";

function addText(slide, text, x, y, w, h, style = {}) {
  const box = slide.shapes.add({
    geometry: "textbox",
    position: { left: x, top: y, width: w, height: h },
    fill: "none",
    line: { style: "solid", fill: "none", width: 0 },
  });
  box.text = text;
  box.text.style = {
    fontSize: style.fontSize ?? 22,
    bold: style.bold ?? false,
    color: style.color ?? ink,
    alignment: style.alignment ?? "left",
  };
  return box;
}

function addRule(slide, x, y, w) {
  slide.shapes.add({
    geometry: "rect",
    position: { left: x, top: y, width: w, height: 2 },
    fill: rule,
    line: { style: "solid", fill: "none", width: 0 },
  });
}

function addPanel(slide, x, y, w, h, fill = panel) {
  return slide.shapes.add({
    geometry: "rect",
    position: { left: x, top: y, width: w, height: h },
    fill,
    line: { style: "solid", fill: "none", width: 0 },
  });
}

function addFooter(slide, n) {
  addText(slide, "CCF 2026 Mooncake · Fragmentation-aware allocation", 42, 664, 760, 28, {
    fontSize: 14,
    color: muted,
  });
  addText(slide, String(n).padStart(2, "0"), 1188, 664, 50, 28, {
    fontSize: 14,
    color: muted,
    alignment: "right",
  });
}

function slideTitle(slide, title, eyebrow = "Mooncake Store") {
  addText(slide, eyebrow, 42, 38, 520, 28, {
    fontSize: 15,
    bold: true,
    color: muted,
  });
  addText(slide, title, 42, 76, 1060, 84, {
    fontSize: 44,
    bold: true,
    color: ink,
  });
  addRule(slide, 42, 166, 1196);
}

function addBullets(slide, items, x, y, w, gap = 58) {
  items.forEach((item, idx) => {
    const top = y + idx * gap;
    addPanel(slide, x, top + 8, 10, 10, accent);
    addText(slide, item, x + 28, top, w - 28, 48, {
      fontSize: 22,
      color: ink,
    });
  });
}

function addMetric(slide, value, label, x, y, w = 260) {
  addPanel(slide, x, y, w, 150, panel);
  addText(slide, value, x + 22, y + 22, w - 44, 56, {
    fontSize: 46,
    bold: true,
  });
  addText(slide, label, x + 22, y + 92, w - 44, 40, {
    fontSize: 18,
    color: muted,
  });
}

async function writeBlob(filePath, blob) {
  await fs.writeFile(filePath, new Uint8Array(await blob.arrayBuffer()));
}

async function main() {
  await fs.mkdir(outDir, { recursive: true });
  await fs.mkdir(qaDir, { recursive: true });

  const deck = Presentation.create({ slideSize: { width: W, height: H } });

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    addText(slide, "Mooncake Store", 42, 40, 500, 34, {
      fontSize: 18,
      bold: true,
      color: muted,
    });
    addText(slide, "Fragmentation-aware allocation", 42, 170, 980, 132, {
      fontSize: 62,
      bold: true,
    });
    addText(
      slide,
      "面向混合大小 KVCache 对象的 Store 分配稳定性优化",
      42,
      330,
      860,
      42,
      { fontSize: 26, color: muted },
    );
    addPanel(slide, 42, 510, 354, 72, panel);
    addText(slide, "赛题2 · Mooncake Store 性能与可扩展性", 64, 530, 320, 30, {
      fontSize: 18,
      bold: true,
    });
    addPanel(slide, 940, 0, 298, 720, panel);
    addText(slide, "opt-in", 982, 82, 200, 56, { fontSize: 42, bold: true });
    addText(slide, "--allocation_strategy=\nfragmentation_aware", 982, 150, 230, 110, {
      fontSize: 18,
      color: muted,
    });
    addFooter(slide, 1);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "这次优化解决的是分配路径里的碎片误判");
    addBullets(
      slide,
      [
        "长期运行的 Store 会经历不同大小 KVCache 对象的频繁分配与释放。",
        "总空闲空间较高的 segment，可能只有很多小洞，无法放下大对象。",
        "原有 free_ratio_first 容易优先尝试这类 segment，带来失败分配和 fallback。",
      ],
      80,
      230,
      1080,
      78,
    );
    addFooter(slide, 2);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "新策略优先看当前请求能不能真的放进去");
    addPanel(slide, 72, 230, 500, 300, panel);
    addText(slide, "free_ratio_first", 100, 260, 300, 34, { fontSize: 28, bold: true });
    addText(slide, "按总空闲比例排序", 100, 320, 360, 34, { fontSize: 22, color: muted });
    addText(slide, "可能选择总空闲多但最大连续区不足的 segment", 100, 376, 392, 76, {
      fontSize: 21,
    });
    addPanel(slide, 708, 230, 500, 300, panel);
    addText(slide, "fragmentation_aware", 736, 260, 380, 34, { fontSize: 28, bold: true });
    addText(slide, "先判断 largest free region 是否 fit", 736, 320, 400, 34, {
      fontSize: 22,
      color: muted,
    });
    addText(slide, "在 bounded sampling 内排序，不引入全局扫描", 736, 376, 392, 76, {
      fontSize: 21,
    });
    addFooter(slide, 3);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "实现保持小而清晰，默认行为不变");
    const rows = [
      ["新增策略", "AllocationStrategyType::FRAGMENTATION_AWARE"],
      ["入口配置", "--allocation_strategy=fragmentation_aware"],
      ["核心实现", "allocation_strategy.h 中新增候选评分与排序"],
      ["测试覆盖", "碎片化场景与 preferred segment 行为"],
      ["Benchmark", "将新策略加入 allocation_strategy_bench 矩阵"],
    ];
    const x = 74;
    const y = 220;
    const rowH = 62;
    rows.forEach((row, idx) => {
      const top = y + idx * rowH;
      addPanel(slide, x, top, 1130, rowH - 8, idx % 2 === 0 ? panel : white);
      addText(slide, row[0], x + 20, top + 14, 220, 28, {
        fontSize: 20,
        bold: true,
      });
      addText(slide, row[1], x + 270, top + 14, 800, 28, {
        fontSize: 20,
        color: muted,
      });
    });
    addFooter(slide, 4);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "合成 Store 模型验证了目标失效模式");
    addMetric(slide, "0/6", "free_ratio_first primary fit success", 74, 232);
    addMetric(slide, "6/6", "fragmentation_aware primary fit success", 372, 232);
    addMetric(slide, "11 → 0", "fallback attempts in deterministic model", 670, 232);
    addMetric(slide, "5.00", "average candidates scored stays bounded", 968, 232);
    addText(
      slide,
      "这不是生产 QPS 结论；它证明的是分配路径中“总空闲空间误导选择”的具体问题和改进方向。",
      74,
      456,
      1070,
      70,
      { fontSize: 24, color: ink },
    );
    addFooter(slide, 5);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "当前提交包已经有代码、文档和复现材料");
    addBullets(
      slide,
      [
        "PR-ready patch 仍可应用到当前 Mooncake upstream main。",
        "DESIGN.md 与 EVALUATION.md 对齐赛题仓库的文档建议。",
        "repro/ 和 logs/ 保留了 deterministic simulation 与 metrics 证据。",
        "SUBMISSION.md 和 GITHUB_RELEASE_GUIDE.md 指向最终平台提交路径。",
      ],
      96,
      220,
      1040,
      70,
    );
    addFooter(slide, 6);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "还需要把证据链从可解释推进到可评测");
    const rows = [
      ["必须补齐", "GitHub 公共仓库、PPT、5 分钟视频、最终 zip 与 SHA256"],
      ["强烈建议", "Mooncake fork 分支或 draft PR，而不是只提交 patch"],
      ["性能补强", "真实 Store benchmark：put/get 吞吐、P50/P99、内存利用率"],
      ["风险边界", "当前不声称 RDMA、官方 CI 或真实 SGLang HiCache benchmark"],
    ];
    rows.forEach((row, idx) => {
      const top = 222 + idx * 86;
      addPanel(slide, 74, top, 1130, 66, idx === 0 ? "#FFE8DD" : panel);
      addText(slide, row[0], 102, top + 18, 180, 30, {
        fontSize: 22,
        bold: true,
        color: idx === 0 ? accent : ink,
      });
      addText(slide, row[1], 310, top + 18, 830, 30, {
        fontSize: 21,
        color: ink,
      });
    });
    addFooter(slide, 7);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "初赛提交建议采用文件加链接模式");
    addText(slide, "提交链接", 80, 228, 220, 34, { fontSize: 28, bold: true });
    addText(slide, "GitHub 公开仓库 + Mooncake fork 分支或 PR", 80, 278, 520, 48, {
      fontSize: 24,
    });
    addText(slide, "提交文件", 80, 390, 220, 34, { fontSize: 28, bold: true });
    addText(slide, "zip 包含源码 patch、文档、PPT、视频链接、repro 和 logs", 80, 440, 620, 58, {
      fontSize: 24,
    });
    addPanel(slide, 820, 228, 320, 270, panel);
    addText(slide, "核心答辩句", 852, 258, 240, 36, { fontSize: 28, bold: true });
    addText(
      slide,
      "我们优化的是 Mooncake Store 的 segment allocation 决策路径，让大对象优先落到能连续 fit 的 segment。",
      852,
      326,
      240,
      120,
      { fontSize: 22 },
    );
    addFooter(slide, 8);
  }

  for (const [index, slide] of deck.slides.items.entries()) {
    const stem = `slide-${String(index + 1).padStart(2, "0")}`;
    await writeBlob(path.join(qaDir, `${stem}.png`), await deck.export({ slide, format: "png", scale: 1 }));
    const layout = await slide.export({ format: "layout" });
    await fs.writeFile(path.join(qaDir, `${stem}.layout.json`), await layout.text());
  }

  await writeBlob(path.join(qaDir, "montage.webp"), await deck.export({
    format: "webp",
    montage: true,
    scale: 1,
  }));

  const pptx = await PresentationFile.exportPptx(deck);
  await pptx.save(finalPptx);
  console.log(finalPptx);
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});

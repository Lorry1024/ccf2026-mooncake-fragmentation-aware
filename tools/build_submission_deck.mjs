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
const finalPptx = path.join(outDir, "Mooncake_FragmentationAware_初赛展示_更新版.pptx");

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
    addText(slide, "Mooncake Store碎片感知优化", 42, 170, 980, 132, {
      fontSize: 62,
      bold: true,
    });
    addText(
      slide,
      "面向混合尺寸KVCache对象的请求相关segment选择",
      42,
      330,
      860,
      42,
      { fontSize: 26, color: muted },
    );
    addPanel(slide, 42, 510, 354, 72, panel);
    addText(slide, "赛题2 · Store吞吐性能与可扩展性", 64, 530, 320, 30, {
      fontSize: 18,
      bold: true,
    });
    addPanel(slide, 940, 0, 298, 720, panel);
    addText(slide, "PR #2797", 982, 82, 200, 56, { fontSize: 42, bold: true });
    addText(slide, "26项通过\n1项跳过", 982, 150, 230, 110, {
      fontSize: 18,
      color: muted,
    });
    addFooter(slide, 1);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "总空闲很多，不代表当前大对象能够落地");
    addBullets(
      slide,
      [
        "混合尺寸KVCache反复分配与释放，逐步产生不连续空闲区。",
        "free_ratio_first只看总空闲比例，可能优先选择无法连续容纳请求的segment。",
        "错误首选会触发allocator失败和fallback，并在高并发下放大额外路径开销。",
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
    slideTitle(slide, "先判断能否容纳，再在可用候选中平衡空间");
    addPanel(slide, 72, 230, 500, 300, panel);
    addText(slide, "free_ratio_first", 100, 260, 300, 34, { fontSize: 28, bold: true });
    addText(slide, "按总空闲比例排序", 100, 320, 360, 34, { fontSize: 22, color: muted });
    addText(slide, "无法区分“容量总量足够”和“最大连续区足够”", 100, 376, 392, 76, {
      fontSize: 21,
    });
    addPanel(slide, 708, 230, 500, 300, panel);
    addText(slide, "fragmentation_aware", 736, 260, 380, 34, { fontSize: 28, bold: true });
    addText(slide, "can_fit优先，score用于同类候选", 736, 320, 400, 34, {
      fontSize: 22,
      color: muted,
    });
    addText(slide, "沿用bounded sampling、preferred/excluded和fallback", 736, 376, 392, 76, {
      fontSize: 21,
    });
    addFooter(slide, 3);
  }

  {
    const slide = deck.slides.add();
    slide.background.fill = white;
    slideTitle(slide, "9个文件完成策略、配置、测试与文档闭环");
    const rows = [
      ["策略类型", "新增FRAGMENTATION_AWARE枚举与工厂分支"],
      ["启动配置", "--allocation_strategy=fragmentation_aware"],
      ["核心排序", "can_fit → score → largest region → stable tie-break"],
      ["正确性", "覆盖碎片反例、preferred语义与多副本去重"],
      ["工程化", "接入benchmark、设计文档、部署文档和CI"],
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
    slideTitle(slide, "相同候选规模下，错误首选与fallback被消除");
    addMetric(slide, "0/6", "基线首选可直接容纳", 74, 232);
    addMetric(slide, "6/6", "新策略首选可直接容纳", 372, 232);
    addMetric(slide, "11 → 0", "fallback尝试次数", 670, 232);
    addMetric(slide, "5.00", "平均候选segment数量", 968, 232);
    addText(
      slide,
      "最终可容纳均为6/6；收益来自排序信号变化。新增决策开销约17.93ns。",
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
    slideTitle(slide, "上游PR与完整CI证明代码具备工程可审查性");
    addBullets(
      slide,
      [
        "Draft PR #2797公开展示完整代码差异与4次提交历史。",
        "GitHub Actions最终结果：26项检查成功，1项按条件跳过。",
        "代码格式、Linux构建、文档构建和相关测试均通过。",
        "比赛仓库同步提供patch、报告、PPT、复现源码、日志和SHA-256。",
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
    slideTitle(slide, "成果直接覆盖赛题2的核心评分维度");
    const rows = [
      ["技术完整性", "真实Store路径接入策略、配置、测试、benchmark和文档"],
      ["场景适配性", "针对混合尺寸KVCache与长期运行碎片化segment"],
      ["创新性", "请求相关can_fit + 连续性评分，避免总空闲误判"],
      ["边界与扩展", "不外推RDMA/HiCache端到端吞吐；接口已为实测准备"],
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
    slideTitle(slide, "项目已经形成可提交、可复现、可评审闭环");
    addText(slide, "代码证据", 80, 228, 220, 34, { fontSize: 28, bold: true });
    addText(slide, "上游PR #2797 + Lorry1024/Mooncake开发分支", 80, 278, 520, 48, {
      fontSize: 24,
    });
    addText(slide, "材料证据", 80, 390, 220, 34, { fontSize: 28, bold: true });
    addText(slide, "公开比赛仓库 + 技术报告 + 复现日志 + 最终压缩包", 80, 440, 620, 58, {
      fontSize: 24,
    });
    addPanel(slide, 820, 228, 320, 270, panel);
    addText(slide, "核心结论", 852, 258, 240, 36, { fontSize: 28, bold: true });
    addText(
      slide,
      "在不改变Mooncake默认行为的前提下，碎片感知排序让大对象优先尝试真正具有足够连续空间的segment。",
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

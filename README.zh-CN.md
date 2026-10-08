# PineappleCAS 简体中文测试版（TI-84 Plus CE）

这是 PineappleCAS 的**试验性汉化分支**，不是已通过 TI-84 Plus CE 真机验证的正式版本。

## 实现方式

TI-84 Plus CE 有 320×240 彩色显示屏，但 GraphX 默认的 ASCII 文本字体不支持将 UTF-8 中文直接显示为汉字。本分支将少量常用简体中文字符制作成 **12×12 像素字模**，直接通过 `gfx_SetPixel` 绘制。

- 汉化界面：输入/输出、功能、选项、化简、求值、展开、求导、帮助，以及各子选项、按钮和操作状态。
- 保留原有的 CAS 算法、数学变量名称（如 Y1、Str1、Ans）、TI-BASIC 的 `SIMP` / `EVAL` 等命令和原有输入输出逻辑。
- ASCII 字符沿用 GraphX 字体，仅需中文的地方绘制内置字模。不更改计算器系统字体，也不支持任意中文输入。
- 帮助页面和来自数学核心的部分英文错误详情仍然保留英文。
- 内置字模只包含界面需要的汉字，不是完整汉字库。

## 编译

请安装官方 [CE C/C++ Toolchain](https://github.com/CE-Programming/toolchain/releases)，然后：

```sh
git clone -b l10n/zh-cn-bitmap-prototype https://github.com/Rok1n/PineappleCAS.git
cd PineappleCAS
make
```

正常情况下，CE 工具链会生成 `bin/PCAS.8xp`。如 GitHub Actions 工作流运行成功，也可以从其 Artifacts 下载编译结果。

注意：本分支尚未完成计算器真机测试。上传前请备份所有未归档的程序与变量。按照原项目 README，将程序归档并安装所需的 C libraries。OS 5.5 或更高版本可能需要按原项目说明使用兼容的 ASM 程序启动方式，某些 OS 版本禁用了原生 ASM 启动。

## 汉字清晰度及兼容性检查

1. 启动 PCAS，检查首页的「输入」「输出」「功能」「选项」以及左侧菜单。
2. 逐项切换，检查复选框标签有无重叠、缺字、黑块或乱码。
3. 分别运行化简/求值/展开/求导，确认弹窗显示中文，计算结果仍正确。
4. 检查英文变量 `Y1`、`Str1`、`Ans` 是否正常；测试退出和重新启动。

如果出现乱码或中文过小，需要调整 `src/calc/zh_font.h` 的位图与 `src/calc/zh_text.h` 的绘制规则。

## 字模许可

位图由 Noto Sans CJK SC Bold 生成。Noto Sans CJK 使用 [SIL Open Font License 1.1](https://openfontlicense.org/) 授权。字模数据并非 TI 官方字体，也不会安装到计算器系统中。

原程序 © Nathan Farlow 及 PineappleCAS 贡献者，遵循原仓库 MIT 许可。

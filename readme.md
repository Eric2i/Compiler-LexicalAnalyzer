# 编译原理作业--题目2.1正则表达式的识别题目1.3

*本文档使用markdown格式*
*为了更好的视觉效果，*
*建议您使用支持markdown的阅读器*

## 文件目录

- Makefile 本项目使用makefile辅助开发
- readme.md 本说明文件
- bin/: 可执行文件
- src/: 源代码文件
- obj/: 目标文件
- test/: 测试输入输出数据

## 测试

1. 清除可能存在的二进制文件或目标文件

   ```bash
   make clean
   ```
2. 正则表达式识别与模拟

   ```bash
   make -e DEBUG=1 test
   ```

   以上命令会自动执行并测试以下两项任务:

   1. 给定一组正则表达式$\mathcal{R}$

      - $\forall r \in \mathcal{R}$, 构建对应的NFA
      - 输出NFA对应的Transition Table。
        注：该项任务所测试的输入: test/input/regex.txt;
   2. 给定一个正则表达式$\mathcal{r}$，与一组待判定字符串集合$\mathcal{S}$;

      - $\forall s \in \mathcal{S}$, 判断$s \in \mathcal{L}(r)$
        注:该项任务所使用的输入与预期输出: test/{input,output}/regex.txt

## 程序流程示意图

![Pipeline](./Pipeline.jpg)

## 使用

1. 编译

   ```bash
   make build
   ```
2. 运行

   ```bash
   # src/Simulator "<regex>" "<string>"
   src/Simulator "a|bc*" "bccc"
   ```

   注意: 您必须添加双引号将正则表达式与待判断的字符串包裹起来.

## 编译环境

**本项目测试开发所采用的编译器信息:**

```
Apple clang version 14.0.0 (clang-1400.0.29.202)
Target: arm64-apple-darwin22.2.0
Thread model: posix
```

## Git

本项目的开发使用Git进行了版本管理，您可以通过git log来查看开发过程中的历史版本信息。

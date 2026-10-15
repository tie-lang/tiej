# tiej

tie 桌面（JIT）；可单独使用。

状态：**执行域引擎已落地** —— `src/core/execrun.tie`（tiej 执行域：命令行执行引擎——  
直启 / 直通 / 捕获 / 进程管道 / 重定向 / 复合链；Windows 与 POSIX 单源双实现，  
`#cfg` 按目标裁剪）。tshell 经 `import` 消费（tshell 绑定 tiej 的第一步）。  
JIT / 内嵌 LLVM 后续批次。

## 引擎（execrun）

- **直启**：CreateProcessW / fork+execv 直接起进程（无 shell 借用）；  
  `cwd` 经 `lpCurrentDirectory` / `chdir` 直传——不拼接 `cd … && …` 前缀串。
- **无 shell 借用**：`&&` / `||` / `;` / 进程管道 / 重定向全部引擎内实现，  
  rc 驱动、逐段可读；启动失败 `started=false` + 明确诊断（不静默）。
- **接口**：单段 `run_one(Seg)`；便捷 `run` / `capture` / `redir`；  
  多段 `run_pipeline` / `chain` / `pipe2`（段与结果跨界用编码串——  
  p.9.12.6 边界：`table<R>`（R 为 struct）不能作函数形参/返回值）。
- **平台**：Windows（CreateProcessW 直启）已全量实测（26 项探针全绿）；
  POSIX（fork+chdir+dup2+execv，pipe2/O_CLOEXEC fd 卫生）已随同批落地，
  本机无 Linux 运行时：验证止于「前端+IR 全过、链接仅缺本机 Linux CRT」层级。
- **已知限制**（登记）：双流捕获顺序读、投喂顺序写——极端双饱和场景存在阻塞  
  风险，后续批次以并发治理。

测试：从本仓根运行

```
tiec tests/exec_probe.tie -o tests/exec_probe.exe && tests/exec_probe.exe
```

（26 项：PATH/rc/cwd/中文 UTF-8 往返/管道/重定向/合流/复合链/跳过传播。）

